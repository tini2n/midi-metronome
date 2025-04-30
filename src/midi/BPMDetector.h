#pragma once
#include <stdint.h>

class BPMDetector
{
public:
    void onClock(uint32_t dtMicros);
    uint16_t value() const { return _bpmInt; }

private:
    uint32_t _buf[8]{};
    uint8_t _idx = 0;
    uint16_t _bpmInt = 0;
};
