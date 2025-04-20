# ESP32 MIDI Metronome

Minimal, hardware‑synced metronome / bar counter for live electronic musicians.

| Feature | Status |
|---------|--------|
| MIDI Clock (24 PPQN) input | ✅ |
| Beat / Bar / 16th counters | ✅ |
| BPM auto‑detect with smoothing | ✅ |
| 4‑bar phrase wrap | ✅ (configurable) |

## Wiring

DIN‑5 pin4 ──220Ω──►| 6N138 LED |──220Ω── DIN‑5 pin5
6N138 pin6 → ESP32 GPIO16
Shield 3V3, GND shared

## Build / Flash

```bash
git clone https://github.com/tini2n/midi-metronome.git
cd esp32-midi-metronome
pio run -t upload -e esp32dev        # requires PlatformIO
pio device monitor -b 115200
```
