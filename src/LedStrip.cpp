#include "LedStrip.h"
#include <Arduino.h>

LedStrip::LedStrip(CRGB* leds, int start_index, int count)
    : _leds(leds), _start(start_index), _count(count) {

  // Ensure all LEDs start off
  for (int i = _start; i < _start + _count; i++) {
    _leds[i] = CRGB::Black;
  }
  FastLED.show();
}

// ------------------------------------------------
// Public API
// ------------------------------------------------

void LedStrip::SetState(bool newState) {
  if (newState == _state && !_animating) return;

  _state = newState;
  start_animation(_state);

  if (_state_cb) {
    _state_cb(_state);
  }
}

void LedStrip::SetLevel(int newLevel) {
  if (newLevel < 0) newLevel = 0;
  if (newLevel > 100) newLevel = 100;
  if (newLevel == _level) return;

  _level = newLevel;

  // Update already-visible LEDs immediately
  refresh_visible_leds();

  if (_level_cb) {
    _level_cb(_level);
  }
}

void LedStrip::SetNightMode(bool newNightMode) {
  if (newNightMode == _nightMode) return;

  _nightMode = newNightMode;

  // Update already-visible LEDs immediately
  refresh_visible_leds();

  if (_night_cb) {
    _night_cb(_nightMode);
  }
}

bool LedStrip::GetState() const     { return _state; }
int  LedStrip::GetLevel() const     { return _level; }
bool LedStrip::GetNightMode() const { return _nightMode; }

// ------------------------------------------------
// Observer registration
// ------------------------------------------------

void LedStrip::onStateChange(StateCallback cb) {
  _state_cb = cb;
}

void LedStrip::onLevelChange(LevelCallback cb) {
  _level_cb = cb;
}

void LedStrip::onNightModeChange(NightModeCallback cb) {
  _night_cb = cb;
}

// ------------------------------------------------
// Main update (call periodically)
// ------------------------------------------------

void LedStrip::update() {
  if (!_animating) return;

  unsigned long now = millis();
  if (now - _last_anim_ms < _anim_interval_ms) return;

  _last_anim_ms = now;
  animate_step();
}

// ------------------------------------------------
// Animation logic
// ------------------------------------------------

void LedStrip::start_animation(bool turning_on) {
  _animating = true;

  if (turning_on) {
    _anim_dir = +1;
    // If we were fully off, start from 0
    if (_anim_pos <= 0) _anim_pos = 0;
  } else {
    _anim_dir = -1;
    // If we were fully on, start from count
    if (_anim_pos >= _count) _anim_pos = _count;
  }
}

void LedStrip::animate_step() {
  if (_anim_dir > 0) {
    // Turning ON
    if (_anim_pos >= _count) {
      _animating = false;
      return;
    }

    render_led(_start + _anim_pos);
    _anim_pos++;

  } else {
    // Turning OFF
    if (_anim_pos <= 0) {
      _animating = false;
      return;
    }

    _anim_pos--;
    _leds[_start + _anim_pos] = CRGB::Black;
  }

  FastLED.show();
}

// ------------------------------------------------
// Rendering helpers
// ------------------------------------------------

void LedStrip::render_led(int index) {
  uint8_t brightness = map(_level, 0, 100, 0, 255);

  if (_nightMode) {
    _leds[index].setRGB(brightness, 0, 0);
  } else {
    _leds[index].setRGB(brightness, brightness, brightness);
  }
}

void LedStrip::refresh_visible_leds() {
  int visible = _anim_pos;

  if (!_state) {
    visible = _anim_pos;  // LEDs already ON before OFF animation
  }

  for (int i = 0; i < visible; i++) {
    render_led(_start + i);
  }

  FastLED.show();
}
