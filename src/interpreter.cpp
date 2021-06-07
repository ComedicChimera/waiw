#include "interpreter.h"

#include <iostream>
#include <fstream>
#include <cstdio>
#include <algorithm>

void Interpreter::loadProgram(const std::string& filename) {
    std::ifstream file;
    file.open(filename);
    if (file.is_open()) {
        std::string line;
        while (getline(file, line)) {
            program.push_back(line);
        }

        file.close();
    } else {
        std::cout << "error: failed to open file\n";
    }
}

void Interpreter::runProgram() {
    while (evalCell()) {
        if (moveY != 0) {
            posY += moveY;
            moveY = 0;
        } else {
            posX += moveX;
        }
    }
}

bool Interpreter::evalCell() {
    if (-1 < posY && posY < program.size()) {
        if (-1 < posX && posX < program[posY].length()) {
            return interpret(program[posY][posX]);
        }
    }

    return false;
}

bool Interpreter::interpret(char c) {
    if (escapeNext) {
        escapeNext = false;

        if (ignoreBlock) {
            // if we are in an ignore, then escape just discards the character
            return true;
        }

        updateValueAcc(std::string(1, c));
        if (!collectingGroup) {
            return handleAwaitingOperator();
        }

        return true;
    } else if (ignoreBlock) {
        if (c == ']') {
            ignoreBlock = false;   
        }

        return true;
    } else {     
        // used for input inside switch statement
        std::string line;
        switch (c) {
            // movement
            case '.':
                posX = 0;
                std::cout << '\n';
            case ';':
                moveY = 1;
                break;
            case '^':
                moveY = -1;
                break;
            case '{':
                moveX = 1;
                break;
            case '}':
                moveX = -1;
                break;
            case '!':
                return false;
            // stack operations and input
            case '_':
                if (stack.size() == 0) {
                    printErrorMsg("stack contains no values");
                    return false;
                }

                updateValueAcc(stack.back());
                return handleAwaitingOperator();
            case ':':
                if (stack.size() == 0) {
                    printErrorMsg("stack contains no values");
                    return false;
                }

                updateValueAcc(stack.back());
                stack.pop_back();
                return handleAwaitingOperator();
            case '$':
                stack.push_back(valueAccumulator);
                break;
            case '&':
                return setAwaitingOperator('&');
            case '|':
                if (stack.size() == 0) {
                    printErrorMsg("stack contains no values");
                    return false;
                }

                std::reverse(stack.begin(), stack.end());
                break;
            case '@':    
                std::cin >> line;
                stack.push_back(line);
                break;
            // ignoring/print blocking
            case '~':
                printNext = false;
                break;
            case '[':
                ignoreBlock = true;
                break;
            case ']':
                // ignores are handled above => if we encountered one
                // here, we know that it is out of place
                printErrorMsg("unexpected `]`");
                return false;
            case '\\':
                escapeNext = true;
                break;
            case ',':
                return true;
            // grouping
            case '(':
                if (collectingGroup) {
                    printErrorMsg("unable to create subgroups");
                    return false;
                }

                valueAccumulator = "";
                collectingGroup = true;
                break;
            case ')':
                if (collectingGroup) {
                    collectingGroup = false;

                    if (printNext) {
                        std::cout << valueAccumulator;
                    } else {
                        printNext = true;
                    }
                    return handleAwaitingOperator();
                } else {
                    printErrorMsg("missing group opening");
                    return false;
                }
                break;
            // operators
            case '=':
            case '<':
            case '>':
            case '+':
            case '-':
            case '*':
            case '/':
            case '%':
                if (valueAccumulator == "") {
                    printErrorMsg("operator missing first value");
                    return false;
                }

                auxValueAccumulator = valueAccumulator;
                valueAccumulator = "";
                return setAwaitingOperator(c);
            default:
                updateValueAcc(std::string(1, c));

                if (!collectingGroup) {
                    return handleAwaitingOperator();
                }            
        }
    }

    return true;
}

void Interpreter::updateValueAcc(const std::string& s) {
    if (collectingGroup) {
        valueAccumulator += s;
        return;
    }

    valueAccumulator = s;
    if (printNext) {
        std::cout << valueAccumulator;
    } else {
        printNext = true;
    }
}

bool Interpreter::setAwaitingOperator(char c) {
    if (awaitingOperator != '\0') {
        printErrorMsg("multiple operators awaiting value");
        return false;
    }

    awaitingOperator = c;
    return true;
}

bool Interpreter::handleAwaitingOperator() {
    if (awaitingOperator == '\0') {
        return true;
    }

    if (awaitingOperator == '&') {
        stack.push_back(valueAccumulator);
        awaitingOperator = '\0';
        return true;
    } else if (awaitingOperator == '=') {
        applyCondition(auxValueAccumulator == valueAccumulator);
    } else {
        int a = 0, b = 0;
        if (convertToInt(auxValueAccumulator, a) && convertToInt(valueAccumulator, b)) {
            switch (awaitingOperator) {
                case '+':
                    stack.push_back(std::to_string(a + b));
                    break;
                case '-':
                    stack.push_back(std::to_string(a - b));
                    break;
                case '*':
                    stack.push_back(std::to_string(a * b));
                    break;
                case '/':
                    stack.push_back(std::to_string(a / b));
                    break;
                case '%':
                    stack.push_back(std::to_string(a % b));
                    break;
                case '<':
                    applyCondition(a < b);
                    break;
                case '>':
                    applyCondition(a > b);
                    break;
            }
        } else {
            return false;
        }
    }

    awaitingOperator = '\0';
    auxValueAccumulator = "";
    return true;
}

bool Interpreter::convertToInt(const std::string &s, int &result) {
    try {
        result = std::stoi(s);
        return true;
    } catch (std::exception *e) {
        printErrorMsg(e->what());
        return false;
    }
}

void Interpreter::applyCondition(bool cond) {
    if (!cond) {
        moveY = 1;
        posX = 0;
    }
}

void Interpreter::printErrorMsg(const std::string& msg) {
    std::cout << "error: " << msg << " at (" << posX << ", " << posY << ")\n";
}
