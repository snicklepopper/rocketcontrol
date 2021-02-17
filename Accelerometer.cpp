#include "Accelerometer.h"

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
    Sprint("Starting acceleromter");
    if (!m_sox.begin_I2C()) {
        while (1) {
            Sprint(".");
            delay(10);
        }
    }
    Sprintln("\nStarted acceleromter.");
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
    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;
    m_sox.getEvent(&accel, &gyro, &temp);

    accel_x = accel.acceleration.x - m_base_accel_x;
    accel_y = accel.acceleration.y - m_base_accel_y;
    accel_z = accel.acceleration.z - m_base_accel_z;
    rot_x = gyro.gyro.x - m_base_rot_x;
    rot_y = gyro.gyro.y - m_base_rot_y;
    rot_z = gyro.gyro.z - m_base_rot_z;
}

// Store current readings as zero 
void Accelerometer::zero()
{
    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;
    m_sox.getEvent(&accel, &gyro, &temp);
    m_base_accel_x = accel.acceleration.x;
    m_base_accel_y = accel.acceleration.y;
    m_base_accel_z = accel.acceleration.z;
    m_base_rot_x = gyro.gyro.x;
    m_base_rot_y = gyro.gyro.y;
    m_base_rot_z = gyro.gyro.z;
}


#if 0
    // sox.setAccelRange(LSM6DS_ACCEL_RANGE_2_G);
    Serial.print("Accelerometer range set to: ");
    switch (sox.getAccelRange()) {
    case LSM6DS_ACCEL_RANGE_2_G:
        Serial.println("+-2G");
        break;
    case LSM6DS_ACCEL_RANGE_4_G:
        Serial.println("+-4G");
        break;
    case LSM6DS_ACCEL_RANGE_8_G:
        Serial.println("+-8G");
        break;
    case LSM6DS_ACCEL_RANGE_16_G:
        Serial.println("+-16G");
        break;
    }
    
    // sox.setGyroRange(LSM6DS_GYRO_RANGE_250_DPS );
    Serial.print("Gyro range set to: ");
    switch (sox.getGyroRange()) {
    case LSM6DS_GYRO_RANGE_125_DPS:
        Serial.println("125 degrees/s");
        break;
    case LSM6DS_GYRO_RANGE_250_DPS:
        Serial.println("250 degrees/s");
        break;
    case LSM6DS_GYRO_RANGE_500_DPS:
        Serial.println("500 degrees/s");
        break;
    case LSM6DS_GYRO_RANGE_1000_DPS:
        Serial.println("1000 degrees/s");
        break;
    case LSM6DS_GYRO_RANGE_2000_DPS:
        Serial.println("2000 degrees/s");
        break;
    case ISM330DHCX_GYRO_RANGE_4000_DPS:
        break; // unsupported range for the DSOX
    }
    
    // sox.setAccelDataRate(LSM6DS_RATE_12_5_HZ);
    Serial.print("Accelerometer data rate set to: ");
    switch (sox.getAccelDataRate()) {
    case LSM6DS_RATE_SHUTDOWN:
        Serial.println("0 Hz");
        break;
    case LSM6DS_RATE_12_5_HZ:
        Serial.println("12.5 Hz");
        break;
    case LSM6DS_RATE_26_HZ:
        Serial.println("26 Hz");
        break;
    case LSM6DS_RATE_52_HZ:
        Serial.println("52 Hz");
        break;
    case LSM6DS_RATE_104_HZ:
        Serial.println("104 Hz");
        break;
    case LSM6DS_RATE_208_HZ:
        Serial.println("208 Hz");
        break;
    case LSM6DS_RATE_416_HZ:
        Serial.println("416 Hz");
        break;
    case LSM6DS_RATE_833_HZ:
        Serial.println("833 Hz");
        break;
    case LSM6DS_RATE_1_66K_HZ:
        Serial.println("1.66 KHz");
        break;
    case LSM6DS_RATE_3_33K_HZ:
        Serial.println("3.33 KHz");
        break;
    case LSM6DS_RATE_6_66K_HZ:
        Serial.println("6.66 KHz");
        break;
    }
    
    // sox.setGyroDataRate(LSM6DS_RATE_12_5_HZ);
    Serial.print("Gyro data rate set to: ");
    switch (sox.getGyroDataRate()) {
    case LSM6DS_RATE_SHUTDOWN:
        Serial.println("0 Hz");
        break;
    case LSM6DS_RATE_12_5_HZ:
        Serial.println("12.5 Hz");
        break;
    case LSM6DS_RATE_26_HZ:
        Serial.println("26 Hz");
        break;
    case LSM6DS_RATE_52_HZ:
        Serial.println("52 Hz");
        break;
    case LSM6DS_RATE_104_HZ:
        Serial.println("104 Hz");
        break;
    case LSM6DS_RATE_208_HZ:
        Serial.println("208 Hz");
        break;
    case LSM6DS_RATE_416_HZ:
        Serial.println("416 Hz");
        break;
    case LSM6DS_RATE_833_HZ:
        Serial.println("833 Hz");
        break;
    case LSM6DS_RATE_1_66K_HZ:
        Serial.println("1.66 KHz");
        break;
    case LSM6DS_RATE_3_33K_HZ:
        Serial.println("3.33 KHz");
        break;
    case LSM6DS_RATE_6_66K_HZ:
        Serial.println("6.66 KHz");
        break;
    }
#endif
