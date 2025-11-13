#ifndef PARSER_HPP
#define PARSER_HPP
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include "ast.hpp"
#include "lexer.hpp"
#include "token.hpp"


class Parser {
private:
	std::unique_ptr<Lexer> lexer;
	Token curToken;
	Token peekToken;
	std::vector<std::string> errors;

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
	bool expectPeek(const TokenType& t);
	bool currentTokenIs(const TokenType& t) const;
	bool peekTokenIs(const TokenType& t) const;


	std::vector<std::string> return_errors() { return errors; };
	void peekError(const TokenType& t);
	
};


#endif // !PARSER_HPP
