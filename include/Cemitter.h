#pragma once
#include <set>
#include <iostream>
#include <sstream>

#include "AST.h"
#include "lexer.h"

bool emit(SyntaxNode *node);
bool emit_program(SyntaxNode *node);

extern std::set<std::string> c_infix_ops;