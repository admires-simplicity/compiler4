#include <cassert>
#include <iostream>
#include <optional>
#include <deque>
#include <functional>

#include "compiler.h"
#include "scope.h"
#include "util.h"

bool panic = false;

bool to_returning_segment(SyntaxNode *node);

class ReturningSegmentVisitor : public SyntaxNodeVisitor {
  bool res = true;

  bool Stmt_to_return(StmtNode *node) {
    bool is_return = to_returning_segment(node->expr);
    if (!is_return) {
      node->expr = new ReturnNode(node->expr);
    }
    return is_return;
  }

public:
  bool get_result() { return res; }

  void visit(StmtNode *node) override {
    res = Stmt_to_return(node);
  }

  void visit(ReturnNode *node) override {}

  void visit(ValueNode *node) override {
    res = false; // cannot turn a value node into a return node inside the visitor
  }

  void visit(ApplyNode *node) override {
    if (node->fn->to_string() == "else") {
      node->args[0]->accept(*this);
      node->args[1]->accept(*this);
    }
    else if (node->fn->to_string() == "then") {
      node->args[1]->accept(*this);
    } else {    
      res = false; // cannot turn an apply node into a return node inside the visitor
    }
  }

  void visit(BlockNode *node) override {
    to_returning_segment(node->children.back());
  }

  void visit(ProgramBlockNode *node) override {
    for (auto *child : node->children) {
      visit(child);
    }
  }

  void visit(LetNode *node) override {
    res = false; // cannot turn a let node into a return node inside the visitor
  }

  void visit(FnDefNode *node) override {
    res = false; // cannot turn a fn def node into a return node inside the visitor
  }
};

bool to_returning_segment(SyntaxNode *node) {
  ReturningSegmentVisitor v;
  node->accept(v);
  return v.get_result();
}


bool format_fn(FnDefNode* node, bool is_main = false) {
  // SyntaxNode* block_node = node->block;
  // to_returning_segment(block_node);
  //node->block = to_returning_segment(node->block);

  if (!is_main) to_returning_segment(node->block);

  return true;
}

SyntaxNode *compile(SyntaxNode *node) {
  BlockNode* program_statements = dynamic_cast<BlockNode*>(node);
  if (program_statements == nullptr) {
    std::cerr << "<Compiler> Error: expected BlockNode, got " << node->name() << "\n";
    return nullptr;
  }

  ProgramBlockNode *program = new ProgramBlockNode();
  std::vector<FnDefNode *> function_decls;
  
  BlockNode* main_block = new BlockNode();
  FnDefNode* main_fn = new FnDefNode(new ValueNode(new CompositeType({new AtomicType("int")}), new Token("main")), {}, main_block);
  // TODO: we'll have to change this later if we decide to support command line arguments

  for (int i = 0; i < program_statements->children.size(); ++i) {
    StmtNode* child = program_statements->children[i];
    if (FnDefNode *fn = dynamic_cast<FnDefNode*>(child->expr)) {
      format_fn(fn);
      program->children.push_back(fn);
    }
    else {
      main_block->children.push_back(child);
    }
  }
  format_fn(main_fn, true);

  program->children.push_back(main_fn);

  // bool well_formed = semantic_analysis(program, global_scope);
  // if (!well_formed) {
  //     std::cerr << "<Compiler> Error: type error found -- aborting compilation\n";
  //     // TODO: delete node
  //     return nullptr;
  // }

  // TODO: delete node
  return program;   
}