#ifndef AllBlack_h
#define AllBlack_h
#include "LedTafelEffect.h"
#include <FastLED.h>

class AllBlack : public LedTafelEffect{
  private:
    CRGB* _leds;
    uint8_t _num_leds;
  public:
    AllBlack(){};
    void init(CRGB* leds, uint8_t num_leds);
    void update();
};

#endif