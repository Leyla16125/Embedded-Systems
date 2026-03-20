#include <Wire.h> //Enables I2C communication
#include <uRTCLib.h> // DS1307 RTC uses I2C protocol

uRTCLib rtc(0x68); // DS1307 RTC

// Digit control pins (D1 D2 D3 D4)
int digitPins[4] = {0, 4, 3, 2};   
// Segment pins: A B C D E F G DP
int segmentPins[8] = {8, 9, 10, 11, 12, 6, 7, 1};

// Segment patterns for numbers 0–9
byte numbers[10][8] = {
  {1,1,1,1,1,1,0,0}, // 0
  {0,1,1,0,0,0,0,0}, // 1
  {1,1,0,1,1,0,1,0}, // 2
  {1,1,1,1,0,0,1,0}, // 3
  {0,1,1,0,0,1,1,0}, // 4
  {1,0,1,1,0,1,1,0}, // 5
  {1,0,1,1,1,1,1,0}, // 6
  {1,1,1,0,0,0,0,0}, // 7
  {1,1,1,1,1,1,1,0}, // 8
  {1,1,1,1,0,1,1,0}  // 9
};

int counter = 0;
byte lastSecond = 0;

const int buttonPin = A0;
bool paused = false;

const int greenLedPin = 13;
const int redLed = 5;  
// Debounced single press event
bool buttonPressedEvent() {
  static bool last = HIGH;
  static unsigned long tLastChange = 0;
  static bool armed = true;

  bool now = digitalRead(buttonPin);

  if (now != last) {
    tLastChange = millis();
    last = now;
  }

  if ((millis() - tLastChange) > 30) {   // debounce
    if (last == LOW && armed) {
      armed = false;
      return true;
    }
    if (last == HIGH) armed = true;
  }
  return false;
}

void setup() {
  Wire.begin();

  pinMode(buttonPin, INPUT_PULLUP);

  pinMode(greenLedPin, OUTPUT);
  pinMode(redLed, OUTPUT);

  for (int i = 0; i < 8; i++)
    pinMode(segmentPins[i], OUTPUT);

  for (int i = 0; i < 4; i++) {
    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], LOW);
  }
}

void loop() {

  bool pressed = buttonPressedEvent();   // detect press ONCE

  if (pressed) {
    paused = !paused;
  }

  rtc.refresh();
  byte currentSecond = rtc.second();

  if (!paused && currentSecond != lastSecond) {
    lastSecond = currentSecond;
    counter++;
    if (counter > 10) counter = 0;
  }

  if (pressed) {

    if (counter == 10) {
      digitalWrite(greenLedPin, HIGH);
      digitalWrite(redLed, LOW);
      delay(1000);
    }
    if (counter != 10) {
      digitalWrite(redLed, HIGH);
      digitalWrite(greenLedPin, LOW);
      delay(1000);
    }

    // Turn both OFF after 1 second
    digitalWrite(greenLedPin, LOW);
    digitalWrite(redLed, LOW);
  }

  displayTwoDigits(counter);
}

// Show one digit on selected 7-segment
void showDigit(int value, int digitIndex) {

  for (int i = 0; i < 4; i++)
    digitalWrite(digitPins[i], LOW);

  for (int i = 0; i < 8; i++)
    digitalWrite(segmentPins[i], numbers[value][i]);

  digitalWrite(digitPins[digitIndex], HIGH);
  delay(3);
}

// Display number only on 3rd & 4th digits
void displayTwoDigits(int num) {
  int tens = num / 10;
  int ones = num % 10;

  if (tens > 0) showDigit(tens, 2);
  showDigit(ones, 3);
}