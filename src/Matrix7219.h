#pragma once
#include <MD_Parola.h>
#include <MD_MAX72xx.h>

class Matrix7219
{
public:
    // call once in setup()
    void begin(uint8_t din, uint8_t clk, uint8_t cs,
               uint8_t intensity = 0 /*0‑15*/);

    // 32×16 text helpers
    void splash();
    void showMetronome(uint8_t bar, uint8_t beat, uint8_t qn, uint16_t bpm);

    // must be called from loop()
    void loop() { if (_parola) _parola->displayAnimate(); }

private:
    static constexpr uint8_t DEV = 8; // 4×2 blocks
                                      //   MD_Parola _parola = MD_Parola(MD_MAX72XX::FC16_HW, 23, 18, 21, DEV);
    MD_Parola *_parola = nullptr;
};
