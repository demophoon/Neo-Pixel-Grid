#include "FastLED.h"

// How many leds in your strip?
#define STRIPS 21      // X
#define NUM_LEDS 20    // Y

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806, define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 22
#define CLOCK_PIN 13

// Define the array of leds
CRGB leds[STRIPS * NUM_LEDS];

//const int X = 16;
//const int Y = 16;
const int X = 21;
const int Y = 20;

class Animation {
  public:
    virtual void draw() = 0;
    virtual void serial_event(String) = 0;
    // Global Helper Animations
    void fadeOut(double);
    void fadeOut(int);

    // Global Intensity Helper
    float intensity();
    void set_intensity(float);

    // Global Color Options
    CRGB first_color();
    void set_first_color(CRGB);
    CRGB second_color();
    void set_second_color(CRGB);
    CRGB third_color();
    void set_third_color(CRGB);
    CRGB fourth_color();
    void set_fourth_color(CRGB);
    CRGB get_led_color(int, int);
    void set_led_color(int, int, CRGB);
    void fade_to_color(int, int, CRGB, int);
    void add_led_color(int, int, CRGB);
    boolean is_valid_light(int, int);
    
  private:
    CRGB _first_color;
    CRGB _second_color;
    CRGB _third_color;
    CRGB _fourth_color;
    int coord(int, int);
    float _intensity;
};

void Animation::fadeOut(double amount) {
  Animation::fadeOut((int)(amount*256));
}
void Animation::fadeOut(int amount) {
  for(int i = 0; i < STRIPS * NUM_LEDS; i+=1) {
    leds[i].fadeToBlackBy(amount);
  }
}
void Animation::set_first_color(CRGB color) {
  _first_color = color;
}
CRGB Animation::first_color() {
  return _first_color;
}
void Animation::set_second_color(CRGB color) {
  _second_color = color;
}
CRGB Animation::second_color() {
  return _second_color;
}
void Animation::set_third_color(CRGB color) {
  _third_color = color;
}
CRGB Animation::third_color() {
  return _third_color;
}
void Animation::set_fourth_color(CRGB color) {
  _fourth_color = color;
}
CRGB Animation::fourth_color() {
  return _fourth_color;
}
CRGB Animation::get_led_color(int x, int y) {
    return leds[coord(x, y)];
}
void Animation::set_led_color(int x, int y, CRGB color) {
    if (!is_valid_light(x, y))
      return;
    leds[coord(x, y)] = color;
    //fade_to_color(x, y, color, 25);
}
void Animation::fade_to_color(int x, int y, CRGB color, int fade) {
  CRGB target = get_led_color(x, y);
  target.r += min(max(-1 * fade, color.r - target.r), fade);
  target.g += min(max(-1 * fade, color.g - target.g), fade);
  target.b += min(max(-1 * fade, color.b - target.b), fade);
  if (!is_valid_light(x, y))
    return;
  leds[coord(x, y)] = target;
}

void Animation::add_led_color(int x, int y, CRGB color) {
  CRGB target = get_led_color(x, y);
  target.r = (color.r / 2) + (target.r / 2);
  target.g = (color.g / 2) + (target.g / 2);
  target.b = (color.b / 2) + (target.b / 2);
  set_led_color(x, y, target);
}

boolean Animation::is_valid_light(int x, int y){
  if (x == 11 && y == 0) {
    return false;
  }
  if (x >= STRIPS || y >= NUM_LEDS || x >= X || y >= Y || x < 0 || y < 0) {
    return false;
  }
  return true;
}

int Animation::coord(int x, int y) {
  if (x == 11) {
    if (y == 0) {
      y = 1;
    }
    y -= 1;
  }
  int ret = (x * NUM_LEDS + y);
  return ret;
}

void Animation::set_intensity(float intensity) {
  if (intensity > 1.0) {
    intensity = 1.0;
  } else if (intensity < 0) {
    intensity = 0.0;
  }
  _intensity = intensity;
}
float Animation::intensity() {
  return _intensity;
}

