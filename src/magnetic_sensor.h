/*
 * ---------------------------------------------------------------------------------
 * Magnetic Sensor Module - TMAG5273
 * ---------------------------------------------------------------------------------
 * Header file for TMAG5273 magnetic angle sensor functionality
 */

#ifndef MAGNETIC_SENSOR_H
#define MAGNETIC_SENSOR_H

#include "SparkFun_TMAG5273_Arduino_Library.h"
#include <Wire.h>
#include <Arduino.h>

void sensor_setup();
float get_angle();

#endif
