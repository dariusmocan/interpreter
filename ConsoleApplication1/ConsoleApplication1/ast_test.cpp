#include <iostream>
#include <string>
#include <memory>
#include "ast.hpp"
#include "token.hpp"

void TestString() {
    auto program = std::make_unique<Program>();
    
    // Create the identifier for the name
    auto nameIdent = std::make_unique<Identifier>(
        Token{TokenTypes::IDENT, "myVar"},
        "myVar"
    );
    
    // Create the identifier for the value
    auto valueIdent = std::make_unique<Identifier>(
        Token{TokenTypes::IDENT, "anotherVar"},
        "anotherVar"
    );
    
    // Create the let statement
    auto letStmt = std::make_unique<LetStatement>(
        Token{TokenTypes::LET, "let"}
    );
    letStmt->name = std::move(nameIdent);
    letStmt->value = std::move(valueIdent);
    
    // Add statement to program
    program->statements.push_back(std::move(letStmt));
    
    // Test the string representation
    std::string expected = "let myVar = anotherVar;";
    std::string actual = program->string();
    
    if (actual != expected) {
        std::cerr << "program.string() wrong. got=\"" << actual << "\"\n";
        std::cerr << "expected=\"" << expected << "\"\n";
        return;
    }
    
    std::cout << "TestString passed!\n";
}

//int main() {
//    TestString();
//    return 0;
//}

