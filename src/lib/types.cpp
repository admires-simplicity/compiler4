#include <map>
#include <string>
#include <cassert>

#include "types.h"
#include "options.h"
#include "util.h"

const std::vector<std::string> basic_types = {
  "unassigned type", // first type a value gets on parsing/lexing
  "void",
  "unit",

  // numeric types
  ///"num", // idk if this is relevant
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

std::map<int, std::string> TypeSet::id_to_type = []() {
    std::map<int, std::string> id_to_type;
    for (int i = 0; i < basic_types.size(); i++) {
      id_to_type[i] = basic_types[i];
    }
    return id_to_type;
  }();

std::map<std::string, int> TypeSet::type_to_id = []() {
    std::map<std::string, int> type_to_id;
    for (int i = 0; i < basic_types.size(); i++) {
      type_to_id[basic_types[i]] = i;
    }
    return type_to_id;
  }();

bool TypeSet::is_type(std::string type) {
  return type_to_id.find(type) != type_to_id.end();
}

bool TypeSet::add_type(std::string type) {
  if (is_type(type)) return false;
  int id = id_to_type.size();
  id_to_type[id] = type;
  type_to_id[type] = id;
  return true;
}

int TypeSet::get_id(std::string type) {
  return type_to_id[type];
}

std::string TypeSet::get_type(int id) {
  return id_to_type[id];
}

std::string TypeIdList::to_string() {
  std::string s = "[";
  for (auto& t : types) {
    if (std::holds_alternative<int>(t)) {
      s += std::to_string(std::get<int>(t));
    } else {
      s += std::get<TypeIdList*>(t)->to_string();
    }
    if (&t != &types.back()) s += ", ";
  }    s += "]";
  return s;
}

std::string type_print_repr(Type type) {
  if (std::holds_alternative<int>(type)) {
    if (flags & PARSE_TREE_TYPE_NAMES) return TypeSet::id_to_type[std::get<int>(type)];
    else /*if (flags & PARSE_TREE_TYPE_IDS)*/ return std::to_string(std::get<int>(type));
  }
  else return std::get<TypeIdList>(type).to_string();
}

int infer_type_id(std::string s) {
    if (is_int(s)) return TypeSet::type_to_id["int"];
    else if (is_num(s)) return TypeSet::type_to_id["float"];
    else return TypeSet::type_to_id["unassigned type"];
}