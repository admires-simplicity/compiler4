#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "lexer.h"
#include "parser.h"

typedef struct {
  std::string expr_str;
  std::string parse_str;
} test_case;

std::string get_canonical_parse_str(std::string s) {
  return Parser(Lexer(std::istringstream(s))).parse()->to_string();
}

int main() {
  const std::vector<test_case> test_cases = {
    {"1", "(1)"},
    {"1 + 2", {"(+ (1) (2))"}},
    {"1 + 2 + 3", "(+ (+ (1) (2)) (3))"},
  };

  int wrong = 0;
  const int cases = test_cases.size();
  for (int i = 0; i < cases; i++) {
    auto [expr_str, parse_str] = test_cases[i];
    std::string res = get_canonical_parse_str(expr_str);
    if (res != parse_str) {
      wrong++;
      std::cout << "Fail " << wrong << ": " << expr_str << " parses to " << res << ", expected " << parse_str << std::endl;
    }
  }
  std::cout << cases - wrong << "/" << cases << " passed" << std::endl;
  if (wrong) std::cout << wrong << " failed" << std::endl;
  return wrong;
}