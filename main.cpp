#include <iostream>
#include <SDL2/SDL.h>
#include <fstream>
#include <iomanip>
#include "OperationParser.h"
#include "Chip8.h"

using namespace std;

int main(int argc, char* argv[]) {
    //SDL_Init(SDL_INIT_EVERYTHING);
    //chip8 64x32 display
    //SDL_Window *sdlWindow = SDL_CreateWindow("chip8 emulator", 0, 0, 640, 320, SDL_WINDOW_SHOWN);
    //SDL_Renderer *renderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED);

    //Clear screen
    /*
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    //0 0 100 150
    SDL_Rect rect = {0, 0, 100, 150};
    SDL_RenderFillRect(renderer, &rect);
    SDL_RenderPresent(renderer);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
    */

    char16_t newch = 0x6101;
    OperationParser parser{};
    parser.parse(newch);


    /*
    char *testFile = "test file";
    ofstream out("test", ios::out | ios::binary);
    if (!out) {
        cout << "Cannot open file to write.\n";
        return -1;
    }
    while (*testFile) out.put(*testFile++);

    out.close();

    if (argc != 2) {
        cout << "Usage: program <filename>\n";
        return -1;
    } else {
        cout << "file argument: " << argv[1] << "\n";
    }
    */

    ifstream in (argv[1], ios::in | ios::binary);

    if (!in) {
        cout << "Cannot open file to read: " << argv[1] << "\n";
        return -1;
    }

    Chip8 chip8(in);
    chip8.readRomInstructions();
    /*
    char ch;
    int charNumbers = 0;
    while (in) {
        in.get(ch);
        charNumbers++;
        if (in) {
            cout << "char: " << ch;
            cout << " Hex: 0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(ch) << endl;
        } else {
            cout << "Char numbers: " << charNumbers << "\n";
        }
    }*/
    in.close();

    return 0;
}