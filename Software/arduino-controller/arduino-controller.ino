#include <AccelStepper.h>

#define MotorInterface 1
#define NB_STEPPERS 7
#define SWITCH_PRESS_STATE 0

#define MOTOR_SPEED 10
#define MOTOR_ACCEL 16

#define DISTANCES_END_VALUE_MARKER "D"

// TODO CHECK REDUCTIONS
const int pulsePins[] =          {   24,  28,  34,  38,  42,  46,  50 };
const int dirPins[] =            {   26,  30,  36,  40,  44,  48,  52 };

const int gearbox_reductions[] = {   10,  50,  10,  50,  10,  10,  10 };
const int micro_steps[] =        {    4,   4,   1,   1,   1,   1,   1 };
const double deg_per_step[] =       {  1.8, 1.8, 1.8, 1.8, 1.8, 1.8, 1.8 };
const int directions[] =         {    1,   1,   1,  -1,   1,   1,   1 };

// TODO CHANGE
const double max_bound[] =         { 180.0, 45.0, 45.0, 45.0, 45.0, 45.0, 45.0};
const double min_bound[] =         { -180.0, -45.0, -45.0, -45.0, -45.0, -45.0, 45.0};

// positions (in steps) of the steppers
double positions[] =             {  0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }; // In degrees

const int limit_switch_pins[] =  {   2, 3, 4, 5, 6, 7, 8 };
const double limit_switch_pos[]= {  90.0,  90.0, 90.0,  45.0, 90.0,  90.0, 90.0 }; // Angle in degrees, must be converted to steps
const double setup_dist_tolerance = 5 / 180.0 * PI; // The previous motor must be in a 5 Degrees tolerance distance of 0 before moving the next motor 

bool setup_failure = false;
int setup_index = 0;
bool is_setup_sequence_over = false;

// https://hackaday.io/project/183279-accelstepper-the-missing-manual/details
AccelStepper *steppers[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };

String inputString = ""; // A string that holds the command sent by the server

void onDataReceived(String command);
String dataToSend();

long getSteps(int motorId, double deg);
double getAngleRad(int motorId, long steps);
double getDistance(int motorId);
void sendPositions();
void send_motor_speed();

void moveTo(int motorId, double angleRad);

bool isPressed(int switchPin);
void run_setup();

double to_radians(double degrees) {
    return degrees / 180.0 * PI;
}

double to_degrees(double rad) {
  return rad / PI * 180.0; 
}


void setup() {

    // Initialize Serial connection
    Serial.begin(9600);
    Serial.println("\nStarting setup...");

    // Initialize all steppers
    for (int i = 0; i < NB_STEPPERS; ++i) {
        steppers[i] = new AccelStepper(1, pulsePins[i], dirPins[i]);
        steppers[i]->setMaxSpeed(MOTOR_SPEED * gearbox_reductions[i] * micro_steps[i]);    // So that all motors go to same speed
        steppers[i]->setAcceleration(MOTOR_ACCEL * gearbox_reductions[i] * micro_steps[i]); // So that all motors go to same speed

        // Set setup speed
        steppers[i]->setSpeed(directions[i]/4.0 * MOTOR_SPEED * gearbox_reductions[i] * micro_steps[i]);

        // Setup the limit switch's pin as an INPUT
        pinMode(limit_switch_pins[i], INPUT);
    }

    // Reserve 200 bytes
    inputString.reserve(200);

    Serial.print("Setup complete : ");
    Serial.println(setup_failure);

}

void loop() {

  if(setup_failure) { return; }

  if (!is_setup_sequence_over) { run_setup(); } 
  
  // Send positions anyway so that we can see the arm setup (Just for the sake of it)

}

void run_setup() {
  if(setup_index >= NB_STEPPERS) { // The last motor was setup
    is_setup_sequence_over = true;
    send_motor_speed(); // Send the motor speed, signaling to the server the arm can now receive commands
    return;

  } else if(setup_index > 0 && getDistance(setup_index-1) > getSteps(setup_index-1, setup_dist_tolerance)) { 
    return; // Wait for previous motor to reach 0 position
    
  } else if(isPressed(setup_index)) {
    Serial.println("Pressed !");
    // The steppers[setup_index] has reached the limit_switch
    // Stop the motor abruptly AND set its current position to its limit switch pos
    steppers[setup_index]->stop();
    steppers[setup_index]->setCurrentPosition(getSteps(setup_index, limit_switch_pos[setup_index]));
     // THIS ONLY WORKS CONSIDERING THAT THE ARM WITH ALL MOTORS TO 0 IS AN ARM POINTING TOWARDS THE CEILING
    steppers[setup_index]->moveTo(0);
    Serial.println(steppers[setup_index]->currentPosition());
    Serial.println(getAngleDeg(setup_index, steppers[setup_index]->distanceToGo()));
    steppers[setup_index]->runToPosition(); 
    setup_index++;

  } else {
    // Move the stepper in the direction of its limit switch, one step by one step
    steppers[setup_index]->runSpeed();
  }
}

void moveTo(int motorId, double deg) {
    // Check that the motor is in bounds
    if ( min_bound[motorId] < deg && deg < max_bound[motorId] ) {
        steppers[motorId]->moveTo(getSteps(motorId, deg));
    }
}

void serialEvent() {
    if(!is_setup_sequence_over) { return; } // Do not accept messages when in setup mode

    while (Serial.available()) {
        // get the new byte:
        char inChar = (char)Serial.read();
        // add it to the inputString:
        inputString += inChar;
        // if the incoming character is a newline, call onDataReceived and clear the string
        // do something about it:
        if (inChar == '\n') {
            onDataReceived(inputString);
            inputString = "";
            break;
        }
    }
}

void onDataReceived(String command) {
    String msg = command;                              // Allocate a string for the message
    msg = msg.substring(0, 2);                         // Remove header

    int index = 0;
    for(int i = 0; i < NB_STEPPERS; i++) {             // For eachs Stepper :
      index = msg.indexOf(DISTANCES_END_VALUE_MARKER); //   Find next "D" marker
      String value = msg.substring(0, index);          //   Isolate the angle value
      moveTo(i, value.toDouble());                     //   Sets the target as the angle
      msg = msg.substring(index+1);                    //   Remove the value from the string
    }
}

String dataToSend() {
    // Send all angle positions
    String msg = "AN";
    for(int i = 0; i < NB_STEPPERS; i++) {
        msg += String(positions[i], 4) + DISTANCES_END_VALUE_MARKER;
    }

    return msg;
}

void send_motor_speed() {
  // Send the speed of the motor, also signals the server the arm did its setup sequence and is read to
  // receive instructions
  Serial.println("SP" + String(MOTOR_SPEED, 4));
}

double getDistance(int motorId) {
  return steppers[motorId]->distanceToGo();
}

void sendPositions() {
    for (int i = 0; i < NB_STEPPERS; i++) { positions[i] = getAngleDeg(i, steppers[i]->currentPosition()); } // Convert the positions in step to radians and store it
    Serial.println(dataToSend()); // Then send all the positions to the arm
}

long getSteps(int motorId, double deg) {
    return (gearbox_reductions[motorId] * micro_steps[motorId] * deg) / deg_per_step[motorId];
}

double getAngleDeg(int motorId, long steps) {
    return (steps * deg_per_step[motorId]) / (gearbox_reductions[motorId] * micro_steps[motorId]);
}

bool isPressed(int switchPin) {
    return digitalRead(limit_switch_pins[switchPin]) == SWITCH_PRESS_STATE;
}
