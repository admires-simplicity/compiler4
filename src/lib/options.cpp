#include <string>
#include <cstdint>
#include <cctype>

#include "options.h"

// main flags
const int PARSE_TREE_ONLY = 1;
const int PRETTY_PARSE_TREE = 2;
const int PARSE_TREE_TYPES = 4;
const int PARSE_TREE_TYPE_IDS = 8;
const int PARSE_TREE_TYPE_NAMES = 16;

const int OUTPUT_IR = 32;

const int DO_NOT_EMIT = 64;

// parser_test flags
const int VERBOSE = 128;

bool type_display_names = true;

uint64_t flags;

int64_t read_flags(int argc, char** argv) {
  int64_t flags = 0;
  for (int i = 1; i < argc; i++) {
    std::string arg = std::string(argv[i]);
    if (arg.size() >= 1 && arg[0] == '-') {
      if (arg.size() >= 2) {
        switch (std::tolower(arg[1])) {
          case 't':
            if (arg.size() >= 3 && arg[2] == 'i') type_display_names = false;
            else if (arg.size() >= 3 && arg[2] == 'n') type_display_names = true;
            break;
          case 'p':
            flags |= PARSE_TREE_ONLY;
            if (arg[1] == 'P') flags |= PRETTY_PARSE_TREE;
            if (arg.size() >= 3 && arg[2] == 't') flags |= PARSE_TREE_TYPES;
            break;
          case 'i':
            // todo: change this to work like -p (can select show types or not)
            flags |= OUTPUT_IR;
            break;
          case 'v':
            flags |= VERBOSE;
            break;
          case 'd':
            flags |= DO_NOT_EMIT;
            break;
        }
      }
    }
  }
  return flags;
}