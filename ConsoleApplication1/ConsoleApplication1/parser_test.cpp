#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <memory>
#include "lexer.hpp"
#include "ast.hpp"
#include "parser.hpp"

// ====== HELPER FUNCTIONS - MUST BE AT THE TOP ======

// Error reporting
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

// Test integer literal
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

// Test identifier
static bool testIdentifier(Expression* exp, const std::string& value) {
    Identifier* ident = dynamic_cast<Identifier*>(exp);
    if (!ident) {
        std::cerr << "exp not Identifier. got=nullptr\n";
        return false;
    }
    
    if (ident->value != value) {
        std::cerr << "ident.value not " << value << ". got=" << ident->value << "\n";
        return false;
    }
    
    if (ident->tokenLiteral() != value) {
        std::cerr << "ident.tokenLiteral not " << value << ". got=" 
                  << ident->tokenLiteral() << "\n";
        return false;
    }
    
    return true;
}

// Test boolean literal - ADD THIS AFTER testIdentifier() FUNCTION
static bool testBooleanLiteral(Expression* exp, bool expected) {
    Boolean* boolean = dynamic_cast<Boolean*>(exp);
    if (!boolean) {
        std::cerr << "exp not Boolean. got=nullptr\n";
        return false;
    }
    
    if (boolean->value != expected) {
        std::cerr << "boolean.value not " << (expected ? "true" : "false") 
                  << ". got=" << (boolean->value ? "true" : "false") << "\n";
        return false;
    }
    
    std::string expectedLiteral = expected ? "true" : "false";
    if (boolean->tokenLiteral() != expectedLiteral) {
        std::cerr << "boolean.tokenLiteral not " << expectedLiteral << ". got=" 
                  << boolean->tokenLiteral() << "\n";
        return false;
    }
    
    return true;
}

// Generic literal expression tester - overloaded versions
static bool testLiteralExpression(Expression* exp, int64_t expected) {
    return testIntegerLiteral(exp, expected);
}

static bool testLiteralExpression(Expression* exp, int expected) {
    return testIntegerLiteral(exp, static_cast<int64_t>(expected));
}

static bool testLiteralExpression(Expression* exp, const std::string& expected) {
    return testIdentifier(exp, expected);
}

static bool testLiteralExpression(Expression* exp, const char* expected) {
    return testIdentifier(exp, std::string(expected));
}

// ADD THIS OVERLOAD to testLiteralExpression AFTER THE EXISTING OVERLOADS
static bool testLiteralExpression(Expression* exp, bool expected) {
    return testBooleanLiteral(exp, expected);
}

// Generic infix expression tester - TEMPLATE MUST BE DECLARED BEFORE USE
template<typename LeftT, typename RightT>
static bool testInfixExpression(Expression* exp, LeftT left, 
                               const std::string& op, RightT right) {
    InfixExpression* opExp = dynamic_cast<InfixExpression*>(exp);
    if (!opExp) {
        std::cerr << "exp is not InfixExpression. got=nullptr\n";
        return false;
    }
    
    if (!testLiteralExpression(opExp->left.get(), left)) {
        return false;
    }
    
    if (opExp->oper != op) {
        std::cerr << "exp.Operator is not '" << op << "'. got='" 
                  << opExp->oper << "'\n";
        return false;
    }
    
    if (!testLiteralExpression(opExp->right.get(), right)) {
        return false;
    }
    
    return true;
}

// ====== TEST FUNCTIONS START HERE ======

// Test let statement
static bool testLetStatement(std::unique_ptr<Statement>& s, const std::string& name) {
    if (s->tokenLiteral() != "let") {
        std::cerr << "s.TokenLiteral not 'let'. got=" << s->tokenLiteral() << "\n";
        return false;
    }

    LetStatement* letStmt = dynamic_cast<LetStatement*>(s.get());
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
    ))";

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

struct PrefixTestInt {
    std::string input;
    std::string op;
    int64_t integerValue;
};

struct PrefixTestBool {
    std::string input;
    std::string op;
    bool boolValue;
};

