#include "AsyncWebserver.h"

// Sprint/Sprintln
#include "my_trace.h"


// 404 error
void error_404(
    WiFiClient&         client)     // (I) Client connection
{
    client.println("HTTP/1.1 404 Not Found");
    client.println("Content-Type: text/plain; charset=utf-8");
    client.println("Access-Control-Allow-Origin: *");
    client.println("\n404 Not Found\n");
    client.stop();
}

AsyncWebserver::AsyncWebserver(
    const IPAddress&    ipAddress,  // (I) IP Address
    const char*         ssid,       // (I) Wifi access point name
    const char*         pass,       // (I) Wifi password
    const char*         webpage,    // (I) Web page
    const size_t        webpage_siz)// (I) Size of web page
    : 
    m_ipAddress(ipAddress), m_ssid(ssid), m_pass(pass), 
    m_webpage(webpage), m_webpage_siz(webpage_siz),
    m_httpServer(80), 
    m_trace_count(0)
{
}


// Add an integer trace
unsigned AsyncWebserver::add_trace(
    const String&       name,       // (I) Name of the trace
    int                 value)      // (I) Initial value
{
    // Store the new value
    if (m_trace_count < MAX_TRACES - 1) {
        m_trace_names[m_trace_count] = name;
        m_trace_values[m_trace_count] = String(value);
        Sprintln("Added trace for " + String(name) + " as #" + String(m_trace_count) + ".");
        unsigned index = m_trace_count;
        m_trace_count++;
        return index;
    } 
    else
    {
        Sprintln("Could not create trace for " + String(name) + ".  Too many active traces.");
        return -1;
    }
}


// Add a floating point trace
unsigned AsyncWebserver::add_trace(
    const String&       name,       // (I) Name of the trace
    float               value)      // (I) Initial value
{
    // Store the new value
    if (m_trace_count < MAX_TRACES - 1) {
        m_trace_names[m_trace_count] = name;
        m_trace_values[m_trace_count] = String(value);
        Sprintln("Added trace for " + String(name) + " as #" + String(m_trace_count) + ".");
        unsigned index = m_trace_count;
        m_trace_count++;
        return index;
    } 
    else
    {
        Sprintln("Could not create trace for " + String(name) + ".  Too many active traces.");
        return -1;
    }
}


// Set an integer trace
void AsyncWebserver::set_trace(
    unsigned            index,      // (I) Number of the trace
    int                 value)      // (I) New value
{
    // Store the new value
    if (index < m_trace_count) {
        m_trace_values[index] = String(value);
    }

    // Send a server-side event to the client if we have one
    send_trace(index);
}


// Set a floating point trace
void AsyncWebserver::set_trace(
    unsigned            index,      // (I) Number of the trace
    float               value)      // (I) New value
{
    // Store the new value
    if (index < m_trace_count) {
        m_trace_values[index] = String(value);
    }

    // Send a server-side event to the client if we have one
    send_trace(index);
}


// Send trace information to the SSE connection
void AsyncWebserver::send_trace(
    unsigned            index)     // (I) Number of the trace
{
    // Send a server-side event to the client if we have one
    if (m_ssePort.connected()) {
        Sprintln("data: " + m_trace_names[index] + "=" + String(m_trace_values[index]));
        m_ssePort.print("event: trace" + String(index) + "\n");
        m_ssePort.print("data: " + m_trace_names[index] + "=" + String(m_trace_values[index]) + "\n\n");
    }
}

// Poll to service client requests
void AsyncWebserver::poll()
{
    // If we lost connection, try to reconnect, but
    // return early from the poll if we couldn't
    if (!wifi_ensure_connected())
        return;

    // Connect to the client
    WiFiClient client = m_httpServer.available();
    if (client) {
        // Service the new connection
        service_http_connection(client);
    }
}


void AsyncWebserver::wifi_connect()
{
    // Set the IP address we want to use
    WiFi.config(m_ipAddress);

    // This is how we wish to be known
    WiFi.setHostname("nano-webserver");

    // Try to connect
    Sprint("Connecting to ");
    Sprintln(m_ssid);
    WiFi.begin(m_ssid, m_pass);

    // Wait five seconds to see if it worked
    delay(5000);

    // DId it work?
    if (WiFi.status() == WL_CONNECTED) 
    {
        Sprint("Connected to " + String(m_ssid));
        wifi_print_status();
        m_httpServer.begin();
    }

    // Switch down to low-power mode
    WiFi.lowPowerMode();
}


