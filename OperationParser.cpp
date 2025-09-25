// 34 potential operations.
// https://johnearnest.github.io/Octo/docs/chip8ref.pdf
#include "OperationParser.h"
//    char16_t allTopFourBits = 0xF000;

OperationParser::OperationParser() {
    allTopFourBits = 0xF000;
}

OperationParser::OpCode OperationParser::parse(char16_t opp) {
    char opCodeCategory = (opp & allTopFourBits) >> 12;
    switch (opCodeCategory) {
        case(0x00):
            //OOE0 clear
            // Return from a subroutine.
            // The interpreter sets the program counter to the address at the top of the stack, then subtracts 1 from the stack pointer.
            //00EE return
            if ((opp & 0x000F) > 0) {
                return ReturnFromSubroutine;
            }
            return ClearScreen;
            break;
        case(0x01):
            //1NNN jump NNN
            return Jump;
            break;
        case(0x02):
            //2NNN Call a subroutine
            return CallSubroutine;
            break;
        case(0x03):
            //3XNN if vx == NN then skip the next operation (program counter + 2)
            return CompareVxEqualSkip;
        case(0x04):
            //4XNN if vx != NN then skip the next operation (program counter + 2)
            return CompareVxNotEqualSkip;
        case(0x05):
            // 5xy0 - Skip next instruction if Vx = Vy. (program counter + 2)
            return VxVyEqualSkip;
        case(0x06):
            //6XNN vx := NN
            return LoadNormalRegister;
            break;
        case(0x07):
            //7XNN vx += NN
            return AddToNormalRegister;
            break;
        case(0x09):
            // 9xy0 - Skip next instruction if Vx != Vy.
            return VxVyNotEqualSkip;
        case(0x0A):
            //ANNN i := NNN
            return LoadIndexRegister;
            break;
        case(0x0B):
            return Skip;
            //BNNN jump0 NNN | jump to address NNN + v0
        case(0x0D):
            //DXYN sprite vx vy N | vf == 1 on collision
            return DrawSprite;
            break;
        case(0x0F):
            return Skip;
            break;
        default:
            return Skip;
            break;
    }
}

