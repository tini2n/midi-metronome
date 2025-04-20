#include <Arduino.h>
#include <MIDI.h>

constexpr uint8_t MIDI_RX_PIN = 16;                       // from opto
constexpr uint8_t MIDI_TX_PIN = 17;                       // keep free for future MIDI OUT
constexpr uint8_t PPQN = 24;                              // MIDI spec (don't change). 24 clocks per quarter note
constexpr uint8_t BEATS_PER_BAR = 4;                      // time‑signature top number
constexpr uint8_t QTR_PER_BEAT = 4;                       // 4 QN per beat (4/4 time)
constexpr uint8_t CLOCKS_PER_QN = PPQN / QTR_PER_BEAT;    // 6
constexpr uint8_t CLOCKS_PER_BEAT = PPQN;                 // 24
constexpr uint16_t CLOCKS_PER_BAR = PPQN * BEATS_PER_BAR; // 96

HardwareSerial MidiHW(2);
MIDI_CREATE_INSTANCE(HardwareSerial, MidiHW, MIDI);

struct ClockState
{
  uint32_t clockCount = 0;      // clocks since last Start
  uint32_t lastClockMicros = 0; // for BPM calc
  float bpm = 0.0f;
  // Derived counters (updated every beat/bar/qtr)
  uint32_t beat = 0; // 0‑based
  uint32_t bar = 0;  // 0‑based
  uint8_t qtr = 0;   // 0..3   (16th‑note within the beat)
};

ClockState st;

void resetCounters() { st = {}; }

// -------------------------------------------------------
// Helper: recompute BPM once per clock tick
void updateBpm()
{
  static uint32_t prev = 0;
  uint32_t now = micros();
  uint32_t diff = now - prev;
  prev = now;

  if (diff > 1000)
  {                                               // ignore <500 kHz noise
    float qn_us = diff * PPQN;                    // µs for 1 quarter note
    st.bpm = (qn_us > 0) ? (60.0e6f / qn_us) : 0; // 60 s / (µs / 1e6)
  }
}

// Helper: update beat / bar counters every clock
void updateCounters()
{
  ++st.clockCount;

  // Quantise to grids
  st.qtr = (st.clockCount % CLOCKS_PER_BEAT) / CLOCKS_PER_QN;
  st.beat = (st.clockCount / CLOCKS_PER_BEAT) % BEATS_PER_BAR;
  st.bar = st.clockCount / CLOCKS_PER_BAR;
}

// Nicely formatted status line
void printStatus(const char *tag)
{
  Serial.printf(
      "%4lu|%2u|%2u CLK:%7lu BPM:%6.1f\n",
      st.bar + 1, // convert to 1‑based for display
      st.beat + 1,
      st.qtr + 1,
      st.clockCount,
      st.bpm);
}

// -------------------------------------------------------
// MIDI callbacks
void onClock()
{
  updateBpm();
  updateCounters();

  // Only print once per beat (every 24 clocks) to keep log readable
  if (st.clockCount % CLOCKS_PER_BEAT == 0)
    printStatus("Clock");
}

void onStart() { resetCounters(); Serial.println("\n--- START ---"); }
void onStop () { Serial.println("\n--- STOP  ---"); }
void onCont () { Serial.println("\n--- CONT  ---"); }

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
  MIDI.setHandleContinue(onCont);
  // MIDI.setHandleNoteOn(onNoteOn);
  MIDI.begin(MIDI_CHANNEL_OMNI);

  Serial.println("🟢  MIDI‑Metronome ready");
}

void loop()
{
  MIDI.read(); // polling handler (fast enough on ESP32)
  // You can add display refresh or LED blink code here every few ms.
}
