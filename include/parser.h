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

  // this function is a slight misnomer, since it handles parens and prefix ops
  // the "value" part is just the first line, `tkn = lexer.next().value()`
  SyntaxNode *parse_value() {
    Token *tkn = lexer.next().value();
    SyntaxNode *val = nullptr;
    if (tkn->literal == "(") {
      uint16_t nest = nesting;
      ++nesting;
      val = parse();
      Token *nxt = lexer.next().value(); // advance / skip ")"
      --nesting;
      assert(nxt->literal == ")");
      assert(nesting == nest);
    } else if (tkn->literal == ")") {
      // error for now, maybe unit or something in the future...
      --nesting;
      return nullptr;
    } else if (prefix_ops.contains(tkn->literal)) {
      val = new SyntaxNode(tkn, std::vector<SyntaxNode*>{parse_value()});
    } else {
      val = new SyntaxNode(tkn);
    }
    return val;
  }

  // true if precedence(op) > precedence(last_op)
  // last_op being whichever op gave us min_precedence -- maybe we should just
  // change this so we pass the op itself.
  bool op_cmp(std::string op, uint32_t min_precedence) {
    auto [assoc, precedence] = binary_precedence[syntax_ids[op]];
    return assoc == Assoc::left && precedence <= min_precedence;
  }

  SyntaxNode *parse_increasing_precedence(SyntaxNode* left, uint32_t min_precedence) {
    if (!lexer.awaiting(2)
      || !bin_ops.contains(lexer.peek().value()->literal)
      || op_cmp(lexer.peek().value()->literal, min_precedence)
      //(lexer.peek().value()->literal) <= min_precedenc
      ) {
      return left;
    }
    Token *op = lexer.next().value();
    SyntaxNode *right = parse_expr(op_precedence(op->literal)); //(1)
    return new SyntaxNode(op, std::vector<SyntaxNode*>{left, right});
  }
  
  SyntaxNode *parse_expr(uint32_t min_precedence) {
    SyntaxNode *left = parse_value();

    while (true) {
      SyntaxNode* next = parse_increasing_precedence(left, min_precedence); // (2)
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
