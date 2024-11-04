#include <iostream>
#include <sstream>

#include "Cemitter.h"

std::set<std::string> c_infix_ops = {
  "<",
  ">",
  "+",
  "-",
  "*",
  "/",
  "^",
  "=",
};



// the point of this function is to output C headers (and maybe globals?)
// before passing the program to emit(SyntaxNode*).
bool emit_program(SyntaxNode* program) {
  std::cout << "#include <stdio.h>\n";
  emit(program);
  return true;
}

class EmitVisitor : public SyntaxNodeVisitor {
  bool res = true;
  std::stringstream output;
  bool output_type = false;
public:
  bool get_result() { return res; }

  std::string get_output() { return output.str(); }

  void visit(ValueNode* node) override {
    if (output_type) {

    }
    output << node->token->literal;
  }

  void visit(ApplyNode* node) override {
    node->fn->accept(*this);

    output << "(";
    for (int i = 0; i < node->args.size(); ++i) {
      node->args[i]->accept(*this);
      // TODO: modify this for special functions (operators)
    }
    output << ")";
  }

  void visit(BlockNode* node) override {
    output << "{\n";
    for (auto child : node->children) {
      child->accept(*this);
    }
    output << "}\n";
  }

  void visit(ProgramBlockNode* node) override {
    for (auto child : node->children) {
      child->accept(*this);
    }
  }

  void visit(LetNode* node) override {
    node->value->accept(*this);
    output << " = ";
    node->ident->accept(*this);
  }

  void visit(FnDefNode* node) override {
    bool last_ot = output_type;
    output_type = true; // set flag to output types in visit(ValueNode*)
    node->ident->accept(*this);
    output << "(";
    for (int i = 0; i < node->args.size(); ++i) {
      node->args[i]->accept(*this);
      output << ", ";
    }
    output_type = last_ot; // reset flag
    if (node->args.size()) output.seekp(-2, std::ios_base::end); // delete last ", "
    output << ") ";
    node->block->accept(*this);
  }
};

bool emit(SyntaxNode* node) {
  EmitVisitor v;
  node->accept(v);
  std::cout << v.get_output();
  return v.get_result();
}