#pragma once
#include <string>
#include <vector>
#include <set>
#include <map>
#include <variant>
#include <initializer_list>
#include <optional>
#include <sstream>

class TypeSet {
  static std::map<int, std::string>& get_id_to_type();
  static std::map<std::string, int>& get_type_to_id();
public:
  static bool is_type(std::string type);
  static bool add_type(std::string type);
  static int get_id(std::string type);
  static std::string get_type_name(int id);
};

class Type {
public:
  virtual std::string to_string() const = 0;
};

class AtomicType : public Type {
  int type_id;
public:
  AtomicType(int type_id) : type_id(type_id) {}
  AtomicType(std::string type) : type_id(TypeSet::get_id(type)) {}

  std::string to_string() const {
    return TypeSet::get_type_name(type_id);
  }
};

class CompositeType : public Type {
  std::vector<Type*> types;
public:
  CompositeType(std::vector<Type*> types) : types(types) {}
  CompositeType(std::initializer_list<Type*> types) : types(types) {}
  CompositeType(std::initializer_list<std::variant<int, std::string, Type*>> types) {
    for (auto& t : types) {
      if (std::holds_alternative<int>(t)) {
        this->types.push_back(new AtomicType(std::get<int>(t)));
      } else if (std::holds_alternative<std::string>(t)) {
        this->types.push_back(new AtomicType(std::get<std::string>(t)));
      } else {
        this->types.push_back(std::get<Type*>(t));
      }
    }
  }

  std::string to_string() const {
    std::stringstream ss;
    ss << "(";
    for (int i = 0; i < types.size(); ++i) {
      ss << types[i]->to_string() << " -> ";
    }
    ss.seekp(-4, std::ios_base::end);
    ss << ")";
    return ss.str().substr(0, ss.tellp());
  }

  const Type *return_type() {
    return types.back();
  }
};


extern const std::array<std::string, 13> basic_types;

int infer_literal_type_id(std::string s); // infer literal value type id