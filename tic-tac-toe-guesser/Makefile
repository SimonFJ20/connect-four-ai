
CFLAGS = -Wall -Wextra -pedantic -pedantic-errors -Wconversion -fsanitize=address,undefined -g -Og
LIBERALS = -lm

all:
	gcc *.c $(CFLAGS) $(LIBERALS)

release:
	gcc *.c $(CFLAGS) $(LIBERALS) -O3 -flto=auto

