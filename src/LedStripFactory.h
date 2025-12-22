#pragma once

#include <vector>
#include <cstdint>

#include "LedStrip.h"
#include "LedStripStateIO.h"
#include "LedStripLevelIO.h"
#include "LedStripNightModeIO.h"

class LedStripFactory {
 public:
  LedStripFactory(CRGB* leds, int total_leds);

  LedStrip* addSegment(
      int start,
      int count,
      const char* state_path,
      const char* level_path,
      const char* nightmode_path,
      int listen_delay = 200);

  void updateAll();

 private:
  CRGB* leds_;
  int total_leds_;

  std::vector<LedStrip*> segments_;
};