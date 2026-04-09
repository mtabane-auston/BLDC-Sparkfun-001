/*
 * ---------------------------------------------------------------------------------
 * Magnetic Sensor Module - TMAG5273
 * ---------------------------------------------------------------------------------
 * Implementation of TMAG5273 magnetic angle sensor functionality
 */

#include "magnetic_sensor.h"

// Create sensor object
TMAG5273 sensor;

// I2C default address
uint8_t i2cAddress = TMAG5273_I2C_ADDRESS_INITIAL;

// Sensor configuration constants
uint8_t conversionAverage = TMAG5273_X32_CONVERSION;
uint8_t magneticChannel = TMAG5273_XYX_ENABLE;
uint8_t angleCalculation = TMAG5273_XY_ANGLE_CALCULATION;

void sensor_setup()
{
    Wire.begin();
    Serial.begin(115200);

    Serial.println("");
    Serial.println("------------------------------------------------------------------");
    Serial.println("TMAG5273 Angle Sensor Initialization");
    Serial.println("------------------------------------------------------------------");
    Serial.println("");

    // Initialize sensor
    if (sensor.begin(i2cAddress, Wire) == true)
    {
        Serial.println("TMAG5273 sensor initialized successfully");
    }
    else
    {
        Serial.println("TMAG5273 failed to initialize - Freezing code.");
        while (1)
            ; // Halt execution
    }

    // Configure sensor
    sensor.setConvAvg(conversionAverage);
    sensor.setMagneticChannel(magneticChannel);
    sensor.setAngleEn(angleCalculation);
    
    Serial.println("Sensor configuration complete");
}

float get_angle()
{
    if ((sensor.getMagneticChannel() != 0) && (sensor.getAngleEn() != 0))
    {
        return sensor.getAngleResult();
    }
    return 0.0;
}
