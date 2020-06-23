#include <FastLED.h>
#include <ArduinoJson.h>

#define LED_PIN 23
#define NUM_LEDS 48
#define BRIGHTNESS 64
#define FADE 42

CRGB leds[NUM_LEDS];

String devs_names[4] =  {
  "BRO",
  "GDE",
  "JMO",
  "MJO",
};

CRGB colors[4] =  {
  CRGB(0, 200, 0),
  CRGB(0, 0, 200),
  CRGB(200, 0, 0),
  CRGB(255, 140, 0),
};

StaticJsonDocument<1024> reviews;
  
int pixels[4][12] = {
  {12, 19, 20, 27, 28, 35, 36, 43, 44, 3, 4, 11},
  {10, 13, 18, 21, 26, 29, 34, 37, 42, 45, 2, 5},
  {6, 9, 14, 17, 22, 25, 30, 33, 38, 41, 46, 1},
  {0, 7, 8, 15, 16, 23, 24, 31, 32, 39, 40, 47},
};

int computeSpeed(int mrCount) {
  return (mrCount > 5) ? 1 : (6 - mrCount);
}

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  for (int tempo=0; tempo<14400; ++tempo) { // 12 * 120,
    for (int x_pos=0; x_pos<12; ++x_pos) {
      for (int element=0; element<4; ++element) {
        if(Serial.available() > 0) {
          StaticJsonDocument<1024> json;
          String data = Serial.readStringUntil('\n');
          DeserializationError error = deserializeJson(json, data);

          if (error) {
            return;
          }
            
          reviews = json;
        }

        if (element < reviews.size()) {
          StaticJsonDocument<1024> review = reviews.getElement(element); 

          if ((tempo/computeSpeed(review["count"])%12 == x_pos)) {
            for (int dev_pos = 0; dev_pos <= 4; ++dev_pos) {
              if (devs_names[dev_pos] == review["who"]) {
                leds[pixels[element][x_pos]] = colors[dev_pos];              
              }
            }
          } else {
            leds[pixels[element][x_pos]].fadeToBlackBy(FADE);
          }
        } else {
            leds[pixels[element][x_pos]].fadeToBlackBy(FADE);
        }

      }

      FastLED.show();
      delay(1);
    }
  }
}
