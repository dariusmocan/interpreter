#ifndef AST_HPP
#define AST_HPP
#include <string>
#include <vector>
#include <memory>
#include "token.hpp"

class Node {
public:
	virtual std::string tokenLiteral() const = 0;
};

class Statement : public Node {
public: 
	virtual void statementLiteral() = 0;
};

class Expression : public Node {
public:
	virtual void expressionLiteral() = 0;
};

class Program : public Node {
public:
	std::vector<std::unique_ptr<Statement>> statements;

	std::string tokenLiteral() const override;
};

class Identifier : public Expression{
public:
	Token token;
	std::string value;

	Identifier(const Token& tok, const std::string& val) : token(tok), value(val) {}

	void expressionLiteral() override {};
	std::string tokenLiteral() const override {
		return token.literal;
	}

};

class LetStatement : public Statement {
public:
	Token token;
	std::unique_ptr<Identifier> name;
	std::unique_ptr<Expression> value;

	LetStatement(const Token& tok) : token(tok) {}

	void statementLiteral() override {};
	std::string tokenLiteral() const override {
		return token.literal;
	}
};

#endif // !AST_HPP
