#include "Matrix7219.h"

void Matrix7219::begin(uint8_t din, uint8_t clk, uint8_t cs, uint8_t intensity)
{
  _parola = new MD_Parola(MD_MAX72XX::FC16_HW, din, clk, cs, DEV);
  _parola->begin();
  _parola->setIntensity(intensity);     // 0 (dim) … 15 (blinding)
  _parola->displayClear();
}

void Matrix7219::splash()
{
  _parola->displayText("MIDI METRONOME",
                      PA_LEFT, 50, 1000,
                      PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  while (!_parola->displayAnimate()) /*wait*/;
  _parola->displayClear();
}

void Matrix7219::showMetronome(uint8_t bar, uint8_t beat, uint8_t qn, uint16_t bpm)
{
  char buf[40];
  snprintf(buf, sizeof(buf),
           "%02u:%u:%u",
           bar, beat, qn);

  _parola->displayReset();   // force instant update, no scroll
  _parola->setTextAlignment(PA_LEFT);
  _parola->print(buf);
}
