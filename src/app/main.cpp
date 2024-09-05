#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <cstdint>
#include <optional>
#include <sstream>
#include <fstream>

#include "util.h"
#include "lexer.h"
#include "parser.h"
#include "AST.h"
#include "compiler.h"
#include "Cemitter.h"

const int PARSE_TREE_ONLY = 1;

int64_t read_flags(int argc, char** argv) {
  int64_t flags = 0;
  for (int i = 1; i < argc; i++) {
    if (std::string(argv[i]) == "-p") {
      flags |= PARSE_TREE_ONLY;
    }
  }
  return flags;
}


int main(int argc, char** argv) {
    Lexer lexer((argc > 1) ? *filename_to_str(argv[argc-1]) : std::cin);

    int64_t flags = read_flags(argc, argv);

    SyntaxNode *parsed = Parser(lexer).parse();
    // std::cout << "\nparsed: " << std::endl;
    // if (parsed) std::cout << parsed->to_string() << std::endl;
    // else std::cout << "null" << std::endl;

    if (flags & PARSE_TREE_ONLY) {
        if (parsed) std::cout << parsed->to_string() << std::endl;
        return 0;
    }

    if (parsed) {
        SyntaxNode *compiled = compile(parsed);
        // std::cout << std::endl << "emitted:" << std::endl;
        emit(compiled);
    }

    return 0;
}