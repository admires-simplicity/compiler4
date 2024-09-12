#pragma once
#include <cstdint>

// main flags
const int PARSE_TREE_ONLY = 1;
const int PRETTY_PARSE_TREE = 2;
const int PARSE_TREE_TYPES = 4;
const int PARSE_TREE_TYPE_IDS = 8;
const int PARSE_TREE_TYPE_NAMES = 16;

// parser_test flags
const int VERBOSE = 32;

extern uint64_t flags;
int64_t read_flags(int argc, char** argv);