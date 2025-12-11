#include "evaluator.hpp"
#include <vector>

static std::unique_ptr<Object> evalStatements(const std::vector<std::unique_ptr<Statement>>& statements);
static std::unique_ptr<Object> evalPrefixExpression(const std::string& op, std::unique_ptr<Object> right);
static std::unique_ptr<Object> evalBangOperatorExpression(std::unique_ptr<Object> right);
static std::unique_ptr<Object> evalMinusPrefixOperatorExpression(std::unique_ptr<Object> right);

std::unique_ptr<Object> eval(Node* node) {

	if (auto* progLit = dynamic_cast<Program*>(node))
		return evalStatements(progLit->statements);

	if (auto* exprStmt = dynamic_cast<ExpressionStatement*>(node))
		return eval(exprStmt->value.get());

	if (auto* prefixExpr = dynamic_cast<PrefixExpression*>(node)) {
		std::unique_ptr<Object> right = eval(prefixExpr->right.get());
		return evalPrefixExpression(prefixExpr->oper, std::move(right));
	}

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

std::unique_ptr<Object> evalPrefixExpression(const std::string& op, std::unique_ptr<Object> right) {
	if (op == "!") {
		return evalBangOperatorExpression(std::move(right));
	}
	else if (op == "-") {
		return evalMinusPrefixOperatorExpression(std::move(right));
	}
	else {
		return nullptr;
	}
}

static std::unique_ptr<Object> evalBangOperatorExpression(std::unique_ptr<Object> right) {
	if (auto* boolObj = dynamic_cast<Boolean*>(right.get())) {
		return std::make_unique<Boolean>(!boolObj->value);
	}

	if (dynamic_cast<Null*>(right.get())) {
		return std::make_unique<Boolean>(true);
	}

	return std::make_unique<Boolean>(false);
}

static std::unique_ptr<Object> evalMinusPrefixOperatorExpression(std::unique_ptr<Object> right) {
	if (auto* intType = dynamic_cast<Integer*>(right.get())) {
		int64_t val = -intType->value;
		return std::make_unique<Integer>(val);
	}
	else {
		return nullptr;
	}
}