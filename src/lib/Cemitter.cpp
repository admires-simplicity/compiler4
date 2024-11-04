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
public:
  bool get_result() { return res; }

  std::string get_output() { return output.str(); }

  void visit(ValueNode* node) override {
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
    node->ident->accept(*this);
    output << "(";
    for (int i = 0; i < node->args.size(); ++i) {
      node->args[i]->accept(*this);
      output << ", ";
    }
    output.seekp(-2, std::ios_base::end); // delete last ", "
    std::cout << ") ";
    node->block->accept(*this);
  }
};

bool emit(SyntaxNode* node) {
  EmitVisitor v;
  node->accept(v);
  std::cout << v.get_output();
  return v.get_result();
}