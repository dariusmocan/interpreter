#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include "token.hpp"

// main class for our Lexer
class Lexer
{
public:
    // Constructor, explicit = not permiting convertions; 
    // must get an 'input', all the rest are initialising on their own
    // readChar - advances all atributes. position - first index. ch - first letter. readPosition - 2nd index 
    explicit Lexer(const std::string& input) : input(input), position(0), readPosition(0), ch(0) {
        readChar();
    }

    // function that returns current token, and reads the next one
    Token nextToken();

private:
    std::string input; // text representing the code
    size_t position; // current position
    size_t readPosition; // next position
    char ch; // curent character


    std::string readIdentifier(); // returning the identifier
    std::string readNumber(); // returning the number
    char peekChar(); // looks at the char ahead of the current position
    void readChar(); // asigning correct values for all the atributes while parsing the whole input
    void skipWhiteSpaces(); // skipping all white space


};

#endif // !1