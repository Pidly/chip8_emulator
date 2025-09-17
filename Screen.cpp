//
// Created by pidly on 9/16/25.
//

#include "Screen.h"

using namespace std;

Screen::Screen() {
    SDL_Init(SDL_INIT_EVERYTHING);
    sdlWindow = SDL_CreateWindow("chip8 emulator", 0, 0, 640, 320, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    //clearScreen();
}

Screen::~Screen() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
}

void Screen::clearScreen() {
    for (unsigned char & i : frameBuffer) {
        i = 0;
    }
}

void Screen::drawSprite(int x, int y, int numOfBytes, char16_t indexRegister, const unsigned char (&memory)[4096]) {

    for (int i = 0; i < numOfBytes; i++) {
        const unsigned char sprite = memory[indexRegister + i];
        //parse out each bit
        for (int p = 0; p < 8; p++) {
            unsigned char pixel = ((sprite >> p) & 0x01);
            //todo need to get individual pixel not the whole character DUH
            unsigned char frameBufferValue = frameBuffer[(8*y) + x];
            SDL_Rect rect = {x, y, 1, 1};

            if (pixel > 0) {
                if (frameBufferValue > 0) {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    //todo this needs to set individual pixel not the char
                    frameBuffer[(8*y) + x] = 0;
                } else {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                    //todo this needs to set individual pixel not the char
                    frameBuffer[(8*y) + x] = 1;
                }
            } else {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                //todo this needs to set individual pixel not the char
                frameBuffer[(8*y) + x] = 0;
            }
            SDL_RenderFillRect(renderer, &rect);
            SDL_RenderPresent(renderer);
        }
    }
    SDL_RenderPresent(renderer);
}

void Screen::printScreen() {
    for (int y = 0; y < 32; y++) {
        for (int c = 0; c < 8; c++) {
            char spring = frameBuffer[(y*8) + c];
            for (int i = 0; i < 8; i++) {
                unsigned char pixel = (spring >> i);
                if (pixel > 0) {
                    cout << "1";
                } else {
                    cout << "0";
                }
            }
        }
        cout << "\n";
    }
}





