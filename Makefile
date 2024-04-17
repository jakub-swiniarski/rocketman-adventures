SRC = $(wildcard *.c)
HDR = $(wildcard *.h)
OBJ = $(SRC:.c=.o)

rocketman: $(OBJ)
	gcc -o $@ $(OBJ) -lraylib -lm

$(OBJ): $(SRC) $(HDR)
	gcc -c $(SRC) -std=c99 -O2

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
