#include <Arduino.h>
#include <FastLED.h>

#include "sensesp.h"
#include "sensesp_app_builder.h"
#include "sensesp/signalk/signalk_output.h"
#include "sensesp/signalk/signalk_value_listener.h"
#include "sensesp/transforms/change_filter.h"

#include "LedStrip.h"
#include "LedStripStateIO.h"
#include "LedStripLevelIO.h"

using namespace sensesp;

#define NUM_LEDS 5
CRGB leds[NUM_LEDS];

void setup() {
  SetupLogging(ESP_LOG_DEBUG);

  SensESPAppBuilder builder;
  sensesp_app = builder
      .set_hostname("led-strip-controller")
      ->get_app();

  FastLED.addLeds<NEOPIXEL, 5>(leds, NUM_LEDS);

  auto* strip = new LedStrip(leds, 0, 3);

  auto* stateIO = new LedStripStateIO(strip);
  auto* levelIO = new LedStripLevelIO(strip);

  auto* sk_state = new SKValueListener<bool>("electrical.switches.cabinLights.state");
  sk_state->connect_to(stateIO);

  auto* sk_level = new SKValueListener<int>("electrical.switches.cabinLights.level");
  sk_level->connect_to(levelIO);

  stateIO
      ->connect_to(new ChangeFilter())
      ->connect_to(new SKOutputBool("electrical.switches.cabinLights.state"));

  levelIO
      ->connect_to(new ChangeFilter())
      ->connect_to(new SKOutputInt("electrical.switches.cabinLights.level"));

  sensesp_app->start();
}

void loop() {
    event_loop()->tick();
}
