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

std::set<std::string> bin_ops = {
  "<",
  ">",
  "+",
  "-",
  "*",
  "/",
  "^",
  "=",
  ",",
  ":",
  "->",
  "then", 
  "else",
};

std::map<std::string, uint32_t> syntax_ids = {
  {"=", 1},
  {"->", 2},
  {",", 3},
  {":", 4},
  {"<", 8},
  {">", 9},
  {"+", 10},
  {"-", 11},
  {"*", 12},
  {"/", 13},
  {"^", 14},
  {"(", 100},
  {"{", 102},
  {";", 110},
  {"fn", 120},
  {"let", 121},
  {"var", 122},

  {"if", 200},
  {"then", 201},
  {"else", 202},

  {"return", 300},
};

std::set<std::string> prefix_ops = {
  "-",
  //"+",
  //"!",
  "fn",
  "let",
  "var",

  "if",

  "return",
};

std::set<std::string> postfix_ops = {
  ";",
};

std::map<uint32_t, uint32_t> unary_precedence = {
  {11, 19},
  // {12, 15},
  // {13, 15},
  {14, 19},

  {120, 3},
  {121, 3},
  {122, 3},
};

std::map<uint32_t, BinPrecedence> binary_precedence = {
  {0, {Assoc::left, 0}}, // "Null" operator -- not actually a valid operator
  // but we need an op to compare to when we start parsing. "left" precedence
  // doesn't matter because precedence only matters when comparing identical
  // operators, and we should never have id = 0.

  {110, {Assoc::left, 1}},

  {300, {Assoc::left, 2}}, // 'return'

  {3, {Assoc::right, 3}},


  {120, {Assoc::left, 4}}, // these are supposed to be "postfix" ops, but
  {121, {Assoc::left, 4}}, // I gave them "binop" precedences just to compare in
  {122, {Assoc::left, 4}}, // can_bind_left ... TODO: fix this?

  //if, then, else
  {200, {Assoc::left, 7}},
  {201, {Assoc::left, 6}},
  {202, {Assoc::left, 5}},
  
  {1, {Assoc::left, 9}},
  {2, {Assoc::right, 10}},
  {4, {Assoc::right, 11}},

  {8, {Assoc::left, 12}},
  {9, {Assoc::left, 12}},
  {10, {Assoc::left, 16}},
  {11, {Assoc::left, 16}},
  {12, {Assoc::left, 21}},
  {13, {Assoc::left, 21}},
  {14, {Assoc::right, 26}},
};

uint32_t op_precedence(std::string op) {
  BinPrecedence bp = binary_precedence[syntax_ids[op]];
  //return bp.precedence + (bp.assoc == Assoc::left ? 1 : 0);
  return bp.precedence;
}

class Parser {
public:
  Lexer lexer;

  Parser(Lexer lexer) : lexer(lexer) {}

  SyntaxNode *parse() {
    return parse_expr(0);
  }

private:
  ArgListNode *parse_parens() {
    Token *tkn = lexer.next().value();
    assert(tkn->literal == "(");

    std::vector<SyntaxNode*> args;

    while (lexer.peek().value()->literal != ")") {
      args.push_back(parse_expr(","));
      if (lexer.peek().value()->literal == ",") lexer.next(); // consume comma
    }

    Token *nxt = lexer.next().value(); // advance / skip ")"
    assert(nxt->literal == ")");

    return (args.size()) ? new ArgListNode(args) : nullptr;
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
      
      // prefix op
      if (is_prefix_op) {
        //val = new SyntaxNode(tkn, std::vector<SyntaxNode*>{parse_expr(syntax_ids[tkn->literal])});
        val = new ApplyNode(new ValueNode(tkn), new ArgListNode({parse_expr(syntax_ids[tkn->literal])}));
      }
      // literal
      else {
        //val = new SyntaxNode(tkn);
        val = new ValueNode(tkn);
      }
    }

    // now we have a value, we can check for postfix ops
    std::optional<Token*> nxt = lexer.peek();
    if (!nxt.has_value()) return val;

    tkn = nxt.value();
    if (tkn->literal == "(") { // TODO: switch this to a Trie representation?
      ArgListNode *args = parse_parens();
      val = new ApplyNode(val, args);
    }    

    return val;
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
      //return new SyntaxNode(op, std::vector<SyntaxNode*>{left, right});
      return new ApplyNode(new ValueNode(op), new ArgListNode({left, right}));
    } else {
      //return new SyntaxNode(op, std::vector<SyntaxNode*>{left});
      return new ApplyNode(new ValueNode(op), new ArgListNode({left}));
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
