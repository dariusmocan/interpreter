#include <string>
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
