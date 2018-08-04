CC=gcc -g -Og
INCFLAGS=-I../include
LIBFLAGS=-L../lib
CFLAGS=$(INCFLAGS) $(LIBFLAGS) -MD
LINKFLAGS=-lpthread -lutils
OBJS=$(wildcard *.o)

prodcons2:prodcons2.o
	$(CC) $(CFLAGS) -o $@ $^ $(CFLAGS) $(LINKFLAGS)

prodcons6:prodcons6.o
	$(CC) $(CFLAGS) -o $@ $^ $(CFLAGS) $(LINKFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

include $(OBJS:.o=.d)

.PHONY: clean

clean:
	rm -f *.o
	rm -f *.d
	ls | grep -v "[.][ch]" | grep -v "makefile" | xargs rm
