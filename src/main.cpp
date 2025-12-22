#include <Arduino.h>
#include <FastLED.h>

#include "sensesp.h"
#include "sensesp_app_builder.h"
#include "sensesp/signalk/signalk_output.h"
#include "sensesp/signalk/signalk_value_listener.h"
#include "sensesp/transforms/change_filter.h"

#include "LedStrip.h"
#include "LedStripStateIO.h"
#include "LedStripNightModeIO.h"
#include "LedStripLevelIO.h"

using namespace sensesp;

#define NUM_LEDS 5
#define DATA_PIN (25)

CRGB leds[NUM_LEDS];

LedStrip* strip = nullptr;

void setup() {
  SetupLogging(ESP_LOG_DEBUG);

  SensESPAppBuilder builder;
  sensesp_app = builder
      .set_hostname("led-strip-controller")
      ->get_app();

  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
  FastLED.clear(true);

  strip = new LedStrip(leds, 0, 3);

  auto* stateIO = new LedStripStateIO(strip);
  auto* nightModeIO = new LedStripNightModeIO(strip);
  auto* levelIO = new LedStripLevelIO(strip);

  int listenDelay = 200;

  auto* sk_state =
      new SKValueListener<bool>(
          "electrical.switches.cabinLights.state", listenDelay);
  sk_state->connect_to(stateIO);

  auto* sk_nightMode =
      new SKValueListener<bool>(
          "electrical.switches.nightMode", listenDelay);
  sk_nightMode->connect_to(nightModeIO);

  auto* sk_level =
      new SKValueListener<int>(
          "electrical.switches.cabinLights.level", listenDelay);
  sk_level->connect_to(levelIO);

  sensesp_app->start();
}

void loop() {
  event_loop()->tick();

  if (strip) {
    strip->update();
  }
}
