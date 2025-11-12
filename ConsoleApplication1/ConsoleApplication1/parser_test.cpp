#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <memory>
#include "lexer.hpp"
#include "ast.hpp"
#include "parser.hpp"


void checkParserErrors(Parser& p) {
    const std::vector<std::string>& errors = p.return_errors();

    if (errors.empty()) {
        return;
    }

    std::cerr << "parser has " << errors.size() << " errors\n";

    for (const auto& msg : errors) {
        std::cerr << "parser error: \"" << msg << "\"\n";
    }

    std::exit(1);  
}

bool testLetStatement(std::unique_ptr<Statement>& s, const std::string& name) {
    if (s->tokenLiteral() != "let") {
        std::cerr << "s.TokenLiteral not 'let'. got=" << s->tokenLiteral() << "\n";
        return false;
    }

    LetStatement* letStmt = dynamic_cast<LetStatement*>(s.get());  // Add .get() here
    if (!letStmt) {
        std::cerr << "s not LetStatement. got=nullptr\n";
        return false;
    }

    if (letStmt->name->value != name) {
        std::cerr << "letStmt.name.value not '" << name << "'. got=" << letStmt->name->value << "\n";
        return false;
    }

    if (letStmt->name->tokenLiteral() != name) {
        std::cerr << "letStmt.name.tokenLiteral() not '" << name << "'. got=" << letStmt->name->tokenLiteral() << "\n";
        return false;
    }

    return true;
}

void TestLetStatements() {
    std::string input = R"(let x 5;
    let = 10;
    let 838383;)";

    auto l = std::make_unique<Lexer>(input);
    Parser p(l);

    std::unique_ptr<Program> program = p.parseProgram();
    checkParserErrors(p);

    if (program == nullptr) {
        std::cerr << "ParseProgram() returned nullptr\n";
        return;
    }

    if (program->statements.size() != 3) {
        std::cerr << "program.statements does not contain 3 statements. got="
            << program->statements.size() << "\n";
        return;
    }

    std::vector<std::string> expectedIdentifiers = { "x", "y", "foobar" };

    for (size_t i = 0; i < expectedIdentifiers.size(); i++) {
        if (!testLetStatement(program -> statements[i], expectedIdentifiers[i])) {
            return;
        }
    }

    std::cout << "All let statement tests passed!\n";
}



int main() {
    TestLetStatements();
    return 0;
}