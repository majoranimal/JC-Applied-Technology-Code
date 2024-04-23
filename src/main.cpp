#include <Arduino.h>
#include <IRremote.h>

/// This script was made in Visual Studio Code using the open-source PlatformIO framework
/// I used the "IRremote" library  
/// The right motors are turntable rotation, the left motors are for elevation

const int TOP_ENDSTOP = 0; // The top endstop for elevation
const int BOTTOM_ENDSTOP = 1; // The bottom endstop for elevation
const int LEFT_ENDSTOP = 2; // The left endstop for the turntable
const int RIGHT_ENDSTOP = 3; // The right endstop for the turntable

const int RSPD = 6; // Speed pin for {} motors --- ENB
const int RCCW = 8; // CCW pin for right motors --- IN4
const int RCW = 7; // CW pin for right motors --- IN3
const int LCW = 11; // CW pin for left motors --- IN2
const int LCCW = 12; // CCW pin for left motors --- IN1
const int LSPD = 9; // Speed pin for {} motors --- ENA

const int ELEVATION_SPEED = 100; // The motor speed for raising and lowering the elevation
const int ROTATIONAL_SPEED = 100; // The motor speed for the turntable


/// Stops all motors and freezes the program until a hard restart.
void emergency_stop() {
  // Sets the all motors to zero speed
  analogWrite(LSPD, LOW);
  analogWrite(RSPD, LOW);

  // Turns off every motor
  digitalWrite(LCW, LOW);
  digitalWrite(LCCW, LOW);
  digitalWrite(RCW, LOW); 
  digitalWrite(RCCW, LOW); 

  while (true) {} // Infinitley loops effectively pausing the program indefinitley
}


// Using a boolean for the direction rotates the platform either clockwise (True) or counter-clockwise (False).
void rotate_turntable(bool direction)
{
  if (direction) { // Rotates the turntable clockwise
    digitalWrite(RCW, HIGH); // Start rotating the motor clockwise
    while (digitalRead(LEFT_ENDSTOP) == 0 or digitalRead(RIGHT_ENDSTOP == 0)) {} // Wait until the left and right endstops are off
    while (digitalRead(LEFT_ENDSTOP) == 1 or digitalRead(RIGHT_ENDSTOP == 1)) {} // Once an endstop is activated
    digitalWrite(RCW, LOW); // Stop rotating the motor
  } else { // CCW
    digitalWrite(RCCW, HIGH); // Start rotating the motor counter-clockwise
    while (digitalRead(LEFT_ENDSTOP) == 0 or digitalRead(RIGHT_ENDSTOP == 0)) {} // Wait until the left and right endstops are off
    while (digitalRead(LEFT_ENDSTOP) == 1 or digitalRead(RIGHT_ENDSTOP == 1)) {} // Once an endstop is activated
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


// Reads the user input to parse the requested action. ///// The values are placeholders for the IR sensor
void check_input(String input)
{
  if (input == "l") { // Rotates the platform counter-clockwise
    rotate_turntable(false);
  } else if (input == "r") { // Rotates the platform clockwise
    rotate_turntable(true);
  } else if (input = "u") { // Raises the platform
    elevate_turntable(true);
  } else if (input = "d") { // Lowers the platform
    elevate_turntable(false);
  } else if (input == "o") { // Emergency stop
    emergency_stop();
  } else { // If the command is invalid print an error to serial
    Serial.println("Error");
  }
}

// Runs at the start of the script.
// Creates a serial connection and moves the platform into the starting position.
void setup()
{
  // All serial commands were used for testing and debugging
  Serial.begin(9600);

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

  analogWrite(LSPD, ELEVATION_SPEED);
  analogWrite(RSPD, ROTATIONAL_SPEED);

  // Lower the platform
  digitalWrite(LCCW, HIGH);
  while (digitalRead(BOTTOM_ENDSTOP) == 1) {}
  digitalWrite(LCCW, LOW);
  
  // Rotates the platform until the opening is facing forward
  digitalWrite(RCW, HIGH);
  while (digitalRead(RIGHT_ENDSTOP) == 1 or digitalRead(LEFT_ENDSTOP) == 1) {}
  digitalWrite(RCW, LOW);

  Serial.println("Initialised");
}


// Loops until either an emergency stop or power loss.
// Waits for user input then calls "check_input()" to select the correct action to be applied. ///// The IR sensor code will be here 
void loop()
{
  Serial.println("L,R,U,D,O: ");

  while (Serial.available() == 0) {}

  check_input(Serial.readString());
}