class Noise : public Animation {
  public:
    Noise() {
      last_millis = millis();
      last_beat = millis();
    }
    void draw() {
      fadeOut(100);
      for (int x=0; x<X*Y; x+=1) {
        if (random8(255) < 50 * intensity()) {
          set_led_color(random16(X), random16(Y), first_color());
        }
      }
      for (int x=1; x<X*Y; x+=1) {
        if (random8(255) < 50 * intensity()) {
          set_led_color(random16(X), random16(Y), second_color());
        }
      }
      int current = millis() - last_millis;
      last_millis = millis();
      set_intensity(max(intensity() - .007, .000));
    }
    void serial_event(String incoming) {
      int current = millis() - last_beat;
      last_beat = millis();
      //FastLED.showColor(CRGB::White);
      set_intensity(intensity() + .10);
    }
  private:
    int last_millis;
    int last_beat;
};

class FourStrips : public Animation {
  public:
    void draw() {
      for (int x=0; x<X; x++) {
        for (int y=0; y<Y; y++) {
          if (y < Y / 4) {
            set_led_color(x,  y, first_color());
          } else if (y < Y / 2) {
            set_led_color(x,  y, second_color());
          } else if (y < 3 * Y / 4) {
            set_led_color(x,  y, third_color());
          } else {
            set_led_color(x,  y, fourth_color());
          }
        }
      }
      set_intensity(max(intensity() - .01, .000));
      fadeOut(min((-1.0 * intensity()) + 1.0, .99));
    }
    void serial_event(String incoming) {
      set_intensity(intensity() + .15);
      return;
    }
};

class Rain : public Animation {
  public:
    Rain();
    void draw() {
      if (flag == true) {
        flag = false;
        return;
      }
      fadeOut(100);

      for (int i=0; i<X*3; i++) {
        if (drops[i][1] == 0) {
          drops[i][1] = 1;
        }
        drops[i][1]--;
        fade_to_color(drops[i][0],  drops[i][1], first_color(), 255);
      }

      set_intensity(max(intensity() - .01, .000)); 
    }

    void serial_event(String incoming) {
      set_intensity(intensity() + .35);
      for (int i=0; i<(int)(X*3.0 * intensity()); i++) {
        drops[i][0] = random(0, X);
        drops[i][1] = random(0, Y);
      }
    }
  private:
    boolean flag;
    int drops[X*3][2];
};

Rain::Rain() {
  flag = true;
  for (int x=0; x<X*3; x++) {
    drops[x][0] = random(0, X);
    drops[x][1] = random(1, Y);
  }
}

class MusicBars : public Animation {
  public:
    MusicBars();
    void draw() {
      fadeOut(15);

      for (int x=0; x<X; x++) {
        int h = Y / 2;
        set_led_color(x,  h + (beat[x] / 50.0 * h), first_color());
        set_led_color(x,  h - (beat[x] / 50.0 * h), first_color());
      }

      for (int x=0; x<X; x++) {
        if (beat[x] > 0) {
          beat[x] -= 1;
        }
      }
    }

    void serial_event(String incoming) {
      int x = random8(X);
      int i = random8(50);
      if (x > 0) {
        beat[x-1] += i / 2;
      }
      beat[random8(X)] += i;
      if (x < X) {
        beat[x+1] += i / 2;
      }
    }
  private:
    boolean flag;
    int beat[X];
};

MusicBars::MusicBars() {
  flag = true;
  for (int x=0; x<X; x++) {
    beat[x] = 0;
  }
}


class Esso : public Animation {
  public:
    Esso();

    void draw() {
      // Bars
      for (int y=0; y<Y; y++) {
        // Orange
        fade_to_color(1, y, first_color(), fade_first);
        fade_to_color(19, y, first_color(), fade_first);
        // Black
        fade_to_color(0, y, second_color(), fade_second);
        fade_to_color(2, y, second_color(), fade_second);
        fade_to_color(18, y, second_color(), fade_second);
        fade_to_color(20, y, second_color(), fade_second);
        if (y > 5) {
          // Orange
          fade_to_color(10, y, first_color(), fade_first);
          // Black
          fade_to_color(9, y, second_color(), fade_second);
          fade_to_color(11, y, second_color(), fade_second);
        }

        if (y >= 2 && y < 4) {
          // White
          fade_to_color(10, y, CRGB(50,50,50), fade_first);
          // Black
          fade_to_color(9, y, CRGB(0,0,0), fade_second);
          fade_to_color(11, y, CRGB(0,0,0), fade_second);
        }
      }
      fadeOut(base_fade);
    }

