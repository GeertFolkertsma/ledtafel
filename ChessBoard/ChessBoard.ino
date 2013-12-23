#include "FastSPI_LED2.h"

#define ROWS 10
#define COLS 10
#define NUM_LEDS (ROWS*COLS)

CRGB leds[NUM_LEDS];

// Return 0-based led index for 0-based x and y grid co-ordinate values
// The LED string snakes through the grid, so every row the positive x direction switches
uint8_t xy2i(uint8_t x, uint8_t y){
  if(y%2){
    // It's an uneven row: x should be reversed---or subtracted from this line's
    //last index, which is the next line's first minus 1
    return (COLS*(y+1) - 1) - x;
  } else {
    // Even row: simply do COLS*y + x
    return COLS*y + x;
  }
}

void setup(){
  // Delay in case the LEDs draw a lot of current and kill the power supply
  delay(1000);
  // Let the controller know we're using WS2801 leds, and give a pointer to the current colour array
  LEDS.addLeds<WS2801, RGB>(leds, NUM_LEDS);
  // Limit the brightness somewhat (scale is 0-255)
  LEDS.setBrightness(64);
  
  // Initialise the leds to checkboard pattern
  for(uint8_t x=1; x<COLS-1; ++x){
    for(uint8_t y=1;y<ROWS-1; ++y){
      if((x+y)%2){
        leds[xy2i(x,y)] = CRGB::Black;
      } else {
        leds[xy2i(x,y)] = CRGB::White;
      }
    }
  }
  // make the edge red-ish
  for(uint8_t ind=0; ind<COLS; ++ind){
    leds[xy2i(ind,0)] = CRGB(50,0,0);
    leds[xy2i(ind,ROWS-1)] = CRGB(50,0,0);
    // Note: this part if only valid if ROWS==COLS
    leds[xy2i(0,ind)] = CRGB(50,0,0);
    leds[xy2i(COLS-1,ind)] = CRGB(50,0,0);
  }
  LEDS.show();
}


void loop(){
  //TODO: pulse brightness or some spectacular other effect..
}

