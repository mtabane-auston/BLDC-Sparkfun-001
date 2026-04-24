/*
 * ---------------------------------------------------------------------------------
 * Motor Control Module - SimpleFOC Smart Knob
 * ---------------------------------------------------------------------------------
 */
#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <Arduino.h>
#include <SimpleFOC.h>

// Pin definitions
#define uh16 16
#define ul17 17
#define vh18 18
#define vl23 23
#define wh19 19
#define wl33 33
#define curSense 39
#define auxBtn1 13
#define auxBtn2 14

// Smart Knob Modes
enum KnobMode {
    MODE_FREE_SPIN,      // No resistance, free rotation
    MODE_BOUNDED,        // Soft endstops at min/max
    MODE_DETENT,         // Snap to positions with haptic feedback
    MODE_SPRING,         // Returns to center position
    MODE_DAMPED          // Heavy resistance, like moving through fluid
};

// Motor control functions
void motor_setup();
void motor_update();
void motor_handle_commands();
void motor_link_sensor(Sensor* sensor);
void motor_enable_closed_loop();

// Smart knob functions
void set_knob_mode(KnobMode mode);
void set_detent_positions(int num_positions);
void set_bounds(float min_angle, float max_angle);
void set_haptic_strength(float strength);
void set_center_position(float angle);

// Getters
float get_current_angle();
int get_current_position();
KnobMode get_current_mode();

#endif