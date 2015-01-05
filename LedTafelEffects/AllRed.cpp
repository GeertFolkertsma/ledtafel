#include "AllRed.h"

void AllRed::init(CRGB* leds, uint8_t num_leds){
  _leds = leds;
  _num_leds = num_leds;
}

void AllRed::update(){
  for(uint8_t i=0; i != _num_leds; ++i){
    _leds[i] = CRGB::Red;
  }
}