#include <iostream>
#include <sstream>

#include "Cemitter.h"

std::set<std::string> special_ops = {
  "<",
  ">",
  "+",
  "-",
  "*",
  "/",
  "^",
  "=",
  ";",
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

  void visit(StmtNode* node) override {
    node->expr->accept(*this);
    output << ";\n";
  }

  void visit(ValueNode* node) override {
    output << node->token->literal;
  }

  void visit(ApplyNode* node) override {
    if (handle_special_op(node)) return;

    node->fn->accept(*this);

    output << "(";
    for (int i = 0; i < node->args.size(); ++i) {
      node->args[i]->accept(*this);
      output << ", ";
    }
    output.seekp(-2, std::ios_base::end); // delete last ", "
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

  void output_fn_id(ValueNode* node) {
    CompositeType* type = dynamic_cast<CompositeType*>(node->type);
    if (type == nullptr) {
      std::cerr << "<Cemitter> Error: function type is not a CompositeType\n";
      res = false;
      return;
    }
    const Type* ret_type = type->return_type();
    if (const AtomicType* type = dynamic_cast<const AtomicType*>(ret_type)) {
      output << type->to_string() << " ";
      output << node->token->literal;
    } else if (const CompositeType* type = dynamic_cast<const CompositeType*>(ret_type)) {
      std::cerr << "<Cemitter> Error: HOF function type handling TBI\n";
    }
  }

  void visit(FnDefNode* node) override {
    output_fn_id(node->ident);
    output << "(";
    for (int i = 0; i < node->args.size(); ++i) {
      node->args[i]->accept(*this);
      output << ", ";
    }
    if (node->args.size()) output.seekp(-2, std::ios_base::end); // delete last ", "
    output << ") ";
    node->block->accept(*this);
  }

  void visit_semicolon(ApplyNode* node) {
    node->args[0]->accept(*this);
    output << ";\n";
  }
   
  void visit_binop(ApplyNode* node) {
    ValueNode *fn = dynamic_cast<ValueNode*>(node->fn);
    std::string op = fn->token->literal;
    output << "(";
    node->args[0]->accept(*this);
    output << ")" << op << "(";
    node->args[1]->accept(*this);
    output << ")";
  }

  void visit_colon(ApplyNode* node) {
    node->args[1]->accept(*this);
    output << " ";
    node->args[0]->accept(*this);
  }

  bool handle_special_op(ApplyNode* node) {
    static const std::unordered_map<std::string, void(EmitVisitor::*)(ApplyNode*)> handlers = {
      {";", &EmitVisitor::visit_semicolon},
      {"<", &EmitVisitor::visit_binop},
      {">", &EmitVisitor::visit_binop},
      {"+", &EmitVisitor::visit_binop},
      {"-", &EmitVisitor::visit_binop},
      {"*", &EmitVisitor::visit_binop},
      {"/", &EmitVisitor::visit_binop},
      {"^", &EmitVisitor::visit_binop},
      {"=", &EmitVisitor::visit_binop},
      {":", &EmitVisitor::visit_colon},
    };

    ValueNode *fn = dynamic_cast<ValueNode*>(node->fn);
    if (fn == nullptr) return false;
    std::string op = fn->token->literal;

    if (auto it = handlers.find(op); it != handlers.end()) {
      auto f = it->second;
      (this->*f)(node);
      return true;
    } else {
      return false;
    }
  }
};

bool emit(SyntaxNode* node) {
  EmitVisitor v;
  node->accept(v);
  std::cout << v.get_output();
  return v.get_result();
}
