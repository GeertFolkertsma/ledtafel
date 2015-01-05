#include <FastLED.h>
#include "LedTafelEffect.h"

#define ROWS 10
#define COLS 10
#define NUM_LEDS (ROWS*COLS)

// This is the output buffer that gets copied onto the table
CRGB leds[NUM_LEDS];

AllBlack zwart(leds, NUM_LEDS);
AllRed rood(leds, NUM_LEDS);

LedTafelEffect* current_effect;

void setup(){
  FastLED.addLeds<WS2801, RGB>(leds, NUM_LEDS);
  zwart.init();
  rood.init();
}

void loop(){
  if((millis()/5000) % 2){
    current_effect = &zwart;
  } else {
    current_effect = &rood;
  }
  
  current_effect->update();
  
  FastLED.show();
}