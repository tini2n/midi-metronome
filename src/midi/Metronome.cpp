#include "Arduino.h"

#include "core/Constants.h"
#include "core/EventBus.h"
#include "core/Hardware.h"

#include "Metronome.h"

static const uint8_t phraseTable[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 32};
static const uint8_t meterTable[] = {2, 3, 4, 5, 6, 7, 8, 9, 12};

constexpr uint8_t CLK_PER_QN = PPQN / 4;
constexpr uint8_t CLK_PER_BEAT = PPQN;
constexpr uint16_t CLK_PER_BAR = PPQN * BEATS;

/* --------------------------------------------------------------------
   Blink helper:  call  blinkLed(period_ms)  once
   It drives the LED HIGH immediately and auto-clears it after N ms
---------------------------------------------------------------------*/
static void blinkLed(uint16_t ms)
{
  static uint32_t offDue = 0; // µs timestamp
  uint32_t now = micros();

  if (ms)
  { // request a new flash
    digitalWrite(PIN_LED_CLK, HIGH);
    offDue = now + (uint32_t)ms * 1000;
  }
  else if (offDue && int32_t(now - offDue) >= 0)
  {
    digitalWrite(PIN_LED_CLK, LOW); // time elapsed
    offDue = 0;
  }
}


void Metronome::begin()
{
  recalcConstants();

  EventBus::subscribe(Event::ClockTick, [this]
                      { onClock(); });
  EventBus::subscribe(Event::Start, [this]
                      { reset();   _running = true; });
  EventBus::subscribe(Event::Stop, [this]
                      { _running = false; });
  EventBus::subscribe(Event::Continue, [this]
                      { _running = true; });
}

void Metronome::reset()
{
  _c = {};
  _clk = 0;
}

void Metronome::recalcConstants()
{
  _beatsPerBar = meterTable[_meterIdx];
  _clkPerBar = _beatsPerBar * CLK_PER_BEAT;
}

void Metronome::adjustPhrase(int8_t d)
{
  int8_t n = _phraseIdx + d;
  _phraseIdx = constrain(n, 0, int(sizeof(phraseTable)) - 1);
}

void Metronome::adjustMeter(int8_t d)
{
  int n = _meterIdx + d;
  _meterIdx = constrain(n, 0, int(sizeof(meterTable)) - 1);
  recalcConstants();
  reset();
}

void Metronome::onClock()
{
  if (!_running)
    return;

  if (_clk % CLK_PER_BEAT == 0)
    blinkLed(30);
  else
    blinkLed(0);

  ++_clk;
  _c.qn = (_clk / CLK_PER_QN) & 0x03;
  _c.beat = (_clk / CLK_PER_BEAT) % _beatsPerBar;
  _c.bar = (_clk / _clkPerBar) % phraseTable[_phraseIdx];
}

uint8_t Metronome::phraseLen() const
{
  return phraseTable[_phraseIdx];
}

