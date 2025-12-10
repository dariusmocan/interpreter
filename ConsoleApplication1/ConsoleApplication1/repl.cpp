#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include "lexer.hpp"
#include "token.hpp"
#include "parser.hpp"
#include "ast.hpp"

std::string PROMPT = ">>"; 

void printParseErrors(std::ostream& out, const std::vector<std::string>& errors) {
	for (const std::string& error : errors) {
		out << "\t" << error << "\n";
	}
}

void Start(std::istream& in, std::ostream& out) {

	std::string line; // storing each line of the user input 

	while (true) {
		out << PROMPT;

		// if it can't get anything anymore, return
		if (!std::getline(in, line)) {
			return;
		}

		std::unique_ptr<Lexer> l = std::make_unique<Lexer>(line);
		Parser p(l);
		std::unique_ptr<Program> program = p.parseProgram();

		std::vector<std::string> errors = p.getErrors();

		if (errors.size() != 0) {
			printParseErrors(out, errors);
			continue;
		}

		out << program->string();
		out << '\n';

	}

}

int main() {
	Start(std::cin, std::cout);
}