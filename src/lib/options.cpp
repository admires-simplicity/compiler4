#include <string>
#include <cstdint>
#include <cctype>

#include "options.h"

uint64_t flags;

int64_t read_flags(int argc, char** argv) {
  int64_t flags = 0;
  for (int i = 1; i < argc; i++) {
    std::string arg = std::string(argv[i]);
    if (arg.size() >= 1 && arg[0] == '-') {
      if (arg.size() >= 2) {
        switch (std::tolower(arg[1])) {
          case 'p':
            flags |= PARSE_TREE_ONLY;
            if (arg[1] == 'P') flags |= PRETTY_PARSE_TREE;
            if (arg.size() >= 3 && arg[2] == 't') flags |= PARSE_TREE_TYPES;
            if (arg.size() >= 4 && arg[3] == 'i') flags |= PARSE_TREE_TYPE_IDS;
            else if (arg.size() >= 4 && arg[3] == 'n') flags |= PARSE_TREE_TYPE_NAMES;
            break;
          case 'v':
            flags |= VERBOSE;
            break;
        }
      }
    }
  }
  return flags;
}