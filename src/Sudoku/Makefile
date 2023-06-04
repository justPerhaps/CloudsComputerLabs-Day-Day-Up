CXXFLAGS+=-O2 -ggdb -DDEBUG
CXXFLAGS+=-Wall -Wextra

all: sudoku_solve

sudoku_solve: main.cc sudoku_basic.cc neighbor.cc
	gcc -o $@ $^ -lpthread

clean:
	rm sudoku_solve
