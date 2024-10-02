#pragma once
#include <cstdint>

// main flags
extern const int PARSE_TREE_ONLY;
extern const int PRETTY_PARSE_TREE;
extern const int PARSE_TREE_TYPES;

extern const int OUTPUT_IR;

extern const int DO_NOT_EMIT;

// parser_test flags
extern const int VERBOSE;

// other flags
extern bool type_display_names; // true == names, false == ids

extern uint64_t flags;
int64_t read_flags(int argc, char** argv);