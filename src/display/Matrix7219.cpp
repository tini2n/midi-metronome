#include "Matrix7219.h"

void Matrix7219::begin(uint8_t din, uint8_t clk, uint8_t cs, uint8_t intensity)
{

  _parola = new MD_Parola(MD_MAX72XX::FC16_HW, din, clk, cs, DEV);
  _parola->begin();
  // _parola->begin(2);
  // _define2LineLayout();
  _parola->setIntensity(intensity); // 0 (dim) … 15 (blinding)
  _parola->displayClear();
}

void Matrix7219::_define2LineLayout()
{
  // Physical chain: modules 0-3 = bottom, 4-7 = top
  _parola->setZone(0, 4, 7); // zone 1 = lower 32×8 line
  _parola->setZone(1, 0, 3); // zone 0 = upper 32×8 line
  _parola->setTextAlignment(PA_LEFT);
}

void Matrix7219::splash()
{
  _parola->displayText("MIDI METRONOME",
                       PA_CENTER, 75, 1000,
                       PA_SCROLL_LEFT, PA_SCROLL_LEFT);

  while (!_parola->displayAnimate())
    yield();
  _parola->displayClear();
}

void Matrix7219::showMetronome(uint8_t bar, uint8_t beat, uint8_t qn, uint8_t meter, uint8_t phrase)
{
  char buf[32];
  // snprintf(buf, sizeof(buf),
  //          "%02u:%u:%u %3u %u/4",
  //          bar, beat, qn + 1, phrase, meter);
  snprintf(buf, sizeof(buf),
           "%02u:%u:%u",
           bar, beat, qn + 1, phrase, meter);

  _parola->displayReset();
  _parola->setTextAlignment(PA_LEFT);
  _parola->print(buf);

  // ––––––––––––––––––––––––––––––––––––
  // if (!_parola)
  //   return;

  // char top[12], bottom[12];
  // snprintf(top, sizeof(top), "%02u:%u:%u ", bar, beat, qn + 1);
  // snprintf(bottom, sizeof(bottom), "P%u T%u ", phrase, meter);

  // _parola->displayZoneText(0, top, PA_LEFT, 0, 0,
  //                          PA_PRINT, PA_NO_EFFECT);
  // _parola->displayZoneText(1, bottom, PA_LEFT, 0, 0,
  //                          PA_PRINT, PA_NO_EFFECT);
}

void Matrix7219::showEdit(const char *tag, uint8_t value) // e.g. “PH 4”
{
  char buf[10];
  snprintf(buf, sizeof(buf), "%s%02u ", tag, value);
  _parola->displayReset();
  _parola->print(buf);

  // ——————————————————————————————————
  // if (!_parola) return;

  // char buf[10];
  // snprintf(buf, sizeof(buf), "%s%02u ", tag, value);
  // _parola->displayZoneText(0, buf, PA_LEFT, 0, 0, PA_PRINT, PA_NO_EFFECT);
  // _parola->displayZoneText(1, buf, PA_LEFT, 0, 0, PA_PRINT, PA_NO_EFFECT);
}