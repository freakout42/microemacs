# Makefile for MEX static

LDFLAGS=-static
LIBS=-lcurses
OBJ=main.o buffer.o cursor.o display.o files.o line.o random.o search.o term.o window.o word.o help.o

mex: main.o $(OBJ)
	$(CC) -o $@ $(LDFLAGS) $(OBJ) /opt/o2/lib/ctypeb.o $(LIBS)
	strip $@

$(SRC):	ed.h

main.o: main.c curkeys.h
	$(CC) $(CFLAGS) -Wno-missing-braces -c main.c

term.o: term.c ed.h
	$(CC) $(CFLAGS) -I/usr/include/ncurses -c term.c

curkeys.h: curkeys.c
	$(CC) -I/usr/include/ncurses curkeys.c -o curkey2
	./curkey2 >$@
	rm curkey2

help.c: help_c.eng
	cp help_c.eng $@

clean:
	rm -f *.o core* mex me curkeys.h help.c curkey2 libmex.a
