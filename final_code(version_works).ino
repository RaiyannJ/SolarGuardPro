// color sensor headers
#include <Wire.h>
#include "Adafruit_AS726x.h"

#define SENSOR_MAX 5000

// constants for color sensor
//create the object
Adafruit_AS726x ams;

//buffer to hold calibrated values
float calibratedValues[AS726x_NUM_CHANNELS];


//Constants for photoresistors
const int pResistor_outside = A2;  // Photoresistor at Arduino analog pin A2
const int pResistor_solar_panel = A3; // Photoresistor at Arduino analog pin A3
const int ledPin=9;       // Led pin at Arduino pin 9

// Constants for motors
// Define pin connections & motor's steps per revolution
const int dirPin = 3;
const int stepPin = 2;
const int stepsPerRevolution = 400;
const int num_Revolution = 1;

//Variables for photoresistor
int value_solar_panel;          // Store value_solar_panel from photoresistor (0-1023)
int value_outside;

void setup(){
   // Motor setup
  // Declare pins as Outputs
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  // photoresistor setup
  pinMode(ledPin, OUTPUT);  // Set lepPin - 9 pin as an output
  pinMode(pResistor_solar_panel, INPUT);// Set pResistor_solar_panel - A0 pin as an input (optional)
  pinMode(pResistor_outside, INPUT);// Set pResistor_solar_panel - A0 pin as an input (optional)

  // color sensor setup
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  //begin and make sure we can talk to the sensor
  if (!ams.begin())
  {
    Serial.println("could not connect to sensor! Please check your wiring.");
    while (1)
      ;
  }
  ams.setConversionType(MODE_2);

    
  Serial.begin(9600);
}

float ColorSensor() {
  float average = 0;
  if (ams.dataReady()) {
    ams.readCalibratedValues(calibratedValues);

    // Calculate the sum of all calibrated values
    float sum = 0;
    for (int i = 0; i < AS726x_NUM_CHANNELS; i++) {
      sum += calibratedValues[i];
    }
    
    // Calculate the average
    average = sum / AS726x_NUM_CHANNELS;
    Serial.print("Average calibrated color value: ");
    Serial.println(average);
  }  
  return average;
}

void MotorMove() {
  bool blackDetected = false;
  
  // Spin the motor until black is detected
  for(int x = 0;!blackDetected; x++){
    if(ColorSensor() > 0) {
      // Black not detected, keep moving
      Serial.println("Moving motor main: ");
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500); // Slow for visibility, adjust as needed
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    } else {
      // Black detected, set the flag
      blackDetected = true;
    }
  }
  
}


void loop(){
  value_solar_panel = analogRead(pResistor_solar_panel);
  value_outside = analogRead(pResistor_outside);
  Serial.println("----- loop start --------------");
  Serial.print("value_outside: ");
  Serial.println(value_outside);
  
  Serial.print("value_solar_panel: ");
  Serial.println(value_solar_panel);

  Serial.print("Color value average: ");
  Serial.println(ColorSensor());
  //You can change value_solar_panel "25"
  if (value_solar_panel > 0.80*value_outside){
    digitalWrite(ledPin, LOW);  //Turn led off
    Serial.print("greater than 80% of light outside: ");
    Serial.println(0.80*value_outside);
  }
  else{
    digitalWrite(ledPin, HIGH); //Turn led on
    Serial.print("less than 80% of light outside: ");
    Serial.println(0.80*value_outside);
    MotorMove();

    Serial.println("Black detected, moving a bit more...");
    for(int x = 0; x < 250; x++) { // Move 20 steps more, adjust as needed
      Serial.print("Motor step moving offset ......... ");
      Serial.println(x);
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(500);
    }

    
  }
  delay(1000); //Small delay
}

// ------------------------------------------------------
