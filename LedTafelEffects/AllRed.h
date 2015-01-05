#ifndef AllRed_h
#define AllRed_h
#include "LedTafelEffect.h"

class AllRed : public LedTafelEffect{
  public:
    AllRed(CRGB* leds, uint8_t num_leds) : LedTafelEffect(leds,num_leds){};
    void init();
    void update();
};

#endif