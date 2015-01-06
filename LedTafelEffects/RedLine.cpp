#include "RedLine.h"

void RedLine::init(){
}

void RedLine::update(){
  for(uint8_t i=0; i != _num_leds; ++i){
    _leds[i] = CRGB::Black;
  }
  for(uint8_t i=0; i != _rows; ++i){
    _leds[xy2i(i,i)] = CRGB::Red;
  }
}