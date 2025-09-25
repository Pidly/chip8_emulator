#ifndef OPERATIONPARSER_H
#define OPERATIONPARSER_H

class OperationParser {
    char16_t allTopFourBits;
public:
    OperationParser();
    enum OpCode {ClearScreen=0, LoadNormalRegister=1, LoadIndexRegister=2, DrawSprite=3, Jump=4, Skip=5, AddToNormalRegister=6, CompareVxEqualSkip=7, CompareVxNotEqualSkip=8, VxVyEqualSkip=9};
    OpCode parse(char16_t opp);
};

#endif //CHIP8_EMULATOR_OPERATIONPARSER_H