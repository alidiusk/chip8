CC=clang
CFLAGS=-Wall -Weverything -Wextra -Wno-c++98-compat -std=c++17 -g
LDFLAGS=-lSDL2 -lstdc++ -lm
INCLUDE=include

SRC=$(wildcard src/*.cpp)
OBJ=$(patsubst %.cpp, %.o, $(SRC))

all: main

%.o: %.cpp
	$(CC) -I$(INCLUDE) $(CFLAGS) -o $@ -c $<

main: $(OBJ)
	$(CC) -I$(INCLUDE) $(CFLAGS) $(LDFLAGS) -o $@ $^

.PHONY: clean

clean:
	@rm $(OBJ)
	@rm main
