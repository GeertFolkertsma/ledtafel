#include "SPI.h"
#include "Adafruit_WS2801.h"

Adafruit_WS2801 grid = Adafruit_WS2801((uint8_t)10, (uint8_t)10);


#define LOOP_TIME 50
#define UPDATE_TIME 3000

#define ROWS 10
#define COLS 10
#define _H 0
#define _S 1
#define _L 2
// colors[y][x][HSL]
byte colors[ROWS][COLS][3];
byte new_colors[ROWS][COLS][3];


uint8_t distance, randval;

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

 
void updateColors(){
  randomSeed(analogRead(0));
  
  for(uint8_t y=0; y<ROWS; ++y)
    for(uint8_t x=0; x<COLS; ++x){
      // Determine colour (hsl space) for this pixel
      distance = abs(y-4.5) + abs(x-4.5); //centre is located at (0,5)
      randval = random(0,10);
      // hue: start at 0 (red), move to 100° (yellow/green-ish) at max distance of 15
      new_colors[y][x][_H] = 6*max(0,(distance*3)/3 + randval/2-2);// + randval;
      // saturation: dist 0 -> 100; 15 -> 0
      new_colors[y][x][_S] = 100;
      // colors[y][x][_S] = colors[y][x][_S]*5/6 + (100-distance*2)/6;
      // lightness/luminosity: 80 at 0, 0 at end
      new_colors[y][x][_L] = max(0,50-distance*3 - randval);
      // colors[y][x][_L] = 50;
    }
}

void sendColors(){
  for(uint8_t y=0; y<ROWS; ++y)
    for(uint8_t x=0; x<COLS; ++x){
      for(uint8_t i=0; i<3; ++i)
        colors[y][x][i] = (uint8_t) (colors[y][x][i]*19.0/20.0 + new_colors[y][x][i]/20.0);
      
      hsl2rgb((double)colors[y][x][_H]/360.0,(double)colors[y][x][_S]/100.0,(double)colors[y][x][_L]/100.0,c);
      grid.setPixelColor(x,y,c[0],c[1],c[2]);
    }
    
  grid.show();
}


void setup(){
  grid.begin();
  grid.show();
  
  // Turn off all LEDs
  for(uint8_t y=0; y<ROWS; ++y)
    for(uint8_t x=0; x<COLS; ++x)
      for(uint8_t i=0; i<3; ++i)
        colors[y][x][i] = 0;
  sendColors();
  
  Serial.begin(57600);
  Serial.println("Haardvuur");
}


unsigned long lastLoopTime = 0;
unsigned long lastUpdateTime = 0;
void loop(){
  if(millis()-lastUpdateTime > UPDATE_TIME){
    updateColors();
    lastUpdateTime = millis();
  }
  sendColors();

  Serial.print(millis()-lastLoopTime);
  Serial.print(" - ");
  
  while(millis()-lastLoopTime < LOOP_TIME){
    //wait
  }
  Serial.println(millis()-lastLoopTime);
  lastLoopTime = millis();
}







