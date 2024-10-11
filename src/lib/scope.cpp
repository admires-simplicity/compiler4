

#include "types.h"
#include "scope.h"

Scope global_scope {
  {"+", new SyntaxNode(TypeIdList{"int", "int", "int"})},
  {"test_v", new SyntaxNode(1)},
};

bool Scope::add(std::string name, SyntaxNode* type) {
  if (exists(name)) return false;
  scope[name] = type;
  return true;
}

std::optional<SyntaxNode*> Scope::get(std::string name) {
  auto it = scope.find(name);
  if (it != scope.end()) return it->second;
  else if (parent != nullptr) return parent->get(name);
  else return std::nullopt;
}

bool Scope::exists(std::string name) {
  return get(name) != std::nullopt;
}