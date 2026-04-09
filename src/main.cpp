/*
 * ---------------------------------------------------------------------------------
 * Copyright (c) 2025, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 * ---------------------------------------------------------------------------------
 */

/*
 * Example 3: Angle Calculations
 * -------------------------
 * This example demonstrates how to configure the TMAG5273 sensor to perform angle calculations
 *
 * * Hardware Connections:
 * - Connect the TMAG5273 sensor to the SparkFun Qwiic connector on your SparkFun microcontroller board.
 *
 * Note: Make sure to install the SparkFun TMAG5273 Arduino Library before running this example.
 * You can install it via the Arduino Library Manager or download it from:
 *      https://github.com/sparkfun/SparkFun_TMAG5273_Arduino_Library
 *
 */
#include "SparkFun_TMAG5273_Arduino_Library.h" // Used to send and recieve specific information from our sensor
#include <Wire.h>                              // Used to establish serial communication on the I2C bus
#include <Arduino.h>
#include <SimpleFOC.h> //http://librarymanager/All#Simple%20FOC


    //GPIO
    #define auxBtn2   13
    #define auxBtn1   14  

    //driver
    #define uh16      16
    #define ul17      17
    #define vh18      18
    #define wh19      19
    #define vl23      23
    #define wl33      33
    #define curSense  32
    
bool state = true;

  
    //motor driver
    BLDCMotor motor = BLDCMotor(7);
    BLDCDriver6PWM driver = BLDCDriver6PWM(uh16, ul17, vh18, vl23, wh19, wl33,  curSense);
    float target_velocity = 0.0;
    Commander command = Commander(Serial);
    void doTarget(char* cmd) { command.scalar(&target_velocity, cmd); }
    void doLimit(char* cmd) { command.scalar(&motor.voltage_limit, cmd); }

    //////////////////////motor demo stuff///////////////////////////
    struct Button{
      const uint8_t PIN;
      uint32_t numberKeyPresses;
      bool pressed;
    };
    Button aux1 = {auxBtn1, 0, false};
    Button aux2 = {auxBtn2, 0, false};

    void IRAM_ATTR isr1(){
      aux1.pressed = true;
      target_velocity = target_velocity*(-1);
      Serial.println("Changing directions.. ");
    }

    void IRAM_ATTR isr2(){
      aux2.numberKeyPresses++;
      aux2.pressed = true;

      if((aux2.numberKeyPresses % 2) == 0)
      {
        target_velocity = 0;
        Serial.println("Stopping motor.. ");
      }
      else
      {
        target_velocity = 5;
        motor.enable();
        Serial.println("Starting motor.. ");
      }
    }

// Create a new sensor object
TMAG5273 sensor;
// I2C default address
uint8_t i2cAddress = TMAG5273_I2C_ADDRESS_INITIAL;
// Set constants for setting up device
uint8_t conversionAverage = TMAG5273_X32_CONVERSION;
uint8_t magneticChannel = TMAG5273_XYX_ENABLE;
uint8_t angleCalculation = TMAG5273_XY_ANGLE_CALCULATION;

void setup()
{
    delay(1000);
    driver.voltage_power_supply = 3.3;
      driver.pwm_frequency = 20000;
      driver.voltage_limit = 4;
      driver.init();
      motor.linkDriver(&driver);
      motor.voltage_limit = 4;
      motor.controller = MotionControlType::velocity_openloop;
      motor.init();
      motor.disable();
      pinMode(aux1.PIN, INPUT_PULLUP); // Sets pin 14 on the ESP32 as an interrupt
      attachInterrupt(aux1.PIN, isr1, FALLING); // Triggers when aux1 is pulled to GND (button pressed)
      pinMode(aux2.PIN, INPUT_PULLUP); // Sets pin 13 on the ESP32 as an interrupt
      attachInterrupt(aux2.PIN, isr2, FALLING); // Triggers when aux2 is pulled to GND (button pressed)
      delay(100);

    Wire.begin();
    // Start serial communication at 115200 baud
    Serial.begin(115200);

    // Begin example of the magnetic sensor code (and add whitespace for easy reading)
    Serial.println("");
    Serial.println("------------------------------------------------------------------");
    Serial.println("TMAG5273 Example 3: Angle Calculations");
    Serial.println("------------------------------------------------------------------");

    Serial.println("");
    // If begin is successful (0), then start example
    if (sensor.begin(i2cAddress, Wire) == true)
    {
        Serial.println("Begin");
    }
    else // Otherwise, infinite loop
    {
        Serial.println("Device failed to setup - Freezing code.");
        while (1)
            ; // Runs forever
    }

    // Set the device at 32x average mode
    sensor.setConvAvg(conversionAverage);

    // Choose new angle to calculate from
    // Can calculate angles between XYX, YXY, YZY, and XZX
    sensor.setMagneticChannel(magneticChannel);

    // Enable the angle calculation register
    // Can choose between XY, YZ, or XZ priority
    sensor.setAngleEn(angleCalculation);
}

void loop()
{

  // =  motor demo stuff =
  // Button Press ISR
      if(aux1.pressed){ 
        aux1.pressed = false;
      }

      // Turning motor on and off
      if(aux2.pressed){ 
        aux2.pressed = false;
      }

      motor.move(target_velocity);

      // user communication
      command.run();
      
  //=========================Magnetic Sensor=============================
    if ((sensor.getMagneticChannel() != 0) &&
        (sensor.getAngleEn() != 0)) // Checks if mag channels are on - turns on in setup
    {
        float angleCalculation = sensor.getAngleResult();

        Serial.print("XYX: ");
        Serial.print(angleCalculation, 4);
        Serial.println("°");
    }
    else
    {
        Serial.println("Mag Channels disabled, stopping..");
        while (1)
            ;
    }

    delay(5);
}