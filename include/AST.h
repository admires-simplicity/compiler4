#pragma once
#include <vector>
#include <string>
#include <map>

#include "lexer.h"
#include "types.h"

class SyntaxNode {
private:
  int max_depth(SyntaxNode *node) {
    if (node->children.size() == 0) return 0;
    int max = 0;
    for (auto child : node->children) {
      int depth = max_depth(child);
      if (depth > max) max = depth;
    }
    return max + 1;
  }

  // std::string val_type_repr() {
  //   std::string res;
  //   if (std::holds_alternative<int>(val_type)) res = std::to_string(std::get<int>(val_type));
  //   else {
  //     res = "(";
  //     TypeIdList& type_id_list = std::get<TypeIdList>(val_type);
  //     for (auto& vt : type_id_list) {

  //     }
  //   }
  // }

  std::string val_type_repr() {
    if (std::holds_alternative<int>(val_type)) return std::to_string(std::get<int>(val_type));
    else return std::get<TypeIdList>(val_type).to_string();
  }
  
  std::string canonical_token_repr(bool display_type = false) { 
    if (type == Type::literal) {
      if (display_type) return val_type_repr() + ":" + token->literal;
      else return token->literal;
    }
    else return token_type_repr[type];
  }

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
  std::variant<int, TypeIdList> val_type; // int -- value, TypeIdList -- function
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

  /*
   * max_inline_depth -- maximum depth of a node which for its children to be
   *                     printed inline. if max_inline_depth = 1, only childless
   *                     nodes will be printed inline.
   */
  std::string to_string(bool pretty = false, bool display_type = false, int indent = 0, int max_inline_depth = 1) {
    std::string s = "";
    if (pretty && indent) {
      s += "\n";
      for (int i = 0; i < indent; i++) s += "  ";
    }

    if (children.size() == 0) {
      s += canonical_token_repr(display_type);
      return s;
    }
    
    s += "(";
    s += canonical_token_repr(display_type);

    int md = (pretty) ? max_depth(this) : 0; // max depth is slow.

    for (auto child : children) {
      s += " ";
      s += (pretty && md > max_inline_depth) ? child->to_string(pretty, display_type, indent+1) : child->to_string(pretty, display_type);
    }
    s += ")";
    return s;
  }
};