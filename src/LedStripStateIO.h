#pragma once
#include "LedStrip.h"
#include "sensesp.h"
#include "sensesp/system/valueconsumer.h"

class LedStripStateIO :
    public sensesp::ValueConsumer<bool> {

public:
    LedStripStateIO(LedStrip* strip);
    void set(const bool& v) override;

private:
    LedStrip* _strip;
    bool _last = false;
};
