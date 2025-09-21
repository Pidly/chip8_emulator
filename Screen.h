//
// Created by pidly on 9/16/25.
//

#ifndef CHIP8_EMULATOR_SCREEN_H
#define CHIP8_EMULATOR_SCREEN_H
#include <SDL2/SDL.h>
#include <iostream>

class Screen {
    //64x32 pixels. 8 pixels per character
    SDL_Window *sdlWindow;
    SDL_Renderer *renderer;

    unsigned char frameBuffer[64*32]; //256
public:
    Screen();
    ~Screen();
    void drawSprite(int x, int y, int numOfBytes, char16_t indexRegister, const unsigned char (&memory)[4096]);
    void clearScreen();
    void printScreen();
    void setSDLBackgroundColor();
    void setSDLSpriteColor();
};


#endif //CHIP8_EMULATOR_SCREEN_H