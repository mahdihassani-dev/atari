/*
  Rui Santos
  Complete project details at Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-http-post-ifttt-thingspeak-arduino/

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
  
  Code compatible with ESP8266 Boards Version 3.0.0 or above 
  (see in Tools > Boards > Boards Manager > ESP8266)
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char* ssid = "EspTest";
const char* password = "";
// Domain Name with full URL Path for HTTP POST Request
const char* serverName = "http://api.thingspeak.com/update";
// Service API Key
String apiKey = "9885BM14TX2FPMUU";

// THE DEFAULT TIMER IS SET TO 10 SECONDS FOR TESTING PURPOSES
// For a final application, check the API call limits per hour/minute to avoid getting blocked/banned
unsigned long lastTime = 0;
// Set timer to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Timer set to 10 seconds (10000)
unsigned long timerDelay = 2000;

void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.write(123);
  }
  // Random seed is a number used to initialize a pseudorandom number generator
  randomSeed(analogRead(0));
}

void loop() {


  //Send an HTTP POST request every 10 seconds
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {

      Serial.write(200);

      WiFiClient client;
      HTTPClient http;

      // Your Domain name with URL path or IP address with path


      int recived = 0;

      if (Serial.available()) {

        http.begin(client, serverName);

        recived = Serial.read();

        // Specify content-type header
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        // Data to send with HTTP POST
        String httpRequestData = "api_key=" + apiKey + "&field3=" + recived;
        // Send HTTP POST request
        int httpResponseCode = http.POST(httpRequestData);

        http.end();


      }
      /*
      // If you need an HTTP request with a content type: application/json, use the following:
      http.addHeader("Content-Type", "application/json");
      // JSON data to send with HTTP POST
      String httpRequestData = "{\"api_key\":\"" + apiKey + "\",\"field1\":\"" + String(random(40)) + "\"}";           
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);*/

      // Serial.print("HTTP Response code: ");
      // Serial.println(httpResponseCode);

      // Free resources

    } else {
      Serial.write(403);
    }
    lastTime = millis();
  }
}