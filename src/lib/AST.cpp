#include <algorithm>

#include "AST.h"
#include "types.h"
#include "bimap.h"
#include "options.h"
#include "util.h"

int depth(SyntaxNode *node);
int indent = 0;

BlockNode::BlockNode(std::vector<SyntaxNode *> children) {
  for (auto child : children) {
    if (StmtNode *stmt = dynamic_cast<StmtNode*>(child)) {
      this->children.push_back(stmt);
    } else {
      this->children.push_back(new StmtNode(child));
    }
  }
}

void StmtNode::accept(SyntaxNodeVisitor &v) {
  v.visit(this);
}

void ReturnNode::accept(SyntaxNodeVisitor &v) {
  v.visit(this);
}

void ValueNode::accept(SyntaxNodeVisitor &v) {
  v.visit(this);
}

void ApplyNode::accept(SyntaxNodeVisitor &v) {
  v.visit(this);
}

void BlockNode::accept(SyntaxNodeVisitor &v) {
  v.visit(this);
}

void ProgramBlockNode::accept(SyntaxNodeVisitor &v) {
  v.visit(this);
}

void LetNode::accept(SyntaxNodeVisitor &v) {
  v.visit(this);
}

void FnDefNode::accept(SyntaxNodeVisitor &v) {
  v.visit(this);
}

inline std::string StmtNode::name() {
  return "stmt";
}

inline std::string ReturnNode::name() {
  return "return";
}

inline std::string ValueNode::name() {
  return "value";
}

inline std::string ApplyNode::name() {
  return "apply";
}

inline std::string BlockNode::name() {
  return "block";
}

inline std::string LetNode::name() {
  return "let";
}

inline std::string FnDefNode::name() {
  return "fn_def";
}

int max_depth(std::vector<SyntaxNode*>& nodes) {
  int d = 0;
  for (auto node : nodes) {
    d = std::max(d, depth(node));
  }
  return d;
}

//template <typename T>
std::string to_string_internal(std::vector<SyntaxNode* /*T*/> nodes, bool _first_no_sep = false) {
  int i = 0;
  bool pretty = pretty_print && max_depth(nodes) > pretty_print_max_depth;

  std::string sep;
  if (pretty) {
    indent += 2;
    sep = "\n";
    sep += std::string(indent, ' ');
  } else {
    sep = " ";
  }

  std::string s;

  auto it = nodes.begin();
  if (_first_no_sep) { // used only for ValueNodes, so that we can display the funciton
  // name in ApplyNodes
    s += (*it)->to_string();
    ++it;
  }

  for (; it != nodes.end(); ++it) {
    s += sep + (*it)->to_string();
  }

  if (pretty) indent = indent - 2;
  return s;
}

std::string StmtNode::to_string() {
  return "(;" + to_string_internal({expr}) + ")";
}

std::string ReturnNode::to_string() {
  return "(" + this->name() + to_string_internal({expr}) + ")";
}

std::string ValueNode::to_string() {
  return (display_types)
    ? type->to_string() + ":" + token->literal
    : token->literal;
}

std::string ApplyNode::to_string() {
  return "(" +
  to_string_internal(add_vec<SyntaxNode*>(std::vector<SyntaxNode*>({fn}), args), true)
  + ")";
}

std::string BlockNode::to_string() {
  return "(" + this->name() + to_string_internal(std::vector<SyntaxNode*>(children.begin(), children.end())) + ")";
}

std::string LetNode::to_string() {
  return "(" + this->name() + to_string_internal({ident, value}) + ")";
}

std::string FnDefNode::to_string() {
  return "(" + this->name() +
  to_string_internal(add_vec<SyntaxNode*>(std::vector<SyntaxNode*>{ident}, add_vec<SyntaxNode*>(this->args, std::vector<SyntaxNode*>{block})))
  + ")";
}

SyntaxNode *ApplyNode::operator[](int i) {
  return args[i];
}

SyntaxNode *BlockNode::operator[](int i) {
  return children[i];
}

class DepthVisitor : public SyntaxNodeVisitor {
  int res = 1;
public:
  int result() {
    return res;
  }
  
  void visit(ValueNode *node) override {} // do nothing

  void visit(StmtNode *node) override {
    res = 1 + depth(node->expr);
  }

  void visit(ReturnNode *node) override {
    res = 1 + depth(node->expr);
  }
  
  void visit(ApplyNode *node) override { // doesn't check fn
    int d = 0;
    for (auto child : node->args) {
      d = std::max(d, depth(child));
    }
    res = d + 1;
  }

  void visit(BlockNode *node) override {
    int d = 0;
    for (auto child : node->children) {
      d = std::max(d, depth(child));
    }
    res = d + 1;
  }

  void visit(ProgramBlockNode *node) override {
    visit(static_cast<BlockNode*>(node));
  }

  void visit(LetNode *node) override {
    int d = std::max(depth(node->ident), depth(node->value));
    res = d + 1;
  }

  void visit(FnDefNode *node) override {
    int d = 0;
    for (auto arg : node->args) {
      d = std::max(d, depth(arg));
    }
    d = std::max(d, depth(node->block));
    res = d + 1;
  }
};

int depth(SyntaxNode *node) {
  DepthVisitor v;
  node->accept(v);
  return v.result();
}