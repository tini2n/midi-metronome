#include "MidiClock.h"
#include "Metronome.h"
#include "BPMDetector.h"
#include "Encoder.h"
#include "Matrix7219.h"

MidiClock midi;
Metronome metronome;
BPMDetector bpm;
Encoder encoder;
Matrix7219 matrix;

constexpr uint8_t MIDI_RX_PIN = 16; // from opto
constexpr uint8_t MIDI_TX_PIN = 17; // keep free for future MIDI OUT

constexpr uint8_t ENC_PIN_A = 32;  // CLK
constexpr uint8_t ENC_PIN_B = 33;  // DT
constexpr uint8_t ENC_SW_PIN = 25; // push button

constexpr uint8_t DIN_PIN = 23; // data in
constexpr uint8_t CLK_PIN = 18; // clock
constexpr uint8_t CS_PIN = 21;  // chip select

void setup()
{
    Serial.begin(115200);

    midi.begin(MIDI_RX_PIN, MIDI_TX_PIN);
    metronome.begin();
    encoder.begin(ENC_PIN_A, ENC_PIN_B, ENC_SW_PIN);
    matrix.begin(DIN_PIN, CLK_PIN, CS_PIN, 1);
    matrix.splash();

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

        matrix.showMetronome(c.bar, c.beat, c.qn, bpm.value());
    }

    matrix.loop();
}