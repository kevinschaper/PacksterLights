#include <Arduino.h>
#include <FastLED.h>

#define NUM_LEDS 48
#define LEFT_DATA_PIN 6
#define RIGHT_DATA_PIN 7
#define INITIAL_BRIGHTNESS 30
#define MIN_BRIGHTNESS 1
#define MAX_BRIGHTNESS 255
#define BRIGHTNESS_PIN 0

//for fire
#define COOLING  70
#define SPARKING 50
#define FRAMES_PER_SECOND 30
bool gReverseDirection = true;


CRGB left[NUM_LEDS];
CRGB right[NUM_LEDS];
CRGB leds[NUM_LEDS];

CRGBPalette16 gPal;

void setup() {
  Serial.begin(57600);
  Serial.println("resetting");
  LEDS.addLeds<WS2812, LEFT_DATA_PIN, GRB>(left, NUM_LEDS).setCorrection( TypicalLEDStrip );
  LEDS.addLeds<WS2812, RIGHT_DATA_PIN, GRB>(right, NUM_LEDS).setCorrection( TypicalLEDStrip );
  LEDS.setBrightness(INITIAL_BRIGHTNESS);

  //for fire
  gPal = HeatColors_p;
}

void readBrightness() {
  FastLED.setBrightness(map(analogRead(0),0,1023,MIN_BRIGHTNESS,MAX_BRIGHTNESS));
}

void fadeall(int rate) {
  for(int i = 0; i < NUM_LEDS; i++) {
    left[i].nscale8(rate);
    right[i].nscale8(rate);
  }
}

void fadeall() {
  fadeall(250);
}

void runningLights() {
  for(int i = 0; i < NUM_LEDS; i++) {
    readBrightness();
    left[i] = CRGB::OrangeRed;
    right[i] = CRGB::OrangeRed;
  }
  FastLED.show();
}

void hazardLights() {

  for(int i = 0; i < NUM_LEDS; i++) {
    left[i] = CRGB::OrangeRed;
  }
  for(int i = 0; i < NUM_LEDS; i++) {
    right[i].nscale8(50);
  }
  readBrightness();
  FastLED.show();
  delay(250);
  for(int i = 0; i < NUM_LEDS; i++) {
    right[i] = CRGB::OrangeRed;
  }
  for(int i = 0; i < NUM_LEDS; i++) {
    left[i].nscale8(50);
  }
  FastLED.setBrightness(map(analogRead(0),0,1023,0,255));
  FastLED.show();
  delay(250);

}

void chase() {

  CRGB color = CRGB(random8(),random8(),random8());
  int delayTime = random8(0,50);
  int fadeRate = random8(100,250);
  for (int i = NUM_LEDS-1; i >= 0 ; i--) {
    readBrightness();
    left[i] = color;
    right[i] = color;
    FastLED.show();
    fadeall(fadeRate);
    FastLED.show();
    delay(delayTime);
  }

}


void sparkles(bool purple) {

  u_int8_t randoSize = random8(NUM_LEDS/4);
  u_int8_t randomLeds[randoSize];

  for (int i = 0; i < randoSize ; i++ ) {
      randomLeds[i] = random8(NUM_LEDS-1);
      if (purple) {
        left[randomLeds[i]].setRGB(68, 5, 186);
        right[randomLeds[i]].setRGB(68, 5, 186);
      } else {
        left[randomLeds[i]].setRGB(random8(), random8(), random8());
        right[randomLeds[i]].setRGB(random8(), random8(), random8());
      }

  }

  readBrightness();
  FastLED.show();
  delay(30);
  fadeall(200);
  readBrightness();
  FastLED.show();

}

void sparkles() {
  sparkles(false);
}

void purpleSparkles() {
  sparkles(true);
}

void Fire2012WithPalette()
{
// Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( gPal, colorindex);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      left[pixelnumber] = color;
      right[pixelnumber] = color;
    }
}

void fireLoop() {
  random16_add_entropy( random());
  Fire2012WithPalette(); // run simulation frame, using palette colors
  FastLED.show(); // display this frame
  FastLED.delay(1000 / FRAMES_PER_SECOND);
}

void loop() {

  readBrightness();

  int setting = map(analogRead(1),0, 1023, 1, 6);

  switch(setting) {
    case 1:
      runningLights();
      break;
    case 2:
      hazardLights();
      break;
    case 3:
      chase();
      break;
    case 4:
      sparkles();
      break;
    case 5:
      purpleSparkles();
      break;
    case 6:
      fireLoop();
      break;
  }

}
