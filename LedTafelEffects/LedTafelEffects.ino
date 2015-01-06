#include <FastLED.h>
#include "LedTafelEffect.h"

#define ROWS 10
#define COLS 10
#define NUM_LEDS (ROWS*COLS)


// This is the output buffer that gets copied onto the table
CRGB leds[NUM_LEDS];

AllBlack effect1(leds);
RedLine effect2(leds);

LedTafelEffect* current_effect;

void setup(){
  LedTafelEffect::setDimensions(ROWS,COLS);
  FastLED.addLeds<WS2801, RGB>(leds, NUM_LEDS);
  effect1.init();
  effect2.init();
}

void loop(){
  if((millis()/5000) % 2){
    current_effect = &effect1;
  } else {
    current_effect = &effect2;
  }
  
  current_effect->update();
  
  FastLED.show();
}