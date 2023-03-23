#include <WiFiNINA.h>
#include <Adafruit_Protomatter.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeMono9pt7b.h>
#include "PubSubClient.h"


uint8_t rgbPins[] = {7, 8, 9, 10, 11, 12};
uint8_t addrPins[] = {17, 18, 19, 20};
uint8_t clockPin = 14;
uint8_t latchPin = 15;
uint8_t oePin = 16;
Adafruit_Protomatter matrix(
64, 4, 1, rgbPins, 4, addrPins, clockPin, latchPin, oePin, false);

char* ssid = "SSID";
const char* password = "WiFi-PASS";
const char* mqtt_server = "192.168.8.213";
WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
long lastMsg2 = 0;
char msg[50];
int value = 0;
float result[3]={0};
int sending;
String temperature;
String humidity;
String co2;
String presence;
String date;
String weather;
String reasoning;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  ProtomatterStatus status = matrix.begin();
  Serial.print("Protomatter begin() status: "); 
  Serial.println((int)status);
  if(status != PROTOMATTER_OK) {
  for(;;);
    } 
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length){
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  matrix.fillScreen(0);
  matrix.show();
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
  if (String(topic) == "esp32/co2") {
    co2 = messageTemp;
  }
  else if(String(topic) == "esp32/temperature"){
    temperature = messageTemp;
  }
  else if(String(topic) == "esp32/humidity"){
    humidity = messageTemp;
  }
  else if(String(topic) == "esp32/presence"){
    presence = messageTemp;
    }
  else if(String(topic) == "esp32/weather"){
    weather = messageTemp;
  } 
  else if(String(topic) == "esp32/date"){
    date = messageTemp;
  }
  else if(String(topic) == "java/output"){
    reasoning = messageTemp;
  }
//  if(presence == "Detected"){
//  matrix.fillScreen(0);
//  matrix.setCursor(0,5);
//  matrix.print("Co2:");
//  matrix.println(co2);
//  matrix.setCursor(0,16);
//  matrix.print("Tem:");
//  matrix.println(temperature);
//  matrix.setCursor(0,24);
//  matrix.print("Hum:");
//  matrix.println(humidity);
//  matrix.show();
//
//  delay(5000);
//  matrix.fillScreen(0);
//  matrix.setCursor(0,12);
//  matrix.println(date);
//  matrix.setCursor(0,24);
//  matrix.print("WX:");
//  matrix.println(weather);
//  matrix.show();
//  delay(5000);
//  if(reasoning == "UNCOMFORTABLE"){
//    matrix.fillScreen(0);
//    matrix.setCursor(0,12);
//    matrix.println("WARNING...");
//    matrix.show();
//    delay(5000); 
//  }
//  }
//  else{
//    matrix.fillScreen(0);
//    matrix.show();
//  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client1")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/presence");
      client.subscribe("esp32/date");
      client.subscribe("esp32/weather");
      client.subscribe("esp32/co2");
      client.subscribe("esp32/temperature");
      client.subscribe("esp32/humidity");
      client.subscribe("java/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  if(now - lastMsg >5000){
    if(reasoning == "UNCOMFORTABLE"){
       matrix.fillScreen(0);
       matrix.setCursor(0,12);
       matrix.println("WARNING...");
       matrix.show();
    }
    else{
      matrix.fillScreen(0);
      matrix.setCursor(0,5);
      matrix.print("Co2:");
      matrix.println(co2);
      matrix.setCursor(0,16);
      matrix.print("Tem:");
      matrix.println(temperature);
      matrix.setCursor(0,24);
      matrix.print("Hum:");
      matrix.println(humidity);
      if(presence == "Detected"){
      matrix.show();
      }
    }
    lastMsg = now;
  }
  if(now - lastMsg2 > 10000){
    if(reasoning == "UNCOMFORTABLE"){
       matrix.fillScreen(0);
       matrix.setCursor(0,12);
       matrix.println("WARNING...");
       matrix.show();
    }
    else{
      matrix.fillScreen(0);
      matrix.setCursor(0,12);
      matrix.println(date);
      matrix.setCursor(0,24);
      matrix.print("WX:");
      matrix.println(weather);
      if(presence == "Detected"){
      matrix.show();
      }
    }
    lastMsg = now;
    lastMsg2 = now;
  }
 }
