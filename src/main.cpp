#include "MidiClock.h"
#include "Metronome.h"
#include "BPMDetector.h"
#include "Encoder.h"

MidiClock midi;
Metronome metronome;
BPMDetector bpm;
Encoder encoder;

constexpr uint8_t MIDI_RX_PIN = 16; // from opto
constexpr uint8_t MIDI_TX_PIN = 17; // keep free for future MIDI OUT

constexpr uint8_t ENC_PIN_A = 32;  // CLK
constexpr uint8_t ENC_PIN_B = 33;  // DT
constexpr uint8_t ENC_SW_PIN = 25; // push button

void setup()
{
    Serial.begin(115200);
    delay(1000);

    midi.begin(MIDI_RX_PIN, MIDI_TX_PIN);
    metronome.begin();
    encoder.begin(ENC_PIN_A, ENC_PIN_B, ENC_SW_PIN);

    Serial.println("🟢 MIDI‑Metronome ready");
}

void loop()
{
    midi.poll();

    if (auto dir = encoder.read())
        metronome.adjustPhrase(dir);

    static uint8_t lastQn = 255;
    auto c = metronome.get();

    if (metronome.isRunning() && c.qn != lastQn)
    {
        lastQn = c.qn;
        Serial.printf("[%u:%u:%u]  (BPM:%u PHS:%u)\n",
                      c.bar, c.beat, c.qn, bpm.value(), metronome.phraseLen());
    }
}