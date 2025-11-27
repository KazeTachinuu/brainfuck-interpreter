CC = gcc
CFLAGS = -Wall -Wextra -pedantic -O2
SRC = brainfuck.c
EXE = bf

all: $(EXE)

$(EXE): $(SRC)
	$(CC) $(CFLAGS) -o $(EXE) $(SRC)

.PHONY: clean test

clean:
	rm -f $(EXE)

test: $(EXE)
	./test.sh ./$(EXE)

