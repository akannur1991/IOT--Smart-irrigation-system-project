#include <ssl_client.h>
#include <Arduino.h>
//#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
//#include <cert.h>
//#include <AWS_IOT.h>
#include <Adafruit_Sensor.h>
#include "ArduinoJson.h" 
#include "DHT.h"

#define DHTPIN 14    // what pin we're connected to

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);

int motorPin = 27;
const int moistPin = 33;
int moisture = 0;

const int waterPin = 32;
int water = 0;

WiFiClientSecure secureClient = WiFiClientSecure();
void callback(char* topic, byte* payload, unsigned int length);
//PubSubClient mqttClient("a3jjkbw4vqw4wh-ats.iot.us-east-1.amazonaws.com",8883,callback,secureClient);
PubSubClient mqttClient(secureClient);

const char AWS_PUBLIC_CERT[] = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";
 
const char AWS_PRIVATE_KEY[] = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpgIBAAKCAQEA8GjjfHb6zZBOhrGxEA4af7l+vc5jUIZ8sY/axst2qKz0ObB3
+SRgm+SthpD8JA2yFVtNOIPVLLvhfXDe5xDA0znRCQuCDPrMcScBdx+//BoaqaQp
zUnRjrsmMaxk/zlceF79EltWT481Lr8Q2rbOE6TRbzd1APWu8j6NLQV8y95WrYlm
jhZspRc5cWMmLchG1lamK0WZSWNhJaK7yAPDiXjiY+Eaa3Zz0uTDBIk72LDGOARx
ljYSXPDs3/J/Z9TdXpiZJw2E1LOHladLIJJQecuKX+V98FrNn6v9Nt46ktwGFf38
4zLoUP8h2OlHE4XN0wEfGChW8pyIWB0JOxsfowIDAQABAoIBAQDe9afkisPXWXo6
WRmJdksFIZd1mK9qww9p7axMqe4GSUDHL+5UVXRzoAV+w3/UQA9k1yQqwbk2n5hk
qgVP7q2mN/yxksauGvr4BeDKBdwbhmor4mAST9/Vq6OkdenE+gHUcOPr3mGRr5L3
I6QR82/zr+u8ZYZRuYjiiTiNBIXU2zcHcInNNURrVkbC5nBvR8A+nddjVB6//PoR
y1sWVLTq3XH2qyFfze5dNLxDr1wTPUjqghVW8ZJdjBAUmabBdu0edFmT1pgHNKsf
A40S2TAzbQ53qM1ES8KwhIWsmRmrgoIr09D5vqr1UjQEUHddjm8qzPUvDQmZtgGk
JcBj0nWBAoGBAPiEurHMLPBJEnj3iNONoCNvohhsIDH1avbe0wDhMeq3hQCda9Qn
hSVkrqSt8Jk9V/dWdblumznQ1cUQ9gSc4urYEtmmqDFu5XAh1MeCyd7elVH97Anq
EQAM64G6C2Bz8j6gvsrVzBbqqw4TlhOpGPRqxCtFPl1fCVUmvh5tcCeRAoGBAPel
qsxrynIQIBqFjo/KzCKiEU7zV5nR6F4VhqzLBZ94+GTD5ITxcU4/sucgcxDhbWmC
eT0/LedK7dy3PZk5LBPlFQUzEHtJCw2uoE7J8q9RD1yRhYVJ7WGP0t7q7Z0HU2PT
+QndOAdyb97GO5TwMBxFMchSvps30wnW87aengHzAoGBAPWqt6dk75OEmJcuE3sV
iJUfGHzN/jGEqn1qHenDs4WUbw0BkQNDIfKqCZ5POAUzx86AEJYDCb1ofLl3H8vF
jueaO2AeyCs9td9YAjBLUOVY4QERYxEV6kJDg1dVI0PwuipA2vpFyJ9q0Td0EypS
QZH0LQGdTJ4egWpEhzIq+vCRAoGBANmnwXXpRwT5Jz4oJCQ9GS1XQ98gAN4wnwoR
4Uv9I/QFXksx4Kb3jZ44XgTFgFxVxNP22Jf771xZfrV+V6VTmmnh7ZekSdgRWqWr
1TlqROHRXK7/N1runrQgtDU4RZ690YzrwPXqeFQTQD+L+tHsSFyQdDCciOnjJph/
U7QttLIBAoGBAN/TuSEea0hu88zP7+HiDXz3VnUE9TA1a+ZK1iPepAeOO+llVqG7
JBVp5Wa+KQNA4/bWYurSjuxq7b20Xkpl3d8cLCZuwReCXe4QBdl99ZXvkd+RRUEv
9nG02Tnp9SpDIfoo0VgSgTc2dm0XHt5NTZZSaPJy4mOUpjUE539gdN5D
-----END RSA PRIVATE KEY-----

)KEY";
 
