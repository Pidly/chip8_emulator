#include <fstream>
#include <iostream>
#include <iomanip>
#include "OperationParser.h"
#include "Chip8.h"

using namespace std;

Chip8::Chip8(ifstream &in): in(in) {
    initFontData();
    readStream();
    programCounter = 0x200;
}


void Chip8::readStream() {
    char ch;
    int charNumbers = 0;
    while (in) {
        in.get(ch);
        if (in) {
            memory[512 + charNumbers] = ch;
            charNumbers++;
            cout << "char: " << ch;
            cout << " Hex: 0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(ch) << endl;
        } else {
            cout << "Rom size in bytes: " << charNumbers+1 << "\n";
        }
    }
}

void Chip8::runInstruction(char16_t instruction) {

    switch (parser.parse(instruction)) {
        case(OperationParser::ClearScreen): {
            screen.clearScreen();
            break;
        }
        case(OperationParser::LoadNormalRegister): {
            const auto registerNumber = static_cast<unsigned char>((instruction >> 8) & 0x0F);
            const auto value = static_cast<unsigned char>(instruction);
            registers[registerNumber] = value;
            break;
        }
        case(OperationParser::LoadIndexRegister): {
            indexRegister = (instruction & 0x0FFF);
            break;
        }
        case(OperationParser::DrawSprite): {
            int xRegisterIndex = ((instruction >> 8) & 0x0F);
            int yRegisterIndex = ((instruction >> 4) & 0x0F);
            int numOfBytes = ((instruction) & 0x0F);

            int xPos = registers[xRegisterIndex];
            int yPos = registers[yRegisterIndex];

            screen.drawSprite(xPos, yPos, numOfBytes, indexRegister, memory);
            break;
        }
        case(OperationParser::Jump): {
            programCounter = (instruction & 0x0FFF);
            break;
        }
        case(OperationParser::AddToNormalRegister): {
            unsigned char xRegister = (instruction >> 8) & 0x0F;
            unsigned char addValue = instruction & 0x0FF;
            registers[xRegister] += addValue;
            break;
        }
        case(OperationParser::CompareVxEqualSkip): {
            //3xNN Skip next instruction if Vx == NN
            unsigned char xRegister = (instruction >> 8) & 0x0F;
            unsigned char value = instruction & 0x0FF;
            if (registers[xRegister] == value) {
                programCounter += 2;
            }
            break;
        }
        case(OperationParser::CompareVxNotEqualSkip): {
            unsigned char xRegister = (instruction >> 8) & 0x0F;
            unsigned char value = instruction & 0x0FF;
            if (registers[xRegister] != value) {
                programCounter += 2;
            }
            break;
        }
        case(OperationParser::VxVyEqualSkip): {
            unsigned char xRegisterIndex = (instruction >> 8) & 0x0F;
            unsigned char yRegisterIndex = (instruction >> 4) & 0x0F;
            if (registers[xRegisterIndex] == registers[yRegisterIndex]) {
                programCounter += 2;
            }
            break;
        }
        default:
            break;
    }
}



void Chip8::readRomInstructions() {
    do {
        unsigned char topInstruction = memory[programCounter];
        unsigned char bottomInstruction = memory[programCounter + 1];
        programCounter = programCounter + 2;
        char16_t instruction = (static_cast<char16_t>(topInstruction) << 8 | bottomInstruction);

        runInstruction(instruction);
    } while (programCounter < sizeof(memory));
}


