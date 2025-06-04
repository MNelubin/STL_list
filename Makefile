# Configuration
PROJECT = STL_list
LIBPROJECT = $(PROJECT).a
TESTPROJECT = test-$(PROJECT)

# Directories
SRC_DIR = src
INC_DIR = include
TEST_DIR = tests

# Tools
CXX = g++
AR = ar
ARFLAGS = rcs

# Flags
# Base CXXFLAGS
CXXFLAGS_BASE = -I$(INC_DIR) -std=c++17 -Wall -g -fPIC -Werror -Wpedantic -O3
# Application specific CXXFLAGS
CXXFLAGS = $(CXXFLAGS_BASE) -DPROJECT_NAME="\"$(PROJECT)\""
CXXFLAGS_COVERAGE = $(CXXFLAGS_BASE) -fprofile-arcs -ftest-coverage
# Linker flags
# Base LDLIBS
LDLIBS_BASE = -lpthread
LDAPPFLAGS = $(LDLIBS_BASE)
LDGTESTFLAGS = $(LDLIBS_BASE) -lgtest -lgtest_main

# Source Files
APP_SRC = main.cpp # main.cpp is in the root directory
LIB_SRC = $(wildcard $(SRC_DIR)/*.cpp)
TEST_SRC_FILES = $(TEST_DIR)/$(TESTPROJECT).cpp  


# Object Files
APP_OBJ = $(notdir $(APP_SRC:.cpp=.o))
LIB_OBJ = $(patsubst $(SRC_DIR)/%.cpp,%.o,$(LIB_SRC))
TEST_OBJ = $(TEST_DIR)/$(TESTPROJECT).o 

# Dependencies
DEPS = $(wildcard $(INC_DIR)/*.h)

# Targets
.PHONY: all test clean cleanall info

default: all

all: info $(PROJECT)

info:
	@echo "Building project $(PROJECT)..."

# Build application
$(PROJECT): $(APP_OBJ) $(LIBPROJECT)
	@echo "Linking application $(PROJECT)..."
	$(CXX) $(CXXFLAGS) -o $@ $(APP_OBJ) $(LIBPROJECT) $(LDAPPFLAGS)

# Build library
$(LIBPROJECT): $(LIB_OBJ)
	@echo "Creating library $(LIBPROJECT)..."
	$(AR) $(ARFLAGS) $@ $^

# Build tests executable
# Depends on the test object file and the library.
# Also, ensure TEST_DIR exists before linking, though $(TEST_OBJ) should handle it.
$(TESTPROJECT): $(TEST_OBJ) $(LIBPROJECT) | create_test_dir
	@echo "Linking test executable $(TESTPROJECT)..."
	$(CXX) $(CXXFLAGS) -o $@ $(TEST_OBJ) $(LIBPROJECT) $(LDGTESTFLAGS)

test: $(TESTPROJECT)
	@echo "Running unit tests (if $(TESTPROJECT) exists and is runnable)..."
	@if [ -f ./$(TESTPROJECT) ]; then ./$(TESTPROJECT); else echo "Test executable '$(TESTPROJECT)' not found or not runnable. Ensure '$(TESTPROJECT).cpp' exists and compiles."; fi

# Rule to compile application source file (main.cpp from root)
$(APP_OBJ): %.o: %.cpp $(DEPS)
	@echo "Compiling $< -> $@..."
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Rule to compile library source files (from SRC_DIR)
$(LIB_OBJ): %.o: $(SRC_DIR)/%.cpp $(DEPS)
	@echo "Compiling $< -> $@..."
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# This target represents the test directory itself.
# It's not .PHONY because it represents a real directory that can exist.
create_test_dir:
	@echo "Ensuring directory $(TEST_DIR) exists..."
	@mkdir -p $(TEST_DIR)

# Rule to compile test source file
# It depends on the source files, headers, and (order-only) on the test directory's existence.
$(TEST_OBJ): $(TEST_SRC_FILES) $(DEPS) | create_test_dir
	@echo "Compiling test source $< -> $@..."
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Clean rules
clean:
	@echo "Cleaning object files..."
	rm -f $(APP_OBJ) $(LIB_OBJ) $(TEST_OBJ)

cleanall: clean
	@echo "Cleaning executables and library..."
	rm -f $(PROJECT) $(LIBPROJECT) $(TESTPROJECT)
	rm -f massif*
	rm -f *.gcno *.gcda

# Coverage rules
coverage: CXXFLAGS = $(CXXFLAGS_COVERAGE)
coverage: $(TESTPROJECT)
	@echo "Running tests for coverage..."
	./$(TESTPROJECT)
	@echo "Generating coverage data..."
	mkdir -p html_coverage
	lcov --ignore-errors version --ignore-errors mismatch --gcov-tool gcov-11 --capture --directory . --output-file html_coverage/coverage.info --rc geninfo_unexecuted_blocks=1 --no-external --base-directory .