const char AWS_DEVICE_CERT[] = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWjCCAkKgAwIBAgIVAJfF+//+vFvrCyTOTEyqkZhqVyrfMA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMDAyMjExOTA3
MjJaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDwaON8dvrNkE6GsbEQ
Dhp/uX69zmNQhnyxj9rGy3aorPQ5sHf5JGCb5K2GkPwkDbIVW004g9Usu+F9cN7n
EMDTOdEJC4IM+sxxJwF3H7/8GhqppCnNSdGOuyYxrGT/OVx4Xv0SW1ZPjzUuvxDa
ts4TpNFvN3UA9a7yPo0tBXzL3latiWaOFmylFzlxYyYtyEbWVqYrRZlJY2ElorvI
A8OJeOJj4RprdnPS5MMEiTvYsMY4BHGWNhJc8Ozf8n9n1N1emJknDYTUs4eVp0sg
klB5y4pf5X3wWs2fq/023jqS3AYV/fzjMuhQ/yHY6UcThc3TAR8YKFbynIhYHQk7
Gx+jAgMBAAGjYDBeMB8GA1UdIwQYMBaAFITUxMkjNmPW3Zbd9sI8GRTYc62nMB0G
A1UdDgQWBBR9mT9Xi6C/EFoL2bgNoSDpjXyMgjAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEABRnlnYm+gx2C9Q0EMJ1atxEn
0Gn8Xo8h1wJkod1AahRyIp0zd6Uz4mZD9r4ocmnbkaSkuWxcvn+ZrcioiBizBKHK
+QHKWmxJj8D/9WjpoI1mywCpLN4ZJ0qYzVu0R5EtzCJIIMy3qzddR3GJgVN+p2JQ
++xdlCPgrEH1kHacYWNozPXQFX3WE1036c0r6U64duNlhdu7U51SA3t4Olvisilk
2R9aA2bsce1FhPMmMeNsZNnNqC4aCDu8x3ghiv8iIkjfhDo/n9WabMlhaqp3AnW5
MYNPa3jkdzCffZwJqgM04vXV90nA9xULKOnbPmB/El/BokigvpQSQnFHvt53/w==
-----END CERTIFICATE-----
)KEY";

char SSID[] = "WiseFi";
const char *PWD = "sucksassfool";
 

//AWS_IOT hornbill;
 
void connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.println(SSID);
  WiFi.begin(SSID, PWD); 
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    //we can even make the ESP32 to sleep
  }
 
  Serial.print("Connected - ");
 
}

