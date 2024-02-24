#include <cstdint>
#include <iostream>
#include <string>
#include <set>

class Lexeme {
public:
  enum class Type {
    identifier,
    number,
    //syntax,
  };

  Type type;
  uint32_t line;
  uint32_t column;
  std::string literal;

  Lexeme(Type type, uint32_t line, uint32_t column, std::string literal) : type(type), line(line), column(column), literal(literal) {}
};

class Lexer {
public:
  

  std::istream &istream;

  Lexer(std::istream &istream) : istream(istream) {}

  Lexeme next() {
    std::string literal;
    skip_whitespace();
    char c = istream.get();
    literal += c;

    bool not_num = false;



    // // lex number
    // if (num(c)) {
    //   while (num(istream.peek()) || istream.peek() == '.') {
    //     c = istream.get();
    //     lexeme += c;
    //   }
    // }
    // // lex identifier
    // else {
    //   while (ident_char(istream.peek())) {
    //     c = istream.get();
    //     lexeme += c;
    //   }
    // }
    // // else -- syntax char (do nothing)

    // return lexeme;
  }

private:
  std::set<char> syntax_chars = {'(', ')', '[', ']', '.', ';'};

  // std::set<std::string> reserved_lexemes = { // lexemes that cannot be within identifiers
  //   "(", ")", "[", "]", "+", "*", ".", ";",
  // };

  bool whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n';
  }

  void skip_whitespace() {
    while (whitespace(istream.peek())) {
      istream.get();
    }
  }

  // bool alpha(char c) {
  //   return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
  // }

  bool num(char c) {
    return c >= '0' && c <= '9';
  }

  bool ident_char(char c) {
    //return !(whitespace(c) || syntax_chars.contains(c));
    return !whitespace(c) && !syntax_chars.contains(c);
    //return !(whitespace(c) || reserved_lexemes.contains(std::string(1, c)));
  }
};