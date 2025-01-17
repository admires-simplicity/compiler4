#pragma once
#include <cstdint>
#include <map>
#include <cassert>
#include "lexer.h"
#include "AST.h"
#include "types.h"
//#include ""

enum class Assoc {
  left,
  right,
};

typedef struct {
  Assoc assoc;
  uint32_t precedence;
} BinPrecedence;

extern std::set<std::string> bin_ops;

extern std::map<std::string, uint32_t> syntax_ids;

extern std::set<std::string> prefix_ops;

extern std::set<std::string> special_bin_ops;

extern std::set<std::string> special_prefix_ops;

extern std::set<std::string> postfix_ops;

extern std::map<uint32_t, uint32_t> unary_precedence;

extern std::map<uint32_t, BinPrecedence> binary_precedence;

uint32_t op_precedence(std::string op);

class Parser {
public:
  Lexer lexer;

  Parser(Lexer lexer) : lexer(lexer) {}

  SyntaxNode *parse() {
    return parse_expr(0);
  }

private:
  SyntaxNode *parse_parens() {
    Token *tkn = lexer.next().value();
    assert(tkn->literal == "(");

    SyntaxNode *expr = parse_expr(0);

    Token *nxt = lexer.next().value();
    assert(nxt->literal == ")");

    return expr;
  }

  std::vector<SyntaxNode*> parse_arglist() {
    Token *tkn = lexer.next().value();
    assert(tkn->literal == "(");

    std::vector<SyntaxNode*> args;

    while (lexer.peek().value()->literal != ")") {
      args.push_back(parse_expr("let")); // TODO: this is a hack because I can't
      // do parse_expr(",") to parse a value up to a comma because comma is
      // right-assoc, and I don't have a general way to get an operator with
      // precedence minimally greater than a given operator.
      if (lexer.peek().value()->literal == ",") lexer.next(); // consume comma
    }

    Token *nxt = lexer.next().value(); // advance / skip ")"
    assert(nxt->literal == ")");

    return args;
  }

  SyntaxNode *handle_special_prefix_op(Token *op) {
    if (op->literal == "return") {
      return new ReturnNode(parse_expr(syntax_ids["return"]));
    }
    return nullptr; // error
  }

  // this function is a slight misnomer, since it handles parens and prefix ops
  // the "value" part is just the first line, `tkn = lexer.next().value()`
  SyntaxNode *parse_value() {
    Token *tkn = lexer.peek().value();
    SyntaxNode *val = nullptr;
    if (tkn->literal == "(") {
      val = parse_parens();
    } else if (tkn->literal == ")") {
      // error for now, maybe unit or something in the future...
      //--nesting is handled in parse_parens
      return nullptr;
    } else if (tkn->literal == "{"){
      lexer.next(); // consume token
      std::vector<SyntaxNode*> children;
      while (lexer.peek().value()->literal != "}") {
        children.push_back(parse());
      }
      lexer.next(); // consume "}"
      //val = new SyntaxNode(SyntaxNode::NodeType::block, children);
      val = new BlockNode(children);
    } else {
      // value which is not a paren or block -- literal or prefix op
      lexer.next(); // consume token
      bool is_prefix_op = prefix_ops.contains(tkn->literal);
      bool is_special_prefix_op = special_prefix_ops.contains(tkn->literal);
      
      // prefix op
      if (is_special_prefix_op) {
        val = handle_special_prefix_op(tkn);
      }
      else if (is_prefix_op) {
        val = new ApplyNode(new ValueNode(tkn), std::vector<SyntaxNode*>{parse_expr(syntax_ids[tkn->literal])});
      }
      // literal
      else {
        //val = new SyntaxNode(tkn);
        val = new ValueNode(tkn);
      }
    }

    // now we have a value, we can check for postfix ops
    std::optional<Token*> nxt = lexer.peek();
    if (!nxt.has_value()) return val; // if we're at the end of the input

    // TODO: allow for repeated ()?
    tkn = nxt.value();
    if (tkn->literal == "(") { // TODO: switch this to a Trie representation?
      std::vector<SyntaxNode*> args = parse_arglist();
      val = new ApplyNode(val, args);
    }    

    return val;
  }

