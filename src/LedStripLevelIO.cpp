#include "LedStripLevelIO.h"

LedStripLevelIO::LedStripLevelIO(LedStrip* strip) : _strip(strip) {
}

void LedStripLevelIO::set(const int& v) {
  _strip->SetLevel(v);
}