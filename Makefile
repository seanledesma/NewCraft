# ============================================================================
# Makefile for Raylib Game Project
# Supports: Native (macOS/Linux) and WebAssembly builds
# ============================================================================

# --- Compiler Detection ---
OS_NAME := $(shell uname -s)
CC_NATIVE := gcc
CC_WEB := emcc

# Detect macOS
ifeq ($(OS_NAME),Darwin)
    CC_NATIVE = clang
endif

# --- Project Structure ---
SRC_DIR := src
LIB_DIR := lib
WEB_DIR := web
BIN_DIR := bin
ASSETS_DIR := assets

# Source files
SOURCES := $(SRC_DIR)/main.c \

# Include directories
INCLUDES := -I$(SRC_DIR) -I$(LIB_DIR)

# --- Output Files ---
NATIVE_RELEASE := $(BIN_DIR)/game
NATIVE_DEBUG := $(BIN_DIR)/game_debug
WEB_RELEASE := $(WEB_DIR)/game.js
WEB_DEBUG := $(WEB_DIR)/game.debug.js

# ============================================================================
# NATIVE BUILD FLAGS
# ============================================================================

# Common flags for all native builds
NATIVE_COMMON_FLAGS := -Wall -Wextra -Wpedantic

# macOS-specific libraries
ifeq ($(OS_NAME),Darwin)
    NATIVE_LIBS := -L$(LIB_DIR) \
                   -framework CoreVideo \
                   -framework IOKit \
                   -framework Cocoa \
                   -framework GLUT \
                   -framework OpenGL \
                   $(LIB_DIR)/libraylib.a
endif

# Linux-specific libraries
ifeq ($(OS_NAME),Linux)
    NATIVE_LIBS := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

# Native Release Flags (Optimized)
NATIVE_RELEASE_FLAGS := $(NATIVE_COMMON_FLAGS) \
                        -O3 \
                        -DNDEBUG \
                        -march=native

# Native Debug Flags (Maximum debugging info) -fsanitize=leak 
NATIVE_DEBUG_FLAGS := $(NATIVE_COMMON_FLAGS) \
                      -g3 \
                      -O0 \
                      -DDEBUG \
                      -fsanitize=address \
                      -fsanitize=undefined \
                      -fno-omit-frame-pointer \
                      -fstack-protector-strong

# ============================================================================
# WEB BUILD FLAGS
# ============================================================================

# Common flags for all web builds
WEB_COMMON_FLAGS := -s USE_GLFW=3 \
                    -s ASYNCIFY \
                    -DPLATFORM_WEB \
                    --preload-file $(ASSETS_DIR)

# Web library
WEB_LIB := $(WEB_DIR)/libraylib.a

# Web Release Flags (Optimized for size and performance)
WEB_RELEASE_FLAGS := $(WEB_COMMON_FLAGS) \
                     -Os \
                     -Wall \
                     -s TOTAL_MEMORY=67108864 \
					 -s STACK_SIZE=1048576 \

# Web Debug Flags (Maximum debugging and safety checks) -s DEMANGLE_SUPPORT=1 
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

.PHONY: all clean help
.PHONY: native native_debug native_release
.PHONY: web web_debug web_release
.PHONY: run run_debug run_web run_web_debug

# Default target
all: native_release

# --- Help Target ---
help:
	@echo "======================================================================"
	@echo "  Raylib Game Build System"
	@echo "======================================================================"
	@echo ""
	@echo "Native Targets:"
	@echo "  make native          - Build native release (alias for native_release)"
	@echo "  make native_release  - Build optimized native executable"
	@echo "  make native_debug    - Build native with debug symbols and sanitizers"
	@echo "  make run             - Build and run native release"
	@echo "  make run_debug       - Build and run native debug"
	@echo ""
	@echo "Web Targets:"
	@echo "  make web             - Build web release (alias for web_release)"
	@echo "  make web_release     - Build optimized WebAssembly"
	@echo "  make web_debug       - Build WebAssembly with debug symbols"
	@echo "  make run_web         - Build web release and show instructions"
	@echo "  make run_web_debug   - Build web debug and show instructions"
	@echo ""
	@echo "Other:"
	@echo "  make clean           - Remove all build artifacts"
	@echo "  make help            - Show this help message"
	@echo ""
	@echo "======================================================================"

# ============================================================================
# NATIVE BUILDS
# ============================================================================

native: native_release

native_release: $(NATIVE_RELEASE)

native_debug: $(NATIVE_DEBUG)

