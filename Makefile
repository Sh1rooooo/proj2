CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS = -L/usr/lib -lexpat -lgtest -lgtest_main -pthread

SRC_DIR = src
INCLUDE_DIR = include
TEST_DIR = testsrc
OBJ_DIR = obj
BIN_DIR = bin

SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
TEST_FILES = $(wildcard $(TEST_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))
TEST_OBJ_FILES = $(patsubst $(TEST_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(TEST_FILES))

MAIN_EXEC = $(BIN_DIR)/project
TEST_EXEC = $(BIN_DIR)/run_tests

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(MAIN_EXEC): $(OBJ_FILES) | $(BIN_DIR)
	$(CXX) $(OBJ_FILES) -o $(MAIN_EXEC) $(LDFLAGS)

$(TEST_EXEC): $(TEST_OBJ_FILES) $(OBJ_FILES) | $(BIN_DIR)
	$(CXX) $(TEST_OBJ_FILES) $(OBJ_FILES) -o $(TEST_EXEC) $(LDFLAGS)

# Run Tests
test: $(TEST_EXEC)
	$(TEST_EXEC)

# Clean Build Files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Default Target
all: $(MAIN_EXEC)

# Rebuild Everything
rebuild: clean all
