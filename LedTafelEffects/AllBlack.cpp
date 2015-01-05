#include "AllBlack.h"

void AllBlack::init(){
}

void AllBlack::update(){
  for(uint8_t i=0; i != _num_leds; ++i){
    _leds[i] = CRGB::Black;
  }
}