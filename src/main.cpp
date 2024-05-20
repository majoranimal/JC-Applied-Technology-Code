#include <Arduino.h>
#include <IRremote.h>

/// This script was made in Visual Studio Code using the open-source PlatformIO framework.
/// I used the "IRremote" library to assist in parsing the IR recievers input.
/// The right motors are turntable rotation, the left motors are for elevation.

/// The * key on the remote activates an emergency stop, in a full scale model this estop would be
/// both digital and physical with a button mounted in the garage area, beside the road area and on the
/// elevator but due to the size constraints it would be difficult to implement this without ruining the aesthetics.

/// The arduino sets a pins output and continues running, because of this the while loops after setting a pin wait
/// until the statement returns true before turning off again.   These while loops allow me to implement an emergency
/// stop using the following code in the while loops which would otherwise be empty.
/// {if (IrReceiver.decode()) {if (IrReceiver.decodedIRData.decodedRawData == '0xE916FF00') {emergency_stop();} IrReceiver.resume();}}
// Free pins: 1, 2, 13

const int TOP_ENDSTOP = 4;    // The top endstop for elevation.
const int BOTTOM_ENDSTOP = 5; // The bottom endstop for elevation.
const int LEFT_ENDSTOP = 10;   // The left endstop for the turntable.
const int RIGHT_ENDSTOP = 3;  // The right endstop for the turntable.

const int RSPD = 6;  // Speed pin for right motors. --- ENB
const int RCCW = 8;  // CCW pin for right motors. --- IN4
const int RCW = 7;   // CW pin for right motors. --- IN3
const int LCW = 11;  // CW pin for left motors. --- IN2
const int LCCW = 12; // CCW pin for left motors. --- IN1
const int LSPD = 9;  // Speed pin for left motors. --- ENA

const int IR_RECIEVER = 10; // Data pin for the IR receiver.

const int RED_LED = 13; // Toggle pin for red LED strip.

const int ELEVATION_SPEED = 100;  // The motor speed for raising and lowering the elevation.
const int ROTATIONAL_SPEED = 200; // The motor speed for the turntable.

boolean elevation_status = false; // False is lowered, true is raised.

// Disables every pin and freezes the program until a hard restart.
// A hard restart is either pressing the button on the top left corner or removing power temporarily.
void emergency_stop()
{
  for (int pin = 0; pin < 20; pin++)
  {                         // Loops once for every pin on the board.
    digitalWrite(pin, LOW); // Turns off "pin" (Effectively disabling every pin).
  }

  while (true)
  { // Loops the code until restarted.
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
    digitalWrite(RCW, HIGH); // Start rotating the motor clockwise.
    while (digitalRead(LEFT_ENDSTOP) == 0 && digitalRead(RIGHT_ENDSTOP) == 0)
    {
      if (IrReceiver.decode())
      {
        if (IrReceiver.decodedIRData.decodedRawData == 0xE916FF00)
        {
          emergency_stop();
        }
        IrReceiver.resume();
      }
    } // Wait until the left and right endstops are off
    while (digitalRead(LEFT_ENDSTOP) == 1 && digitalRead(RIGHT_ENDSTOP) == 1)
    {
      if (IrReceiver.decode())
      {
        if (IrReceiver.decodedIRData.decodedRawData == 0xE916FF00)
        {
          emergency_stop();
        }
        IrReceiver.resume();
      }
    }                       // Once an endstop is activated
    digitalWrite(RCW, LOW); // Stop rotating the motor.
  }
  else
  {                           // CCW.
    digitalWrite(RCCW, HIGH); // Start rotating the motor counter-clockwise.
    while (digitalRead(LEFT_ENDSTOP) == 0 && digitalRead(RIGHT_ENDSTOP) == 0)
    {
      if (IrReceiver.decode())
      {
        if (IrReceiver.decodedIRData.decodedRawData == 0xE916FF00)
        {
          emergency_stop();
        }
        IrReceiver.resume();
      }
    } // Wait until the left and right endstops are off
    while (digitalRead(LEFT_ENDSTOP) == 1 && digitalRead(RIGHT_ENDSTOP) == 1)
    {
      if (IrReceiver.decode())
      {
        if (IrReceiver.decodedIRData.decodedRawData == 0xE916FF00)
        {
          emergency_stop();
        }
        IrReceiver.resume();
      }
    }                        // Once an endstop is activated
    digitalWrite(RCCW, LOW); // Stop rotating the motor.
  }
}

