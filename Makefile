SOURCES=$(wildcard src/*.c)
HEADERS=$(wildcard src/*.h)
OBJECTS=$(patsubst src/%.c,%.o,$(SOURCES))
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

install: rocketman res
	mkdir -p /usr/share/rocketman/
	cp rocketman /usr/bin/
	cp res/* /usr/share/rocketman/

uninstall:
	rm /usr/bin/rocketman
	rm -rf /usr/share/rocketman
