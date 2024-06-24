#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdint>

#include "lexer.h"
#include "parser.h"

typedef struct {
  std::string expr_str;
  std::string parse_str;
} test_case;

std::string get_canonical_parse_str(std::string s) {
  return Parser(Lexer(std::istringstream(s))).parse()->to_string();
}

const int VERBOSE = 1;

int64_t read_flags(int argc, char** argv) {
  int64_t flags = 0;
  for (int i = 1; i < argc; i++) {
    if (std::string(argv[i]) == "-v") {
      flags |= VERBOSE;
    }
  }
  return flags;
}

int main(int argc, char** argv) {
  const int64_t flags = read_flags(argc, argv); 

  const std::vector<test_case> test_cases = {
    // value test
    {"1", "(1)"},

    // plus test
    {"1 + 2", {"(+ (1) (2))"}},
    
    // + * assoc tests
    {"1 + 2 + 3", "(+ (+ (1) (2)) (3))"},
    {"1 * 2 + 3", "(+ (* (1) (2)) (3))"},
    {"1 + 2 * 3", "(+ (1) (* (2) (3)))"},
    {"1 * 2 * 3", "(* (* (1) (2)) (3))"},

    // 3 op test
    {"1 + 2 + 3 + 4", "(+ (+ (+ (1) (2)) (3)) (4))"},

    // 3 op + * assoc tests
    {"1 + 2 + 3 * 4", "(+ (+ (1) (2)) (* (3) (4)))"},
    {"1 + 2 * 3 + 4", "(+ (+ (1) (* (2) (3))) (4))"},
    {"1 * 2 + 3 + 4", "(+ (+ (* (1) (2)) (3)) (4))"},

    {"1 + 2 * 3 * 4", "(+ (1) (* (* (2) (3)) (4)))"},
    {"1 * 2 + 3 * 4", "(+ (* (1) (2)) (* (3) (4)))"},
    {"1 * 2 * 3 + 4", "(+ (* (* (1) (2)) (3)) (4))"},

    {"1 * 2 * 3 * 4", "(* (* (* (1) (2)) (3)) (4))"},

    // various multi op + * > < tests
    {"1 + 2 * 3 > 4 + 5 * 6", "(> (+ (1) (* (2) (3))) (+ (4) (* (5) (6))))"},
    {"1 * 2 + 3 < 4 * 5 + 6", "(< (+ (* (1) (2)) (3)) (+ (* (4) (5)) (6)))"},
    {"1 + 2 * 3 > 4 * 5 + 6", "(> (+ (1) (* (2) (3))) (+ (* (4) (5)) (6)))"},
    {"1 * 2 + 3 < 4 + 5 * 6", "(< (+ (* (1) (2)) (3)) (+ (4) (* (5) (6))))"},
    {"1 + 2 * 3 + 4 * 5 + 6 > 7 * 8 + 9 * 10 + 11 * 12",
    "(> (+ (+ (+ (1) (* (2) (3))) (* (4) (5))) (6)) (+ (+ (* (7) (8)) (* (9) (10))) (* (11) (12))))"},

    // paren value tests
    {"(1)", "(1)"},
    {"((2))", "(2)"},
    {"(((3)))", "(3)"},

    // paren binop tests
    {"(1 + 2)", "(+ (1) (2))"},
    {"((1 + 2))", "(+ (1) (2))"},

    // paren value in expr tests
    {"1 + (2)", "(+ (1) (2))"},
    {"1 + ((2))", "(+ (1) (2))"},
    {"(1) + 2", "(+ (1) (2))"},
    {"((1)) + 2", "(+ (1) (2))"},

    // 1 op paren assoc tests
    {"(1 + 2) + 3", "(+ (+ (1) (2)) (3))"},
    {"1 + (2 + 3)", "(+ (1) (+ (2) (3)))"},
    {"(1 + 2) + 3 + 4", "(+ (+ (+ (1) (2)) (3)) (4))"},
    {"1 + (2 + 3) + 4", "(+ (+ (1) (+ (2) (3))) (4))"},
    {"1 + 2 + (3 + 4)", "(+ (+ (1) (2)) (+ (3) (4)))"},

    // + * paren assoc tests
    {"1 * (2 + 3) + 4", "(+ (* (1) (+ (2) (3))) (4))"},
    {"1 + (2 + 3) * 4", "(+ (1) (* (+ (2) (3)) (4)))"},
    {"1 * (2 + 3) * 4", "(* (* (1) (+ (2) (3))) (4))"},

    // > < same precedence tests
    {"1 < 2 < 3", "(< (< (1) (2)) (3))"},
    {"1 > 2 > 3", "(> (> (1) (2)) (3))"},
    {"1 < 2 > 3", "(> (< (1) (2)) (3))"},
    {"1 > 2 < 3", "(< (> (1) (2)) (3))"},

    // + - same precedence tests
    {"1 + 2 - 3", "(- (+ (1) (2)) (3))"},
    {"1 - 2 + 3", "(+ (- (1) (2)) (3))"},
    {"1 + 2 + 3 - 4", "(- (+ (+ (1) (2)) (3)) (4))"},
    {"1 - 2 + 3 + 4", "(+ (+ (- (1) (2)) (3)) (4))"},
    {"1 + 2 - 3 + 4", "(+ (- (+ (1) (2)) (3)) (4))"},
    {"1 - 2 - 3 + 4", "(+ (- (- (1) (2)) (3)) (4))"},
    {"1 + 2 - 3 - 4", "(- (- (+ (1) (2)) (3)) (4))"},
    {"1 - 2 + 3 - 4", "(- (+ (- (1) (2)) (3)) (4))"},

    // * / same precedence tests
    {"1 * 2 / 3", "(/ (* (1) (2)) (3))"},
    {"1 / 2 * 3", "(* (/ (1) (2)) (3))"},
    {"1 * 2 * 3 / 4", "(/ (* (* (1) (2)) (3)) (4))"},
    {"1 / 2 * 3 * 4", "(* (* (/ (1) (2)) (3)) (4))"},
    {"1 * 2 / 3 * 4", "(* (/ (* (1) (2)) (3)) (4))"},
    {"1 / 2 / 3 * 4", "(* (/ (/ (1) (2)) (3)) (4))"},
    {"1 * 2 / 3 / 4", "(/ (/ (* (1) (2)) (3)) (4))"},
    {"1 / 2 * 3 / 4", "(/ (* (/ (1) (2)) (3)) (4))"},

    // various + - * / tests
    {"1 + 2 - 3 * 4 / 5 + 6", "(+ (- (+ (1) (2)) (/ (* (3) (4)) (5))) (6))"},
    {"1 - 2 + 3 / 4 * 5 - 6", "(- (+ (- (1) (2)) (* (/ (3) (4)) (5))) (6))"},
    {"1 / 2 - 3 + 4 - 5 * 6", "(- (+ (- (/ (1) (2)) (3)) (4)) (* (5) (6)))"},
    {"1 * 2 + 3 - 4 + 5 / 6", "(+ (- (+ (* (1) (2)) (3)) (4)) (/ (5) (6)))"},

    // unary - ! tests
    {"- 1", "(- (1))"},
    {"- 1 + 2", "(+ (- (1)) (2))"},
    {"1 + - 2", "(+ (1) (- (2)))"},
    {"- (1 + 2)", "(- (+ (1) (2)))"},
    // can't do "-1" because '-' is not a syntax char
    // {"! 1", "(! (1))"},
    // {"! (1)", "(! (1))"},
    // {"! 1 + 1", }

    // ^ tests (right associative)
    {"2 ^ 3", "(^ (2) (3))"},
    {"1 ^ 2 ^ 3", "(^ (1) (^ (2) (3)))"},
    {"1 ^ 2 ^ 3 ^ 4", "(^ (1) (^ (2) (^ (3) (4))))"},
    {"1 + 2 * 3 ^ 4", "(+ (1) (* (2) (^ (3) (4))))"},
    {"1 ^ 2 * 3 + 4", "(+ (* (^ (1) (2)) (3)) (4))"},

    {"1 + 2 ^ 3 ^ 4", "(+ (1) (^ (2) (^ (3) (4))))"},
    {"1 ^ 2 ^ 3 + 4", "(+ (^ (1) (^ (2) (3))) (4))"},
    {"1 * 2 ^ 3 ^ 4", "(* (1) (^ (2) (^ (3) (4))))"},
    {"1 ^ 2 ^ 3 * 4", "(* (^ (1) (^ (2) (3))) (4))"},

    {"1 + 2 ^ 3 + 4 * 5 ^ 6 * 7 ^ 8 ^ 9 * 10",
    "(+ (+ (1) (^ (2) (3))) (* (* (* (4) (^ (5) (6))) (^ (7) (^ (8) (9)))) (10)))"},
 };

  // const std::vector<test_case> test_cases = {
  //   {"1", "(1)"},

  //   {"1 + 2", {"(+ (1) (2))"}},

  //   {"1 + 2 + 3", "(+ (1) (+ (2) (3)))"},
  //   {"1 + 2 * 3", "(+ (1) (* (2) (3)))"},
  //   {"1 * 2 + 3", "(+ (* (1) (2)) (3))"},
  //   {"1 * 2 * 3", "(* (1) (* (2) (3)))"},

  //   {"1 + 2 + 3 + 4", "(+ (1) (+ (2) (+ (3) (4))))"},

  //   {"1 + 2 + 3 * 4", "(+ (1) (+ (2) (* (3) (4))))"},
  //   {"1 + 2 * 3 + 4", "(+ (1) (+ (* (2) (3)) (4)))"},
  //   {"1 * 2 + 3 + 4", "(+ (* (1) (2)) (+ (3) (4)))"},

  //   {"1 + 2 * 3 * 4", "(+ (1) (* (2) (* (3) (4))))"},
  //   {"1 + 2 * 3 * 4", "(+ (1) (* (2) (* (3) (4))))"},
  //   {"1 * 2 + 3 * 4", "(+ (* (1) (2)) (* (3) (4))"},
  //   {"1 * 2 * 3 + 4", "(+ (* (1) (* (2) (3))) (4))"},

  //   {"1 * 2 * 3 * 4", "(* (1) (* (2) (* (3) (4))))"},
  // };

  int wrong = 0;
  int correct = 0;
  const int cases = test_cases.size();
  for (int i = 0; i < cases; i++) {
    auto [expr_str, parse_str] = test_cases[i];
    std::string res = get_canonical_parse_str(expr_str);
    if (res != parse_str) {
      ++wrong;
      std::cout << "[" << i << "] Fail " << wrong << ": " << expr_str << " parses to " << res << ", expected " << parse_str << std::endl;
    } else if (flags & VERBOSE) {
      ++correct;
      std::cout << "[" << i << "] Pass " << correct << ": " << expr_str << " parses to " << res << std::endl;
    }
  }
  std::cout << cases - wrong << "/" << cases << " passed" << std::endl;
  if (wrong) std::cout << wrong << " failed" << std::endl;
  return wrong;
}