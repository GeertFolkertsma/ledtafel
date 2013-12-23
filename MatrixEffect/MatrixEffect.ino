#include "FastSPI_LED2.h"

#define ROWS 10
#define COLS 10
#define NUM_LEDS (ROWS*COLS)

CRGB leds[NUM_LEDS];

#define LOOP_TIME 20
#define UPDATE_TIME 150

#define _R 0
#define _G 1
#define _B 2

CRGB new_colors[NUM_LEDS];


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

void sendColors(){
  for(uint8_t y=0; y<ROWS; ++y)
    for(uint8_t x=0; x<COLS; ++x){
      for(uint8_t i=0; i<3; ++i)
        leds[xy2i(x,y)][i] = (uint8_t) (((uint16_t) leds[xy2i(x,y)][i]*19 + new_colors[xy2i(x,y)][i])/20);
    }
  LEDS.show();
}

boolean onPixels[ROWS][COLS];
void updateColors(){
  
  for(uint8_t y=ROWS-1; y>0; --y)
    for(uint8_t x=0; x<COLS; ++x){
      if(onPixels[y-1][x]){
        // turn this pixel on if the preceding one was high
        onPixels[y][x] = true;
        // and turn off that preceding one
        onPixels[y-1][x] = false;
      } else {
        onPixels[y][x] = false;
      }
    }

  // Seed a new pixel
  if(random(0,10)>7){
    uint8_t randx = random(0,COLS);
    onPixels[0][randx] = true;
    new_colors[xy2i(randx,0)].setHSV(random(0,256), 255, 255);
  }
  
  for(uint8_t y=ROWS-1; y>0; --y)
    for(uint8_t x=0; x<COLS; ++x){
      if(onPixels[y][x]){
        for(uint8_t i=0; i<3; ++i)
          new_colors[xy2i(x,y)][i] = new_colors[xy2i(x,y-1)][i];
      } else {
        for(uint8_t i=0; i<3; ++i)
          new_colors[xy2i(x,y)][i] = 0;
      }
    }
  for(uint8_t x=0; x<COLS; ++x){
    //first row -> turn off the off pixels after propagation of colour
    if(!onPixels[0][x]){
      new_colors[xy2i(x,0)] = CRGB::Black;
    }
  }
}

void setup(){
  // Delay in case the LEDs draw a lot of current and kill the power supply
  delay(1000);
  // Let the controller know we're using WS2801 leds, and give a pointer to the current colour array
  LEDS.addLeds<WS2801, RGB>(leds, NUM_LEDS);
  // Limit the brightness somewhat (scale is 0-255)
  LEDS.setBrightness(127);
  
  // Initialise the leds to black (off)
  for(uint8_t i=0; i<NUM_LEDS; ++i){
    leds[i] = CRGB::Black;
  }
  LEDS.show();
}

unsigned long lastLoopTime = 0;
unsigned long lastUpdateTime = 0;
void loop(){
  if(millis()-lastUpdateTime > UPDATE_TIME){
    updateColors();
    lastUpdateTime = millis();
  }
  sendColors();
  
  while(millis()-lastLoopTime < LOOP_TIME){
    //wait
  }
  lastLoopTime = millis();
}
