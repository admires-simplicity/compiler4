#include "AST.h"
#include "types.h"
#include "bimap.h"

Bimap<SyntaxNode::NodeType, std::string> SyntaxNode::NodeType_repr = {
  {NodeType::literal, "literal"},
  {NodeType::identifier, "identifier"},
  {NodeType::value, "value"},
  {NodeType::apply, "apply"},
  {NodeType::block, "block"}, 
  {NodeType::program_block, "program_block"},

  {NodeType::add, "+"},
};

std::unordered_map<SyntaxNode::NodeType, Type>& SyntaxNode::get_NodeType_to_Type_map() {
  static std::unordered_map<SyntaxNode::NodeType, Type> NodeType_to_Type {
    {SyntaxNode::NodeType::add, TypeIdList{"num", "num", "num"}},
  };
  return NodeType_to_Type;
}

Type SyntaxNode::NodeType_to_Type(SyntaxNode::NodeType type) {
  return get_NodeType_to_Type_map()[type];
}

void SyntaxNode::set_type() {
  if (type != NodeType::literal) {
    val_type = NodeType_to_Type(type);
  }
}