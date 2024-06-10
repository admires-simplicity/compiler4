SRC_DIR := src
INCLUDE_DIR := include
BUILD_DIR := build

SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
HEADERS := $(wildcard $(INCLUDE_DIR)/*.h)

# Identify the main source file and its corresponding executable name
MAIN_SRC := $(SRC_DIR)/main.cpp
MAIN_OBJ := $(BUILD_DIR)/main.o
MAIN_EXE := compiler4

# For each source file, create an executable name by removing the path and extension
OTHER_SOURCES := $(filter-out $(MAIN_SRC), $(SOURCES))
OTHER_EXECUTABLES := $(patsubst $(SRC_DIR)/%.cpp,%,$(OTHER_SOURCES))

CXX := g++
CXXFLAGS := -std=c++20 -I$(INCLUDE_DIR)

# Rule for creating object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule for creating the main executable
$(MAIN_EXE): $(MAIN_OBJ)
	$(CXX) $(MAIN_OBJ) -o $(MAIN_EXE)

# Rule for creating other executables in the current directory
%: $(BUILD_DIR)/%.o
	$(CXX) $< -o $@

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Generate individual targets for each executable, except main
$(foreach exe,$(OTHER_EXECUTABLES),$(eval $(exe): $(BUILD_DIR)/$(exe).o))

# Default goal is to build all executables
.DEFAULT_GOAL := all
.PHONY: all
all: $(MAIN_EXE) $(OTHER_EXECUTABLES)

# Clean rule
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(OTHER_EXECUTABLES) $(MAIN_EXE)
