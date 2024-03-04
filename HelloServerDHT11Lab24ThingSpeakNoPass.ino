// This code is derived from the HelloServer Example
// in the (ESP32) WebServer library .
//
// It hosts a webpage which has one temperature reading to display.
// The webpage is always the same apart from the reading which would change.
// The getTemp() function simulates getting a temperature reading.
// homePage.h contains 2 constant string literals which is the two parts of the
// webpage that never change.
// handleRoot() builds up the webpage by adding as a C++ String:
// homePagePart1 + getTemp() +homePagePart2
// It then serves the webpage with the command:
// server.send(200, "text/html", message);
// Note the text is served as html.
//
// Replace the code in the homepage.h file with your own website HTML code.
//
// This example requires only an ESP32 and download cable. No other hardware is reuired.
// A wifi SSID and password is required.
// Written by: Natasha Rohan  12/3/23
//
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "homepage.h"
#include "secrets.h"
#include "ThingSpeak.h"  // always include thingspeak header file after other header files and custom macros

#include <DFRobot_DHT11.h>
#define TRIG_PIN 26            // ESP32 pin GPIO26 connected to Ultrasonic Sensor's TRIG pin
#define ECHO_PIN 25            // ESP32 pin GPIO25 connected to Ultrasonic Sensor's ECHO pin
DFRobot_DHT11 DHT;
#define DHT11_PIN 19
#define REPORTING_PERIOD_MS 20000  // report to thingspeak every 20s
WiFiClient client;

const char* ssid = "Rory";
const char* pass = "12345678";
unsigned long myChannelNumber = SECRET_CH_ID;
const char* myWriteAPIKey = SECRET_WRITE_APIKEY;

// Initialize our values
uint32_t tsLastReport = 0;

int number1 = 0;
String myStatus = "";
WebServer server(80);

//temp function to simulate temp sensor
//String getTemp() {
//  return "25.4";
//}
float temp = 0;
String getTemp() {
  DHT.read(DHT11_PIN);
  temp = DHT.temperature;

  return String(temp);
}


void handleRoot() {
  String message = homePagePart1 + getTemp() + homePagePart2;
  server.send(200, "text/html", message);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
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

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  ThingSpeak.begin(client);  // Initialize ThingSpeak
   pinMode(TRIG_PIN, OUTPUT);    // set ESP32 pin to output mode
  pinMode(ECHO_PIN, INPUT);     // set ESP32 pin to input mode
  delay(1000);
}

void loop(void) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }
  DHT.read(DHT11_PIN);
  Serial.print("temp:");
  temp = DHT.temperature;
  Serial.print(temp);
  // set the fields with the values
  ThingSpeak.setField(1, number1);
  ThingSpeak.setField(2, temp);

  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {

    if (temp > 25) {
      myStatus = String("Temperature is too high");
    } else if (temp < 15) {
      myStatus = String("Temperature is too low");
    } else {
      myStatus = String("normal tempreature");
    }
    digitalWrite(TRIG_PIN, LOW);
            delayMicroseconds(5);
            digitalWrite(TRIG_PIN, HIGH);
            delayMicroseconds(10);
            digitalWrite(TRIG_PIN, LOW);
            // measure duration of pulse from ECHO pin
            duration = pulseIn(ECHO_PIN, HIGH);
            // calculate the distance
            cms = (duration / 2) / 29.1;
            inches = (duration / 2) / 74;
            // print the value to Serial Monitor
            Serial.print(cms);
            Serial.print("cms");
            Serial.println();
            delay(250);
           

    // set the status
    ThingSpeak.setStatus(myStatus);

    // write to the ThingSpeak channel
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if (x == 200) {
      Serial.println("Channel update successful.");
    } else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    // change the values
    number1++;
    if (number1 > 99) {
      number1 = 0;
    }
  }
  tsLastReport = millis();  //update the time stamp
  server.handleClient();
  delay(2);  //allow the cpu to switch to other tasks
}
