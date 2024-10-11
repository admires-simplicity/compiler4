#pragma once
#include <string>
#include <vector>
#include <set>
#include <map>
#include <variant>
#include <initializer_list>

class TypeIdList;

extern const std::array<std::string, 12> basic_types;

class TypeSet {
public:
  static std::map<int, std::string>& get_id_to_type();
  static std::map<std::string, int>& get_type_to_id();
public:
  static bool is_type(std::string type);
  static bool add_type(std::string type);
  static int get_id(std::string type);
  static std::string get_type_name(int id);
};

using Type = std::variant<int, TypeIdList>;
using TypeVarVec = std::vector<std::variant<int, std::string, TypeIdList*>>;

std::string type_to_string(Type type, bool _typename=true); // TODO: just make Type into a class so this can be a method

class TypeIdList {
private:
  using TypeIdVec = std::vector<std::variant<int, TypeIdList*>>;
  template<typename T>
  void make_types(T types) { // TODO: put this in a cpp file. should I add type id type checking here?
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
public:
  TypeIdVec types;
  TypeIdList(std::initializer_list<std::variant<int, std::string, TypeIdList*>> types) {
    make_types(types);  
  }
  TypeIdList(std::vector<std::variant<int, std::string, TypeIdList*>> types) {
    make_types(types);
  }
  std::string to_string();

  int size() { return types.size(); }

  std::variant<int, TypeIdList*> operator[](int i) {
    return types[i];
  }
};

std::string type_print_repr(Type type);

int infer_literal_type_id(std::string s); // infer literal value type id