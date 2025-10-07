//
// Created by pidly on 9/16/25.
//

#ifndef CHIP8_EMULATOR_CHIP8_H
#define CHIP8_EMULATOR_CHIP8_H
#include "Screen.h"
#include <SDL2/SDL.h>
#include <map>
#include <chrono>

class Chip8 {
    uint8_t keypad[16];
    std::map<SDL_Keycode, uint8_t> keyMap;
    static constexpr uint8_t VF_REGISTER = 15;
    static constexpr uint8_t NUMBER_OF_KEYPAD_BUTTONS = 16;
    unsigned char registers[16];
    int delayTimer;
    int soundTimer;
    unsigned char memory[4096];
    char16_t indexRegister;
    char16_t programCounter;
    char16_t stack[16];
    int8_t stackPointer;
    void initFontData();
    std::ifstream &in;
    void readStream();
    void handleInput();
    Screen screen;
public:
    Chip8(std::ifstream &in);
    void readRomInstructions(int numberOfInstructions);
    void runInstruction(char16_t instruction);
    void runEmulator();
};

#endif //CHIP8_EMULATOR_CHIP8_H