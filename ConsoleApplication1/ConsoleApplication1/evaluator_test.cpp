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
        {"-5",5},
        {"-10",-10}
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
        {"false", false}
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