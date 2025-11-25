CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS = -pthread

# C++ source files (ONLY the real ones)
CPP_SRC = \
    src/basicIO.cpp \
    src/main.cpp

# Assembly source file
ASM_SRC = src/syscall.S

# Object files
CPP_OBJ = $(CPP_SRC:src/%.cpp=build/%.o)
ASM_OBJ = $(ASM_SRC:src/%.S=build/%.o)

OBJ = $(CPP_OBJ) $(ASM_OBJ)

all: debug

debug: CXXFLAGS += -g
debug: build_dir $(OBJ)
	$(CXX) -nostartfiles $(CXXFLAGS) $(OBJ) $(LDFLAGS) -o cellsim_debug

release: CXXFLAGS += -O3
release: build_dir $(OBJ)
	$(CXX) -nostartfiles $(CXXFLAGS) $(OBJ) $(LDFLAGS) -o cellsim


build_dir:
	mkdir -p build

# Compile .cpp → .o
build/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Assemble .S → .o    
build/%.o: src/%.S
	nasm -f elf64 $< -o $@

clean:
	rm -rf build cellsim cellsim_debug

.PHONY: all debug release clean build_dir
