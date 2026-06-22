# ============================================================================
# Makefile for Raylib Game Project
# Supports: macOS, Linux, Windows (w64devkit), and WebAssembly builds
# ============================================================================

# --- OS Detection ---
ifeq ($(OS),Windows_NT)
    PLATFORM := Windows
else
    OS_NAME := $(shell uname -s)
    ifeq ($(OS_NAME),Darwin)
        PLATFORM := macOS
    else
        PLATFORM := Linux
    endif
endif

# --- Compiler Detection ---
ifeq ($(PLATFORM),macOS)
    CC_NATIVE := clang
else
    CC_NATIVE := gcc
endif
CC_WEB := emcc

# --- Project Structure ---
SRC_DIR := src
LIB_DIR := lib
WEB_DIR := web
BIN_DIR := bin
ASSETS_DIR := assets

# Platform-specific raylib location
ifeq ($(PLATFORM),Windows)
    RAYLIB_DIR := $(LIB_DIR)/win64
else ifeq ($(PLATFORM),macOS)
    RAYLIB_DIR := $(LIB_DIR)/macos
else
    RAYLIB_DIR := $(LIB_DIR)/linux
endif

# Source files
SOURCES := $(SRC_DIR)/main.c \
           $(SRC_DIR)/chunk.c \
           $(SRC_DIR)/hash.c \
           $(SRC_DIR)/world.c \
		   $(SRC_DIR)/mesh.c \
		   $(SRC_DIR)/player.c \

# Include directories
INCLUDES := -I$(SRC_DIR) -I$(LIB_DIR) -I$(RAYLIB_DIR)

# --- Output Files ---
ifeq ($(PLATFORM),Windows)
    NATIVE_RELEASE := $(BIN_DIR)/game.exe
    NATIVE_DEBUG   := $(BIN_DIR)/game_debug.exe
else
    NATIVE_RELEASE := $(BIN_DIR)/game
    NATIVE_DEBUG   := $(BIN_DIR)/game_debug
endif

WEB_RELEASE := $(WEB_DIR)/game.js
WEB_DEBUG   := $(WEB_DIR)/game.debug.js

# ============================================================================
# NATIVE BUILD FLAGS
# ============================================================================

NATIVE_COMMON_FLAGS := -Wall -Wextra -Wpedantic

# Platform-specific libraries
ifeq ($(PLATFORM),macOS)
    NATIVE_LIBS := -L$(RAYLIB_DIR) \
                   -framework CoreVideo \
                   -framework IOKit \
                   -framework Cocoa \
                   -framework GLUT \
                   -framework OpenGL \
                   $(RAYLIB_DIR)/libraylib.a
endif

ifeq ($(PLATFORM),Linux)
    NATIVE_LIBS := -L$(RAYLIB_DIR) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

ifeq ($(PLATFORM),Windows)
    NATIVE_LIBS := -L$(RAYLIB_DIR) -lraylib -lopengl32 -lgdi32 -lwinmm
endif

# Release flags
NATIVE_RELEASE_FLAGS := $(NATIVE_COMMON_FLAGS) \
                        -O3 \
                        -DNDEBUG \
                        -march=native

# Debug flags — sanitizers not supported on Windows/w64devkit, skip them there
ifeq ($(PLATFORM),Windows)
    NATIVE_DEBUG_FLAGS := $(NATIVE_COMMON_FLAGS) \
                          -g3 \
                          -O0 \
                          -DDEBUG \
                          -fstack-protector-strong
else ifeq ($(PLATFORM),macOS)
    NATIVE_DEBUG_FLAGS := $(NATIVE_COMMON_FLAGS) \
                          -g3 \
                          -glldb \
                          -O0 \
                          -DDEBUG \
                          -fwrapv \
                          -fsanitize=address \
                          -fsanitize=undefined \
                          -fno-omit-frame-pointer \
                          -fstack-protector-strong \
                          -fsanitize-recover=signed-integer-overflow
else
    NATIVE_DEBUG_FLAGS := $(NATIVE_COMMON_FLAGS) \
                          -g3 \
                          -O0 \
                          -DDEBUG \
                          -fwrapv \
                          -fsanitize=address \
                          -fsanitize=undefined \
                          -fno-omit-frame-pointer \
                          -fstack-protector-strong \
                          -fsanitize-recover=signed-integer-overflow
endif
# ============================================================================
# WEB BUILD FLAGS
# ============================================================================

WEB_COMMON_FLAGS := -s USE_GLFW=3 \
                    -s ASYNCIFY \
                    -DPLATFORM_WEB \
                    --preload-file $(ASSETS_DIR)

WEB_LIB := $(WEB_DIR)/libraylib.a

WEB_RELEASE_FLAGS := $(WEB_COMMON_FLAGS) \
                     -Os \
                     -Wall \
                     -s TOTAL_MEMORY=67108864 \
                     -s STACK_SIZE=1048576

WEB_DEBUG_FLAGS := $(WEB_COMMON_FLAGS) \
                   -g3 \
                   -O0 \
                   -Wall \
                   -Wextra \
                   -s ASSERTIONS=2 \
                   -s SAFE_HEAP=1 \
                   -s STACK_OVERFLOW_CHECK=2 \
                   -s ALLOW_MEMORY_GROWTH=1 \
                   -s INITIAL_MEMORY=67108864 \
                   -s STACK_SIZE=1048576 \
                   -s GL_ASSERTIONS=1 \
                   -s GL_DEBUG=1 \
                   --source-map-base http://localhost:8000/ \
                   -gsource-map

# ============================================================================
# BUILD TARGETS
# ============================================================================

.PHONY: all clean help info
.PHONY: native native_debug native_release
.PHONY: web web_debug web_release
.PHONY: run run_debug run_web run_web_debug

