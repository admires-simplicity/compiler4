#pragma once
#include <unordered_map>
#include <initializer_list>

template <typename A, typename B>
class Bimap {
  std::unordered_map<A, B> a_to_b;
  std::unordered_map<B, A> b_to_a;
public:
  Bimap(std::initializer_list<std::pair<A, B>> pairs) {
    for (auto [a, b] : pairs) {
      a_to_b[a] = b;
      b_to_a[b] = a;
    }
  }

  Bimap(std::initializer_list<std::pair<B, A>> pairs) {
    for (auto [b, a] : pairs) {
      a_to_b[a] = b;
      b_to_a[b] = a;
    }
  }

  void insert(A a, B b) {
    a_to_b[a] = b;
    b_to_a[b] = a;
  }

  void insert(B b, A a) {
    a_to_b[a] = b;
    b_to_a[b] = a;
  }

  B lookup(A a) {
    return a_to_b[a];
  }

  A lookup(B b) {
    return b_to_a[b];
  }

  A operator[](B b) {
    return lookup(b);
  }

  B operator[](A a) {
    return lookup(a);
  }
};