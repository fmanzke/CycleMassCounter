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
//  this module displays the number of counted bicycles
//
////////////////////////////////////////////////////////////////////////////////

#include "Arduino.h"
#include "FastLED.h"

#define NUM_MODULES 3 // Number of modules
#define NUM_SEGMENTS 7 // Number of segments
#define NUM_LED_PER_SEGMENT 3 // Number of leds per segment

#define NUM_LEDS ( NUM_MODULES * NUM_SEGMENTS * NUM_LED_PER_SEGMENT ) // Number of LEDs

// Define color order for your strip
#define COLOR_ORDER BGR
// Data pin that led data will be written out over
#define DATA_PIN 11
// Clock pin only needed for SPI based chipsets when not using hardware SPI
#define CLOCK_PIN 12

//overall brightness from 0 (off) to 255
#define BRIGHTNESS 2

//delay for next number
#define DELAY 500

CRGB ledColorOn = CRGB::Green; // Color used for leds on
CRGB ledColorOff = CRGB::Black; // Color used for leds off

// This is an array of leds.  One item for each led in your strip.
CRGB leds[NUM_LEDS];

byte digits[10][7] =
{{0, 1, 1, 1, 1, 1, 1},  // Digit 0
 {0, 0, 0, 1, 1, 0, 0},  // Digit 1
 {1, 0, 1, 1, 0, 1, 1},  // Digit 2
 {1, 0, 1, 1, 1, 1, 0},  // Digit 3
 {1, 1, 0, 1, 1, 0, 0},  // Digit 4
 {1, 1, 1, 0, 1, 1, 0},  // Digit 5
 {1, 1, 1, 0, 1, 1, 1},  // Digit 6
 {0, 0, 1, 1, 1, 0, 0},  // Digit 7
 {1, 1, 1, 1, 1, 1, 1},  // Digit 8
 {1, 1, 1, 1, 1, 1, 0}}; // Digit 9 | 2D Array for numbers on 7 segment
int clockToValue(int hour, int minute)
{
        return hour * 100 + minute;
}

// Convert time to array needet for display
void valueToArray(int value) {
        int cursor = 0;
        for (int m = 0; m < NUM_MODULES; m++) {
                int digit = value % 10; // get last digit in value
                for (int s = 0; s < NUM_SEGMENTS; s++) {
                        for (int l = 0; l < NUM_LED_PER_SEGMENT; l++) {
                                if (digits[digit][s] == 1) {
                                        leds[cursor] = ledColorOn;
                                }
                                else if (digits[digit][s] == 0) {
                                        leds[cursor] = ledColorOff;
                                };
                                cursor++;
                        };
                };
                value /= 10;
        };
};

void loop()  // Main loop
{
        for (int v = 0; v <= 999; v++)
        {
                valueToArray(v);
                FastLED.show(); // Display leds array
                // Wait a little bit
                delay(DELAY);
        }
}

void singleLight(){
        // Move a single led
        for (int ledIndex = 0; ledIndex < NUM_LEDS; ledIndex++) {
                // Turn our current led on to white, then show the leds
                leds[ledIndex] = ledColorOn;
                // Show the leds (only one of which is set to white, from above)
                FastLED.show();
                // Wait a little bit
                delay(10);
                // Turn our current led back to black for the next loop around
                leds[ledIndex] = ledColorOff;
        }
}

void setup() {
        Serial.begin(115200);
        FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, COLOR_ORDER>(leds, NUM_LEDS);
        FastLED.setBrightness(BRIGHTNESS); // Set initial brightness
        singleLight();
}
