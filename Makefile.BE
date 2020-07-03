# Makefile for me on BeOS

LIBS=	-ltermcap

OBJ=	buffer.o cursor.o files.o \
	line.o random.o \
	search.o window.o word.o \
	display.o help.o main.o term.o

me: $(OBJ)
	cc -o me $(LFLAGS) $(OBJ) $(LIBS)

buffer.o: buffer.c ed.h
	cc $(CFLAGS) -c buffer.c

cursor.o: cursor.c ed.h
	cc $(CFLAGS) -c cursor.c

display.o: display.c ed.h
	cc $(CFLAGS) -c display.c

files.o: files.c ed.h
	cc $(CFLAGS) -c files.c

help.o: help.c ed.h
	cc $(CFLAGS) -c help.c

line.o: line.c ed.h
	cc $(CFLAGS) -c line.c

main.o: main.c ed.h
	cc $(CFLAGS) -c main.c

random.o: random.c ed.h
	cc $(CFLAGS) -c random.c

search.o: search.c ed.h
	cc $(CFLAGS) -c search.c

term.o: term.c ed.h
	cc $(CFLAGS) -c term.c

window.o: window.c ed.h
	cc $(CFLAGS) -c window.c

word.o: word.c ed.h
	cc $(CFLAGS) -c word.c

help.c: help_c.eng
	-ln -f $? $@

install: me
	strip me
	su bin -c "cp me /boot/home/bin"

clean:
	rm -f *.o core me curkeys.h help.c