    void serial_event(String incoming) {
      int action = incoming[1];
      if (incoming.length() == 1) {
        action = random(3) + 1;
      }

      if (action == 1 || action == 2) {
        for (int y=0; y<Y; y++) {
          // Orange
          fade_to_color(1, y, third_color(), 255);
          fade_to_color(19, y, third_color(), 255);
          fade_to_color(0, y, fourth_color(), 255);
          fade_to_color(2, y, fourth_color(), 255);
          fade_to_color(18, y, fourth_color(), 255);
          fade_to_color(20, y, fourth_color(), 255);
          if (y > 5) {
            // Orange
            fade_to_color(10, y, third_color(), 255);
            fade_to_color(9, y, fourth_color(), 255);
            fade_to_color(11, y, fourth_color(), 255);
          }
        }
      }

      if (action == 2 || action == 3) {
        for (int y=0; y<Y; y++) {
          if (y >= 2 && y < 4) {
            // White
            fade_to_color(10, y, CRGB(255,255,255), 255);
            // Black
            fade_to_color(9, y, CRGB(255,255,255), 255);
            fade_to_color(11, y, CRGB(255,255,255), 255);
          }
        }
      }

    }
    private:
      int base_fade;
      int fade_first;
      int fade_second;
};

Esso::Esso() {
  fade_first = 5;
  fade_second = 15;
  base_fade = min(fade_first, fade_second);
  fade_first += base_fade;
  fade_second += base_fade;
  set_second_color(CRGB::Black);
  set_fourth_color(CRGB::White);
}

class PuppetLabs : public Animation {
  public:
    PuppetLabs();
    void draw() {
      CRGB puppet_primary = CRGB(110, 0, 120);
      CRGB puppet_secondary = CRGB(175, 40, 125);
      CRGB puppet_yellow = CRGB(200, 126, 17);
      set_led_color(3, 1, puppet_primary);
      set_led_color(4, 2, puppet_secondary);
      for (int i=1; i < 16; i++) {
        set_led_color(i+2, 1, puppet_yellow);
      }
      for (int i=2; i < 17; i++) {
        set_led_color(i+2, 2, puppet_yellow);
      }
      
      for (int i=0; i < 13; i++) {
        set_led_color(i/2+2, i+1, puppet_primary);
        set_led_color(i/2+3, i+1, puppet_primary);
      }
           
      set_led_color(18, 3, puppet_secondary);
      set_led_color(18, 4, puppet_secondary);
      
      set_led_color(17, 3, puppet_secondary);
      set_led_color(17, 4, puppet_secondary);
      set_led_color(17, 5, puppet_secondary);
      set_led_color(17, 6, puppet_secondary);

      set_led_color(16, 5, puppet_secondary);
      set_led_color(16, 6, puppet_secondary);
      set_led_color(16, 7, puppet_secondary);
      set_led_color(16, 8, puppet_secondary);

      set_led_color(15, 7, puppet_secondary);
      set_led_color(15, 8, puppet_secondary);
      set_led_color(15, 9, puppet_secondary);
      set_led_color(15, 10, puppet_secondary);

      set_led_color(14, 9, puppet_secondary);
      set_led_color(14, 10, puppet_secondary);
      set_led_color(14, 11, puppet_secondary);
      set_led_color(14, 12, puppet_secondary);
      
      set_led_color(13, 11, puppet_secondary);
      set_led_color(13, 12, puppet_secondary);
      set_led_color(13, 13, puppet_secondary);
      set_led_color(13, 14, puppet_secondary);
      
      set_led_color(13, 15, puppet_secondary);
      set_led_color(13, 16, puppet_secondary);
      set_led_color(13, 17, puppet_secondary);
      set_led_color(13, 18, puppet_secondary);
      set_led_color(13, 19, puppet_secondary);
      
      set_led_color(12, 12, puppet_secondary);
      set_led_color(12, 13, puppet_secondary);
      set_led_color(12, 14, puppet_secondary);
      set_led_color(12, 15, puppet_secondary);
      set_led_color(12, 16, puppet_secondary);
      set_led_color(12, 17, puppet_secondary);
      set_led_color(12, 18, puppet_secondary);
      set_led_color(12, 19, puppet_secondary);
      
      for (int i=7; i < 16; i++) {
        set_led_color(i, 19, puppet_secondary);
      }
      for (int i=8; i < 15; i++) {
        set_led_color(i, 18, puppet_secondary);
      }
      set_led_color(9, 17, puppet_secondary);
      set_led_color(2, 1, CRGB::Black);

//      set_led_color(1, 1, puppet_primary);
//      set_led_color(2, 1, puppet_primary);
//      set_led_color(2, 2, puppet_primary);
//      set_led_color(3, 2, puppet_primary);
//      set_led_color(2, 3, puppet_primary);
//      set_led_color(3, 3, puppet_primary);
//      set_led_color(3, 4, puppet_primary);
//      set_led_color(4, 4, puppet_primary);
//      set_led_color(4, 5, puppet_primary);
//      set_led_color(5, 5, puppet_primary);
//      set_led_color(5, 6, puppet_primary);
//      set_led_color(6, 6, puppet_primary);
//      set_led_color(6, 7, puppet_primary);
//      set_led_color(7, 7, puppet_primary);
//      set_led_color(7, 8, puppet_primary);
//      set_led_color(8, 8, puppet_primary);
    }
    void serial_event(String incoming) {
    }
};

