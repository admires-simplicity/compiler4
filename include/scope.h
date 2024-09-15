#pragma once

#include <map>
#include <string>

#include "types.h"

using Scope = std::map<std::string, Type>;

Scope global_scope;