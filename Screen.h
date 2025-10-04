//
// Created by pidly on 9/16/25.
//

#ifndef CHIP8_EMULATOR_SCREEN_H
#define CHIP8_EMULATOR_SCREEN_H
#include <SDL2/SDL.h>

class Screen {
    SDL_Window *sdlWindow;
    SDL_Renderer *renderer;
    static constexpr int FRAME_BUFFER_WIDTH = 64;
    static constexpr int FRAME_BUFFER_HEIGHT = 32;
    static constexpr int FRAME_BUFFER_ELEMENTS = FRAME_BUFFER_WIDTH * FRAME_BUFFER_HEIGHT;
    unsigned char frameBuffer[FRAME_BUFFER_ELEMENTS];
public:
    Screen();
    ~Screen();
    bool drawSprite(int x, int y, int numOfBytes, char16_t indexRegister, const unsigned char (&memory)[4096]);
    void clearScreen();
    void printScreen();
    void setSDLBackgroundColor();
    void setSDLSpriteColor();
};


#endif //CHIP8_EMULATOR_SCREEN_H