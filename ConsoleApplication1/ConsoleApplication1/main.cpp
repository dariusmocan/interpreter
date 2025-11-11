#include <iostream>
#include <string>
#include "token.hpp"
#include "lexer.hpp"
#include "repl.hpp"

int main() {

    // Greeting
    std::cout << "Hello " << "username"
        << "! This is the MIL programming language!" << std::endl;
    std::cout << "Feel free to type in commands" << std::endl;

    Start(std::cin, std::cout);

    return 0;
}
