// Secrets
#include "arduino_secrets.h"

// Arduino Wifi library
#include <WiFiNINA.h>

// Sprintln/Sprint
#include "my_trace.h"

// The webpage
#include "webpage.h"

#include "AsyncWebserver.h"
#include "Accelerometer.h"
#include "Altimeter.h"

// Website
IPAddress ip(192, 168, 1, 253);
AsyncWebserver webserver(ip, SECRET_SSID, SECRET_PASS, webpage_gz, sizeof(webpage_gz));

// Accelerometer
Accelerometer accelerometer;

// Altimeter
Altimeter altimeter;

// Values we send to the client
unsigned id_accel_x;
unsigned id_accel_y;
unsigned id_accel_z;
unsigned id_accel;
unsigned id_accel_max;
unsigned id_gyro_x;
unsigned id_gyro_y;
unsigned id_gyro_z;
unsigned id_temp;
unsigned id_altitude;

float max_accel = 0;

String reset()
{
    max_accel = 0;
    accelerometer.zero();
    altimeter.zero();

    return "Reset complete.";
}

void setup() 
{
    // Connect the serial port
    Serial.begin(115200);
    Sprintln("setup()");

    // Register the number we want to send to the client
    id_altitude = webserver.add_trace("altitude", 0);
    id_temp = webserver.add_trace("temp", 0);
    id_accel = webserver.add_trace("accel", 0);
    id_accel_max = webserver.add_trace("accel_max", 0);
    id_accel_x = webserver.add_trace("accel_x", 0);
    id_accel_y = webserver.add_trace("accel_y", 0);
    id_accel_z = webserver.add_trace("accel_z", 0);
    id_gyro_x = webserver.add_trace("gyro_x", 0);
    id_gyro_y = webserver.add_trace("gyro_y", 0);
    id_gyro_z = webserver.add_trace("gyro_z", 0);

    // Add buttons for stuff
    webserver.add_button("Reset", reset);

    // Start the sensors
    accelerometer.init();
    altimeter.init();
}

void loop()
{
    // Check whether anything's happening on the webserver
    webserver.poll();

    // Sense stuff
    float accel_x, accel_y, accel_z, rot_x, rot_y, rot_z;
    accelerometer.read(accel_x, accel_y, accel_z, rot_x, rot_y, rot_z);

    // Net acceleration
    float accel_net = sqrt(accel_x * accel_x + accel_y * accel_y + accel_z * accel_z);
    max_accel = max(max_accel, accel_net);

    // Read the altitude from the barometer
    float altitude, temp;
    altimeter.read(altitude, temp);

    // Observe a new value
    webserver.set_trace(id_altitude, altitude);
    webserver.set_trace(id_temp, temp);
    webserver.set_trace(id_accel, accel_net);
    webserver.set_trace(id_accel_max, max_accel);
    webserver.set_trace(id_accel_x, accel_x);
    webserver.set_trace(id_accel_y, accel_y);
    webserver.set_trace(id_accel_z, accel_z);
    webserver.set_trace(id_gyro_x, rot_x);
    webserver.set_trace(id_gyro_y, rot_y);
    webserver.set_trace(id_gyro_z, rot_z);
}
