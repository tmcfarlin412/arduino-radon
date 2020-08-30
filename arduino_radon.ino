#include "Conf.h"
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
  
// Set these to run example. 
#define FIREBASE_HOST "arduino-radon.firebaseio.com" 
#define FIREBASE_AUTH "lUJTEKcfWyqEswj17UdQPPrh7bygbbFxXjqkuVtu" 
//#define WIFI_SSID "baby-bungalo"
//#define WIFI_PASSWORD "thecactus!snottrash"
#define DEEP_SLEEP_TIME 21600e6  // 6 hours in microseconds

const int INPUT_PIEZO = A0;
const String FIREBASE_RTDB_PATH_PIEZO= "piezo-1";

const long utcOffsetInSeconds = 0;  // Chicago UTC/GMT -6 hours
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

int sensorReading;

void setup() { 
  Serial.begin(9600); 
  
  // connect to wifi. 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); 
  Serial.print("connecting"); 
  while (WiFi.status() != WL_CONNECTED) { 
    Serial.print("."); 
    delay(500); 
  } 
  Serial.println(); 
  Serial.print("connected: "); 
  Serial.println(WiFi.localIP()); 
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  timeClient.begin();
} 

void loop() { 
  timeClient.update();

  String m = Firebase.getString("/");
  sensorReading = analogRead(INPUT_PIEZO);
  String readingPath = FIREBASE_RTDB_PATH_PIEZO + "/" + timeClient.getEpochTime();
  
  Serial.println("Setting " + readingPath + " to " + sensorReading);  
  Firebase.setFloat(readingPath, sensorReading);
  
  // handle error 
  if (Firebase.failed()) { 
    Serial.println("Setting " + FIREBASE_RTDB_PATH_PIEZO + " failed"); 
    Serial.println(Firebase.error());   
  }

  Serial.println("Sleeping...");
  delay(30000);
//  ESP.deepSleep(DEEP_SLEEP_TIME);
} 
