#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "lexer.hpp"
#include "token.hpp"

std::string PROMPT = ">>";

void Start(std::istream& in, std::ostream& out) {

	std::string line; // storing each line of the user input 

	while (true) {
		out << PROMPT;

		// if it can't get anything anymore, return
		if (!std::getline(in, line)) {
			return;
		}

		Lexer lexer(line);

		Token tok = lexer.nextToken();
		// while there are tokens in the lexer/input => output the token type and literal
		while (tok.type != TokenTypes::EOF_) {
			out << "tok type : " << tok.type << "; tok literal : " << tok.literal << "\n";
			tok = lexer.nextToken();
		}

	}

}
