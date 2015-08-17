#ifndef LedTafelEffect_h
#define LedTafelEffect_h
#include <FastLED.h>

class LedTafelEffect{
  protected:
    CRGB* _leds;
    static uint8_t _rows, _cols, _num_leds;
  public:
    LedTafelEffect(CRGB* leds) : _leds(leds) {};
    static void setDimensions(uint8_t rows, uint8_t cols);
    virtual void init(){};
    // virtual void deinit();
    virtual void update(){};
  protected:
    // Shared utility functions
    static uint8_t xy2i(uint8_t x, uint8_t y);
};

#include "AllBlack.h"
#include "AllRed.h"
#include "RedLine.h"
#include "Haardvuur.h"
#include "GameOfLife.h"

#endif