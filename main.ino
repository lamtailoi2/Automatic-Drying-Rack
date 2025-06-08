#include <Servo.h>

Servo myServo;

const int photoresistorPin = A0; // Photoresistor (LDR) on A0
const int rainSensorPin = A1;    // Rain Sensor on A1
const int servoPin = 7;          // Servo on pin 7

void setup() {
  myServo.attach(servoPin);
  pinMode(photoresistorPin, INPUT);
  pinMode(rainSensorPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  int rainValue = analogRead(rainSensorPin);        // Read rain sensor
  int lightValue = analogRead(photoresistorPin);    // Read light sensor

  // Log sensor values
  Serial.print("Rain value: ");
  Serial.print(rainValue);
  Serial.print(" | Light value: ");
  Serial.println(lightValue);

  // Thresholds: adjust as needed for your environment
  int rainThreshold = 600;   // Rain detected if below this value
  int lightThreshold = 300;  // Dark if below this value

  if (rainValue < rainThreshold) {
    // Rain detected, highest priority
    myServo.write(180);
    Serial.println("Rain detected! Servo moved to 180°.");
  } else if (lightValue < lightThreshold) {
    // Dark detected, high priority
    myServo.write(180);
    Serial.println("Dark detected! Servo moved to 180°.");
  } else {
    // Normal light, servo returns to start
    myServo.write(0);
    Serial.println("Normal light. Servo returned to 0°.");
  }

  delay(500); // Small delay
}