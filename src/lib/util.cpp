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