#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "DHTesp.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <LCD_I2C.h>

LCD_I2C lcd(0x27, 16, 2); // Default address of most PCF8574 modules, change according
DHTesp dht;

const char* ssid = "Oi_9B63";
const char* password = "5G3c7T2a";

ESP8266WebServer server(80);


void handleRoot() {
  String umidade = String(dht.getHumidity());
  String temperatura = String(dht.getTemperature());
 
  server.send(200, "text/plain", umidade+"e"+temperatura);  // 70.0e23.0
}

void handleNotFound(){
  String message = "File Not Found\n";
  server.send(404, "text/plain", message);
}

void setup(void){
  dht.setup(14, DHTesp::DHT11); // D5
  lcd.begin();
  lcd.backlight();
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
  delay(1500);
  float u = dht.getHumidity();
  float t = dht.getTemperature();
  String texto_display = " " + String(u) + " | " + String(t) + " ";
  Serial.print("Umidade (%): ");
  Serial.print(u, 1);
  Serial.print("\t");
  Serial.print("Temperatura (C): ");
  Serial.print(t, 1);
  Serial.print("\t");
  Serial.println();
  lcd.print(" UMIDA | TEMPE "); // You can make spaces using well... spaces
  lcd.setCursor(0,1); // Or setting the cursor in the desired position.
  lcd.print(texto_display);
  delay(500);
  // lcd.clear();
}