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
const char* password = "12345678";
// Domain Name with full URL Path for HTTP POST Request
const char* serverName = "http://api.thingspeak.com/update";
// Service API Key
String apiKey = "9885BM14TX2FPMUU";


void setup() {
  Serial.begin(9600);

  delay(1000);

  while (Serial.read() == 0) {
  }

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.write(123);
  }
}

void loop() {

  if (Serial.read() == 0) {

  } else {

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
        String httpRequestData = "";
        if (recived >= 32) {
          httpRequestData = "api_key=" + apiKey + "&field4=" + recived;
        } else {
          httpRequestData = "api_key=" + apiKey + "&field5=" + recived;
        }

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
  }
}