#pragma once
#include <cstdint>
#include <map>
#include <cassert>
#include "lexer.h"
#include "AST.h"

// enum class Precedence {
//   // none,
//   // assignment, // =
//   // or_, // or
//   // and_, // and
//   // equality, // == !=
//   // comparison, // < > <= >=
//   // term, // + -
//   // factor, // * /
//   // unary, // ! -
//   // call, // . ()
//   // primary
// };



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
  "->"
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
};

std::set<std::string> prefix_ops = {
  "-",
  //"+",
  //"!",
  "fn",
};

std::map<uint32_t, uint32_t> unary_precedence = {
  {11, 15},
  // {12, 15},
  // {13, 15},
  {14, 15},
};

std::map<uint32_t, BinPrecedence> binary_precedence = {
  {0, {Assoc::left, 0}}, // "Null" operator -- not actually a valid operator
  // but we need an op to compare to when we start parsing. "left" precedence
  // doesn't matter because precedence only matters when comparing identical
  // operators, and we should never have id = 0.

  {1, {Assoc::left, 1}},
  {2, {Assoc::right, 2}},
  {3, {Assoc::right, 3}},
  {4, {Assoc::right, 4}},

  {7, {Assoc::right, 3}},
  {8, {Assoc::left, 5}},
  {9, {Assoc::left, 5}},
  {10, {Assoc::left, 10}},
  {11, {Assoc::left, 10}},
  {12, {Assoc::left, 15}},
  {13, {Assoc::left, 15}},
  {14, {Assoc::right, 20}},
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
  uint16_t nesting = 0;

  SyntaxNode *parse_parens() {
    Token *tkn = lexer.next().value();
    assert(tkn->literal == "(");
    uint16_t nest = nesting;
    ++nesting;
    SyntaxNode *val = parse();
    Token *nxt = lexer.next().value(); // advance / skip ")"
    --nesting;
    assert(nxt->literal == ")");
    assert(nesting == nest);
    return val;
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
    } else {
      lexer.next(); // consume token
      if (prefix_ops.contains(tkn->literal)) {
        val = new SyntaxNode(tkn, std::vector<SyntaxNode*>{parse_value()});
      } else {
        val = new SyntaxNode(tkn);
      }
    }

    // now we have a value, we can check for postfix ops
    std::optional<Token*> nxt = lexer.peek();
    if (!nxt.has_value()) return val;

    tkn = nxt.value();
    if (tkn->literal == "(") { // switch this to a Trie representation...
      SyntaxNode *args = parse_parens();
      if (args) val = new SyntaxNode(new Token(Token::Type::apply), std::vector<SyntaxNode*>{val, args});
      else      val = new SyntaxNode(new Token(Token::Type::apply), std::vector<SyntaxNode*>{val});
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
    } else {
      return curr_precedence > last_precedence;
    }
  }

  SyntaxNode *parse_increasing_precedence(SyntaxNode* left, uint32_t last_op_id) {
    if (!lexer.awaiting(2) // don't have an op and value
      || !bin_ops.contains(lexer.peek().value()->literal) // next lexeme is not an op
      || !can_bind_left(syntax_ids[lexer.peek().value()->literal], last_op_id) // next op has lower precedence than last op
      ) {
      return left;
    }
    Token *op = lexer.next().value();
    SyntaxNode *right = parse_expr(syntax_ids[op->literal]); //(1)
    return new SyntaxNode(op, std::vector<SyntaxNode*>{left, right});
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








// MAYBE a better solution for operator precedence is just to handle precedence
// for binary ops and make n-ary ops non-associative.



// I think the way I will handle precedence for binary and n-ary operators
// is to just have precedence and associativity for binary ops, but only
// have precedence for n-ary ops. 
// I think maybe instead of "associativity" I should have something like
// self-precedence, i.e. when you encounter A op B op C, instead of asking
// whether it "groups" as (A op B) op C or A op (B op C) syntactically, you ask
// which of those identical operators has higher precedence.
// e.g. something like "eager" vs "lazy" operators...

// this should also work for n-ary operators, e.g.
// if A then if B then C else E else F

// if_then_else eager: 10
// eager: if A then (if B then C) else E
//   so if A then if B then C else E else F gives syntax error because "else F" is dangling

// lazy: if A then (if B then C else E) else F -- fine

// eager -- the first operator should finish first
// lazy -- the first operator should finish later







// 3 > 4 + 5 * 6

//   >
//  / \
// 3   +
//    / \
//   4   *
//      /  \
//     5    6


// 6 * 5 + 4 < 3

//       <
//      / \
//     +   3
//    / \
//   *   4
//  / \
// 6   5




// right leaning tree:

// get_expr():
//   left = lexer.lex
  
//   if ! bin_op(lexer.peek):
//     return leaf
   
//   op = lexer.lex
//   right = get_expr
//   return ast_node(op, left, right)


// left leaning tree:

// get_expr():
//   left = lexer.lex

//   while lex.peek:
  
//     if ! bin_op(lexer.peek):
//       return left
  
//     op = lexer.lex
//     right = lexer.lex
//     left = ast_node(op, left, right)

//   return left
  



// 3 * 6 + 2
// left = 3
// op = *


// 3 > 4 * 5 + 6

//   > : 8
//  / \
// 3   + : 10
//    / \
//   *   6 (* : 11)
//  / \
// 4   5


// 1 + 2 + 3
//     + : 10
//    / \
//   +   3 (+ : 10)
//  / \
// 1   2  



// 1 * 2 + 3 * 4

//      +
//    /   \
//   *     *
//  / \   / \
// 1   2 3   4
