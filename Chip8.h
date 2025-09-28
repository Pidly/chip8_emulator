//
// Created by pidly on 9/16/25.
//

#ifndef CHIP8_EMULATOR_CHIP8_H
#define CHIP8_EMULATOR_CHIP8_H
#include "Screen.h"
#include <chrono>

class Chip8 {
    static constexpr uint8_t VF_REGISTER = 15;
    unsigned char registers[16];
    int8_t delayTimer;
    unsigned char memory[4096];
    char16_t indexRegister;
    char16_t programCounter;
    char16_t stack[16];
    int8_t stackPointer;
    void initFontData();
    std::ifstream &in;
    void readStream();
    Screen screen;
public:
    Chip8(std::ifstream &in);
    void readRomInstructions(int numberOfInstructions);
    void runInstruction(char16_t instruction);
    void runEmulator();
};

#endif //CHIP8_EMULATOR_CHIP8_H