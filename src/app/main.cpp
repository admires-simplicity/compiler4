#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <cstdint>
#include <optional>
#include <sstream>
#include <fstream>
#include <cctype>

#include "util.h"
#include "lexer.h"
#include "parser.h"
#include "AST.h"
#include "compiler.h"
#include "Cemitter.h"

const int PARSE_TREE_ONLY = 1;
const int PRETTY_PARSE_TREE = 2;
const int PARSE_TREE_TYPES = 4;

int64_t read_flags(int argc, char** argv) {
  int64_t flags = 0;
  for (int i = 1; i < argc; i++) {
    std::string arg = std::string(argv[i]);
    if (arg.size() >= 1 && arg[0] == '-') {
      if (arg.size() >= 2 && std::tolower(arg[1]) == 'p') {
        flags |= PARSE_TREE_ONLY;
        if (arg[1] == 'P') flags |= PRETTY_PARSE_TREE;
        if (arg.size() >= 3 && arg[2] == 't') flags |= PARSE_TREE_TYPES;
      }
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
        if (parsed) std::cout << parsed->to_string(flags & PRETTY_PARSE_TREE, flags & PARSE_TREE_TYPES) << std::endl;
        return 0;
    }

    if (parsed) {
        SyntaxNode *compiled = compile(parsed);
        // std::cout << std::endl << "emitted:" << std::endl;
        emit(compiled);
    }

    return 0;
}