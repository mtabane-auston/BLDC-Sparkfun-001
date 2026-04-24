/*
 * ---------------------------------------------------------------------------------
 * Magnetic Sensor Module - TMAG5273
 * ---------------------------------------------------------------------------------
 */
#include "magnetic_sensor.h"

// Create sensor object
TMAG5273 sensor;
uint8_t i2cAddress = TMAG5273_I2C_ADDRESS_INITIAL;

// Sensor configuration
uint8_t conversionAverage = TMAG5273_X32_CONVERSION;
uint8_t magneticChannel = TMAG5273_XYX_ENABLE;
uint8_t angleCalculation = TMAG5273_XY_ANGLE_CALCULATION;

// Global custom sensor instance
TMAG5273Sensor* foc_sensor = nullptr;

void sensor_setup()
{
    Wire.begin();
    Serial.println("TMAG5273 Angle Sensor Initialization");
    
    if (sensor.begin(i2cAddress, Wire) == true)
    {
        Serial.println("✓ TMAG5273 initialized successfully");
    }
    else
    {
        Serial.println("✗ TMAG5273 failed - Freezing");
        while (1);
    }
    
    sensor.setConvAvg(conversionAverage);
    sensor.setMagneticChannel(magneticChannel);
    sensor.setAngleEn(angleCalculation);
    Serial.println("✓ Sensor configured");
}

float get_angle()
{
    if ((sensor.getMagneticChannel() != 0) && (sensor.getAngleEn() != 0))
    {
        return sensor.getAngleResult();
    }
    return 0.0;
}

TMAG5273* get_sensor_instance()
{
    return &sensor;
}

// SimpleFOC Sensor Implementation
TMAG5273Sensor::TMAG5273Sensor(TMAG5273* sensor_ptr)
{
    _sensor = sensor_ptr;
}

void TMAG5273Sensor::init()
{
    // Sensor already initialized in sensor_setup()
}

float TMAG5273Sensor::getSensorAngle()
{
    if ((_sensor->getMagneticChannel() != 0) && (_sensor->getAngleEn() != 0))
    {
        float angle_deg = _sensor->getAngleResult();
        return angle_deg * PI / 180.0; // Convert to radians
    }
    return 0.0;
}