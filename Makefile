
CPP_FLAGS = -std=c++23 -Wall -Wextra -Wpedantic -Wconversion -pedantic -pedantic-errors

RELEASE=0

ifeq ($(RELEASE),1)
	CPP_FLAGS += -Werror -O3
else
	CPP_FLAGS += -fsanitize=address,undefined
endif

HEADERS = $(wildcard src/*.hpp)

CPP_FILES =     \
	main.cpp    \
	console.cpp

O_FILES = $(patsubst %.cpp,build/%.o,$(CPP_FILES))

game: $(O_FILES)
	g++ -o build/$@ $^ $(CPP_FLAGS)

build/%.o: src/%.cpp build_dir $(HEADERS)
	g++ $< -c -o $@ $(CPP_FLAGS)

build_dir:
	mkdir -p build/

clean:
	rm -rf build/

