#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include <iostream>
#include <string>
#include <memory>
#include "object.hpp"
#include "ast.hpp"



std::unique_ptr<Object> eval(Node* node);

std::unique_ptr<Object> evalStatements(const std::vector<std::unique_ptr<Statement>>& statements);



#endif // !EVALUATOR_HPP
