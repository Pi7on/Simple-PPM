# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lm

# Directories
SRC_DIR = src
BIN_DIR = bin
LINUX_BIN = $(BIN_DIR)/linux
WINDOWS_BIN = $(BIN_DIR)/windows
DATA_DIR = data

# Source files and object files
SOURCES = $(wildcard $(SRC_DIR)/*.c)
HEADERS = $(wildcard $(SRC_DIR)/*.h)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(LINUX_BIN)/%.o, $(SOURCES))

# Target binary
TARGET_LINUX = $(LINUX_BIN)/main
TARGET_WINDOWS = $(WINDOWS_BIN)/main.exe

# Default target
.PHONY: all
all: $(TARGET_LINUX)

# Linux build
$(TARGET_LINUX): $(OBJECTS) | $(LINUX_BIN)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Built Linux binary: $@"

# Compile object files for Linux
$(LINUX_BIN)/%.o: $(SRC_DIR)/%.c $(HEADERS) | $(LINUX_BIN)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled: $< -> $@"

# Windows build (requires cross-compiler like MinGW)
.PHONY: windows
windows: $(TARGET_WINDOWS)

$(TARGET_WINDOWS): $(SOURCES) $(HEADERS) | $(WINDOWS_BIN)
	x86_64-w64-mingw32-gcc $(CFLAGS) $(SOURCES) -o $@ $(LDFLAGS)
	@echo "Built Windows binary: $@"

# Create directories if they don't exist
$(LINUX_BIN):
	@mkdir -p $@

$(WINDOWS_BIN):
	@mkdir -p $@

# Clean build artifacts
.PHONY: clean
clean:
	@rm -rf $(LINUX_BIN) $(WINDOWS_BIN)
	@echo "Cleaned up build artifacts."

# Run the application (Linux)
.PHONY: run
run: $(TARGET_LINUX)
	$(TARGET_LINUX)

# Help message
.PHONY: help
help:
	@echo "Usage:"
	@echo "  make          - Build the project for Linux"
	@echo "  make windows  - Build the project for Windows (requires MinGW)"
	@echo "  make run      - Run the compiled Linux binary"
	@echo "  make clean    - Clean up build artifacts"