// Using a boolean either raises (True) or lowers (False) the platform.
void elevate_turntable(bool direction)
{
  if (direction)
  { // Raises the platform.
    if (digitalRead(TOP_ENDSTOP) == 1)
    {                          // If the top endstop isn't active.
      digitalWrite(LCW, HIGH); // Turn on the elevation motors.
      while (digitalRead(TOP_ENDSTOP) == 1)
      {
        if (IrReceiver.decode())
        {
          if (IrReceiver.decodedIRData.decodedRawData == 0xE916FF00)
          {
            emergency_stop();
          }
          IrReceiver.resume();
        }
      }                       // Once the endstop is active
      digitalWrite(LCW, LOW); // Turn off the motors.
    }
  }
  else
  { // Lowers the platform.
    if (digitalRead(BOTTOM_ENDSTOP) == 1)
    {                           // If the bottom endstop isn't active.
      digitalWrite(LCCW, HIGH); // Turn on the elevation motors.
      while (digitalRead(BOTTOM_ENDSTOP) == 1)
      {
        if (IrReceiver.decode())
        {
          if (IrReceiver.decodedIRData.decodedRawData == 0xE916FF00)
          {
            emergency_stop();
          }
          IrReceiver.resume();
        }
      }                        // Once the endstop is active
      digitalWrite(LCCW, LOW); // Turn off the motors.
    }
  }
}

void reset_mechanisms()
{
  // Set the speeds for the elevation and rotation.
  analogWrite(LSPD, ELEVATION_SPEED);
  analogWrite(RSPD, ROTATIONAL_SPEED);

  // Lower the platform.
  digitalWrite(LCCW, HIGH);
  while (digitalRead(BOTTOM_ENDSTOP) == 1)
  {
    if (IrReceiver.decode())
    {
      if (IrReceiver.decodedIRData.decodedRawData == 0xE916FF00)
      {
        emergency_stop();
      }
      IrReceiver.resume();
    }
  }
  digitalWrite(LCCW, LOW);

  // Rotates the platform until the opening is facing forward.
  digitalWrite(RCW, HIGH);
  while (digitalRead(RIGHT_ENDSTOP) == 1 && digitalRead(LEFT_ENDSTOP) == 1)
  {
    if (IrReceiver.decode())
    {
      if (IrReceiver.decodedIRData.decodedRawData == 0xE916FF00)
      {
        emergency_stop();
      }
      IrReceiver.resume();
    }
  }
  digitalWrite(RCW, LOW);

  // Updates the elevation status to lowered.
  elevation_status = false;
}

// Runs at the start of the script.
void setup()
{
  // Sets each pin to be either input or output.
  // INPUT_PULLUP activates the pin's built in resistor which
  // simplifies the setup for endstops and other buttons.
  pinMode(LSPD, OUTPUT);
  pinMode(LCW, OUTPUT);
  pinMode(LCCW, OUTPUT);
  pinMode(RCW, OUTPUT);
  pinMode(RCCW, OUTPUT);
  pinMode(RSPD, OUTPUT);
  pinMode(TOP_ENDSTOP, INPUT_PULLUP);
  pinMode(BOTTOM_ENDSTOP, INPUT_PULLUP);
  pinMode(LEFT_ENDSTOP, INPUT_PULLUP);
  pinMode(RIGHT_ENDSTOP, INPUT_PULLUP);
  pinMode(RIGHT_ENDSTOP, INPUT_PULLUP);

  // Resets all mechanisms to the default position.
  reset_mechanisms();

  // Enables the IR reciever.
  IrReceiver.begin(IR_RECIEVER, DISABLE_LED_FEEDBACK);
}

// Loops until either an emergency stop or power loss.
// Waits for input from the IR Receiver then uses a
// switch-case statement to select the appropriate action.
void loop()
{
  if (IrReceiver.decode())
  {                                                  // Decode IR reciever input and pause receiving (When succesful returns true and continues running the code)
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
    // case 0xE916FF00: // *
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

    IrReceiver.resume(); // Resumes IR reciever readings
  }
}