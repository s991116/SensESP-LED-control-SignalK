#pragma once

#include <FastLED.h>
#include <functional>

class LedStrip {
 public:
  using StateCallback = std::function<void(bool)>;
  using LevelCallback = std::function<void(int)>;
  using NightModeCallback = std::function<void(bool)>;

  LedStrip(CRGB* leds, int start_index, int count);

  // External control
  void SetState(bool newState);
  void SetLevel(int newLevel);      // 0–100
  void SetNightMode(bool newNightMode);

  bool GetState() const;
  int  GetLevel() const;
  bool GetNightMode() const;

  // Observer registration
  void onStateChange(StateCallback cb);
  void onLevelChange(LevelCallback cb);
  void onNightModeChange(NightModeCallback cb);

  void update();

 private:
  // Rendering helpers
  void render_led(int index);
  void refresh_visible_leds();

  // Animation
  void start_animation(bool turning_on);
  void animate_step();

  // Hardware
  CRGB* _leds;
  int   _start;
  int   _count;

  // State
  bool _state       = false;
  bool _nightMode   = false;
  int  _level       = 50;

  // Animation state
  bool _animating   = false;
  int  _anim_pos    = 0;   // 0 .. _count
  int  _anim_dir    = 0;   // +1 = ON, -1 = OFF

  // Timing
  unsigned long _last_anim_ms = 0;
  const unsigned long _anim_interval_ms = 80;

  // Observers
  StateCallback     _state_cb;
  LevelCallback     _level_cb;
  NightModeCallback _night_cb;
};
