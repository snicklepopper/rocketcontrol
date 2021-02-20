#ifndef __accelerometer_h
#define __accelerometer_h


struct Accelerometer {
    float               m_base_accel_x; // Acceleration along x axis - offset value
    float               m_base_accel_y; // Acceleration along y axis - offset value
    float               m_base_accel_z; // Acceleration along z axis - offset value
    float               m_base_rot_x;   // Rotation around x axis - offset value
    float               m_base_rot_y;   // Rotation around y axis - offset value
    float               m_base_rot_z;   // Rotation around z axis - offset value

public:
    // Construct
    Accelerometer();

    // Initialise
    void init();

    // Read values
    void read(
        float&              accel_x,    // (O) Acceleration along x axis
        float&              accel_y,    // (O) Acceleration along y axis
        float&              accel_z,    // (O) Acceleration along z axis
        float&              rot_x,      // (O) Rotation around x axis
        float&              rot_y,      // (O) Rotation around y axis
        float&              rot_z);     // (O) Rotation around z axis

    // Store current readings as zero 
    void zero();
};


#endif
