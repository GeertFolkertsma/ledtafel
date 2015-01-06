#ifndef AllRed_h
#define AllRed_h
#include "LedTafelEffect.h"

class AllRed : public LedTafelEffect{
  public:
    AllRed(CRGB* leds) : LedTafelEffect(leds){};
    void init();
    void update();
};

#endif