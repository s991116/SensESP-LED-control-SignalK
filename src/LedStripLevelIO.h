#pragma once
#include "LedStrip.h"
#include "sensesp.h"
#include "sensesp/system/valueconsumer.h"
#include "sensesp/system/valueproducer.h"


class LedStripLevelIO :
    public sensesp::ValueConsumer<int>, 
    public sensesp::ValueProducer<int>  {

public:
    LedStripLevelIO(LedStrip* strip);
    void set(const int& v) override;

private:
    LedStrip* _strip;
    int _last = -1;
};
