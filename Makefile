SOURCES=$(wildcard src/*.c)
HEADERS=$(wildcard headers/*.h)
OBJECTS=functions.o main.o
FLAGS=-lraylib -lm

rocketman: $(OBJECTS)
	gcc -o rocketman $(OBJECTS) $(FLAGS)

$(OBJECTS): $(SOURCES) $(HEADERS)
	gcc -c $(SOURCES)

.PHONY: clean run install uninstall

clean:
	rm *.o rocketman

run: rocketman
	./rocketman

install: rocketman assets_rocketman/
	cp rocketman /usr/bin/
	cp -r assets_rocketman /usr/share/

uninstall:
	rm /usr/bin/rocketman
	rm -rf /usr/share/assets_rocketman
