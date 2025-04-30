#include "core/Hardware.h"
#include "midi/MidiClock.h"
#include "midi/MidiOut.h"
#include "midi/Metronome.h"
#include "midi/BPMDetector.h"
#include "input/Encoder.h"
#include "display/Matrix7219.h"

#include "app/AppController.h"

MidiClock midiClock;
MidiOut midiTx;
Metronome metronome;
BPMDetector bpm;
Encoder encoder;
Matrix7219 matrix;

AppController app;

void setup()
{
    Serial.begin(115200);

    pinMode(PIN_LED_CLK, OUTPUT);
    midiTx.attach(Serial2);
    midiClock.begin(MIDI_RX_PIN, MIDI_TX_PIN);
    metronome.begin();
    encoder.begin(ENC_PIN_A, ENC_PIN_B, ENC_SW_PIN);
    matrix.begin(DIN_PIN, CLK_PIN, CS_PIN, 1);
    matrix.splash();

    app.begin(&encoder, &metronome, &matrix, &bpm);

    Serial.println("🟢 MIDI‑Metronome ready");
}

void loop()
{
    midiClock.poll();

    app.pollLogic();
    app.pollDisplay();
}