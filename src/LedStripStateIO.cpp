#include "LedStripStateIO.h"
#include "sensesp/system/lambda_consumer.h"

LedStripStateIO::LedStripStateIO(LedStrip* strip) : _strip(strip) {
}

void LedStripStateIO::set(const bool& v) {
  _strip->SetState(v);
}