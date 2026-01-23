// #include <NimBLEDevice.h>
#include <ESP32Servo.h>
#include "Fuzzy.h"
#include "GeometricShapes.h"

Servo esc, steer;
Fuzzy fuzzy;

const int PIN_ESC   = 23;
const int PIN_STEER = 22;
const int LeftTCRT1 = 33;
const int LeftTCRT2 = 32;
const int RightTCRT1 = 34;
const int RightTCRT2 = 35;

int leftValue1 = 0;
int leftValue2 = 0;
int rightValue1 = 0;
int rightValue2 = 0;


void setup() {
  Serial.begin(115200);
  pinMode(LeftTCRT1, INPUT);
  pinMode(LeftTCRT2, INPUT);
  pinMode(RightTCRT1, INPUT);
  pinMode(RightTCRT2, INPUT); 

  // ESC & Servo
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  esc.setPeriodHertz(50);
  esc.attach(PIN_ESC, 1000, 2000);
  steer.attach(PIN_STEER, 1000, 2000);
  esc.writeMicroseconds(1500);
  steer.writeMicroseconds(1570); // Mitte angepasst
  delay(6000);
  esc.writeMicroseconds(1650);
}

void loop() {
    esc.writeMicroseconds(1650);

    int sL = analogRead(33);
    int bL = analogRead(32);
    int sR = analogRead(34);
    int bR = analogRead(35);

    //TODO: wenn beide werte hoch sind, anhalten

    // 1. Fuzzy inference
    InputMembershipDegrees inM = fuzzy.inference(sL, bL, sR, bR);

    // 2. Regeln
    OutputMembershipDegrees outM = fuzzy.rules(inM);

    // 3. Defuzzify -> PWM
    float steering_pwm = fuzzy.defuzzify(outM);

    Serial.print("PWM: ");
    Serial.println(steering_pwm);

    steer.writeMicroseconds(steering_pwm);




  delay(20);  
}
