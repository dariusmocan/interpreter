#include "evaluator.hpp"
#include <vector>

static std::unique_ptr<Object> evalProgram(const std::vector<std::unique_ptr<Statement>>& statements);
static std::unique_ptr<Object> evalPrefixExpression(const std::string& op, std::unique_ptr<Object> right);
static std::unique_ptr<Object> evalBangOperatorExpression(std::unique_ptr<Object> right);
static std::unique_ptr<Object> evalMinusPrefixOperatorExpression(std::unique_ptr<Object> right);
static std::unique_ptr<Object> evalInfixExpression(const std::string& op,
	std::unique_ptr<Object> left,std::unique_ptr<Object> right);
static std::unique_ptr<Object> evalIntegerInfixExpression(const std::string& op,
	Integer* left, Integer* right);
static std::unique_ptr<Object> evalIfExpression(IfExpression* ifExpr);
static std::unique_ptr<Object> evalBlockStatement(BlockStatement* block);


bool isTruthy(Object* obj) {

	if (obj == nullptr) {
		return false;
	}

	if (dynamic_cast<Null*>(obj)) {
		return false;
	}

	if (Boolean* boolObj = dynamic_cast<Boolean*>(obj)) {
		return boolObj->value;
	}

	return true;
}

bool isError(Object* obj) {
	if (obj) {
		return obj->Type() == objectTypes::ERROR_OBJ;
	}

	return false;
}


std::unique_ptr<Object> eval(Node* node) {

	if (auto* progLit = dynamic_cast<Program*>(node))
		return evalProgram(progLit->statements);

	if (auto* exprStmt = dynamic_cast<ExpressionStatement*>(node))
		return eval(exprStmt->value.get());

	if (auto* prefixExpr = dynamic_cast<PrefixExpression*>(node)) {
		std::unique_ptr<Object> right = eval(prefixExpr->right.get());

		if (isError(right.get())) {
			return right;
		}

		return evalPrefixExpression(prefixExpr->oper, std::move(right));
	}

	if (auto* infixExpr = dynamic_cast<InfixExpression*>(node)){
		std::unique_ptr<Object> left = eval(infixExpr->left.get());
		if (isError(left.get())) {
			return left;
		}

		std::unique_ptr<Object> right = eval(infixExpr->right.get());
		if (isError(right.get())) {
			return right;
		}

		return evalInfixExpression(infixExpr->oper, std::move(left), std::move(right));
	}

	if (auto* ifExpr = dynamic_cast<IfExpression*>(node)) {
		return evalIfExpression(ifExpr);
	}

	if (auto* blockStmt = dynamic_cast<BlockStatement*>(node)) {
		return evalBlockStatement(blockStmt);
	}

	if (auto* returnStmt = dynamic_cast<ReturnStatement*>(node)) {
		std::unique_ptr<Object> val = eval(returnStmt->value.get());
		if (isError(val.get())) {
			return val;
		}

		return std::make_unique<ReturnValue>(std::move(val));
	}

	if (auto* intLit = dynamic_cast<IntegerLiteral*>(node))
		return std::make_unique<Integer>(intLit->value);

	if (auto* boolLit = dynamic_cast<BooleanLiteral*>(node))
		return std::make_unique<Boolean>(boolLit->value);

	return nullptr;
}



std::unique_ptr<Object> evalProgram(const std::vector<std::unique_ptr<Statement>>& statements) {
	std::unique_ptr<Object> result;

	for (const auto& stmt : statements) {
		result = eval(stmt.get());

		if (result && result->Type() == objectTypes::RETURN_OBJ) {
			ReturnValue* returnVal = dynamic_cast<ReturnValue*>(result.get());
			return std::move(returnVal->value);
		}

		if (result && result->Type() == objectTypes::ERROR_OBJ) {
			return result;
		}
	}

	return result;
}


static std::unique_ptr<Object> evalBlockStatement(BlockStatement* block) {
	std::unique_ptr<Object> result;

	for (const auto& stmt : block->statements) {
		result = eval(stmt.get());

		if (result && result->Type() == objectTypes::RETURN_OBJ) {
			return result;
		}

		if (result && result->Type() == objectTypes::ERROR_OBJ) {
			return result;
		}
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
		return std::make_unique<Error>("Unknown operator : " + op + "; object type : " + right->Type());
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
		return std::make_unique<Error>("Type missmatch : " + right->Type());
	}
}

static std::unique_ptr<Object> evalInfixExpression(const std::string & op, std::unique_ptr<Object> left, 
	std::unique_ptr<Object> right) {
	auto* leftExpr = dynamic_cast<Integer*>(left.get());
	auto* rightExpr = dynamic_cast<Integer*>(right.get());
	if ( leftExpr && rightExpr){
		return evalIntegerInfixExpression(op, leftExpr, rightExpr);
	}
	if (op == "==") {
		auto* leftBool = dynamic_cast<Boolean*>(left.get());
		auto* rightBool = dynamic_cast<Boolean*>(right.get());

		if (leftBool && rightBool) {
			return std::make_unique<Boolean>(leftBool->value == rightBool->value);
		}
	}
	if (op == "!=") {
		auto* leftBool = dynamic_cast<Boolean*>(left.get());
		auto* rightBool = dynamic_cast<Boolean*>(right.get());

		if (leftBool && rightBool) {
			return std::make_unique<Boolean>(leftBool->value != rightBool->value);
		}
	}

	if (left->Type() != right->Type()) {
		return std::make_unique<Error>("type mismatch: " + left->Type() + " + " + right->Type());
	}

	return std::make_unique<Error>("unknown operator : " + op + "; object types: " + left->Type() + right->Type());

}

static std::unique_ptr<Object> evalIntegerInfixExpression(const std::string& op,
	Integer* left, Integer* right) {
	int64_t left_val = left->value, right_val = right->value;

	if (op == "+") {
		return std::make_unique<Integer>(left_val + right_val);
	}
	else if (op == "-") {
		return std::make_unique<Integer>(left_val - right_val);
	}
	else if (op == "*") {
		return std::make_unique<Integer>(left_val * right_val);
	}
	else if (op == "/") {
		if (right_val == 0) {
			return std::make_unique<Error>("Division by zero");
		}

		return std::make_unique<Integer>(left_val / right_val);
	}
	else if (op == "<") {
		return std::make_unique<Boolean>(left_val < right_val);
	}
	else if (op == ">") {
		return std::make_unique<Boolean>(left_val > right_val);
	}
	else if (op == "==") {
		return std::make_unique<Boolean>(left_val == right_val);
	}
	else if (op == "!=") {
		return std::make_unique<Boolean>(left_val != right_val);
	}
	else {
		return std::make_unique<Error>("Unknown operator : " + op);
	}
}

static std::unique_ptr<Object> evalIfExpression(IfExpression* ifExpr) {
	std::unique_ptr<Object> condition = eval(ifExpr->condition.get());
	if (isError(condition.get())) {
		return condition;
	}


	if (isTruthy(condition.get())) {
		return eval(ifExpr->consequence.get());
	}
	else if (ifExpr->alternative != nullptr){
		return eval(ifExpr->alternative.get());
	}
	else {
		return nullptr;
	}


}
