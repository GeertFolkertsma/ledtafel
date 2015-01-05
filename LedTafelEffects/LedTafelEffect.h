#ifndef LedTafelEffect_h
#define LedTafelEffect_h
#include <FastLED.h>

class LedTafelEffect{
  protected:
    CRGB* _leds;
    uint8_t _num_leds;
  public:
    LedTafelEffect(CRGB* leds, uint8_t num_leds) : _leds(leds), _num_leds(num_leds) {};
    virtual void init(){};
    // virtual void deinit();
    virtual void update(){};
};

#include "AllBlack.h"
#include "AllRed.h"

#endif