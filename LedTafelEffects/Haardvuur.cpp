#include "Haardvuur.h"

Haardvuur::~Haardvuur(){
  // be sure to free up the memory we took
  deinit();
}

void Haardvuur::init(){
  if(is_initialised){
    return;
  }
  
  // Initialise heatmap
  heat = new uint8_t[_num_leds];
  heat_old = new uint8_t[_num_leds];
  for(uint8_t i=0; i != _num_leds; ++i){
    heat[i] = 0;
    heat_old[i] = 0;
  }
  
  // Initialise probability sum
  probability_sum = new uint8_t[_rows];
  uint8_t sum = 0;
  for(int8_t i=0; i != _cols; ++i){
    sum += _cols/2  - abs(i-_cols/2);
    probability_sum[i] = sum;
  }
  
  // Initialise colour palette
  gPal = HeatColors_p;
  
  is_initialised = true;
}

void Haardvuur::deinit(){
  delete [] heat;
  delete [] heat_old;
  delete [] probability_sum;
  is_initialised = false;
}

void Haardvuur::update(){
  // Random is used a lot, so 'add entropy' here
  random16_add_entropy(random());

  if(loopCounter++ == FIRE_EVERY){
    loopCounter = 0;
    updateFire();
  }
  
  // Calculate the proper LED colours
  uint8_t frac = loopCounter * (255/FIRE_EVERY);
  uint8_t current_heat;
  for(uint8_t i=0; i!=_num_leds; ++i){
    // apparently palette works best if max is 240 (due to wrap)
    current_heat = lerp8by8(heat_old[i],heat[i],frac);
    _leds[i] = ColorFromPalette( gPal, scale8(current_heat,240));
  }
}


void Haardvuur::updateFire(){
  uint8_t i;
  uint16_t heat_sum; // to hold a large sum of heat sources
  
  // 1. each cell cools down a bit
  for( i=0; i != _num_leds; ++i){
    // copy heatmap to old heatmap for interpolation
    heat_old[i] = heat[i];
    heat[i] = qsub8(heat[i], random8(0, COOLING));
  }
  
  // 2. now heat drifts up (a lot) and sideways (a bit)
  for( uint8_t y=_cols-1; y != 0; --y){
    for( uint8_t x=0; x != _rows; ++x){
      i = xy2i(x,y);
      heat_sum = 4*(uint16_t)heat[xy2i(x,y-1)];
      if(y != 1){
        heat_sum += 2*(uint16_t)heat[xy2i(x,y-2)];
      }
      if(x != 0){
        heat_sum += heat[xy2i(x-1,y-1)];
      }
      if(x != _cols-1){
        heat_sum += heat[xy2i(x+1,y-1)];
      }
      
      heat[i] = (heat_sum/8);
    }
  }
  
  // 3. then new 'sparks' are ignited along the bottom
  //with double probability in the 'middle' columns
  if( random8() < SPARKING ){
    // spark somewhere at the bottom
    i = xy2i(selectSparkCell(),0);
    heat[i] = qadd8(heat[i], random8(160,255));
  }
}

uint8_t Haardvuur::selectSparkCell(){
  // this function selects a random spark cell at the bottom,
  //but probability is a linear ramp towards the middle
  uint8_t r = random8(probability_sum[_cols-1]+1);
  for(uint8_t i=0; i != _cols; ++i){
    if(r <= probability_sum[i]){
      return i;
    }
  }
  // this should not happen!
  return 0;
}