#include <AccelStepper.h>

#define MotorInterface 1
#define NB_STEPPERS 7

// TODO CHECK REDUCTIONS
const int gearbox_reductions[] = {   50,  10,  20,  20,  10,  10,  10 };
const int pulsePins[] =          {    0,   0,   0,   0,   0,   0,   0 };
const int dirPins[] =            {    0,   0,   0,   0,   0,   0,   0 };
const int micro_steps[] =        {    4,   4, ...};
const int deg_per_step[] =       {  1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8 };

// Remaining distances (in steps) of the steppers to target position
const double distances[] =       {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0 };

const int limit_switch_pins[] = { 0, 0, 0, 0, 0, 0, 0 };

// https://hackaday.io/project/183279-accelstepper-the-missing-manual/details
AccelStepper steppers[7];

String inputString = ""; // A string that holds the command sent by the server

void onDataReceived(String command);
String dataToSend();

long getSteps(int motorId, double angle);
double getAngleRad(int motorId, long steps);
void sendDistances();

void setup() {
    // Initialize all steppers
    for (int i = 0; i < NB_STEPPERS; ++i) {
        steppers[i] = new AccelStepper(MotorInterface, pulsePins[i], dirPins[i]);
        steppers[i].setMaxSpeed(50 * gearbox_reductions[i] * micro_steps[i]); // So that all motors go to same speed
        steppers[i].setAcceleration(2 * gearbox_reductions[i] * micro_steps[i]); // So that all motors accelerate equally
    }

    // Initialize Serial connection
    Serial.begin(9600);
    // Reserve 200 bytes
    inputString.reserve(200);
}

void loop() {
    // put your main code here, to run repeatedly:
    // Double check
    for (int i = 0; i < NB_STEPPERS; ++i) {
        // Make the stepper take one step if one is needed
        // Relative to the current position and target position
        steppers[i].run();
    }

    sendDistances();
}

void moveTo(int motorId, float angleRad) {
    steppers[motorId].moveTo(getSteps(motorId, angleRad));
}

void serialEvent() {
    while (Serial.available()) {
        // get the new byte:
        char inChar = (char)Serial.read();
        // add it to the inputString:
        inputString += inChar;
        // if the incoming character is a newline, set a flag so the main loop can
        // do something about it:
        if (inChar == '\n') {
            break;
        }
    }
    onDataReceived(inputString);
}

void onDataReceived(String command) {
    // TODO Set all targets position from angles
}

String dataToSend() {
    // TODO Send all distances
}

void sendDistances() {
    for (int i = 0; i < NB_STEPPERS; ++i) { distances[i] = steppers[i].distanceToGo(); }
    Serial.println(dataToSend());
}

long getSteps(int motorId, double angleRad) {
    return gearbox_reductions[motorId] * micro_steps[motorId] * (angleRad / deg_per_step[motorId])
}

double getAngleRad(int motorId, long steps) {
    return (steps * deg_per_step[motorId]) / (gearbox_reductions[motorId] * micro_steps[motorId])
}