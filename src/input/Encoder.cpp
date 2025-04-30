#include "Encoder.h"
static uint8_t A, B;
static volatile uint8_t state = 0;
const int8_t lut[16] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};

void Encoder::begin(uint8_t a, uint8_t b, uint8_t sw)
{
  A = a;
  B = b;
  pinMode(A, INPUT_PULLUP);
  pinMode(B, INPUT_PULLUP);
  pinMode(sw, INPUT_PULLUP);

  state = ((digitalRead(A) << 1) | digitalRead(B));

  attachInterrupt(A, isrAB, CHANGE);
  attachInterrupt(B, isrAB, CHANGE);
  attachInterrupt(sw, isrSW, FALLING);
}
void IRAM_ATTR Encoder::isrAB()
{
  state = (state << 2) | ((digitalRead(A) << 1) | digitalRead(B));
  state &= 0x0F;
  int8_t m = lut[state];
  if (m)
    qStep += m;
}
void IRAM_ATTR Encoder::isrSW() { _btn = true; }
Encoder::Dir Encoder::read()
{
  int8_t detent = 0;

  noInterrupts();
  while (qStep >= 4)
  {
    qStep -= 4;
    detent++;
  }
  while (qStep <= -4)
  {
    qStep += 4;
    detent--;
  }
  interrupts();

  return static_cast<Dir>(detent);
  // int8_t d = _delta;
  // _delta = 0;
  // return (Dir)d;
}
bool Encoder::button()
{
  bool f = _btn;
  _btn = false;
  return f;
}