static void TestParsingPrefixExpressions() {
    // Integer prefix tests
    std::vector<PrefixTestInt> intPrefixTests = {
        {"!5;", "!", 5},
        {"-15;", "-", 15}
    };

    for (const auto& tt : intPrefixTests) {
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

    // Boolean prefix tests - ADD THESE
    std::vector<PrefixTestBool> boolPrefixTests = {
        {"!true;", "!", true},
        {"!false;", "!", false}
    };

    for (const auto& tt : boolPrefixTests) {
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

        if (!testBooleanLiteral(exp->right.get(), tt.boolValue)) {
            return;
        }
    }

    std::cout << "TestParsingPrefixExpressions passed!\n";
}

struct InfixTestInt {
    std::string input;
    int64_t leftValue;
    std::string op;
    int64_t rightValue;
};

struct InfixTestBool {
    std::string input;
    bool leftValue;
    std::string op;
    bool rightValue;
};

static void TestParsingInfixExpressions() {
    // Integer infix tests
    std::vector<InfixTestInt> intInfixTests = {
        {"5 + 5;", 5, "+", 5},
        {"5 - 5;", 5, "-", 5},
        {"5 * 5;", 5, "*", 5},
        {"5 / 5;", 5, "/", 5},
        {"5 > 5;", 5, ">", 5},
        {"5 < 5;", 5, "<", 5},
        {"5 == 5;", 5, "==", 5},
        {"5 != 5;", 5, "!=", 5}
    };

    for (const auto& tt : intInfixTests) {
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

        if (!testInfixExpression(stmt->value.get(), tt.leftValue, tt.op, tt.rightValue)) {
            return;
        }
    }

    // Boolean infix tests - ADD THESE
    std::vector<InfixTestBool> boolInfixTests = {
        {"true == true", true, "==", true},
        {"true != false", true, "!=", false},
        {"false == false", false, "==", false}
    };

    for (const auto& tt : boolInfixTests) {
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

        if (!testInfixExpression(stmt->value.get(), tt.leftValue, tt.op, tt.rightValue)) {
            return;
        }
    }

    std::cout << "TestParsingInfixExpressions passed!\n";
}

// Add this test - it uses the new helper functions to test mixed integer/identifier expressions
static void TestParsingInfixExpressionsWithIdentifiers() {
    struct Test {
        std::string input;
        std::string left;
        std::string op;
        std::string right;
    };
    
    std::vector<Test> tests = {
        {"a + b;", "a", "+", "b"},
        {"a - b;", "a", "-", "b"},
        {"a * b;", "a", "*", "b"},
        {"a / b;", "a", "/", "b"},
        {"a > b;", "a", ">", "b"},
        {"a < b;", "a", "<", "b"},
        {"a == b;", "a", "==", "b"},
        {"a != b;", "a", "!=", "b"}
    };
    
    for (const auto& tt : tests) {
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
        
        // Use the helper with identifiers instead of integers
        if (!testInfixExpression(stmt->value.get(), tt.left, tt.op, tt.right)) {
            return;
        }
    }
    
    std::cout << "TestParsingInfixExpressionsWithIdentifiers passed!\n";
}

// Add this test - demonstrates mixed types (integer and identifier)
static void TestParsingMixedInfixExpressions() {
    auto l = std::make_unique<Lexer>("5 + a;");
    Parser p(l);
    std::unique_ptr<Program> program = p.parseProgram();
    checkParserErrors(p);
    
    if (program->statements.size() != 1) {
        std::cerr << "program.statements does not contain 1 statement\n";
        return;
    }
    
    ExpressionStatement* stmt = dynamic_cast<ExpressionStatement*>(
        program->statements[0].get());
    if (!stmt) {
        std::cerr << "program.statements[0] is not ExpressionStatement\n";
        return;
    }
    
    // Mixed types: integer and identifier
    if (!testInfixExpression(stmt->value.get(), 5, "+", "a")) {
        return;
    }
    
    std::cout << "TestParsingMixedInfixExpressions passed!\n";
}

struct OperatorPrecedenceTest {
    std::string input;
    std::string expected;
};

static void TestOperatorPrecedenceParsing() {
    std::vector<OperatorPrecedenceTest> tests = {
        {"-a * b", "((-a) * b)"},
        {"!-a", "(!(-a))"},
        {"a + b + c", "((a + b) + c)"},
        {"a + b - c", "((a + b) - c)"},
        {"a * b * c", "((a * b) * c)"},
        {"a * b / c", "((a * b) / c)"},
        {"a + b / c", "(a + (b / c))"},
        {"a + b * c + d / e - f", "(((a + (b * c)) + (d / e)) - f)"},
        {"3 + 4; -5 * 5", "(3 + 4)((-5) * 5)"},
        {"5 > 4 == 3 < 4", "((5 > 4) == (3 < 4))"},
        {"5 < 4 != 3 > 4", "((5 < 4) != (3 > 4))"},
        {"3 + 4 * 5 == 3 * 1 + 4 * 5", "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))"},
        {"true", "true"},
        {"false", "false"},
        {"3 > 5 == false", "((3 > 5) == false)"},
        {"3 < 5 == true", "((3 < 5) == true)"},
        // ADD THESE GROUPED EXPRESSION TESTS:
        {"1 + (2 + 3) + 4", "((1 + (2 + 3)) + 4)"},
        {"(5 + 5) * 2", "((5 + 5) * 2)"},
        {"2 / (5 + 5)", "(2 / (5 + 5))"},
        {"-(5 + 5)", "(-(5 + 5))"},
        {"!(true == true)", "(!(true == true))"}
    };

    for (const auto& tt : tests) {
        auto l = std::make_unique<Lexer>(tt.input);
        Parser p(l);
        std::unique_ptr<Program> program = p.parseProgram();
        checkParserErrors(p);

        std::string actual = program->string();
        if (actual != tt.expected) {
            std::cerr << "expected='" << tt.expected << "', got='" << actual << "'\n";
            return;
        }
    }

    std::cout << "TestOperatorPrecedenceParsing passed!\n";
}

static void TestBooleanExpression() {
    std::string input = "true;";
    
    auto l = std::make_unique<Lexer>(input);
    Parser p(l);
    std::unique_ptr<Program> program = p.parseProgram();
    checkParserErrors(p);
    
    if (program->statements.size() != 1) {
        std::cerr << "program has not enough statements. got="
            << program->statements.size() << "\n";
        return;
    }
    
    ExpressionStatement* stmt = dynamic_cast<ExpressionStatement*>(
        program->statements[0].get());
    if (!stmt) {
        std::cerr << "program.statements[0] is not ExpressionStatement\n";
        return;
    }
    
    // Test using the helper
    if (!testBooleanLiteral(stmt->value.get(), true)) {
        return;
    }
    
    std::cout << "TestBooleanExpression passed!\n";
}

static void TestBooleanInfixExpressions() {
    struct Test {
        std::string input;
        bool leftValue;
        std::string op;
        bool rightValue;
    };
    
    std::vector<Test> tests = {
        {"true == true;", true, "==", true},
        {"true != false;", true, "!=", false},
        {"false == false;", false, "==", false}
    };
    
    for (const auto& tt : tests) {
        auto l = std::make_unique<Lexer>(tt.input);
        Parser p(l);
        std::unique_ptr<Program> program = p.parseProgram();
        checkParserErrors(p);
        
        if (program->statements.size() != 1) {
            std::cerr << "program.statements does not contain 1 statement\n";
            return;
        }
        
        ExpressionStatement* stmt = dynamic_cast<ExpressionStatement*>(
            program->statements[0].get());
        if (!stmt) {
            std::cerr << "program.statements[0] is not ExpressionStatement\n";
            return;
        }
        
        // Use the generic template helper!
        if (!testInfixExpression(stmt->value.get(), tt.leftValue, tt.op, tt.rightValue)) {
            return;
        }
    }
    
    std::cout << "TestBooleanInfixExpressions passed!\n";
}

// Add this test - demonstrates mixed boolean expressions with integers and identifiers
static void TestMixedBooleanExpressions() {
    struct Test {
        std::string input;
        std::string expected;
    };
    
    std::vector<Test> tests = {
        {"3 > 5 == false", "((3 > 5) == false)"},
        {"3 < 5 == true", "((3 < 5) == true)"},
        {"1 + 2 == 3", "((1 + 2) == 3)"}
    };
    
    for (const auto& tt : tests) {
        auto l = std::make_unique<Lexer>(tt.input);
        Parser p(l);
        std::unique_ptr<Program> program = p.parseProgram();
        checkParserErrors(p);
        
        std::string actual = program->string();
        if (actual != tt.expected) {
            std::cerr << "expected='" << tt.expected << "', got='" << actual << "'\n";
            return;
        }
    }
    
    std::cout << "TestMixedBooleanExpressions passed!\n";
}

static void TestBooleanPrefixExpressions() {
    struct Test {
        std::string input;
        std::string op;
        bool boolValue;
    };
    
    std::vector<Test> tests = {
        {"!true;", "!", true},
        {"!false;", "!", false}
    };
    
    for (const auto& tt : tests) {
        auto l = std::make_unique<Lexer>(tt.input);
        Parser p(l);
        std::unique_ptr<Program> program = p.parseProgram();
        checkParserErrors(p);
        
        if (program->statements.size() != 1) {
            std::cerr << "program.statements does not contain 1 statement\n";
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
            std::cerr << "exp.Operator is not '" << tt.op << "'\n";
            return;
        }
        
        // Test the boolean right side
        if (!testBooleanLiteral(exp->right.get(), tt.boolValue)) {
            return;
        }
    }
    
    std::cout << "TestBooleanPrefixExpressions passed!\n";
}

static void TestIfExpression() {
    std::string input = "if (x < y) { x }";
    
    auto l = std::make_unique<Lexer>(input);
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
    
    IfExpression* exp = dynamic_cast<IfExpression*>(stmt->value.get());
    if (!exp) {
        std::cerr << "stmt.Expression is not IfExpression\n";
        return;
    }
    
    if (!testInfixExpression(exp->condition.get(), "x", "<", "y")) {
        return;
    }
    
    if (exp->consequence->statements.size() != 1) {
        std::cerr << "consequence is not 1 statement. got="
            << exp->consequence->statements.size() << "\n";
        return;
    }
    
    ExpressionStatement* consequence = dynamic_cast<ExpressionStatement*>(
        exp->consequence->statements[0].get());
    if (!consequence) {
        std::cerr << "consequence.statements[0] is not ExpressionStatement\n";
        return;
    }
    
    if (!testIdentifier(consequence->value.get(), "x")) {
        return;
    }
    
    if (exp->alternative != nullptr) {
        std::cerr << "exp.alternative was not nullptr. got=" 
            << exp->alternative->string() << "\n";
        return;
    }
    
    std::cout << "TestIfExpression passed!\n";
}

static void TestIfElseExpression() {
    std::string input = "if (x < y) { x } else { y }";
    
    auto l = std::make_unique<Lexer>(input);
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
    
    IfExpression* exp = dynamic_cast<IfExpression*>(stmt->value.get());
    if (!exp) {
        std::cerr << "stmt.Expression is not IfExpression\n";
        return;
    }
    
    if (!testInfixExpression(exp->condition.get(), "x", "<", "y")) {
        return;
    }
    
    if (exp->consequence->statements.size() != 1) {
        std::cerr << "consequence is not 1 statement. got="
            << exp->consequence->statements.size() << "\n";
        return;
    }
    
    ExpressionStatement* consequence = dynamic_cast<ExpressionStatement*>(
        exp->consequence->statements[0].get());
    if (!consequence) {
        std::cerr << "consequence.statements[0] is not ExpressionStatement\n";
        return;
    }
    
    if (!testIdentifier(consequence->value.get(), "x")) {
        return;
    }
    
    // Test the alternative (else clause)
    if (exp->alternative == nullptr) {
        std::cerr << "exp.alternative was nullptr\n";
        return;
    }
    
    if (exp->alternative->statements.size() != 1) {
        std::cerr << "alternative is not 1 statement. got="
            << exp->alternative->statements.size() << "\n";
        return;
    }
    
    ExpressionStatement* alternative = dynamic_cast<ExpressionStatement*>(
        exp->alternative->statements[0].get());
    if (!alternative) {
        std::cerr << "alternative.statements[0] is not ExpressionStatement\n";
        return;
    }
    
    if (!testIdentifier(alternative->value.get(), "y")) {
        return;
    }
    
    std::cout << "TestIfElseExpression passed!\n";
}

int main() {
    TestIdentifierExpression();
    TestIntegerLiteralExpression();
    TestBooleanExpression();
    TestParsingPrefixExpressions();
    TestBooleanPrefixExpressions();
    TestParsingInfixExpressions();
    TestParsingInfixExpressionsWithIdentifiers();
    TestBooleanInfixExpressions();
    TestMixedBooleanExpressions();
    TestParsingMixedInfixExpressions();
    TestOperatorPrecedenceParsing();
    TestIfExpression();
    TestIfElseExpression();

    std::cout << "\n=== All tests passed! ===\n";
    
    return 0;
}