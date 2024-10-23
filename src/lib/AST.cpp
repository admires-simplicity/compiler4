#include "AST.h"
#include "types.h"
#include "bimap.h"

// Bimap<SyntaxNode::NodeType, std::string> SyntaxNode::NodeType_repr = {
//   {NodeType::literal, "literal"},
//   {NodeType::identifier, "identifier"},
//   {NodeType::value, "value"},
//   {NodeType::fn_def, "fn_def"},
//   {NodeType::apply, "apply"},
//   {NodeType::block, "block"}, 
//   {NodeType::program_block, "program_block"},

//   {NodeType::arg_list, "arg_list"},

//   {NodeType::add, "+"},
// };

// std::unordered_map<SyntaxNode::NodeType, Type>& SyntaxNode::get_NodeType_to_Type_map() {
//   static std::unordered_map<SyntaxNode::NodeType, Type> NodeType_to_Type {
//     {SyntaxNode::NodeType::add, TypeIdList{"num", "num", "num"}},
//   };
//   return NodeType_to_Type;
// }

// Type SyntaxNode::NodeType_to_Type(SyntaxNode::NodeType type) {
//   return get_NodeType_to_Type_map()[type];
// }

// void SyntaxNode::set_type() {
//   if (type != NodeType::literal) {
//     val_type = NodeType_to_Type(type);
//   }
// }

inline std::string ValueNode::name() {
  return "ValueNode";
}

inline std::string ApplyNode::name() {
  return "ApplyNode";
}

inline std::string BlockNode::name() {
  return "BlockNode";
}

inline std::string LetNode::name() {
  return "LetNode";
}

inline std::string FnDefNode::name() {
  return "FnDefNode";
}

std::string ValueNode::to_string() {
  return token->literal;
}

std::string ApplyNode::to_string() {
  std::string s = "(" + fn->to_string();
  for (auto arg : args) {
    s += " " + arg->to_string();
  }
  s += ")";
  return s;
}

std::string BlockNode::to_string() {
  std::string s = "(block ";
  for (auto child : children) {
    s += child->to_string() + " ";
  }
  s.pop_back();
  s += ")";
  return s;
}

std::string LetNode::to_string() {
  return "(let " + ident->to_string() + " " + value->to_string() + ")";
}

std::string FnDefNode::to_string() {
  std::string s = "(fn_def " + ident->to_string();
  for (auto arg : args) {
    s += " " + arg->to_string();
  }
  s += " " + block->to_string() + ")";
  return s;
}

SyntaxNode *ApplyNode::operator[](int i) {
  return args[i];
}

SyntaxNode *BlockNode::operator[](int i) {
  return children[i];
}
