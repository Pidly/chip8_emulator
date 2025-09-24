#include <iostream>
#include <SDL2/SDL.h>
#include <fstream>
#include <iomanip>
#include "OperationParser.h"
#include "Chip8.h"

using namespace std;

int main(int argc, char* argv[]) {
    char16_t newch = 0x6101;
    OperationParser parser{};
    parser.parse(newch);

    ifstream in (argv[1], ios::in | ios::binary);

    if (!in) {
        cout << "Cannot open file to read: " << argv[1] << "\n";
        return -1;
    }

    Chip8 chip8(in);
    //Vx, Vy, number of bytes.

    chip8.runInstruction(0x00E0);
    chip8.runInstruction(0x6102);
    chip8.runInstruction(0x6005);
    chip8.runInstruction(0xA091);
    chip8.runInstruction(0xD105);

    cout << "Running instructions\n";
    chip8.runInstruction(0x00E0);

    chip8.readRomInstructions();

    in.close();

    return 0;
}