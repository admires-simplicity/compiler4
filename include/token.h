#include <vector>
#include <string>

class Token {
public:
  enum class Type {
    identifier,
    number,
    plus,
    multiply,
  };

  Type type;
  std::string literal;
  std::vector<Token *> children;

  Token(Type type, std::string literal) : type(type), literal(literal) {}

  std::string to_string() {
    std::string s = "";
    s += "(";
    s += literal;
    for (auto child : children) {
      s += " ";
      s += child->to_string();
    }
    s += ")";
    return s;
  }

};

struct Token_ {
  Token::Type type;
  std::string literal;
  std::vector<Token_> children;
};
