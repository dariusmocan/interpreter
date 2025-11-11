#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "lexer.hpp"
#include "token.hpp"

std::string PROMPT = ">>";

void Start(std::istream& in, std::ostream& out) {

	std::string line;

	while (true) {
		out << PROMPT;

		if (!std::getline(in, line)) {
			return;
		}

		Lexer lexer(line);

		Token tok = lexer.nextToken();
		while (tok.type != TokenTypes::EOF_) {
			out << "tok type : " << tok.type << "; tok literal : " << tok.literal << "\n";
			tok = lexer.nextToken();
		}

	}

}
