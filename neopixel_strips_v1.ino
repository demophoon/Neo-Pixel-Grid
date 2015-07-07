#include "FastLED.h"

// How many leds in your strip?
#define STRIPS 20
#define NUM_LEDS 21
// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806, define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 22
#define CLOCK_PIN 13

// Define the array of leds
CRGB leds[STRIPS * NUM_LEDS];
int coordinates[NUM_LEDS];

// Animation Variables

int drops[30][3];

void setup() {
  Serial.begin(9600);

  LEDS.setBrightness(32);
  FastLED.addLeds<NEOPIXEL, 22>(leds, STRIPS * 0, STRIPS);
  FastLED.addLeds<NEOPIXEL, 23>(leds, STRIPS * 1, STRIPS);
  FastLED.addLeds<NEOPIXEL, 24>(leds, STRIPS * 2, STRIPS);
  FastLED.addLeds<NEOPIXEL, 25>(leds, STRIPS * 3, STRIPS);
  FastLED.addLeds<NEOPIXEL, 26>(leds, STRIPS * 4, STRIPS);
  FastLED.addLeds<NEOPIXEL, 27>(leds, STRIPS * 5, STRIPS);
  FastLED.addLeds<NEOPIXEL, 28>(leds, STRIPS * 6, STRIPS);
  FastLED.addLeds<NEOPIXEL, 29>(leds, STRIPS * 7, STRIPS);
  FastLED.addLeds<NEOPIXEL, 30>(leds, STRIPS * 8, STRIPS);
  FastLED.addLeds<NEOPIXEL, 31>(leds, STRIPS * 9, STRIPS);
  FastLED.addLeds<NEOPIXEL, 32>(leds, STRIPS * 10, STRIPS);
  FastLED.addLeds<NEOPIXEL, 33>(leds, STRIPS * 11, STRIPS);
  FastLED.addLeds<NEOPIXEL, 34>(leds, STRIPS * 12, STRIPS);
  FastLED.addLeds<NEOPIXEL, 35>(leds, STRIPS * 13, STRIPS);
  FastLED.addLeds<NEOPIXEL, 36>(leds, STRIPS * 14, STRIPS);
  FastLED.addLeds<NEOPIXEL, 37>(leds, STRIPS * 15, STRIPS);
  FastLED.addLeds<NEOPIXEL, 38>(leds, STRIPS * 16, STRIPS);
  FastLED.addLeds<NEOPIXEL, 39>(leds, STRIPS * 17, STRIPS);
  FastLED.addLeds<NEOPIXEL, 44>(leds, STRIPS * 18, STRIPS);
  FastLED.addLeds<NEOPIXEL, 45>(leds, STRIPS * 19, STRIPS);
  FastLED.addLeds<NEOPIXEL, 52>(leds, STRIPS * 20, STRIPS);

  FastLED.showColor(CRGB::Black);


  for (int i=0; i<30; i++) {
    drops[i][0] = random(0, STRIPS + 1);
    drops[i][1] = random(1, NUM_LEDS);
    drops[i][2] = random(1, 50);
  }

}

int screenRefresh = 2;
int screenDelay = 25;
int mode = 7;
int flag = 0;
int frames = 0;

int customAnimation[10] = {0};
int primaryColor = 4;
int secondaryColor = 2;
int tertiaryColor = 3;

CRGB getRGBColor(int color) {
  switch (color) {
    case 0:
      return CRGB::White;
      break;
    case 1:
      return CRGB::Black;
      break;
    case 2:
      return CRGB::Red;
      break;
    case 3:
      return CRGB::Orange;
      break;
    case 4:
      return CRGB::Yellow;
      break;
  }
}

