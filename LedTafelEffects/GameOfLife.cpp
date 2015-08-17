#include "GameOfLife.h"

GameOfLife::~GameOfLife(){
  // be sure to free up the memory we took
  deinit();
}

void GameOfLife::init(){
  if(is_initialised){
    return;
  }
  
  // Initialise all the alive,prev,etc.
  newleds = new CRGB[_num_leds];
  alive = new bool[_num_leds];
  next = new bool[_num_leds];
  prev = new bool[_num_leds];
  
  // Init colours
  randomSeed(analogRead(A3));
  generateColour();
  from_hue = to_hue; from_val = to_val; from_sat = to_sat;
  generateColour();
  
  // seed the first cells
  seedFlag = false;
  last_GoLstep = 0;
  GoLseed();
  
  is_initialised = true;
}

void GameOfLife::deinit(){
  delete [] newleds;
  delete [] alive;
  delete [] prev;
  delete [] next;
  is_initialised = false;
}

void GameOfLife::update(){
  uint8_t hue, saturation, value;
  
  if(seedFlag && millis()-seedTime > SEED_DELAY){
    seedFlag = false;
    GoLseed();
  }
  
  if(millis()-last_GoLstep > UPDATE_TIME){
    GoLstep();
    last_GoLstep = millis();
  }
  
  // Interpolate between from_hue and to_hue
  if(++hue_i == HUE_SLOWDOWN*255){
    //select a new hue
    from_hue = to_hue;
    from_sat = to_sat;
    from_val = to_val;
    generateColour();
    
    hue_i = 0;
  }
  hue = lerp8by8(from_hue,to_hue,hue_i/HUE_SLOWDOWN);
  saturation = lerp8by8(from_sat,to_sat,hue_i/HUE_SLOWDOWN);
  value = lerp8by8(from_val,to_val,hue_i/HUE_SLOWDOWN);
  
  // Set colour for ALIVE cells
  for(uint8_t i=0; i != _num_leds; ++i){
    if(alive[i]){
      newleds[i].setHSV(hue, saturation, value);
    } else {
      newleds[i] = CRGB::Black;
    }
    // FO-LP this into the leds array  
    for(uint8_t j=0; j != 3; ++j){
      _leds[i][j] = (uint8_t) (((uint16_t) _leds[i][j]*24 + newleds[i][j])/25);
    }
  }
}

void GameOfLife::GoLseed(){
  // seed initial cells
  for(uint8_t i=0; i!=_num_leds;++i){
    alive[i] = random(0,10)>5;
  }
}

void GameOfLife::GoLstep(){
  // Run 1 simulation step of the GoL algorithm
  
  // Loop through all cells and determine their next state
  for(uint8_t y=0; y!=_rows; ++y){
    for(uint8_t x=0; x!=_cols; ++x){
      // First reset the next array
      next[xy2i(x,y)] = false;
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
        next[xy2i(x,y)] = true;
      } else if(alive[xy2i(x,y)] && n==2){
        //stays alive with 2 neighbours
        next[xy2i(x,y)] = true;
      }
      // All other cases: die or stay dead
    }
  }
  // Try to find a stall, or period-1 oscillation
  boolean same_prev = true;
  boolean same_alive = true;
  for(uint8_t i=0; i!=_num_leds; ++i){
    if(next[i] != prev[i]) same_prev = false;
    if(next[i] != alive[i]) same_alive = false;
  }
  if((same_prev || same_alive) && !seedFlag){
    seedFlag = true;
    seedTime = millis();
  }
  
  // Now copy current to prev and next to current (alive)
  memcpy(prev,alive,_num_leds * sizeof(bool));
  memcpy(alive,next,_num_leds * sizeof(bool));
}

uint8_t GameOfLife::getNeighbour(int8_t y, int8_t x){
  if( y<0 || y>=_rows || x<0 || x>=_cols) return 0;
  if(alive[xy2i(x,y)]) return 1;
  return 0;
}

void GameOfLife::generateColour(){
  to_hue = random(H_MIN,H_MAX);
  to_sat = random(S_MIN,S_MAX);
  to_val = random(V_MIN,V_MAX);
}