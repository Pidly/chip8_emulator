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

bool Screen::drawSprite(int x, int y, int numOfBytes, char16_t indexRegister, const unsigned char (&memory)[4096]) {
    bool collision = false;
    bool wrapXSprite = false;
    bool wrapYSprite = false;
    if (x >= FRAME_BUFFER_WIDTH) {
        wrapXSprite = true;
    }
    if ((y*FRAME_BUFFER_WIDTH) >= FRAME_BUFFER_ELEMENTS) {
        wrapYSprite = true;
    }
    int pixelX;
    int pixelY;

    for (int i = 0; i < numOfBytes; i++) {
        const unsigned char sprite = memory[indexRegister + i];
        for (int j = 0; j < 8; j++) {
            unsigned char pixelOn = (sprite << j) & 0x80;
            if (pixelOn > 0) {
                if (wrapXSprite) {
                    pixelX = (x + j) % FRAME_BUFFER_WIDTH;
                } else {
                    pixelX = (x + j);
                    if (pixelX >= FRAME_BUFFER_WIDTH) {
                        break;
                    }
                }
                if (wrapYSprite) {
                    pixelY = ((y % FRAME_BUFFER_HEIGHT) + i) * 64;
                    if (pixelY >= FRAME_BUFFER_ELEMENTS) {
                        break;
                    }
                } else {
                    pixelY = (y + i) * FRAME_BUFFER_WIDTH;
                    if (pixelY >= FRAME_BUFFER_ELEMENTS) {
                        break;
                    }
                }
                if (frameBuffer[pixelX + pixelY] > 0) {
                    collision = true;
                    frameBuffer[pixelX + pixelY] = 0;
                } else {
                    frameBuffer[pixelY + pixelX] = 1;
                }
            }
        }
    }
    return collision;
}

void Screen::printScreen() {
    setSDLBackgroundColor();
    SDL_RenderClear(renderer);
    setSDLSpriteColor();

    for (int i = 0; i < (FRAME_BUFFER_ELEMENTS); i++) {
        int x = (i % 64) * 10;
        int y = (i / 64) * 10;
        int width = 10;
        int height = 10;

        if (frameBuffer[i] > 0) {
            SDL_Rect rect = {x, y, width, height};
            SDL_RenderFillRect(renderer, &rect);
        }
    }
    SDL_RenderPresent(renderer);
}

void Screen::setSDLBackgroundColor() {
    SDL_SetRenderDrawColor(renderer, 0, 51, 0, 255);
}

void Screen::setSDLSpriteColor() {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
}