/*
 *Instructions to implement:
 * Ref: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
 *      https://johnearnest.github.io/Octo/docs/chip8ref.pdf
 *      https://github.com/trapexit/chip-8_documentation
 *  64x32 display = 2048 pixels
 * 00e0 -  Clear the display
 * Annn - LD I, addr
 *      The value of register I is set to nnn.
 * Dxyn - DRW Vx, Vy, nibble
 *     Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
The interpreter reads n bytes from memory, starting at the address stored in I.
These bytes are then displayed as sprites on screen at coordinates (Vx, Vy).
Sprites are XORed onto the existing screen. If this causes any pixels to be erased,
VF is set to 1, otherwise it is set to 0. If the sprite is positioned so part of it
is outside the coordinates of the display, it wraps around to the opposite side of the screen.
See instruction 8xy3 for more information on XOR, and section 2.4, Display, for more information
on the Chip-8 screen and sprites.
 * 1NNN jump NNN
 *      Jump to a machine code routine at nnn.
 *
    00E0 - Clear the screen
    6xnn - Load normal register with immediate value
    Annn - Load index register with immediate value
    Dxyn - Draw sprite to screen (only aligned)
    1nnn - Jump
 */

void Chip8::initFontData() {
    // 0
    memory[0x050] = 0xF0;
    memory[0x051] = 0x90;
    memory[0x052] = 0x90;
    memory[0x053] = 0x90;
    memory[0x054] = 0xF0;
    // 1
    memory[0x055] = 0x20;
    memory[0x056] = 0x60;
    memory[0x057] = 0x20;
    memory[0x058] = 0x20;
    memory[0x059] = 0x70;
    // 2
    memory[0x05A] = 0xF0;
    memory[0x05B] = 0x10;
    memory[0x05C] = 0xF0;
    memory[0x05D] = 0x80;
    memory[0x05E] = 0xF0;
    // 3
    memory[0x05F] = 0xF0;
    memory[0x060] = 0x10;
    memory[0x061] = 0xF0;
    memory[0x062] = 0x10;
    memory[0x063] = 0xF0;
    // 4
    memory[0x064] = 0x90;
    memory[0x065] = 0x90;
    memory[0x066] = 0xF0;
    memory[0x067] = 0x10;
    memory[0x068] = 0x10;
    // 5
    memory[0x069] = 0xF0;
    memory[0x06A] = 0x80;
    memory[0x06B] = 0xF0;
    memory[0x06C] = 0x10;
    memory[0x06D] = 0xF0;
    // 6
    memory[0x06E] = 0xF0;
    memory[0x06F] = 0x80;
    memory[0x070] = 0xF0;
    memory[0x071] = 0x90;
    memory[0x072] = 0xF0;
    // 7
    memory[0x073] = 0xF0;
    memory[0x074] = 0x10;
    memory[0x075] = 0x20;
    memory[0x076] = 0x40;
    memory[0x077] = 0x40;
    // 8
    memory[0x078] = 0xF0;
    memory[0x079] = 0x90;
    memory[0x07A] = 0xF0;
    memory[0x07B] = 0x90;
    memory[0x07C] = 0xF0;
    // 9
    memory[0x07D] = 0xF0;
    memory[0x07E] = 0x90;
    memory[0x07F] = 0xF0;
    memory[0x080] = 0x10;
    memory[0x081] = 0xF0;
    // A
    memory[0x082] = 0xF0;
    memory[0x083] = 0x90;
    memory[0x084] = 0xF0;
    memory[0x085] = 0x90;
    memory[0x086] = 0x90;
    // B
    memory[0x087] = 0xE0;
    memory[0x088] = 0x90;
    memory[0x089] = 0xE0;
    memory[0x08A] = 0x90;
    memory[0x08B] = 0xE0;
    // C
    memory[0x08C] = 0xF0;
    memory[0x08D] = 0x80;
    memory[0x08E] = 0x80;
    memory[0x08F] = 0x80;
    memory[0x090] = 0xF0;
    // D
    memory[0x091] = 0xE0;
    memory[0x092] = 0x90;
    memory[0x093] = 0x90;
    memory[0x094] = 0x90;
    memory[0x095] = 0xE0;
    // E
    memory[0x096] = 0xF0;
    memory[0x097] = 0x80;
    memory[0x098] = 0xF0;
    memory[0x099] = 0x80;
    memory[0x09A] = 0xF0;
    // F
    memory[0x09B] = 0xF0;
    memory[0x09C] = 0x80;
    memory[0x09D] = 0xF0;
    memory[0x09E] = 0x80;
    memory[0x09F] = 0x80;
}



