#include <WiFi.h>
#include <Wire.h>
#include "ThingsBoard.h"

// =====================
// I2C Configuration
// =====================
#define SLAVE_ADDR 0x08
#define I2C_SDA 22
#define I2C_SCL 21
#define MSG_SIZE 20

// =====================
// ThingsBoard Config
// =====================
#define TOKEN "SBVmue4mW7G3MIFkMHCu"
#define THINGSBOARD_SERVER "demo.thingsboard.io"

// =====================
// WiFi Credentials
// =====================
const char* ssid = "Filipps";
const char* password = "Fi112345";

// =====================
// Objects & Variables
// =====================
WiFiClient espClient;
ThingsBoard tb(espClient);
int status = WL_IDLE_STATUS;

String serialString = "";

// =====================
// Setup
// =====================
void setup() {
  Serial.begin(9600);
  Wire.begin(I2C_SDA, I2C_SCL);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
}

// =====================
// Main Loop
// =====================
void loop() {
  if (!tb.connected()) {
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Failed to connect to ThingsBoard");
      delay(1000);
      return;
    }
  }

  updateState();
  tb.loop();
  delay(200);
}

// =====================
// I2C Send Request
// =====================
void sendRequest(String request) {
  Wire.beginTransmission(SLAVE_ADDR);

  byte buffer[MSG_SIZE] = {0};
  for (int i = 0; i < request.length() && i < MSG_SIZE; i++) {
    buffer[i] = (byte)request.charAt(i);
  }

  Serial.println("Sending msg: " + request);
  Wire.write(buffer, MSG_SIZE);
  Wire.endTransmission();

  delay(200);
}

// =====================
// I2C Read Response
// =====================
void readSerial() {
  serialString = "";

  Wire.requestFrom(SLAVE_ADDR, MSG_SIZE);
  while (Wire.available()) {
    char c = Wire.read();
    if (c != 0) {
      serialString += c;
    }
  }

  delay(200);
}

// =====================
// Update System State
// =====================
void updateState() {
  char charBuf[50];

  // Acid Pump
  sendRequest("sendstateAC");
  readSerial();
  serialString.toCharArray(charBuf, 50);
  tb.sendTelemetryString("Acid Pump State", charBuf);

  // Alkaline Pump
  sendRequest("sendstateAL");
  readSerial();
  serialString.toCharArray(charBuf, 50);
  tb.sendTelemetryString("Alkaline Pump State", charBuf);

  // Heater
  sendRequest("sendstateH");
  readSerial();
  serialString.toCharArray(charBuf, 50);
  tb.sendTelemetryString("Heater State", charBuf);

  // Motor Speed
  sendRequest("sendstateS");
  readSerial();
  tb.sendTelemetryFloat("Motor Speed", serialString.toFloat());

  // pH Level
  sendRequest("sendstateP");
  readSerial();
  tb.sendTelemetryFloat("pH Level", serialString.toFloat());

  // Temperature
  sendRequest("sendstateT");
  readSerial();
  tb.sendTelemetryFloat("Temperature", serialString.toFloat());
}
