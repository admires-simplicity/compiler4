#include <cassert>
#include <iostream>
#include <optional>
#include <deque>
#include <functional>

#include "compiler.h"
#include "scope.h"
#include "util.h"


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

// SyntaxNode *to_returning_segment(SyntaxNode *node) {
//     if (node->token->literal == "return") return node;
//     else if (node->token->literal == ";") {
//         node->children[0] = to_returning_segment(node->children[0]);
//     }
//     else if (node->token->literal == "then") {
//         node->children[1] = to_returning_segment(node->children[1]);
//     }
//     else if (node->token->literal == "else") {
//         node->children[0] = to_returning_segment(node->children[0]);
//         node->children[1] = to_returning_segment(node->children[1]);
//     }
//     else if (node->type == SyntaxNode::NodeType::block) {
//         for (auto &child : node->children) {
//             if (child->token->literal == "then"
//              || child->token->literal == "else"
//              || child->type == SyntaxNode::NodeType::block) {
//                 child = to_returning_segment(child);
//             }
//         }
//     }
//     else {
//         node = new SyntaxNode("return", {node});
//     }
//     return node; // since we only ever return node, this is kind of superfluous...
// }

// bool last_to_return(SyntaxNode *node) {
//     assert(node->type == SyntaxNode::NodeType::block);
//     bool make_stmt = false;
//     SyntaxNode **dest = &node->children[node->children.size()-1];
//     SyntaxNode *last_stmt = *dest; //node->children[node->children.size()-1];
//
//     // TODO: this only works if last_stmt is actually an expression statement (in C)
//     // e.g. this won't work for blocks or various other AST types like var defns
//
//     if (last_stmt->token->literal != ";") {
//         *dest = new SyntaxNode(";", {last_stmt});
//         last_stmt = *dest;
//     }
//
//     //last_stmt->children = { new SyntaxNode("return", last_stmt->children) }; // TODO: is this overkill? Shouldn't we only ever need last_stmt->child[0] ?
//     *dest = to_returning_segment(last_stmt);
//
//     return true;
// }

// bool last_to_return(SyntaxNode *node) {
//   return false; // call to_returning_segment here
// }

// class ReturningSegmentVisitor : public SyntaxNodeVisitor {
//   bool res = true;
// public:
//   bool get_result() { return res; }

//   void visit(ValueNode *node) override {
//     assert(false); // nonsensical, should never be called. just needed for completeness to satisfy visitor interface.
//   }

//   void visit(ApplyNode *node) override {
//     ValueNode *fn = dynamic_cast<ValueNode*>(node->fn);
//     if (fn == nullptr) {
//       res = false;
//       return;
//     }
//     if (fn->token->)

//   }

//   void visit(BlockNode *node) override {
//   }

//   void visit(ProgramBlockNode *node) override {
//   }

//   void visit(LetNode *node) override {
//   }

//   void visit(FnDefNode *node) override {
//   }
// };

bool last_to_stmt(SyntaxNode* ) {
  return false;
}

bool to_returning_segment(SyntaxNode* &node) {
  // I can't use a visitor because I need to reassign node in a few cases
  if (ValueNode *vn = dynamic_cast<ValueNode*>(node)) {

  }
  return false;
}

// SyntaxNode *comma_to_arg_list(SyntaxNode *node) {
//     assert(node != nullptr && 1);
//     //std::cout << "comma_to_arg_list : " << node->to_string() << "\n";
//
//     assert(node->type == SyntaxNode::NodeType::literal && node->token->literal == ",");
//     SyntaxNode *rest = node->children[1];
//     Type ret_type = TypeIdList{"int"}; // TODO: this is a placeholder 
//
//     assert(rest != nullptr && 2);
//
//     node->children.erase(node->children.begin() + 1);  
//
//     while (rest != nullptr && rest->type == SyntaxNode::NodeType::literal && rest->token->literal == ",") {
//         //std::cout << rest->children[0]->to_string() << "\n";
//         node->children.push_back(rest->children[0]);
//         rest = rest->children[1];
//     }
//     assert(rest != nullptr && 3);
//     node->children.push_back(rest);
//
//     //std::cout << "comma_to_arg_list : " << node->to_string() << "\n";
//
//     return node;
// }
// /* where
//     // TODO: I just had the idea that I could just write a program that handles
//     //       tests for C++ programs à la Pyret.
//     comma_to_arg_list(SyntaxNode(",", {SyntaxNode("a", SyntaxNode(",", {SyntaxNode("b", SyntaxNode(",", {SyntaxNode("c", SyntaxNode("d"))}))}))})) ==
//     SyntaxNode("arg_list", {SyntaxNode("a"), SyntaxNode("b"), SyntaxNode("c", SyntaxNode("d"))});
// */

