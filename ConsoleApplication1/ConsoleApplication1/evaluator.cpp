#include "evaluator.hpp"
#include <vector>

static std::unique_ptr<Object> evalProgram(const std::vector<std::unique_ptr<Statement>>& statements, std::shared_ptr<Environment> env);
static std::unique_ptr<Object> evalPrefixExpression(const std::string& op, std::unique_ptr<Object> right);
static std::unique_ptr<Object> evalBangOperatorExpression(std::unique_ptr<Object> right);
static std::unique_ptr<Object> evalMinusPrefixOperatorExpression(std::unique_ptr<Object> right);
static std::unique_ptr<Object> evalInfixExpression(const std::string& op,
	std::unique_ptr<Object> left, std::unique_ptr<Object> right);
static std::unique_ptr<Object> evalIntegerInfixExpression(const std::string& op,
	Integer* left, Integer* right);
static std::unique_ptr<Object> evalIfExpression(IfExpression* ifExpr, std::shared_ptr<Environment> env);
static std::unique_ptr<Object> evalBlockStatement(BlockStatement* block, std::shared_ptr<Environment> env);
static std::unique_ptr<Object> evalIdentifier(Identifier* ident, std::shared_ptr<Environment> env);


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


std::unique_ptr<Object> eval(Node* node, std::shared_ptr<Environment> env) {

	if (auto* progLit = dynamic_cast<Program*>(node))
		return evalProgram(progLit->statements, env);

	if (auto* exprStmt = dynamic_cast<ExpressionStatement*>(node))
		return eval(exprStmt->value.get(), env);

	if (auto* prefixExpr = dynamic_cast<PrefixExpression*>(node)) {
		std::unique_ptr<Object> right = eval(prefixExpr->right.get(), env);

		if (isError(right.get())) {
			return right;
		}

		return evalPrefixExpression(prefixExpr->oper, std::move(right));
	}

	if (auto* infixExpr = dynamic_cast<InfixExpression*>(node)){
		std::unique_ptr<Object> left = eval(infixExpr->left.get(), env);
		if (isError(left.get())) {
			return left;
		}

		std::unique_ptr<Object> right = eval(infixExpr->right.get(), env);
		if (isError(right.get())) {
			return right;
		}

		return evalInfixExpression(infixExpr->oper, std::move(left), std::move(right));
	}

	if (auto* ifExpr = dynamic_cast<IfExpression*>(node)) {
		return evalIfExpression(ifExpr, env);
	}

	if (auto* blockStmt = dynamic_cast<BlockStatement*>(node)) {
		return evalBlockStatement(blockStmt, env);
	}

	if (auto* returnStmt = dynamic_cast<ReturnStatement*>(node)) {
		std::unique_ptr<Object> val = eval(returnStmt->value.get(), env);
		if (isError(val.get())) {
			return val;
		}

		return std::make_unique<ReturnValue>(std::move(val));
	}

	if (auto* letStmt = dynamic_cast<LetStatement*>(node)) {
		std::unique_ptr<Object> val = eval(letStmt->value.get(), env);
		if (isError(val.get())) {
			return val;
		}

		env->setObject(letStmt->name->value, std::shared_ptr<Object>(val.release()));
		return nullptr;
	}

	if (auto* ident = dynamic_cast<Identifier*>(node)) {
		return evalIdentifier(ident, env);
	}

	if (auto* intLit = dynamic_cast<IntegerLiteral*>(node))
		return std::make_unique<Integer>(intLit->value);

	if (auto* boolLit = dynamic_cast<BooleanLiteral*>(node))
		return std::make_unique<Boolean>(boolLit->value);

	return nullptr;
}



std::unique_ptr<Object> evalProgram(const std::vector<std::unique_ptr<Statement>>& statements
	, std::shared_ptr<Environment> env) {
	std::unique_ptr<Object> result;

	for (const auto& stmt : statements) {
		result = eval(stmt.get(), env);

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


static std::unique_ptr<Object> evalBlockStatement(BlockStatement* block
	, std::shared_ptr<Environment> env) {
	std::unique_ptr<Object> result;

	for (const auto& stmt : block->statements) {
		result = eval(stmt.get(), env);

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

static std::unique_ptr<Object> evalIfExpression(IfExpression* ifExpr, std::shared_ptr<Environment> env) {
	std::unique_ptr<Object> condition = eval(ifExpr->condition.get(), env);
	if (isError(condition.get())) {
		return condition;
	}


	if (isTruthy(condition.get())) {
		return eval(ifExpr->consequence.get(), env);
	}
	else if (ifExpr->alternative != nullptr){
		return eval(ifExpr->alternative.get(), env);
	}
	else {
		return nullptr;
	}
}

static std::unique_ptr<Object> evalIdentifier(Identifier* ident, std::shared_ptr<Environment> env) {
	auto [obj, found] = env->getObject(ident->value);

	if (!found) {
		return std::make_unique<Error>("identifier not found: " + ident->value);
	}

	if (auto* intVal = dynamic_cast<Integer*>(obj.get())) {
		return std::make_unique<Integer>(intVal->value);
	}
	if (auto* boolVal = dynamic_cast<Boolean*>(obj.get())) {
		return std::make_unique<Boolean>(boolVal->value);
	}
	if (dynamic_cast<Null*>(obj.get())) {
		return std::make_unique<Null>();
	}

	return std::make_unique<Error>("unknown object type: " + obj->Type());

}