  SyntaxNode *parse_special_bin_op_right_expr(std::string op) {
    if (op == "else" || op == "then") {
      SyntaxNode *right = parse_expr(0);
      if (BlockNode *block = dynamic_cast<BlockNode*>(right)) {
        return block;
      } else {
        return new BlockNode(std::vector<SyntaxNode*>{right});
      }
    }
    return nullptr; // error
  }

  SyntaxNode *node_to_block(SyntaxNode *node) {
    if (BlockNode *block = dynamic_cast<BlockNode*>(node)) {
      return block;
    } else {
      return new BlockNode(std::vector<SyntaxNode*>{node});
    }
  }

  SyntaxNode *handle_special_bin_op(Token *op, SyntaxNode *left, SyntaxNode *right) {
    if (op->literal == "then" || op->literal == "else") {
      return new ApplyNode(new ValueNode(op), std::vector<SyntaxNode*>{left, node_to_block(right)});
    }
    else if (op->literal == "=") {
      // /// syntaxes for value assignment ///////////
      // // typed value assignment
      // (=
      //   (: <ident> <type>)
      //   <value>)
      // // untyped value assignment
      // (=
      //   <ident>
      //   <value>)
      // /// syntaxes for function definition ////////
      // // typed function definition
      // (=
      //   (->
      //     (f <arg1> ... <argn>)
      //     <ret-type>)
      //   <body>)
      // // untyped function definition
      // (=
      //   (f <arg1> ... <argn>)
      //   <body>)
      
      //return new ApplyNode(new ValueNode(op), std::vector<SyntaxNode*>{left, right});

      ApplyNode *signature = dynamic_cast<ApplyNode*>(left);
      if (signature == nullptr) { // untyped value assignment
        //return new ApplyNode(new ValueNode(op), std::vector<SyntaxNode*>{left, right});
        std::cerr << "<Parser> TO BE IMPLEMENTED: untyped value assignment" << std::endl;
        return nullptr;
      }
      
      ValueNode *sig_op = dynamic_cast<ValueNode*>(signature->fn);
      if (sig_op == nullptr) {
        std::cerr << "<Parser> Error: expected \":\", \"->\", or function name string, got " << signature->fn->to_string() << std::endl;
        return nullptr;
      }
      else if (sig_op->token->literal == ":") { // typed value assignment
        return new ApplyNode(new ValueNode(op), std::vector<SyntaxNode*>{signature, right});
      }
      else if (sig_op->token->literal == "->") { // typed function definition
        // (->                     -- signature
        //   (f <arg1> ... <argn>) -- fn_sig      -- f is fn_sig->fn, <arg1> ... <argn> is fn_sig->args
        //   <ret-type>)           -- ret_type        
        ApplyNode *fn_sig = dynamic_cast<ApplyNode*>(signature->args[0]);
        if (fn_sig == nullptr) {
          std::cerr << "<Parser> Error: expected function signature, got " << signature->args[0]->to_string() << std::endl;
          return nullptr;
        }

        ValueNode *fn_name = dynamic_cast<ValueNode*>(fn_sig->fn);
        if (fn_name == nullptr) {
          std::cerr << "<Parser> Error: expected identifier, got " << fn_sig->fn->to_string() << std::endl;
          return nullptr;
        }

        ValueNode *ret_type = dynamic_cast<ValueNode*>(signature->args[signature->args.size() - 1]);
        if (ret_type == nullptr) {
          std::cerr << "<Parser> Error: expected return type, got " << signature->args[signature->args.size() - 1]->to_string() << std::endl;
          return nullptr;
        }

        Type *t = new CompositeType(std::vector<Type*> {new AtomicType(ret_type->token->literal)}); // TODO: maybe give CompositeType a move 
        fn_name = new ValueNode(t, fn_name->token);

        BlockNode *block = dynamic_cast<BlockNode*>(right);
        if (block == nullptr) {
          return new FnDefNode(fn_name, fn_sig->args, new BlockNode(std::vector<SyntaxNode*>{right}));
        } else {
          return new FnDefNode(fn_name, fn_sig->args, block);
        }
      }
      else { // untyped function definition
        std::cerr << "<Parser> TO BE IMPLEMENTED: untyped function definition" << std::endl;
        return nullptr;
      }

    }

    return nullptr; // error
  }

