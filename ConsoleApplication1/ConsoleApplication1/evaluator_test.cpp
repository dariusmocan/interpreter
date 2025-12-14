#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include "lexer.hpp"
#include "parser.hpp"
#include "ast.hpp"
#include "object.hpp"
#include "evaluator.hpp"

// ====== HELPER FUNCTIONS ======

// Helper to run the full pipeline: lex -> parse -> eval
static std::unique_ptr<Object> testEval(const std::string& input) {
    auto l = std::make_unique<Lexer>(input);
    Parser p(l);
    std::unique_ptr<Program> program = p.parseProgram();
    auto env = std::make_shared<Environment>();  // CREATE NEW ENVIRONMENT
    
    return eval(program.get(), env);  // PASS ENVIRONMENT
}

// Test helper for Integer objects
static bool testIntegerObject(Object* obj, int64_t expected) {
    Integer* result = dynamic_cast<Integer*>(obj);
    if (!result) {
        std::cerr << "object is not Integer. got=" << (obj ? obj->Type() : "nullptr") << "\n";
        return false;
    }
    
    if (result->value != expected) {
        std::cerr << "object has wrong value. got=" << result->value 
                  << ", want=" << expected << "\n";
        return false;
    }
    
    return true;
}

// Test helper for Boolean objects
static bool testBooleanObject(Object* obj, bool expected) {
    Boolean* result = dynamic_cast<Boolean*>(obj);
    if (!result) {
        std::cerr << "object is not Boolean. got=" << (obj ? obj->Type() : "nullptr") << "\n";
        return false;
    }
    
    if (result->value != expected) {
        std::cerr << "object has wrong value. got=" << (result->value ? "true" : "false")
                  << ", want=" << (expected ? "true" : "false") << "\n";
        return false;
    }
    
    return true;
}

// Test helper for NULL objects
static bool testNullObject(Object* obj) {
    if (obj != nullptr) {
        std::cerr << "object is not NULL. got=" << obj->Type() 
                  << " (" << obj->Inspect() << ")\n";
        return false;
    }
    return true;
}

// Test helper for Error objects
static bool testErrorObject(Object* obj, const std::string& expectedMessage) {
    Error* errObj = dynamic_cast<Error*>(obj);
    if (!errObj) {
        std::cerr << "object is not Error. got=" << (obj ? obj->Type() : "nullptr") << "\n";
        return false;
    }
    
    if (errObj->message != expectedMessage) {
        std::cerr << "wrong error message. expected=\"" << expectedMessage 
                  << "\", got=\"" << errObj->message << "\"\n";
        return false;
    }
    
    return true;
}

// ====== TEST FUNCTIONS ======