bool AsyncWebserver::wifi_ensure_connected()
{
    // Check whether we lost the connection
    if (WiFi.status() != WL_CONNECTED) {
        // We did lose it
        Sprintln("Wifi connection lost.");

        // Stop the SSE connection
        m_ssePort.stop();

        // End the old wifi connection
        WiFi.end();

        // Start the connection again
        wifi_connect();
    }

    // Are we now connected?
    return (WiFi.status() == WL_CONNECTED);
}


// Print wifi status
void AsyncWebserver::wifi_print_status()
{
    // print the SSID of the network you're attached to:
    Sprintln("SSID: " + String(WiFi.SSID()));

    // print your board's IP address:
    IPAddress ip = WiFi.localIP();
    Sprint("IP Address: ");
    Sprintln(ip);

    // Network gateway
    Sprint("Gateway: ");
    Sprintln(WiFi.gatewayIP());

    // Netmask
    Sprint("Netmask: ");
    Sprintln(WiFi.subnetMask());

    // print the received signal strength:
    Sprint("signal strength (RSSI):");
    Sprint(WiFi.RSSI());
    Sprintln(" dBm");
}


// Service an HTTP request
void AsyncWebserver::service_http_connection(
    WiFiClient&       client)     // (I) Website client connection
{
    Sprintln("New connection from " + String(client.remoteIP()));
    String elname = "";
    String value = "";

    // Continue while the client is connected
    String header = "";
    while (client.connected()) 
    {
        // Is new data ready?
        if (client.available()) 
        {
            // Read characters into the header...
            char c = client.read();
            if (c != '\r') {
                header += c;
            }

            /// ...the header is complete when it ends in two newlines
            if (header.substring(header.length() - 2) == "\n\n") 
            {
                Sprint(header.substring(0, header.length() - 1));

                // Is this a new connection
                if (header.indexOf("HEAD /?") > -1) {
                    // Return 200 
                    client.println("HTTP/1.1 200 OK");

                    // Always ignore CORS
                    client.println("Access-Control-Allow-Origin: *");

                    // Stop sending 
                    client.stop();
                    Sprintln("--Client disconnected");

                    // Find the search string to the right of the ? in the url
                    header.remove(0, header.indexOf("?")+1);
                    header.remove(header.indexOf(" "));

                    // Break into element=value
                    String elname = header.substring(0, header.indexOf("="));
                    String value = header.substring(header.indexOf("=") + 1);
                    Sprintln("Client wants to set " + elname + " to " + value + ".");
                }
                else if (header.indexOf("event-stream") > -1) 
                {
                    // Client wants to start a new server-side event stream

                    // If there was an old SSE client, close it
                    if (m_ssePort.connected()) {
                        m_ssePort.stop();
                    }

                    // Say we want to keep this connection open
                    client.println("HTTP/1.1 200 OK\nConnection: Keep-Alive");

                    // Always ignore CORS
                    client.println("Access-Control-Allow-Origin: *");

                    // We're going to be sending an event stream
                    client.println("Content-Type: text/event-stream\nCache-Control: no-cache\n");

                    // Use the existing connection for server-side events
                    m_ssePort = client;

                    // Transmit all current state
                    for (unsigned index = 0; index < m_trace_count; ++index) 
                    {
                        send_trace(index);
                    }

                    // We're running
                    Sprintln("--SSE client established");
                    break;
                } 
                else if (header.indexOf("POST / HTTP") > -1) 
                {
                    // Received a POST command.  Does this happen on 
                    // page close?

                    // Stop the client connection
                    client.println("HTTP/1.1 200 OK\n");
                    client.println("Access-Control-Allow-Origin: *");
                    client.stop();

                    // Stop the server-side event connection too
                    m_ssePort.stop();

                    // All gone
                    Sprintln("--Interface page was closed; all clients disconnected");
                }
                else if (header.indexOf("GET / HTTP") > -1) 
                {
                    // Basic page GET.  Return the stored web page
                    client.println("HTTP/1.1 200 OK\nContent-Type: text/html");
                    client.println("Access-Control-Allow-Origin: *");
                    client.println("");
                    const int webpage_gz_length = m_webpage_siz;
                    Sprint("Sending page of ");
                    Sprintln(webpage_gz_length);
                    int packetsize = 1024;
                    int done = 0;
                    while (webpage_gz_length > done) {
                        if (webpage_gz_length - done < packetsize) {
                            packetsize = webpage_gz_length - done;
                        }
                        client.write(m_webpage + done, packetsize * sizeof(char));
                        done = done + packetsize;
                        Sprint("...");
                        Sprintln(done);
                    }
                    delay(1000);
                    client.stop();
                    Sprintln("--Interface webpage sent; client disconnected");
                } 
                else if (header.indexOf("GET /author HTTP") > -1) 
                {
                    // They want to know the author
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/plain; charset=utf-8");
                    client.println("Access-Control-Allow-Origin: *");
                    client.println("\nDavid Tonge, david.tonge@davidtonge.com\n");
                    client.stop();
                    Sprintln("--Email address sent; client disconnected");
                } 
                else if (header.startsWith("GET "))
                {
                    // Page request
                    String page = header.substring(5);
                    page = page.substring(0, page.indexOf(" "));
                    Sprintln("REQUEST FOR PAGE >" + page + "<");

                    // Is this a request for teh button list?
                    bool handled = false;
                    if (page == "buttons") 
                    {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/plain; charset=utf-8");
                        client.println("Access-Control-Allow-Origin: *");
                        client.println("\n");
                        for (unsigned i=0; i<m_button_count; ++i)
                            client.println(m_button_names[i]);
                        client.stop();
                        handled = true;
                    }
                    else
                    {
                        // Find the function for the button with this name
                        for (unsigned i=0; i<m_button_count; ++i)
                        {
                            Sprintln("Checking " + String(i) + ":" + m_button_names[i] + " for " + page);
                            if (m_button_names[i] == page) 
                            {
                                String result = m_button_functions[i]();
                                client.println("HTTP/1.1 200 OK");
                                client.println("Content-Type: text/plain; charset=utf-8");
                                client.println("Access-Control-Allow-Origin: *");
                                client.println("\n");
                                client.println(result);
                                client.stop();
                                handled = true;
                                break;
                            }
                        }
                    }

                    // No idea what they want?
                    if (!handled) 
                    {
                        error_404(client);
                        Sprintln("--Page not found; client disconnected");
                    }
                }
                else 
                {
                    // No idea what they want
                    error_404(client);
                    Sprintln("--Page not found; client disconnected");
                }
            }
        }
    }
}


