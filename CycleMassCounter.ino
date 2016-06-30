// Define the number of samples to keep track of.  The higher the number,
// the more the readings will be smoothed, but the slower the output will
// respond to the input.  Using a constant rather than a normal variable lets
// use this value to determine the size of the readings array.
const int numReadings = 5;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average

int limit = 0;                  // the analog value with no pressure applied
float factor = 0.15;            // factor above which pressure is counted

int count = 0;                  // number of pressure impulses
boolean counted = false;        // flag to avoid double counting of single pressure impulses

int inputPin = A0;              // input pin of MPX5010DP pressure sensor
int vccPin = 3;                 // using pin HIGH as supply voltage for pressure sensor
#define ledPin 13               // led pin, using internal led on pin 13

void setup() {
  // initialize serial communication with computer:
  Serial.begin(115200);

  pinMode(vccPin, OUTPUT);
  digitalWrite(vccPin, HIGH);

  //initialize ambient pressure value
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    limit += analogRead(inputPin);
  }
  limit = limit / numReadings;

  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
}

void loop() {

  //show pressure impulses with led as fallback
  digitalWrite(ledPin, counted ? HIGH : LOW);

  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = analogRead(inputPin);
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }
  // calculate the average:
  average = total / numReadings;


  if (average  >  limit + limit * factor)
  {
    if (!counted)
    {
      counted = true;
      count++;
    }
  }
  else
  {
    counted = false;
  }
  // Serial output for using with serial plotter
  Serial.print("0,");
  Serial.print(limit);
  Serial.print(",");
  Serial.print(average);
  Serial.print(",");
  Serial.print(count);
  Serial.println();
  delay(1); // delay in between reads for stability
}
