#include "AppController.h"
#include <Arduino.h>

void AppController::begin(Encoder *e, Metronome *m, Matrix7219 *d, BPMDetector *b)
{
    _enc = e;
    _metro = m;
    _disp = d;
    _bpm = b;
}

void AppController::handleEncoder()
{
    if (_enc->button())
    { // toggle edit target
        _mode = (_mode == PHRASE) ? METER : PHRASE;
        drawEdit();
    }

    int8_t dir = _enc->read();
    if (dir == 0)
        return;

    if (_mode == PHRASE)
        _metro->adjustPhrase(dir);
    else
        _metro->adjustMeter(dir);

    drawEdit();
}

void AppController::drawEdit()
{
    if (_mode == PHRASE)
        _disp->showEdit("P ", _metro->phraseLen());
    else
        _disp->showEdit("T ", _metro->beatsPerBar());
}

void AppController::pollLogic() { handleEncoder(); }

void AppController::pollDisplay()
{
    auto c = _metro->get();
    if (_metro->isRunning() && c.qn != _lastQn)
    {
        _lastQn = c.qn;
        _disp->showMetronome(c.bar, c.beat, c.qn,
                             _metro->beatsPerBar(), _metro->phraseLen());

        Serial.printf("[%u:%u:%u]  %3uBPM  TS:%u/4  PH:%u\n",
                      c.bar, c.beat, c.qn,
                      _bpm->value(),
                      _metro->beatsPerBar(),
                      _metro->phraseLen());
    }
    _disp->loop();
}
