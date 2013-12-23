#include "FastSPI_LED2.h"

#define ROWS 10
#define COLS 10
#define NUM_LEDS (ROWS*COLS)

CRGB leds[NUM_LEDS];
// Keep the current colour available in HSV
CHSV colors[NUM_LEDS];
CHSV new_colors[NUM_LEDS];


#define LOOP_TIME 20
#define UPDATE_TIME 150


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

// Send the colours out to the LEDs---right now also includes the low-pass filtering
void sendColors(){
  // We do not need to use the array structure here---just copy some values
  for(uint8_t i_led=0; i_led<NUM_LEDS; ++i_led){
    // First do a FO low-pass between new_colors and colors (in HSV space)
    // Actually, for this effect, copy the hue and saturation directly,
    // and only do FO low-pass on value (lightness)
    colors[i_led].h = new_colors[i_led].h;
    colors[i_led].s = new_colors[i_led].s;
    colors[i_led].v = (uint8_t) (((uint16_t) colors[i_led].v*19 + new_colors[i_led].v)/20);
    
    // Then copy the current HSV value to the current RGB value
    leds[i_led] = colors[i_led];
  }
    
  
  LEDS.show();
}

// Update the colours: in this case, propagate the "on" pixels downwards and seed
// new pixels on the first row, to obtain a "Matrix effect"
boolean onPixels[ROWS][COLS];
void updateColors(){
  
  // Loop through the rows, starting from the bottom, copying the "on" state from the
  // row above and then clearing that pixel, to have the "on" state "walk down" the cols
  for(uint8_t y=ROWS-1; y>0; --y)
    for(uint8_t x=0; x<COLS; ++x){
      if(onPixels[y-1][x]){
        // turn this pixel on if the preceding one was high
        onPixels[y][x] = true;
        // and turn off that preceding one
        onPixels[y-1][x] = false;
      }
    }

  // Seed a new pixel in the first row, with a 30% chance
  if(random(0,10)>7){
    uint8_t randx = random(0,COLS);
    onPixels[0][randx] = true;
    new_colors[xy2i(randx,0)].setHSV(random(0,256), 255, 255);
  }
  
  // Now percolate down the colours from top to bottom (again by looping from bottom to top)
  for(uint8_t y=ROWS-1; y>0; --y)
    for(uint8_t x=0; x<COLS; ++x){
      if(onPixels[y][x]){
        // If on, copy the colour from the row above
        new_colors[xy2i(x,y)] = new_colors[xy2i(x,y-1)];
      } else {
        // If off, turn the pixel off (the actual pixel colour has a FO low-pass filter)
        new_colors[xy2i(x,y)].setHSV(0,0,0);
      }
    }
  
  // Finally, turn off all pixels in the first row that are not "on"
  // This is a separate loop, because the on pixels should not get their colour from row -1
  for(uint8_t x=0; x<COLS; ++x){
    //first row -> turn off the off pixels after propagation of colour
    if(!onPixels[0][x]){
      new_colors[xy2i(x,0)].setHSV(0,0,0);
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
