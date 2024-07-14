#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <cstdint>
#include <optional>
#include <sstream>

#include "lexer.h"
#include "parser.h"
#include "AST.h"
#include "Cemitter.h"

int main() {
    Lexer lexer(std::cin);

    SyntaxNode *parsed = Parser(lexer).parse();
    std::cout << "\nparsed: " << std::endl;
    if (parsed) std::cout << parsed->to_string() << std::endl;
    else std::cout << "null" << std::endl;

    if (parsed) emit(parsed);

    return 0;
}