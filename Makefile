SRC = $(wildcard *.c)
OBJ = $(patsubst %.c,bin/%.o, $(SRC))
EXE=cam.out

all: $(EXE)

bin/%.o: %.c *.h
	gcc -o $@ -c $< `pkg-config --cflags opencv --libs opencv`

$(EXE) : $(OBJ)
	gcc -o $@ $^ -Wall `pkg-config --cflags opencv --libs opencv` -ldl -lm -lpthread

clean:
	rm -rf bin/*.o *.out
