#include <Wire.h>
#include <RTClib.h>
#include <Servo.h>

// Pin assignments
const int photoresistorPin = A0;
const int rainSensorPin    = A1;
const int servoPin         = 7;

// Thresholds (adjust as needed)
const int rainThreshold  = 600;
const int lightThreshold = 300;

// Drying time window (24-hour format)
const int dryingStartHour   = 8;   // 8:00
const int dryingStartMinute = 0;
const int dryingEndHour     = 16;  // 17:00 (5PM)
const int dryingEndMinute   = 23;

Servo myServo;
RTC_DS1307 rtc;

int lastServoPos = -1; // Track last servo position

void setup() {
  Serial.begin(9600);
  myServo.attach(servoPin);
  pinMode(photoresistorPin, INPUT);
  pinMode(rainSensorPin, INPUT);

  // RTC setup
  Wire.begin();
  rtc.begin();
  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running, setting time to compile time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  // Read sensors
  int rainValue  = analogRead(rainSensorPin);
  int lightValue = analogRead(photoresistorPin);
  DateTime now   = rtc.now();

  // Log sensor values and time
  Serial.print("Time: ");
  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(" | Rain value: ");
  Serial.print(rainValue);
  Serial.print(" | Light value: ");
  Serial.println(lightValue);

  // Check conditions
  bool raining   = rainValue < rainThreshold;
  bool dark      = lightValue < lightThreshold;
  bool dryingNow = isWithinDryingTime(now);

  int desiredPos;
  String status;

  if (!dryingNow) {
    desiredPos = 180;
    status = "Drying time ended! Servo moved to 180°.";
  } else if (raining) {
    desiredPos = 180;
    status = "Rain detected! Servo moved to 180°.";
  } else if (dark) {
    desiredPos = 180;
    status = "Dark detected! Servo moved to 180°.";
  } else {
    desiredPos = 0;
    status = "Normal light and time. Servo returned to 0°.";
  }

  // Only write if changed
  if (desiredPos != lastServoPos) {
    myServo.write(desiredPos);
    lastServoPos = desiredPos;
    Serial.println(status);
  }

  delay(1000); // 1 second delay
}

bool isWithinDryingTime(int nowHour, int nowMinute) {
  int nowMins = nowHour * 60 + nowMinute;
  int startMins = dryingStartHour * 60 + dryingStartMinute;
  int endMins   = dryingEndHour   * 60 + dryingEndMinute;
  if (startMins < endMins) {
    // Current day
    return (nowMins >= startMins) && (nowMins < endMins);
    
  } else { 
    // Next day
    return (nowMins >= startMins) || (nowMins < endMins);
  }
}