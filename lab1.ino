#include <Wire.h>
#include <uRTCLib.h>

uRTCLib rtc(0x68); // DS1307 RTC

// Digit control pins (D1 D2 D3 D4)
int digitPins[4] = {5, 4, 3, 2};

// Segment pins: A B C D E F G DP
int segmentPins[8] = {8, 9, 10, 11, 12, 6, 7, 1};

// 10 row(0-9)- 8 segments
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
byte lastSecond = 0; // previous RTC second value.

const int buttonPin = A0;     // push button
bool paused = false;          // false = counting, true = stopped

bool buttonPressedEvent() {
  static bool last = HIGH; //button is not pressed.
  static unsigned long tLastChange = 0;// remembers when state changed
  static bool armed = true; //armed = false → ignore button until it is released

  bool now = digitalRead(buttonPin);// current state of the button 

  if (now != last) {
    tLastChange = millis();//changed
    last = now;
  }

  if ((millis() - tLastChange) > 30) {  // Has 30 milliseconds passed since the last button change?
    if (last == LOW && armed) {          // pressed (INPUT_PULLUP)
      armed = false; // allow next press
      return true;
    }
    if (last == HIGH) armed = true;     //Now system is ready for next press
  }
  return false; //no valid press happened:
}

void setup() {
  Wire.begin();// Turn on the communication system

  pinMode(buttonPin, INPUT_PULLUP);

  for (int i = 0; i < 8; i++)
    pinMode(segmentPins[i], OUTPUT);

  for (int i = 0; i < 4; i++) {
    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], LOW);// Turn digits OFF
  }
}

void loop() {

  if (buttonPressedEvent()) {
    paused = !paused; // press to stop, press again to continue
  }

  rtc.refresh();// resart 0

  byte currentSecond = rtc.second();//0

  // Increment counter once per second ONLY if not paused
  if (!paused && currentSecond != lastSecond) {
    lastSecond = currentSecond;
    counter++;
    if (counter > 10) counter = 0; // wrap around 0 → 10
  }

  // Keep refreshing display rapidly
  displayTwoDigits(counter);// show counter
}

// Show one digit on selected 7-segment
void showDigit(int value, int digitIndex) {
  // Turn all digits OFF
  for (int i = 0; i < 4; i++)
    digitalWrite(digitPins[i], LOW);

  // choose number
  for (int i = 0; i < 8; i++)
    digitalWrite(segmentPins[i], numbers[value][i]);

  // Enable selected digit
  digitalWrite(digitPins[digitIndex], HIGH);
  delay(3); // choose index
}

// Display number only on 3rd & 4th digits
void displayTwoDigits(int num) {
  int tens = num / 10;
  int ones = num % 10;

  if (tens > 0) showDigit(tens, 2); // 3rd digit
  showDigit(ones, 3);               // 4th digit
}
