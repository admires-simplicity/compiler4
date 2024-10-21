#pragma once
#include <vector>
#include <string>
#include <map>

#include "lexer.h"
#include "types.h"
#include "options.h"
#include "bimap.h"

// class SyntaxNode {
// public:
//   enum class NodeType {
//     literal,
//     identifier,
//     value,
//     fn_def,
//     apply,
//     block,
//     program_block,

//     arg_list,

//     add,
//   };

//   static Bimap<NodeType, std::string> NodeType_repr;

//   std::unordered_map<SyntaxNode::NodeType, Type>& get_NodeType_to_Type_map();
//   Type NodeType_to_Type(NodeType type);



//   NodeType type = NodeType::literal;
//   Token *token;
//   Type val_type; // int -- value, TypeIdList -- function // TODO: think about how to actually typecheck Types
//   std::vector<SyntaxNode *> children;

//   SyntaxNode(std::string string) : token(new Token(string)) {}
//   SyntaxNode(std::string string, std::vector<SyntaxNode *> children) : token(new Token(string)), children(children) {}
  
//   SyntaxNode(Token *token) : token(token) {}
//   SyntaxNode(Token *token, std::vector<SyntaxNode *> children) : token(token), children(children) {}
//   //SyntaxNode(Token *token, Type type) : token(token), val_type(type) {}

//   SyntaxNode(NodeType type) : type(type), val_type(NodeType_to_Type(type)) {}
//   SyntaxNode(NodeType type, std::vector<SyntaxNode *> children) : type(type), children(children), val_type(NodeType_to_Type(type)) {}

//   SyntaxNode(Type type) : val_type(type) {}

//   ~SyntaxNode() {
//     delete token;
//     for (auto child : children) {
//       delete child;
//     }
//   }

//   std::string type_name() {
//     return NodeType_repr[type];
//   }

//   void set_type();

//   /*
//    * max_inline_depth -- maximum depth of a node which for its children to be
//    *                     printed inline. if max_inline_depth = 1, only childless
//    *                     nodes will be printed inline.
//    */
//   std::string to_string(bool pretty = false, bool display_type = false,  int max_inline_depth = 1, int indent = 0) {
//     std::string s = "";
//     if (pretty && indent) {
//       s += "\n";
//       for (int i = 0; i < indent; i++) s += "  ";
//     }

//     if (children.size() == 0) {
//       s += node_signature(display_type);
//       return s;
//     }
    
//     s += "(";
//     s += node_signature(display_type);

//     int md = (pretty) ? max_depth(this) : 0; // max depth is slow.
//     // what would be better is to memoize the depth of every node whenever
//     // to_string is called by an external function.

//     for (auto child : children) {
//       s += " ";
//       s += (pretty && md > max_inline_depth) ? child->to_string(pretty, display_type, max_inline_depth, indent+1) : child->to_string(pretty, display_type);
//     }
//     s += ")";
//     return s;
//   }

// private:
//   int max_depth(SyntaxNode *node) {
//     if (node->children.size() == 0) return 0;
//     int max = 0;
//     for (auto child : node->children) {
//       int depth = max_depth(child);
//       if (depth > max) max = depth;
//     }
//     return max + 1;
//   }

//   bool has_type() { // TODO: sort of hacky
//     switch (type) {
//       case NodeType::literal:
//       case NodeType::fn_def:
//         return true;
//       default:
//         return false;
//     }
//   }

//   std::string node_signature(bool display_type = false) {
//     return
//       ((display_type) ? (type_print_repr(val_type) + ":") : "")
//         +
//       ((type == NodeType::literal) ? token->literal : NodeType_repr[type]);
//   }

// };

class SyntaxNode {
public:
  virtual std::string to_string() = 0;
};

class ValueNode : public SyntaxNode {
public:
  Type type = TypeSet::get_id("unassigned type");
  Token *token;
  ValueNode(Type type, Token *token) : type(type), token(token) {}
  ValueNode(Token* token) : token(token) {}

  std::string to_string() {
    return token->literal;
  }
};

class IdentNode : public ValueNode {
public:
  IdentNode(Type type, Token *token) : ValueNode(type, token) {}
  IdentNode(Token *token) : ValueNode(TypeSet::get_id("dynamic"), token) {}
};

class ArgListNode : public SyntaxNode {
public:
  std::vector<SyntaxNode *> args;
  ArgListNode(std::vector<SyntaxNode *> args) : args(args) {}

  std::string to_string() override {
    std::string s;
    for (auto arg : args) {
      s += arg->to_string() + " ";
    }
    if (s.size()) s.pop_back();
    return s;
  }
};

class ApplyNode : public SyntaxNode {
public:
  SyntaxNode *fn;
  ArgListNode *args;
  ApplyNode(SyntaxNode *fn, ArgListNode *args) : fn(fn), args(args) {}

  std::string to_string() override {
    std::string s = "(" + fn->to_string();
    if (args) s += " " + args->to_string();
    s += ")";
    return s;
  }
};

class BlockNode : public SyntaxNode {
public:
  std::vector<SyntaxNode *> children;
  BlockNode(std::vector<SyntaxNode *> children) : children(children) {}

  std::string to_string() override {
    std::string s = "(block ";
    for (auto child : children) {
      s += child->to_string() + " ";
    }
    s.pop_back();
    s += ")";
    return s;
  }
};

class LetNode : public SyntaxNode {
public:
  IdentNode *ident;
  ValueNode *value;
  LetNode(IdentNode *ident, ValueNode *value) : ident(ident), value(value) {}

  std::string to_string() override {
    return "(let " + ident->to_string() + " " + value->to_string() + ")";
  }
};

class FnDefNode : public SyntaxNode {
public:
  IdentNode *ident;
  Type type = TypeSet::get_id("dynamic");
  ArgListNode *args;
  BlockNode *block;
  FnDefNode(IdentNode *ident, ArgListNode *args, BlockNode *block) : ident(ident), args(args), block(block) {}
  FnDefNode(IdentNode *ident, Type type, ArgListNode *args, BlockNode *block) : ident(ident), type(type), args(args), block(block) {}

  std::string to_string() override {
    return "(fn_def " + ident->to_string() + " " + args->to_string() + " " + block->to_string() + ")";
  }
};