#include "LedStripStateIO.h"
#include "sensesp/system/lambda_consumer.h"

LedStripStateIO::LedStripStateIO(LedStrip* strip) : _strip(strip) {
    strip->onStateChange([this](bool v){
        this->emit_if_changed(v);
    });
}

void LedStripStateIO::set(const bool& v) {
    if (v != _strip->GetState())
        _strip->SetState(v);
}

void LedStripStateIO::emit_if_changed(bool v) {
    if (v == _last) return;
    _last = v;
    emit(v);
}
