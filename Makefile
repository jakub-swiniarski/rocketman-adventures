SOURCES=src/main.c src/functions.c
OBJECTS=main.o functions.o
FLAGS=-lraylib -lm

main: $(OBJECTS)
	gcc -o main $(OBJECTS) $(FLAGS)

$(OBJECTS): $(SOURCES)
	gcc -c $(SOURCES)

.PHONY: clean run

clean:
	rm *.o main

run: main
	./main
