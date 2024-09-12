# Directories
SRC_DIR := src
LIB_DIR := $(SRC_DIR)/lib
APP_DIR := $(SRC_DIR)/app
INCLUDE_DIR := include
BUILD_DIR := build

# Source files
LIB_SOURCES := $(wildcard $(LIB_DIR)/*.cpp)
APP_SOURCES := $(wildcard $(APP_DIR)/*.cpp)

# Headers
HEADERS := $(wildcard $(INCLUDE_DIR)/*.h)

# Main source file and its corresponding executable name
MAIN_SRC := $(APP_DIR)/main.cpp
MAIN_OBJ := $(BUILD_DIR)/main.o
MAIN_EXE := compiler4

# Integration test result directory variables
INT_C_RESULTS := integration\ testing/c\ results
INT_EXES := integration\ testing/exes
INT_EXE_OUTPUTS := integration\ testing/exe\ outputs
INT_EXPECTS := integration\ testing\expects
INT_TEST_DIRS = $(INT_C_RESULTS) $(INT_EXES) $(INT_EXE_OUTPUTS) $(INT_EXPECTS)

# Other executable sources
OTHER_SOURCES := $(filter-out $(MAIN_SRC), $(APP_SOURCES))
OTHER_EXECUTABLES := $(patsubst $(APP_DIR)/%.cpp,%,$(OTHER_SOURCES))

# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++20 -I$(INCLUDE_DIR)

# Debug and Release flags
DEBUG_FLAGS := -g -O0
RELEASE_FLAGS := -O2

# Default to release build
ifeq ($(DEBUG), 1)
	CXXFLAGS += $(DEBUG_FLAGS)
else
	CXXFLAGS += $(RELEASE_FLAGS)
endif

# Rule for creating object files from lib directory
$(BUILD_DIR)/%.o: $(LIB_DIR)/%.cpp $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule for creating object files from app directory
$(BUILD_DIR)/%.o: $(APP_DIR)/%.cpp $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule for creating the main executable
$(MAIN_EXE): $(MAIN_OBJ) $(patsubst $(LIB_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(LIB_SOURCES))
	$(CXX) $(MAIN_OBJ) $(patsubst $(LIB_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(LIB_SOURCES)) -o $(MAIN_EXE)

# Rule for creating other executables in the current directory
%: $(BUILD_DIR)/%.o $(patsubst $(LIB_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(LIB_SOURCES))
	$(CXX) $^ -o $@

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Generate individual targets for each executable, except main
$(foreach exe,$(OTHER_EXECUTABLES),$(eval $(exe): $(BUILD_DIR)/$(exe).o $(patsubst $(LIB_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(LIB_SOURCES))))

# Default goal is to build all executables
.DEFAULT_GOAL := all
.PHONY: all
all: $(MAIN_EXE) $(OTHER_EXECUTABLES)

# Clean rules
.PHONY: clean cleantest
clean:
	rm -rf $(BUILD_DIR) $(OTHER_EXECUTABLES) $(MAIN_EXE) $(INT_TEST_DIRS)
cleantest:
	rm -rf $(INT_TEST_DIRS)
