#include <MIDI.h>
#include "MidiClock.h"
#include "BPMDetector.h"
#include "core/Hardware.h"

extern BPMDetector bpm;

extern MidiOut midiTx;
MidiOut *MidiClock::_thru = nullptr;

static HardwareSerial Hw(2);
MIDI_CREATE_INSTANCE(HardwareSerial, Hw, MIDIin);

void MidiClock::begin(uint8_t rx, uint8_t tx)
{
    
    _thru = &midiTx;
    Hw.begin(31250, SERIAL_8N1, rx, tx);
    MIDIin.begin(MIDI_CHANNEL_OMNI);
    MIDIin.setHandleClock([]
                          {
                              _tickFlag = true;
                              if (_thru)
                                  _thru->clk(); // forward byte
                          });
    MIDIin.setHandleStart([]
                          {
                              EventBus::publish(Event::Start);
                              if (_thru)
                                  _thru->start(); // forward byte
                          });
    MIDIin.setHandleStop([]
                         {
                             EventBus::publish(Event::Stop);
                             if (_thru)
                                 _thru->stop(); // forward byte
                         });
    MIDIin.setHandleContinue([]
                             {
                                 EventBus::publish(Event::Continue);
                                 if (_thru)
                                     _thru->start(); // forward byte
                             });
}

void MidiClock::poll()
{
    static uint32_t prev = micros();
    MIDIin.read();

    if (_tickFlag)
    {
        _tickFlag = false;
        uint32_t now = micros();
        uint32_t dt = now - prev;
        prev = now;

        bpm.onClock(dt);
        EventBus::publish(Event::ClockTick);
    }
}
