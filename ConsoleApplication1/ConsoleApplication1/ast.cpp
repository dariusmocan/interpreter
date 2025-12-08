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
    
    // out << ";";
    return out.str();
}

std::string BlockStatement::string() const {
    std::ostringstream out;

    for (const auto& statement : statements) {
        out << statement->string();
    }

    return out.str();
}

std::string PrefixExpression::string() const {
    std::ostringstream out;

    out << "(" << oper << right -> string() << ")";

    return out.str();
}

std::string InfixExpression::string() const {
    std::ostringstream out;

    out << "(" << left->string() << " " +  oper + " " << right->string() << ")";

    return out.str();
}

std::string IfExpression::string() const {
    std::ostringstream out;

    out << "if " << condition->string() << " " << consequence->string();

    if (alternative != nullptr) {
        out << "else " << alternative->string();
    }

    return out.str();
}

std::string FunctionLiteral::string() const {
    std::ostringstream out;

    out << tokenLiteral() << "(";

    std::vector<std::string> params;

    for (const auto& param : parameters)
        params.push_back(param->string());

    for (int i = 0; i < params.size(); i++) {
        out << params[i];

        if (i < params.size() - 1)
            out << ",";
    }

    out << ")";
    out << body->string();

    return out.str();
}
