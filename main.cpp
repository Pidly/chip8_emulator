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

    chip8.runEmulator();

    in.close();

    return 0;
}