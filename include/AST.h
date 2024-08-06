#pragma once
#include <vector>
#include <string>

#include "lexer.h"


class SyntaxNode {
public:
  // enum class Type {
  //   identifier,
  //   literal,
  // };

  Token *token;
  std::vector<SyntaxNode *> children;

  SyntaxNode(std::string string) : token(new Token(string)) {}
  SyntaxNode(std::string string, std::vector<SyntaxNode *> children) : token(new Token(string)), children(children) {}
  
  SyntaxNode(Token *token) : token(token) {}
  SyntaxNode(Token *token, std::vector<SyntaxNode *> children) : token(token), children(children) {}

  SyntaxNode(Token::Type type, std::vector<SyntaxNode *> children) : token(new Token(type)), children(children) {}

  ~SyntaxNode() {
    delete token;
    for (auto child : children) {
      delete child;
    }
  }

  std::string to_string() {
    if (children.size() == 0) return canonical_token_repr();
    
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