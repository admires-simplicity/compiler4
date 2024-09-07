#pragma once
#include <string>
#include <vector>
#include <set>
#include <map>

extern const std::vector<std::string> basic_types;

class TypeSet {
public:
  static std::map<int, std::string> id_to_type;
  static std::map<std::string, int> type_to_id;
  bool is_type(std::string type);
  bool add_type(std::string type);
  int get_id(std::string type);
  std::string get_type(int id);
};