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
	// returns a program that has a vector with all the statements
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
	// maybe if implementing TokenTypes as enum later
	// 3 main types of statements : let, return, anything else
	if (curToken.type == TokenTypes::LET) {
		return parseLetStatement();
	} else if (curToken.type == TokenTypes::RETURN) {
		return parseReturnStatement();
	} else {
		return parseExpressionStatement();
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

std::unique_ptr<ReturnStatement> Parser::parseReturnStatement() {
	
	std::unique_ptr<ReturnStatement> stmt = std::make_unique<ReturnStatement>(curToken);

	while (!currentTokenIs(TokenTypes::SEMICOLON)) {
		nextToken_parser();
	}

	return stmt;
}

std::unique_ptr<ExpressionStatement> Parser::parseExpressionStatement() {

	std::unique_ptr<ExpressionStatement> stmt = std::make_unique<ExpressionStatement>(curToken);

	stmt->value = parseExpression(LOWEST);

	if (peekTokenIs(TokenTypes::SEMICOLON)) {
		nextToken_parser();
	}

	return stmt;
}

std::unique_ptr<Expression> Parser::parseExpression(Precedence p) {

	auto prefixIT = prefixParseFns.find(curToken.type);

	if (prefixIT == prefixParseFns.end()) {
		return nullptr;
	}

	prefixParseFn prefix = prefixIT->second;
	std::unique_ptr<Expression> leftExp = prefix();

	return leftExp;

}

std::unique_ptr<Expression> Parser::parseIdentifier() {
	return std::make_unique<Identifier>(curToken, curToken.literal);
}

std::unique_ptr<Expression> Parser::parseIntegerLiteral() {

	try {
		int64_t value = std::stoll(curToken.literal);
		return std::make_unique<IntegerLiteral>(curToken, value);
	}
	catch (...) {
		std::string msg = "Could not transform : " + curToken.literal + "to integer!";
		std::cerr << msg;
		errors.push_back(msg);
		return nullptr;
	}

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

void Parser::registerPrefix(const TokenType& tokentype, prefixParseFn fn) {
	prefixParseFns[tokentype] = fn;
}

void Parser::registerInfix(const TokenType& tokenType, infixParseFn fn) {
	infixParseFns[tokenType] = fn;
}

void Parser::peekError(const TokenType& t) {
	std::string msg = "expected next token to be : '" + t + "', got '" +
		peekToken.type + "' instead";
	errors.push_back(msg);
}



