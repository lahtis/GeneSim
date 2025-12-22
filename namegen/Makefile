# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude
LDFLAGS = 

# Directories
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

# Files
# Find all .c files in the src directory
SOURCES = $(wildcard $(SRC_DIR)/*.c)
# Convert .c file paths to .o file paths in the obj directory
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))
# Final executable name
TARGET = $(BIN_DIR)/namegen

# Default target: Build the executable
all: $(TARGET)

# Linking: Link object files into the final executable
$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Compiling: Build object files from source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create necessary directories if they don't exist
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

# Cleaning: Remove build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Testing: Run the automated test suite
test: $(TARGET)
	python3 test.py

# Prevent conflicts with files named all, clean, or test
.PHONY: all clean test
