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

// Syntax Tree printing flags
bool display_types = false;
bool pretty_print = false;
int pretty_print_max_depth = 1;

uint64_t flags;

void print_options(std::string& arg) {
  for (int j = 2; j < arg.size(); ++j) {
    if (std::tolower(arg[j]) == 't') display_types = true;
    if (std::tolower(arg[j]) == 'p') {
      pretty_print = true;
      if (j+1 < arg.size() && std::isdigit(arg[j+1])) {
        pretty_print_max_depth = std::stoi(arg.substr(j+1));
      }
    }
  }
}

int64_t read_flags(int argc, char** argv) {
  int64_t flags = 0;
  for (int i = 1; i < argc; ++i) {
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
            print_options(arg);
            break;
          case 'i':
            flags |= OUTPUT_IR;
            print_options(arg);
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