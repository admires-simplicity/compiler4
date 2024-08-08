#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <cstdint>
#include <optional>
#include <sstream>
#include <fstream>

#include "lexer.h"
#include "parser.h"
#include "AST.h"
#include "compiler.h"
#include "Cemitter.h"

std::istringstream *filename_to_str(std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return nullptr;
    
    std::stringstream buf;
    buf << "{\n" << file.rdbuf() << "\n}"; // interpret as block
    return new std::istringstream(buf.str());
}

int main(int argc, char** argv) {
    Lexer lexer((argc == 2) ? *filename_to_str(argv[1]) : std::cin);

    SyntaxNode *parsed = Parser(lexer).parse();
    // std::cout << "\nparsed: " << std::endl;
    // if (parsed) std::cout << parsed->to_string() << std::endl;
    // else std::cout << "null" << std::endl;

    if (parsed) {
        SyntaxNode *compiled = compile(parsed);
        // std::cout << std::endl << "emitted:" << std::endl;
        emit(compiled);
    }

    return 0;
}