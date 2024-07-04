#pragma once
#include <set>
#include <iostream>
//#include <sstream>
#include "AST.h"

std::set<std::string> c_infix_ops = {
  "<",
  ">",
  "+",
  "-",
  "*",
  "/",
  "^",
};

bool emit_literal(SyntaxNode *node) {
  std::cout << node->token->literal;
  return true;
}

bool emit(SyntaxNode *node) {
  if (node->children.empty()) {
    emit_literal(node);
  } else if (c_infix_ops.contains(node->token->literal)) {
    if (node->children.size() != 2) {
      std::cerr << "<CEmitter> Error: infix operator " << node->token->literal << " must have exactly 2 children\n";
      return false;
    }
    std::cout << "(";
    emit(node->children[0]);
    std::cout << ") " << node->token->literal << " (";
    emit(node->children[1]);
    std::cout << ")";
  } else {
    std::cerr << "<CEmitter> Error: unhandled node type " << node->token->literal << "\n";
    return false;
  }
  return true;
}
