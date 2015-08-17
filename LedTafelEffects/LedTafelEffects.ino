#include <FastLED.h>
#include "LedTafelEffect.h"

#define ROWS 10
#define COLS 10
#define NUM_LEDS (ROWS*COLS)

#define CYCLE_PERIOD (10*60*1000)
uint32_t last_cycle_time = 0;

// This is the output buffer that gets copied onto the table
CRGB leds[NUM_LEDS];

GameOfLife effect1(leds);
Haardvuur effect2(leds);

LedTafelEffect* current_effect;

void setup(){
  LedTafelEffect::setDimensions(ROWS,COLS);
  FastLED.addLeds<WS2801, RGB>(leds, NUM_LEDS);
  effect1.init();
  effect2.init();
  
  current_effect = &effect1;
}

uint8_t effect_i = 0;
void loop(){
  
  if(millis() - last_cycle_time > CYCLE_PERIOD ){
    last_cycle_time = millis();
    switch(effect_i++){
      case 0: current_effect = &effect1; break;
      case 1: current_effect = &effect2; break;
      default: effect_i = 0; break;
    }
  }
  
  current_effect->update();
  
  FastLED.show();
  
  FastLED.delay(20);
}