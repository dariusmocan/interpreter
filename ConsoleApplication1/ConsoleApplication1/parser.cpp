#include <string>
#include "parser.hpp"
#include "token.hpp"
#include "lexer.hpp"
#include "ast.hpp"


void Parser::nextToken_parser() {
	curToken = peekToken;
	peekToken = lexer -> nextToken();
}

std::unique_ptr<Program> Parser::parseProgram() {
	auto program = std::make_unique<Program>();

	while (curToken.type != TokenTypes::EOF_) {
		std::unique_ptr<Statement> stmt = parseStatement();
		if (stmt != nullptr) {
			program->statements.push_back(std::move(stmt));
		}
		nextToken_parser();
	}

	return program;
}

std::unique_ptr<Statement> Parser::parseStatement() {
	// Note : not using switch because it doesn't work on std::string type
	// maybe implementing TokenTypes as enum later
	if (curToken.type == TokenTypes::LET) {
		return parseLetStatement();
	} else {
		return nullptr;
	}
}

std::unique_ptr<LetStatement> Parser::parseLetStatement() {

	std::unique_ptr<LetStatement> stmt = std::make_unique<LetStatement>(curToken);

	// the first token after 'let' must be an identificator. if not, incorrect.
	// we look ahead to check if peekToken is an IDENT, and then move tokens once so that
	// curToken is the IDENT
	if (!expectPeek(TokenTypes::IDENT)) {
		return nullptr;
	}
	stmt->name = std::make_unique<Identifier>(curToken, curToken.literal);

	// same logic repeated for assign cus after IDENT there must be a '='
	if (!expectPeek(TokenTypes::ASSIGN)) {
		return nullptr;
	}

	// TODO : expressions until we
	// encounter a semicolon
	while (curToken.type != TokenTypes::SEMICOLON) {
		nextToken_parser();
	}

	return stmt;
}

bool Parser::currentTokenIs(const TokenType& t) const {
	return curToken.type == t;
}

bool Parser::peekTokenIs(const TokenType& t) const {
	return peekToken.type == t;
}

bool Parser::expectPeek(const TokenType& t) {
	if (peekToken.type == t) {
		nextToken_parser();
		return true;
	} else {
		peekError(t);
		return false;
	}
}

void Parser::peekError(const TokenType& t) {
	std::string msg = "expected next token to be : '" + t + "', got '" +
		peekToken.type + "' instead";
	errors.push_back(msg);
}



