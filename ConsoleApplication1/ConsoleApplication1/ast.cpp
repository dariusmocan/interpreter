#include <string>
#include <sstream>
#include <vector>
#include "ast.hpp"


std::string Program::tokenLiteral() const{
    if (!statements.empty()) {
        return statements[0]->tokenLiteral();
    }
    else {
        return "";
    }
}

std::string Program::string() const {
    std::ostringstream out;

    for (const auto& s : statements) {
        out << s->string();
    }

    return out.str();
}

std::string LetStatement::string() const {
    std::ostringstream out;

    out << tokenLiteral() << " ";
    out << name->string();
    out << "=";

    if (value != nullptr) {
        out << value->string();
    }

    out << ";";
    return out.str();
}

std::string ReturnStatement::string() const {
    std::ostringstream out;

    out << tokenLiteral() << " ";

    if (value != nullptr) {
        out << value->string();
    }

    out << ";";
    return out.str();
}

std::string ExpressionStatement::string() const {
    std::ostringstream out;

    if (value != nullptr) {
        out << value->string();
    }
    
    out << ";";
    return out.str();
}

std::string PrefixExpression::string() const {
    std::ostringstream out;

    out << "(" << oper << right << ")";

    return out.str();
}
