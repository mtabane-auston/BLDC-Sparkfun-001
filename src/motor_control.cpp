/*
 * ---------------------------------------------------------------------------------
 * Motor Control Module - SimpleFOC Smart Knob
 * ---------------------------------------------------------------------------------
 */
#include "motor_control.h"

// Motor and driver objects
BLDCMotor motor = BLDCMotor(7);
BLDCDriver6PWM driver = BLDCDriver6PWM(uh16, ul17, vh18, vl23, wh19, wl33, curSense);

// Smart Knob State
KnobMode current_mode = MODE_DETENT;
int num_detents = 12;              // Number of detent positions
float min_angle = 0;               // Minimum angle (radians)
float max_angle = 2 * PI;          // Maximum angle (radians)
float haptic_strength = 3.0;       // Torque strength for haptics
float center_angle = 0;            // Center position for spring mode
float detent_strength = 2.0;       // Strength of detent snapping
float boundary_stiffness = 15.0;   // Stiffness at boundaries

// Current state
float current_angle = 0;
int current_position = 0;
float last_angle = 0;

// Commander
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
void doMode(char* cmd) {
    int mode = atoi(cmd);
    if (mode >= 0 && mode <= 4) {
        set_knob_mode((KnobMode)mode);
    }
}

void doDetents(char* cmd) {
    int detents = atoi(cmd);
    if (detents > 0 && detents <= 100) {
        set_detent_positions(detents);
    }
}

void doHaptic(char* cmd) {
    float strength = atof(cmd);
    set_haptic_strength(strength);
}

// Interrupt handlers - cycle through modes
void IRAM_ATTR isr1() {
    aux1.pressed = true;
}

void IRAM_ATTR isr2() {
    aux2.pressed = true;
}

void motor_setup()
{
    // Driver configuration
    driver.voltage_power_supply = 3.3;
    driver.pwm_frequency = 20000;
    driver.voltage_limit = 3.3;
    driver.init();
    
    // Motor configuration
    motor.linkDriver(&driver);
    motor.voltage_limit = 3.3;
    motor.controller = MotionControlType::torque; // Torque mode for haptics
    motor.init();
    
    // Button interrupts
    pinMode(aux1.PIN, INPUT_PULLUP);
    attachInterrupt(aux1.PIN, isr1, FALLING);
    pinMode(aux2.PIN, INPUT_PULLUP);
    attachInterrupt(aux2.PIN, isr2, FALLING);
    
    // Commander commands (cast to avoid warnings)
    command.add('M', doMode, (char*)"mode (0-4)");
    command.add('D', doDetents, (char*)"num detents");
    command.add('H', doHaptic, (char*)"haptic strength");
    
    Serial.println("✓ Motor control initialized");
}

// Calculate haptic torque based on mode
float calculate_haptic_torque()
{
    float torque = 0;
    float position_error = 0;
    float target_position_angle = 0;
    float detent_spacing = 0;
    float velocity = 0;
    
    switch (current_mode) {
        case MODE_FREE_SPIN:
            torque = 0; // No resistance
            break;
            
        case MODE_BOUNDED:
            // Soft endstops
            if (current_angle < min_angle) {
                torque = boundary_stiffness * (min_angle - current_angle);
            } else if (current_angle > max_angle) {
                torque = boundary_stiffness * (max_angle - current_angle);
            }
            break;
            
        case MODE_DETENT:
            // Calculate nearest detent position
            detent_spacing = (max_angle - min_angle) / num_detents;
            current_position = round((current_angle - min_angle) / detent_spacing);
            current_position = constrain(current_position, 0, num_detents);
            
            target_position_angle = min_angle + (current_position * detent_spacing);
            position_error = target_position_angle - current_angle;
            
            // Snap to detent with proportional torque
            torque = detent_strength * position_error;
            
            // Add boundaries
            if (current_angle < min_angle) {
                torque += boundary_stiffness * (min_angle - current_angle);
            } else if (current_angle > max_angle) {
                torque += boundary_stiffness * (max_angle - current_angle);
            }
            break;
            
        case MODE_SPRING:
            // Return to center with spring force
            position_error = center_angle - current_angle;
            torque = haptic_strength * position_error;
            break;
            
        case MODE_DAMPED:
            // Damping proportional to velocity
            velocity = motor.shaft_velocity;
            torque = -haptic_strength * velocity;
            break;
    }
    
    return constrain(torque, -motor.voltage_limit, motor.voltage_limit);
}

void motor_update()
{
    // Get current angle
    current_angle = motor.shaft_angle;
    
    // Handle button presses - cycle modes
    if (aux1.pressed) {
        aux1.pressed = false;
        current_mode = (KnobMode)((current_mode + 1) % 5);
        Serial.print("Mode changed to: ");
        Serial.println(current_mode);
    }
    
    if (aux2.pressed) {
        aux2.pressed = false;
        // Adjust detents
        num_detents += 4;
        if (num_detents > 48) num_detents = 4;
        Serial.print("Detents: ");
        Serial.println(num_detents);
    }
    
    // Calculate and apply haptic torque
    float haptic_torque = calculate_haptic_torque();
    motor.move(haptic_torque);
    
    // Must call loopFOC for closed-loop control
    motor.loopFOC();
    
    last_angle = current_angle;
}

void motor_handle_commands()
{
    command.run();
}

void motor_link_sensor(Sensor* sensor)
{
    motor.linkSensor(sensor);
    Serial.println("✓ Sensor linked to motor");
}

void motor_enable_closed_loop()
{
    motor.controller = MotionControlType::torque;
    motor.initFOC();
    Serial.println("✓ Closed-loop TORQUE mode enabled (Smart Knob)");
}

// Smart Knob Configuration Functions
void set_knob_mode(KnobMode mode)
{
    current_mode = mode;
    const char* mode_names[] = {"FREE_SPIN", "BOUNDED", "DETENT", "SPRING", "DAMPED"};
    Serial.print("Knob mode: ");
    Serial.println(mode_names[mode]);
}

void set_detent_positions(int num_positions)
{
    num_detents = constrain(num_positions, 2, 100);
    Serial.print("Detents set to: ");
    Serial.println(num_detents);
}

void set_bounds(float min, float max)
{
    min_angle = min;
    max_angle = max;
    Serial.print("Bounds: ");
    Serial.print(min);
    Serial.print(" to ");
    Serial.println(max);
}

void set_haptic_strength(float strength)
{
    haptic_strength = constrain(strength, 0.1, 10.0);
    detent_strength = haptic_strength;
    Serial.print("Haptic strength: ");
    Serial.println(haptic_strength);
}

void set_center_position(float angle)
{
    center_angle = angle;
}

float get_current_angle()
{
    return current_angle;
}

int get_current_position()
{
    return current_position;
}

KnobMode get_current_mode()
{
    return current_mode;
}