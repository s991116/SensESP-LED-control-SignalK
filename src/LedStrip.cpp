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
}

void LedStrip::SetNightMode(bool s) {
    if (s == _nightMode) return;
    _nightMode = s;
    update_leds();
}


void LedStrip::SetLevel(int l) {
    if (l < 0) l = 0;
    if (l > 100) l = 100;

    if (l == _level) return;

    _level = l;

    update_leds();
}

void LedStrip::update_leds() {
    for (int i = _start; i < _start + _count; i++) {
        if (!_state) {
            _RGBleds[i] = CRGB::Black;
        } else {
            uint8_t brightness = map(_level, 0, 100, 0, 255);
            if(_nightMode)
              _RGBleds[i].setRGB(brightness, 0, 0);
            else
              _RGBleds[i].setRGB(brightness, brightness, brightness);
        }
    }
    FastLED.show();
}
