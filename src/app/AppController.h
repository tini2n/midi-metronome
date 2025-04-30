#pragma once
#include "midi/Metronome.h"
#include "midi/BPMDetector.h"
#include "input/Encoder.h"
#include "display/Matrix7219.h"

class AppController {
public:
  void begin(Encoder* enc, Metronome* metro,
             Matrix7219* disp, BPMDetector* bpm);

  void pollLogic();
  void pollDisplay();

private:
  enum EditMode { PHRASE, METER };

  Encoder*    _enc   = nullptr;
  Metronome*  _metro = nullptr;
  Matrix7219* _disp  = nullptr;
  BPMDetector* _bpm  = nullptr;

  EditMode _mode = PHRASE;
  uint8_t  _lastQn = 255;
  void     handleEncoder();
  void     drawEdit();
};
