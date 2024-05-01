#include <Arduino.h>
#include <IRremote.h>

/// This script was made in Visual Studio Code using the open-source PlatformIO framework
/// I used the "IRremote" library to assist in parsing the IR recievers input
/// The right motors are turntable rotation, the left motors are for elevation

const int TOP_ENDSTOP = 4; // The top endstop for elevation
const int BOTTOM_ENDSTOP = 5; // The bottom endstop for elevation
const int LEFT_ENDSTOP = 2; // The left endstop for the turntable
const int RIGHT_ENDSTOP = 3; // The right endstop for the turntable

const int RSPD = 6; // Speed pin for {} motors --- ENB
const int RCCW = 8; // CCW pin for right motors --- IN4
const int RCW = 7; // CW pin for right motors --- IN3
const int LCW = 11; // CW pin for left motors --- IN2
const int LCCW = 12; // CCW pin for left motors --- IN1
const int LSPD = 9; // Speed pin for {} motors --- ENA

const int IR_RECIEVER = 10; // Data pin for the IR receiver

const int ELEVATION_SPEED = 100; // The motor speed for raising and lowering the elevation
const int ROTATIONAL_SPEED = 200; // The motor speed for the turntable

// Using a boolean for the direction rotates the platform either clockwise (True) or counter-clockwise (False).
void rotate_turntable(bool direction)
{
  if (direction) { // Rotates the turntable clockwise
    digitalWrite(RCW, HIGH); // Start rotating the motor clockwise
    while (digitalRead(LEFT_ENDSTOP) == 0 && digitalRead(RIGHT_ENDSTOP) == 0) {} // Wait until the left and right endstops are off
    while (digitalRead(LEFT_ENDSTOP) == 1 && digitalRead(RIGHT_ENDSTOP) == 1) {} // Once an endstop is activated
    digitalWrite(RCW, LOW); // Stop rotating the motor
  } else { // CCW
    digitalWrite(RCCW, HIGH); // Start rotating the motor counter-clockwise
    while (digitalRead(LEFT_ENDSTOP) == 0 && digitalRead(RIGHT_ENDSTOP) == 0) {} // Wait until the left and right endstops are off
    while (digitalRead(LEFT_ENDSTOP) == 1 && digitalRead(RIGHT_ENDSTOP) == 1) {} // Once an endstop is activated
    digitalWrite(RCCW, LOW); // Stop rotating the motor
  }
}


// Using a boolean either raises (True) or lowers (False) the platform.
void elevate_turntable(bool direction)
{
  if (direction) { // Raises the platform
    if (digitalRead(TOP_ENDSTOP) == 1) { // If the top endstop isn't active
      digitalWrite(LCW, HIGH); // Turn on the elevation motors
      while (digitalRead(TOP_ENDSTOP) == 1) {} // Once the endstop is active
      digitalWrite(LCW, LOW); // Turn off the motors
    }
  } else { // Lowers the platform
    if (digitalRead(BOTTOM_ENDSTOP) == 1) { // If the bottom endstop isn't active
      digitalWrite(LCCW, HIGH); // Turn on the elevation motors
      while (digitalRead(BOTTOM_ENDSTOP) == 1) {} // Once the endstop is active
      digitalWrite(LCCW, LOW); // Turn off the motors
    }
  }
}

// False is lowered, true is raised
boolean elevation_status = false;

void reset_mechanisms()
{
  // Set the speeds for the elevation and rotation
  analogWrite(LSPD, ELEVATION_SPEED);
  analogWrite(RSPD, ROTATIONAL_SPEED);

  // Lower the platform
  digitalWrite(LCCW, HIGH);
  while (digitalRead(BOTTOM_ENDSTOP) == 1) {}
  digitalWrite(LCCW, LOW);
  
  // Rotates the platform until the opening is facing forward
  digitalWrite(RCW, HIGH);
  while (digitalRead(RIGHT_ENDSTOP) == 1 && digitalRead(LEFT_ENDSTOP) == 1) {}
  digitalWrite(RCW, LOW);

  // Updates the elevation status to lowered informing the 
  elevation_status = false;
}


// Runs at the start of the script.
void setup()
{

  // Sets each pin to be either input or output 
  // INPUT_PULLUP activates the pin's built in resistor, this simplifies the setup for endstops and other buttons
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

  reset_mechanisms();

  // Enables the IR reciever
  IrReceiver.begin(IR_RECIEVER, DISABLE_LED_FEEDBACK);

}


// Loops until either an emergency stop or power loss.
// Waits for user input then calls 'check_input()' to select the correct action to be applied. ///// The IR sensor code will be here 
void loop()
{
  if (IrReceiver.decode()) {
    switch(IrReceiver.decodedIRData.decodedRawData)
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
      case 0xE718FF00: // Up
        elevate_turntable(true);
        elevation_status = true;
        break;

      case 0xF708FF00: // Left
        rotate_turntable(true);
        break;
      case 0xE31CFF00: // Ok
        reset_mechanisms();
        break;

      case 0xA55AFF00: // Right
        rotate_turntable(false); 
        break;

      case 0xAD52FF00: // Down
        elevate_turntable(false);
        elevation_status = false;
        break;
    }
    IrReceiver.resume();
  }
}