PuppetLabs::PuppetLabs() {
}

class Nexus : public Animation {
  public:
    Nexus();
    void draw() {
      fadeOut(64);
      for (int i=0; i<(intensity() * 30); i++) {
        // Move Trail
        switch(trails[i][2]) {
          case 0:
            trails[i][0] += 1;
            break;
          case 1:
            trails[i][0] -= 1;
            break;
          case 2:
            trails[i][1] += 1;
            break;
          case 3:
            trails[i][1] -= 1;
            break;
        }
        
        if (!is_valid_light(trails[i][0], trails[i][1])) {
          trails[i][0] = random(0, X);
          trails[i][1] = random(0, Y);
          trails[i][2] = random(4);
        }
        
        // Draw Trail
        switch(i%4) {
          case 0:
            fade_to_color(trails[i][0], trails[i][1], first_color(), 200);
            break;
          case 1:
            fade_to_color(trails[i][0], trails[i][1], second_color(), 200);
            break;
          case 2:
            fade_to_color(trails[i][0], trails[i][1], third_color(), 200);
            break;
          case 3:
            fade_to_color(trails[i][0], trails[i][1], fourth_color(), 200);
            break;
        }
      }
      set_intensity(max(intensity() - .01, .000));
    }

    void serial_event(String incoming) {
      set_intensity(intensity() + .15);
    }
  private:
    int trails[30][3];
};

Nexus::Nexus() {
  for (int i=0; i<10; i++) {
    trails[i][0] = random(0, X);
    trails[i][1] = random(0, Y);
    trails[i][2] = random(4);
  }
}

class Squares : public Animation {
  public:
    Squares();
    void draw() {
      if (frame++ < limit) {
        return;
      } else {
        frame = 0;
      }
      fadeOut(200);
      for (int i=0; i<max(X, Y); i++) {
        if (beats[i] >= 0) {
          for (int x=0; x<beats[i]; x++) {
            add_led_color(x, beats[i], first_color());
            add_led_color(beats[i], x, first_color());
            add_led_color(max(X, Y) - x, max(X, Y) - beats[i], first_color());
            add_led_color(max(X, Y) - beats[i], max(X, Y) - x, first_color());
            
            add_led_color(x, max(X, Y) - beats[i], second_color());
            add_led_color(beats[i], max(X, Y) - x, second_color());
            add_led_color(max(X, Y) - x,beats[i], second_color());
            add_led_color(max(X, Y) - beats[i],x, second_color());
          }
          add_led_color(beats[i], beats[i], first_color());
          add_led_color(max(X, Y) - beats[i], max(X, Y) - beats[i], first_color());
          beats[i]--;
        }
      }
    }

