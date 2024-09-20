#pragma once
#include <string>
#include <vector>
#include <set>
#include <map>
#include <variant>
#include <initializer_list>

class TypeIdList;

extern const std::vector<std::string> basic_types;

class TypeSet {
  static std::map<int, std::string>& get_id_to_type();
  static std::map<std::string, int>& get_type_to_id();
public:
  static bool is_type(std::string type);
  static bool add_type(std::string type);
  static int get_id(std::string type);
  static std::string get_type(int id);
};

using Type = std::variant<int, TypeIdList>;

class TypeIdList {
private:
  using InternTypeIdList = std::vector<std::variant<int, TypeIdList*>>;
public:
  InternTypeIdList types;
  TypeIdList(std::initializer_list<std::variant<int, std::string, TypeIdList*>> types) {
    for (auto& t : types) {
      if (std::holds_alternative<int>(t)) {
        this->types.push_back(std::get<int>(t));
      } else if (std::holds_alternative<std::string>(t)) {
        this->types.push_back(TypeSet::get_id(std::get<std::string>(t)));
      } else {
        this->types.push_back(std::get<TypeIdList*>(t));
      }
    }
  }
  std::string to_string();
};

std::string type_print_repr(Type type);

int infer_type_id(std::string s); // infer literal value type id