#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "DHTesp.h"
#include <LCD_I2C.h>

#ifndef STASSID
#define STASSID "Oi_9B63"
#define STAPSK "5G3c7T2a"
#endif

#define water A0

const char* ssid = STASSID;
const char* password = STAPSK;

const int waterpumpsensor = 0;  // D3
const int dhtesp = 14;          // D5
const int sun = 2;              // D4

DHTesp dht;
LCD_I2C lcd(0x27, 16, 2);       // D1 - D2
ESP8266WebServer server(80);    // 192.168.100.21 


void handleNotFound(){
  String message = "Endpoint Not Allowed\n";
  server.send(404, "text/plain", message);
}

void handleRoot() {
  String textWaterPump = "\"WaterPump\" : " + String(digitalRead(waterpumpsensor));
  String textTemperature = "\"Temperature\" : " + String(dht.getTemperature()) + ",";
  String textHumidity = "\"humidity\" : " + String(dht.getHumidity()) + ",";
  String textSun = "\"Sun\" : " + String(digitalRead(sun)) + ",";
  String textJson = "{" + textTemperature + textHumidity + textSun + textWaterPump + "}";
  server.send(200, "application/json", textJson);
}

void waterPumpOn() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
  } else {
    digitalWrite(waterpumpsensor, LOW);
    server.send(200, "application/json", " Pump: " + String(digitalRead(waterpumpsensor)));
    Serial.println(digitalRead(waterpumpsensor));
  }
}

void waterPumpOff() {
  if (server.method() != HTTP_POST) {
    server.send(405, "application/json", "{\"status_code\" : 405, \"res\" : \"Method Not Allowed\" }");
  } else {
    digitalWrite(waterpumpsensor, HIGH);
    server.send(200, "application/json", " Pump: " + String(digitalRead(waterpumpsensor)));
    Serial.println(digitalRead(waterpumpsensor));
  }
}

void sunOn() {
  if (server.method() != HTTP_POST) {
    server.send(405, "application/json", "{\"status_code\" : 405, \"res\" : \"Method Not Allowed\" }");
  } else {
    digitalWrite(sun, LOW);
    server.send(200, "application/json", " Pump: " + String(digitalRead(sun)));
    Serial.println(digitalRead(sun));
  }
}

void sunOff() {
  if (server.method() != HTTP_POST) {
    server.send(405, "application/json", "{\"status_code\" : 405, \"res\" : \"Method Not Allowed\" }");
  } else {
    digitalWrite(sun, HIGH);
    server.send(200, "application/json", " Pump: " + String(digitalRead(sun)));
    Serial.println(digitalRead(sun));
  }
}

void setup(void){
  pinMode(waterpumpsensor, OUTPUT);
  pinMode(sun, OUTPUT);

  dht.setup(dhtesp, DHTesp::DHT11);
  Serial.begin(115200);
  lcd.begin();
  lcd.backlight();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

  }

  // Serial.println("Connected to : " + String(ssid) + "\n" +"IP address : " + WiFi.localIP());
  Serial.println("Connected to : " + String(ssid));
  Serial.print("IP address : ");
  Serial.println(WiFi.localIP());
  // lcd.print(ssid);
  // lcd.setCursor(0,1);
  // lcd.print(WiFi.localIP());
  // lcd.clear();

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
  
  server.on("/", handleRoot);
  server.on("/pump_on/", waterPumpOn);
  server.on("/pump_off/", waterPumpOff);
  server.on("/sun_on/", sunOn);
  server.on("/sun_off", sunOff);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
  float u = dht.getHumidity();
  float t = dht.getTemperature();
  int waterString = analogRead(water);

  String waterCalc = String(waterString);

  if (waterString > 0 and waterString < 10) {
    // return "000" + String(valor);
    waterCalc = String("000" + waterString);
  }

  if (waterString > 9 and waterString < 100) {
    waterCalc = String("00" +waterString);
  }

  if (waterString > 99 and waterString < 1000) {
    waterCalc = String("0" + waterString);
  }

  if (waterString > 999 and waterString < 10000) {
    waterCalc = String(waterString);
  }


  String texto_display = String(u) + "|" + String(t) + "|" + String(waterString);
  Serial.print("Umidade (%): ");
  Serial.print(u, 1);
  Serial.print("\t");
  Serial.print("Temperatura (C): ");
  Serial.print(t, 1);
  Serial.print("\t");
  Serial.print("Agua: ");
  Serial.print(analogRead(water));
  Serial.print("\t");
  Serial.println();
  lcd.print("UMIDA|TEMPE|H2O"); // You can make spaces using well... spaces
  lcd.setCursor(0,2); // Or setting the cursor in the desired position.
  lcd.print(texto_display);
  delay(2000);
}