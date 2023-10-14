#include <secrets.h>
#include <DHT.h>
#include <DHT_U.h>
#include <DHTesp.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Ticker.h>


#define WATER A0
#define SUN_RELAY 13    // D7
#define WIND_RELAY 15 // D8


Ticker timer;
Ticker timedRelayControl;
tm timeinfo;

WiFiClientSecure net;
WiFiUDP ntpUDP;
ESP8266WebServer server(80);
PubSubClient client(net);

NTPClient timeClient(ntpUDP, NTP_SERVER, TIME_ZONE * 3600, DAYLIGHT_OFFSET_SEC);

DHT_Unified dht(14, DHT11); // D5


BearSSL::X509List cert(cacert);
BearSSL::X509List client_crt(client_cert);
BearSSL::PrivateKey key(privkey);


unsigned long lastMillis = 0;
const long interval = 5000;
bool relayState = LOW;


void controlRelay(bool ligar, int relayPin) {
  digitalWrite(relayPin, ligar ? LOW : HIGH);
  relayState = ligar;
}


void toggleRelay(int relayPin) {
  controlRelay(!digitalRead(relayPin), relayPin);
}


void handleWindOn() {
  controlRelay(true, WIND_RELAY);
  server.send(200, "text/plain", "Wind Relay Status: " + WIND_RELAY);
}


void handleWindOff() {
  controlRelay(false, WIND_RELAY);
  server.send(200, "text/plain", "Wind Relay Status: " + WIND_RELAY);
}


void handleSunOn() {
  controlRelay(true, SUN_RELAY);
  server.send(200, "text/plain", "Sun Relay Status: " + SUN_RELAY);
}


void handleSunOff() {
  controlRelay(false, SUN_RELAY);
  server.send(200, "text/plain", "Sun Relay Status: " + SUN_RELAY);
}


void updateTime() {
  timeClient.update();
  time_t now = timeClient.getEpochTime();
  gmtime_r(&now, &timeinfo);
}


void setupTimedRelayControl() {
  String timeOn = "06:30";
  String timeOff = "18:30";

  pinMode(SUN_RELAY, OUTPUT);

  timedRelayControl.attach(420, [timeOn, timeOff]() {
    timeClient.update();
    String currentTime = timeClient.getFormattedTime();

    if (strcmp(currentTime.c_str(), timeOn.c_str()) == 0) {
      controlRelay(true, SUN_RELAY);

    } else if (strcmp(currentTime.c_str(), timeOff.c_str()) == 0) {
      controlRelay(false, SUN_RELAY); 
    }
  });
}


void handleRoot() {

  bool windRelayState = digitalRead(WIND_RELAY) == HIGH;
  bool sunRelayState = digitalRead(SUN_RELAY) == HIGH;

  String response = "Device Status:\n";
  response += "Wind Relay: " + String(windRelayState ? "ON" : "OFF") + "\n";
  response += "Sun Relay: " + String(sunRelayState ? "ON" : "OFF") + "\n";

  server.send(200, "text/plain", response);
}


void getDHT11() {
  char jsonBuffer[512];
  char dateStr[20];
  char timeStr[20];

  StaticJsonDocument<200> json;

  sensors_event_t temperature_float;
  sensors_event_t humidity_float;

  timeClient.update();

  updateTime();

  strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", &timeinfo);
  strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);

  dht.temperature().getEvent(&temperature_float);
  dht.humidity().getEvent(&humidity_float);

  json["humidity"] = humidity_float.relative_humidity;
  json["temperature"] = temperature_float.temperature;
  json["date"] = dateStr;
  json["time"] = timeStr;

  serializeJson(json, jsonBuffer);

  server.send(200, "application/json", jsonBuffer);
}


void handleNotFound() {
  String message = "File Not Found\n";
  server.send(404, "text/plain", message);
}


