#include <Arduino.h>
#include <IRremote.h>

/// This script was made in Visual Studio Code using the open-source PlatformIO framework.
/// I used the "IRremote" library to assist in parsing the IR receivers input.
/// The right motors are turntable rotation, the left motors are for elevation.

/// The * key on the remote activates an emergency stop, in a full scale model this estop would be
/// both digital and physical with a button mounted in the garage area, beside the road area and on the
/// elevator but due to the size constraints it would be difficult to implement this without ruining the aesthetics.

/// The arduino sets a pins output and continues running, because of this the while loops after setting a pin wait
/// until the statement returns true before turning off again.   These while loops allow me to implement an emergency
/// stop using the following code in the while loops which would otherwise be empty.
/// {if (IrReceiver.decode()) {if (IrReceiver.decodedIRData.decodedRawData == '0xE916FF00') {emergency_stop();} IrReceiver.resume();}}

// Unused pins: 16, 17, 18

const int RIGHT_TOP_ENDSTOP = 4;    // The top endstop for the right elevation.
const int RIGHT_BOTTOM_ENDSTOP = 5; // The bottom endstop for the right elevation.
//const int LEFT_TOP_ENDSTOP = 14;    // The top endstop for the left elevation
//const int LEFT_BOTTOM_ENDSTOP = 15; // The bottom endstop for the left elevation

const int LEFT_ENDSTOP = 10; // The left endstop for the turntable.
const int RIGHT_ENDSTOP = 3; // The right endstop for the turntable.

const int RIGHT_ELEVATOR_SPEED = 6; // Speed pin for right motors. --- ENB
const int TURNTABLE_CCW = 8;   // CCW pin for the right elevator motors. --- IN4
const int TURNTABLE_CW = 7;    // CW pin for the right elevator motors. --- IN3
const int LEFT_ELEVATOR_CW = 11;    // CW pin for left elevator motors. --- IN2
const int LEFT_ELEVATOR_CCW = 12;   // CCW pin for left elevator motors. --- IN1
const int LEFT_ELEVATOR_SPEED = 9;  // Speed pin for left motors. --- ENA

const int RIGHT_ELEVATOR_CW = 0;  // CW pin for the turntable motor
const int RIGHT_ELEVATOR_CCW = 1; // CCW pin for the turntable motor

const int IR_RECEIVER = 10; // Data pin for the IR receiver.

const int RED_LED = 13; // Toggle pin for red LED strip.

const int ELEVATION_SPEED = 100;  // The motor speed for raising and lowering the elevation.
const int ROTATIONAL_SPEED = 200; // The motor speed for the turntable.

boolean elevation_status = false; // False is lowered, true is raised.

uint32_t light_toggle_delay = 0; // This variable stores the time that the next light toggle should be at




// While any motor is rotating flash red warning lights, if the IR receiver has been given an
// emergency stop signal run the emergency stop function.
void motor_check()
{
  uint32_t start_time = millis(); // Saves the start time as a variable

  if (light_toggle_delay >= start_time) // If the time stored in the light toggle delay is either equal to or after the start time
  {
    digitalWrite(RED_LED, !digitalRead(RED_LED)); // Toggle the red LED strip

    light_toggle_delay = start_time + 1500; // And set the light toggle delay to the start time + 1500
  }

  if (IrReceiver.decode()) // If the IR receiver has a new value
  {
    if (IrReceiver.decodedIRData.decodedRawData == 0xE916FF00) // Check if that value is equal to the emergency stop button
    {
      emergency_stop(); // Then run the emergency stop function
    }
    IrReceiver.resume(); // If the emergency stop wasn't run, resume IR receiver scanning
  }
}




// Disables every pin and freezes the program until a hard restart.
// A hard restart is either pressing the button on the top left corner or removing power temporarily.
void emergency_stop()
{
  for (int pin = 0; pin < 20; pin++) // Loop once for every pin on the board.
  {
    digitalWrite(pin, LOW); // Turn off the current pin
  }

  while (true) // Start an infinite loop, this effectively disabled the arduino uno
  {
  }
}




