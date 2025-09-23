//
// Created by pidly on 9/16/25.
//

#ifndef CHIP8_EMULATOR_CHIP8_H
#define CHIP8_EMULATOR_CHIP8_H
#include "OperationParser.h"
#include "Screen.h"

class Chip8 {
    //6XNN 	LD VX, NN 	CHIP-8 	Load immediate value NN into register VX
    //V0 to VF
    unsigned char registers[16];
    unsigned char memory[4096];
    unsigned char frameBuffer[64*32];
    char16_t indexRegister;
    char16_t programCounter;
    char16_t stack[16];
    void initFontData();
    std::ifstream &in;
    void readStream();
    OperationParser parser;
    Screen screen;
public:
    Chip8(std::ifstream &in);
    void readRomInstructions();
    void runInstruction(char16_t instruction);
};

#endif //CHIP8_EMULATOR_CHIP8_H