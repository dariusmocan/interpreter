#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <memory>
#include "lexer.hpp"
#include "ast.hpp"
#include "parser.hpp"


static void checkParserErrors(Parser& p) {
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

static bool testLetStatement(std::unique_ptr<Statement>& s, const std::string& name) {
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

static void TestLetStatements() {
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

static void TestReturnStatements() {
    std::string input = R"(
    return 5;
    return 10;
    return 993322;
    )";

    auto l = std::make_unique<Lexer>(input);
    Parser p(l);

    std::unique_ptr<Program> program = p.parseProgram();
    checkParserErrors(p);

    if (program->statements.size() != 3) {
        std::cerr << "program.statements does not contain 3 statements. got="
            << program->statements.size() << "\n";
        return;
    }

    for (size_t i = 0; i < program->statements.size(); i++) {
        ReturnStatement* returnStmt = dynamic_cast<ReturnStatement*>(program->statements[i].get());
        if (!returnStmt) {
            std::cerr << "stmt not ReturnStatement. got=nullptr\n";
            continue;
        }

        if (returnStmt->tokenLiteral() != "return") {
            std::cerr << "returnStmt.tokenLiteral not 'return', got "
                << returnStmt->tokenLiteral() << "\n";
        }
    }

    std::cout << "All return statement tests passed!\n";
}

static void TestIdentifierExpression() {
    std::string input = "foobar;";
    
    auto l = std::make_unique<Lexer>(input);
    Parser p(l);
    
    std::unique_ptr<Program> program = p.parseProgram();
    checkParserErrors(p);
    
    if (program->statements.size() != 1) {
        std::cerr << "program has not enough statements. got="
            << program->statements.size() << "\n";
        return;
    }
    
    ExpressionStatement* stmt = dynamic_cast<ExpressionStatement*>(program->statements[0].get());
    if (!stmt) {
        std::cerr << "program.statements[0] is not ExpressionStatement. got=nullptr\n";
        return;
    }
    
    Identifier* ident = dynamic_cast<Identifier*>(stmt->value.get());
    if (!ident) {
        std::cerr << "exp not Identifier. got=nullptr\n";
        return;
    }
    
    if (ident->value != "foobar") {
        std::cerr << "ident.value not foobar. got=" << ident->value << "\n";
        return;
    }
    
    if (ident->tokenLiteral() != "foobar") {
        std::cerr << "ident.tokenLiteral not foobar. got="
            << ident->tokenLiteral() << "\n";
        return;
    }
    
    std::cout << "TestIdentifierExpression passed!\n";
}

static void TestIntegerLiteralExpression() {
    std::string input = "5;";
    auto l = std::make_unique<Lexer>(input);
    Parser p(l);
    std::unique_ptr<Program> program = p.parseProgram();
    checkParserErrors(p);

    if (program->statements.size() != 1) {
        std::cerr << "Program has not enough statements. got = " << program->statements.size() << "\n";
        return;
    }

    ExpressionStatement* stmt = dynamic_cast<ExpressionStatement*>(program->statements[0].get());

    if (!stmt) {
        std::cerr << "program[0] is not an expression statement. got nullptr\n";
    }


    IntegerLiteral* literal = dynamic_cast<IntegerLiteral*>(stmt->value.get());

    if (!literal) {
        std::cerr << "exp not IntegerLiteral. got = " << literal->value;
        return;
    }

    if (literal->value != 5) {
        std::cerr << "ident.value not 5. got=" << literal->value << "\n";
        return;
    }

    if (literal->tokenLiteral() != "5") {
        std::cerr << "ident.tokenLiteral not 5. got="
            << literal->tokenLiteral() << "\n";
        return;
    }

    std::cout << "TestIntegerLiteral passed!\n";

}

struct PrefixTest {
    std::string input;
    std::string op;
    int64_t integerValue;
};

static bool testIntegerLiteral(Expression* exp, int64_t value) {
    IntegerLiteral* intLit = dynamic_cast<IntegerLiteral*>(exp);
    if (!intLit) {
        std::cerr << "exp not IntegerLiteral. got=nullptr\n";
        return false;
    }

    if (intLit->value != value) {
        std::cerr << "intLit.value not " << value << ". got=" << intLit->value << "\n";
        return false;
    }

    if (intLit->tokenLiteral() != std::to_string(value)) {
        std::cerr << "intLit.tokenLiteral not " << value << ". got="
            << intLit->tokenLiteral() << "\n";
        return false;
    }

    return true;
}

static void TestParsingPrefixExpressions() {
    std::vector<PrefixTest> prefixTests = {
        {"!5;", "!", 5},
        {"-15;", "-", 15}
    };

    for (const auto& tt : prefixTests) {
        auto l = std::make_unique<Lexer>(tt.input);
        Parser p(l);
        std::unique_ptr<Program> program = p.parseProgram();
        checkParserErrors(p);

        if (program->statements.size() != 1) {
            std::cerr << "program.statements does not contain 1 statement. got="
                << program->statements.size() << "\n";
            return;
        }

        ExpressionStatement* stmt = dynamic_cast<ExpressionStatement*>(
            program->statements[0].get());
        if (!stmt) {
            std::cerr << "program.statements[0] is not ExpressionStatement\n";
            return;
        }

        PrefixExpression* exp = dynamic_cast<PrefixExpression*>(stmt->value.get());
        if (!exp) {
            std::cerr << "stmt is not PrefixExpression\n";
            return;
        }

        if (exp->oper != tt.op) {
            std::cerr << "exp.Operator is not '" << tt.op << "'. got="
                << exp->oper << "\n";
            return;
        }

        if (!testIntegerLiteral(exp->right.get(), tt.integerValue)) {
            return;
        }
    }

    std::cout << "TestParsingPrefixExpressions passed!\n";
}

struct InfixTest {
    std::string input;
    int64_t leftValue;
    std::string op;
    int64_t rightValue;
};

static void TestParsingInfixExpressions() {
    std::vector<InfixTest> infixTests = {
        {"5 + 5;", 5, "+", 5},
        {"5 - 5;", 5, "-", 5},
        {"5 * 5;", 5, "*", 5},
        {"5 / 5;", 5, "/", 5},
        {"5 > 5;", 5, ">", 5},
        {"5 < 5;", 5, "<", 5},
        {"5 == 5;", 5, "==", 5},
        {"5 != 5;", 5, "!=", 5}
    };

    for (const auto& tt : infixTests) {
        auto l = std::make_unique<Lexer>(tt.input);
        Parser p(l);
        std::unique_ptr<Program> program = p.parseProgram();
        checkParserErrors(p);

        if (program->statements.size() != 1) {
            std::cerr << "program.statements does not contain 1 statement. got="
                << program->statements.size() << "\n";
            return;
        }

        ExpressionStatement* stmt = dynamic_cast<ExpressionStatement*>(
            program->statements[0].get());
        if (!stmt) {
            std::cerr << "program.statements[0] is not ExpressionStatement\n";
            return;
        }

        InfixExpression* exp = dynamic_cast<InfixExpression*>(stmt->value.get());
        if (!exp) {
            std::cerr << "exp is not InfixExpression\n";
            return;
        }

        if (!testIntegerLiteral(exp->left.get(), tt.leftValue)) {
            return;
        }

        if (exp->oper != tt.op) {
            std::cerr << "exp.Operator is not '" << tt.op << "'. got="
                << exp->oper << "\n";
            return;
        }

        if (!testIntegerLiteral(exp->right.get(), tt.rightValue)) {
            return;
        }
    }

    std::cout << "TestParsingInfixExpressions passed!\n";
}

int main() {

    std::cout << "=== DEBUG: Tokenizing '5 + 5;' ===\n";
    auto l_test = std::make_unique<Lexer>("5 + 5;");
    for (int i = 0; i < 5; i++) {
        Token t = l_test->nextToken();
        std::cout << "Token: type='" << t.type << "', literal='" << t.literal << "'\n";
    }
    std::cout << "=== END DEBUG ===\n\n";


    //TestLetStatements();
    //TestReturnStatements();
    //TestIdentifierExpression();
    //TestIntegerLiteralExpression();
    //TestParsingPrefixExpressions();
    TestParsingInfixExpressions();

    return 0;
}