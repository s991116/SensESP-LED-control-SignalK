#pragma once
#include "LedStrip.h"
#include "sensesp.h"
#include "sensesp/system/valueconsumer.h"
#include "sensesp/system/valueproducer.h"


class LedStripNightModeIO :
    public sensesp::ValueConsumer<bool>, 
    public sensesp::ValueProducer<bool>  {

public:
    LedStripNightModeIO(LedStrip* strip);
    void set(const bool& v) override;

private:
    LedStrip* _strip;
    bool _last = false;
};
