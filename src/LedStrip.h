#pragma once
#include <functional>
#include <FastLED.h>

class LedStrip {
public:
    using StateCallback = std::function<void(bool)>;
    using LevelCallback = std::function<void(int)>;

    LedStrip(CRGB* leds, int start_index, int count);

    void SetState(bool s);
    bool GetState() const;

    void SetLevel(int l);
    int GetLevel() const;

    void onStateChange(StateCallback cb);
    void onLevelChange(LevelCallback cb);

private:
    void update_leds();

    CRGB* _RGBleds;
    int _start;
    int _count;

    bool _state;
    int _level;

    StateCallback _state_cb;
    LevelCallback _level_cb;
};
