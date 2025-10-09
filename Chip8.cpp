#include <fstream>
#include <iostream>
#include <iomanip>
#include "Chip8.h"
#include <stdexcept>
#include <sstream>

using namespace std;

void audioCallback(void *userdata, uint8_t *stream, int len) {
    int volume = 1000;
    int audio_sample_rate = 44100;
    int square_wave_freq = 440;

    int16_t *audio_data = (int16_t *)stream;
    static uint32_t running_sample_index = 0;

    const int32_t square_wave_period = audio_sample_rate / square_wave_freq;
    const int32_t half_square_wave_period = square_wave_period / 2;

    for (int i = 0; i < len / 2; i++)
        audio_data[i] = ((running_sample_index++ / half_square_wave_period) % 2) ?
                        volume :
                        -volume;
}

Chip8::Chip8(ifstream &in): in(in) {
    initFontData();
    stackPointer = 0;
    delayTimer = 60;
    soundTimer = 60;
    readStream();
    programCounter = 0x200;
    keyMap = {
        {SDLK_1, 0x1}, {SDLK_2, 0x2}, {SDLK_3, 0x3}, {SDLK_4, 0xC},
        {SDLK_q, 0x4}, {SDLK_w, 0x5}, {SDLK_e, 0x6}, {SDLK_r, 0xD},
        {SDLK_a, 0x7}, {SDLK_s, 0x8}, {SDLK_d, 0x9}, {SDLK_f, 0xE},
        {SDLK_z, 0xA}, {SDLK_x, 0x0}, {SDLK_c, 0xB}, {SDLK_v, 0xF}
    };

    spec = (SDL_AudioSpec){
        .freq = 44100,
        .format = AUDIO_S16LSB,
        .channels = 1,
        .samples = 512,
        .callback = audioCallback
    };
    deviceId = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);
}

Chip8::~Chip8() {
    SDL_FreeAudioStream(stream);
}


void Chip8::readStream() {
    char ch;
    int charNumbers = 0;
    while (in) {
        in.get(ch);
        if (in) {
            memory[512 + charNumbers] = ch;
            charNumbers++;
        } else {
            cout << "Rom size in bytes: " << charNumbers+1 << "\n";
        }
    }
}

