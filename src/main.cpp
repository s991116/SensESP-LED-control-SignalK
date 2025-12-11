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
#define DATA_PIN (25)

CRGB leds[NUM_LEDS];

void setup() {
  SetupLogging(ESP_LOG_DEBUG);

  SensESPAppBuilder builder;
  sensesp_app = builder
      .set_hostname("led-strip-controller")
      ->get_app();

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  auto* strip   = new LedStrip(leds, 0, 3);
  auto* stateIO = new LedStripStateIO(strip);
  auto* levelIO = new LedStripLevelIO(strip);

  //
  // --- SignalK INPUT LISTENERS ---
  //
  auto* sk_state = new SKValueListener<bool>("electrical.switches.cabinLights.state");
  sk_state->connect_to(stateIO);

  auto* sk_level = new SKValueListener<int>("electrical.switches.cabinLights.level");
  sk_level->connect_to(levelIO);

  //
  // --- HENT AKTUEL VÆRDI FRA SIGNALK VED OPSTART ---
  //
  auto* sk_client = sensesp_app->get_signalk_client();

  sk_client->set_connected_callback([stateIO, levelIO, sk_client]() {

    ESP_LOGD("INIT", "Connected to SignalK -> requesting initial values...");

    // Request state (bool)
    sk_client->request_value(
        "electrical.switches.cabinLights.state",
        [stateIO](JsonObject obj) {
          if (obj.containsKey("value")) {
            bool val = obj["value"].as<bool>();
            ESP_LOGD("INIT", "Initial state from SK: %d", val);
            stateIO->set(val);
          }
        }
    );

    // Request level (int)
    sk_client->request_value(
        "electrical.switches.cabinLights.level",
        [levelIO](JsonObject obj) {
          if (obj.containsKey("value")) {
            int lvl = obj["value"].as<int>();
            ESP_LOGD("INIT", "Initial level from SK: %d", lvl);
            levelIO->set(lvl);
          }
        }
    );
  });

  //
  // Start SenseESP
  //
  sensesp_app->start();
}

void loop() {
  event_loop()->tick();
}