// void to_fn_def(SyntaxNode *node) {
//     //std::cout << "to_fn_def : " << node->to_string() << "\n";
//
//     // we shouldn't have to do is_fn_def because we only call this function immediately
//     // after checking is_fn_def
//     assert(node->type == SyntaxNode::NodeType::literal && node->token->literal == ";");
//     // TODO: technically this test is incorrect because it is semantically legal
//     // to have the last statement in a block to be a function definition with
//     // no semicolon... maybe we should just make that illegal at the program_block level?
//     node = node->children[0];
//     assert(node->type == SyntaxNode::NodeType::literal && node->token->literal == "=");
//
//     node->type = SyntaxNode::NodeType::fn_def; 
//     SyntaxNode *sig = node->children[0];
//     assert(sig->type == SyntaxNode::NodeType::literal && sig->token->literal == "->");
//     // sig = (-> child[0]=(apply child[0]=<name> child[1]=<args>) child[1]=<ret_type>)
//     SyntaxNode *name = sig->children[0]->children[0];
//     SyntaxNode *ret_type = sig->children[1];
//     //std::cout << "ret type is " << ret_type->to_string() << "\n";
//
//     TypeVarVec fn_type;
//     SyntaxNode *arg_list;
//
//     if (sig->children[0]->children.size() > 1) {
//         arg_list = sig->children[0]->children[1];
//         if (arg_list->type == SyntaxNode::NodeType::literal && arg_list->token->literal == ",") {
//             comma_to_arg_list(arg_list); // TODO: we should convert all commas, not just ones inside fn defns, so is this really the right place for this?
//         } else {
//             arg_list = new SyntaxNode(",", {arg_list}); // TODO: too hacky... make better
//         }
//
//         //arg_list->children.push_back(ret_type);
//
//         // TODO: this is wrong because it only handles atomic types and not compound/function types
//         for (auto arg : arg_list->children) {
//             if (arg->type == SyntaxNode::NodeType::literal && arg->token->literal == ":") {
//                 fn_type.push_back(arg->children[1]->token->literal); // TODO: change this to something like
//                 //fn_type.push_back(infer_type_id(arg->children[1]->token->literal));
//             }
//             else {
//                 fn_type.push_back(arg->token->literal);
//             }
//         }
//     } else {
//         arg_list = new SyntaxNode(",", {}); // todo: awful
//     }
//
//     fn_type.push_back(ret_type->token->literal);
//     //node->type = SyntaxNode::NodeType::literal;
//     node->val_type = fn_type;
//     // TODO: I should actually be chaing these into type nodes before inserting them as "types"
//
//     SyntaxNode *body = node->children[1];
//
//     node->children[0] = arg_list; // TODO: this is incorrect because we're leaking node->children[0]
//     // TODO: maybe change node->children[0] type to some explicit arg_list type
//
//     node->children.insert(node->children.begin(), name);
//
//     // TODO: we should actually be checking arg types before we set the type of the function,
//     //       that way we can just do arg->val_type instead of arg->token->literal
//
//     //std::cout << "node->val_type : " << type_to_string(node->val_type) << "\n";
//
//     // std::cout << "to_fn_def : " << node->to_string(false, true) << "\n";
// }

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

