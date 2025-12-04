// Signal K application file.

#include <memory>

#include "Arduino.h"
#include "sensesp.h"
#include "sensesp/signalk/signalk_output.h"
#include "sensesp/system/lambda_consumer.h"
#include "sensesp/controllers/smart_switch_controller.h"
#include "sensesp_app_builder.h"

#include "FastLED.h"

using namespace sensesp;

//Define SmartSwitch

//Define LED-strip setup
#define NUM_LEDS 3
CRGB leds[NUM_LEDS];


// The setup function performs one-time application initialization.

void setup() {
  SetupLogging(ESP_LOG_DEBUG);
  FastLED.addLeds<NEOPIXEL, 2>(leds, NUM_LEDS);
  // Construct the global SensESPApp() object
  SensESPAppBuilder builder;
  sensesp_app = (&builder)
                    // Set a custom hostname for the app.
                    ->set_hostname("LED-strip-controller")
                    // Optionally, hard-code the WiFi and Signal K server
                    // settings. This is normally not needed.
                    //->set_wifi_client("Trevangsvej_197", "xx")
                    //->set_wifi_access_point("LED-strip-controller", "xx")
                    //->set_sk_server("192.168.1.181", 80)
                    //->set_sk_server("openplotter.local", 80)
                    ->enable_uptime_sensor()
                    ->get_app();

  
  // To avoid garbage collecting all shared pointers created in setup(),
  // loop from here.
  while (true) {
    loop();
  }
}

void loop() { event_loop()->tick(); }
