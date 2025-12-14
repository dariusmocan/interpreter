#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include <iostream>
#include <string>
#include <memory>
#include <utility>
#include "object.hpp"
#include "ast.hpp"



std::unique_ptr<Object> eval(Node* node, std::shared_ptr<Environment> env);


#endif // !EVALUATOR_HPP
