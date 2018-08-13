CC=gcc -g -Og
INCFLAGS=-I../include
LIBFLAGS=-L../lib
CFLAGS=$(INCFLAGS) $(LIBFLAGS) -MD
LINKFLAGS=-lpthread -lutils
OBJS=$(wildcard *.o)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

include $(OBJS:.o=.d)

.PHONY: clean

clean:
	rm -f *.o
	rm -f *.d
	ls | grep -v "[.][ch]" | grep -v "makefile" | xargs rm
