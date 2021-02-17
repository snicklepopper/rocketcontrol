#ifndef __altimeter_h
#define __altimeter_h

// Adafruit library
#include <Adafruit_BME280.h>


struct Altimeter {
    Adafruit_BME280     m_bme;          // The barometer/altimeter

    float               m_base_pressure;// Pressure - offset value

public:
    // Construct
    Altimeter();

    // Initialise
    void init();

    // Read values
    void read(
        float&              altitude,   // (O) Altitude
        float&              temp);      // (O) Temperature

    // Store current readings as zero 
    void zero();
};


#endif
