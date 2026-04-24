/*
 * ---------------------------------------------------------------------------------
 * Magnetic Sensor Module - TMAG5273
 * ---------------------------------------------------------------------------------
 */
#ifndef MAGNETIC_SENSOR_H
#define MAGNETIC_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <SparkFun_TMAG5273_Arduino_Library.h>
#include <SimpleFOC.h>

// Custom sensor class implementing SimpleFOC Sensor interface
class TMAG5273Sensor : public Sensor
{
public:
    TMAG5273Sensor(TMAG5273* sensor_ptr);
    void init();
    float getSensorAngle();
    
private:
    TMAG5273* _sensor;
};

// Module functions
void sensor_setup();
float get_angle();
TMAG5273* get_sensor_instance();

#endif