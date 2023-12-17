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
	mkdir -p /usr/local/share/rocketman/
	cp rocketman /usr/local/bin/
	cp res/* /usr/local/share/rocketman/

uninstall:
	rm /usr/local/bin/rocketman
	rm -rf /usr/local/share/rocketman
