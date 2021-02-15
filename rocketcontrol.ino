// Secrets
#include "arduino_secrets.h"

// Arduino Wifi library
#include <WiFiNINA.h>

// Sprintln/Sprint
#include "my_trace.h"

// The webpage
#include "webpage.h"

// The web server thing
#include "AsyncWebserver.h"

// The gyroscope
#include <Adafruit_LSM6DSOX.h>

// Website
IPAddress ip(192, 168, 1, 253);
AsyncWebserver webserver(ip, SECRET_SSID, SECRET_PASS, webpage_gz, sizeof(webpage_gz));

// Accelerometer
Adafruit_LSM6DSOX sox;

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

unsigned counter = 0;
float max_accel = 0;

String reset()
{
    max_accel = 0;

    return "Reset complete.";
}

void setup() 
{
    // Connect the serial port
    Serial.begin(115200);
    Sprintln("setup()");

    // Register the number we want to send to the client
    id_accel_x = webserver.add_trace("accel_x", 0);
    id_accel_y = webserver.add_trace("accel_y", 0);
    id_accel_z = webserver.add_trace("accel_z", 0);
    id_accel = webserver.add_trace("accel", 0);
    id_accel_max = webserver.add_trace("accel_max", 0);
    id_gyro_x = webserver.add_trace("gyro_x", 0);
    id_gyro_y = webserver.add_trace("gyro_y", 0);
    id_gyro_z = webserver.add_trace("gyro_z", 0);
    id_temp = webserver.add_trace("temp", 0);

    // Add buttons for stuff
    webserver.add_button("Reset", reset);

    // Start the accelerometer
    Sprint("Starting acceleromter");
    if (!sox.begin_I2C()) {
        while (1) {
            Sprint(".");
            delay(10);
        }
    }
    Sprintln("\nStarted acceleromter.");

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
}

int n = 1;
void loop()
{
    // Check whether anything's happening on the webserver
    webserver.poll();

    // Sense stuff
    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;
    sox.getEvent(&accel, &gyro, &temp);

    // Net acceleration
    float accel_net = sqrt(accel.acceleration.x * accel.acceleration.x +
                           accel.acceleration.y * accel.acceleration.y +
                           accel.acceleration.z * accel.acceleration.z);
    max_accel = max(max_accel, accel_net);

    // Observe a new value
    webserver.set_trace(id_temp, temp.temperature);
    webserver.set_trace(id_accel_x, accel.acceleration.x);
    webserver.set_trace(id_accel_y, accel.acceleration.y);
    webserver.set_trace(id_accel_z, accel.acceleration.z);
    webserver.set_trace(id_accel, accel_net);
    webserver.set_trace(id_accel_max, max_accel);
    webserver.set_trace(id_gyro_x, gyro.gyro.x);
    webserver.set_trace(id_gyro_y, gyro.gyro.y);
    webserver.set_trace(id_gyro_z, gyro.gyro.z);


  Serial.print("\t\tTemperature ");
  Serial.print(temp.temperature);
  Serial.println(" deg C");
 
  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("\t\tAccel X: ");
  Serial.print(accel.acceleration.x);
  Serial.print(" \tY: ");
  Serial.print(accel.acceleration.y);
  Serial.print(" \tZ: ");
  Serial.print(accel.acceleration.z);
  Serial.println(" m/s^2 ");
 
  /* Display the results (rotation is measured in rad/s) */
  Serial.print("\t\tGyro X: ");
  Serial.print(gyro.gyro.x);
  Serial.print(" \tY: ");
  Serial.print(gyro.gyro.y);
  Serial.print(" \tZ: ");
  Serial.print(gyro.gyro.z);
  Serial.println(" radians/s ");
  Serial.println();
 
  delay(100);
}
