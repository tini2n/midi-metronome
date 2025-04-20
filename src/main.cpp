#include <Arduino.h>
#include <MIDI.h>

HardwareSerial hwSerial2(2);
MIDI_CREATE_INSTANCE(HardwareSerial, hwSerial2, MIDI);

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("Hello ESP‑WROOM‑32 MIDI Metronome!");

  hwSerial2.begin(31250, SERIAL_8N1, 16, 17);
  MIDI.begin(MIDI_CHANNEL_OMNI);

  Serial.println("MIDI started");
}

void loop()
{
  if (MIDI.read())
  {
    Serial.print("MIDI: ");
    Serial.print(MIDI.getType());
    switch (MIDI.getType())
    {

    case midi::Start:
      Serial.println("[START]");
      break;

    case midi::Clock:
      Serial.println("CLK"); // 24 per quarter‑note
      break;

    case midi::NoteOn:
      Serial.printf("NoteOn  ch:%d  %3d  vel:%3d\n",
                    MIDI.getChannel(),
                    MIDI.getData1(),
                    MIDI.getData2());
      break;

    case midi::NoteOff:
      Serial.printf("NoteOff ch:%d  %3d\n",
                    MIDI.getChannel(),
                    MIDI.getData1());
      break;

    default:
      // handle other message types if you want
      break;
    }
  }
}