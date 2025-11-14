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

public:
	Parser(std::unique_ptr<Lexer>& l) : lexer(std::move(l)) {
		nextToken_parser();
		nextToken_parser();
	}
	void nextToken_parser();
	std::unique_ptr<Program> parseProgram();
	std::unique_ptr<Statement> parseStatement();
	std::unique_ptr<LetStatement> parseLetStatement();
	std::unique_ptr<ReturnStatement> parseReturnStatement();
	std::unique_ptr<ExpressionStatement> parseExpressionStatement();
	std::unique_ptr<Expression> parseExpression(Precedence p);
	bool expectPeek(const TokenType& t);
	bool currentTokenIs(const TokenType& t) const;
	bool peekTokenIs(const TokenType& t) const;

	void registerPrefix(const TokenType& tokenType, prefixParseFn fn);
	void registerInfix(const TokenType& tokenType, infixParseFn fn);

	// error functions for debugging
	std::vector<std::string> return_errors() { return errors; };
	void peekError(const TokenType& t);
	
};


#endif // !PARSER_HPP
