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

std::unique_ptr<BlockStatement> Parser::parseBlockStatement() {
	std::unique_ptr<BlockStatement> block = std::make_unique<BlockStatement>(curToken);

	nextToken_parser();

	while (!currentTokenIs(TokenTypes::RBRACE) && !currentTokenIs(TokenTypes::EOF_)) {
		std::unique_ptr<Statement> stmt = parseStatement();
		if (stmt != nullptr) {
			block->statements.push_back(std::move(stmt));
		}
		nextToken_parser();
	}

	return block;
}

std::unique_ptr<Expression> Parser::parseExpression(Precedence p) {

	auto prefixIT = prefixParseFns.find(curToken.type);

	if (prefixIT == prefixParseFns.end()) {
		noPrefixParseFnError(curToken.type);
		return nullptr;
	}
	prefixParseFn prefix = prefixIT->second;

	std::unique_ptr<Expression> leftExp = prefix();

	while (!peekTokenIs(TokenTypes::SEMICOLON) && p < peekPrecedence()) {
		auto infixIT = infixParseFns.find(peekToken.type);

		if (infixIT == infixParseFns.end()) {
			return leftExp;
		}

		nextToken_parser();

		infixParseFn infix = infixIT->second;
		leftExp = infix(std::move(leftExp));
	}

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

std::unique_ptr<Expression> Parser::parseBoolean() {
	bool value = currentTokenIs(TokenTypes::TRUE);
	return std::make_unique<Boolean>(curToken, value);
}

std::unique_ptr<Expression> Parser::parseGroupedExpression() {
	nextToken_parser();

	auto exp = parseExpression(LOWEST);

	if (!expectPeek(TokenTypes::RPAREN)) {
		return nullptr;
	}

	return exp;
}

std::unique_ptr<Expression> Parser::parseIfExpression() {
	std::unique_ptr<IfExpression> expression = std::make_unique<IfExpression>(curToken);

	if (!expectPeek(TokenTypes::LPAREN)) {
		return nullptr;
	}

	nextToken_parser();

	expression->condition = parseExpression(LOWEST);

	if (!expectPeek(TokenTypes::RPAREN)) {
		return nullptr;
	}

	if (!expectPeek(TokenTypes::LBRACE)) {
		return nullptr;
	}

	expression->consequence = parseBlockStatement();

	if (peekTokenIs(TokenTypes::ELSE)) {
		nextToken_parser();

		if (!expectPeek(TokenTypes::LBRACE)) {
			return nullptr;
		}

		expression->alternative = parseBlockStatement();
	}

	return expression;
}

std::unique_ptr<Expression> Parser::parsePrefixExpression() {
	// create prefixExpression with curent token and its literal : ! || -
	std::unique_ptr<PrefixExpression> expression = std::make_unique<PrefixExpression>(curToken);
	expression->oper = curToken.literal;

	// then we advance and check for the rest of the expression
	nextToken_parser();
	expression->right = parseExpression(PREFIX);

	return expression;
}

std::unique_ptr<Expression> Parser::parseInfixExpression(std::unique_ptr<Expression> left) {
	std::unique_ptr<InfixExpression> expression = std::make_unique<InfixExpression>(curToken);
	expression -> oper = curToken.literal;
	expression -> left = std::move(left);

	Precedence prec = currPrecedence();
	nextToken_parser();
	expression->right = parseExpression(prec);

	return expression;
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

Precedence Parser::currPrecedence() const {
	auto it = precedences.find(curToken.type);

	if (it != precedences.end()) {
		return it->second;
	}

	return LOWEST;
}

Precedence Parser::peekPrecedence() const {
	auto it = precedences.find(peekToken.type);

	if (it != precedences.end()) {
		return it->second;
	}

	return LOWEST;
}

void Parser::registerPrefix(const TokenType& tokentype, prefixParseFn fn) {
	prefixParseFns[tokentype] = fn;
}

void Parser::registerInfix(const TokenType& tokenType, infixParseFn fn) {
	infixParseFns[tokenType] = fn;
}

void Parser::noPrefixParseFnError(const TokenType& tokentype) {
	std::string msg = "no prefix parse function for : " + tokentype + " found!";
	errors.push_back(msg);
}

void Parser::peekError(const TokenType& t) {
	std::string msg = "expected next token to be : '" + t + "', got '" +
		peekToken.type + "' instead";
	errors.push_back(msg);
}




