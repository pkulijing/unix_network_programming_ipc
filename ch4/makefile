CC=gcc -g -Og
INCFLAGS=-I../include
LIBFLAGS=-L../lib
CFLAGS=$(INCFLAGS) $(LIBFLAGS) -MD
OBJS=$(wildcard *.o)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<  

mainserver: mainserver.o fifoutils.o
	$(CC) $(CFLAGS) -o $@ $^ $(CFLAGS)

mainclient: mainclient.o fifoutils.o
	$(CC) $(CFLAGS) -o $@ $^ $(CFLAGS)

include $(OBJS:.o=.d)

.PHONY: clean

clean:
	rm -f *.o
	rm -f *.d
	ls | grep -v "[.][ch]" | grep -v "makefile" | xargs rm
