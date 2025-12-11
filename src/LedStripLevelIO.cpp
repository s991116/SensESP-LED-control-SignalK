#include "LedStripLevelIO.h"

LedStripLevelIO::LedStripLevelIO(LedStrip* strip) : _strip(strip) {
    strip->onLevelChange([this](int v){
        this->emit_if_changed(v);
    });
}

void LedStripLevelIO::set(const int& v) {
    if (v != _strip->GetLevel())
        _strip->SetLevel(v);
}

void LedStripLevelIO::emit_if_changed(int v) {
    if (v == _last) return;
    _last = v;
    emit(v);
}
