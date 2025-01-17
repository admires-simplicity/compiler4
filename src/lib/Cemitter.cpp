#include <iostream>
#include <sstream>

#include "parser.h"
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

  void visit(ReturnNode* node) override {
    output << "return ";
    node->expr->accept(*this);
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
    const CompositeType* type = dynamic_cast<const CompositeType*>(node->get_type());
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

  // void visit_semicolon(ApplyNode* node) {
  //   node->args[0]->accept(*this);
  //   output << ";\n";
  // }
   
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

  void visit_if(ApplyNode* node) {
    output << "if (";
    node->args[0]->accept(*this); // condition
    output << ") ";
  }

  void visit_then(ApplyNode* node) {
    node->args[0]->accept(*this); // if-expr
    node->args[1]->accept(*this); // then-expr/block OR else-then-expr/block
  }

  void visit_else(ApplyNode* node) {
    node->args[0]->accept(*this); // then-expr/block
    output << "else\n";
    node->args[1]->accept(*this); // else-expr/block
  }

  void visit_let(ApplyNode* node) {
    ApplyNode *eq = dynamic_cast<ApplyNode*>(node->args[0]);
    if (eq == nullptr) {
      std::cerr << "<Cemitter> Error: let node does not have an = operator\n";
      res = false;
      return;
    }
    ApplyNode *colon = dynamic_cast<ApplyNode*>(eq->args[0]);
    if (colon == nullptr) {
      std::cerr << "<Cemitter> Error: let node does not have a colon operator [Untyped vars TBI]\n";
      res = false;
      return;
    }
    SyntaxNode *referand = eq->args[1];
    
    colon->accept(*this);
    output << " = ";
    referand->accept(*this);
  }

  bool handle_special_op(ApplyNode* node) {
    static const std::unordered_map<std::string, void(EmitVisitor::*)(ApplyNode*)> handlers = {
      // {";", &EmitVisitor::visit_semicolon},
      // {"<", &EmitVisitor::visit_binop},
      // {">", &EmitVisitor::visit_binop},
      // {"+", &EmitVisitor::visit_binop},
      // {"-", &EmitVisitor::visit_binop},
      // {"*", &EmitVisitor::visit_binop},
      // {"/", &EmitVisitor::visit_binop},
      // {"^", &EmitVisitor::visit_binop},
      // {"=", &EmitVisitor::visit_binop},
      {":", &EmitVisitor::visit_colon},
      {"if", &EmitVisitor::visit_if},
      {"then", &EmitVisitor::visit_then},
      {"else", &EmitVisitor::visit_else},
      {"let", &EmitVisitor::visit_let},
    };

    ValueNode *fn = dynamic_cast<ValueNode*>(node->fn); // get function pointer
    if (fn == nullptr) return false;
    std::string op = fn->token->literal;

    if (auto it = handlers.find(op); it != handlers.end()) {
      auto f = it->second;
      (this->*f)(node);
      return true;
    } else if (bin_ops.contains(node->fn->to_string())) {
      EmitVisitor::visit_binop(node);
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
