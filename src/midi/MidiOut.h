#pragma once
#include <MIDI.h>
#include <HardwareSerial.h>

class MidiOut
{
public:
    void attach(HardwareSerial &s) { _ser = &s; }

    inline void clk() { _ser->write(0xF8); }
    inline void start() { _ser->write(0xFA); }
    inline void stop() { _ser->write(0xFC); }

private:
    HardwareSerial *_ser = nullptr;
};