// Using a boolean for the direction rotates the platform either clockwise (True) or counter-clockwise (False).
void rotate_turntable(bool direction)
{
  if (elevation_status)
  {
    return;
  }
  if (direction)
  {                          // Rotates the turntable clockwise.
    digitalWrite(TURNTABLE_CW, HIGH); // Start rotating the motor clockwise.
    while (digitalRead(LEFT_ENDSTOP) == 0 && digitalRead(RIGHT_ENDSTOP) == 0)
    {
      motor_check();
    } // Wait until the left and right endstops are off
    while (digitalRead(LEFT_ENDSTOP) == 1 && digitalRead(RIGHT_ENDSTOP) == 1)
    {
      motor_check();
    }                       // Once an endstop is activated
    digitalWrite(TURNTABLE_CW, LOW); // Stop rotating the motor.
  }
  else
  {                           // CCW.
    digitalWrite(TURNTABLE_CCW, HIGH); // Start rotating the motor counter-clockwise.
    while (digitalRead(LEFT_ENDSTOP) == 0 && digitalRead(RIGHT_ENDSTOP) == 0)
    {
      motor_check();
    } // Wait until the left and right endstops are off
    while (digitalRead(LEFT_ENDSTOP) == 1 && digitalRead(RIGHT_ENDSTOP) == 1)
    {
      motor_check();
    }                        // Once an endstop is activated
    digitalWrite(TURNTABLE_CCW, LOW); // Stop rotating the motor.
  }

  digitalWrite(RED_LED, LOW);
}




// Using a boolean either raises (True) or lowers (False) the platform.
void elevate_turntable(bool direction)
{
  if (direction)
  { // Raises the platform.
    if (digitalRead(RIGHT_TOP_ENDSTOP) == 1)
    {                                        // If the top endstop isn't active.
      digitalWrite(LEFT_ELEVATOR_CW, HIGH);  // Turn on the elevation motors.
      digitalWrite(RIGHT_ELEVATOR_CW, HIGH); // Turn on the elevation motors.
      while (digitalRead(RIGHT_TOP_ENDSTOP) == 1)
      {
        motor_check();
      } // Once the endstop is active
      digitalWrite(LEFT_ELEVATOR_CW, LOW);  // Turn on the elevation motors.
      digitalWrite(RIGHT_ELEVATOR_CW, LOW); // Turn on the elevation motors.
    }
  }
  else
  { // Lowers the platform.
    if (digitalRead(RIGHT_BOTTOM_ENDSTOP) == 1)
    {                                         // If the bottom endstop isn't active.
      digitalWrite(LEFT_ELEVATOR_CCW, HIGH);  // Turn on the elevation motors.
      digitalWrite(RIGHT_ELEVATOR_CCW, HIGH); // Turn on the elevation motors.
      while (digitalRead(RIGHT_BOTTOM_ENDSTOP) == 1)
      {
        motor_check();
      } // Once the endstop is active
      digitalWrite(LEFT_ELEVATOR_CCW, LOW);  // Turn on the elevation motors.
      digitalWrite(RIGHT_ELEVATOR_CCW, LOW); // Turn on the elevation motors.
    }
  }
  digitalWrite(RED_LED, !digitalRead(RED_LED));
}

