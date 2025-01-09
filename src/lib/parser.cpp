#include <parser.h>

std::set<std::string> bin_ops = {
  "<",
  ">",
  "==",
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

  {"==", 301},

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

std::set<std::string> special_bin_ops = {
  "then",
  "else",

  "=",
};

std::set<std::string> special_prefix_ops = {
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

  {110, {Assoc::left, 1}}, // ';'

  {300, {Assoc::left, 2}}, // 'return'

  {3, {Assoc::right, 3}}, // ','


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

  {301, {Assoc::left, 11}},
  
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