// Add a button
typedef String (*action_fn)();
void AsyncWebserver::add_button(
    const String&       name,       // (I) Name of the action
    action_fn           fn)         // (I) Function to call on click
{
    if (m_button_count < MAX_BUTTONS - 1) 
    {
        m_button_functions[m_button_count] = fn;
        m_button_names[m_button_count] = name;
        ++m_button_count;
    }
    else
    {
        Sprintln("Cannot add handler for " + name + " as already have too many buttons.");
    }
}


#if 0
IPAddress ip(192, 168, 1, 253);
WiFiServer server(80);
WiFiClient sse;
int ledPin = 12;
boolean on = false;
float volume = 0;
int cowbell = 1000;
float zdata[] = {1,1,1,1,1};
int zdatasize = sizeof(zdata) / sizeof(zdata[0]);
int ledState = 0;
unsigned long previousCowbellMillis = 0;
unsigned long previousGravityMillis = 0;


void setup() {
  #ifdef DEBUG
  Serial.begin(115200);
  #endif
  Sprintln("Setup()");
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ledPin, OUTPUT);
  //IMU.begin();
  WiFiConnect();
}

void loop() {
  //Sprintln("loop() = true");

  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW);
    Sprintln("\nLost WiFi connection");
    sse.stop();
    WiFi.end();
    WiFiConnect();
  }

  if (sse.connected()) {
    //Sprintln("sse.connnected() = true");
    sse.println("event: cowbell\ndata: " + (String)random(100) + "\n");
    //sse.println("event:message\ntick tock\n");
  }
  
  WiFiClient client = server.available();

  if (client) {
    Sprintln("\n--New client");
    String header = "";
    String elname = "";
    String value = "";
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c != '\r') {
          header += c;
        }
        if (header.substring(header.length() - 2) == "\n\n") {
          Sprint(header.substring(0, header.length() - 1));
          if (header.indexOf("HEAD /?") > -1) {
            client.println("HTTP/1.1 200 OK");
            #ifdef DEBUG
            client.println("Access-Control-Allow-Origin: *");
            #endif
            client.println("");
            client.stop();
            header.remove(0, header.indexOf("?")+1);
            header.remove(header.indexOf(" "));
            elname = header.substring(0, header.indexOf("="));
            value = header.substring(header.indexOf("=") + 1);
            boolean goodSettings = true;
            if (elname == "switch") {
              on = (value == "true");
            } else if (elname == "volume") {
              volume = (value.toFloat()*255)/100;
            } else if (elname == "cowbell") {
              cowbell = 1000 - (value.toInt()*10);
            } else {
              goodSettings = false;
            }
            if (goodSettings) {
              Sprintln("Sending to SSE, data: " + elname + " set to " + value);
              sse.println("data: " + elname + " set to " + value);
            } else {
              Sprintln("Sending to SSE, Bad data; ignored");
              sse.println("Bad data; ignored");
            }
            sse.println("");
            Sprintln("--Client disconnected");
          } else if (header.indexOf("event-stream") > -1) {
            if (sse.connected()) {
              sse.stop();
            }
            client.println("HTTP/1.1 200 OK\nConnection: Keep-Alive");
            #ifdef DEBUG
            client.println("Access-Control-Allow-Origin: *");
            #endif
            client.println("Content-Type: text/event-stream\nCache-Control: no-cache\n");
            sse = client;
            if (on) {
              Sprintln("Sending to SSE, event: sw\ndata: true\n");
              sse.println("event: sw\ndata: true\n");
            } else {
              Sprintln("Sending to SSE, event: sw\ndata: false\n");
              sse.println("event: sw\ndata: false\n");
            }
            sse.println("event: volume\ndata: " + (String)round((volume*100)/255) + "\n");
            sse.println("event: cowbell\ndata: " + (String)((1000-cowbell)/10) + "\n");
            Sprintln("--SSE client established");
            break;
          } else if (header.indexOf("POST / HTTP") > -1) {
            client.println("HTTP/1.1 200 OK\n");
            client.stop();
            sse.stop();
            Sprintln("--Interface page was closed; all clients disconnected");
          } else if (header.indexOf("GET / HTTP") > -1) {
            //client.println("HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Encoding: gzip\n");
            const int webpage_gz_length = sizeof(webpage_gz);
            Sprint("Sending page of");
            Sprintln(webpage_gz_length);
            int packetsize = 1024;
            int done = 0;
            while (webpage_gz_length > done) {
              if (webpage_gz_length - done < packetsize) {
                packetsize = webpage_gz_length - done;
              }
              client.write(webpage_gz + done, packetsize * sizeof(char));
              done = done + packetsize;
              Sprint("...");
              Sprintln(done);
            }
            delay(1000);
            client.stop();
            Sprintln("--Interface webpage sent; client disconnected");
          } else if (header.indexOf("GET /author HTTP") > -1) {
            client.println("HTTP/1.1 200 OK\nContent-Type: text/plain; charset=utf-8\n\nOscar den Uijl, oscar@den-uijl.nl\n");
            client.stop();
            Sprintln("--Email address sent; client disconnected");
          } else {
            client.println("HTTP/1.1 404 Not Found\nContent-Type: text/plain; charset=utf-8\n\n404 Not Found\n");
            client.stop();
            Sprintln("--Page not found; client disconnected");
          }
        }
      }
    }
  }
  
  unsigned long currentCowbellMillis = millis();
  if ((currentCowbellMillis - previousCowbellMillis) > cowbell) {
    previousCowbellMillis = currentCowbellMillis;   
    if (on) {
      if (cowbell == 1000) {
        analogWrite(ledPin, (int) round(volume));
      } else {
        if (ledState == 0) {
          ledState = 1;
          analogWrite(ledPin, (int) round(volume));
        } else {
          ledState = 0;
          analogWrite(ledPin, 0);
        }
      }
    } else {
      analogWrite(ledPin, 0);
    }
  }

  float x, y, z, zsum = 0;
}
#endif
