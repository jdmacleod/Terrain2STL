# define some project variables, using git if available
PROJECTNAME := $(shell basename $(PWD))
COMMIT=$(shell git rev-parse --short HEAD 2>/dev/null || echo "no-git")
BRANCH=$(shell git rev-parse --abbrev-ref HEAD 2>/dev/null || echo "no-git")
BUILD_DATE := $(shell date +%Y-%m-%dT%H:%M:%S%z)
CURRENT_DIR := $(shell pwd)
CURRENT_USER := $(shell whoami)
VERSION := $(shell git describe --tags --abbrev=0 2>/dev/null || echo "v0.0.0-nogit")


# Define standard variables for compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Detect the operating system
OS := $(shell uname -s)

# Conditional architecture flags
ifeq ($(OS),Darwin)
################################################################################
# macOS specific settings
################################################################################
# Define architecture-specific variables
ifeq ($(shell uname -m),arm64)
# Apple Silicon (M1/M2/M3...)
BREW_PREFIX := /opt/homebrew
ARCH_FLAGS := -arch arm64 -mcpu=native
else
# Intel (x86_64)
BREW_PREFIX := /usr/local
ARCH_FLAGS := -arch x86_64
endif

else ifeq ($(OS),Linux)
################################################################################
# Linux specific settings
################################################################################
ARCH_FLAGS = -m64
endif


# Combine all flags
CFLAGS += $(ARCH_FLAGS)

# Use brew's clang if available, otherwise system default
CLANG := $(BREW_PREFIX)/opt/llvm/bin/clang
# Check if clang exists in the brew prefix
ifeq ($(wildcard $(CLANG)),)
    CLANG := clang
endif

.PHONY: help

help: ## Display this help screen
	@echo
	@echo "Usage:"
	@echo
	@sed -n 's/^\([A-Za-z0-9_.-]*\):.*## \(.*\)$$/\t\1: \2/p' Makefile | sort | column -t -s ':'
	@echo
	
default: ## Build the celevstl executable
	$(CC) src/elevstl.c src/STLWriter.c src/elevation.c -o celevstl -lm $(CFLAGS)

test: default ## test the celevstl executable
	./celevstl 44.1928 -69.0851 40 40 1.7 0 1 3 1 test.stl

zip: ## Build the celevstl-zip executable
	$(CC) src/elevstl-zip.c src/STLWriter.c src/elevation-zip.c src/readzip.c -o celevstl-zip -lm -lzip -lz $(CFLAGS)
	
test-zip: zip ## test the celevstl-zip executable
	./celevstl-zip 44.1928 -69.0851 40 40 1.7 0 1 3 1 test-zip.stl

ziptile: ## Build the celevstl-ziptile executable
	$(CC) src/elevstl-ziptile.c src/STLWriter.c src/elevation-ziptile.c src/readzip.c src/tiles.c -o celevstl-ziptile -lm -lzip -lz $(CFLAGS)

test-ziptile: ziptile ## test the celevstl-ziptile executable
	./celevstl-ziptile 44.1928 -69.0851 40 40 1.7 0 1 3 1 test-ziptile.stl

readziphgt: ## Build the readziphgt test executable
	$(CC) src/readziphgt_main.c src/readzip.c -o readziphgt -lzip -lz $(CFLAGS)

tileszip: ## Build the tileszip test executable
	$(CC) src/tileszip_main.c src/tiles.c src/readzip.c -o tileszip -lzip -lz $(CFLAGS)

server-dirs:  ## create server output directories
	mkdir -p logs
	mkdir -p stls

run-server: default server-dirs ## run the terrain server with node.js
	npm install
	node terrainServer.js

run-server-aws: ziptile server-dirs ## run the terrain server for AWS with node.js
	npm install
	node terrainServer-aws.js

clean: ## Clean up build artifacts
	rm -f celevstl celevstl-zip celevstl-ziptile readziphgt tileszip
	rm -f test.stl test-zip.stl test-ziptile.stl