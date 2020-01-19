#include <ESP8266WiFi.h>

#define UART_BAUD 9600
#define packTimeout 10
#define bufferSize 4096

#define MODE_AP
//#define MODE_STA

#ifdef MODE_AP
#error Comment this line and set ssid and pw for the accesspoint.
const char *ssid = "esp03_serial";
const char *pw = "password";
IPAddress ip(192, 168, 0, 1);
IPAddress netmask(255, 255, 255, 0);
const int port = 4200;
#endif

#ifdef MODE_STA
#error Comment this line and set ssid and pw for your network.
const char *ssid = "myrouter";
const char *pw = "password";
const int port = 4200;
#endif

#include <WiFiClient.h>
WiFiServer server(port);
WiFiClient client;

uint8_t buf1[bufferSize];
uint16_t i1=0;

uint8_t buf2[bufferSize];
uint16_t i2=0;

void setup() {

  delay(500);
  
  Serial.begin(UART_BAUD);
  Serial.setTimeout(packTimeout);

  #ifdef MODE_AP 
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(ip, ip, netmask);
  WiFi.softAP(ssid, pw);
  #endif

  #ifdef MODE_STA
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pw);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
  #endif

  //Serial.println("Starting TCP Server");
  server.begin();
}

void loop() {

  if (!client.connected()) {
    client = server.available();
    return;
  }

  if (client.available()) {
    int availableForWrite = Serial.availableForWrite();
    if (availableForWrite > bufferSize) {
      availableForWrite = bufferSize;
    }
    while(client.available()) {
      buf1[i1] = (uint8_t)client.read();
      i1++;
      if (i1==availableForWrite) {
        break;
      }
    }
    Serial.write(buf1, i1);
    i1 = 0;
  }

  if (Serial.available()) {

    i2 = Serial.readBytes(buf2, bufferSize);
    
    uint16_t offset = 0;
    while (i2 > offset) {
      if (offset != 0) {
        delay(packTimeout);
      }
      offset += client.write((char*)&buf2[offset], i2 - offset);
    }
    i2 = 0;
  }
  
}
