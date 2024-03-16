// #pragma once
// #include <vector>
// #include <string>
// #include <utility>
// #include <cstdint>

// #include "lexer.h"

// class Token {
// public:
//   enum class Type {
//     // identifier,
//     // number,
//     // plus,
//     // multiply,
//     number,
//     literal,
//   };

//   Type type;
//   std::string literal;
//   std::pair<uint32_t, uint32_t> position;

//   Token(Type type, std::string literal, uint32_t line, uint32_t column) : type(type), literal(literal), position(std::make_pair(line, column)) {}
// };

// class Tokenizer {
// public:
//   Lexer lexer;

//   Tokenizer(Lexer lexer) : lexer(lexer) {}
// };

// Token *tokenize(Lexeme lexeme) {
//   if (lexeme.type == Lexeme::Type::number) {
//     return new Token(Token::Type::number, lexeme.literal, lexeme.line, lexeme.column);
//   } else {
//     return new Token(Token::Type::literal, lexeme.literal, lexeme.line, lexeme.column);
//   }
// }