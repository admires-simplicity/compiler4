#pragma once
#include <cstdint>
#include <iostream>
#include <string>
#include <set>
#include <deque>
#include <optional>

class Token {
public:
  enum class Type {
    identifier,
    number,
    //syntax,
  };

  Type type;
  std::string literal;
  uint32_t line;
  uint32_t column;

  Token(Type type, std::string literal, uint32_t line, uint32_t column) : type(type), literal(literal), line(line), column(column) {}
};

class Lexer {
public:
  std::istream& istream;

  Lexer(std::istream& istream) : istream(istream) {}
  Lexer(std::istream&& istream) : istream(istream) {}

  bool nonempty() {
    return peek().has_value();
  }

  std::optional<Token*> peek() {
    return peek(0);
  }

  std::optional<Token*> peek(int i) {
    lex_all();
    if (lexeme_deque.size() <= i) {
      return std::nullopt;
    }
    return lexeme_deque[i];
  }

  // true if there are at least n lexemes in the stream to be processed
  bool awaiting(uint32_t n) {
    lex_all();
    return lexeme_deque.size() >= n;
  }

  std::optional<Token*> next() {
    std::optional<Token*> lexeme = peek();
    if (lexeme.has_value()) {
      lexeme_deque.pop_front();
    }
    return lexeme;
  }

private:
  std::deque<Token*> lexeme_deque;

  int lex_all() {
    int count = 0;
    std::optional<Token*> next_lexeme;
    while (!istream.eof()) {
      next_lexeme = lex_next();
      if (next_lexeme.has_value()) lexeme_deque.push_back(*next_lexeme);
      count++;
    }
    return count;
  }

  std::optional<Token*> lex_next() {
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

    return new Token(not_num ? Token::Type::identifier : Token::Type::number, literal, 0, 0);
  }


  std::set<char> syntax_chars = {'(', ')', '[', ']', '.', ',', ';', '\n',
    // // arithmetic ops
    // '-', '+', '*', '/', '>', '<', '^', '=', 
    // // logical ops
    // '!',
  };

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