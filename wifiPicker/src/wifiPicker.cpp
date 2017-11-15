#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <WiFiMulti.h>
#include <WebSocketsServer.h>
#include <ESPmDNS.h>
#include "wifiPicker.h"

WiFiMulti WiFiMulti;
WebServer wifiPickerServer(80);
WebSocketsServer wifiPickerWebSocket = WebSocketsServer(81);

bool DEBUG_PRINT = false;
bool IS_SETUP_DONE = false;
String NEW_SSID_STRING;
String NEW_PW_STRING;
String NETWORK_LIST;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            break;
        case WStype_CONNECTED:
            break;
        case WStype_TEXT:
            if(String((const char *)payload).startsWith("READYFORNETWORKS"))
            {
                wifiPickerWebSocket.sendTXT(num, NETWORK_LIST);
            }
            break;
    }
}

void setDebugPrint(bool ONOFF)
{
    DEBUG_PRINT = ONOFF;
}
void debugPrint(String PRINT)
{
    if(DEBUG_PRINT)
    {
        Serial.println(PRINT);
    }
}
bool attemptAutoConnect(void)
{    
    int counter = 0;
    bool WAS_ABLE_TO_CONNNECT_TO_NETWORK = true;
    bool BOOT_INTO_AP_MODE_FLAG = false;
    
    WiFi.begin();
    // TODO: If AP mode flag is false, attempt to
    // connect known network.
    if(!BOOT_INTO_AP_MODE_FLAG)
    {
        while (WiFi.status() != WL_CONNECTED) 
        {
            delay(500);
            counter++;
            if(counter>=20)
            {
                debugPrint("CAN NOT CONNECT TO NETWORK");
                WAS_ABLE_TO_CONNNECT_TO_NETWORK = false;
                break;
            }
        }
    }
    return WAS_ABLE_TO_CONNNECT_TO_NETWORK?1:0;
}
bool attemptManualConnect(const char* NEW_SSID, const char* NEW_PW)
{    
    int counter = 0;
    bool WAS_ABLE_TO_CONNNECT_TO_NETWORK = true;
    bool BOOT_INTO_AP_MODE_FLAG = false;
    
    WiFi.begin(NEW_SSID,NEW_PW);
    debugPrint("ATTEMPTING TO CONNECT TO NEW NETWORK WITH SSID:"+String(NEW_SSID)+" AND PW:"+String(NEW_PW));
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        counter++;
        if(counter>=20)
        {
            debugPrint("CAN NOT CONNECT TO NETWORK");
            debugPrint("RESETTING");
            ESP.restart();
            break;
        }
    }
    debugPrint("CONNNECTION ESTABLISHED");
    return 1;
}
String wifiScan(void)
{
    String wifiNetworkList;
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    debugPrint("SCANNING NETWORKS");
    int NUMBER_OF_NETWORKS = WiFi.scanNetworks();
    if (NUMBER_OF_NETWORKS == 0) 
    {
        debugPrint("NO NETWORKS FOUND");
    } 
    else 
    {
        debugPrint(String(NUMBER_OF_NETWORKS)+" NETWORKS FOUND");
        for (int NETWORK_ID = 0; NETWORK_ID < NUMBER_OF_NETWORKS; ++NETWORK_ID) 
        {
            wifiNetworkList += String(WiFi.SSID(NETWORK_ID));
            wifiNetworkList += "\n";
            delay(10);
        }
    }
    
    return wifiNetworkList;
}
void handleWifiPickerRoot() 
{
    wifiPickerServer.send_P(200, "text/html", WIFIPICKER_HTML);
}
void handleWifiPickerDone() 
{
    wifiPickerServer.send(200, "text/plain", "DONE!");
    
    NEW_SSID_STRING = wifiPickerServer.arg("SSID");
    NEW_PW_STRING = wifiPickerServer.arg("PW");
    
    debugPrint("NEW SSID: "+NEW_SSID_STRING);
    debugPrint("NEW PW: "+NEW_PW_STRING);
    IS_SETUP_DONE = true;
}
void handleWifiPickerNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += wifiPickerServer.uri();
  message += "\nMethod: ";
  message += (wifiPickerServer.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += wifiPickerServer.args();
  message += "\n";
  for (uint8_t i=0; i<wifiPickerServer.args(); i++){
    message += " " + wifiPickerServer.argName(i) + ": " + wifiPickerServer.arg(i) + "\n";
  }
  wifiPickerServer.send(404, "text/plain", message);
}
bool wifiPicker(const char* AP_SSID, const char* AP_PW)
{
    debugPrint("ATTEMPTING TO CONNECT TO KNOWN NETWORK");
    if(attemptAutoConnect())
    {
        debugPrint("CONNNECTION ESTABLISHED");
    }
    else
    {        
        //SCAN FOR NETWORKS
        NETWORK_LIST = wifiScan();
        
        //CREAT ACCESS POINT
        WiFi.softAP(AP_SSID, AP_PW); 
        
        debugPrint(" "); 
        debugPrint("CONNECT TO:");
        debugPrint(String(AP_SSID));
        debugPrint("NAVIGATE TO IP ADDRESS 192.168.4.1");
        debugPrint("PICK A NETWORK FROM THE SCANNED LIST");
        debugPrint("ENTER PASSWORD AND HIT OK TO CONNECT");
        debugPrint(" "); 
        
        //HANDLE WEB SERVER REQUESTS        
        wifiPickerServer.on("/", handleWifiPickerRoot);
        wifiPickerServer.on("/DONE", handleWifiPickerDone);
        wifiPickerServer.onNotFound(handleWifiPickerNotFound);
        wifiPickerServer.begin();
        wifiPickerWebSocket.begin();
        wifiPickerWebSocket.onEvent(webSocketEvent);
        
        //MAINTAIN THE AP AND WEBSERVER UNTIL CREDENTIALS HAVE BEEN SUBMITTED
        while(!IS_SETUP_DONE)
        {
            wifiPickerServer.handleClient();
            wifiPickerWebSocket.loop();
        }
        debugPrint(" ");
        debugPrint("SUCCESFULLY GOT CREDENTIALS");

        //DISCONNECT TURN OFF THE AP AND SPIN BACK UP IN STATION MODE WITH NEW CREDENTIALS
        WiFi.disconnect();
        WiFi.mode(WIFI_STA);
        
        const char* NEW_SSID = NEW_SSID_STRING.c_str();
        const char* NEW_PW = NEW_PW_STRING.c_str();
        attemptManualConnect(NEW_SSID, NEW_PW);
    }
    return 1;
}
