/*
 * ---------------------------------------------------------------------------------
 * Motor Control Module - SimpleFOC
 * ---------------------------------------------------------------------------------
 * Header file for BLDC motor control functionality
 */

#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <Arduino.h>
#include <SimpleFOC.h>

// GPIO pin definitions
#define auxBtn2   13
#define auxBtn1   14  

// Driver pins
#define uh16      16
#define ul17      17
#define vh18      18
#define wh19      19
#define vl23      23
#define wl33      33
#define curSense  32

// Function declarations
void motor_setup();
void motor_update();
void motor_handle_commands();
void set_target_velocity(float velocity);
float get_target_velocity();

#endif
