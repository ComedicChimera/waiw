#include <iostream>

#include "interpreter.h"

int main(int argc, char* argv[]) {
    if (argc == 2) {
        Interpreter interpreter;
        interpreter.loadProgram(argv[1]);
        interpreter.runProgram();
    } else {
        std::cout << "error: waiw expects one argument: a filename\n";
    }
}
