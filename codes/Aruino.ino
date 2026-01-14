#include <Wire.h>

// ================= I2C =================
#define SLAVE_ADDR 9
#define MSG_SIZE 20

// ================= Pins =================
#define TEMP_PIN A0
#define PH_PIN   A1

#define ACID_PUMP_PIN     13
#define ALKALINE_PUMP_PIN 12
#define HEATER_PIN        8
#define MOTOR_PIN         11   // PWM pin
#define SPEED_PIN         7    // Speed sensor input

// ================= State Variables =================
bool acidPumpIsOn = false;
bool alkalinePumpIsOn = false;
bool heaterIsOn = true;

float heaterMinTemp = 28.0f;
float heaterMaxTemp = 32.0f;

float motorValue = 60.0f; // PWM value
long motorSpeedRPM = 0;

float pHValue = 0.0f;
float pHMin = 3.0f;
float pHMax = 7.0f;

float temperatureValue = 30.0f;
float tempCalibrationM = 0.09467997f;
float tempCalibrationC = 17.0146f;

String lastRequest = "";

// ================= Setup =================
void setup() {
  pinMode(ACID_PUMP_PIN, OUTPUT);
  pinMode(ALKALINE_PUMP_PIN, OUTPUT);
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(SPEED_PIN, INPUT);

  Wire.begin(SLAVE_ADDR);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  Serial.begin(9600);
}

// ================= Main Loop =================
void loop() {
  updateReadings();
  temperatureControl();
  pHControl();
  updateState();
}

// ================= Control Logic =================
void temperatureControl() {
  if (temperatureValue < heaterMinTemp) heaterIsOn = true;
  if (temperatureValue > heaterMaxTemp) heaterIsOn = false;
}

void pHControl() {
  if (pHValue < pHMin) {
    alkalinePumpIsOn = true;
    acidPumpIsOn = false;
  } else if (pHValue > pHMax) {
    alkalinePumpIsOn = false;
    acidPumpIsOn = true;
  } else {
    alkalinePumpIsOn = false;
    acidPumpIsOn = false;
  }
}

// ================= Apply Outputs =================
void updateState() {
  digitalWrite(ACID_PUMP_PIN, acidPumpIsOn ? HIGH : LOW);
  digitalWrite(ALKALINE_PUMP_PIN, alkalinePumpIsOn ? HIGH : LOW);
  digitalWrite(HEATER_PIN, heaterIsOn ? HIGH : LOW);

  analogWrite(MOTOR_PIN, (int)motorValue);
}

// ================= Sensor Readings =================
void updateReadings() {
  long timeOn = pulseIn(SPEED_PIN, HIGH);
  long timeOff = pulseIn(SPEED_PIN, LOW);
  long total = 2 * (timeOn + timeOff);

  if (total > 0) {
    motorSpeedRPM = 60000000L / total;
  }

  pHValue = mapPH(analogRead(PH_PIN));
  temperatureValue = mapTemperature(analogRead(TEMP_PIN));
}

// ================= I2C Communication =================
void receiveEvent(int bytes) {
  String received = "";
  while (Wire.available()) {
    char c = Wire.read();
    received += c;
  }
  lastRequest = received;
}

void requestEvent() {
  if (lastRequest == "sendstateAC") {
    sendAnswer(acidPumpIsOn ? "ON" : "OFF");
  } 
  else if (lastRequest == "sendstateAL") {
    sendAnswer(alkalinePumpIsOn ? "ON" : "OFF");
  } 
  else if (lastRequest == "sendstateH") {
    sendAnswer(heaterIsOn ? "ON" : "OFF");
  } 
  else if (lastRequest == "sendstates") {
    sendAnswer(String(motorSpeedRPM));
  } 
  else if (lastRequest == "sendstateP") {
    sendAnswer(String(pHValue));
  } 
  else if (lastRequest == "sendstateT") {
    sendAnswer(String(temperatureValue));
  } 
  else {
    sendAnswer("UNKNOWN");
  }
}

// ================= Helpers =================
void sendAnswer(String answer) {
  byte response[MSG_SIZE] = {0};
  for (int i = 0; i < MSG_SIZE && i < answer.length(); i++) {
    response[i] = (byte)answer.charAt(i);
  }
  Wire.write(response, MSG_SIZE);
}

float mapPH(float value) {
  return map(value, 538, 464, 4, 10);
}

float mapTemperature(float value) {
  return value * tempCalibrationM + tempCalibrationC;
}
