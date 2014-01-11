#include "FastSPI_LED2.h"


#define LOOP_TIME 20
#define UPDATE_TIME 1000

#define ROWS 10
#define COLS 10
#define NUM_LEDS (ROWS*COLS)

// Init/define led arrays
CRGB leds[NUM_LEDS];    //current colours
CRGB newleds[NUM_LEDS]; //new colours

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

// currently-alive cells
boolean alive[ROWS][COLS];
// cells alive in next step
boolean next[ROWS][COLS];

uint8_t getNeighbour(int8_t y, int8_t x){
  if( y<0 || y>=ROWS || x<0 || x>=COLS) return 0;
  if(alive[y][x]) return 1;
  return 0;
}

void GoLstep(){
  //execute a GoL-step
  for(uint8_t y=0; y!=ROWS; ++y){
    for(uint8_t x=0; x!=COLS; ++x){
      // First reset the next array
      next[y][x] = false;
      // Then calculate number of neighbours (n)
      uint8_t n = 0;
      // loop through all neighbouring cells
      for(int8_t dy=-1; dy!=2; ++dy){
        for(int8_t dx=-1; dx!=2; ++dx){
          if( dx==0 && dy==0 ) continue; //do not count itself
          n += getNeighbour((int8_t)y+dy,(int8_t)x+dx);
        }
      }
      // Make a decision based on number of alive neighbours
      if(n==3){
        // keeps living or is born next round
        next[y][x] = true;
      } else if(alive[y][x] && n==2){
        //stays alive with 2 neighbours
        next[y][x] = true;
      }
      // All other cases: die or stay dead
    }
  }
  // Now copy next to current (alive)
  memcpy(alive,next,sizeof(next));
  
  // Finally, update the newleds array
  for(uint8_t y=0; y!=ROWS; ++y){
    for(uint8_t x=0; x!=COLS; ++x){
      if(alive[y][x]){
        newleds[xy2i(x,y)].setHSV(127, 255, 255);
      } else {
        newleds[xy2i(x,y)] = CRGB::Black;
      }
    }
  }
}

void sendColors(){
  // simple FO low-pass between newleds and leds
  for(uint8_t i_led=0; i_led!=NUM_LEDS; ++i_led)
    for(uint8_t i=0; i!=3; ++i)
      leds[i_led][i] = (uint8_t) (((uint16_t) leds[i_led][i]*19 + newleds[i_led][i])/20);
  
  LEDS.show();
}



void setup(){
  LEDS.addLeds<WS2801, RGB>(leds, NUM_LEDS);
  // Limit the brightness somewhat (scale is 0-255)
  LEDS.setBrightness(127);
  
  // Initialise the leds to black (off)
  for(uint8_t i=0; i<NUM_LEDS; ++i){
    leds[i] = CRGB::Black;
  }
  LEDS.show();
  
  // seed initial cells
  randomSeed(analogRead(A0));
  for(uint8_t y=0; y!=ROWS; ++y){
    for(uint8_t x=0; x!=COLS; ++x){
      alive[y][x] = random(0,10)>5;
    }
  }
}


unsigned long lastLoopTime = 0;
unsigned long lastUpdateTime = 0;
void loop(){
  if(millis()-lastUpdateTime > UPDATE_TIME){
    GoLstep();
    lastUpdateTime = millis();
  }
  sendColors();
  
  while(millis()-lastLoopTime < LOOP_TIME){
    //wait
  }
  lastLoopTime = millis();
}






