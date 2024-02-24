SRC_DIR := src
INCLUDE_DIR := include
BUILD_DIR := build

SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
HEADERS := $(wildcard $(INCLUDE_DIR)/*.h)

OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

OUTPUT := compiler4
.DEFAULT_GOAL := $(OUTPUT)

CXX := g++
CXXFLAGS := -std=c++20 -I$(INCLUDE_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(OUTPUT): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(OUTPUT)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(OUTPUT)
