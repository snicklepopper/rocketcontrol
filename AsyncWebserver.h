#ifndef __async_webserver_h
#define __async_webserver_h

// Arduino Wifi library
#include <WiFiNINA.h>

#define MAX_TRACES 10


struct AsyncWebserver {
    const IPAddress&        m_ipAddress;
    const char*             m_ssid;
    const char*             m_pass;
    const char*             m_webpage;
    const size_t            m_webpage_siz;                  // Size of web page
    WiFiServer              m_httpServer;
    WiFiClient              m_ssePort;

    // Integers to trace
    String                  m_trace_names[MAX_TRACES];      // Names of traces
    unsigned                m_trace_count;                  // Number of traces
    int                     m_trace_values[MAX_TRACES];     // Values of trace

    // Send trace information to the SSE connection
    void send_trace(
        unsigned            index);     // (I) Number of the trace

    // Service an HTTP request
    void service_http_connection(
        WiFiClient&         client);    // (I) Website client connection

    // Connected to wifi
    void wifi_connect();

    // Ensure we're connected to wifi
    bool wifi_ensure_connected();

    // Print wifi status
    void wifi_print_status();

public:
    // Construct
    AsyncWebserver(
        const IPAddress&    ipAddress,  // (I) IP Address
        const char*         ssid,       // (I) Wifi access point name
        const char*         pass,       // (I) Wifi password
        const char*         webpage,    // (I) Web page
        const size_t        webpage_siz);// (I) Size of web page

    // Add an integer trace
    unsigned add_trace_int(
        const String&       name,       // (I) Name of the trace
        int                 value);     // (I) Initial value

    // Set an integer trace
    void set_trace_int(
        unsigned            index,      // (I) Number of the trace
        int                 value);     // (I) New value

    // Poll
    void poll();
};


#endif
