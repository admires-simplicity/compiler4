#pragma once
#include <vector>
#include <string>

#include "lexer.h"


class SyntaxNode {
public:
  enum class Type {
    literal,
    identifier,
    value,
    apply,
    block,
    program_block,
  };

  std::map<Type, std::string> token_type_repr {
    {Type::literal, "literal"},
    {Type::identifier, "identifier"},
    {Type::value, "value"},
    {Type::apply, "apply"},
    {Type::block, "block"}, 
    {Type::program_block, "program_block"},
  };

  Type type = Type::literal;
  Token *token;
  std::vector<SyntaxNode *> children;

  SyntaxNode(std::string string) : token(new Token(string)) {}
  SyntaxNode(std::string string, std::vector<SyntaxNode *> children) : token(new Token(string)), children(children) {}
  
  SyntaxNode(Token *token) : token(token) {}
  SyntaxNode(Token *token, std::vector<SyntaxNode *> children) : token(token), children(children) {}

  SyntaxNode(Type type) : type(type) {}
  SyntaxNode(Type type, std::vector<SyntaxNode *> children) : type(type), children(children) {}

  ~SyntaxNode() {
    delete token;
    for (auto child : children) {
      delete child;
    }
  }

  std::string type_name() {
    return token_type_repr[type];
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
    if (type == Type::literal) return token->literal;
    else return token_type_repr[type];
  }
};