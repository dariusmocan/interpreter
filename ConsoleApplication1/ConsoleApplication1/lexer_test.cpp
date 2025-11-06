// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <cassert>
#include <vector>
#include "token.hpp"
#include "lexer.hpp"


// testing if so far our lexer works right by predefining an input and output
// then comparing the predefined results with the ones the function returned
void TestNextToken() {
    std::string input = 
        R"(let five = 5;
        let ten = 10;
        let add = fn(x, y){
            x + y;
        };
        let result = add(five, ten);
    )";
    std::vector<Token> expected = {
        {TokenTypes::LET, "let"},
        {TokenTypes::IDENT, "five"},
        {TokenTypes::ASSIGN, "="},
        {TokenTypes::INT, "5"},
        {TokenTypes::SEMICOLON, ";"},
        {TokenTypes::LET, "let"},
        {TokenTypes::IDENT, "ten"},
        {TokenTypes::ASSIGN, "="},
        {TokenTypes::INT, "10"},
        {TokenTypes::SEMICOLON, ";"},
        {TokenTypes::LET, "let"},
        {TokenTypes::IDENT, "add"},
        {TokenTypes::ASSIGN, "="},
        {TokenTypes::FUNCTION, "fn"},
        {TokenTypes::LPAREN, "("},
        {TokenTypes::IDENT, "x"},
        {TokenTypes::COMMA, ","},
        {TokenTypes::IDENT, "y"},
        {TokenTypes::RPAREN, ")"},
        {TokenTypes::LBRACE, "{"},
        {TokenTypes::IDENT, "x"},
        {TokenTypes::PLUS, "+"},
        {TokenTypes::IDENT, "y"},
        {TokenTypes::SEMICOLON, ";"},
        {TokenTypes::RBRACE, "}"},
        {TokenTypes::SEMICOLON, ";"},
        {TokenTypes::LET, "let"},
        {TokenTypes::IDENT, "result"},
        {TokenTypes::ASSIGN, "="},
        {TokenTypes::IDENT, "add"},
        {TokenTypes::LPAREN, "("},
        {TokenTypes::IDENT, "five"},
        {TokenTypes::COMMA, ","},
        {TokenTypes::IDENT, "ten"},
        {TokenTypes::RPAREN, ")"},
        {TokenTypes::SEMICOLON, ";"},
        {TokenTypes::EOF_, ""},
    };

    Lexer l(input);

    for (size_t i = 0; i < expected.size(); i++) {
        Token tok = l.nextToken();

        std::cout << "tok.type: '" << tok.type
            << "', expected.type: '" << expected[i].type << "'\n";
        std::cout << "tok.Literal: '" << tok.literal
            << "', expected.Literal: '" << expected[i].literal << "'\n";

        assert(tok.type == expected[i].type);
        assert(tok.literal == expected[i].literal);
    }

    std::cout << "All token tests passed!\n";
};

int main()
{
    TestNextToken();

}

