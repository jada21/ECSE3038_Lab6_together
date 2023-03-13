/* 620141014
JADA CAMPBELL
ECSE3038 LAB4  */
#include <Arduino.h>
#include <Arduino.h>
#include <Wifi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "env.h"
#include <cstdlib>
#include <iostream>
#include <string>
#define fanPin 22
#define lightPin 23


void setup() 
{
  pinMode (fanPin, OUTPUT);
  pinMode (lightPin, OUTPUT);
  Serial.begin(9600);
  WiFi.begin(WIFI_USER, WIFI_PASS);
  Serial.println("Connecting");

  while(WiFi.status() != WL_CONNECTED)    
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}
  
  
void loop() 
{
  if(WiFi.status()== WL_CONNECTED)
  {
    String endpoint = "http://127.0.0.1:8000/api/temperature";
    HTTPClient http;  
    http.begin(endpoint);

    // Specify headers
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Content-length", "23");

    StaticJsonDocument<1024> doc;               // Empty JSONDocument
    String httpRequestData;                   //Emtpy string to be used to store HTTP request data string
    
    srand (static_cast <unsigned> (time(0)));
    float randNum = 21 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(33-21)));
    doc["temperature"] = randNum;
    Serial.print("temp: ");
    Serial.println(randNum);
    Serial.println();

    serializeJson(doc, httpRequestData);
    int httpResponseCode = http.PUT(httpRequestData);   //PUT REQUEST, sends temp
    String http_response;
    Serial.println("put success");
    if (httpResponseCode>0) 
    {
      Serial.print("HTTP Response code from request: ");
      Serial.println(httpResponseCode);

      Serial.print("HTTP Response from server: ");
      http_response = http.getString();
      Serial.println(http_response);
      Serial.println(" ");
    }
    else                                      //if http response code is negative
    {
      Serial.print("Error code: ");
      http_response = http.getString();
      Serial.println(httpResponseCode);
    }
    http.end();

    //GET PART:
    String endpoint2 = "http://127.0.0.1:8000/api/state";
    
    http.begin(endpoint2);       //start connection to api url
    httpResponseCode = http.GET();      //performs get request and receives status code response
    
    if (httpResponseCode>0) 
    {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      Serial.print("Response from server: ");
      http_response = http.getString();       //gets worded/verbose response
      Serial.println(http_response);
    }
    else 
    {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
    
    StaticJsonDocument<1024> doc1;            //document to store deserialized json
    DeserializationError error = deserializeJson(doc1, http_response);

    if (error) 
    {
      Serial.print("Could not deserialize json");
      Serial.println(error.c_str());
      return;
    }
    
    const bool fanState = doc1["fan"];
    const bool lightState = doc1["light"]; 
    Serial.println("data:");
    Serial.print("fan state:   ");
    Serial.println(fanState);
    Serial.print("light state:  ");
    Serial.println(lightState);

    digitalWrite (fanPin, fanState);
    digitalWrite(lightPin, lightState);
    http.end();
    
  }
  else 
  {
    Serial.println("WiFi Disconnected");
  }
  delay(10000);

}

