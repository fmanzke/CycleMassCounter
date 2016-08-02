////////////////////////////////////////////////////////////////////////////////
//
// CycleMassCounter
//
//  counting bicycles during events like critical mass
//  kick-off at cyclehack berlin 2016 http://cyclehackberlin.de/
//  inspired by http://www.cyclehack.com/catalogue/happy-counter/
//
//  Hardware: Arduino, pressure sensor MPX5010DP, silicone rubber tube, zip tie
//
//  https://github.com/fmanzke/CycleMassCounter
//
//  this module counts bicycles
//
////////////////////////////////////////////////////////////////////////////////

#include "Arduino.h"

// Define the number of samples to keep track of.  The higher the number,
// the more the readings will be smoothed, but the slower the output will
// respond to the input.  Using a constant rather than a normal variable lets
// use this value to determine the size of the readings array.
#define numPressureReadings 3
int pressureReadings[numPressureReadings];      // the readings from the analog input
int pressureReadIndex = 0;              // the index of the current reading
unsigned long pressureTotal = 0;                  // the running total
int pressureAverage = 0;                // the average
unsigned long pressureDuration = 0;
unsigned long pressureStart = 0;

int pressureValue = 0;

#define numLimitReadings 256
int limitReadings[numLimitReadings];      // the readings from the analog input
int limitReadIndex = 0;              // the index of the current reading
unsigned long limitTotal = 0;                  // the running total
int limitAverage = 0;                // the average value with no pressure applied
unsigned int adjustLimit = 0;

float factor = 0.15;            // factor above which pressure is counted

int count = 0;                  // number of pressure impulses
boolean counted = false;        // flag to avoid double counting of single pressure impulses

int inputPin = A0;              // input pin of MPX5010DP pressure sensor
int vccPin = 3;                 // using pin HIGH as supply voltage for pressure sensor
#define ledPin 13               // led pin, using internal led on pin 13

void setup() {
  //indicates start of setup
  digitalWrite(ledPin, HIGH);
  pinMode(vccPin, OUTPUT);
  digitalWrite(vccPin, HIGH);

  // initialize serial communication with computer:
  Serial.begin(115200);

  //initialize ambient pressure value
  for (int thisReading = 0; thisReading < numLimitReadings; thisReading++) {
    limitReadings[thisReading] =  analogRead(inputPin);
    limitTotal += limitReadings[thisReading];
  }
  limitAverage = limitTotal / numLimitReadings;


  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numPressureReadings; thisReading++) {
    pressureReadings[thisReading] = 0;
  }

  //indicates end of setup
  digitalWrite(ledPin, LOW);
}

void loop() {

  //show pressure impulses with led as fallback
  digitalWrite(ledPin, counted ? HIGH : LOW);

  int pressureReading = analogRead(inputPin);

  // subtract the last reading:
  pressureTotal = pressureTotal - pressureReadings[pressureReadIndex];
  // read from the sensor:
  pressureReadings[pressureReadIndex] = pressureReading;
  // add the reading to the total:
  pressureTotal = pressureTotal + pressureReadings[pressureReadIndex];
  // advance to the next position in the array:
  pressureReadIndex = pressureReadIndex + 1;
  // if we're at the end of the array...
  if (pressureReadIndex >= numPressureReadings) {
    // ...wrap around to the beginning:
    pressureReadIndex = 0;
  }
  // calculate the average:
  pressureAverage = pressureTotal / numPressureReadings;


  if (adjustLimit % 10 == 0)
  {
    //adjust limit over time
    // subtract the last reading:
    limitTotal = limitTotal - limitReadings[limitReadIndex];
    // read from the sensor:
    limitReadings[limitReadIndex] = pressureReading;
    // add the reading to the total:
    limitTotal = limitTotal + limitReadings[limitReadIndex];
    // advance to the next position in the array:
    limitReadIndex = limitReadIndex + 1;
    // if we're at the end of the array...
    if (limitReadIndex >= numLimitReadings) {
      // ...wrap around to the beginning:
      limitReadIndex = 0;
    }
    // calculate the average:
    limitAverage = limitTotal / numLimitReadings;
  }
  adjustLimit++;

  // pressure detection
  if (pressureAverage  >  limitAverage + limitAverage * factor)
  {
    if (!counted)
    {
      pressureStart = millis();
      pressureValue = max(pressureValue, pressureAverage);
      counted = true;
      count++;
    }
  }
  else
  {
    if (counted)
    {
      pressureDuration = millis() - pressureStart;

      counted = false;
      /*
            Serial.print("pressureDuration=");
            Serial.print(pressureDuration);
            Serial.print("\tpressureValue=");
            Serial.print(pressureValue);
            Serial.println();
      */
      //reset pressureValue
      pressureValue = 0;
      //reset pressureDuration
      pressureDuration = 0;
    }
  }
  // Serial output for using with serial plotter
  Serial.print("0,");
  Serial.print(limitAverage);
  Serial.print(",");
  Serial.print(limitAverage + limitAverage * factor);
  Serial.print(",");
  Serial.print(pressureAverage);
  Serial.println();
  delay(1); // delay in between reads for stability
}
