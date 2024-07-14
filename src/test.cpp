#include <map>

class Precedence {
public:
  int precedence;
};

class BinPrecedence : Precedence {
public:
  enum class Assoc {
    left,
    right,
  };
  Assoc assoc;
};

class UnaryPrecedence : Precedence {
};

int main() {
  return 0;
}