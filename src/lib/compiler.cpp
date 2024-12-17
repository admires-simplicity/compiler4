#include <cassert>
#include <iostream>
#include <optional>
#include <deque>
#include <functional>

#include "compiler.h"
#include "scope.h"
#include "util.h"

bool panic = false;

bool is_fn_def(SyntaxNode *node) {
  StmtNode* stmt = dynamic_cast<StmtNode*>(node);
  if (stmt == nullptr) return false;
  ApplyNode* equals = dynamic_cast<ApplyNode*>(stmt->expr);
  if (equals == nullptr) return false;
  if (equals->fn->to_string() != "=") return false; // TODO: I'm not sure I like this
  ApplyNode* arrow = dynamic_cast<ApplyNode*>(equals->args[0]);
  if (arrow == nullptr) return false;
  if (arrow->fn->to_string() != "->") return false;
  return true;
}

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

Type *infer_fn_type(SyntaxNode* node) {
  // we expect infer_fn_type is passed an equals node like
  // (=
  //   (->
  //     (f (: <arg1_name> <arg1_type>) ...)
  //     <rtype>)
  //   <function_block>)
  ApplyNode* equals = dynamic_cast<ApplyNode*>(node);
  ApplyNode* arrow = dynamic_cast<ApplyNode*>(equals->args[0]);
  ApplyNode* signature = dynamic_cast<ApplyNode*>(arrow->args[0]);
  ValueNode* ret_type = dynamic_cast<ValueNode*>(arrow->args[1]);
  BlockNode* body = dynamic_cast<BlockNode*>(equals->args[1]);

  std::vector<Type*> types;
  for (SyntaxNode* arg : signature->args) {
    ApplyNode* arg_apply = dynamic_cast<ApplyNode*>(arg);
    ValueNode* arg_name = dynamic_cast<ValueNode*>(arg_apply->args[0]);
    ValueNode* arg_type = dynamic_cast<ValueNode*>(arg_apply->args[1]);
    types.push_back(new AtomicType(arg_type->token->literal));
  }
  types.push_back(new AtomicType(ret_type->token->literal));



  return new CompositeType(types);
}

FnDefNode* to_FnDefNode(StmtNode* node) {
  ApplyNode* equals = dynamic_cast<ApplyNode*>(node->expr);
  ApplyNode* arrow = dynamic_cast<ApplyNode*>(equals->args[0]);
  ApplyNode* signature = dynamic_cast<ApplyNode*>(arrow->args[0]);
  
  ValueNode* name = dynamic_cast<ValueNode*>(signature->fn); // TODO: in general signature->fn might not be a literal ValueNode!
  std::vector<SyntaxNode*> args = signature->args;
  BlockNode* FnBlock = dynamic_cast<BlockNode*>(equals->args[1]);

  FnDefNode *fn = new FnDefNode(name, args, FnBlock);
  fn->ident->type = infer_fn_type(equals);

  //TODO: we're gonna have to modify this once we can support function types
  //  so we can emit the return type properly

  return fn;
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
  FnDefNode* main_fn = new FnDefNode(new ValueNode("main"), {}, main_block);
  main_fn->ident->type = new CompositeType({new AtomicType("int")}); // (int) function type // TODO: we'll have to change this later if we decide to support command line arguments

  for (int i = 0; i < program_statements->children.size(); ++i) {
    StmtNode* child = program_statements->children[i];
    if (is_fn_def(child)) {
      FnDefNode *fn = to_FnDefNode(child);
      format_fn(fn);
      program->children.push_back(fn);
    }
    else {
      main_block->children.push_back(child);
    }
  }
  format_fn(main_fn, true);

  for (auto& function : function_decls) {
    program->children.push_back(function);
  }
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