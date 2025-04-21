#include "Constants.h"
#include "Metronome.h"
#include "EventBus.h"

static const uint8_t phraseTable[] = {1, 2, 4, 8, 16, 32};
constexpr uint8_t CLK_PER_QN = PPQN / 4;
constexpr uint8_t CLK_PER_BEAT = PPQN;
constexpr uint16_t CLK_PER_BAR = PPQN * BEATS;

void Metronome::begin()
{
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

void Metronome::adjustPhrase(int8_t d)
{
  int8_t n = _phraseIdx + d;
  if (n < 0)
    n = 0;
  if (n > 5)
    n = 5;
  _phraseIdx = n;
}

void Metronome::onClock()
{
  if (!_running) return;

  ++_clk;
  _c.qn = (_clk / CLK_PER_QN) & 0x03;
  _c.beat = (_clk / CLK_PER_BEAT) & 0x03;
  uint8_t barsPerPhrase = phraseTable[_phraseIdx];
  _c.bar = (_clk / CLK_PER_BAR) % barsPerPhrase;
}

uint8_t Metronome::phraseLen() const
{
    return phraseTable[_phraseIdx];
}