#pragma once
#include <stdint.h>

struct Counters
{
    uint8_t bar, beat, qn;
};

class Metronome
{
public:
    void begin();
    void reset();
    void adjustPhrase(int8_t inc);
    void adjustMeter(int8_t inc);
    uint8_t phraseLen() const;
    uint8_t beatsPerBar() const { return _beatsPerBar; }
    bool isRunning() const { return _running; }
    const Counters &get() const { return _c; }

private:
    void onClock();
    void recalcConstants();

    Counters _c{};
    uint32_t _clk = 0;
    uint8_t _phraseIdx = 2; // phraseTable[2] = 4
    uint8_t  _meterIdx  = 2;  // 4/4
    uint8_t  _beatsPerBar = 4;
    uint16_t _clkPerBar   = 96;          // recalculated
    bool _running = false;
};
