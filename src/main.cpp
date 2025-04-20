#include <Arduino.h>
#include <MIDI.h>

constexpr uint8_t MIDI_RX_PIN = 16;                       // from opto
constexpr uint8_t MIDI_TX_PIN = 17;                       // keep free for future MIDI OUT
constexpr uint8_t PPQN = 24;                              // MIDI spec (don't change). 24 clocks per quarter note
constexpr uint8_t BEATS_PER_BAR = 4;                      // time‑signature top number
constexpr uint8_t QTR_PER_BEAT = 4;                       // 4 QN per beat (4/4 time)
constexpr uint8_t BARS_PER_PHRASE = 4;                    // 4 bars per phrase
constexpr uint8_t CLOCKS_PER_QN = PPQN / QTR_PER_BEAT;    // 6
constexpr uint8_t CLOCKS_PER_BEAT = PPQN;                 // 24
constexpr uint16_t CLOCKS_PER_BAR = PPQN * BEATS_PER_BAR; // 96

HardwareSerial MidiHW(2);
MIDI_CREATE_INSTANCE(HardwareSerial, MidiHW, MIDI);

struct ClockState
{
  uint32_t clock = 0; // clocks since last Start
  float bpm = 0.0f;

  // Derived counters (updated every beat/bar/qtr)
  uint32_t beat = 0; // 0‑based
  uint32_t bar = 0;  // 0‑based
  uint8_t qtr = 0;   // 0..3   (16th‑note within the beat)

  bool running = false; // true if Start received
};

ClockState st;

void resetCounters() { st = {}; }

// -------------------------------------------------------
// Helper: recompute BPM once per clock tick
void updateBpm(uint32_t dtMicros)
{
  // dtMicros = µs per clock; 1 QN = 24 clocks
  if (dtMicros == 0)
    return;
  float instBpm = 60.0e6f / (dtMicros * PPQN);

  // simple low‑pass: keep 90 % old, 10 % new
  st.bpm = st.bpm * 0.90f + instBpm * 0.10f;
}

// Helper: update beat / bar counters every clock
void updateCounters()
{
  ++st.clock;

  st.qtr = (st.clock / CLOCKS_PER_QN) % QTR_PER_BEAT;
  st.beat = (st.clock / CLOCKS_PER_BEAT) % BEATS_PER_BAR;
  st.bar = (st.clock / CLOCKS_PER_BAR) % BARS_PER_PHRASE;
}

void printStatus(const char *tag)
{
  Serial.printf("[%u:%u:%u]   |CLK:%lu|BPM:%5.1f|\n",
                st.bar, st.beat, st.qtr, st.clock, st.bpm);
}

// -------------------------------------------------------
// MIDI callbacks
void onClock()
{
  static uint32_t lastMicros = 0;
  uint32_t now = micros();
  uint32_t dt = now - lastMicros;
  lastMicros = now;

  if (!st.running)
    return; // ignore stray clocks while stopped

  updateBpm(dt);
  updateCounters();

  // Only print once per beat (every 24 clocks) to keep log readable
  // if (st.clock % CLOCKS_PER_BEAT == 0)
  if (st.clock % CLOCKS_PER_QN == 0)
    printStatus("Clock");
}

void onStart()
{
  resetCounters();
  st.running = true;
  Serial.println("\n--- START ---");
}
void onStop()
{
  st.running = false;
  Serial.println("\n--- STOP  ---");
}
void onContinue()
{
  st.running = true;
  Serial.println("\n--- CONT  ---");
}

void onNoteOn(byte ch, byte note, byte vel)
{
  Serial.printf("Note| On ch:%d  %3d  vel:%3d\n", ch, note, vel);
}

// -------------------------------------------------------
void setup()
{
  Serial.begin(115200);
  delay(1000);

  MidiHW.begin(31250, SERIAL_8N1, MIDI_RX_PIN, MIDI_TX_PIN);

  MIDI.setHandleClock(onClock);
  MIDI.setHandleStart(onStart);
  MIDI.setHandleStop(onStop);
  MIDI.setHandleContinue(onContinue);
  // MIDI.setHandleNoteOn(onNoteOn); // for debugging
  MIDI.begin(MIDI_CHANNEL_OMNI);

  Serial.println("🟢 MIDI‑Metronome ready");
}

void loop()
{
  MIDI.read();
}
