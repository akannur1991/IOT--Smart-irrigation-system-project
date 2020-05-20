#include <ssl_client.h>
#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <cert.h>
 
const char *SSID = "Shubham's iPhone";
const char *PWD = "shubh7482";
 
WiFiClientSecure secureClient = WiFiClientSecure();
PubSubClient mqttClient(secureClient);
 
void connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.println(SSID);
  WiFi.begin(SSID, PWD); 
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    // we can even make the ESP32 to sleep
  }
 
  Serial.print("Connected - ");
  
}
 
void connectToAWS() {
  mqttClient.setServer("http://a3jjkbw4vqw4wh-ats.iot.us-east-1.amazonaws.com/", 8883);
  secureClient.setCACert(AWS_PUBLIC_CERT);
  secureClient.setCertificate(AWS_DEVICE_CERT);
  secureClient.setPrivateKey(AWS_PRIVATE_KEY);
 
  Serial.println("Connecting to MQTT....");
 
  mqttClient.connect("Irrigation");
   
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT....Retry");
    mqttClient.connect("Irrigation");
    delay(5000);
  }
 
  Serial.println("MQTT Connected");
}
 
void setup() {
  Serial.begin(9600);
  connectToWiFi();
  connectToAWS();
}
