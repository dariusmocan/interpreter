#include <string>
#include <unordered_map>
#include "token.hpp"

// map for all the KEYWORDS and their actual values
static const std::unordered_map<std::string, TokenType> keywords = {
	{"fn", TokenTypes::FUNCTION},
	{"let", TokenTypes::LET},
	{"true", TokenTypes::TRUE},
	{"false", TokenTypes::FALSE},
	{"if", TokenTypes::IF},
	{"else", TokenTypes::ELSE},
	{"return", TokenTypes::RETURN}
};

// input : a string - value
// return : its KEYWORD | IDENT 
TokenType lookUpIdent(const std::string& ident) {
	auto found = keywords.find(ident); // returns a pointer to the correct value {value, TokenType} or end if not found
	if (found != keywords.end()) {
		return found -> second; // found -> second => TokenType
	}
	return TokenTypes::IDENT;
}