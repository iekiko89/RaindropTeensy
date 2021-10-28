#include <Arduino.h>

#include <WS2812Serial.h>
#define USE_WS2812SERIAL
#include <FastLED.h>
#define FASTLED_INTERNAL 


//Data pin for leds are required to be serial
//   Teensy 4.1:  1, 8, 14, 17, 20, 24, 29, 35, 47, 53
//standard boilerplate for my LED codes
#define DATA_PIN             14         // Output pin for LEDs [5]
#define MODE_PIN            15          // Input pin for button to change mode [3]
#define COLOR_ORDER         GRB         // Color order of LED string [GRB]
#define CHIPSET             WS2812B     // LED string type [WS2182B]
#define BRIGHTNESS          96          // Overall brightness [50]
//#define LAST_VISIBLE_LED  191         // Last LED that's visible
//#define MAX_MILLIAMPS     5000        // Max current in mA to draw from supply [500]
//#define SAMPLE_WINDOW     100         // How many ms to sample audio for [100]
//#define DEBOUNCE_MS       20          // Number of ms to debounce the button [20]
//#define LONG_PRESS        500         // Number of ms to hold the button to count as long press [500]
//#define PATTERN_TIME      30          // Seconds to show each pattern on autoChange [30]
//#define kMatrixWidth      27          // Matrix width [15]
//#define kMatrixHeight     12          // Matrix height [11]
//#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
#define NUM_LEDS 192                    // There are 25 LEDs on this strand
#define safety_pixel NUM_LEDS + 1

#define MAX_DIMENSION ((kMatrixWidth>kMatrixHeight) ? kMatrixWidth : kMatrixHeight)   // Largest dimension of matrix
//CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
//CRGB* const leds( leds_plus_safety_pixel);
CRGB leds[NUM_LEDS];
//uint8_t brightness = BRIGHTNESS;
/* helper function is not needed in this pattern
// Helper to map XY coordinates to irregular matrix
uint16_t XY (uint16_t x, uint16_t y) {
  // any out of bounds address maps to the first hidden pixel
  if ( (x >= kMatrixWidth) || (y >= kMatrixHeight) ) {
    return (safety_pixel);
  }

  const uint16_t XYTable[] = {
192,192,192,96,97,98,99,100,101,102,103,104,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,
192,192,105,106,107,108,109,110,111,112,113,114,115,192,192,192,192,192,192,192,192,192,192,192,192,192,192,
192,116,117,118,119,120,121,122,123,124,125,126,127,128,192,192,192,192,192,192,192,192,192,192,192,192,192,
129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,192,192,192,192,192,192,192,192,192,192,192,192,
144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,0,1,2,3,4,5,6,7,8,192,192,192,
192,159,160,161,162,163,164,165,166,167,168,169,170,171,9,10,11,12,13,14,15,16,17,18,19,192,192,
192,192,172,173,174,175,176,177,178,179,180,181,182,20,21,22,23,24,25,26,27,28,29,30,31,32,192,
192,192,192,183,184,185,186,187,188,189,190,191,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
192,192,192,192,192,192,192,192,192,192,192,192,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,
192,192,192,192,192,192,192,192,192,192,192,192,192,63,64,65,66,67,68,69,70,71,72,73,74,75,192,
192,192,192,192,192,192,192,192,192,192,192,192,192,192,76,77,78,79,80,81,82,83,84,85,86,192,192,
192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,87,88,89,90,91,92,93,94,95,192,192,192
  };

  uint16_t i = (y * kMatrixWidth) + x;
  uint16_t j = XYTable[i];
  return j;
}
*/

//define pattern specific Global Variables
byte brightness[NUM_LEDS];      // The brightness of each LED is controlled individually through this array
byte minHue =0;                 // Keeps the hue within a certain range - this is the lower limit
byte maxHue =30;                // Keeps the hue within a certain range - this is upper limit
byte maxBrightness = 200;       // Limits the brightness of the LEDs
byte hue[NUM_LEDS];             // The hue of each LED is controlled individually through this array 
byte startVal[NUM_LEDS];        // The startVal array helps to randomise the LED pattern




void setup() {
  Serial.begin(57600);
  Serial.println("resetting");
  LEDS.addLeds<WS2812SERIAL,DATA_PIN,RGB>(leds,NUM_LEDS);
  LEDS.setBrightness(BRIGHTNESS);

  //Clear the LED strip
  FastLED.clear();
  //This helps to create a truly random pattern each time
  random16_add_entropy(random());

  //Initialise the startVal and hue arrays
  for(int i = 0; i<NUM_LEDS; i++){
    startVal[i] = random8();
    hue[i] = random(minHue, maxHue);
  }  
}

void loop() {
  //Cycle through each LED
  for(int i = 0; i<NUM_LEDS; i++){
    //The brightness of each LED is based on the startValue, 
    //and has a sinusoidal pattern
    startVal[i]++;
    brightness[i] = sin8(startVal[i]);
    
    //Set the hue and brightness of each LED
    leds[i] = CHSV(hue[i], 255, map(brightness[i],0,255,0,maxBrightness));
    
    //This helps to further randomise the pattern
    if(random(1000)<100){
      startVal[i] = startVal[i]+2;
    }
    
    //The hue will only change when the LED is "off"
    if(brightness[i] <3){
      //Randomise the hue for the next on off cycle
      hue[i] = random(minHue, maxHue); 
      
      //Each time an LED turns off, the hue "range" is incremented.
      //This allows the LEDs to transition through each colour of the rainbow
      minHue++;
      maxHue++;
    }
  }
  FastLED.show();   // Show the next frame of the LED pattern
  delay(10);        // Slow down the animation slightly
}
