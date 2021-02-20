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
    Sprint("Initialising altimeter...");
    bool status = m_bme.begin();
    if (status) 
    {
        Sprintln("OK.");
    }
    else 
    {
        Sprintln("Failed.");
    }

    // Zero the altimeter
    zero();
}

// Read values
void Altimeter::read(
    float&              altitude,   // (O) Altitude
    float&              temp)       // (O) Temperature
{
    Sprintln("Base pressure is" + String(m_base_pressure));
    altitude = m_bme.readAltitude(m_base_pressure);
    temp = m_bme.readTemperature();
    Sprintln("Altitude = " + String(altitude) + ", temp = " + String(temp));
}

// Store current readings as zero 
void Altimeter::zero()
{
    m_base_pressure = m_bme.readPressure() * 0.01;
    Sprintln("Base pressure is" + String(m_base_pressure));
}
