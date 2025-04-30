#pragma once
#include <stdint.h>

// MIDI
constexpr uint8_t MIDI_RX_PIN = 16;
constexpr uint8_t MIDI_TX_PIN = 17;
constexpr uint8_t PIN_LED_CLK = 26;

// Encoder
constexpr uint8_t ENC_PIN_A = 32;  // CLK
constexpr uint8_t ENC_PIN_B = 33;  // DT
constexpr uint8_t ENC_SW_PIN = 25; // push button

// Matrix 7219
constexpr uint8_t DIN_PIN = 23; // data in
constexpr uint8_t CLK_PIN = 18; // clock
constexpr uint8_t CS_PIN = 21;  // chip select
