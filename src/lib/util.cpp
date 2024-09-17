#include <string>
#include <fstream>
#include <sstream>

#include "util.h"

std::istringstream *filename_to_str(std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return nullptr;
    
    std::stringstream buf;
    buf << "{\n" << file.rdbuf() << "\n}"; // interpret as block
    return new std::istringstream(buf.str());
}

bool is_int(std::string s) {
    for (char c : s) {
        if (!isdigit(c)) return false;
    }
    return true;
}

bool is_num(std::string s) {
    bool found_dot = false;
    for (char c : s) {
        if (c == '.') {
            if (found_dot) return false;
            found_dot = true;
        }
        else if (!isdigit(c)) return false;
    }
    return true;
}