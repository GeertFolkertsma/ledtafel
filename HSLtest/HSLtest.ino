#include "SPI.h"
#include "Adafruit_WS2801.h"

Adafruit_WS2801 grid = Adafruit_WS2801((uint8_t)10, (uint8_t)10);


#define LOOP_TIME 50

#define ROWS 10
#define COLS 10

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

void setup(){
  grid.begin();
  grid.show();
  
  for(uint8_t y=0; y<2; ++y)
    for(uint8_t x=0; x<COLS; ++x){
      
      hsl2rgb((double)x/10.0, (double)0.5, (double)0.6, c);
      grid.setPixelColor(x,y,c[0],c[1],c[2]);
    }
  
  grid.show();
}

void loop(){
  //do nothing
}
