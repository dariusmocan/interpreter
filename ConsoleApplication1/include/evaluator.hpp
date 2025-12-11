#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include <iostream>
#include <string>
#include <memory>
#include "object.hpp"
#include "ast.hpp"



std::unique_ptr<Object> eval(Node* node);


#endif // !EVALUATOR_HPP
