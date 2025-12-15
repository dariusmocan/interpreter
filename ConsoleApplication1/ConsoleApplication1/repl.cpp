#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include "lexer.hpp"
#include "token.hpp"
#include "parser.hpp"
#include "ast.hpp"
#include "evaluator.hpp"

std::string PROMPT = ">>"; 

void printParseErrors(std::ostream& out, const std::vector<std::string>& errors) {
	for (const std::string& error : errors) {
		out << "\t" << error << "\n";
	}
}

void Start(std::istream& in, std::ostream& out) {

	std::string line; // storing each line of the user input
	std::shared_ptr<Environment> env = std::make_shared<Environment>();
	std::vector<std::unique_ptr<Program>> programs;

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

		// showing parser results
		/*out << program->string();
		out << '\n';*/


		std::unique_ptr<Object> evaluator = eval(program.get(), env);

		programs.push_back(std::move(program));

		if (evaluator != nullptr) {
			out << evaluator->Inspect();
			out << '\n';
		}

	}

}

int main() {
	Start(std::cin, std::cout);


	return 0;
}
