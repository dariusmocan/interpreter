#ifndef PARSER_HPP
#define PARSER_HPP
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include "ast.hpp"
#include "lexer.hpp"
#include "token.hpp"

enum Precedence {
	LOWEST = 0,
	EQUALS, // ==
	LESSGREATER, // > or <
	SUM, // +
	PRODUCT, // *
	PREFIX, // -x or !x 
	CALL // functions
};


using prefixParseFn = std::function<std::unique_ptr<Expression>()>;
using infixParseFn = std::function<std::unique_ptr<Expression>(std::unique_ptr<Expression>)>;


class Parser {
private:
	std::unique_ptr<Lexer> lexer;
	Token curToken;
	Token peekToken;
	std::vector<std::string> errors;

	std::unordered_map<TokenType, prefixParseFn> prefixParseFns;
	std::unordered_map<TokenType, infixParseFn> infixParseFns;
	std::unordered_map<TokenType, Precedence> precedences;

public:
	Parser(std::unique_ptr<Lexer>& l) : lexer(std::move(l)) {
		nextToken_parser();
		nextToken_parser();

		precedences[TokenTypes::EQ] = EQUALS;
		precedences[TokenTypes::NOT_EQ] = EQUALS;
		precedences[TokenTypes::LT] = LESSGREATER;
		precedences[TokenTypes::GT] = LESSGREATER;
		precedences[TokenTypes::PLUS] = SUM;
		precedences[TokenTypes::MINUS] = SUM;
		precedences[TokenTypes::SLASH] = PRODUCT;
		precedences[TokenTypes::ASTERISK] = PRODUCT;

		registerPrefix(TokenTypes::IDENT, [this]() {
			return parseIdentifier();
			});

		registerPrefix(TokenTypes::INT, [this]() {
			return parseIntegerLiteral();
			});

		registerPrefix(TokenTypes::TRUE, [this]() {
			return parseBoolean();
			});

		registerPrefix(TokenTypes::FALSE, [this]() {
			return parseBoolean();
			});

		registerPrefix(TokenTypes::BANG, [this]() {
			return parsePrefixExpression();
			});

		registerPrefix(TokenTypes::MINUS, [this]() {
			return parsePrefixExpression();
			});

		registerPrefix(TokenTypes::LPAREN, [this]() {
			return parseGroupedExpression();
			});

		registerInfix(TokenTypes::PLUS, [this](std::unique_ptr<Expression> left) {
			return parseInfixExpression(std::move(left));
			});

		registerInfix(TokenTypes::MINUS, [this](std::unique_ptr<Expression> left) {
			return parseInfixExpression(std::move(left));
			});

		registerInfix(TokenTypes::SLASH, [this](std::unique_ptr<Expression> left) {
			return parseInfixExpression(std::move(left));
			});

		registerInfix(TokenTypes::ASTERISK, [this](std::unique_ptr<Expression> left) {
			return parseInfixExpression(std::move(left));
			});

		registerInfix(TokenTypes::EQ, [this](std::unique_ptr<Expression> left) {
			return parseInfixExpression(std::move(left));
			});

		registerInfix(TokenTypes::NOT_EQ, [this](std::unique_ptr<Expression> left) {
			return parseInfixExpression(std::move(left));
			});

		registerInfix(TokenTypes::LT, [this](std::unique_ptr<Expression> left) {
			return parseInfixExpression(std::move(left));
			});

		registerInfix(TokenTypes::GT, [this](std::unique_ptr<Expression> left) {
			return parseInfixExpression(std::move(left));
			});

	}
	void nextToken_parser();
	std::unique_ptr<Program> parseProgram(); // the core of the parser
	std::unique_ptr<Statement> parseStatement();
	std::unique_ptr<LetStatement> parseLetStatement();
	std::unique_ptr<ReturnStatement> parseReturnStatement();
	std::unique_ptr<ExpressionStatement> parseExpressionStatement();
	std::unique_ptr<Expression> parseExpression(Precedence p);
	std::unique_ptr<Expression> parseIdentifier();
	std::unique_ptr<Expression> parseIntegerLiteral();
	std::unique_ptr<Expression> parseBoolean();
	std::unique_ptr<Expression> parseGroupedExpression();
	std::unique_ptr<Expression> parsePrefixExpression();
	std::unique_ptr<Expression> parseInfixExpression(std::unique_ptr<Expression> left);


	bool expectPeek(const TokenType& t);
	bool currentTokenIs(const TokenType& t) const;
	bool peekTokenIs(const TokenType& t) const;
	Precedence currPrecedence() const;
	Precedence peekPrecedence() const;


	void registerPrefix(const TokenType& tokenType, prefixParseFn fn);
	void registerInfix(const TokenType& tokenType, infixParseFn fn);

	// error functions for debugging
	std::vector<std::string> return_errors() { return errors; };
	void peekError(const TokenType& t);
	void noPrefixParseFnError(const TokenType& t);

};


#endif // !PARSER_HPP
