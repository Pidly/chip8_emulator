#include <iostream>
#include <SDL2/SDL.h>


int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *sdlWindow = SDL_CreateWindow("chip8 emulator", 0, 0, 1000, 500, SDL_WINDOW_SHOWN);

    SDL_Quit();
    return 0;
}