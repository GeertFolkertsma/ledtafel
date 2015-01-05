#include <FastLED.h>
#include "AllBlack.h"
#include "AllRed.h"

#define ROWS 10
#define COLS 10
#define NUM_LEDS (ROWS*COLS)

// This is the output buffer that gets copied onto the table
CRGB leds[NUM_LEDS];

AllBlack zwart;
AllRed rood;


void setup(){
  FastLED.addLeds<WS2801, RGB>(leds, NUM_LEDS);
  zwart.init(leds,NUM_LEDS);
  rood.init(leds,NUM_LEDS);
}

void loop(){
  if((millis()/5000) % 2){
    zwart.update();
  } else {
    rood.update();
  }
  
  FastLED.show();
}