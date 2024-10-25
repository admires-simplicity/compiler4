#pragma once

#include <string>
#include <vector>

std::istringstream *filename_to_str(std::string filename);

bool is_int(std::string s);
bool is_num(std::string s);

template <typename T, typename... Vectors>
std::vector<T> add_vec(Vectors&&... vectors) {
    std::vector<T> res; 
    res.reserve((std::forward<Vectors>(vectors).size() + ...)); 
    (res.insert(res.end(), std::forward<Vectors>(vectors).begin(), std::forward<Vectors>(vectors).end()), ...);
    return res;
}