$(NATIVE_RELEASE): $(SOURCES)
	@echo "======================================================================"
	@echo "Building Native Release..."
	@echo "======================================================================"
	@mkdir -p $(BIN_DIR)
	$(CC_NATIVE) $(SOURCES) $(INCLUDES) $(NATIVE_RELEASE_FLAGS) -o $@ $(NATIVE_LIBS)
	@echo "✓ Native release build complete: $@"
	@echo ""

$(NATIVE_DEBUG): $(SOURCES)
	@echo "======================================================================"
	@echo "Building Native Debug (with AddressSanitizer)..."
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
	@echo "  Output files: game.js, game.wasm, game.data"
	@echo ""

$(WEB_DEBUG): $(SOURCES)
	@echo "======================================================================"
	@echo "Building WebAssembly Debug..."
	@echo "======================================================================"
	@mkdir -p $(WEB_DIR)
	$(CC_WEB) $(SOURCES) $(INCLUDES) -L$(WEB_DIR) $(WEB_LIB) $(WEB_DEBUG_FLAGS) -o $@
	@echo "✓ Web debug build complete: $@"
	@echo "  Output files: game.debug.js, game.debug.wasm, game.debug.data"
	@echo "  Source map: game.debug.wasm.map"
	@echo ""

# ============================================================================
# RUN TARGETS
# ============================================================================

run: $(NATIVE_RELEASE)
	@echo "======================================================================"
	@echo "Running Native Release..."
	@echo "======================================================================"
	./$(NATIVE_RELEASE)

run_debug: $(NATIVE_DEBUG)
	@echo "======================================================================"
	@echo "Running Native Debug (AddressSanitizer enabled)..."
	@echo "======================================================================"
	@echo "Note: Memory errors will be caught and reported"
	@echo ""
	./$(NATIVE_DEBUG)

run_web: $(WEB_RELEASE)
	@echo "======================================================================"
	@echo "Web Release Build Complete"
	@echo "======================================================================"
	@echo ""
	@echo "To run the web build:"
	@echo "  1. cd $(WEB_DIR)"
	@echo "  2. python3 -m http.server 8000"
	@echo "  3. Open browser to: http://localhost:8000/game.html"
	@echo ""
	@echo "Make sure your HTML file loads 'game.js'"
	@echo "======================================================================"

run_web_debug: $(WEB_DEBUG)
	@echo "======================================================================"
	@echo "Web Debug Build Complete"
	@echo "======================================================================"
	@echo ""
	@echo "To run the debug build:"
	@echo "  1. cd $(WEB_DIR)"
	@echo "  2. python3 -m http.server 8000"
	@echo "  3. Update your HTML to load 'game.debug.js' instead of 'game.js'"
	@echo "  4. Open browser to: http://localhost:8000/game.html"
	@echo "  5. Open browser console (F12) to see debug output"
	@echo ""
	@echo "Debug features enabled:"
	@echo "  - Source maps for debugging"
	@echo "  - Memory safety checks (SAFE_HEAP)"
	@echo "  - Stack overflow detection"
	@echo "  - Assertion checks"
	@echo "  - WebGL debug mode"
	@echo ""
	@echo "======================================================================"

# ============================================================================
# CLEAN TARGET
# ============================================================================

clean:
	@echo "======================================================================"
	@echo "Cleaning build artifacts..."
	@echo "======================================================================"
	-rm -f $(NATIVE_RELEASE)
	-rm -f $(NATIVE_DEBUG)
	-rm -f $(WEB_DIR)/game.js
	-rm -f $(WEB_DIR)/game.wasm
	-rm -f $(WEB_DIR)/game.data
	-rm -f $(WEB_DIR)/game.debug.js
	-rm -f $(WEB_DIR)/game.debug.wasm
	-rm -f $(WEB_DIR)/game.debug.data
	-rm -f $(WEB_DIR)/game.debug.wasm.map
	@echo "✓ Clean complete"
	@echo ""

# ============================================================================
# DEBUGGING INFO TARGET (useful for troubleshooting)
# ============================================================================

.PHONY: info
info:
	@echo "======================================================================"
	@echo "Build Configuration"
	@echo "======================================================================"
	@echo "Operating System: $(OS_NAME)"
	@echo "Native Compiler:  $(CC_NATIVE)"
	@echo "Web Compiler:     $(CC_WEB)"
	@echo ""
	@echo "Directories:"
	@echo "  Source:  $(SRC_DIR)"
	@echo "  Library: $(LIB_DIR)"
	@echo "  Web:     $(WEB_DIR)"
	@echo "  Binary:  $(BIN_DIR)"
	@echo "  Assets:  $(ASSETS_DIR)"
	@echo ""
	@echo "Source Files:"
	@echo "$(SOURCES)" | tr ' ' '\n' | sed 's/^/  /'
	@echo ""
	@echo "======================================================================"