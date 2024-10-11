#pragma once
#include <vector>
#include <string>
#include <map>

#include "lexer.h"
#include "types.h"
#include "options.h"
#include "bimap.h"

class SyntaxNode {
public:
  enum class NodeType {
    literal,
    identifier,
    value,
    fn_def,
    apply,
    block,
    program_block,

    arg_list,

    add,
  };

  static Bimap<NodeType, std::string> NodeType_repr;

  std::unordered_map<SyntaxNode::NodeType, Type>& get_NodeType_to_Type_map();
  Type NodeType_to_Type(NodeType type);



  NodeType type = NodeType::literal;
  Token *token;
  Type val_type; // int -- value, TypeIdList -- function // TODO: think about how to actually typecheck Types
  std::vector<SyntaxNode *> children;

  SyntaxNode(std::string string) : token(new Token(string)) {}
  SyntaxNode(std::string string, std::vector<SyntaxNode *> children) : token(new Token(string)), children(children) {}
  
  SyntaxNode(Token *token) : token(token) {}
  SyntaxNode(Token *token, std::vector<SyntaxNode *> children) : token(token), children(children) {}
  //SyntaxNode(Token *token, Type type) : token(token), val_type(type) {}

  SyntaxNode(NodeType type) : type(type), val_type(NodeType_to_Type(type)) {}
  SyntaxNode(NodeType type, std::vector<SyntaxNode *> children) : type(type), children(children), val_type(NodeType_to_Type(type)) {}

  SyntaxNode(Type type) : val_type(type) {}

  ~SyntaxNode() {
    delete token;
    for (auto child : children) {
      delete child;
    }
  }

  std::string type_name() {
    return NodeType_repr[type];
  }

  void set_type();

  /*
   * max_inline_depth -- maximum depth of a node which for its children to be
   *                     printed inline. if max_inline_depth = 1, only childless
   *                     nodes will be printed inline.
   */
  std::string to_string(bool pretty = false, bool display_type = false,  int max_inline_depth = 1, int indent = 0) {
    std::string s = "";
    if (pretty && indent) {
      s += "\n";
      for (int i = 0; i < indent; i++) s += "  ";
    }

    if (children.size() == 0) {
      s += node_signature(display_type);
      return s;
    }
    
    s += "(";
    s += node_signature(display_type);

    int md = (pretty) ? max_depth(this) : 0; // max depth is slow.
    // what would be better is to memoize the depth of every node whenever
    // to_string is called by an external function.

    for (auto child : children) {
      s += " ";
      s += (pretty && md > max_inline_depth) ? child->to_string(pretty, display_type, max_inline_depth, indent+1) : child->to_string(pretty, display_type);
    }
    s += ")";
    return s;
  }

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

  bool has_type() { // TODO: sort of hacky
    switch (type) {
      case NodeType::literal:
      case NodeType::fn_def:
        return true;
      default:
        return false;
    }
  }

  std::string node_signature(bool display_type = false) {
    return
      ((display_type) ? (type_print_repr(val_type) + ":") : "")
        +
      ((type == NodeType::literal) ? token->literal : NodeType_repr[type]);
  }

};