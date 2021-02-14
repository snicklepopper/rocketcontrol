// Secrets
#include "arduino_secrets.h"

// Arduino Wifi library
#include <WiFiNINA.h>

// Sprintln/Sprint
#include "my_trace.h"

// The webpage
#include "webpage.h"

#include "AsyncWebserver.h"

// Website
IPAddress ip(192, 168, 1, 253);
AsyncWebserver webserver(ip, SECRET_SSID, SECRET_PASS, webpage_gz, sizeof(webpage_gz));

// Values we send to the client
unsigned id_random;
unsigned id_counter;
unsigned id_temp;

unsigned counter = 0;

void setup() 
{
    // Connect the serial port
    Serial.begin(115200);
    Sprintln("setup()");

    // Register a number we want to send to the client
    id_random = webserver.add_trace_int("random", random(100));
    id_counter = webserver.add_trace_int("counter", counter);
    id_temp = webserver.add_trace_int("temp", 21);
}

int n = 1;
void loop()
{
    // Check whether anything's happening on the webserver
    webserver.poll();

    // Observe a new value
    if (counter % 1000 == 0) webserver.set_trace_int(id_random, random(100));
    webserver.set_trace_int(id_counter, ++counter);
}