void callback(char* topic,byte* payload,unsigned int length1){    
Serial.print("message arrived[");
Serial.print(topic);
Serial.println("]");

for(int i=0;i<length1;i++){
  Serial.println("Payload Value");
  Serial.println(payload[i]); 
  
}
if(payload[0]==1) {
  digitalWrite(motorPin,HIGH);    //ASCII VALUE OF '1' IS 49
}
else if (payload[0]==2) {
  digitalWrite(motorPin,LOW);//ASCII VALUE OF '2' IS 50
}

Serial.println();
}

 void connectToAWS() {
  mqttClient.setServer("a3jjkbw4vqw4wh-ats.iot.us-east-1.amazonaws.com", 8883);
  mqttClient.setCallback(callback);
  secureClient.setCACert(AWS_PUBLIC_CERT);
  secureClient.setCertificate(AWS_DEVICE_CERT);
  secureClient.setPrivateKey(AWS_PRIVATE_KEY);
 
  Serial.println("Connecting to MQTT....");
 
  mqttClient.connect("Irrigation");
  // mqttClient.connect("Irrigation","shubham93dhupar@gmail.com","davpublic74824");
  Serial.println(mqttClient.state());
  while (!mqttClient.connected()) {
    Serial.println(mqttClient.state());
    Serial.println("Connecting to MQTT....Retry");
    mqttClient.connect("Irrigation");
    //mqttClient.connect("Irrigation","shubham93dhupar@gmail.com","davpublic74824");
    delay(5000);
  } 
  Serial.println(mqttClient.state());
 
  Serial.println("MQTT Connected");
}
/*void connectToAWS() {
if(hornbill.connect(HOST_ADDRESS,CLIENT_ID)== 0) // Connect to AWS using Host Address and Cliend ID
    {
        Serial.println("Connected to AWS");
        delay(1000);
    }
    else
    {
        Serial.println("AWS connection failed, Check the HOST Address");
        while(1);
    }
}*/


void loop (){
  moisture = analogRead(moistPin);
  //Serial.println(moisture);
  water = analogRead(waterPin);
  //Serial.println(water);
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  // Compute the hit Index using celsius
  float heatIndex = dht.computeHeatIndex(temperature, humidity, false);

 
  Serial.print("Temperature:");
  Serial.println(temperature);
  Serial.print("Humidity:");
  Serial.println(humidity);
  Serial.print("Heat Index:");
  Serial.println(heatIndex);
  Serial.print("Soil Moisture:");
  Serial.println(moisture);
  Serial.print("Soil water content:");
  Serial.println(water);
  
  StaticJsonDocument<128> jsonDoc;
  JsonObject eventDoc = jsonDoc.createNestedObject("event");
  eventDoc["temp"] = temperature;
  eventDoc["hum"] = humidity;
  //eventDoc["hi"] = heatIndex;
  eventDoc["moist"] = moisture;
  eventDoc["water"] = water;
  char jsonBuffer[128];
 
  serializeJson(eventDoc, jsonBuffer);
  mqttClient.publish("Irrigation", jsonBuffer);
  delay(1000);
  if(mqttClient.connect("Irrigation")) {
    mqttClient.subscribe("Result");
    Serial.println(mqttClient.subscribe("Result"));
   // mqttClient.loop();
    delay(1000);
    }
  if (temperature > 10 && humidity > 150 && moisture < 2000 &&  water < 2000){
    Serial.println("start motor");
    for (int i=0;i<10;i++){
    digitalWrite(motorPin,HIGH); //ASCII VALUE OF '1' IS 49
    delay(1000);
    digitalWrite(motorPin,LOW);
    delay(1000);
    }
    delay(60000);
}
else if (temperature > 10 && humidity > 150 && moisture > 2000 &&  water < 2000) {
  Serial.println("stop motor");
  digitalWrite(motorPin,LOW);//ASCII VALUE OF '2' IS 50
  delay(1000);
  }
else if (temperature > 10 && humidity > 150 && moisture < 2000 &&  water > 2000) {
  Serial.println("stop motor");
  digitalWrite(motorPin,LOW);//ASCII VALUE OF '2' IS 50
  delay(1000);
  }
}
  




  
void setup() {
  pinMode(motorPin, OUTPUT);
  pinMode(moistPin, INPUT);
  pinMode(waterPin, INPUT);
  Serial.begin(115200);
  connectToWiFi();
  connectToAWS();
   
}
