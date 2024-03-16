#pragma once
#include <cstdint>
#include <iostream>
#include <string>
#include <set>
#include <queue>
#include <optional>

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

  bool nonempty() {
    return peek().has_value();
  }

  std::optional<Lexeme> peek() {
    lex_all();
    if (lexeme_queue.empty()) {
      return std::nullopt;
    }
    return lexeme_queue.front();
  }

  std::optional<Lexeme> next() {
    std::optional<Lexeme> lexeme = peek();
    if (lexeme.has_value()) {
      lexeme_queue.pop();
    }
    return lexeme;
  }

private:
  std::queue<Lexeme> lexeme_queue;

  int lex_all() {
    int count = 0;
    std::optional<Lexeme> next_lexeme;
    while (!istream.eof()) {
      next_lexeme = lex_next();
      if (next_lexeme.has_value()) lexeme_queue.push(*next_lexeme);
      count++;
    }
    return count;
  }

  std::optional<Lexeme> lex_next() {
    std::string literal;
    skip_whitespace();

    if (istream.eof()) {
      return std::nullopt;
    }
    
    char c = istream.get();
    literal += c;

    bool not_num = false;

    // lex number
    if (num(c)) {
      bool found_dot = false;
      while (num(istream.peek()) || (istream.peek() == '.' && !found_dot)) {
        c = istream.get();
        if (istream.eof()) break;
        if (c == '.') {
          found_dot = true;
        }
        literal += c;
      }
    }
    // lex string
    else if (c == '"') {
      while (istream.peek() != '"') {
        c = istream.get();
        if (istream.eof()) break;
        literal += c;
      }
      c = istream.get(); // closing quote
      if (istream.eof()) {
        return std::nullopt; // not sure if this is how i want to do this
      }
      literal += c;
    }
    // lex identifier
    else if (!syntax_chars.contains(c)) {
      // while (ident_char(istream.peek())) {
      while (!whitespace(istream.peek()) && !syntax_chars.contains(istream.peek())) {
        c = istream.get();
        if (istream.eof()) break;
        literal += c;
      }
    }
    // else -- syntax char (do nothing)
    
    if (literal == "//") {
      while (istream.peek() != '\n' && !istream.eof()) {
        c = istream.get();
      }
      return std::nullopt;
    }

    return Lexeme(not_num ? Lexeme::Type::identifier : Lexeme::Type::number, 0, 0, literal);
  }


  std::set<char> syntax_chars = {'(', ')', '[', ']', '.', ',', ';', '\n'};

  // std::set<std::string> reserved_lexemes = { // lexemes that cannot be within identifiers
  //   "(", ")", "[", "]", "+", "*", ".", ";",
  // };

  bool whitespace(char c) {
    return c == ' ' || c == '\t';
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

  // bool ident_char(char c) {
  //   //return !(whitespace(c) || syntax_chars.contains(c));
  //   return !whitespace(c) && !syntax_chars.contains(c) && !istream.eof();
  //   //return !(whitespace(c) || reserved_lexemes.contains(std::string(1, c)));
  // }
};