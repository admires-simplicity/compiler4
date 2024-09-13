#pragma once
#include <cstdint>
#include <iostream>
#include <string>
#include <set>
#include <deque>
#include <map>
#include <optional>
#include <cassert>

class Token {
public:
  std::string literal;
  uint32_t line;
  uint32_t column;

  Token(std::string literal) : literal(literal) {}
  Token(std::string literal, uint32_t line, uint32_t column) : literal(literal), line(line), column(column) {}
};

class Lexer {
public:
  std::istream& istream;

  Lexer() : istream(std::cin) {}
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
    skip_whitespace_and_comments();

    if (istream.eof()) {
      return std::nullopt;
    }
    
    char c = istream.get();
    literal += c;

    bool not_num = true; // TODO: test this because I changed the logic on it

    // lex number
    // lexes a series of digits with at most one decimal point
    if (num(c)) {
      not_num = false;
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
    // lexes a series of characters between two quote marks
    else if (c == '"') {
      while (istream.peek() != '"') {
        c = istream.get();
        if (istream.eof()) break;
        literal += c;
      }
      c = istream.get(); // closing quote
      literal += c;
    }

    // lex identifier
    // lexes a series of characters that are not whitespace or syntax characters
    // and stops if it finds a coment
    else if (!syntax_chars.contains(c)) {
      char last_c = 0;
      while (!whitespace(istream.peek()) && !syntax_chars.contains(istream.peek())) {
        c = istream.get();
        if (istream.eof()) break;
        if ((c == '/' || c == '*') && last_c == '/') {
          literal = literal.substr(0, literal.size()-1);
          istream.putback(c);
          istream.putback(last_c);
          break;
        }
        literal += c;
        last_c = c;
      }
    }

    // else -- syntax char (do nothing)

    assert(literal.size() > 0); // sanity check

    return new Token(literal);
  }


  std::set<char> syntax_chars = {'(', ')', '[', ']', '.', ',', ';', };

  bool whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n';
  }

  void skip_whitespace() {
    while (whitespace(istream.peek())) {
      istream.get();
    }
  }

  void skip_whitespace_and_comments() {
    while (!istream.eof()) {
      skip_whitespace();
      if (istream.peek() != '/') {
        return;
      }
      char c1 = istream.get();
      if (istream.peek() != '/' && istream.peek() != '*') {
        istream.putback(c1);
        return;
      }

      char c2 = istream.get();

      // "//" comment
      if (c2 == '/') {
        while (istream.peek() != '\n' && !istream.eof()) {
          istream.get(); // consume next
        }
      }

      // "/*...*/" comment
      else if (c2 == '*') {
        c1 = istream.get(); // we have to consume 2 characters so that the loop condition
        c2 = istream.get(); // doesn't return false on the first iteration in
        // the condition that the comment has a / directly after the first *
        // like --> /*/
        while (!(c1 == '*' && c2 == '/') && !istream.eof()) {
          c1 = c2;
          c2 = istream.get(); // consume next
        }
      }

      else {
        istream.putback(c2);
        istream.putback(c1);
      }

      // after reading comment, we are either
      // (1) at eof
      // (2) at the beginning of a new line
      // in which case we are either
      // (2a) at whitespace
      // (2b) at a comment
      // (2c) at a valid lexeme
      // or we are
      // (3) at a valid lexeme
      
      // case (1) loop will terminate, since the loop condition is false
      // case (2a) we will skip whitespace and continue
      // case (2b) we will read another comment and continue
      // case (2c) we will return
      // case (3) we will return

      // so the next iteration of the loop should have valid effects
    }
  }

  bool num(char c) {
    return c >= '0' && c <= '9';
  }
};