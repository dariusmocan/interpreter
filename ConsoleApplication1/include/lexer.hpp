#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include "token.hpp"

class Lexer
{
public:
    explicit Lexer(const std::string& input) : input(input), position(0), readPosition(0), ch(0) {
        readChar();
    }

    Token NextToken();

private:
    std::string input;
    size_t position;
    size_t readPosition;
    char ch;

    void readChar();
};

#endif // !1