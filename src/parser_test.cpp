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
    {"1", "(1)"},

    {"1 + 2", {"(+ (1) (2))"}},

    {"1 + 2 + 3", "(+ (+ (1) (2)) (3))"},
    {"1 * 2 + 3", "(+ (* (1) (2)) (3))"},
    {"1 + 2 * 3", "(+ (1) (* (2) (3)))"},
    {"1 * 2 * 3", "(* (* (1) (2)) (3))"},

    {"1 + 2 + 3 + 4", "(+ (+ (+ (1) (2)) (3)) (4))"},

    {"1 + 2 + 3 * 4", "(+ (+ (1) (2)) (* (3) (4)))"},
    {"1 + 2 * 3 + 4", "(+ (+ (1) (* (2) (3))) (4))"},
    {"1 * 2 + 3 + 4", "(+ (+ (* (1) (2)) (3)) (4))"},

    {"1 + 2 * 3 * 4", "(+ (1) (* (* (2) (3)) (4)))"},
    {"1 * 2 + 3 * 4", "(+ (* (1) (2)) (* (3) (4)))"},
    {"1 * 2 * 3 + 4", "(+ (* (* (1) (2)) (3)) (4))"},

    {"1 * 2 * 3 * 4", "(* (* (* (1) (2)) (3)) (4))"},

    {"1 + 2 * 3 > 4 + 5 * 6", "(> (+ (1) (* (2) (3))) (+ (4) (* (5) (6))))"},
    {"1 * 2 + 3 < 4 * 5 + 6", "(< (+ (* (1) (2)) (3)) (+ (* (4) (5)) (6)))"},
    {"1 + 2 * 3 > 4 * 5 + 6", "(> (+ (1) (* (2) (3))) (+ (* (4) (5)) (6)))"},
    {"1 * 2 + 3 < 4 + 5 * 6", "(< (+ (* (1) (2)) (3)) (+ (4) (* (5) (6))))"},

    {"1 + 2 * 3 + 4 * 5 + 6 > 7 * 8 + 9 * 10 + 11 * 12",
    "(> (+ (+ (+ (1) (* (2) (3))) (* (4) (5))) (6)) (+ (+ (* (7) (8)) (* (9) (10))) (* (11) (12))))"},

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