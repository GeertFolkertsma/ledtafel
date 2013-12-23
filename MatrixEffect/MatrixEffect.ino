#include "FastSPI_LED2.h"

#define ROWS 10
#define COLS 10
#define NUM_LEDS (ROWS*COLS)

CRGB leds[NUM_LEDS];

#define LOOP_TIME 50
#define UPDATE_TIME 150

#define _R 0
#define _G 1
#define _B 2
// colors[y][x][RGB]
byte colors[ROWS][COLS][3];
byte new_colors[ROWS][COLS][3];


// Return 0-based led index for 0-based x and y grid co-ordinate values
// The LED string snakes through the grid, so every row the positive x direction switches
uint8_t xy2i(uint8_t x, uint8_t y){
  if(y%2){
    // It's an uneven row: x should be reversed---or subtracted from the next row's first i
    return COLS*(y+1) - 1 - x;
  } else {
    // Even row: simply do COLS*y + x
    return COLS*y + x;
  }
}

void sendColors(){
  for(uint8_t y=0; y<ROWS; ++y)
    for(uint8_t x=0; x<COLS; ++x){
      for(uint8_t i=0; i<3; ++i)
        colors[y][x][i] = (uint8_t) (colors[y][x][i]*19.0/20.0 + new_colors[y][x][i]/20.0);
      
      leds[xy2i(x,y)] = CRGB(colors[y][x][0],colors[y][x][1],colors[y][x][2]);
      // grid.setPixelColor(x,y,colors[y][x][0],colors[y][x][1],colors[y][x][2]);
    }
  // grid.show();
  LEDS.show();
}

/*
#define NUM_RAND_COLOURS 5
uint8_t rand_colours[NUM_RAND_COLOURS][3] = {
  0, 255, 0,
  255, 0, 0,
  0, 0, 255,
  255, 127, 0,
  100, 0, 50
};*/

byte c[3];
double hue2rgb(double p, double q, double t) {
    if(t < 0) t += 1;
    if(t > 1) t -= 1;
    if(t < 1.0/6.0) return p + (q - p) * 6 * t;
    if(t < 1.0/2.0) return q;
    if(t < 2.0/3.0) return p + (q - p) * (2.0/3.0 - t) * 6;
    return p;
}
void hsl2rgb(double h, double s, double l, byte rgb[]) {
    double r, g, b;

    if (s == 0) {
        r = g = b = l; // achromatic
    } else {
        double q = l < 0.5 ? l * (1 + s) : l + s - l * s;
        double p = 2 * l - q;
        r = hue2rgb(p, q, h + 1.0/3.0);
        g = hue2rgb(p, q, h);
        b = hue2rgb(p, q, h - 1.0/3.0);
    }

    rgb[0] = r * 255;
    rgb[1] = g * 255;
    rgb[2] = b * 255;
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
  if(random(0,10)>6)
    onPixels[0][random(0,COLS)] = true;
  for(uint8_t x=0; x<COLS; ++x){
    //first row
    if(onPixels[0][x]){
      // Determine a random hue
      hsl2rgb((double)random(0,21)/20.0, (double)1.0, (double)0.5, c);
      
      for(uint8_t i=0; i<3; ++i)
        new_colors[0][x][i] = c[i];
    }
  }
  for(uint8_t y=ROWS-1; y>0; --y)
    for(uint8_t x=0; x<COLS; ++x){
      if(onPixels[y][x]){
        for(uint8_t i=0; i<3; ++i)
          new_colors[y][x][i] = new_colors[y-1][x][i];
      } else {
        for(uint8_t i=0; i<3; ++i)
          new_colors[y][x][i] = 0;
      }
    }
  for(uint8_t x=0; x<COLS; ++x){
    //first row -> turn off the off pixels after propagation of colour
    if(!onPixels[0][x]){
      new_colors[0][x][_G] = 0;
      new_colors[0][x][_R] = 0;
      new_colors[0][x][_B] = 0;
    }
  }
}

void setup(){
  delay(1000);
  LEDS.addLeds<WS2801, RGB>(leds, NUM_LEDS);
  LEDS.setBrightness(64);
  
  for(uint8_t i=0; i<NUM_LEDS; ++i){
    leds[i] = CRGB::Black;
  }
  LEDS.show();
  
  // Turn off all LEDs, initialise pixel seeds on first row
  for(uint8_t y=0; y<ROWS; ++y)
    for(uint8_t x=0; x<COLS; ++x){
      // clear pixels
      for(uint8_t i=0; i<3; ++i)
        colors[y][x][i] = 100;
      //seed pixels
      
      if(y==0)
        onPixels[y][x] = (random(0,10)>5);
      else
        onPixels[y][x] = false;
      
    }
  sendColors();
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