    void serial_event(String incoming) {
      for (int i=0; i<max(X, Y); i++) {
        if (beats[i] == -1) {
          beats[i] = max(X, Y) / 2;
          break;
        }
      }
    }
  private:
    int beats[max(X, Y)];
    int limit;
    int frame;
};

Squares::Squares() {
  frame = 0;
  limit = 5;
  for (int i=0; i<max(X, Y); i++) {
    beats[i] = -1;
  }
}

class FlashySquares : public Animation {
  public:
    FlashySquares();
    void draw() {
      fadeOut(150);
      if (frame++ > skip) {
        frame = 0;
      } else {
        return;
      };
      for (int x=0; x<random8(8); x++) {
        int start_x = random8(X/square_size);
        int start_y = random8(Y/square_size);
        if (start_x % 2 == start_y % 2) {
          draw_square(start_x * square_size, start_y * square_size, square_size, first_color());
        } else {
          draw_square(start_x * square_size, start_y * square_size, square_size, second_color());
        }
      }
    }
    void serial_event(String incoming) {
      FastLED.showColor(first_color());
    }
    void draw_square(int, int, int);
    void draw_square(int, int, int, CRGB);
  private:
    int frame;
    int skip;
    int square_size;
};

FlashySquares::FlashySquares() {
  frame = 0;
  skip = 2;
  square_size = 4;
}

void FlashySquares::draw_square(int sx, int sy, int width) {
  draw_square(sx, sy, width, first_color());
}

void FlashySquares::draw_square(int sx, int sy, int width, CRGB color) {
  for (int x=sx; x<sx + width; x++) {
    for (int y=sy; y<sy + width; y++) {
      set_led_color(x, y, color);
    }
  }
}

class Numbers : public Animation {
  public:
    Numbers();
    void draw() {
      for (int x=0; x<3; x++) {
        for (int y=0; y<5; y++) {
          if (digits[x * 3 + y]) {
            fade_to_color(x, y, first_color(), 200);
          }
        }
      }
    }

    void serial_event(String incoming) {
      return;
    }
  private:
    int current_digit;
    boolean digits[15];
};

Numbers::Numbers() {
//  digits[0] = {
//    true, true, true,
//    true, false, true,
//    true, false, true,
//    true, true, true
//  };
}

const int NUM_ANIMATIONS = 9;
Animation *animations[NUM_ANIMATIONS];
Animation *current;

