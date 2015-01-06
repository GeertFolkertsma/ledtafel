#include "LedTafelEffect.h"


// initialise static member values (to reasonable values?)
// TODO: move to #define'd parameters
uint8_t LedTafelEffect::_rows = 0;
uint8_t LedTafelEffect::_cols = 0;
uint8_t LedTafelEffect::_num_leds = 0;

void LedTafelEffect::setDimensions(uint8_t rows, uint8_t cols){
  _rows = rows;
  _cols = cols;
  _num_leds = _rows * _cols;
}

uint8_t LedTafelEffect::xy2i(uint8_t x, uint8_t y) {
  // (0,0) is bottom-left corner; x is right; y is up
  // LED string snake starts at (0,0) and goes up first
  //so x always increases, but y alternates between + and -
  if(x%2 == 0){
    // In the 'even' rows (first row is x=0), y increases
    return _rows*x + y;
  } else {
    // and in the 'odd' rows, y decreases
    return _rows*(x+1) - 1 - y;
  }
}