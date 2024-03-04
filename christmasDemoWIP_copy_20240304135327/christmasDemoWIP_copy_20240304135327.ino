/*
 * This ESP32 code is for my smart home system
 *
 * 
 */
 #include <Adafruit_Fingerprint.h>  //fingerprint
 #define mySerial Serial2 //fingerprint
#include <Wire.h>              //lcd
#include "rgb_lcd.h"           // lcd
#define TRIG_PIN 26            // ESP32 pin GPIO26 connected to Ultrasonic Sensor's TRIG pin
#define ECHO_PIN 25            // ESP32 pin GPIO25 connected to Ultrasonic Sensor's ECHO pin
#define BUZZER_PIN 23          // ESP32 pin GPIO17 connected to Piezo Buzzer's pin
#define DISTANCE_THRESHOLD 50  // centimeters
#include <DFRobot_DHT11.h>
DFRobot_DHT11 DHT;
#define DHT11_PIN 19
#include <Servo.h>
static const int SERVO_PIN = 13;  //not changing during execution of code
rgb_lcd lcd;
const int colorR = 255;  //lcd
const int colorG = 0;    //lcd
//const int TRIG_PIN = 26;
//const int ECHO_PIN = 25;
const int colorB = 0;  //lcd


//variables will change:
long duration;  // at least 32 bits
float cms, inches, distance; // floating decimal point

Servo servo1;
//----------------------------------------------------
//keypad
#include <Keypad.h>
#define ROW_NUM 4     //four rows
#define COLUMN_NUM 3  //three columns

char keys[ROW_NUM][COLUMN_NUM] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' },
};
byte pin_rows[ROW_NUM] = { 18, 5, 14, 27 };
byte pin_column[COLUMN_NUM] = { 4, 0, 2 };

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);  //using previously defined array

const String password = "7890";  //  password here
String input_password;  //take in the input
//--------------------------------------------------------------
//keypad end
bool alarmArmed = true;  //either true or false
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);  //fingerprint


void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.setRGB(colorR, colorG, colorB);  //lcd

  Serial.begin(115200);         // initialize serial port
  pinMode(TRIG_PIN, OUTPUT);    // set ESP32 pin to output mode
  pinMode(ECHO_PIN, INPUT);     // set ESP32 pin to input mode
  pinMode(BUZZER_PIN, OUTPUT);  // set ESP32 pin to output mode
  servo1.attach(SERVO_PIN);
  servo1.write(0);
  input_password.reserve(32);  // maximum input characters is 33, change if needed
  delay(1000);

  while (!Serial);  // For Yun/Leo/Micro/Zero/...  //fingerprint start
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  }
  else {
    Serial.println("Waiting for valid finger...");
      Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");  //fingerprint end
  }
}

void loop() {
  

    
 // {
  //getFingerprintID();
 // delay(50);           //don't ned to run this at full speed.
//}

  
  if (alarmArmed == true) {

    
   

    if (distance < DISTANCE_THRESHOLD) { //less than
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter password ");
 

      char key = keypad.getKey();  //to declare character type variable can only store 1 character

      if (key) {
        lcd.setCursor(2, 1);
        lcd.print(key);

  
  uint8_t p = finger.getImage();
   if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
    p = finger.image2Tz();
    if (p == FINGERPRINT_OK) {
       Serial.println("Found a print match!"); 
        lcd.clear();
            lcd.print(" ACCESS GRANTED!");
             digitalWrite(BUZZER_PIN, HIGH);
            delay(100);
             digitalWrite(BUZZER_PIN, LOW);
              delay(100);
             digitalWrite(BUZZER_PIN, HIGH);
            delay(100);
             digitalWrite(BUZZER_PIN, LOW);
            int posDegrees = 180;
            posDegrees >= 0;
            servo1.write(posDegrees);
            delay(15);


            delay(1000);
            lcd.clear();
            DHT.read(DHT11_PIN);
            lcd.print("temp:");
            lcd.print(DHT.temperature);
            lcd.setCursor(0, 1);
            lcd.print("  humi:");
            lcd.println(DHT.humidity);
            delay(2000);

            lcd.clear();
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
            Serial.print(inches);
            Serial.print("in,");
            Serial.print(cms);
            Serial.print("cm");
            Serial.println();
            delay(250);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(" cms: ");
          lcd.print(cms);
            delay(2000);

        if (key == '*') {
          input_password = "";  // clear input password
        } else if (key == '#') {
          if (password == input_password) {
            lcd.clear();
            lcd.print(" ACCESS GRANTED!");
             digitalWrite(BUZZER_PIN, HIGH);
            delay(100);
             digitalWrite(BUZZER_PIN, LOW);
              delay(100);
             digitalWrite(BUZZER_PIN, HIGH);
            delay(100);
             digitalWrite(BUZZER_PIN, LOW);
            int posDegrees = 180;
            posDegrees >= 0;
            servo1.write(posDegrees);
            delay(15);


            delay(1000);
            lcd.clear();
            DHT.read(DHT11_PIN);
            lcd.print("temp:");
            lcd.print(DHT.temperature);
            lcd.setCursor(0, 1);
            lcd.print("  humi:");
            lcd.println(DHT.humidity);
            delay(2000);

            lcd.clear();
            digitalWrite(TRIG_PIN, LOW);
            delayMicroseconds(5);
            digitalWrite(TRIG_PIN, HIGH);
            delayMicroseconds(10);
            digitalWrite(TRIG_PIN, LOW);
            // measure duration of pulse from ECHO pin
            duration = pulseIn(ECHO_PIN, HIGH);
            // calculate the distance
            //cms = (duration / 2) / 29.1;
           // inches = (duration / 2) / 74;
            // print the value to Serial Monitor
            Serial.print(inches);
            Serial.print("in,");
            Serial.print(cms);
            Serial.print("cm");
            Serial.println();
            delay(250);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(" cms: ");
          lcd.print(cms);
            delay(2000);

          } else {
            lcd.setCursor(0, 0);
            lcd.print("ACCESS DENIED!");
            int posDegrees = 180;
            posDegrees <= 0;
            digitalWrite(BUZZER_PIN, HIGH);
            delay(1000);
             digitalWrite(BUZZER_PIN, LOW);
              delay(1000);
          }
          input_password = "";  // clear input password
        } else {
          input_password += key;  // append new character to input password string
        }
       }
      }
    }
  }
}
  
  delay(100);
 
}