void loop() {
  frames++;
  if (frames > 100) {
    mode++;
    frames = 0;
    if (mode > 8) {
      mode = 0;
    }
  }
  if (screenRefresh == 2) {
    FastLED.show();
  }
  switch (mode) {
    case 0:
      rain();
      break;
    case 1:
      fadeUp();
      break;
    case 2:
      stars();
      break;
    case 3:
      fire();
      break;
    case 4:
      // Custom fire
      fadeOut((int)random(25,75));

      int hsv[3];
      rgbToHsl(getRGBColor(primaryColor), hsv);
      for (int i=0; i<random(STRIPS*2); i++) {
        leds[coord(random(STRIPS + 1),0)] = CHSV(random(hsv[0] - 20, hsv[0] + 20) % 255, random(max(0, hsv[1] - 10), min(255, hsv[1] + 10)), hsv[2]);
      }
      for(int i = 0; i < STRIPS * NUM_LEDS; i+=random(5)) {
        if (leds[i] && random(100) == 5) {
          leds[i] += CRGB(getRGBColor(secondaryColor).r / 10, getRGBColor(secondaryColor).g / 10, getRGBColor(secondaryColor).b / 10);
        }
      }
      fadeUp();
      for (int i=0; i<STRIPS+1; i++) {
        leds[coord(random(STRIPS + 1),1)].fadeToBlackBy(64);
      }
      break;
    case 5:
      // 4 Quadrants
      for (int x=0; x<NUM_LEDS; x++) {
        for (int y=0; y<STRIPS; y++) {
          if (x < NUM_LEDS/2 && y < STRIPS/2 || x >= NUM_LEDS/2 && y >= STRIPS/2 ) {
            leds[coord(x,y)] = getRGBColor(primaryColor);
          } else {
            leds[coord(x,y)] = getRGBColor(secondaryColor);
          }
        }
      }
      break;
    case 6:
      // White Noise
      fadeOut(100);
      for (int x=0; x<STRIPS*NUM_LEDS; x++) {
        if (random8(50) == 0) {
          leds[random16(STRIPS*NUM_LEDS)] = CRGB::White;
        }
      }
      break;
    case 7:
      // Grid
      flag = 0;
      for (int x=0; x<NUM_LEDS; x++) {
        for (int y=0; y<STRIPS; y++) {
          if (x%4 == 0 && y%4 == 0) {
            leds[coord(x,y)] = CRGB::Black;
          } else if (x%4 == 0 || y%4 == 0) {
            leds[coord(x,y)] = getRGBColor(secondaryColor);
          } else {
            leds[coord(x,y)] = getRGBColor(primaryColor);
          }
        }
      }
      break;
      case 8:
        // Android Nexus Wallpaper
        fadeOut(64);
        for (int i=0; i<10; i++) {

          if (drops[i][2] > 3) {
            drops[i][2] = random(4);
          }

          if (drops[i][0] <= 0 || drops[i][0] >= NUM_LEDS || drops[i][1] <= 0 || drops[i][1] >= STRIPS) {
            drops[i][0] = random(1, STRIPS);
            drops[i][1] = random(1, NUM_LEDS);
            drops[i][2] = random(4);
          }

          // Move Pixel
          switch(drops[i][2]) {
            case 0:
              drops[i][0] += 1;
              break;
            case 1:
              drops[i][0] -= 1;
              break;
            case 2:
              drops[i][1] += 1;
              break;
            case 3:
              drops[i][1] -= 1;
              break;
          }

          // Get Color
          switch(i%4) {
            case 0:
              leds[coord(drops[i][0], drops[i][1])] = CRGB::Red;
              break;
            case 1:
              leds[coord(drops[i][0], drops[i][1])] = CRGB::Blue;
              break;
            case 2:
              leds[coord(drops[i][0], drops[i][1])] = CRGB::Yellow;
              break;
            case 3:
              leds[coord(drops[i][0], drops[i][1])] = CRGB::Green;
              break;
          }
        }

  }
  delay(screenDelay);
}

void processMessage(int *msg) {
  int fn = msg[0];
  switch (fn) {
    case 0:
      // TODO
      //leds[coord(msg[1], msg[2])] = makeColor(msg[3], msg[4], msg[5]);
      break;
    case 1:
      screenRefresh = msg[1] % 3;
      break;
    case 2:
      // TODO
      //FastLED.showColor(makeColor(msg[1], msg[2], msg[3]));
      break;
  }
}

void serialEvent() {
  int msg[10];
  int msgPos = 0;
  for(int x=0; x<10; x++) {
    msg[x] = 0;
  }
  while (Serial.available() > 0) {
    int it = Serial.read();
    for (int x=0; x<it; x++) {
      delay(1);
      if (Serial.available() > 0) {
        msg[x] = Serial.read();
      } else {
        msg[x] = 0;
      }
    }
    processMessage(msg);
  }
}

// Animations

void fadeLeft() {
  for (int x=0; x<NUM_LEDS-1; x++) {
    for (int y=0; y<STRIPS; y++) {
      leds[coord(x,y)] = leds[coord(x+1,y)];
    }
  }
  fadeOut(64);
  leds[coord(10, 10)] = CRGB::White;
}

void fadeDown() {
  for (int y=0; y<STRIPS-1; y++) {
    for (int x=0; x<NUM_LEDS; x++) {
      leds[coord(x,y)] = leds[coord(x,y+1)];
    }
  }
  fadeOut(64);
  leds[coord(10, 10)] = CRGB::White;
}

void fadeUp() {
  for (int y=STRIPS-1; y>0; y--) {
    for (int x=0; x<NUM_LEDS; x++) {
      leds[coord(x,y)] = leds[coord(x,y-1)];
    }
  }
}

void fadeRight() {
  for (int x=NUM_LEDS-1; x>0; x--) {
    for (int y=0; y<STRIPS; y++) {
      leds[coord(x,y)] = leds[coord(x-1,y)];
    }
  }
  fadeOut(64);
  leds[coord(10, 10)] = CRGB::White;
}


