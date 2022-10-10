#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include <IRremote.h>
#include <Stepper.h>
#include <Wire.h>

//STEPPER STUFF ----
//stepper movement
float stepsPerRevolution = 1.00;        //*100
float speed = 50 / stepsPerRevolution;  //max 1000
float focusAmount = 0;

String newStepsPerRevolution = String(stepsPerRevolution);

// Creates an instance of stepper class
// Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence
Stepper myStepper = Stepper(stepsPerRevolution * 100, 8, 10, 9, 11);

//IR STUFF ----
// IR reciever pin
const int RECV_PIN = 7;
IRrecv irrecv(RECV_PIN);
decode_results results;
unsigned long key_value = 0;

//OLED STUFF
// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

// Define proper RST_PIN if required.
#define RST_PIN -1

SSD1306AsciiAvrI2c oled;
//------------------------------------------------------------------------------
void setup() {
  //initialize IR reading
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
  oled.println("Focus Amount 0");
  oled.setCursor(0, 2);
  oled.println("Focus Level 0");
}
//------------------------------------------------------------------------------
void updateDisplay() {
  oled.clear();
  oled.setCursor(0, 0);
  oled.println("Focus Amount " + String(focusAmount));
  oled.setCursor(0, 2);
  oled.println("Focus Steps " + String(stepsPerRevolution));
}

void updateSPR(String button) {
  newStepsPerRevolution += button;  //adds 1.00 to newStepsPerRevolution
  float sprValue = newStepsPerRevolution.toFloat()/100;
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
        //diplay
        oled.clear();
        oled.print(">>|");

        //move stepper
        myStepper.setSpeed(speed);
        myStepper.step(stepsPerRevolution * 100);
        Serial.println(">>|");

        //update display
        focusAmount += stepsPerRevolution;  //approx 5x20steps = one notch
        updateDisplay();

        break;

      //|<<
      case 0XFF22DD:
        //diplay
        oled.clear();
        oled.print("|<<");

        //move stepper
        myStepper.setSpeed(speed);
        myStepper.step(-stepsPerRevolution * 100);
        Serial.println(">>|");


        focusAmount -= stepsPerRevolution;  //approx 5x20steps = one notch
        //update display
        updateDisplay();

        break;

      //EQ or New Step Int (visualized on screen as a float)
      //If pressed user inputs numbers on remote for new step count
      case 0XFF906F:
        oled.setCursor(0, 4);
        oled.println("NEW FOCUS STEPS: " + newStepsPerRevolution);
        newStepsPerRevolution = "";

        while (newStepsPerRevolution.length() < 3) {  //while length of newStepsPerRevolution are less than 4 keep adding
          if (irrecv.decode(&results)) {              //if button is pressed switch case
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
        stepsPerRevolution = newStepsPerRevolution.toFloat() / 100;
        updateDisplay();
    }
    key_value = results.value;
    irrecv.resume();
  }
}
