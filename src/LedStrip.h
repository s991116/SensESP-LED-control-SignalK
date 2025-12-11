#pragma once
#include <functional>
#include <FastLED.h>

class LedStrip {
public:
    LedStrip(CRGB* leds, int start_index, int count);

    void SetState(bool s);
    void SetLevel(int l);


private:
    void update_leds();

    CRGB* _RGBleds;
    int _start;
    int _count;

    bool _state;
    int _level;
};
