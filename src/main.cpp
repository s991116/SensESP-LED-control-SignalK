#include <Arduino.h>
#include <FastLED.h>

#include "sensesp.h"
#include "sensesp_app_builder.h"
#include "sensesp/signalk/signalk_output.h"
#include "sensesp/signalk/signalk_value_listener.h"
#include "sensesp/transforms/change_filter.h"

#include "LedStrip.h"
#include "LedStripFactory.h"
#include "LedStripStateIO.h"
#include "LedStripNightModeIO.h"
#include "LedStripLevelIO.h"

using namespace sensesp;

#define NUM_LEDS 5
#define DATA_PIN (25)

CRGB leds[NUM_LEDS];

LedStrip* strip = nullptr;
LedStripFactory* factory = nullptr;

void setup() {
  SetupLogging(ESP_LOG_DEBUG);

  SensESPAppBuilder builder;
  sensesp_app = builder
      .set_hostname("led-strip-controller")
      ->get_app();

    FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
  FastLED.clear(true);

  factory = new LedStripFactory(leds, NUM_LEDS);

  factory->addSegment(
      0, 3,
      "electrical.switches.lightStairs.state",
      "electrical.switches.lightStairs.level",
      "electrical.switches.nightMode");

  factory->addSegment(
      3, 2,
      "electrical.switches.lightOutside.state",
      "electrical.switches.lightOutside.level",
      "electrical.switches.nightMode");

  event_loop()->onRepeat(1000, []() {
    static bool emitted = false;
    if (!emitted) {
        factory->emitInitialState();
        ESP_LOGI("Main", "Initial LED state sent to Signal K");
        emitted = true;
    }
  });

  sensesp_app->start(); 

}

void loop() {
  event_loop()->tick();
  factory->updateAll();  
}