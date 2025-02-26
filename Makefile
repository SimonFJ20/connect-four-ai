
MAKEFLAGS := --jobs=$(shell nproc)

CPP_FLAGS = \
	-std=c++23 \
	-Wall \
	-Wextra \
	-Wpedantic \
	-Wconversion \
	-pedantic \
	-pedantic-errors \

FEATURE_FLAGS =
OPTIMIZATION =

RELEASE=0

ifeq ($(RELEASE),1)
	CPP_FLAGS += -Werror
	FEATURE_FLAGS += -flto=auto
	OPTIMIZATION = -O3
else
	CPP_FLAGS += -g
	FEATURE_FLAGS += -fsanitize=address,undefined
	OPTIMIZATION = -Og
endif

HEADERS = $(wildcard src/*.hpp)

CPP_FILES =     \
	main.cpp    \
	board.cpp   \
	deci_tree_ai.cpp \
	nn_model.cpp \
	console.cpp \

O_FILES = $(patsubst %.cpp,build/%.o,$(CPP_FILES))

all: build_dir game

game: $(O_FILES)
	g++ -o build/$@ $^ $(FEATURE_FLAGS) $(OPTIMIZATION)

build/%.o: src/%.cpp $(HEADERS)
	g++ $< -c -o $@ $(CPP_FLAGS) $(OPTIMIZATION) $(FEATURE_FLAGS)

build_dir:
	mkdir -p build/

clean:
	rm -rf build/

