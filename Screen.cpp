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
    clearScreen();
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
    printScreen();
}

void Screen::drawSprite(int x, int y, int numOfBytes, char16_t indexRegister, const unsigned char (&memory)[4096]) {

    for (int i = 0; i < numOfBytes; i++) {
        const unsigned char sprite = memory[indexRegister + i];

        for (int j = 0; j < 8; j++) {
            unsigned char pixelOn = (sprite << j) & 0x80;
            if (pixelOn > 0) {
                int pixelX = x + j;
                int pixelY = (y + i) * 64;
                if (frameBuffer[pixelX + pixelY] > 0) {
                    frameBuffer[pixelX + pixelY] = 0;
                } else {
                    frameBuffer[pixelY + pixelX] = 1;
                }
            }
        }
    }
    printScreen();
}

void Screen::printScreen() {
    cout << "\n";

    for (int i = 0; i < sizeof(frameBuffer); i++) {
        if (i % 64 == 0) {
            cout << "\n";
        }
        int x = (i % 64) * 10;
        int y = (i / 64) * 10;
        int width = 10;
        int height = 10;

        if (frameBuffer[i] > 0) {
            setSDLSpriteColor();
            unsigned char fbValue = frameBuffer[i];
            SDL_Rect rect = {x, y, width, height};
            SDL_RenderFillRect(renderer, &rect);
             cout << "1";
        } else {
            setSDLBackgroundColor();
            SDL_Rect rect = {x, y, width, height};
            SDL_RenderFillRect(renderer, &rect);
            cout << "0";
        }
    }
    SDL_RenderPresent(renderer);
}

void Screen::setSDLBackgroundColor() {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
}

void Screen::setSDLSpriteColor() {
    SDL_SetRenderDrawColor(renderer, 0, 51, 0, 255);
}







