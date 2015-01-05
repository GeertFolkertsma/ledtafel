#include "AllRed.h"

void AllRed::init(){
}

void AllRed::update(){
  for(uint8_t i=0; i != _num_leds; ++i){
    _leds[i] = CRGB::Red;
  }
}