#include "Altimeter.h"

// Sprint/Sprintln
#include "my_trace.h"


// Constructor
Altimeter::Altimeter() 
    : 
    m_base_pressure(0)
{
}

// Initialise
void Altimeter::init()
{
    // Initialise the barometer
    bool status = m_bme.begin();  

    // Zero the altimeter
    zero();
}

// Read values
void Altimeter::read(
    float&              altitude,   // (O) Altitude
    float&              temp)       // (O) Temperature
{
    altitude = m_bme.readAltitude(m_base_pressure);
    temp = m_bme.readTemperature();
}

// Store current readings as zero 
void Altimeter::zero()
{
    m_base_pressure = m_bme.readPressure();
}
