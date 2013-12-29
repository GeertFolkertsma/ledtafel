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
#define WALKER_TIMESTEP 50
uint16_t step_period = 1;
uint8_t leds_index = 0;
uint8_t colour_counter=0; //keep track of the remaining colours in led (100-leds_left)
uint8_t step_counter=0; //keep track of the remaining steps of the currently-moving colour

const CRGB moving_colours[] = {CRGB::Red, CRGB::Green, CRGB::Blue};

void seed_walker(uint8_t start=0){
  //restart the walker
  step_period = 1;
  leds_index = min(start,NUM_LEDS-1);
  colour_counter = 0;
  step_counter = 0;
  // Initialise the white leds
  for(uint8_t i_led=0; i_led != NUM_LEDS; ++i_led){
    walker_leds[i_led] = (i_led>=start) ? CRGB::White : CRGB::Black;
    walker_leds[i_led]/= 2;
  }
}

void step_led(){
  // First, turn off completely the led at step_counter (the light will leave this spot)
  walker_leds[step_counter] = CRGB::Black;
  // Re-set the step period 'countdown timer'
  step_period = 1000/(1+leds_index);
  
  // Then, check whether we still have steps left on the current led
  if(step_counter==0){
    // the current moving led has left the grid -> find a new led to move out
    if(colour_counter==0){
      // it was the last led for this starting position: find a new starting position
      leds_index++;
      colour_counter = 3; //3 colours to move out
      // TODO: leds_index = 100 -> what to do?
    }
    colour_counter--;
    step_counter = leds_index;
    // Pause 1 step before moving out this new led
    return;
  }
  
  if(step_counter==leds_index){
    // Turn off the current colour in the current 'source' led
    if(colour_counter==2){
      // moved out blue
      walker_leds[leds_index] = CRGB::Yellow;
    } else if(colour_counter==1) {
      // moved out green as well
      walker_leds[leds_index] = CRGB::Red;
    } else if(colour_counter==0){
      walker_leds[leds_index] = CRGB::Black;
    }
  }
  
  step_counter--;
  // Light up the properly-coloured led in the current position
  walker_leds[step_counter] = moving_colours[colour_counter];
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
  
  seed_walker(1);
}


uint16_t seconds_left = 90*60;


unsigned long prevStepTime = 0;
unsigned long prevCounterTime = 0;
unsigned long prevUpdateTime = 0;
unsigned long prevReportTime = 0;

void loop(){
  if(millis()-prevStepTime >= WALKER_TIMESTEP){
    prevStepTime = millis();
    if(--step_period == 0){
      // step_period is reset inside step_led();
      step_led();
    }
  }
  
  if(millis()-prevCounterTime >= 500){
    prevCounterTime = millis();
    writeDigits(seconds_left--);
  }
  
  if(millis()-prevUpdateTime >= UPDATE_TIME){
    prevUpdateTime = millis();
    for(uint8_t i_led=0; i_led != NUM_LEDS; ++i_led){
      for(uint8_t i=0; i!=3; ++i)
        leds[i_led][i] = (uint8_t) (((uint16_t)leds[i_led][i]*19 + (uint16_t)walker_leds[i_led][i]/2 + (uint16_t)counter_leds[i_led][i]/3)/20);
    }
    update();
  }
  
  if(millis()-prevReportTime >= 500){
    prevReportTime = millis();
    Serial.println(step_period);
  }
}

