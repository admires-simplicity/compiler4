#pragma once
#include "token.h"



class SyntaxNode {
public:
  enum class Type {
    identifier,
    literal,
    apply,
  };

  Token *token;
  std::vector<SyntaxNode *> children;

  SyntaxNode(Token *token) : token(token) {}
  SyntaxNode(Token *token, std::vector<SyntaxNode *> children) : token(token), children(children) {}

  std::string to_string() {
    std::string s = "";
    s += "(";
    s += token->literal;
    for (auto child : children) {
      s += " ";
      s += child->to_string();
    }
    s += ")";
    return s;
  }
};