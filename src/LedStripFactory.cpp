#include "LedStripFactory.h"

#include <Arduino.h>
#include "sensesp/signalk/signalk_value_listener.h"

using namespace sensesp;

LedStripFactory::LedStripFactory(CRGB* leds, int total_leds)
    : leds_(leds), total_leds_(total_leds) {
}

LedStrip* LedStripFactory::addSegment(
    int start,
    int count,
    const char* state_path,
    const char* level_path,
    const char* nightmode_path,
    int listen_delay) {

  if (start < 0 || start + count > total_leds_) {
    ESP_LOGE("LedStripFactory", "Segment out of bounds");
    return nullptr;
  }

  auto* strip = new LedStrip(leds_, start, count);
  segments_.push_back(strip);

  // IO adapters
  auto* stateIO = new LedStripStateIO(strip);
  auto* levelIO = new LedStripLevelIO(strip);
  auto* nightIO = new LedStripNightModeIO(strip);

  // Signal K listeners
  auto* sk_state =
      new SKValueListener<bool>(state_path, listen_delay);
  sk_state->connect_to(stateIO);

  auto* sk_level =
      new SKValueListener<int>(level_path, listen_delay);
  sk_level->connect_to(levelIO);

  auto* sk_night =
      new SKValueListener<bool>(nightmode_path, listen_delay);
  sk_night->connect_to(nightIO);

  return strip;
}

void LedStripFactory::updateAll() {
  for (auto* s : segments_) {
    s->update();
  }
}
