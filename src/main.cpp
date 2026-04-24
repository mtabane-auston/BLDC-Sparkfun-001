/*
 * ---------------------------------------------------------------------------------
 * Smart Knob Controller - TMAG5273 + SimpleFOC
 * ---------------------------------------------------------------------------------
 * Features:
 * - Multiple modes: Free spin, Bounded, Detent, Spring, Damped
 * - Haptic feedback with adjustable strength
 * - Button 1: Cycle through modes
 * - Button 2: Adjust number of detents (4, 8, 12, 16, 24, 48)
 * - Serial commands: M<mode>, D<detents>, H<strength>
 */
#include "magnetic_sensor.h"
#include "motor_control.h"
#include <Arduino.h>

extern TMAG5273Sensor* foc_sensor;

void setup()
{
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("");
    Serial.println("==================================================================");
    Serial.println("         SMART KNOB CONTROLLER - TMAG5273 + SimpleFOC");
    Serial.println("==================================================================");
    Serial.println("");
    
    // Initialize magnetic sensor
    sensor_setup();
    
    // Initialize motor control
    motor_setup();
    
    // Create SimpleFOC sensor wrapper
    foc_sensor = new TMAG5273Sensor(get_sensor_instance());
    
    // Link sensor to motor
    motor_link_sensor(foc_sensor);
    
    // Enable closed-loop torque control
    motor_enable_closed_loop();
    
    // Configure smart knob
    set_knob_mode(MODE_DETENT);        // Start in detent mode
    set_detent_positions(12);          // 12 positions (like a clock)
    set_bounds(0, 2 * PI);             // Full rotation
    set_haptic_strength(3.0);          // Medium strength
    
    Serial.println("");
    Serial.println("------------------------------------------------------------------");
    Serial.println("CONTROLS:");
    Serial.println("  Button 1 (GPIO 13): Cycle modes");
    Serial.println("  Button 2 (GPIO 14): Adjust detents (4/8/12/16/24/48)");
    Serial.println("");
    Serial.println("SERIAL COMMANDS:");
    Serial.println("  M<0-4>   - Set mode (0=Free, 1=Bounded, 2=Detent, 3=Spring, 4=Damped)");
    Serial.println("  D<num>   - Set detent positions (2-100)");
    Serial.println("  H<value> - Set haptic strength (0.1-10.0)");
    Serial.println("------------------------------------------------------------------");
    Serial.println("");
    Serial.println("✓ Smart Knob Ready!");
    Serial.println("");
}

void loop()
{
    // Update motor haptics
    motor_update();
    
    // Handle serial commands
    motor_handle_commands();
    
    // Optional: Print status every second
    static unsigned long last_print = 0;
    if (millis() - last_print > 1000) {
        last_print = millis();
        
        Serial.print("Angle: ");
        Serial.print(get_current_angle() * 180.0 / PI);
        Serial.print("°  Position: ");
        Serial.print(get_current_position());
        Serial.print("  Mode: ");
        Serial.println(get_current_mode());
    }
}