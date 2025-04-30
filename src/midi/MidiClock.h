#pragma once
#include <Arduino.h>
#include "core/EventBus.h"
#include "midi/MidiOut.h"

class MidiClock
{
public:
    void begin(uint8_t rxPin, uint8_t txPin = 255);
    void poll();

private:
    static void IRAM_ATTR isrByte();
    static inline volatile bool _tickFlag = false;
    static MidiOut* _thru;
};
