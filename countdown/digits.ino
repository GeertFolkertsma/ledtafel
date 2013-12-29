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

// Add digits for countdown timer
const boolean zero[] = {
  true, true, true,
  true, false, true,
  true, false, true,
  true, false, true,
  true, true, true
};
boolean one[] = {
  false, false, true,
  false, false, true,
  false, false, true,
  false, false, true,
  false, false, true
};
const boolean two[] = {
  true, true, true,
  false, false, true,
  true, true, true,
  true, false, false,
  true, true, true
};
const boolean three[] = {
  true, true, true,
  false, false, true,
  false, true, true,
  false, false, true,
  true, true, true
};
const boolean four[] = {
  true, false, true,
  true, false, true,
  true, true, true,
  false, false, true,
  false, false, true
};
const boolean five[] = {
  true, true, true,
  true, false, false,
  true, true, true,
  false, false, true,
  true, true, true
};
const boolean six[] = {
  true, true, true,
  true, false, false,
  true, true, true,
  true, false, true,
  true, true, true
};
boolean seven[] = {
  true, true, true,
  false, false, true,
  false, false, true,
  false, false, true,
  false, false, true
};
const boolean eight[] = {
  true, true, true,
  true, false, true,
  true, true, true,
  true, false, true,
  true, true, true
};
const boolean nine[] = {
  true, true, true,
  true, false, true,
  true, true, true,
  false, false, true,
  true, true, true
};

const boolean* digits[] = {zero, one, two, three, four, five, six, seven, eight, nine};


void writeDigit(uint8_t c, uint8_t dx, uint8_t dy, CRGB* colour){
  // Digits are 3 by 5 (x by y)
  for(uint8_t y=0; y != 5; ++y){
    Serial.println();
    for(uint8_t x=0; x!=3; ++x){
      if(digits[c][x+3*y]){
        leds[xy2i(dx+x,dy+y)] = colour;
      } else {
        leds[xy2i(dx+x,dy+y)] = colour;
      }
    }
  }
}

void setup(){
  // Delay in case the LEDs draw a lot of current and kill the power supply
  delay(1000);
  // Let the controller know we're using WS2801 leds, and give a pointer to the current colour array
  LEDS.addLeds<WS2801, RGB>(leds, NUM_LEDS);
  // Limit the brightness somewhat (scale is 0-255)
  LEDS.setBrightness(48);
  
  for(uint8_t i=0; i<NUM_LEDS; ++i){
    leds[i] = CRGB::Black;
  }
  
  LEDS.show();
  
  Serial.begin(57600);
}

uint8_t ds[] = {0,1,2,3};
CRGB digit_colours[] = {CRGB::Red, CRGB::OrangeRed, CRGB::Cyan, CRGB::Blue};
void loop(){
  for(uint8_t i=0; i!=4; ++i){
    writeDigit(ds[i]++, (i>1?2:0) + (i%2)*5, (i>1?5:0), digit_colours[i]);
    if(ds[i]>9) ds[i] = 0;
  }
  LEDS.show();
  delay(5000);
}


