#include "evaluator.hpp"
#include <vector>


std::unique_ptr<Object> eval(Node* node) {

	if (auto* progLit = dynamic_cast<Program*>(node))
		return evalStatements(progLit->statements);

	if (auto* exprStmt = dynamic_cast<ExpressionStatement*>(node))
		return eval(exprStmt->value.get());

	if (auto* intLit = dynamic_cast<IntegerLiteral*>(node))
		return std::make_unique<Integer>(intLit->value);

	if (auto* boolLit = dynamic_cast<BooleanLiteral*>(node))
		return std::make_unique<Boolean>(boolLit->value);

	return nullptr;
}

std::unique_ptr<Object> evalStatements(const std::vector<std::unique_ptr<Statement>>& statements) {
	std::unique_ptr<Object> result;

	for (const auto& stmt : statements) {
		result = eval(stmt.get());
	}

	return result;
}