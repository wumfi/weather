#include <FastLED.h>

// How many leds in your strip?
#define NUM_LEDS 10

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
#define DATA_PIN D5

// Define the array of leds
CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  Pulse(1);
  Pulse(2);
  Pulse(3);
  Pulse(4);
}

void Alloff() {
  int ledctr;
  for(ledctr=0;ledctr<10;ledctr++) {
    leds[ledctr]=CRGB::Black;
    FastLED.show();
  }
}
void Pulse(int col) {
  int ledctr;
  int fadectr;
  for(ledctr=0;ledctr<10;ledctr++) {
    switch(col) {
      case 1:
        leds[ledctr]=CRGB::Red;
        break;
      case 2:
        leds[ledctr]=CRGB::Cyan;
        break;
      case 3:
        leds[ledctr]=CRGB::Green;
        break;
      case 4:
        leds[ledctr]=CRGB::White;
        break;
    }
  }
  for(fadectr=0;fadectr<256;fadectr++) {
    FastLED.setBrightness(fadectr);
    FastLED.show();
    delay(2);
  }
  for(fadectr=255;fadectr>0;fadectr--) {
    FastLED.setBrightness(fadectr);
    FastLED.show();
    delay(2);
  }
}

void loop() {
Alloff();
}
