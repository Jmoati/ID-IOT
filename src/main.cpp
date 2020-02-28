#include <FastLED.h>
#include <ArduinoJson.h>

#define LED_PIN 5
#define NUM_LEDS 84
#define BRIGHTNESS 64
#define FADE 42

CRGB leds[NUM_LEDS];

String devs_names[5] =  {
  "BRO",
  "GDE",
  "JMO",
  "TAL",
  "LPH"
};

CRGB colors[5] =  {
  CRGB(0, 200, 0),
  CRGB(0, 0, 200),
  CRGB(200, 0, 0),
  CRGB(255, 140, 0),
  CRGB(50, 0, 255),
};

StaticJsonDocument<1024> reviews;
  
int pixels[5][12] = {
  {35, 47, 49, 61, 63, 75, 77, 5,  7,  19, 21, 33},
  {32, 36, 46, 50, 60, 64, 74, 78, 4,  8,  18, 22},
  {23, 31, 37, 45, 51, 59, 65, 73, 79, 3,   9, 17},
  {16, 24, 30, 38, 44, 52, 58, 66, 72, 80,  2, 10},
  {11, 15, 25, 29, 39, 43, 53, 57, 67, 71, 81,  1}
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
    for (int tempo=0; tempo<14400; ++tempo) { // 12 * 120
        for (int x_pos=0; x_pos<12; ++x_pos) {
          if(Serial.available() > 0) {
            StaticJsonDocument<1024> json;
            String data = Serial.readStringUntil('\n');
            DeserializationError error = deserializeJson(json, data);

            if (error) {
              return;
            }
            
            reviews = json;
          }
          for (int element=0; element<5; ++element) {
            if (element < reviews.size()) {
              StaticJsonDocument<1024> review = reviews.getElement(element); 

              if ((tempo/computeSpeed(review["count"])%12 == x_pos)) {
                for (int dev_pos = 0; dev_pos <= 5; ++dev_pos) {
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
