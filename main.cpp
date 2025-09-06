#include <iostream>
#include <SDL2/SDL.h>


int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_EVERYTHING);
    //chip8 64x32 display
    SDL_Window *sdlWindow = SDL_CreateWindow("chip8 emulator", 0, 0, 640, 320, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED);

    //Clear screen
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_Rect rect = SDL_Rect(0, 0, 100, 150);
    SDL_RenderFillRect(renderer, &rect);
    SDL_RenderPresent(renderer);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
    return 0;
}