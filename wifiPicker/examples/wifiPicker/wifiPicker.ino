#include <WiFi.h>
#include "wifiPicker.h"

/****************************************************************
SSID and PW for new network if no previously known network is 
available.
****************************************************************/
const char* AP_SSID = "ACCESS_POINT_SSID";
const char* AP_PW   = "ACCESS_POINT_PW";

void setup()
{
    /****************************************************************
    Start serial communications
    ****************************************************************/
    Serial.begin(115200);
    Serial.println();
    Serial.println("Attempting to connect to known network");

    /****************************************************************
    Turn debug information on
    ****************************************************************/
    setDebugPrint(true);
    
    /****************************************************************
    Attempt to connect to a known network. If no known network is 
    available, create a network with name AP_SSID and password AP_PW.
    Connect to the network and navigate to 192.168.4.1
    Select your network and enter in password to connect
    ****************************************************************/
    wifiPicker(AP_SSID,AP_PW);

    /****************************************************************
    If connection is successful, display IP address of ESP32
    ****************************************************************/
    Serial.println("Connected! IP is: ");
    Serial.println(WiFi.localIP());
}
void loop()
{
}