void stars() {
  fadeOut(128);
  leds[coord(random(STRIPS + 1),random(NUM_LEDS + 1))] = getRGBColor(primaryColor);
}

void rain() {
  fadeOut(128);
  flag = 0;
  for (int i=0; i<30; i++) {

    if (drops[i][2] > 254) {
      drops[i][2] = 0;
    }
    drops[i][2]++;

    if (drops[i][1] == 0) {
      if (flag < 3) {
        drops[i][0] = random(0, STRIPS + 1);
        drops[i][1] = NUM_LEDS;
      } else {
        drops[i][1] = 1;
      }
      flag++;
    }
    drops[i][1]--;
    leds[coord(drops[i][0], drops[i][1])] = CHSV(drops[i][2], 255, 255);
  }
}

void fire() {
  fadeOut((int)random(50,100));

  for (int i=0; i<random(STRIPS*2); i++) {
    leds[coord(random(STRIPS + 1),0)] = CHSV(random(40), random(200, 255), 255);
  }
  for(int i = 0; i < STRIPS * NUM_LEDS; i++) {
    if (leds[i] && random(100) == 5) {
      leds[i] += CRGB(15, 5, 0);
    }
  }
  fadeUp();
  for (int i=0; i<STRIPS+1; i++) {
    leds[coord(random(STRIPS + 1),1)].fadeToBlackBy(64);
  }
}

void fadeOut(double amount) {
  fadeOut((int)amount*256);
}

void fadeOut(int amount) {
  for(int i = 0; i < STRIPS * NUM_LEDS; i+=1) {
    leds[i].fadeToBlackBy(amount);
  }
}

// Helper Functions

void rgbToHsl(CRGB color, int hsl[]) {
    byte r = color.r;
    byte g = color.g;
    byte b = color.b;
    double rd = (double) r/255;
    double gd = (double) g/255;
    double bd = (double) b/255;
    double maxn = max(rd, max(gd, bd));
    double minn = min(rd, min(gd, bd));
    double h, s, l = (maxn + minn) / 2;

    if (maxn == minn) {
        h = s = 0; // achromatic
    } else {
        double d = maxn - minn;
        s = l > 0.5 ? d / (2 - maxn - minn) : d / (maxn + minn);
        if (maxn == rd) {
            h = (gd - bd) / d + (gd < bd ? 6 : 0);
        } else if (maxn == gd) {
            h = (bd - rd) / d + 2;
        } else if (maxn == bd) {
            h = (rd - gd) / d + 4;
        }
        h /= 6;
    }
    hsl[0] = h*256;
    hsl[1] = s*256;
    hsl[2] = l*256;
}


int coord(int x, int y) {
  if (x == 11) {
    if (y == 0) {
      return NUM_LEDS*2 + STRIPS;
    }
    y--;
  }
  if (y >= NUM_LEDS || y < 0) {
    y = NUM_LEDS*2;
    x = STRIPS*2;
  }
  return (x * STRIPS + y);
}

// Extra Functions

void loop2() {
  static_noise();
}

void static_noise() {
  int delay_num = 100;
  // First slide the led in one direction
  for(int i = 0; i < STRIPS * NUM_LEDS; i+=1) {
    if (random(4) == 1) {
      leds[i] = CRGB::White;
    } else {
      leds[i] = CRGB::Black;
    }
  }
  FastLED.show();
  delay(delay_num);
}

void loopx() {
  for (int x=0; x<STRIPS; x++) {
    leds[coord(x, x)] = CRGB::Red;
  }
  for (int x=0; x<STRIPS; x++) {
    leds[coord(STRIPS - x, x)] = CRGB::Blue;
  }
  FastLED.show();
  delay(1000);
}

void rainbow() {
  static uint8_t hue = 0;
  FastLED.showColor(CHSV(hue+=1, 255, 128));
  delay(10);
}

void loop3() {
  int delay_num = 0;
      // First slide the led in one direction
    for(int i = 0; i < STRIPS * NUM_LEDS; i+=1) {
            leds[i] = CRGB::Cyan;
                FastLED.show();
                delay(delay_num);
                leds[i] = CRGB::Red;
        }
        for(int i = 1; i < STRIPS * NUM_LEDS; i+=1) {
            leds[i] = CRGB::Black;
        }
}

void loop4() {
  int delay_num = 0;
      // First slide the led in one direction
    for(int i = 0; i < STRIPS * NUM_LEDS; i+=1) {
        // Set the i'th led to red
            leds[i] = CRGB::Cyan;
        // Show the leds
        }
        FastLED.show();
        delay(delay_num);
        for(int i = 1; i < STRIPS * NUM_LEDS; i+=1) {
            leds[i] = CRGB::Black;
        }
        FastLED.show();
        delay(delay_num);
}