all: native_release

# --- Help ---
help:
	@echo "======================================================================"
	@echo "  Raylib Game Build System (Platform: $(PLATFORM))"
	@echo "======================================================================"
	@echo ""
	@echo "Native Targets:"
	@echo "  make native          - Build native release"
	@echo "  make native_release  - Build optimized native executable"
	@echo "  make native_debug    - Build native with debug symbols"
	@echo "  make run             - Build and run native release"
	@echo "  make run_debug       - Build and run native debug"
	@echo ""
	@echo "Web Targets:"
	@echo "  make web             - Build web release"
	@echo "  make web_release     - Build optimized WebAssembly"
	@echo "  make web_debug       - Build WebAssembly with debug symbols"
	@echo "  make run_web         - Build web release and show instructions"
	@echo "  make run_web_debug   - Build web debug and show instructions"
	@echo ""
	@echo "Other:"
	@echo "  make clean           - Remove all build artifacts"
	@echo "  make info            - Show build configuration"
	@echo "  make help            - Show this help message"
	@echo "======================================================================"

# ============================================================================
# NATIVE BUILDS
# ============================================================================

native: native_release

native_release: $(NATIVE_RELEASE)

native_debug: $(NATIVE_DEBUG)

$(NATIVE_RELEASE): $(SOURCES)
	@echo "======================================================================"
	@echo "Building Native Release... ($(PLATFORM))"
	@echo "======================================================================"
	@mkdir -p $(BIN_DIR)
	$(CC_NATIVE) $(SOURCES) $(INCLUDES) $(NATIVE_RELEASE_FLAGS) -o $@ $(NATIVE_LIBS)
	@echo "✓ Native release build complete: $@"
	@echo ""

$(NATIVE_DEBUG): $(SOURCES)
	@echo "======================================================================"
	@echo "Building Native Debug... ($(PLATFORM))"
	@echo "======================================================================"
	@mkdir -p $(BIN_DIR)
	$(CC_NATIVE) $(SOURCES) $(INCLUDES) $(NATIVE_DEBUG_FLAGS) -o $@ $(NATIVE_LIBS)
	@echo "✓ Native debug build complete: $@"
	@echo ""

# ============================================================================
# WEB BUILDS
# ============================================================================

web: web_release

web_release: $(WEB_RELEASE)

web_debug: $(WEB_DEBUG)

$(WEB_RELEASE): $(SOURCES)
	@echo "======================================================================"
	@echo "Building WebAssembly Release..."
	@echo "======================================================================"
	@mkdir -p $(WEB_DIR)
	$(CC_WEB) $(SOURCES) $(INCLUDES) -L$(WEB_DIR) $(WEB_LIB) $(WEB_RELEASE_FLAGS) -o $@
	@echo "✓ Web release build complete: $@"
	@echo ""

$(WEB_DEBUG): $(SOURCES)
	@echo "======================================================================"
	@echo "Building WebAssembly Debug..."
	@echo "======================================================================"
	@mkdir -p $(WEB_DIR)
	$(CC_WEB) $(SOURCES) $(INCLUDES) -L$(WEB_DIR) $(WEB_LIB) $(WEB_DEBUG_FLAGS) -o $@
	@echo "✓ Web debug build complete: $@"
	@echo ""

# ============================================================================
# RUN TARGETS
# ============================================================================

run: $(NATIVE_RELEASE)
	@echo "Running Native Release... ($(PLATFORM))"
	./$(NATIVE_RELEASE)

run_debug: $(NATIVE_DEBUG)
	@echo "Running Native Debug... ($(PLATFORM))"
	./$(NATIVE_DEBUG)

run_web: $(WEB_RELEASE)
	@echo "To run: cd $(WEB_DIR) && python3 -m http.server 8000"
	@echo "Then open: http://localhost:8000/game.html"

run_web_debug: $(WEB_DEBUG)
	@echo "To run: cd $(WEB_DIR) && python3 -m http.server 8000"
	@echo "Then open: http://localhost:8000/game.html (load game.debug.js)"

# ============================================================================
# CLEAN
# ============================================================================

clean:
	@echo "Cleaning build artifacts..."
	-rm -f $(BIN_DIR)/game $(BIN_DIR)/game.exe
	-rm -f $(BIN_DIR)/game_debug $(BIN_DIR)/game_debug.exe
	-rm -f $(WEB_DIR)/game.js $(WEB_DIR)/game.wasm $(WEB_DIR)/game.data
	-rm -f $(WEB_DIR)/game.debug.js $(WEB_DIR)/game.debug.wasm $(WEB_DIR)/game.debug.data
	-rm -f $(WEB_DIR)/game.debug.wasm.map
	@echo "✓ Clean complete"

# ============================================================================
# INFO
# ============================================================================

info:
	@echo "======================================================================"
	@echo "Build Configuration"
	@echo "======================================================================"
	@echo "Platform:         $(PLATFORM)"
	@echo "Native Compiler:  $(CC_NATIVE)"
	@echo "Web Compiler:     $(CC_WEB)"
	@echo "Raylib Dir:       $(RAYLIB_DIR)"
	@echo ""
	@echo "Directories:"
	@echo "  Source:  $(SRC_DIR)"
	@echo "  Library: $(LIB_DIR)"
	@echo "  Web:     $(WEB_DIR)"
	@echo "  Binary:  $(BIN_DIR)"
	@echo "  Assets:  $(ASSETS_DIR)"
	@echo ""
	@echo "Output:"
	@echo "  Release: $(NATIVE_RELEASE)"
	@echo "  Debug:   $(NATIVE_DEBUG)"
	@echo "======================================================================"