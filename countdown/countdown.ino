#include "FastSPI_LED2.h"

#define ROWS 10
#define COLS 10
#define NUM_LEDS (ROWS*COLS)

CRGB leds[NUM_LEDS];
CRGB walker_leds[NUM_LEDS]; //separate array that holds the leds that 'walk/move out' of the table
CRGB counter_leds[NUM_LEDS]; //separate array that holds the counter (digits)

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

// Add digits for countdown timer
const boolean zero[] = {
  true, true, true,
  true, false, true,
  true, false, true,
  true, false, true,
  true, true, true
};
boolean one[] = {
  false, false, true,
  false, false, true,
  false, false, true,
  false, false, true,
  false, false, true
};
const boolean two[] = {
  true, true, true,
  false, false, true,
  true, true, true,
  true, false, false,
  true, true, true
};
const boolean three[] = {
  true, true, true,
  false, false, true,
  false, true, true,
  false, false, true,
  true, true, true
};
const boolean four[] = {
  true, false, true,
  true, false, true,
  true, true, true,
  false, false, true,
  false, false, true
};
const boolean five[] = {
  true, true, true,
  true, false, false,
  true, true, true,
  false, false, true,
  true, true, true
};
const boolean six[] = {
  true, true, true,
  true, false, false,
  true, true, true,
  true, false, true,
  true, true, true
};
boolean seven[] = {
  true, true, true,
  false, false, true,
  false, false, true,
  false, false, true,
  false, false, true
};
const boolean eight[] = {
  true, true, true,
  true, false, true,
  true, true, true,
  true, false, true,
  true, true, true
};
const boolean nine[] = {
  true, true, true,
  true, false, true,
  true, true, true,
  false, false, true,
  true, true, true
};


const boolean* digits[] = {zero, one, two, three, four, five, six, seven, eight, nine};


void writeDigit(uint8_t c, uint8_t dx, uint8_t dy, CRGB colour){
  // Digits are 3 by 5 (x by y)
  for(uint8_t y=0; y != 5; ++y){
    Serial.println();
    for(uint8_t x=0; x!=3; ++x){
      if(digits[c][x+3*y]){
        counter_leds[xy2i(dx+x,dy+y)] = colour;
      } else {
        counter_leds[xy2i(dx+x,dy+y)] = CRGB::Black;
      }
    }
  }
}

void writeDigits(uint16_t secs){
  // minutes left: seconds / 60 ; 
  uint8_t mins = secs/60;
  // 10 minutes left: minutes_left / 10
  writeDigit(mins/10, 0, 0, CRGB::Magenta);
  // last digit: minutes left % 10
  writeDigit(mins % 10, 5, 0, CRGB::Magenta);
  // Seconds left: seconds_left-60*mnutes_left
  writeDigit((secs-60*mins)/10, 2, 5, CRGB::Cyan);
  writeDigit((secs-60*mins)%10, 7, 5, CRGB::Cyan);
}

#define UPDATE_TIME 20
#define WALKER_TIMESTEP 100
uint16_t step_period = 1;
uint8_t leds_index = 0;
uint8_t colour_counter=0; //keep track of the remaining colours in led (100-leds_left)
uint8_t step_counter=0; //keep track of the remaining steps of the currently-moving colour

const CRGB moving_colours[] = {CRGB::Blue, CRGB::Green, CRGB::Red};

#define TOTAL_SECONDS ((uint16_t)(90*60))
#define SECONDS_PER_LED ((uint16_t)TOTAL_SECONDS/NUM_LEDS)
#define SECONDS_PER_COLOUR ((uint16_t) SECONDS_PER_LED/3)
#define DECASECONDS_PER_LED (10*SECONDS_PER_LED)
#define DECASECONDS_PER_COLOUR (10*SECONDS_PER_COLOUR)

void draw_walker(uint16_t decasecs){
  // Draw the complete walker for the current amount of seconds left
  
  // 1: determine the led that is currently 'walking out'
  //    Since each led should take equal time to walk out, this is simply 100*s/T
  uint8_t current_led = ((10*TOTAL_SECONDS - decasecs) / DECASECONDS_PER_LED);
  // 2: determine which of the three colours is moving out
  uint16_t decasecs_in_led = DECASECONDS_PER_LED - decasecs % DECASECONDS_PER_LED;
  uint8_t current_colour = ((uint16_t) decasecs_in_led * 3) / DECASECONDS_PER_LED;
  // 3: determine the position of the current colour
  uint8_t current_pos = current_led-1 - ((decasecs_in_led % DECASECONDS_PER_COLOUR) * current_led) / DECASECONDS_PER_COLOUR;
  
  // The following happens at the very end of the last led
  if(current_colour==3){
    return;
  }
  
  // Then, do the colouring!
  for(uint8_t i_led=0; i_led <= current_led; ++i_led){
    // these are all black
    walker_leds[i_led] = CRGB::Black;
  }
  for(uint8_t i_led= min(current_led+1,NUM_LEDS-1); i_led < NUM_LEDS; ++i_led){
    // these are white
    walker_leds[i_led] = CRGB::White;
  }
  // Now, the current led's colour
  switch(current_colour){
    case 0: //blue has left
      walker_leds[current_led] = CRGB::Yellow;
    break;
    case 1: //green has also left
      walker_leds[current_led] = CRGB::Red;
    break;
    break;
    // TODO: default case?
  }
  // Finally, the walking led
  walker_leds[current_pos] = moving_colours[current_colour];
}

void update(){
  LEDS.show();
}


void setup(){
  // Delay in case the LEDs draw a lot of current and kill the power supply
  delay(1000);
  // Let the controller know we're using WS2801 leds, and give a pointer to the current colour array
  LEDS.addLeds<WS2801, RGB>(leds, NUM_LEDS);
  // Limit the brightness somewhat (scale is 0-255)
  LEDS.setBrightness(64);
  
  for(uint8_t i=0; i<NUM_LEDS; ++i){
    leds[i] = CRGB::White;
    counter_leds[i] = CRGB::Black;
  }
  
  LEDS.show();
  Serial.begin(57600);
}


uint16_t decaseconds_left = 10*TOTAL_SECONDS/4;


unsigned long prevStepTime = 0;
unsigned long prevUpdateTime = 0;
unsigned long prevReportTime = 0;

void loop(){
  if(millis()-prevStepTime >= WALKER_TIMESTEP){
    unsigned long error = millis()-prevStepTime - WALKER_TIMESTEP;
    prevStepTime = millis() - error;
    draw_walker(decaseconds_left--);
    if(decaseconds_left%10==0){
      writeDigits(decaseconds_left/10);
    }
  }
  
  if(millis()-prevUpdateTime >= UPDATE_TIME){
    prevUpdateTime = millis();
    for(uint8_t i_led=0; i_led != NUM_LEDS; ++i_led){
      for(uint8_t i=0; i!=3; ++i)
        // leds[i_led][i] = walker_leds[i_led][i];
        leds[i_led][i] = (uint8_t) (((uint16_t)leds[i_led][i]*19 + (uint16_t)walker_leds[i_led][i]/2 + (uint16_t)counter_leds[i_led][i]/3)/20);
    }
    update();
  }
  
  if(millis()-prevReportTime >= 500){
    prevReportTime = millis();
//    Serial.println(step_period);
  }
}