static void TestEvalIntegerExpression() {
    struct Test {
        std::string input;
        int64_t expected;
    };
    
    std::vector<Test> tests = {
        {"5", 5},
        {"10", 10},
        {"-5", -5},
        {"-10", -10},
        {"5 + 5 + 5 + 5 - 10", 10},
        {"2 * 2 * 2 * 2 * 2", 32},
        {"-50 + 100 + -50", 0},
        {"5 * 2 + 10", 20},
        {"5 + 2 * 10", 25},
        {"20 + 2 * -10", 0},
        {"50 / 2 * 2 + 10", 60},
        {"2 * (5 + 10)", 30},
        {"3 * 3 * 3 + 10", 37},
        {"3 * (3 * 3) + 10", 37},
        {"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50},
    };
    
    for (const auto& tt : tests) {
        std::unique_ptr<Object> evaluated = testEval(tt.input);
        if (!testIntegerObject(evaluated.get(), tt.expected)) {
            return;
        }
    }
    
    std::cout << "TestEvalIntegerExpression passed!\n";
}

static void TestEvalBooleanExpression() {
    struct Test {
        std::string input;
        bool expected;
    };
    
    std::vector<Test> tests = {
        {"true", true},
        {"false", false},
        {"1 < 2", true},
        {"1 > 2", false},
        {"1 < 1", false},
        {"1 > 1", false},
        {"1 == 1", true},
        {"1 != 1", false},
        {"1 == 2", false},
        {"1 != 2", true},
        {"true == true", true},
        {"false == false", true},
        {"true == false", false},
        {"true != false", true},
        {"false != true", true},
        {"(1 < 2) == true", true},
        {"(1 < 2) == false", false},
        {"(1 > 2) == true", false},
        {"(1 > 2) == false", true}
    };
    
    for (const auto& tt : tests) {
        std::unique_ptr<Object> evaluated = testEval(tt.input);
        if (!testBooleanObject(evaluated.get(), tt.expected)) {
            return;
        }
    }
    
    std::cout << "TestEvalBooleanExpression passed!\n";
}

static void TestBangOperator() {
    struct Test {
        std::string input;
        bool expected;
    };
    
    std::vector<Test> tests = {
        {"!true", false},
        {"!false", true},
        {"!5", false},
        {"!!true", true},
        {"!!false", false},
        {"!!5", true}
    };
    
    for (const auto& tt : tests) {
        std::unique_ptr<Object> evaluated = testEval(tt.input);
        if (!testBooleanObject(evaluated.get(), tt.expected)) {
            return;
        }
    }
    
    std::cout << "TestBangOperator passed!\n";
}

static void TestIfElseExpressions() {
    struct Test {
        std::string input;
        bool expectNull;      // true if expecting null, false if expecting integer
        int64_t expectedInt;  // only used if expectNull is false
    };
    
    std::vector<Test> tests = {
        {"if (true) { 10 }", false, 10},
        {"if (false) { 10 }", true, 0},
        {"if (1) { 10 }", false, 10},
        {"if (1 < 2) { 10 }", false, 10},
        {"if (1 > 2) { 10 }", true, 0},
        {"if (1 > 2) { 10 } else { 20 }", false, 20},
        {"if (1 < 2) { 10 } else { 20 }", false, 10}
    };
    
    for (const auto& tt : tests) {
        std::unique_ptr<Object> evaluated = testEval(tt.input);
        
        if (tt.expectNull) {
            if (!testNullObject(evaluated.get())) {
                return;
            }
        } else {
            if (!testIntegerObject(evaluated.get(), tt.expectedInt)) {
                return;
            }
        }
    }
    
    std::cout << "TestIfElseExpressions passed!\n";
}

static void TestReturnStatements() {
    struct Test {
        std::string input;
        int64_t expected;
    };
    
    std::vector<Test> tests = {
        {"return 10;", 10},
        {"return 10; 9;", 10},
        {"return 2 * 5; 9;", 10},
        {"9; return 2 * 5; 9;", 10},
        {R"(
        if (10 > 1) {
            if (10 > 1) {
                return 10;
            }
            return 1;
        })", 10}
    };
    
    for (const auto& tt : tests) {
        std::unique_ptr<Object> evaluated = testEval(tt.input);
        if (!testIntegerObject(evaluated.get(), tt.expected)) {
            return;
        }
    }
    
    std::cout << "TestReturnStatements passed!\n";
}

static void TestErrorHandling() {
    struct Test {
        std::string input;
        std::string expectedMessage;
    };
    
    std::vector<Test> tests = {
        {"5 + true;", "type mismatch: INTEGER + BOOLEAN"},
        {"5 + true; 5;", "type mismatch: INTEGER + BOOLEAN"},
        {"-true", "unknown operator: -BOOLEAN"},
        {"true + false;", "unknown operator: BOOLEAN + BOOLEAN"},
        {"5; true + false; 5", "unknown operator: BOOLEAN + BOOLEAN"},
        {"if (10 > 1) { true + false; }", "unknown operator: BOOLEAN + BOOLEAN"},
        {R"(
        if (10 > 1) {
            if (10 > 1) {
                return true + false;
            }
            return 1;
        })", "unknown operator: BOOLEAN + BOOLEAN"},
        {"foobar", "identifier not found: foobar"}
    };
    
    for (const auto& tt : tests) {
        std::unique_ptr<Object> evaluated = testEval(tt.input);
        if (!testErrorObject(evaluated.get(), tt.expectedMessage)) {
            return;
        }
    }
    
    std::cout << "TestErrorHandling passed!\n";
}

static void TestLetStatements() {
    struct Test {
        std::string input;
        int64_t expected;
    };
    
    std::vector<Test> tests = {
        {"let a = 5; a;", 5},
        {"let a = 5 * 5; a;", 25},
        {"let a = 5; let b = a; b;", 5},
        {"let a = 5; let b = a; let c = a + b + 5; c;", 15},
    };
    
    for (const auto& tt : tests) {
        std::unique_ptr<Object> evaluated = testEval(tt.input);
        if (!testIntegerObject(evaluated.get(), tt.expected)) {
            return;
        }
    }
    
    std::cout << "TestLetStatements passed!\n";
}

// ====== MAIN ======

//int main() {
//    TestEvalIntegerExpression();
//    TestEvalBooleanExpression();
//    TestBangOperator();
//    TestIfElseExpressions();
//    TestReturnStatements();
//    TestErrorHandling();
//    TestLetStatements();
//    
//    std::cout << "\n=== All evaluator tests passed! ===\n";
//    return 0;
//}