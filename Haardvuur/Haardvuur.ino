// TODO: changing/other palette?
#include <FastLED.h>
// #define SERIAL_DEBUG
#ifdef SERIAL_DEBUG
#include <Streaming.h>
#endif

#define ROWS 10
#define COLS 10
#define NUM_LEDS (ROWS*COLS)

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

// direct output buffer
CRGB leds[NUM_LEDS];
// Heatmap, with the 'temperature' of the fire
uint8_t heat[NUM_LEDS];
uint8_t heat_old[NUM_LEDS];
// Current colour palette
CRGBPalette16 gPal;


uint8_t xy2i(uint8_t x, uint8_t y){
  // (0,0) is bottom-left corner; x is right; y is up
  // LED string snake starts at (0,0) and goes up first
  //so x always increases, but y alternates between + and -
  if(x%2 == 0){
    // In the 'even' rows (first row is x=0), y increases
    return ROWS*x + y;
  } else {
    // and in the 'odd' rows, y decreases
    return ROWS*(x+1) - 1 - y;
  }
}

void setup(){
#ifdef SERIAL_DEBUG
  Serial.begin(57600);
  Serial << "Haardvuur.ino" << endl;
#endif
  FastLED.delay(1000);
  
  FastLED.addLeds<WS2801, RGB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
#ifdef SERIAL_DEBUG
  //test to see if everything is working (only when debugging anyway)
  fill_rainbow(leds, NUM_LEDS, 0, 255/NUM_LEDS);
  FastLED.show();
  delay(2000);
  // blank strip
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  LEDS.show();
#endif
}

// This code works along the lines of "Fire2012" but now in 2-D
// The following things happen:
// 1. Each cell cools down a bit to the air
// 2. Heat drifts up (a lot) and sideways (a bit)
// 3. New 'sparks' 

uint8_t loopCounter = 1;
#ifdef SERIAL_DEBUG
unsigned long prev_looptime;
#endif
void loop(){
  // Random is used a lot, so 'add entropy' here
  random16_add_entropy(random());
  
  // Execute the fire simulation function
  if(loopCounter++ == FIRE_EVERY){
    loopCounter = 0;
    FireAway();
  }
  
  // Select colour map
#if COLOURMODE == C_HEATMAP
  gPal = HeatColors_p;
#elif COLOURMODE == C_RANDOMWALK
  // random walk the hue (with rollover)
  static uint8_t hue;
  hue += random8(3) - 1;
  CRGB darkcolor = CHSV(hue, 255, 192); //pure, 3/4 brightness
  CRGB lightcolor = CHSV(hue, 128, 255);//half-white, full brightness
  gPal = CRGBPalette16(CRGB::Black, darkcolor, lightcolor, CRGB::White);
#endif
  // Calculate the proper LED colours
  ColourLeds(loopCounter);
  
  FastLED.show();
#ifdef SERIAL_DEBUG
  Serial << (millis()-prev_looptime) << endl;
  prev_looptime = millis();
#endif
  FastLED.delay(LOOP_TIME);
  
  selectSparkCell();
}

bool probability_sum_initialised = false;
uint8_t selectSparkCell(){
  // this function selects a random spark cell at the bottom,
  //but probability is a linear ramp towards the middle
  static uint8_t probability_sum[COLS];
  if(!probability_sum_initialised){
    uint8_t sum = 0;
    for(int8_t i=0; i != COLS; ++i){
      sum += COLS/2  - abs(i-COLS/2);
      probability_sum[i] = sum;
    }
#ifdef SERIAL_DEBUG
    Serial << "prob.sum. initialised: ";
    for(uint8_t i=0; i != COLS; ++i){
      Serial << probability_sum[i] << ",";
    }
    Serial << endl;
#endif
    probability_sum_initialised = true;
  }
  
  uint8_t r = random8(probability_sum[COLS-1]+1);
  for(uint8_t i=0; i != COLS; ++i){
    if(r <= probability_sum[i]){
#ifdef SERIAL_DEBUG
      Serial << "i: " << i << endl;
#endif
      return i;
    }
  }
  // this should not happen!
  return 0;
}

void FireAway(){
  uint8_t i;
  uint16_t heat_sum; // to hold a large sum of heat sources
  
  // 1. each cell cools down a bit
  for( i=0; i != NUM_LEDS; ++i){
    // copy heatmap to old heatmap for interpolation
    heat_old[i] = heat[i];
    heat[i] = qsub8(heat[i], random8(0, COOLING));
  }
  
  // 2. now heat drifts up (a lot) and sideways (a bit)
  for( uint8_t y=COLS-1; y != 0; --y){
    for( uint8_t x=0; x != ROWS; ++x){
      i = xy2i(x,y);
      heat_sum = 4*(uint16_t)heat[xy2i(x,y-1)];
      if(y != 1){
        heat_sum += 2*(uint16_t)heat[xy2i(x,y-2)];
      }
      if(x != 0){
        heat_sum += heat[xy2i(x-1,y-1)];
      }
      if(x != COLS-1){
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

void ColourLeds(uint8_t count){
  // Calculate the proper LED colours
  uint8_t frac = count * (255/FIRE_EVERY);
  uint8_t current_heat;
  for(uint8_t i=0; i!=NUM_LEDS; ++i){
    // apparently palette works best if max is 240
    current_heat = lerp8by8(heat_old[i],heat[i],frac);
    leds[i] = ColorFromPalette( gPal, scale8(current_heat,240));
  }
}

