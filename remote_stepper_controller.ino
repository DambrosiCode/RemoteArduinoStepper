#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include <IRremote.h>
#include <Wire.h>
//-- STEPPER STUFF
float steps = 500;

const float stepPerRev = 3200;

float degrees = (steps / stepPerRev) * 360;
float stepsDistance = 0;

String newStepDegrees = "";  //temp holds when user is inputting steps on remote

// defines pins numbers
const int stepPin = 3;
const int dirPin = 2;

const int ms_one = 7;
const int ms_two = 8;
const int ms_three = 9;

//--- IR STUFF
// IR reciever pin
const int RECV_PIN = 5;
IRrecv irrecv(RECV_PIN);
decode_results results;
unsigned long key_value = 0;

//-- OLED STUFF
// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

// Define proper RST_PIN if required.
#define RST_PIN -1

SSD1306AsciiAvrI2c oled;

// -------------------------------------------
void setup() {
  //init IR reading
  Serial.begin(9600);
  irrecv.enableIRIn();
  irrecv.blink13(true);

//init oled
#if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else   // RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif  // RST_PIN >= 0
  // Call oled.setI2cClock(frequency) to change from the default frequency.

  oled.setFont(System5x7);
  oled.clear();
  oled.setCursor(0, 0);
  oled.println("Focus Amount " + String(degrees));
  oled.setCursor(0, 2);
  oled.println("Focus Change " + String(stepsDistance));

  //init stepper
  // Sets the two pins as Outputs
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  // Sets the two pins as Outputs
  pinMode(ms_one, OUTPUT);
  pinMode(ms_two, OUTPUT);
  pinMode(ms_three, OUTPUT);

  digitalWrite(ms_one, HIGH);
  digitalWrite(ms_two, HIGH);
  digitalWrite(ms_three, HIGH);
}

void moveStepper(int steps, bool forward) {
  if (forward) {
    digitalWrite(dirPin, HIGH);  // Enables the motor to move in a particular direction
    // Makes 200 pulses for making one full cycle rotation
    for (int x = 0; x < steps; x++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
  } else {
    digitalWrite(dirPin, LOW);  // Enables the motor to move in a particular direction
    // Makes 200 pulses for making one full cycle rotation
    for (int x = 0; x < steps; x++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }
  }
}

void updateDisplay() {
  Serial.println(degrees);
  Serial.println(steps);
  oled.clear();
  oled.setCursor(0, 0);
  oled.println("Focus Amount " + String(degrees));
  oled.setCursor(0, 2);
  oled.println("Focus Distance " + String(stepsDistance));
}

void updateSPR(String button) {
  newStepDegrees += button;  //adds 1.00 to newStepDegrees
  float sprValue = newStepDegrees.toFloat() / 100;
  //update display
  oled.setCursor(0, 4);
  oled.println("NEW FOCUS STEPS: " + String(sprValue));
}

void loop() {
  if (irrecv.decode(&results)) {
    //if results repeat in value
    //if (results.value == 0XFFFFFFFF)
    //  results.value = key_value;
    switch (results.value) {
      //>>|
      case 0XFF02FD:
        moveStepper(steps, true);
        stepsDistance += degrees;
        updateDisplay();
        break;
      //|<<
      case 0XFF22DD:
        moveStepper(steps, false);
        stepsDistance -= degrees;
        updateDisplay();
        break;

      //EQ or New Step Int (visualized on screen as a float)
      case 0XFF906F:
        oled.setCursor(0, 4);
        oled.println("NEW FOCUS STEPS: " + newStepDegrees);
        newStepDegrees = "";
        while (newStepDegrees.length() < 3) {  //while length of newStepDegrees are less than 4 keep adding
          if (irrecv.decode(&results)) {       //if button is pressed switch case
            switch (results.value) {
              //BUTTON - 0
              case 0XFF6897:
                updateSPR("0");
                break;
                //BUTTON - 1
              case 0XFF30CF:
                updateSPR("1");
                break;
                //BUTTON - 2
              case 0XFF18E7:
                updateSPR("2");
                break;
                //BUTTON - 3
              case 0XFF7A85:
                updateSPR("3");
                break;
                //BUTTON - 4
              case 0XFF10EF:
                updateSPR("4");
                break;
                //BUTTON - 5
              case 0XFF38C7:
                updateSPR("5");
                break;
                //BUTTON - 6
              case 0XFF5AA5:
                updateSPR("6");
                break;
                //BUTTON - 7
              case 0XFF42BD:
                updateSPR("7");
                break;
                //BUTTON - 8
              case 0XFF4AB5:
                updateSPR("8");
                break;
                //BUTTON - 9
              case 0XFF52AD:
                updateSPR("9");
                break;
            }
            key_value = results.value;
            irrecv.resume();
          }
        }
        //after the while loop
        oled.setCursor(0, 4);
        oled.clear();
        steps = (newStepDegrees.toFloat()/100)*stepPerRev/360;
        degrees = (steps / stepPerRev) * 360;
        updateDisplay();
    }
    key_value = results.value;
    irrecv.resume();
  }
}
