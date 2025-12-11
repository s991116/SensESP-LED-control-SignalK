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
#define NUM_LEDS 5
CRGB leds[NUM_LEDS];

//Listen for Button press on SignalK
const char* sk_path_led_state = "electrical.switches.cabinLights.state";
const char* sk_path_led_level = "electrical.switches.cabinLights.level";
#include <FastLED.h>

class LedStrip {
public:
    using StateCallback = std::function<void(bool)>;
    using LevelCallback = std::function<void(int)>;

    LedStrip(CRGB* leds, int start_index, int count)
        : _RGBleds(leds), _start(start_index), _count(count) {

        // Initial values
        _state = false;
        _level = 0;

        update_leds();
    }

    // --------------------------
    // Public Set/Get
    // --------------------------

    void SetState(bool s) {
        if (s == _state) return;

        _state = s;
        update_leds();

        if (_state_cb) {
            _state_cb(_state);
        }
    }

    bool GetState() const {
        return _state;
    }

    void SetLevel(int l) {
        if (l < 0) l = 0;
        if (l > 100) l = 100;

        if (l == _level) return;

        if(l > 0 && _level == 0 && !_state) {
          SetState(true);
        }
        _level = l;

        // Level 0 always forces state = OFF
        if (_level == 0) {
            SetState(false);
        }

        update_leds();

        if (_level_cb) {
            _level_cb(_level);
        }
    }

    int GetLevel() const {
        return _level;
    }

    // --------------------------
    // Observer registrering
    // --------------------------

    void onStateChange(StateCallback cb) { _state_cb = cb; }
    void onLevelChange(LevelCallback cb) { _level_cb = cb; }

private:
    // --------------------------
    // Internal LED handling
    // --------------------------

    void update_leds() {
        for (int i = _start; i < _start + _count; i++) {
            if (_state == false) {
                _RGBleds[i] = CRGB::Black;
            } else {
                uint8_t brightness = map(_level, 0, 100, 0, 255);
                _RGBleds[i].setRGB(brightness, brightness, brightness);
            }
        }

        FastLED.show();
    }

    // --------------------------
    // Private members
    // --------------------------

    CRGB* _RGBleds;
    int _start;
    int _count;

    bool _state;
    int _level;

    StateCallback _state_cb;
    LevelCallback _level_cb;
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
  auto* strip = new LedStrip(leds,0, 3);

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