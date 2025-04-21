#include "MidiClock.h"
#include "BPMDetector.h"
#include <MIDI.h>

extern BPMDetector bpm;

static HardwareSerial Hw(2);
MIDI_CREATE_INSTANCE(HardwareSerial, Hw, MIDIin);

void MidiClock::begin(uint8_t rx, uint8_t tx)
{
    Hw.begin(31250, SERIAL_8N1, rx, tx);
    MIDIin.begin(MIDI_CHANNEL_OMNI);
    MIDIin.setHandleClock([]
                          { _tickFlag = true; });
    MIDIin.setHandleStart([]
                          { EventBus::publish(Event::Start); });
    MIDIin.setHandleStop([]
                         { EventBus::publish(Event::Stop); });
    MIDIin.setHandleContinue([]
                             { EventBus::publish(Event::Continue); });
}

void MidiClock::poll()
{
    static uint32_t prev = micros();
    MIDIin.read();
    
    if (_tickFlag)
    {
        _tickFlag = false;
        uint32_t now = micros();
        uint32_t dt  = now - prev;
        prev = now;

        bpm.onClock(dt);   
        EventBus::publish(Event::ClockTick);
    }
}
