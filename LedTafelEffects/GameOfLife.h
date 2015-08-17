#ifndef GameOfLife_h
#define GameOfLife_h
#include "LedTafelEffect.h"


// Time between GoL steps in ms (5000 default -> one step every 5 s)
#define UPDATE_TIME 5000
// How long to wait with seeding a new field after a stall or period-2 oscillations (ms)
#define SEED_DELAY 15000
// Hue is linearly interpolated between randomised values
// the speed of the interpolation is 255 * HUE_SLOWDOWN * LOOP_TIME ~= 5s * HUE_SLOWDOWN
#define HUE_SLOWDOWN 10

//random colour parameters (min/max for each h,s,v)
#define H_MIN 0
#define H_MAX 255
#define S_MIN 180
#define S_MAX 255
#define V_MIN 180
#define V_MAX 255



class GameOfLife : public LedTafelEffect{
  bool is_initialised;
  // GoL data structures
  bool* alive; // cells that are alive
  bool* next; // cells alive in next step
  bool* prev; // previous state to detect period-1 oscillations
  // colours
  CRGB* newleds; // new colours to fade in
  uint8_t from_hue, from_sat, from_val;
  uint8_t to_hue, to_sat, to_val;
  uint16_t hue_i; //interpolation counter
  // timing
  uint32_t last_GoLstep;
  bool seedFlag;
  uint32_t seedTime;
  public:
    GameOfLife(CRGB* leds) : LedTafelEffect(leds) , is_initialised(false){};
    ~GameOfLife();
    void init();
    void deinit();
    void update();
  private:
    uint8_t getNeighbour(int8_t y, int8_t x);
    void generateColour();
    void GoLseed();
    void GoLstep();
};

#endif