// TODO: changing/other palette?
#include <FastLED.h>

#define ROWS 10
#define COLS 10
#define NUM_LEDS (ROWS*COLS)

// Fire settings
#define BRIGHTNESS 180
#define LOOP_TIME 500
#define COOLING 55
#define SPARKING 120

// direct output buffer
CRGB leds[NUM_LEDS];
// Heatmap, with the 'temperature' of the fire
uint8_t heat[NUM_LEDS];
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
  FastLED.delay(1000);
  
  FastLED.addLeds<WS2812, RGB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  //test to see if everything is working
  fill_rainbow(leds, NUM_LEDS, 0, 255/NUM_LEDS);
  FastLED.show();
  delay(2000);
  // blank strip
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  LEDS.show();
  
  gPal = HeatColors_p;
}

// This code works along the lines of "Fire2012" but now in 2-D
// The following things happen:
// 1. Each cell cools down a bit to the air
// 2. Heat drifts up (a lot) and sideways (a bit)
// 3. New 'sparks' 

void loop(){
  // Random is used a lot, so 'add entropy' here
  random16_add_entropy(random());
  
  // Execute the fire simulation function
  FireAway();
  
  // Calculate the proper LED colours
  ColourLeds();
  
  FastLED.show();
  FastLED.delay(LOOP_TIME);
}

void FireAway(){
  uint8_t i;
  uint16_t heat_sum; // to hold a large sum of heat sources
  
  // 1. each cell cools down a bit
  for( i=0; i != NUM_LEDS; ++i){
    heat[i] = qsub8(heat[i], random8(0, COOLING));
  }
  
  // 2. now heat drifts up (a lot) and sideways (a bit)
  for( uint8_t y=COLS-1; y != 2; --y){
    for( uint8_t x=0; x != ROWS; ++x){
      i = xy2i(x,y);
      heat_sum = 2*(uint16_t)heat[xy2i(x,y-1)] + 4*(uint16_t)heat[xy2i(x,y-2)];
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
    i = xy2i(random8(COLS-1),0);
    heat[i] = qadd8(heat[i], random8(160,255));
  }
  if( random8() < SPARKING ){
    // spark somewhere at the bottom, in the middle
    i = xy2i(random8(COLS/4,COLS-COLS/4),0);
    heat[i] = qadd8(heat[i], random8(160,255));
  }
}

void ColourLeds(){
  // Calculate the proper LED colours
  for(uint8_t i=0; i!=NUM_LEDS; ++i){
    // apparently palette works best if max is 240
    leds[i] = ColorFromPalette( gPal, scale8(heat[i],240));
  }
}

