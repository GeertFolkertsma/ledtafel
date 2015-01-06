#ifndef Haardvuur_h
#define Haardvuur_h
#include "LedTafelEffect.h"

// Fire settings
#define BRIGHTNESS 180
#define LOOP_TIME 33
#define FIRE_EVERY 10 //update fire every n loops only; interpolate in-between
#define COOLING 40
#define SPARKING 130
// Colours used by the fire animation
#define C_HEATMAP 1
#define C_RANDOMWALK 2
#define COLOURMODE C_RANDOMWALK

#ifndef COLOURMODE
#define COLOURMODE C_HEATMAP
#endif


class Haardvuur : public LedTafelEffect{
  uint8_t *heat, *heat_old;
  uint8_t *probability_sum;
  uint8_t hue, loopCounter;
  bool is_initialised;
  CRGBPalette16 gPal;
  public:
    Haardvuur(CRGB* leds) : LedTafelEffect(leds) , is_initialised(false){};
    ~Haardvuur();
    void init();
    void deinit();
    void update();
  private:
    uint8_t selectSparkCell();
    void updateFire();
};

#endif