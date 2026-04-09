/*
 * ---------------------------------------------------------------------------------
 * Main Robot Controller
 * ---------------------------------------------------------------------------------
 * Combines TMAG5273 magnetic angle sensor and SimpleFOC BLDC motor control
 */

#include "magnetic_sensor.h"
#include "motor_control.h"
#include <Arduino.h>

void setup()
{
    delay(1000);
    
    // Initialize motor control
    motor_setup();
    
    // Initialize magnetic sensor
    sensor_setup();
}

void loop()
{
    // Update motor position/velocity
    motor_update();
    
    // Read magnetic sensor angle
    float angle = get_angle();
    
    // Print angle to serial (optional - can remove if not needed)
    Serial.print("Angle (XY): ");
    Serial.print(angle, 4);
    Serial.println("°");
    
    // Handle serial commands (Commander)
    motor_handle_commands();
    
    delay(5);
}