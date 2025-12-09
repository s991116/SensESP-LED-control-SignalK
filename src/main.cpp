// Signal K application file.

#include <memory>

#include "Arduino.h"
#include "sensesp.h"
#include "sensesp/signalk/signalk_output.h"
#include <sensesp/signalk/signalk_value_listener.h>
#include "sensesp/system/lambda_consumer.h"
#include "sensesp/controllers/smart_switch_controller.h"
#include "sensesp_app_builder.h"

#include "FastLED.h"

using namespace sensesp;

//Define SmartSwitch

//Define LED-strip setup
#define NUM_LEDS 3
CRGB leds[NUM_LEDS];

//Listen for Button press on SignalK
const char* sk_path_led_state = "electrical.switches.cabinLights.state";
const char* sk_path_led_level = "electrical.switches.cabinLights.level";

class LedStrip : 
  public ValueConsumer<bool>,
  public ValueProducer<bool> {

  public:
    bool LEDState = false;
    int  LEDLevel = 0;

    bool lastSentState = false;    // Used to only sent if there is a change

    LedStrip(bool initialState = false, int initialLevel = 0) :
    _state(initialState),
    _level(initialLevel),
    _lastStateSent(initialState),
    _lastLevelSent(initialLevel) {
      ESP_LOGD("LedStrip", "LedStrip initialized");
    }

  void SetState(bool state) {
    _state = state;
    ESP_LOGD(TAG, "LedStrip SetState returns: %d", _state);
  }

  bool GetState() const {
    ESP_LOGD(TAG, "LedStrip GetState returns: %d", _state);
    return _state;
  }

  void SetLevel(int level) {
    _level = level;
    ESP_LOGD(TAG, "LedStrip SetLevel returns: %d", _level);
    if (_level == 0) {
      SetState(false);
    }
  }

  int GetLevel() const {
    ESP_LOGD(TAG, "LedStrip GetLevel returns: %d", _level);
    return _level;
  }

  // Consumer part
  void set(const bool& newState) override {
    SetState(newState);
    emit_state_if_changed();
  }

//  void set(const int& newLevel) override {
//    SetLevel(newLevel);
//    emit_level_if_changed();
//  }

    //Producer part
    void update() {
      emit_state_if_changed();
      //emit_level_if_changed();
    }

  private:
    bool _state;
    int  _level;

    bool _lastStateSent;
    int  _lastLevelSent;


  void emit_state_if_changed() {
    if (_state != _lastStateSent) {
      _lastStateSent = _state;
      this->ValueProducer<bool>::emit(_state);
    }
  }

//  void emit_level_if_changed() {
//    if (_level != _lastLevelSent) {
//      _lastLevelSent = _level;
//      this->ValueProducer<int>::emit(_level);
//    }
//  }
};


// The setup function performs one-time application initialization.
void setup() {
  SetupLogging(ESP_LOG_DEBUG);
  FastLED.addLeds<NEOPIXEL, 2>(leds, NUM_LEDS);
  // Construct the global SensESPApp() object
  SensESPAppBuilder builder;
  sensesp_app = (&builder)
                    // Set a custom hostname for the app.
                    ->set_hostname("LED-strip-controller")
                    ->get_app();

  LedStrip ledStrip = LedStrip();
  //Listen for changes and read value
  auto* listener = new SKValueListener<bool>(sk_path_led_state);
  listener->connect_to(&ledStrip);
  //Send changes to SignalSK
  ledStrip.connect_to(new SKOutputBool(sk_path_led_state));
 
  ESP_LOGD("Setup", "LedStrip Consumer and Producer setup complete");

  sensesp_app->start();
  // To avoid garbage collecting all shared pointers created in setup(),
  // loop from here.
  while (true) {
    loop();
  }
}

void loop() { event_loop()->tick(); }
