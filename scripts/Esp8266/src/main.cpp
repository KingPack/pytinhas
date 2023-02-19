#include <DHT.h>
#include <DHT_U.h>
#include "DHTesp.h"
#include <Arduino.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <Adafruit_Sensor.h>
#include <secrets.h>

#include <WiFiClientSecure.h>
#include <PubSubClient.h>

#define THINGNAME "esp8266_pytinhas"
#define WATER A0
#define AWS_IOT_PUBLISH_TOPIC   "esp8266_pytinhas/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp8266_pytinhas/sub"


WiFiClientSecure net;
DHT_Unified dht(2, DHT11);
ESP8266WebServer server(80);

BearSSL::X509List cert(cacert);
BearSSL::X509List client_crt(client_cert);
BearSSL::PrivateKey key(private_key);

PubSubClient client(net);

void handleRoot() {
  char jsonBuffer[512];
  StaticJsonDocument<200> json;
  sensors_event_t temperature_float;
  sensors_event_t humidity_float;
  dht.temperature().getEvent(&temperature_float);
  dht.humidity().getEvent(&humidity_float);
  json["humidity"] = humidity_float.relative_humidity;
  json["temperature"] = temperature_float.temperature;
  // String json = "{ \"temperature\": " + String(temperature_float.temperature) + ", \"humidity\": " + String(humidity_float.relative_humidity) + ", \"water\": \"" + WATER + "\" }";
  serializeJson(json, jsonBuffer);
  
  server.send(200, "application/json", jsonBuffer);  // 70.0e23.0
}


void handleNotFound(){
  String message = "File Not Found\n";
  server.send(404, "text/plain", message);
}


void messageReceived(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Received [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void connectAWS(){
 
  net.setTrustAnchors(&cert);
  net.setClientRSACert(&client_crt, &key);
 
  client.setServer(MQTT_HOST, 8883);
  client.setCallback(messageReceived);
 
  Serial.println("Connecting to AWS IOT");
 
  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(1000);
  }
 
  if (!client.connected()) {
    Serial.println("AWS IoT Timeout!");
    return;
  }
  
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
  Serial.println("AWS IoT Connected!");
}


void connectWIFI(){

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
  
  // Urls 
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}


void setup(void){
  Serial.begin(115200);
  dht.begin();
  connectWIFI();
  
}


void loop(void){
  server.handleClient();
  sensors_event_t temperature_float;
  sensors_event_t humidity_float;
  dht.temperature().getEvent(&temperature_float);
  dht.humidity().getEvent(&humidity_float);
  
  String texto_display = " " + String(humidity_float.relative_humidity) + " | " + String(temperature_float.temperature) + " ";
  
  Serial.print("Humidity (%): ");
  Serial.print(humidity_float.relative_humidity, 2);
  Serial.print("\t");
  Serial.print("Temperature (C): ");
  Serial.print(temperature_float.temperature, 2);
  Serial.print("\t");
  Serial.print("Water: ");
  Serial.print(analogRead(WATER));
  Serial.print("\t");
  Serial.println();
  delay(2000);
}