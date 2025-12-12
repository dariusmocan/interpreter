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
    
    return eval(program.get());
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

// ====== MAIN ======

//int main() {
//    TestEvalIntegerExpression();
//    TestEvalBooleanExpression();
//    TestBangOperator();
//    
//    std::cout << "\n=== All evaluator tests passed! ===\n";
//    return 0;
//}