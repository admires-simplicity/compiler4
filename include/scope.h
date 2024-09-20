#pragma once

#include <map>
#include <string>
#include <optional>

#include "AST.h"
#include "types.h"

Scope global_scope;

class Scope {
public:
  Scope *parent;
  std::map<std::string, SyntaxNode*> scope; // TODO: if I change SyntaxNode into a
  // class hierarchy, I could just do map<string, Identifier*> so this would be clearer

  Scope(std::initializer_list<std::pair<const std::string, SyntaxNode*>> list) : scope(list) {}
  Scope(std::initializer_list<std::pair<const std::string, SyntaxNode*>> list, Scope* parent) : scope(list), parent(parent) {}

  bool add(std::string name, SyntaxNode* value);
  std::optional<SyntaxNode*> get(std::string name);
  bool exists(std::string name);
};