void NTPConnect(void) {
  Serial.println("Setting time using SNTP");

  while (!timeClient.update()) {
    delay(1000);
    Serial.print(".");
  }

  time_t now = timeClient.getEpochTime();
  gmtime_r(&now, &timeinfo);

  Serial.println("done!");
  Serial.print("Current time: ");
  Serial.println(asctime(&timeinfo));
}


void messageReceived(char* topic, byte* payload, unsigned int length) {
  Serial.println("Received [");
  Serial.print(topic);
  Serial.print("]: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}


void handleRelayControl(bool windRelay, bool sunRelay) {
  digitalWrite(WIND_RELAY, windRelay ? HIGH : LOW);
  digitalWrite(SUN_RELAY, sunRelay ? HIGH : LOW);
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");

  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  Serial.println();

  StaticJsonDocument<200> doc;

  DeserializationError error = deserializeJson(doc, payload, length);

  if (error == DeserializationError::Ok) {
    Serial.println("Valid JSON received:");

    JsonObject obj = doc.as<JsonObject>();

    if (obj.containsKey("message") && obj.containsKey("wind_relay") && obj.containsKey("sun_relay")) {
      const char* message = obj["message"];
      bool windRelay = obj["wind_relay"];
      bool sunRelay = obj["sun_relay"];

      Serial.print("Message: ");
      Serial.println(message);
      Serial.print("Wind Relay: ");
      Serial.println(windRelay);
      Serial.print("Sun Relay: ");
      Serial.println(sunRelay);

      handleRelayControl(windRelay, sunRelay);
    } else {
      Serial.println("Invalid JSON format. Missing required keys.");
    }
  } else {
    Serial.println("Error deserializing JSON. Invalid payload.");
  }
}


void connectAWS() {
  delay(3000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println(String("Attempting to connect to SSID: ") + String(WIFI_SSID));

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("");
  NTPConnect();

  net.setTrustAnchors(&cert);
  net.setClientRSACert(&client_crt, &key);

  client.setServer(MQTT_HOST, 8883);
  client.setCallback(messageReceived);
  client.setCallback(callback);

  Serial.println("Connecting to AWS IOT");

  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(1000);
  }

  if (!client.connected()) {
    Serial.println("AWS IoT Timeout!");
    return;
  }

  client.subscribe(pytinhaSub);
  client.subscribe(mqttTopic);

  Serial.println("AWS IoT Connected!");
}


void connectWIFI() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  // Routes
  server.on("/", handleRoot);
  server.on("/dht11", getDHT11);
  server.on("/wind-on", HTTP_GET, handleWindOn);
  server.on("/wind-off", HTTP_GET, handleWindOff);
  server.on("/sun-on", HTTP_GET, handleSunOn);
  server.on("/sun-off", HTTP_GET, handleSunOff);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}


void publishMessage() {
  StaticJsonDocument<200> json;
  char jsonBuffer[512];
  char dateStr[20];
  char timeStr[20];

  timeClient.update();
  updateTime();
  strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", &timeinfo);
  strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);

  sensors_event_t temperature_float;
  sensors_event_t humidity_float;

  dht.temperature().getEvent(&temperature_float);
  dht.humidity().getEvent(&humidity_float);

  json["humidity"] = humidity_float.relative_humidity;
  json["temperature"] = temperature_float.temperature;
  json["date"] = dateStr;
  json["time"] = timeStr;

  serializeJson(json, jsonBuffer);

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}


void setup(void) {
  Serial.begin(115200);
  connectWIFI();
  configTime(TIME_ZONE * 3600, 0 * 3600, NTP_SERVER, "time.nist.gov");
  connectAWS();
  dht.begin();
  setupTimedRelayControl();
}


void loop(void) {
  updateTime();
  server.handleClient();
  delay(2000);

  if (!client.connected()) {
    connectAWS();
  } else {
    client.loop();
    if (millis() - lastMillis > 5000) {
      lastMillis = millis();
      publishMessage();
    }
  }
}