// Moves the mechanisms into their default position
void reset_mechanisms()
{
  // Set the speeds for the elevation and rotation.
  analogWrite(LEFT_ELEVATOR_SPEED, ELEVATION_SPEED);
  analogWrite(RIGHT_ELEVATOR_SPEED, ROTATIONAL_SPEED);

  // Lower the platform.
  digitalWrite(LEFT_ELEVATOR_CCW, HIGH);
  digitalWrite(RIGHT_ELEVATOR_CCW, HIGH);
  while (digitalRead(RIGHT_BOTTOM_ENDSTOP) == 1)
  {
    motor_check();
  }
  digitalWrite(LEFT_ELEVATOR_CCW, LOW);
  digitalWrite(RIGHT_ELEVATOR_CCW, LOW);

  // Rotates the platform until the opening is facing forward.
  digitalWrite(TURNTABLE_CW, HIGH);
  while (digitalRead(RIGHT_ENDSTOP) == 1 && digitalRead(LEFT_ENDSTOP) == 1)
  {
    motor_check();
  }
  digitalWrite(TURNTABLE_CCW, LOW);

  // Updates the elevation status to lowered.
  elevation_status = false;
  digitalWrite(RED_LED, !digitalRead(RED_LED));
}

// Runs at the start of the script.
void setup()
{
  // Sets each pin to be either input or output.
  // INPUT_PULLUP activates the pin's built in resistor which
  // removes the need for an external resistor on buttons.
  pinMode(LEFT_ELEVATOR_SPEED, OUTPUT);
  pinMode(LEFT_ELEVATOR_CW, OUTPUT);
  pinMode(LEFT_ELEVATOR_CCW, OUTPUT);
  pinMode(RIGHT_ELEVATOR_CW, OUTPUT);
  pinMode(RIGHT_ELEVATOR_CCW, OUTPUT);
  pinMode(RIGHT_ELEVATOR_SPEED, OUTPUT);

  pinMode(RED_LED, OUTPUT);
  pinMode(IR_RECEIVER, INPUT);

  pinMode(TURNTABLE_CW, OUTPUT);
  pinMode(TURNTABLE_CCW, OUTPUT);

  //pinMode(LEFT_TOP_ENDSTOP, INPUT_PULLUP);
  //pinMode(LEFT_BOTTOM_ENDSTOP, INPUT_PULLUP);
  pinMode(RIGHT_TOP_ENDSTOP, INPUT_PULLUP);
  pinMode(RIGHT_BOTTOM_ENDSTOP, INPUT_PULLUP);

  pinMode(LEFT_ENDSTOP, INPUT_PULLUP);
  pinMode(RIGHT_ENDSTOP, INPUT_PULLUP);

  // Resets all mechanisms to the default position.
  reset_mechanisms();

  // Enables the IR receiver.
  IrReceiver.begin(IR_RECEIVER, DISABLE_LED_FEEDBACK);
}

// Loops until either an emergency stop or power loss.
// Waits for input from the IR Receiver then uses a
// switch-case statement to select the appropriate action.
void loop()
{
  if (IrReceiver.decode())
  {                                                  // Decode IR receiver input and pause receiving (When succesful returns true and continues running the code)
    switch (IrReceiver.decodedIRData.decodedRawData) // Runs the code associated with the decoded value.
    {
    // case 0xBA45FF00: // 1
    // case 0xB946FF00: // 2
    // case 0xB847FF00: // 3
    // case 0xBB44FF00: // 4
    // case 0xBF40FF00: // 5
    // case 0xBC43FF00: // 6
    // case 0xF807FF00: // 7
    // case 0xEA15FF00: // 8
    // case 0xF609FF00: // 9
    case 0xE916FF00: // *
      emergency_stop();
    // case 0xE619FF00: // 0
    // case 0xF20DFF00: // #
    case 0xE718FF00:           // Up.
      elevate_turntable(true); // Raise platform.
      elevation_status = true;
      break;

    case 0xF708FF00:          // Left.
      rotate_turntable(true); // Rotate counter-clockwise.
      break;

    case 0xE31CFF00:      // Ok.
      reset_mechanisms(); // Reset the mechanisms.
      break;

    case 0xA55AFF00:           // Right.
      rotate_turntable(false); // Rotate clockwise
      break;

    case 0xAD52FF00:            // Down
      elevate_turntable(false); // Lower platform.
      elevation_status = false;
      break;
    }

    IrReceiver.resume(); // Resumes IR receiver readings
  }
}