void setup() {
  Serial.begin(9600);

  LEDS.setBrightness(64);
  FastLED.addLeds<NEOPIXEL, 22>(leds, 20 * 0, 20);
  FastLED.addLeds<NEOPIXEL, 23>(leds, 20 * 1, 20);
  FastLED.addLeds<NEOPIXEL, 24>(leds, 20 * 2, 20);
  FastLED.addLeds<NEOPIXEL, 25>(leds, 20 * 3, 20);
  FastLED.addLeds<NEOPIXEL, 26>(leds, 20 * 4, 20);
  FastLED.addLeds<NEOPIXEL, 27>(leds, 20 * 5, 20);
  FastLED.addLeds<NEOPIXEL, 28>(leds, 20 * 6, 20);
  FastLED.addLeds<NEOPIXEL, 29>(leds, 20 * 7, 20);
  FastLED.addLeds<NEOPIXEL, 30>(leds, 20 * 8, 20);
  FastLED.addLeds<NEOPIXEL, 31>(leds, 20 * 9, 20);
  FastLED.addLeds<NEOPIXEL, 32>(leds, 20 * 10, 20);
  FastLED.addLeds<NEOPIXEL, 33>(leds, 20 * 11, 20);
  FastLED.addLeds<NEOPIXEL, 34>(leds, 20 * 12, 20);
  FastLED.addLeds<NEOPIXEL, 35>(leds, 20 * 13, 20);
  FastLED.addLeds<NEOPIXEL, 36>(leds, 20 * 14, 20);
  FastLED.addLeds<NEOPIXEL, 37>(leds, 20 * 15, 20);
  FastLED.addLeds<NEOPIXEL, 38>(leds, 20 * 16, 20);
  FastLED.addLeds<NEOPIXEL, 39>(leds, 20 * 17, 20);
  FastLED.addLeds<NEOPIXEL, 44>(leds, 20 * 18, 20);
  FastLED.addLeds<NEOPIXEL, 45>(leds, 20 * 19, 20);
  FastLED.addLeds<NEOPIXEL, 52>(leds, 20 * 20, 20);

  //FastLED.addLeds<NEOPIXEL, 9>(leds, 0, 120);
  FastLED.showColor(CRGB::Black);
  animations[0] = new FourStrips;
  animations[1] = new Squares;
  animations[2] = new Noise;
  animations[3] = new Rain;
  animations[4] = new MusicBars;
  animations[5] = new Esso;
  animations[6] = new Nexus;
  animations[7] = new FlashySquares;
  animations[8] = new PuppetLabs;

  current = animations[8];
  for (int i=0; i<NUM_ANIMATIONS; i++) {
    animations[i]->set_intensity(.5);
    animations[i]->set_first_color(CRGB::Red);
    animations[i]->set_second_color(CRGB::Green);
    animations[i]->set_third_color(CRGB::Blue);
    animations[i]->set_fourth_color(CRGB::White);
  }
}

int frames = 0;
long max_brightness = 91800 * 1;

void loop() {
  frames++;
  if (frames % 150 == 0) {
    frames = 0;
    //current = animations[random(NUM_ANIMATIONS)];
  }
  //current->set_intensity(frames/150.0);

  long total = 0;
  for (int x=0; x<X*Y; x++) {
    total += leds[x].r;
    total += leds[x].g;
    total += leds[x].b;
  }
  int brightness = 255;
  if (total > max_brightness) {
    brightness = ((float)max_brightness / (float)total) * 255;
    Serial.println(brightness);
  }
  LEDS.setBrightness(brightness);
  current->draw();
  FastLED.show();
  //delay(2);
}


void processMessage(String msg) {
  int header = (int)msg[0];
  Serial.println(header);
  
  switch (header) {
    case 1:
      // First Color
      for (int i=0; i<NUM_ANIMATIONS; i++) {
        animations[i]->set_first_color(CRGB((int)msg[1]-1, (int)msg[2]-1, (int)msg[3]-1));
      }
      break;
    case 2:
      // Second Color
      for (int i=0; i<NUM_ANIMATIONS; i++) {
        animations[i]->set_second_color(CRGB((int)msg[1]-1, (int)msg[2]-1, (int)msg[3]-1));
      }
      break;
    case 3:
      // Third Color
      for (int i=0; i<NUM_ANIMATIONS; i++) {
        animations[i]->set_third_color(CRGB((int)msg[1]-1, (int)msg[2]-1, (int)msg[3]-1));
      }
      break;
    case 4:
      // Fourth Color
      for (int i=0; i<NUM_ANIMATIONS; i++) {
        animations[i]->set_fourth_color(CRGB((int)msg[1]-1, (int)msg[2]-1, (int)msg[3]-1));
      }
      break;
    case 5:
      // Mode
      current = animations[((int)msg[1]-1) % NUM_ANIMATIONS];
      break;
    default:
      current->serial_event(msg);
  }
}

void serialEvent() {
  String inputString = "";
  inputString.reserve(200);
  while (Serial.available()) {
    delay(1);
    char inChar = (char)Serial.read();
    inputString += inChar;
  }
  if (inputString != "") {
    processMessage(inputString);
  }
}
