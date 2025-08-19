CXX := g++
LDLIBS = -lsqlite3 -lpthread -lrt -lstdc++fs
CXXFLAGS := -std=c++17 -Wall -Wextra -I./src/include

SRC_DIR := src
COMMON_DIR := $(SRC_DIR)/common
TARGETS_DIR := $(SRC_DIR)/targets
BUILD_DIR := build

COMMON_SRCS := $(wildcard $(COMMON_DIR)/*.cpp)
COMMON_OBJS := $(patsubst $(COMMON_DIR)/%.cpp,$(BUILD_DIR)/common/%.o,$(COMMON_SRCS))

TARGET_SRCS := $(wildcard $(TARGETS_DIR)/*.cpp)
TARGET_NAMES := $(notdir $(TARGET_SRCS:.cpp=))
TARGET_BINS := $(patsubst %,$(BUILD_DIR)/targets/%,$(TARGET_NAMES))

# Default target
all: $(TARGET_BINS)

# Rule to build each target binary
$(BUILD_DIR)/targets/%: $(TARGETS_DIR)/%.cpp $(COMMON_OBJS) | $(BUILD_DIR)/targets
	$(CXX) $(CXXFLAGS) $< $(COMMON_OBJS) -o $@ $(LDLIBS)

# Compile common .cpp to .o
$(BUILD_DIR)/common/%.o: $(COMMON_DIR)/%.cpp | $(BUILD_DIR)/common
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create necessary directories
$(BUILD_DIR)/common:
	mkdir -p $@

$(BUILD_DIR)/targets:
	mkdir -p $@

# Clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
