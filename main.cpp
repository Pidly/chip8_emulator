#include <iostream>
#include <SDL2/SDL.h>
#include <fstream>
#include <iomanip>
#include "Chip8.h"

using namespace std;

int main(int argc, char* argv[]) {
    ifstream in (argv[1], ios::in | ios::binary);

    if (!in) {
        cout << "Cannot open file to read: " << argv[1] << "\n";
        return -1;
    }

    Chip8 chip8(in);

    chip8.runInstruction(0x00E0);
    chip8.runInstruction(0x6102);
    chip8.runInstruction(0x6005);
    chip8.runInstruction(0xA091);
    chip8.runInstruction(0xD105);
    chip8.runInstruction(0x8100);

    cout << "Running instructions\n";
    chip8.runInstruction(0x00E0);

    chip8.readRomInstructions();

    in.close();

    return 0;
}