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

std::map<std::string, uint32_t> syntax_ids = {
  {"<", 8},
  {">", 9},
  {"+", 10},
  {"*", 11},
};

std::map<uint32_t, BinPrecedence> binary_precedence = {
  {8, {Assoc::left, 4}},
  {9, {Assoc::left, 4}},
  {10, {Assoc::left, 5}},
  {11, {Assoc::left, 6}},
};

class Parser {
public:
  Lexer lexer;

  Parser(Lexer lexer) : lexer(lexer) {}

  SyntaxNode *parse_increasing_precedence(uint32_t min_precedence) {
    std::optional<Lexeme> leftLexeme = lexer.next();

    if (!leftLexeme.has_value()) {
      return nullptr;
    }

    
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


