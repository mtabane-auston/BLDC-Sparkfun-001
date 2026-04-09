/*
 * ---------------------------------------------------------------------------------
 * Motor Control Module - SimpleFOC
 * ---------------------------------------------------------------------------------
 * Implementation of BLDC motor control functionality
 */

#include "motor_control.h"

// Motor and driver objects
BLDCMotor motor = BLDCMotor(7);
BLDCDriver6PWM driver = BLDCDriver6PWM(uh16, ul17, vh18, vl23, wh19, wl33, curSense);

// Control variables
float target_velocity = 0.0;
Commander command = Commander(Serial);

// Button structure
struct Button {
    const uint8_t PIN;
    uint32_t numberKeyPresses;
    bool pressed;
};

Button aux1 = {auxBtn1, 0, false};
Button aux2 = {auxBtn2, 0, false};

// Command callbacks
void doTarget(char* cmd) { command.scalar(&target_velocity, cmd); }
void doLimit(char* cmd) { command.scalar(&motor.voltage_limit, cmd); }

// Interrupt handlers
void IRAM_ATTR isr1() {
    aux1.pressed = true;
    target_velocity = target_velocity * (-1);
    Serial.println("Changing directions..");
}

void IRAM_ATTR isr2() {
    aux2.numberKeyPresses++;
    aux2.pressed = true;

    if ((aux2.numberKeyPresses % 2) == 0) {
        target_velocity = 0;
        Serial.println("Stopping motor..");
    } else {
        target_velocity = 5;
        motor.enable();
        Serial.println("Starting motor..");
    }
}

void motor_setup()
{
    // Driver configuration
    driver.voltage_power_supply = 3.3;
    driver.pwm_frequency = 20000;
    driver.voltage_limit = 4;
    driver.init();

    // Motor configuration
    motor.linkDriver(&driver);
    motor.voltage_limit = 4;
    motor.controller = MotionControlType::velocity_openloop;
    motor.init();
    motor.disable();

    // Button interrupts
    pinMode(aux1.PIN, INPUT_PULLUP);
    attachInterrupt(aux1.PIN, isr1, FALLING);
    pinMode(aux2.PIN, INPUT_PULLUP);
    attachInterrupt(aux2.PIN, isr2, FALLING);

    // Commander commands
    command.add('T', doTarget, "target velocity");
    command.add('L', doLimit, "voltage limit");

    Serial.println("Motor control initialized");
    delay(100);
}

void motor_update()
{
    // Handle button presses
    if (aux1.pressed) {
        aux1.pressed = false;
    }

    if (aux2.pressed) {
        aux2.pressed = false;
    }

    // Update motor
    motor.move(target_velocity);
}

void motor_handle_commands()
{
    // Process serial commands
    command.run();
}

void set_target_velocity(float velocity)
{
    target_velocity = velocity;
}

float get_target_velocity()
{
    return target_velocity;
}
