#include <string>
#include <cctype>
#include "lexer.hpp"
#include "token.hpp"

// auxiliary function to verify if char is a letter or '_'
bool static isLetter(char ch) {
	if (isalpha(static_cast<unsigned char>(ch)) || ch == '_') {
		return true;
	}
	return false;
}

void Lexer::readChar() {
	if (readPosition >= input.size()) {
		ch = 0; // indicating end of file (EOF)
	}else {
		ch = input[readPosition];
	}
	position = readPosition;
	readPosition += 1;
}

// if ch is any number of white spaces, we skip
void Lexer::skipWhiteSpaces() {
	while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
		readChar();
	}
}

// principal function for lexing the input. returns : current Token
Token Lexer::nextToken() {
	// everytime skip the white spaces if any
	skipWhiteSpaces();
	//initiate a new Token object which we will return
	Token tok;

	// all the current cases for ch
	switch (ch){
		case '=':
			if (peekChar() == '=') {
				tok = Token{ TokenTypes::EQ, std::string(1,ch) + input[readPosition]};
				readChar();
				break;
			} else {
				tok = Token{ TokenTypes::ASSIGN, std::string(1,ch) };
				break;
			}
		case '!':
			if (peekChar() == '=') {
				tok = Token{ TokenTypes::NOT_EQ, std::string(1,ch) + input[readPosition] };
				readChar();
				break;
			}
			else {
				tok = Token{ TokenTypes::BANG, std::string(1,ch) };
				break;
			}
		case '*':
			tok = Token{ TokenTypes::ASTERISK, std::string(1,ch) };
			break;
		case '/':
			tok = Token{ TokenTypes::SLASH, std::string(1,ch) };
			break;
		case '<':
			tok = Token{ TokenTypes::LT, std::string(1,ch) };
			break;
		case '>':
			tok = Token{ TokenTypes::GT, std::string(1,ch) };
			break;
		case ';':
			tok = Token{ TokenTypes::SEMICOLON, std::string(1,ch) };
			break;
		case '(':
			tok = Token{ TokenTypes::LPAREN, std::string(1,ch) };
			break;
		case ')':
			tok = Token{ TokenTypes::RPAREN, std::string(1,ch) };
			break;
		case ',':
			tok = Token{ TokenTypes::COMMA, std::string(1,ch) };
			break;
		case '+':
			tok = Token{ TokenTypes::PLUS, std::string(1,ch) };
			break;
		case '-':
			tok = Token{ TokenTypes::MINUS, std::string(1,ch) };
			break;
		case '{':
			tok = Token{ TokenTypes::LBRACE, std::string(1,ch) };
			break;
		case '}':
			tok = Token{ TokenTypes::RBRACE, std::string(1,ch) };
			break;
		case 0:
			tok = Token{ TokenTypes::EOF_, ""};
			break;
		case '"':
			tok.type = TokenTypes::STRING;
			tok.literal = readString();
			break;
		default:
			if (isLetter(ch)) {
				std::string literal = readIdentifier();
				tok = Token{ lookUpIdent(literal), literal};
				return tok; // we return here because readIdentifier() already read the next char
			}
			else if (isdigit(ch)) {
				tok = Token{ TokenTypes::INT, readNumber()};
				return tok; // same as in the letter case
			}else {
				tok = Token{ TokenTypes::ILLEGAL, std::string(1,ch) };
				break;
			}
	}
	readChar(); // move to the next char
	return tok; // return current token
	}


std::string Lexer::readIdentifier() {
	size_t start_position = position;
	while (isLetter(ch)) {
		readChar();
	}
	return input.substr(start_position, position - start_position); // return the positions. input : "var = 123". output: var
}

std::string Lexer::readNumber() {
	size_t start_position = position;
	while (isdigit(ch)) {
		readChar();
	}
	return input.substr(start_position, position - start_position); // return the positions. input : "var = 123". output: 123
}

std::string Lexer::readString() {
	size_t pos = position + 1;

	while (true) {
		readChar();
		if (ch == '"' || ch == 0) {
			break;
		}
	}

	return input.substr(pos, position - pos);

}

char Lexer::peekChar() {
	if (readPosition > input.length()) {
		return 0;
	}
	else {
		return input[readPosition];
	}
}


