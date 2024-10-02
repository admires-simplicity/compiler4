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


std::string type_to_string(Type type, bool _typename) {
  if (std::holds_alternative<int>(type)) {
    int id = std::get<int>(type);
    if (_typename) return TypeSet::get_type_name(id);
    else return std::to_string(id);
  }
  else {
    return std::get<TypeIdList>(type).to_string();
  }
}

std::map<int, std::string>& TypeSet::get_id_to_type() {
  static std::map<int, std::string> id_to_type = []() {
    std::map<int, std::string> id_to_type;
    for (int i = 0; i < basic_types.size(); i++)
    {
      id_to_type[i] = basic_types[i];
    }
    return id_to_type;
  }();
  return id_to_type;
}

std::map<std::string, int>& TypeSet::get_type_to_id() {
  static std::map<std::string, int> type_to_id = []() {
    std::map<std::string, int> type_to_id;
    for (int i = 0; i < basic_types.size(); i++) {
      type_to_id[basic_types[i]] = i;
    }
    return type_to_id;
  }();
  return type_to_id;
}

std::string type_to_id(Type type, bool _typenames=true) {
  if (std::holds_alternative<int>(type)) {
    if (_typenames) return TypeSet::get_type_name(std::get<int>(type));
    else return std::to_string(std::get<int>(type));
  }
  else {
    return std::get<TypeIdList>(type).to_string();
  }
}

bool TypeSet::is_type(std::string type) {
  return get_type_to_id().find(type) != get_type_to_id().end();
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

std::string TypeIdList::to_string() {
  std::string s = "[";
  for (auto& t : types) {
    if (std::holds_alternative<int>(t)) {
      s += typeid_print_repr(std::get<int>(t));
    } else {
      s += std::get<TypeIdList*>(t)->to_string();
    }
    if (&t != &types.back()) s += ", ";
  }    s += "]";
  return s;
}

std::string type_print_repr(Type type) {
  if (std::holds_alternative<int>(type)) {
    return typeid_print_repr(std::get<int>(type));
  }
  else return std::get<TypeIdList>(type).to_string();
}

int infer_type_id(std::string s) {
    if (is_int(s)) return TypeSet::get_id("int");
    else if (is_num(s)) return TypeSet::get_id("float");
    else return TypeSet::get_id("unassigned type");
}