void Chip8::runInstruction(char16_t instruction) {
    //Switch based on the top 4 bits of the instruction
    switch ((instruction >> 12) & 0x0F) {
        case(0x0): {
            // 00EE return from subroutine
            // The interpreter sets the program counter to the address at the top of the stack,
            // then subtracts 1 from the stack pointer.
            if ((instruction & 0x00FF) == 0xEE) {
                if (stackPointer < 0) {
                    std::stringstream ss;
                    ss << std::hex << std::setw(4) << std::setfill('0') << static_cast<uint16_t>(instruction);

                    std::string hexString = ss.str();
                    throw std::out_of_range("Stack pointer size: " + std::to_string(stackPointer) +
                        " smaller than 0 on instruction: " + hexString + " programCounter=" +
                        std::to_string(programCounter));
                }
                programCounter = stack[stackPointer];
                stackPointer -= 1;
            } else if ((instruction & 0x00FF) == 0x00E0) {
                // OOE0 clear
                screen.clearScreen();
            }
            break;
        }
        case(0x1): {
            //1NNN jump NNN
            programCounter = (instruction & 0x0FFF);
            break;
        }
        case(0x2): {
            /*
            *Call subroutine at nnn.
            *The interpreter increments the stack pointer, then puts the current PC on the top of the stack. The PC is then set to nnn.
             */
            stackPointer += 1;
            int maxStackValues = sizeof(stack) / sizeof(stack[0]) - 1;
            if (stackPointer > maxStackValues) {
                std::stringstream ss;
                ss << std::hex << std::setw(4) << std::setfill('0') << static_cast<uint16_t>(instruction);

                std::string hexString = ss.str();
                throw std::out_of_range("Stack pointer size: " + std::to_string(stackPointer) +
                    " larger than stack size on instruction: " + hexString + " programCounter=" +
                    std::to_string(programCounter));
            }
            stack[stackPointer] = programCounter;
            programCounter = instruction & 0x0FFF;
            break;
        }
        case(0x3): {
            //3xNN Skip next instruction if Vx == NN
            unsigned char xRegister = (instruction >> 8) & 0x0F;
            unsigned char value = instruction & 0x0FF;
            if (registers[xRegister] == value) {
                programCounter += 2;
            }
            break;
        }
        case(0x4): {
            //4XNN if vx != NN then skip the next operation (program counter + 2)
            unsigned char xRegister = (instruction >> 8) & 0x0F;
            unsigned char value = instruction & 0x0FF;
            if (registers[xRegister] != value) {
                programCounter += 2;
            }
            break;
        }
        case(0x5): {
            // 5xy0 - Skip next instruction if Vx = Vy. (program counter + 2)
            unsigned char xRegisterIndex = (instruction >> 8) & 0x0F;
            unsigned char yRegisterIndex = (instruction >> 4) & 0x0F;
            if (registers[xRegisterIndex] == registers[yRegisterIndex]) {
                programCounter += 2;
            }
            break;
        }
        case(0x6): {
            //6XNN load register Vx with value NN
            const auto registerNumber = static_cast<unsigned char>((instruction >> 8) & 0x0F);
            const auto value = static_cast<unsigned char>(instruction);
            registers[registerNumber] = value;
            break;
        }
        case(0x7): {
            //7XNN Vx += NN
            unsigned char xRegister = (instruction >> 8) & 0x0F;
            unsigned char addValue = instruction & 0x00FF;
            registers[xRegister] += addValue;
            break;
        }
        case(0x8): {
            unsigned char xRegister = (instruction >> 8) & 0x0F;
            unsigned char yRegister = (instruction >> 4) & 0x0F;

            switch (instruction & 0x000F) {
                case(0): {
                    // 8xy0 - LD Vx, Vy; Stores the value of register Vy in register Vx.
                    registers[xRegister] = registers[yRegister];
                    break;
                }
                case(1): {
                    // 8xy1 - OR Vx, Vy; Set Vx = Vx OR Vy. Performs a bitwise OR on the values of Vx and Vy.
                    registers[xRegister] = registers[xRegister] | registers[yRegister];
                    registers[VF_REGISTER] = 0;
                    break;
                }
                case(2): {
                    //8xy2 - AND Vx, Vy
                    //Set Vx = Vx AND Vy.
                    //Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx. A bitwise AND
                    //compares the corrseponding bits from two values, and if both bits are 1, then the same bit in
                    //the result is also 1. Otherwise, it is 0.
                    registers[xRegister] = registers[xRegister] & registers[yRegister];
                    registers[VF_REGISTER] = 0;
                    break;
                }
                case(3): {
                    //8xy3 - XOR Vx, Vy
                    //Set Vx = Vx XOR Vy.
                    //Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx.
                    //An exclusive OR compares the corrseponding bits from two values, and if the bits are not both
                    //the same, then the corresponding bit in the result is set to 1. Otherwise, it is 0.
                    registers[xRegister] = registers[xRegister] ^ registers[yRegister];
                    registers[VF_REGISTER] = 0;
                    break;
                }
                case(4): {
                    //8xy4 - ADD Vx, Vy
                    //Set Vx = Vx + Vy, set VF = carry.
                    //The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,)
                    //VF is set to 1, otherwise 0. Only the lowest 8 bits of the result are kept, and stored in Vx.
                    uint16_t result = registers[xRegister] + registers[yRegister];
                    if (result > 255) {
                        registers[xRegister] = (result & 0xFF);
                        registers[VF_REGISTER] = 1;
                    } else {
                        registers[xRegister] = result;
                        registers[VF_REGISTER] = 0;
                    }
                    break;
                }
                case(5): {
                    //8xy5 - SUB Vx, Vy
                    //Set Vx = Vx - Vy, set VF = NOT borrow.
                    //If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.
                    int result = registers[xRegister] - registers[yRegister];
                    if (result < 0) {
                        //ordering is important here because if vX(15) IS vF register(also 15) some un-expected behavior can happen.
                        //although I'm not sure why a program would try to store the results in the vF register since it's used for the carry.

                        //setting the register first CAUSES ERRORS on some tests because vF register carry gets overwritten the next line with the result.
                        registers[xRegister] = result;
                        registers[VF_REGISTER] = 0x00;
                    } else {
                        registers[xRegister] = result;
                        registers[VF_REGISTER] = 0x01;
                    }
                    break;
                }
                case(6): {
                    // 8xy6 - SHR Vx {, Vy}
                    // Set Vx = Vx SHR 1.
                    //If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.
                    registers[xRegister] = registers[yRegister];
                    if ((registers[xRegister] & 0x01) > 0) {
                        registers[xRegister] = registers[xRegister] >> 1;
                        registers[VF_REGISTER] = 1;
                    } else {
                        registers[xRegister] = registers[xRegister] >> 1;
                        registers[VF_REGISTER] = 0;
                    }
                    break;
                }
                case(7): {
                    //8xy7 - SUBN Vx, Vy
                    //Set Vx = Vy - Vx, set VF = NOT borrow.
                    //If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.
                    registers[xRegister] = registers[yRegister] - registers[xRegister];
                    if (registers[yRegister] > registers[xRegister]) {
                        registers[VF_REGISTER] = 1;
                    } else {
                        registers[VF_REGISTER] = 0;
                    }
                    break;
                }
                case(0xE): {
                    //8xyE - SHL Vx {, Vy}
                    //Set Vx = Vx SHL 1.
                    //If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
                    registers[xRegister] = registers[yRegister];
                    if ((registers[xRegister] & 0x80) > 1) {
                        registers[xRegister] = registers[xRegister] << 1;
                        registers[VF_REGISTER] = 1;
                    } else {
                        registers[xRegister] = registers[xRegister] << 1;
                        registers[VF_REGISTER] = 0;
                    }
                    break;
                }
            }
            break;
        }
        case(0x9): {
            // 9xy0 - Skip next instruction if Vx != Vy.
            unsigned char xRegisterIndex = (instruction >> 8) & 0x0F;
            unsigned char yRegisterIndex = (instruction >> 4) & 0x0F;
            if (registers[xRegisterIndex] != registers[yRegisterIndex]) {
                programCounter += 2;
            }
            break;
        }
        case(0xA): {
            //ANNN i := NNN
            indexRegister = (instruction & 0x0FFF);
            break;
        }
        case(0xB): {
            //Bnnn - JP V0, addr
            //Jump to location nnn + V0.
            //The program counter is set to nnn plus the value of V0.
            programCounter = (instruction & 0x0FFF);
            break;
        }
        case(0xD): {
            //DXYN sprite vx vy N | vf == 1 on collision
            int xRegisterIndex = ((instruction >> 8) & 0x0F);
            int yRegisterIndex = ((instruction >> 4) & 0x0F);
            int numOfBytes = ((instruction) & 0x0F);

            int xPos = registers[xRegisterIndex];
            int yPos = registers[yRegisterIndex];

            bool collision = screen.drawSprite(xPos, yPos, numOfBytes, indexRegister, memory);
            if (collision) {
                registers[VF_REGISTER] = 1;
            } else {
                registers[VF_REGISTER] = 0;
            }
            break;
        }
        case(0xE): {
            unsigned char leastSignificantByte = instruction;
            uint8_t keyCode = registers[(instruction >> 8) & 0x0F];
            //Ex9E - SKP Vx
            //Skip next instruction if key with the value of Vx is pressed.
            //Checks the keyboard, and if the key corresponding to the value of Vx is currently in the down position, PC is increased by 2.
            if (leastSignificantByte == 0x9E) {
                if (keypad[keyCode] > 0) {
                    programCounter += 2;
                }
            }
            //ExA1 - SKNP Vx
            //Skip next instruction if key with the value of Vx is not pressed.
            //Checks the keyboard, and if the key corresponding to the value of Vx is currently in the up position, PC is increased by 2.
            if (leastSignificantByte == 0xA1) {
                if (keypad[keyCode] == 0) {
                    programCounter += 2;
                }
            }
            break;
        }
        case(0xF): {
            uint8_t xRegisterIndex = (instruction >> 8) & 0x0F;
            //Fx65, Fx55, Fx33, Fx1E
            switch (instruction & 0xFF) {
                case(0x07): {
                    //Fx07 - LD Vx, DT
                    //Set Vx = delay timer value.
                    //The value of DT is placed into Vx.
                    registers[xRegisterIndex] = delayTimer;
                    break;
                }
                case(0x0A): {
                    //Fx0A - LD Vx, K
                    //Wait for a key press, store the value of the key in Vx.
                    //All execution stops until a key is pressed, then the value of that key is stored in Vx.
                    int16_t keyPadIndex = -1;
                    while (keyPadIndex < 0) {
                        handleInput();
                        for (int i = 0; i < NUMBER_OF_KEYPAD_BUTTONS; i++) {
                            if (keypad[i] > 0) {
                                keyPadIndex = i;
                            }
                        }
                    }
                    registers[xRegisterIndex] = keypad[keyPadIndex];
                    break;
                }
                case(0x15): {
                    //Fx15 - LD DT, Vx
                    //Set delay timer = Vx.
                    //DT is set equal to the value of Vx.
                    delayTimer = registers[xRegisterIndex];
                    break;
                }
                case(0x18): {
                    //Fx18 - LD ST, Vx
                    //Set sound timer = Vx.
                    //ST is set equal to the value of Vx.
                    soundTimer = registers[xRegisterIndex];
                    break;
                }
                case(0x1E): {
                    //Fx1E - ADD I, Vx
                    //Set I = I + Vx.
                    //The values of I and Vx are added, and the results are stored in I.
                    indexRegister = indexRegister + registers[xRegisterIndex];
                    break;
                }
                case(0x33): {
                    //Fx33 - LD B, Vx
                    //Store BCD representation of Vx in memory locations I, I+1, and I+2.
                    //The interpreter takes the decimal value of Vx, and places the hundreds digit in memory
                    //at location in I, the tens digit at location I+1, and the ones digit at location I+2.
                    unsigned char hundreds = registers[xRegisterIndex] / 100;
                    unsigned char tens = (registers[xRegisterIndex] % 100) / 10;
                    unsigned char ones = registers[xRegisterIndex] % 10;
                    memory[indexRegister] = hundreds;
                    memory[indexRegister + 1] = tens;
                    memory[indexRegister + 2] = ones;
                    break;
                }
                case(0x55): {
                    //Fx55 - LD [I], Vx
                    //Store registers V0 through Vx in memory starting at location I.
                    //The interpreter copies the values of registers V0 through Vx into memory, starting at the address in I.
                    for (int i = 0; i <= xRegisterIndex; i++) {
                        memory[indexRegister] = registers[i];
                        indexRegister++;
                    }
                    break;
                }
                case(0x65): {
                    //Fx65 - LD Vx, [I]
                    //Read registers V0 through Vx from memory starting at location I.
                    //The interpreter reads values from memory starting at location I into registers V0 through Vx.
                    for (int i = 0; i <= xRegisterIndex; i++) {
                        registers[i] = memory[indexRegister];
                        indexRegister++;
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
}



void Chip8::readRomInstructions(int numberOfInstructions) {
    int numOfInsExecuted = 0;
    do {
        unsigned char topInstruction = memory[programCounter];
        unsigned char bottomInstruction = memory[programCounter + 1];
        programCounter = programCounter + 2;
        char16_t instruction = (static_cast<char16_t>(topInstruction) << 8 | bottomInstruction);

        try {
            runInstruction(instruction);
        } catch (std::out_of_range &e) {
            std::cerr << "Out of range exception: " << e.what() << endl;
            return;
        }
        numOfInsExecuted++;
    } while (numOfInsExecuted < numberOfInstructions);
}

void Chip8::runEmulator() {
    const double fps = 60;
    const std::chrono::nanoseconds frame_duration(static_cast<long long>(1.0 / fps * 1'000'000'000));
    auto lastFrameTime = chrono::high_resolution_clock::now();

    bool running = true;
    int numOfInstructions = 10;

    while (running) {
        auto currentTime = chrono::high_resolution_clock::now();
        auto elapsedTime = currentTime - lastFrameTime;

        if (elapsedTime >= frame_duration) {
            handleInput();
            delayTimer--;
            soundTimer--;
            readRomInstructions(numOfInstructions);
            lastFrameTime = currentTime;
            screen.printScreen();
        }

        if (delayTimer < 0) {
            delayTimer = 60;
        }
        if (soundTimer > 0) {
            //Audio on
            SDL_PauseAudioDevice(deviceId, 0);
        } else {
            soundTimer = 0;
            //Audio off
            SDL_PauseAudioDevice(deviceId, 1);
        }
    }
}

void Chip8::handleInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_KEYDOWN) {
            auto it = keyMap.find(event.key.keysym.sym);
            if (it != keyMap.end()) {
                keypad[it->second] = 1; // Mark key as pressed
            }
        } else if (event.type == SDL_KEYUP) {
            auto it = keyMap.find(event.key.keysym.sym);
            if (it != keyMap.end()) {
                keypad[it->second] = 0; // Mark key as released
            }
        }
    }
}



/*
 *Instructions to implement:
 * Ref: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
 *      https://johnearnest.github.io/Octo/docs/chip8ref.pdf
 *      https://github.com/trapexit/chip-8_documentation
 *  64x32 display = 2048 pixels
 * 00e0 -  Clear the display
 * Annn - LD I, addr
 *      The value of register I is set to nnn.
 * Dxyn - DRW Vx, Vy, nibble
 *     Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
The interpreter reads n bytes from memory, starting at the address stored in I.
These bytes are then displayed as sprites on screen at coordinates (Vx, Vy).
Sprites are XORed onto the existing screen. If this causes any pixels to be erased,
VF is set to 1, otherwise it is set to 0. If the sprite is positioned so part of it
is outside the coordinates of the display, it wraps around to the opposite side of the screen.
See instruction 8xy3 for more information on XOR, and section 2.4, Display, for more information
on the Chip-8 screen and sprites.
 * 1NNN jump NNN
 *      Jump to a machine code routine at nnn.
 *
    00E0 - Clear the screen
    6xnn - Load normal register with immediate value
    Annn - Load index register with immediate value
    Dxyn - Draw sprite to screen (only aligned)
    1nnn - Jump
 */

void Chip8::initFontData() {
    // 0
    memory[0x050] = 0xF0;
    memory[0x051] = 0x90;
    memory[0x052] = 0x90;
    memory[0x053] = 0x90;
    memory[0x054] = 0xF0;
    // 1
    memory[0x055] = 0x20;
    memory[0x056] = 0x60;
    memory[0x057] = 0x20;
    memory[0x058] = 0x20;
    memory[0x059] = 0x70;
    // 2
    memory[0x05A] = 0xF0;
    memory[0x05B] = 0x10;
    memory[0x05C] = 0xF0;
    memory[0x05D] = 0x80;
    memory[0x05E] = 0xF0;
    // 3
    memory[0x05F] = 0xF0;
    memory[0x060] = 0x10;
    memory[0x061] = 0xF0;
    memory[0x062] = 0x10;
    memory[0x063] = 0xF0;
    // 4
    memory[0x064] = 0x90;
    memory[0x065] = 0x90;
    memory[0x066] = 0xF0;
    memory[0x067] = 0x10;
    memory[0x068] = 0x10;
    // 5
    memory[0x069] = 0xF0;
    memory[0x06A] = 0x80;
    memory[0x06B] = 0xF0;
    memory[0x06C] = 0x10;
    memory[0x06D] = 0xF0;
    // 6
    memory[0x06E] = 0xF0;
    memory[0x06F] = 0x80;
    memory[0x070] = 0xF0;
    memory[0x071] = 0x90;
    memory[0x072] = 0xF0;
    // 7
    memory[0x073] = 0xF0;
    memory[0x074] = 0x10;
    memory[0x075] = 0x20;
    memory[0x076] = 0x40;
    memory[0x077] = 0x40;
    // 8
    memory[0x078] = 0xF0;
    memory[0x079] = 0x90;
    memory[0x07A] = 0xF0;
    memory[0x07B] = 0x90;
    memory[0x07C] = 0xF0;
    // 9
    memory[0x07D] = 0xF0;
    memory[0x07E] = 0x90;
    memory[0x07F] = 0xF0;
    memory[0x080] = 0x10;
    memory[0x081] = 0xF0;
    // A
    memory[0x082] = 0xF0;
    memory[0x083] = 0x90;
    memory[0x084] = 0xF0;
    memory[0x085] = 0x90;
    memory[0x086] = 0x90;
    // B
    memory[0x087] = 0xE0;
    memory[0x088] = 0x90;
    memory[0x089] = 0xE0;
    memory[0x08A] = 0x90;
    memory[0x08B] = 0xE0;
    // C
    memory[0x08C] = 0xF0;
    memory[0x08D] = 0x80;
    memory[0x08E] = 0x80;
    memory[0x08F] = 0x80;
    memory[0x090] = 0xF0;
    // D
    memory[0x091] = 0xE0;
    memory[0x092] = 0x90;
    memory[0x093] = 0x90;
    memory[0x094] = 0x90;
    memory[0x095] = 0xE0;
    // E
    memory[0x096] = 0xF0;
    memory[0x097] = 0x80;
    memory[0x098] = 0xF0;
    memory[0x099] = 0x80;
    memory[0x09A] = 0xF0;
    // F
    memory[0x09B] = 0xF0;
    memory[0x09C] = 0x80;
    memory[0x09D] = 0xF0;
    memory[0x09E] = 0x80;
    memory[0x09F] = 0x80;
}



