# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++20 -Iinclude
DEBUGFLAGS := -g
DEPFLAGS := -MMD -MP

# Source and build directories for lib and app
DIRS = lib app
BUILD_DIRS = $(foreach dir,$(DIRS),build/$(dir))
DEBUG_DIRS = $(foreach dir,$(DIRS),debug/$(dir))

# Object files (release and debug)
LIB_OBJS := $(patsubst src/lib/%.cpp,build/lib/%.o,$(wildcard src/lib/*.cpp))
APP_OBJS := $(patsubst src/app/%.cpp,build/app/%.o,$(wildcard src/app/*.cpp))

DEBUG_LIB_OBJS := $(patsubst src/lib/%.cpp,debug/lib/%.o,$(wildcard src/lib/*.cpp))
DEBUG_APP_OBJS := $(patsubst src/app/%.cpp,debug/app/%.o,$(wildcard src/app/*.cpp))

# Dependency files
LIB_DEPS := $(LIB_OBJS:.o=.d)
APP_DEPS := $(APP_OBJS:.o=.d)

DEBUG_LIB_DEPS := $(DEBUG_LIB_OBJS:.o=.d)
DEBUG_APP_DEPS := $(DEBUG_APP_OBJS:.o=.d)

# Targets
EXE := compiler4
DEBUG_EXE := debug/compiler4

EXE_OBJ := build/app/main.o
DEBUG_EXE_OBJ := debug/app/main.o

EXES := $(patsubst build/app/%.o,%,$(filter-out $(EXE_OBJ),$(APP_OBJS)))
DEBUG_EXES := $(patsubst debug/app/%.o,debug/%,$(filter-out $(DEBUG_EXE_OBJ),$(DEBUG_APP_OBJS)))

# Compound Targets
release: $(EXE) $(EXES)
debug: $(DEBUG_EXE) $(DEBUG_EXES)
all: release debug

# Build compiler4 (release)
$(EXE): $(EXE_OBJ) $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Build compiler4 (debug)
$(DEBUG_EXE): $(DEBUG_EXE_OBJ) $(DEBUG_LIB_OBJS)
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) -o $@ $^

# Build other executables (release)
%: build/app/%.o $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Build other executables (debug)
debug/%: debug/app/%.o $(DEBUG_LIB_OBJS)
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) -o $@ $^

# Object file compilation rule (release)
build/%.o: src/%.cpp | $(BUILD_DIRS)
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -c $< -o $@

# Debug object file compilation rule
debug/%.o: src/%.cpp | $(DEBUG_DIRS)
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) $(DEPFLAGS) -c $< -o $@

# Create directories if they don't exist
$(BUILD_DIRS) $(DEBUG_DIRS):
	mkdir -p $@

# Clean
clean: clean-release clean-debug
clean-release:
	rm -rf build $(EXE) $(EXES) $(LIB_DEPS) $(APP_DEPS)
clean-debug:
	rm -rf debug $(DEBUG_EXE) $(DEBUG_EXES) $(DEBUG_LIB_DEPS) $(DEBUG_APP_DEPS)

# Include dependency files if they exist
-include $(LIB_DEPS) $(APP_DEPS) $(DEBUG_LIB_DEPS) $(DEBUG_APP_DEPS)
