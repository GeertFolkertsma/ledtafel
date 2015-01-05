#ifndef AllRed_h
#define AllRed_h
#include "LedTafelEffect.h"
#include <FastLED.h>

class AllRed : public LedTafelEffect{
  private:
    CRGB* _leds;
    uint8_t _num_leds;
  public:
    AllRed(){};
    void init(CRGB* leds, uint8_t num_leds);
    void update();
};

#endif