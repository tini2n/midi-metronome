#pragma once
#include <stdint.h>

struct Counters
{
    uint8_t bar, beat, qn;
};

class Metronome
{
public:
    bool isRunning() const { return _running; }
    void begin();
    void adjustPhrase(int8_t inc); // from rotary
    uint8_t phraseLen() const;
    const Counters &get() const { return _c; }

private:
    void onClock(); // internal
    Counters _c{};
    uint32_t _clk = 0;
    uint8_t _phraseIdx = 2; // phraseTable[2] = 4
    bool _running = false;
    void reset();
};
