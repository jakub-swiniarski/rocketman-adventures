SRC = $(wildcard *.c)
HDR = $(wildcard *.h)
OBJ = $(SRC:.c=.o)

all: rocketman

%.o: %.c
	gcc -c -std=c99 -O2 $<

$(OBJ): $(HDR)

rocketman: $(OBJ)
	gcc -o $@ $(OBJ) -lraylib -lm

run: all
	./rocketman

clean:
	rm -f *.o rocketman

install: all
	mkdir -p /usr/local/share/rocketman/
	cp -f rocketman /usr/local/bin/
	cp -f res/* /usr/local/share/rocketman/

uninstall:
	rm -f /usr/local/bin/rocketman
	rm -rf /usr/local/share/rocketman

.PHONY: all run clean install uninstall
