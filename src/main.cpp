#include <Arduino.h>
#include <MIDI.h>

// ---------------------------------------------------------
// MIDI metronome: receive MIDI clock and start/stop
// ---------------------------------------------------------

// --- MIDI IN/OUT PINS ------------------------------------
constexpr uint8_t MIDI_RX_PIN = 16; // from opto
constexpr uint8_t MIDI_TX_PIN = 17; // keep free for future MIDI OUT
// --- ROTARY ENCODER PINS ---------------------------------
constexpr uint8_t ENC_PIN_A = 32;  // CLK
constexpr uint8_t ENC_PIN_B = 33;  // DT
constexpr uint8_t ENC_SW_PIN = 25; // push button

constexpr uint8_t PPQN = 24;         // MIDI spec (don't change). 24 clocks per quarter note
constexpr uint8_t BEATS_PER_BAR = 4; // time‑signature top number
constexpr uint8_t QTR_PER_BEAT = 4;  // 4 QN per beat (4/4 time)

constexpr uint8_t CLOCKS_PER_QN = PPQN / QTR_PER_BEAT;    // 6
constexpr uint8_t CLOCKS_PER_BEAT = PPQN;                 // 24
constexpr uint16_t CLOCKS_PER_BAR = PPQN * BEATS_PER_BAR; // 96

// allowed phrase lengths we will step through
const uint8_t phraseTable[] = {1, 2, 4, 8, 16, 32};
const uint8_t tableSize = sizeof(phraseTable) / sizeof(phraseTable[0]);
volatile uint8_t barsPerPhrase = 4; // start with 4 (changes by encoder)
volatile uint8_t lastAB = 0;          // prev 2‑bit A|B
volatile int8_t  encSteps = 0;      // detent steps seen
volatile bool swPressed = false;

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
  st.bar = (st.clock / CLOCKS_PER_BAR) % barsPerPhrase;
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

// ——— Rotary encoder ISR ——————
void IRAM_ATTR isrEncSw()
{
  static uint32_t last = 0; // debounce 5 ms
  uint32_t now = millis();

  if (now - last > 5)
    swPressed = true;

  last = now;
}

void IRAM_ATTR isrEncChange()         // ONE ISR handles A and B
{
  static uint8_t lastAB = 0b11; // Start with both high (rest state)
  uint8_t ab = (digitalRead(ENC_PIN_A) << 1) | digitalRead(ENC_PIN_B);
  
  // Only update when state changes
  if (ab != lastAB) {
    // Calculate transition using previous and current state
    uint8_t transition = (lastAB << 2) | ab;
    
    switch (transition) {
      case 0b0001: // 00 -> 01 (CCW)
        encSteps--;
        break;
        
      case 0b0010: // 00 -> 10 (CW)
        encSteps++;
        break;
    }
    lastAB = ab; // Save current state
  }
}

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

  pinMode(ENC_PIN_A, INPUT_PULLUP);
  pinMode(ENC_PIN_B, INPUT_PULLUP);
  pinMode(ENC_SW_PIN, INPUT_PULLUP);

  attachInterrupt(ENC_PIN_A, isrEncChange, CHANGE);
  attachInterrupt(ENC_PIN_B, isrEncChange, CHANGE);
  attachInterrupt(ENC_SW_PIN, isrEncSw, FALLING); // active‑low push

  Serial.println("🟢 MIDI‑Metronome ready");
}

void loop()
{
  MIDI.read();

  if (encSteps != 0)
  {
    int8_t diff = encSteps;
    encSteps = 0;
    // find current index in phraseTable
    int idx = 0;
    while (idx < tableSize && phraseTable[idx] != barsPerPhrase) idx++;
    idx = constrain(idx + diff, 0, tableSize - 1);
    barsPerPhrase = phraseTable[idx];
    Serial.printf(">>> phrase len = %u bars\n", barsPerPhrase);
  }

  if (swPressed)
  { // reset phrase counter
    swPressed = false;
    Serial.println(">>> counters reset (button)");
  }
}
