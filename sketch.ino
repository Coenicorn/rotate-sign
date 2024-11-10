#include <Arduino.h>
#include <Servo.h>
#include <I2C_RTC.h>

static Servo s1;
static DS3231 RTC;

#define SERVO_MIN (uint16_t)(1000)
#define SERVO_MAX (uint16_t)(2000)
#define MINUTES_IN_DAY (uint16_t)(720)

#define STATUS_LED_PIN 8

void setup() {  
  s1.attach(9);
  RTC.begin();
  Serial.begin(9600);

  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);

  // RUN ONCE
  // set hour mode
  RTC.setHourMode(CLOCK_H24);
}

void loop() {
  if (RTC.isRunning()) {
    digitalWrite(STATUS_LED_PIN, LOW);
    // calculate current minute of day
    uint8_t hourOfDay = RTC.getHours();
    uint16_t minuteOfDay = hourOfDay * 60;

    Serial.print("Minutes: ");
    Serial.print(minuteOfDay);

    uint16_t servoRotateValue = (int)((float)(SERVO_MAX) / (float)(MINUTES_IN_DAY) * (float)(minuteOfDay)) + SERVO_MIN;
    Serial.print(" Servo value: ");
    Serial.print(servoRotateValue);

    s1.writeMicroseconds(servoRotateValue);

    delay(10000);
  } else {
    digitalWrite(STATUS_LED_PIN, HIGH);
    delay(1000);
  }
}

