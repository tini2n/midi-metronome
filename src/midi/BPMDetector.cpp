#include "BPMDetector.h"
#include "core/Constants.h"
#include <algorithm>

void BPMDetector::onClock(uint32_t dt)
{
  if (!dt)
    return;
  _buf[_idx] = dt * PPQN; // µs per beat
  _idx = (_idx + 1) & 7;
  uint64_t sum = 0;
  for (auto v : _buf)
    sum += v;
  uint32_t avg = sum / 8;
  _bpmInt = (uint16_t)((60000000ULL + avg / 2) / avg);
}
