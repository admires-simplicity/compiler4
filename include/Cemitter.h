#pragma once
#include <set>
#include <iostream>
//#include <sstream>

#include "AST.h"
#include "lexer.h"

bool emit(SyntaxNode *node);

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

std::map<std::string, std::string> c_std_include_map = {
  {"printf", "<stdio.h>"},
};

bool emit_literal(SyntaxNode *node) {
  assert(node->type == SyntaxNode::NodeType::literal);
  assert(node->token);
  std::cout << node->token->literal;
  return true;
}

void emit_block(SyntaxNode *node) {
  for (auto child : node->children) {
    emit(child);
    //std::cout << ";\n";
  }
}

bool emit(SyntaxNode *node) {
  if (!node) {
    std::cerr << "<CEmitter> Error: null node\n";
    return false;
  }
  // if (!node->token) {
  //   std::cerr << "<CEmitter> Error: null token\n";
  //   return false;
  // }

  //std::cout << "\"" << node->token->token_type_repr[node->token->type] << ": " << node->token->literal << "\"\n";

  switch (node->type) {
    //case SyntaxNode::Type::identifier:
    case SyntaxNode::NodeType::literal:
      // "syntax" types will have to be here for now
      if (c_infix_ops.contains(node->token->literal)) {
        //std::cout << "(";
        emit(node->children[0]);
        emit_literal(node);
        emit(node->children[1]);
        //std::cout << ")";
      }

      else if (node->token->literal == ";") {
        emit(node->children[0]);
        std::cout << ";\n";
      }

      else if (node->token->literal == ":") {
        SyntaxNode *ident = node->children[0];
        SyntaxNode *type = node->children[1];
        emit(type);
        std::cout << " ";
        emit(ident);

      }

      else if (node->token->literal == "=") { // TODO: delete this. it will be deprecated soon. (After I fix main defn in compiler.cpp)
        SyntaxNode *lh = node->children[0];
        SyntaxNode *rh = node->children[1];

        if (lh->token->literal == "->") { // typed function defn
          SyntaxNode *call_sig = lh->children[0];
          SyntaxNode *rtype = lh->children[1];
          assert(call_sig->type == SyntaxNode::NodeType::apply);
          emit(rtype);
          std::cout << " ";
          emit(call_sig); // TODO: this is broken?
          emit(rh);
        }
      }
      
      else if (node->token->literal == ",") {
        for (int i = 0; i < node->children.size(); i++) {
          emit(node->children[i]);
          if (i < node->children.size() - 1) std::cout << ", ";
        }
      }

      else if (node->token->literal == "then") {
        emit(node->children[0]);
 
        if (node->children[1]->type == SyntaxNode::NodeType::block) {
          emit(node->children[1]);
        }
        else {
          std::cout << "{\n";
          emit(node->children[1]);
          std::cout << ";";
          std::cout << "\n}\n";  
        }
      }

      else if (node->token->literal == "if") {
        std::cout << "if (";
        emit(node->children[0]);
        std::cout << ") ";
      }

      else if (node->token->literal == "else") {
        emit(node->children[0]);

        if (node->children[1]->type == SyntaxNode::NodeType::block) {
          std::cout << "else ";
          emit(node->children[1]);
        }
        else {
          std::cout << "else {\n";
          emit(node->children[1]);
          std::cout << ";";
          std::cout << "\n}\n";
        }
      }
      
      else if (node->token->literal == "return") {
        std::cout << "return ";
        emit(node->children[0]);
      }

      else if (node->token->literal == "let") {
        SyntaxNode *assignment = node->children[0];
        SyntaxNode *def = assignment->children[0];
        SyntaxNode *name = def->children[0];
        SyntaxNode *type = def->children[1];
        SyntaxNode *val = assignment->children[1];
        emit(type);
        std::cout << " ";
        emit(name);
        std::cout << " = ";
        emit(val);
      }

      else {
        return emit_literal(node); // same as Type::number
      }
  
      break;

    // case SyntaxNode::Type::number:
    //   return emit_literal(node);

    case SyntaxNode::NodeType::fn_def: {
      SyntaxNode *name = node->children[0];
      SyntaxNode *params = node->children[1];
      SyntaxNode *body= node->children[2];

      TypeIdList &param_types = std::get<TypeIdList>(node->val_type); // TODO: think about, just using params directly?
      std::cout << TypeSet::get_type_name(std::get<int>(param_types[param_types.size() - 1]))
        << " " << name->token->literal << "(";
      emit(params);
      std::cout << ") {\n";
      emit(body);
      std::cout << "}\n";
      break;
    }
      

    case SyntaxNode::NodeType::apply:
      emit(node->children[0]);
      std::cout << "(";
      if (node->children.size() > 1) emit(node->children[1]);
      std::cout << ")";
      break;

    case SyntaxNode::NodeType::block:
      std::cout << "{\n";
      emit_block(node);
      std::cout << "}\n";
      break;

    case SyntaxNode::NodeType::program_block:
      printf("#include <stdio.h>\n");
      emit_block(node);
      break;

    default:
      std::cerr << "<CEmitter> Error: unhandled node type " << node->type_name() << "\n";
      return false;
  }

  return true;
}
