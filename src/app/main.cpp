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

#include "options.h"

int main(int argc, char** argv) {  
  Type t = TypeIdList{"int", "int", "int"};

  flags = read_flags(argc, argv);

  Lexer lexer((argc > 1) ? *filename_to_str(argv[argc-1]) : std::cin);

  SyntaxNode *parsed = Parser(lexer).parse();

  if (flags & PARSE_TREE_ONLY) {

    if (parsed) std::cout << parsed->to_string(flags & PRETTY_PARSE_TREE, flags & PARSE_TREE_TYPES, 1) << std::endl;
    else std::cout << "parsing failed" << std::endl;
    return 0;
  }

  if (parsed) {
    SyntaxNode *compiled = compile(parsed);
    emit(compiled);
  }

  return 0;
}