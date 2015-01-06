#ifndef RedLine_h
#define RedLine_h
#include "LedTafelEffect.h"

class RedLine : public LedTafelEffect {
  public:
    RedLine(CRGB* leds) : LedTafelEffect(leds){};
    void init();
    void update();
};

#endif