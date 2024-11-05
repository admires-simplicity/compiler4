#include <map>
#include <string>
#include <cassert>
#include <functional>
#include <optional>

#include "types.h"
#include "options.h"
#include "util.h"

#include <array>
constexpr std::array<std::string, 13> basic_types = { // TODO: turn this into static local reference initialization
  "unassigned type", // first type a value gets on parsing/lexing
  "dynamic",

  "void",
  "unit",

  // numeric types
  "num",
  "int",
  "i32",
  "i64",
  "float",
  "f32",
  "f64",
  // "rat", // might be too based for the world to handle...
  // "real", // is actually a dependent type (int -> rat)

  "char",
  "bool",
};

constexpr std::array<std::string, 2> syntax_types = {
  "type",
  "identifier",
};

template<typename K, typename V, std::size_t N>
void add_arr_to_map(
    std::map<K, V>& m,
    const std::array<std::string, N>& arr,
    std::function<void(std::map<K, V>&, std::string, int)> f) {
  for (const auto& type_name : arr) {
    f(m, type_name, m.size());
  }
}

template<typename K, typename V>
std::map<K, V> make_map(std::function<void(std::map<K, V>&, std::string, int)> f) {
  std::map<K, V> m;
  auto add = [&](const auto& arr) {
    add_arr_to_map(m, arr, f);
  };
  add(basic_types);
  add(syntax_types);
  return m;
}

std::map<int, std::string>& TypeSet::get_id_to_type() {
  static std::map<int, std::string> id_to_type =
  make_map<int, std::string>([](auto& m, auto type_name, auto id) {
    m[id] = type_name;
  });
  return id_to_type;
}

std::map<std::string, int>& TypeSet::get_type_to_id() {
  static std::map<std::string, int> type_to_id =
  make_map<std::string, int>([](auto& m, auto type_name, auto id) {
    m[type_name] = id;
  });
  return type_to_id;
}

// TODO: this is wrong because we should break it up into internal_type and user_type
bool TypeSet::is_type(std::string type) {
  return get_type_to_id().contains(type);
}

bool TypeSet::add_type(std::string type) {
  if (is_type(type)) return false;
  int id = get_id_to_type().size();
  get_id_to_type()[id] = type;
  get_type_to_id()[type] = id;
  return true;
}

int TypeSet::get_id(std::string type) {
  return get_type_to_id()[type];
}

std::string TypeSet::get_type_name(int id) {
  return get_id_to_type()[id];
}

std::string typeid_print_repr(int id) {
  if (type_display_names) return TypeSet::get_type_name(id);
  else return std::to_string(id);
}

int infer_literal_type_id(std::string s) {
    if (is_int(s)) return TypeSet::get_id("int");
    else if (is_num(s)) return TypeSet::get_id("float");
    else return TypeSet::get_id("unassigned type");
}