  // true if current operator can bind value as its left operator
  //      otherwise value binds as right of last operator
  bool can_bind_left(uint32_t current_op_id, uint32_t last_op_id) {
    // auto [assoc, precedence] = binary_precedence[syntax_ids[op]];
    // return assoc == Assoc::left && precedence <= min_precedence;
    auto [curr_assoc, curr_precedence] = binary_precedence[current_op_id];
    auto [last_assoc, last_precedence] = binary_precedence[last_op_id];
    if (current_op_id == last_op_id) {
      return curr_assoc == Assoc::right; // right assoc op can take left operand
    } else if (last_op_id == 0 && current_op_id == syntax_ids[";"]) {
      return true; // kind of a hack
    } else {
      return curr_precedence > last_precedence;
    }
  }

  // this function will only parse expressions with operators of higher precedence
  // than last_op_id.
  // meaning,
  // A op1 B op2 C
  //       ^ parse_increasing_precedence(A, op1)
  // will only bind op2 to B if op2 has higher precedence than op1
  // OR if op2 == op1 is right associative (in which case op2 indeed has higher precedence)
  SyntaxNode *parse_increasing_precedence(SyntaxNode* left, uint32_t last_op_id) { // 
    bool can_parse_postfix = false;
    bool can_parse_binary = false;
    if (!lexer.awaiting(1)) return left;

    Token *nxt = lexer.peek().value();

    if (postfix_ops.contains(lexer.peek().value()->literal)
      && can_bind_left(syntax_ids[nxt->literal], last_op_id)) {
      // std::cout << "can parse postfix" << std::endl;
      // std::cout << "postfix: " << nxt->literal << std::endl;
      // std::cout << "last id: " << last_op_id << std::endl;
      can_parse_postfix = true;
    }

    // check if we can parse a bin op
    if (lexer.awaiting(2) // don't have an op and value
      && bin_ops.contains(lexer.peek().value()->literal) // next lexeme is not an op
      && can_bind_left(syntax_ids[lexer.peek().value()->literal], last_op_id)) // next op has lower precedence than last op      
      can_parse_binary = true;
    
    if (!can_parse_postfix && !can_parse_binary) return left;
    
    //if (can_parse_postfix && can_parse_binary) {} // TODO: think about this problematic case

    Token *op = lexer.next().value();
    if (bin_ops.contains(op->literal)) {
      SyntaxNode *right = parse_expr(syntax_ids[op->literal]); //(1)
      // if (special_bin_ops.contains(op->literal)) right = parse_special_bin_op_right_expr(op->literal);
      // else right = parse_expr(syntax_ids[op->literal]); //(1)
      if (special_bin_ops.contains(op->literal)) return handle_special_bin_op(op, left, right); // TODO: change this to if (handle_special_bin_op) ...
      else
      return new ApplyNode(new ValueNode(op), std::vector<SyntaxNode*>{left, right});
    } else { // postfix
      if (op->literal == ";") {
        return new StmtNode(left);
      } else {
        return new ApplyNode(new ValueNode(op), std::vector<SyntaxNode*>{left}); // right now there actually are no other postfix ops...
      }
    }
  }
  
  SyntaxNode *parse_expr(std::string last_op) {
    return parse_expr(syntax_ids[last_op]);
  }

  SyntaxNode *parse_expr(uint32_t last_op_id) {
    SyntaxNode *left = parse_value();

    while (true) {
      SyntaxNode* next = parse_increasing_precedence(left, last_op_id); // (2)
      if (next == left) break;
      left = next;
    }
    return left;
  }

  // (1) parse_expr calls parse_increase_precedence, so it will only parse
  // operators of higher precedence than the current operator --> all
  // sub-operators in right will be of higher precedence than op.

  // (2) this makes a left-leaning tree, because everything that has been
  // parsed so far is used as the left of the tree built in
  // parse_increasing_precedence

};




// idea for optimizing parser:
// instead of checking `token == str`, for a bunch of strings, we could do
// something like `int parse_rule = trie[token]`, and switch on those...
