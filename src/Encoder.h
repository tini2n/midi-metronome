#pragma once
#include <Arduino.h>

class Encoder
{
  // Rotary encoder with push button
public:
  enum Dir : int8_t
  {
    None = 0,
    CW = 1,
    CCW = -1
  };
  void begin(uint8_t pinA, uint8_t pinB, uint8_t pinSw);
  Dir read();
  bool button();

private:
  static void IRAM_ATTR isrAB();
  static void IRAM_ATTR isrSW();
  static inline volatile int8_t  qStep = 0;   // quarter‑step accumulator
  static inline volatile bool _btn = false;
};