// SyntaxNode *get_block_in_fn_defn(SyntaxNode *node) {
//     if (node->token->literal == ";") node = node->children[0];
//     node = node->children[1]; // (= (-> ...) <block>) -> <block>
//     return node;
// }

// bool semantic_analysis(SyntaxNode *node, Scope& scope) {
//     bool found_error = false;
//
//     //std::cout << SyntaxNode::NodeType_repr[node->type] << (node->token ? " " + node->token->literal : "") << "\n";
//
//     for (auto child : node->children) {
//         semantic_analysis(child, scope);
//     }
//
//     switch (node->type) {
//         case SyntaxNode::NodeType::literal:
//             if (node->children.size() == 0) {
//                 if (scope.exists(node->token->literal)) {
//                     node->val_type = scope.get(node->token->literal).value()->val_type;
//                 }
//                 else {
//                     node->val_type = infer_literal_type_id(node->token->literal);
//                     if (std::get<int>(node->val_type) == TypeSet::get_id("unassigned type")) {
//                         std::cerr << "<Compiler> Error: unable to assign type for value \"" << node->token->literal << "\"\n";
//                         // TODO: add line and pos numbers
//                         found_error = true;
//                     }
//                 }
//             }
//             // case for unary and bin ops
//             else {
//                 // TODO: make better
//                 if (scope.exists(node->token->literal)) {
//                     node->val_type = scope.get(node->token->literal).value()->val_type;
//                 }
//                 else if (node->token->literal == ":") {
//                     node->val_type = TypeSet::get_id("void");
//                 }
//                 else if (node->token->literal == "=") {
//                     // SyntaxNode *lhs = node->children[0];
//                     // SyntaxNode *type = lhs->children[0];
//                     // SyntaxNode *ident = lhs->children[1];
//                     // SyntaxNode *rhs = node->children[1];
//                     // if (rhs->val_type != TypeSet::interpret_type(type->token->literal)) {
//                     //     std::cerr << "<Compiler> Error: type mismatch in assignment\n";
//                     //     found_error = true;
//                     // }
//                     // else {
//                     //     scope.add(ident->token->literal, rhs);
//                     //     // TODO: this is a hack because we should be interpreting rhs
//                     //     // ourselves if it can be evaluated as a compile time constant,
//                     //     // but here we're just offloading that to the target language
//                     //     // compiler.
//                     // }
//                 }
//                 else if (node->token->literal == "let") {
//                     SyntaxNode *eq = node->children[0];
//                     SyntaxNode *type_decl = eq->children[0];
//                     SyntaxNode *ident = type_decl->children[0];
//                     SyntaxNode *type_node = type_decl->children[1];
//                     SyntaxNode *value = eq->children[1];
//                     std::optional<Type> type = TypeSet::interpret_type(type_node->token->literal);
//                     if (value->val_type != type) {
//                         std::cerr << "<Compiler> Error: type mismatch in let declaration\n";
//                         found_error = true;
//                     }
//                     if (scope.exists(ident->token->literal)) {
//                         std::cerr << "<Compiler> Error: redeclaration of variable \"" << ident->token->literal << "\"\n";
//                         found_error = true;
//                     }
//                     if (type == std::nullopt) {
//                         std::cerr << "<Compiler> Error: unknown type \"" << type_node->token->literal << "\"\n";
//                         found_error = true;
//                     }
//                     if (!found_error) {
//                         scope.add(ident->token->literal, value); // TODO: when we add compile time value interpreting we'll have to change this
//                         ident->val_type = TypeSet::get_id("identifier");
//                         type_node->val_type = TypeSet::get_id("type");
//                     }
//                 }
//                 else {
//                     std::cerr << "<Compiler> Error: unknown operator \"" << node->token->literal << "\"\n";
//                     found_error = true;
//                 }
//             }
//             break;
//
//
//     }
//
//     return !found_error;
// }

bool format_fn(FnDefNode* node) {
  //last_to_stmt(node->block);
  //to_returning_segment(node->block);

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
  format_fn(main_fn);

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