#pragma once
#include <string>
#include <vector>
#include <set>
#include <map>
#include <variant>

class TypeIdList;

using Type = std::variant<int, TypeIdList>;

extern const std::vector<std::string> basic_types;

class TypeIdList {
public:
  std::vector<std::variant<int, TypeIdList*>> types;
  TypeIdList() {}
  TypeIdList(std::vector<std::variant<int, TypeIdList*>> types) : types(types) {}
  std::string to_string();
};

class TypeSet {
public:
  static std::map<int, std::string> id_to_type;
  static std::map<std::string, int> type_to_id;
  bool is_type(std::string type);
  bool add_type(std::string type);
  int get_id(std::string type);
  std::string get_type(int id);
};

std::string type_print_repr(Type type);

int infer_type_id(std::string s); // infer literal value type id

Type BasicType(int id);
Type BasicType(std::string type);
Type CompoundType(TypeIdList types);