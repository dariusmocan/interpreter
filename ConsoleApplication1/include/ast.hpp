#ifndef AST_HPP
#define AST_HPP
#include <string>
#include <vector>
#include <memory>
#include "token.hpp"

class Node {
public:
	virtual std::string tokenLiteral() const = 0;
	virtual std::string string() const = 0;
};

// general definition of statements, will be used for more concise ones
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
	std::string string() const override;
};

// @brief Identifier class : must have a token {IDENT | INT, 5} => value = 5 | "abc"  
// inherits expression for cases where the identifier must return a value (e.g. let x (undefined) = defined_value + 4)
class Identifier : public Expression{
public:
	Token token;
	std::string value;

	Identifier(const Token& tok, const std::string& val) : token(tok), value(val) {};

	void expressionLiteral() override {};
	std::string tokenLiteral() const override {
		return token.literal;
	};
	std::string string() const override {
		return value;
	};

};

// @brief token {TokenTypes::LET, let}, name : undefined_identifier, = (no need to store), value : 8 
class LetStatement : public Statement {
public:
	Token token;
	std::unique_ptr<Identifier> name;
	std::unique_ptr<Expression> value;

	LetStatement(const Token& tok) : token(tok) {};

	void statementLiteral() override {};
	std::string tokenLiteral() const override {
		return token.literal;
	};
	std::string string() const override;
};

// @brief just as let but composed only of the token {TokenTypes::RETURN, "return"} and value
class ReturnStatement : public Statement {
public:
	Token token;
	std::unique_ptr<Expression> value;

	ReturnStatement(const Token& tok) : token(tok) {};

	void statementLiteral() override {};
	std::string tokenLiteral() const override {
		return token.literal;
	};
	std::string string() const override;
};

// @brief wraps the expresions as statements so that they can be stored in the vector of program
// wraps all the expressions as : x + sum(a + b)
class ExpressionStatement : public Statement {
public:
	Token token;
	std::unique_ptr<Expression> value;

	ExpressionStatement(const Token& tok) : token(tok) {};

	void statementLiteral() override {};
	std::string tokenLiteral() const override {
		return token.literal;
	};
	std::string string() const override;
};

class BlockStatement : public Statement{
public:
	Token token;
	std::vector<std::unique_ptr<Statement>> statements;

	BlockStatement(const Token& tok) : token(tok) {};
	void statementLiteral() override {};
	std::string tokenLiteral() const override {
		return token.literal;
	};
	std::string string() const override;

};


// @brief storing TokenTypes::INT as integer values
class IntegerLiteral : public Expression {
public:
	Token token;
	int64_t value;

	IntegerLiteral(const Token& tok, int64_t val) : token(tok), value(val) {};

	void expressionLiteral() override {};
	std::string tokenLiteral() const override {
		return token.literal;
	};

	std::string string() const override {
		return token.literal;
	};
};

// @brief strogin TokenTypes::BOOL 
class Boolean : public Expression {
public:
	Token token;
	bool value;

	Boolean(const Token& tok, bool val) : token(tok), value(val) {};

	void expressionLiteral() override {};
	std::string tokenLiteral() const override {
		return token.literal;
	};
	std::string string() const override {
		return token.literal;
	};

};

class PrefixExpression : public Expression {
public:
	Token token;
	std::string oper;
	std::unique_ptr<Expression> right;

	PrefixExpression(const Token& tok) : token(tok) {};

	void expressionLiteral() override {};
	std::string tokenLiteral() const override {
		return token.literal;
	};
	std::string string() const override;

};

class InfixExpression : public Expression {
public:
    Token token;
    std::unique_ptr<Expression> left;
    std::string oper;
    std::unique_ptr<Expression> right;

    InfixExpression(const Token& tok) : token(tok) {};

    void expressionLiteral() override {};
	std::string tokenLiteral() const override {
		return token.literal;
	};
    std::string string() const override;
};

class IfExpression : public Expression {
public:
	Token token;
	std::unique_ptr<Expression> condition;
	std::unique_ptr<BlockStatement> consequence;
	std::unique_ptr<BlockStatement> alternative;

	IfExpression(const Token& tok) : token(tok) {};

	void expressionLiteral() override {};
	std::string tokenLiteral() const override {
		return token.literal;
	};
	std::string string() const override;

};

class FunctionLiteral : public Expression {
public:
	Token token;
	std::vector<std::unique_ptr<Identifier>> parameters;
	std::unique_ptr<BlockStatement> body;

	FunctionLiteral(const Token& tok) : token(tok) {};

	void expressionLiteral() override {};
	std::string tokenLiteral() const override {
		return token.literal;
	};
	std::string string() const override;
};

class CallExpression : public Expression {
public:
	Token token; // the '('
	std::unique_ptr<Expression> function; //
	std::vector<std::unique_ptr<Expression>> arguments;

	CallExpression(Token tok) : token(tok) {};

	void expressionLiteral() override {};
	std::string tokenLiteral() const override {
		return token.literal;
	};
	std::string string() const override;
};


#endif // !AST_HPP
