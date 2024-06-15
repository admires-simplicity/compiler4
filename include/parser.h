#pragma once
#include <cstdint>
#include <map>
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
  "*",
};

std::map<std::string, uint32_t> syntax_ids = {
  {"<", 8},
  {">", 9},
  {"+", 10},
  {"*", 11},
};

std::map<uint32_t, BinPrecedence> binary_precedence = {
  {8, {Assoc::left, 4}},
  {9, {Assoc::left, 5}},
  {10, {Assoc::left, 6}},
  {11, {Assoc::left, 7}},
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
    //return parse_decreasing_precedence(0);
    //return right_leaning_crappy_parser();
    return parse(0);
    //return nullptr;
  }

private:

    // parses monotonic increasing precedence:
    // strictly increasing sequences parsed through the indirect recursive call
    // to parse_increasing_precedence within parse, giving right-leaning
    // (right-associative) trees, constant sequences parsed within the current
    // superordinate parse call, giving left-leaning (left-associative) trees.
    SyntaxNode *parse_increasing_precedence(uint32_t min_precedence) {
    SyntaxNode *left = new SyntaxNode(lexer.next().value());
    if (!lexer.awaiting(2)
      || !bin_ops.contains(lexer.peek().value()->literal)
      || op_precedence(lexer.peek().value()->literal) < min_precedence) {
      return left;
    }
    Token *op = lexer.next().value();
    SyntaxNode *right = parse(op_precedence(op->literal) + 1);
    return new SyntaxNode(op, std::vector<SyntaxNode*>{left, right});
  }

  SyntaxNode *parse(uint32_t min_precedence) {
    SyntaxNode *left = parse_increasing_precedence(min_precedence);
    if (!lexer.awaiting(2)
      || !bin_ops.contains(lexer.peek().value()->literal)
      || op_precedence(lexer.peek().value()->literal) < min_precedence) {
      return left;
    }
    while (lexer.awaiting(2) && bin_ops.contains(lexer.peek().value()->literal)) {
      Token *op = lexer.next().value();
      SyntaxNode *right = parse_increasing_precedence(op_precedence(op->literal) + 1);
      left = new SyntaxNode(op, std::vector<SyntaxNode*>{left, right});
    }
    return left;
  }

};








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
