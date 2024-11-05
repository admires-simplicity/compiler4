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
  // TypeIdList t = TypeIdList{"int", "int", "int"};
  //Type t = CompositeType(std::variant<int, std::string, Type>("int", "int", "int"));
  Type *t1 = new AtomicType("int");
  Type *t2 = new CompositeType{t1};
  Type *t3 = new CompositeType{std::variant<int, std::string, Type*>("int")};
  Type *t4a = new CompositeType{"int", 1, new AtomicType("int")};
  Type *t4b = new CompositeType{{"int", 1, new AtomicType("int")}};
  Type *t5 = new CompositeType{"int", "int", "int"};
  CompositeType t6 = CompositeType{"int", "int", "int"};


  flags = read_flags(argc, argv);

  Lexer lexer((argc > 1) ? *filename_to_str(argv[argc-1]) : std::cin);

  // parsing
  SyntaxNode *parsed = Parser(lexer).parse();
  if (parsed == nullptr) {
    std::cout << "parsing failed" << std::endl;
    return 0;
  }

  if (flags & PARSE_TREE_ONLY) {
    std::cout << parsed->to_string(/*flags & PRETTY_PARSE_TREE, flags & PARSE_TREE_TYPES, 1*/) << std::endl;
    return 0;
  }

  // compile
  SyntaxNode *compiled = compile(parsed);
  if (compiled == nullptr) {
    std::cout << "compilation failed" << std::endl;
    return 0;
  }

  if (flags & OUTPUT_IR) {
    std::cout << compiled->to_string(/*true, true, 1*/) << std::endl;
    return 0;
  }

  if (flags & DO_NOT_EMIT) {
    return 0;
  }

  // emit
  emit_program(compiled);

  return 0;
}