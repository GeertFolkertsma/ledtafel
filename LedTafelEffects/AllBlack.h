#ifndef AllBlack_h
#define AllBlack_h
#include "LedTafelEffect.h"

class AllBlack : public LedTafelEffect{
  public:
    AllBlack(CRGB* leds, uint8_t num_leds) : LedTafelEffect(leds,num_leds){};
    void init();
    void update();
};

#endif