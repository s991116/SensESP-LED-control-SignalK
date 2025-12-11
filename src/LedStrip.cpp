#include "LedStrip.h"
#include <Arduino.h>
#include <FastLED.h>

LedStrip::LedStrip(CRGB* leds, int start_index, int count)
    : _RGBleds(leds), _start(start_index), _count(count) {

    _state = false;
    _level = 0;
    update_leds();
}

void LedStrip::SetState(bool s) {
    if (s == _state) return;
    _state = s;

    update_leds();
    if (_state_cb) _state_cb(_state);
}

bool LedStrip::GetState() const {
    return _state;
}

void LedStrip::SetLevel(int l) {
    if (l < 0) l = 0;
    if (l > 100) l = 100;

    if (l == _level) return;

    if (l > 0 && _level == 0 && !_state)
        SetState(true);

    _level = l;

    if (_level == 0)
        SetState(false);

    update_leds();
    if (_level_cb) _level_cb(_level);
}

int LedStrip::GetLevel() const {
    return _level;
}

void LedStrip::onStateChange(StateCallback cb) {
    _state_cb = cb;
}

void LedStrip::onLevelChange(LevelCallback cb) {
    _level_cb = cb;
}

void LedStrip::update_leds() {
    for (int i = _start; i < _start + _count; i++) {
        if (!_state) {
            _RGBleds[i] = CRGB::Black;
        } else {
            uint8_t brightness = map(_level, 0, 100, 0, 255);
            _RGBleds[i].setRGB(brightness, brightness, brightness);
        }
    }
    FastLED.show();
}
