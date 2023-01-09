#include <AccelStepper.h>

#define m1Pulse  6     // Blue   - 28BYJ48 pin 1
#define m1Dir  5     // Pink   - 28BYJ48 pin 2
#define m1Reduc 50    // Motor Gearbox Reduction
#define m1MicStep 4   // Microstepping of the stepepr

#define m2Pulse  11     // Yellow - 28BYJ48 pin 3
#define m2Dir  10     // Orange - 28BYJ48 pin 
#define m2Reduc 10    // Motor Gearbox Reduction
#define m2MicStep 4   // Microstepping of the stepper

#define degPerStep 1.8 // Number of degrees in one step

#define MotorInterface 1
 
// Define two motor objects
// The sequence 1-3-2-4 is required for proper sequencing of 28BYJ48
AccelStepper stepper1(MotorInterface, m1Pulse, m1Dir);
AccelStepper stepper2(MotorInterface, m2Pulse, m2Dir);

void setup() {
  // put your setup code here, to run once:
  stepper1.setMaxSpeed(10de00);
  stepper2.setMaxSpeed(1000);

  stepper1.setAcceleration(500);
  stepper2.setAcceleration(500);
}

void loop() {
  // put your main code here, to run repeatedly:

  moveTo(1, 90);
  delay(2000);
  moveTo(2, 90);
  delay(2000);
  moveTo(1, 0);
  delay(2000);
  moveTo(2, 0);
  delay(2000);
}

void moveTo(int mIndex, float deg) {
  if (mIndex == 1) {
    stepper1.moveTo(deg/degPerStep * m1Reduc * m1MicStep);
    stepper1.runToPosition();

  } else if (mIndex == 2) {
    stepper2.moveTo(deg/degPerStep * m2Reduc * m2MicStep);
    stepper2.runToPosition();
  }
}

void serialEvent() {

}
