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
    {"1", "1"},

    // plus test
    {"1 + 2", {"(+ 1 2)"}},
    
    // + * assoc tests
    {"1 + 2 + 3", "(+ (+ 1 2) 3)"},
    {"1 * 2 + 3", "(+ (* 1 2) 3)"},
    {"1 + 2 * 3", "(+ 1 (* 2 3))"},
    {"1 * 2 * 3", "(* (* 1 2) 3)"},

    // 3 op test
    {"1 + 2 + 3 + 4", "(+ (+ (+ 1 2) 3) 4)"},

    // 3 op + * assoc tests
    {"1 + 2 + 3 * 4", "(+ (+ 1 2) (* 3 4))"},
    {"1 + 2 * 3 + 4", "(+ (+ 1 (* 2 3)) 4)"},
    {"1 * 2 + 3 + 4", "(+ (+ (* 1 2) 3) 4)"},

    {"1 + 2 * 3 * 4", "(+ 1 (* (* 2 3) 4))"},
    {"1 * 2 + 3 * 4", "(+ (* 1 2) (* 3 4))"},
    {"1 * 2 * 3 + 4", "(+ (* (* 1 2) 3) 4)"},

    {"1 * 2 * 3 * 4", "(* (* (* 1 2) 3) 4)"},

    // various multi op + * > < tests
    {"1 + 2 * 3 > 4 + 5 * 6", "(> (+ 1 (* 2 3)) (+ 4 (* 5 6)))"},
    {"1 * 2 + 3 < 4 * 5 + 6", "(< (+ (* 1 2) 3) (+ (* 4 5) 6))"},
    {"1 + 2 * 3 > 4 * 5 + 6", "(> (+ 1 (* 2 3)) (+ (* 4 5) 6))"},
    {"1 * 2 + 3 < 4 + 5 * 6", "(< (+ (* 1 2) 3) (+ 4 (* 5 6)))"},
    {"1 + 2 * 3 + 4 * 5 + 6 > 7 * 8 + 9 * 10 + 11 * 12",
    "(> (+ (+ (+ 1 (* 2 3)) (* 4 5)) 6) (+ (+ (* 7 8) (* 9 10)) (* 11 12)))"},

    // paren value tests
    {"1", "1"},
    {"(2)", "2"},
    {"((3))", "3"},

    // paren binop tests
    {"(1 + 2)", "(+ 1 2)"},
    {"((1 + 2))", "(+ 1 2)"},

    // paren value in expr tests
    {"1 + 2", "(+ 1 2)"},
    {"1 + (2)", "(+ 1 2)"},
    {"1 + 2", "(+ 1 2)"},
    {"(1) + 2", "(+ 1 2)"},

    // 1 op paren assoc tests
    {"(1 + 2) + 3", "(+ (+ 1 2) 3)"},
    {"1 + (2 + 3)", "(+ 1 (+ 2 3))"},
    {"(1 + 2) + 3 + 4", "(+ (+ (+ 1 2) 3) 4)"},
    {"1 + (2 + 3) + 4", "(+ (+ 1 (+ 2 3)) 4)"},
    {"1 + 2 + (3 + 4)", "(+ (+ 1 2) (+ 3 4))"},

    // + * paren assoc tests
    {"1 * (2 + 3) + 4", "(+ (* 1 (+ 2 3)) 4)"},
    {"1 + (2 + 3) * 4", "(+ 1 (* (+ 2 3) 4))"},
    {"1 * (2 + 3) * 4", "(* (* 1 (+ 2 3)) 4)"},

    // > < same precedence tests
    {"1 < 2 < 3", "(< (< 1 2) 3)"},
    {"1 > 2 > 3", "(> (> 1 2) 3)"},
    {"1 < 2 > 3", "(> (< 1 2) 3)"},
    {"1 > 2 < 3", "(< (> 1 2) 3)"},

    // + - same precedence tests
    {"1 + 2 - 3", "(- (+ 1 2) 3)"},
    {"1 - 2 + 3", "(+ (- 1 2) 3)"},
    {"1 + 2 + 3 - 4", "(- (+ (+ 1 2) 3) 4)"},
    {"1 - 2 + 3 + 4", "(+ (+ (- 1 2) 3) 4)"},
    {"1 + 2 - 3 + 4", "(+ (- (+ 1 2) 3) 4)"},
    {"1 - 2 - 3 + 4", "(+ (- (- 1 2) 3) 4)"},
    {"1 + 2 - 3 - 4", "(- (- (+ 1 2) 3) 4)"},
    {"1 - 2 + 3 - 4", "(- (+ (- 1 2) 3) 4)"},

    // * / same precedence tests
    {"1 * 2 / 3", "(/ (* 1 2) 3)"},
    {"1 / 2 * 3", "(* (/ 1 2) 3)"},
    {"1 * 2 * 3 / 4", "(/ (* (* 1 2) 3) 4)"},
    {"1 / 2 * 3 * 4", "(* (* (/ 1 2) 3) 4)"},
    {"1 * 2 / 3 * 4", "(* (/ (* 1 2) 3) 4)"},
    {"1 / 2 / 3 * 4", "(* (/ (/ 1 2) 3) 4)"},
    {"1 * 2 / 3 / 4", "(/ (/ (* 1 2) 3) 4)"},
    {"1 / 2 * 3 / 4", "(/ (* (/ 1 2) 3) 4)"},

    // various + - * / tests
    {"1 + 2 - 3 * 4 / 5 + 6", "(+ (- (+ 1 2) (/ (* 3 4) 5)) 6)"},
    {"1 - 2 + 3 / 4 * 5 - 6", "(- (+ (- 1 2) (* (/ 3 4) 5)) 6)"},
    {"1 / 2 - 3 + 4 - 5 * 6", "(- (+ (- (/ 1 2) 3) 4) (* 5 6))"},
    {"1 * 2 + 3 - 4 + 5 / 6", "(+ (- (+ (* 1 2) 3) 4) (/ 5 6))"},

    // unary - ! tests
    {"- 1", "(- 1)"},
    {"- 1 + 2", "(+ (- 1) 2)"},
    {"1 + - 2", "(+ 1 (- 2))"},
    {"- (1 + 2)", "(- (+ 1 2))"},
    // can't do "-1" because '-' is not a syntax char
    // {"! 1", "(! 1)"},
    // {"! 1", "(! 1)"},
    // {"! 1 + 1", }

    // ^ tests (right associative)
    {"2 ^ 3", "(^ 2 3)"},
    {"1 ^ 2 ^ 3", "(^ 1 (^ 2 3))"},
    {"1 ^ 2 ^ 3 ^ 4", "(^ 1 (^ 2 (^ 3 4)))"},
    {"1 + 2 * 3 ^ 4", "(+ 1 (* 2 (^ 3 4)))"},
    {"1 ^ 2 * 3 + 4", "(+ (* (^ 1 2) 3) 4)"},

    {"1 + 2 ^ 3 ^ 4", "(+ 1 (^ 2 (^ 3 4)))"},
    {"1 ^ 2 ^ 3 + 4", "(+ (^ 1 (^ 2 3)) 4)"},
    {"1 * 2 ^ 3 ^ 4", "(* 1 (^ 2 (^ 3 4)))"},
    {"1 ^ 2 ^ 3 * 4", "(* (^ 1 (^ 2 3)) 4)"},

    {"1 + 2 ^ 3 + 4 * 5 ^ 6 * 7 ^ 8 ^ 9 * 10",
    "(+ (+ 1 (^ 2 3)) (* (* (* 4 (^ 5 6)) (^ 7 (^ 8 9))) 10))"},

    // = arithmetic tests
    {"1 + 2 ^ 3 * 4 = 33", "(= (+ 1 (* (^ 2 3) 4)) 33)"},
    {"1 + 1 = 1 * 1 = 1 ^ 1", "(= (= (+ 1 1) (* 1 1)) (^ 1 1))"},

    // -> predicate tests
    {"A -> B", "(-> A B)"},
    {"A -> B -> C", "(-> A (-> B C))"},
    {"A -> B -> C -> D", "(-> A (-> B (-> C D)))"},
    {"(A -> B) -> (C -> D -> E) -> F", "(-> (-> A B) (-> (-> C (-> D E)) F))"},

    // =, -> mix test (incomplete syntax)
    {"A -> B -> C = D -> E", "(= (-> A (-> B C)) (-> D E))"},

    // : tests
    {"A : B", "(: A B)"},
    {"A : B : C", "(: A (: B C))"},

    // , tests
    {"A , B", "(, A B)"},
    {"A , B , C", "(, A (, B C))"},

    // : , mix test
    {"(A : B, C : D)", "(, (: A B) (: C D))"},
    {"(A : B, C : D, E : F)", "(, (: A B) (, (: C D) (: E F)))"},

    // call tests
    {"f()", "(apply f)"},
    {"f(x)", "(apply f x)"},
    {"f(x, y)", "(apply f (, x y))"},
    {"f(x, y, z)", "(apply f (, x (, y z)))"},
    
    // string tests
    {"\"string\"", "\"string\""}, // this works trivially because I haven't actually made a distinction between identifiers and strings yet... lol

    // printf test
    {"printf(\"hello, world\")", "(apply printf \"hello, world\")"},
    {"printf(\"display some number: %d\\n\", 123);", "(; (apply printf (, \"display some number: %d\\n\" 123)))"},

    // parse semicolon
    {"1 + 1;", "(; (+ 1 1))"},

    // parse block
    {"{ printf(\"first printf\\n\"); printf(\"second printf\\n\"); }", "(block (; (apply printf \"first printf\\n\")) (; (apply printf \"second printf\\n\")))"},
    {"{ printf(\"first printf\\n\"); printf(\"second printf\\n\")  }", "(block (; (apply printf \"first printf\\n\")) (apply printf \"second printf\\n\"))"},
    
    // function type signature test
    {"f(x : int, y : int) -> int", "(-> (apply f (, (: x int) (: y int))) int)"},

    // function definition parse tests
    {"f(x) = { x; }", "(= (apply f x) (block (; x)))"}, // TODO (maybe): change parser to use something like "defun" instead of "apply" ?
    {"f(x : int, y : int) -> int = { x + y }", "(= (-> (apply f (, (: x int) (: y int))) int) (block (+ x y)))"},

    // let test
    {"let x = 1", "(let (= x 1))"},
    {"let x = 1;", "(; (let (= x 1)))"},

    // let with type
    {"let x : int = 1", "(let (= (: x int) 1))"},
    {"let x : int = 1;", "(; (let (= (: x int) 1)))"},

    // var tests
    {"var x = 1", "(var (= x 1))"},
    {"var x = 1;", "(; (var (= x 1)))"},

    // var tests with type
    {"var x : int = 1", "(var (= (: x int) 1))"},
    {"var x : int = 1;", "(; (var (= (: x int) 1)))"},

    // fn tests
    {"fn f(x) { x; }", "(fn (apply f x) (block (; x)))"},
    {"fn f(x : int, y : int) -> int { x + y }", "(fn (-> (apply f (, (: x int) (: y int))) int) (block (+ x y)))"},
    // I originally made the "fn" prefix in order to distinguish between the
    // "apply" token/operator and function application, but actually I still
    // have to use apply here, so that doesn't even matter...
    // IDK if I will want to use "fn <sig> <body>", or just "<sig> = <body>"
    {"fn f(x : int, y : int) -> int { \n\
        a : int = x + y; \n\
        b : int = x * y; \n\
        a + b }", "(fn (-> (apply f (, (: x int) (: y int))) int) (block (; (= (: a int) (+ x y))) (; (= (: b int) (* x y))) (+ a b)))"},
 };

  // const std::vector<test_case> test_cases = {
  //   {"1", "1"},

  //   {"1 + 2", {"(+ 1 2)"}},

  //   {"1 + 2 + 3", "(+ 1 (+ 2 3))"},
  //   {"1 + 2 * 3", "(+ 1 (* 2 3))"},
  //   {"1 * 2 + 3", "(+ (* 1 2) 3)"},
  //   {"1 * 2 * 3", "(* 1 (* 2 3))"},

  //   {"1 + 2 + 3 + 4", "(+ 1 (+ 2 (+ 3 4)))"},

  //   {"1 + 2 + 3 * 4", "(+ 1 (+ 2 (* 3 4)))"},
  //   {"1 + 2 * 3 + 4", "(+ 1 (+ (* 2 3) 4))"},
  //   {"1 * 2 + 3 + 4", "(+ (* 1 2) (+ 3 4))"},

  //   {"1 + 2 * 3 * 4", "(+ 1 (* 2 (* 3 4)))"},
  //   {"1 + 2 * 3 * 4", "(+ 1 (* 2 (* 3 4)))"},
  //   {"1 * 2 + 3 * 4", "(+ (* 1 2) (* 3 4)"},
  //   {"1 * 2 * 3 + 4", "(+ (* 1 (* 2 3)) 4)"},

  //   {"1 * 2 * 3 * 4", "(* 1 (* 2 (* 3 4)))"},
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