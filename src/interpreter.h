#pragma once

#include <string>
#include <vector>

class Interpreter {
    std::vector<std::string> program;

    std::vector<std::string> stack;
    std::string valueAccumulator;

    int moveX = 1, moveY = 0;
    int posX = 0, posY = 0;

    bool printNext = true, ignoreBlock = false, escapeNext = false;
    bool collectingGroup = false;

    char awaitingOperator = '\0';
    std::string auxValueAccumulator;

    bool evalCell();
    bool interpret(char c);

    void updateValueAcc(const std::string &s);
    bool setAwaitingOperator(char c);
    bool handleAwaitingOperator();
    void applyCondition(bool cond);
    bool convertToInt(const std::string &s, int &result);

    void printErrorMsg(const std::string& msg);

public:
    Interpreter() {}

    void loadProgram(const std::string& filename);
    void runProgram();
};
