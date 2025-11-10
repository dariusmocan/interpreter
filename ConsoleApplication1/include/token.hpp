#ifndef TOKEN_HPP  // or could have used pragma once
#define TOKEN_HPP

#include <string>
#include <unordered_map>

// could be better defined as an enum probably
typedef std::string TokenType;

// refers to all the types the lexer is gonna 
// classify tokens as
namespace TokenTypes {
	const std::string ILLEGAL = "ILLEGAL";
	const std::string EOF_ = "EOF";

	//	Identifiers + literals
	const std::string IDENT = "IDENT";
	const std::string INT = "INT";

	// Operators
	const std::string ASSIGN = "=";
	const std::string PLUS = "+";
	const std::string MINUS = "-";
	const std::string BANG = "!";
	const std::string ASTERISK = "*";
	const std::string SLASH = "/";

	const std::string LT = "<";
	const std::string GT = ">";

	// Complex operators
	const std::string EQ = "EQ";
	const std::string NOT_EQ = "NOT_EQ";

	// Delimiters
	const std::string COMMA = ",";
	const std::string SEMICOLON = ";";

	const std::string LPAREN = "(";
	const std::string RPAREN = ")";
	const std::string LBRACE = "{";
	const std::string RBRACE = "}";

	// KEYWORDS
	const std::string FUNCTION = "FUNCTION";
	const std::string LET = "LET";
	const std::string TRUE = "TRUE";
	const std::string FALSE = "FALSE";
	const std::string IF = "IF";
	const std::string ELSE = "ELSE";
	const std::string RETURN = "RETURN";

}

struct Token {
	TokenType type; // type : 'Identifier', 'Number'
	std::string literal; // text of the token
};

// input : indent (e.g. "var")
// output : type (e.g. IDENT)
TokenType lookUpIdent(const std::string& ident);


#endif // !TOKEN_HPP