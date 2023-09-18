SOURCES=main.c
OBJECTS=main.o
FLAGS=-lraylib

main: $(OBJECTS)
	gcc -o main $(OBJECTS) $(FLAGS)

$(OBJECTS): $(SOURCES)
	gcc -c $(SOURCES)

.PHONY: clean run

clean:
	rm *.o main

run: main
	./main
