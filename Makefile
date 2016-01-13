SRC = $(wildcard *.c)
OBJ = $(patsubst %.c,bin/%.o, $(SRC))

all: main

bin/%.o: %.c *.h
	gcc -o $@ -c $< `pkg-config --cflags opencv --libs opencv`

main : $(OBJ)
	gcc -o $@ $^ -Wall `pkg-config --cflags opencv --libs opencv` -ldl -lm -lpthread

clean:
	rm -rf bin/*.o serie
