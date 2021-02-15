#ifndef __async_webserver_h
#define __async_webserver_h

// Arduino Wifi library
#include <WiFiNINA.h>

#define MAX_TRACES 20
#define MAX_BUTTONS 5


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
    String                  m_trace_values[MAX_TRACES];     // Values of trace

    // Buttons to click
    typedef String (*action_fn)();
    String                  m_button_names[MAX_BUTTONS];    // Names of buttons
    unsigned                m_button_count;                 // Number of buttons
    action_fn               m_button_functions[MAX_BUTTONS];// Button handlers

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

    // Add a trace
    unsigned add_trace(
        const String&       name,       // (I) Name of the trace
        int                 value);     // (I) Initial value
    unsigned add_trace(
        const String&       name,       // (I) Name of the trace
        float               value);     // (I) Initial value

    // Set a trace
    void set_trace(
        unsigned            index,      // (I) Number of the trace
        int                 value);     // (I) New value
    void set_trace(
        unsigned            index,      // (I) Number of the trace
        float               value);     // (I) New value

    // Add a button
    void add_button(
        const String&       name,       // (I) Name of the action
        action_fn           fn);        // (I) Function to call on click

    // Poll
    void poll();
};


#endif
