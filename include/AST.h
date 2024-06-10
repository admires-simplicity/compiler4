#pragma once
#include "lexer.h"


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
  ~SyntaxNode() {
    delete token;
    for (auto child : children) {
      delete child;
    }
  }

  std::string to_string() {
    std::string s = "";
    s += "(";
    s += canonical_token_repr();
    for (auto child : children) {
      s += " ";
      s += child->to_string();
    }
    s += ")";
    return s;
  }

private:
  std::string canonical_token_repr() {
    if (token->literal == "\n") return "newline";
    else return token->literal;
  }
};