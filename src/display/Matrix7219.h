#pragma once
#include <MD_Parola.h>
#include <MD_MAX72xx.h>

constexpr uint8_t MODULES_PER_ROW = 4;
constexpr uint8_t ROWS = 2;
constexpr uint8_t DEV = MODULES_PER_ROW * ROWS; // 8 modules

class Matrix7219
{
public:
    void begin(uint8_t din, uint8_t clk, uint8_t cs, uint8_t intensity = 0);
    void splash();
    void showMetronome(uint8_t bar, uint8_t beat, uint8_t qn,
                       uint8_t meter, uint8_t phrase);
    void showEdit(const char *tag, uint8_t value);
    inline void loop() { _parola->displayAnimate(); }

private:
    void _define2LineLayout();
    MD_Parola *_parola = nullptr;
};
