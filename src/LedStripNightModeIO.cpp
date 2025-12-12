#include "LedStripNightModeIO.h"
#include "sensesp/system/lambda_consumer.h"

LedStripNightModeIO::LedStripNightModeIO(LedStrip* strip) : _strip(strip) {
}

void LedStripNightModeIO::set(const bool& v) {
  _strip->SetNightMode(v);
}