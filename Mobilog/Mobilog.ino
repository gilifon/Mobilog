#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#define MAX_SIZE 27*47

//const char* ssid = "4Z1KD-S";
const char* ssid = "Gil";
const char* password = "";

unsigned long previousMillis = 0; // required for delay of CAT command
const long interval = 1000; // delay interval (milliseconds)

SoftwareSerial radioSerial(D2, -1, true, MAX_SIZE);

boolean stringComplete = false; // A flag that indicate a command has arrived through the serial port
String inputString = ""; //The raw string of the command
String Frequency = ""; //The frequency after parsing

WiFiServer server(80); //The local web server

void setup() {
  //start radio serial
  //radioSerial.begin(9600);

  //start monitor serial
  Serial.begin(115200);
  while (!Serial) {}

  // Connect to WiFi network
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }

  // Start the server
  server.begin();

  // Print the IP address
  Serial.print(WiFi.localIP());
}

void loop() {
  SendCommandToRadio("FA;");
  CheckForData();
  if (stringComplete)
  {
    Frequency = inputString;
    inputString = ""; //reset the input string
    stringComplete = false; //reset the complete flag
  }

  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  while (!client.available()) {
    delay(1);
  }

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  client.flush();

  // Match the request
  if (request.indexOf("/cmd=fa") != -1)  {
    // Return the response
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println(""); //  do not forget this one
    Frequency.replace("FA","");
    Frequency.replace(";","");
    client.print(Frequency);
    delay(1);
  }
  else
  {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/json");
    client.println(""); //  do not forget this one
    client.print("error");
  }
}

void SendCommandToRadio(String command)
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    //radioSerial.print(command);
    Serial.print(command);
    delay(250);
  }
}

void CheckForData() {
  //  while (radioSerial.available()) {
  //    char inChar = (char)radioSerial.read();
  //    inputString += inChar;
  //    if (inChar == ';') {
  //      inputString.trim(); //trim the string
  //      stringComplete = true;
  //    }
  //  }
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == ';') {
      inputString.trim(); //trim the string
      stringComplete = true;
    }
  }
}
