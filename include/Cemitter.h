#pragma once
#include <set>
#include <iostream>
//#include <sstream>

#include "AST.h"
#include "lexer.h"

bool emit(SyntaxNode *node);

std::set<std::string> c_infix_ops = {
  "<",
  ">",
  "+",
  "-",
  "*",
  "/",
  "^",
};

std::map<std::string, std::string> c_std_include_map = {
  {"printf", "<stdio.h>"},
};

bool emit_literal(SyntaxNode *node) {
  std::cout << node->token->literal;
  return true;
}

void emit_block(SyntaxNode *node) {
  for (auto child : node->children) {
    emit(child);
    //std::cout << ";\n";
  }
}

bool emit(SyntaxNode *node) {
  if (!node) {
    std::cerr << "<CEmitter> Error: null node\n";
    return false;
  }
  if (!node->token) {
    std::cerr << "<CEmitter> Error: null token\n";
    return false;
  }

  //std::cout << "\"" << node->token->token_type_repr[node->token->type] << ": " << node->token->literal << "\"\n";

  switch (node->token->type) {
    case Token::Type::identifier:
      // "syntax" types will have to be here for now
      if (node->token->literal == ";") {
        emit(node->children[0]);
        std::cout << ";\n";
      } else if (node->token->literal == ",") {
        emit(node->children[0]);
        std::cout << ", ";
        emit(node->children[1]);
      } else {
        return emit_literal(node); // same as Type::number
      }
      break;
    case Token::Type::number:
      return emit_literal(node);
    case Token::Type::apply:
      // std::cout << node->token->literal << "(";
      // emit(node->children[0]);
      // std::cout << ")";
      emit(node->children[0]);
      std::cout << "(";
      emit(node->children[1]);
      std::cout << ")";
      break;
    case Token::Type::block:
      std::cout << "{\n";
      emit_block(node);
      std::cout << "}\n";
      break;
    case Token::Type::program_block:
      emit_block(node);
      break;
    default:
      std::cerr << "<CEmitter> Error: unhandled node type " << node->token->literal << "\n";
      return false;
  }

  // if (node->children.empty()) {
  //   emit_literal(node);
  // } else if (c_infix_ops.contains(node->token->literal)) {
  //   if (node->children.size() != 2) {
  //     std::cerr << "<CEmitter> Error: infix operator " << node->token->literal << " must have exactly 2 children\n";
  //     return false;
  //   }
  //   std::cout << "(";
  //   emit(node->children[0]);
  //   std::cout << ") " << node->token->literal << " (";
  //   emit(node->children[1]);
  //   std::cout << ")";
  // } else {
  //   std::cerr << "<CEmitter> Error: unhandled node type " << node->token->literal << "\n";
  //   return false;
  // }
  return true;
}
