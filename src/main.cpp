// Signal K application file.

#include <memory>

#include "Arduino.h"
#include "sensesp.h"
#include "sensesp/signalk/signalk_output.h"
#include <sensesp/signalk/signalk_value_listener.h>
#include "sensesp/transforms/change_filter.h"
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

class LedStrip {

public:
    using StateCallback = std::function<void(bool)>;
    using LevelCallback = std::function<void(int)>;

    void onStateChange(StateCallback cb) { _state_callback = cb; }
    void onLevelChange(LevelCallback cb) { _level_callback = cb; }

    void SetState(bool s) {
      ESP_LOGD("LedStrip", "SetState → %d", s);      
        if (_state != s) {
          _state = s;
          if (_state_callback) 
            _state_callback(s);   // 🔥 emit til IO-klassen
        }
    }

    void SetLevel(int l) {
        ESP_LOGD("LedStrip", "SetLevel → %d", l);
        if (_level != l) {
          _level = l;
          if (l == 0) 
            SetState(false);

          if (_level_callback) 
            _level_callback(l);   // 🔥 emit til IO-klassen
        }
    }

    bool GetState() const { 
      ESP_LOGD("LedStrip", "GetState");
      return _state; 
    }
    
    int  GetLevel() const { 
      ESP_LOGD("LedStrip", "GetLevel");
      return _level; 
    }

private:
    bool _state = false;
    int  _level = 0;

    StateCallback _state_callback;
    LevelCallback _level_callback;
};

class LedStripStateIO :
    public ValueConsumer<bool>,
    public ValueProducer<bool> {

  public:
    LedStripStateIO(LedStrip* strip) : _strip(strip) {

        // 🔥 registrér dig som "output-observer"
        _strip->onStateChange([this](bool v){
            this->emit_if_changed(v);
        });
    }

    // consumer (input fra Signal K)
    void set(const bool& v) override {
        if (v != _strip->GetState()) {
            _strip->SetState(v);
        }
    }

  private:
    LedStrip* _strip;
    bool _last = false;

    void emit_if_changed(bool v) {
        ESP_LOGD("LedStripStateIO", "emit_if_changed → %d", v);      
        if (v == _last) return;
        _last = v;
        emit(v);
    }
};

class LedStripLevelIO :
    public ValueConsumer<int>,
    public ValueProducer<int> {

  public:
    LedStripLevelIO(LedStrip* strip) : _strip(strip) {

        _strip->onLevelChange([this](int v){
            this->emit_if_changed(v);
        });
    }

    void set(const int& v) override {
      ESP_LOGD("LedStripLevelIO", "set → %d", v);
  
      if (v != _strip->GetLevel()) {
        _strip->SetLevel(v);
      }
    }

  private:
    LedStrip* _strip;
    int _last = -1;

    void emit_if_changed(int v) {
      ESP_LOGD("LedStripLevelIO", "emit_if_changed → %d", v);
      if (v == _last) return;
        _last = v;
        emit(v);
    }
};

void setup() {
  // Setup debug logging
  SetupLogging(ESP_LOG_DEBUG);

  // Create the SensESP app
  SensESPAppBuilder builder;
  sensesp_app = (&builder)
      ->set_hostname("led-strip-controller")
      ->get_app();

  // Create the LED strip model
  auto* strip = new LedStrip();

  // Create IO interfaces
  auto* stateIO = new LedStripStateIO(strip);
  auto* levelIO = new LedStripLevelIO(strip);

  // --- SIGNAL K INPUT (listeners) ---

  // Listen for SK state changes (bool)
  auto* sk_state_listener = new SKValueListener<bool>("electrical.switches.cabinLights.state");
  sk_state_listener->connect_to(stateIO);

  // Listen for SK level changes (int)
  auto* sk_level_listener = new SKValueListener<int>("electrical.switches.cabinLights.level");
  sk_level_listener->connect_to(levelIO);

  // --- SIGNAL K OUTPUT (emit only when changed) ---

// Emit state → SK
  stateIO
      ->connect_to(new ChangeFilter())
      ->connect_to(new SKOutputBool(
          "electrical.switches.cabinLights.state"));

  // Emit level → SK
  levelIO
      ->connect_to(new ChangeFilter())
      ->connect_to(new SKOutputInt(
          "electrical.switches.cabinLights.level"));

  ESP_LOGD("Setup", "LED strip IO fully initialized");

  // Start SenseESP
  sensesp_app->start();
}

void loop() {
  event_loop()->tick();
}