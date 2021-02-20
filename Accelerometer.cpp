#include "Accelerometer.h"

// IMU built into Nano 33 IOT
#include <Arduino_LSM6DS3.h>

// Sprint/Sprintln
#include "my_trace.h"


// Constructor
Accelerometer::Accelerometer() 
    : 
    m_base_accel_x(0),
    m_base_accel_y(0),
    m_base_accel_z(0),
    m_base_rot_x(0),
    m_base_rot_y(0),
    m_base_rot_z(0)
{
}

// Initialise
void Accelerometer::init()
{
    Sprintln("Starting acceleromter");
    if (!IMU.begin()) 
    {
        Sprintln("\nFailed to started acceleromter.");
    }
    else
    {
        Sprintln("\nStarted acceleromter.");
        Sprintln("Accelerometer sample rate = ");
        Sprintln(IMU.accelerationSampleRate());
        Sprintln(" Hz");
    }
}

// Read values
void Accelerometer::read(
    float&              accel_x,    // (O) Acceleration along x axis
    float&              accel_y,    // (O) Acceleration along y axis
    float&              accel_z,    // (O) Acceleration along z axis
    float&              rot_x,      // (O) Rotation around x axis
    float&              rot_y,      // (O) Rotation around y axis
    float&              rot_z)      // (O) Rotation around z axis
{
    if (IMU.accelerationAvailable()) 
    {
        IMU.readAcceleration(accel_x, accel_y, accel_z);
        Sprintln("Acceleration " + String(accel_x) + ", " + String(accel_y) + ", " + String(accel_z));
    }

    if (IMU.gyroscopeAvailable())
    {
        IMU.readGyroscope(rot_x, rot_y, rot_z);
        Sprintln("Gyroscope " + String(rot_x) + ", " + String(rot_y) + ", " + String(rot_z));
    }

    // Adjust for the zero value
    accel_x -= m_base_accel_x;
    accel_y -= m_base_accel_y;
    accel_z -= m_base_accel_z;
    rot_x -= m_base_rot_x;
    rot_y -= m_base_rot_y;
    rot_z -= m_base_rot_z;
}

// Store current readings as zero 
void Accelerometer::zero()
{
    float accel_x, accel_y, accel_z, rot_x, rot_y, rot_z;
    read(accel_x, accel_y, accel_z, rot_x, rot_y, rot_z);
    m_base_accel_x += accel_x;
    m_base_accel_y += accel_y;
    m_base_accel_z += accel_z;
    m_base_rot_x += rot_x;
    m_base_rot_y += rot_y;
    m_base_rot_z += rot_z;
}
