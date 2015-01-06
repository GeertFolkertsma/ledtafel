#ifndef AllBlack_h
#define AllBlack_h
#include "LedTafelEffect.h"

class AllBlack : public LedTafelEffect{
  public:
    AllBlack(CRGB* leds) : LedTafelEffect(leds){};
    void init();
    void update();
};

#endif