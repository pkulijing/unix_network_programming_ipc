CC=gcc -g -Og
INCFLAGS=-I../include
LIBFLAGS=-L../lib
CFLAGS=$(INCFLAGS) $(LIBFLAGS) -MD
LINKFLAGS=-lrt
OBJS=$(wildcard *.o)

mqnotifythread1: mqnotifythread1.o mqutils.o
	$(CC) $(CFLAGS) -o $@ $^ $(CFLAGS) $(LINKFLAGS)

mqnotifysig5: mqnotifysig5.o mqutils.o
	$(CC) $(CFLAGS) -o $@ $^ $(CFLAGS) $(LINKFLAGS)

mqnotifysig4: mqnotifysig4.o mqutils.o
	$(CC) $(CFLAGS) -o $@ $^ $(CFLAGS) $(LINKFLAGS)

mqnotifysig3: mqnotifysig3.o mqutils.o
	$(CC) $(CFLAGS) -o $@ $^ $(CFLAGS) $(LINKFLAGS)

mqnotifysig2: mqnotifysig2.o mqutils.o
	$(CC) $(CFLAGS) -o $@ $^ $(CFLAGS) $(LINKFLAGS)

mqnotifysig1: mqnotifysig1.o mqutils.o
	$(CC) $(CFLAGS) -o $@ $^ $(CFLAGS) $(LINKFLAGS)

mqsend: mqsend.o mqutils.o
	$(CC) $(CFLAGS) -o $@ $^ $(CFLAGS) $(LINKFLAGS)

mqreceive: mqreceive.o mqutils.o
	$(CC) $(CFLAGS) -o $@ $^ $(CFLAGS) $(LINKFLAGS)

mqgetattr: mqgetattr.o mqutils.o
	$(CC) $(CFLAGS) -o $@ $^ $(CFLAGS) $(LINKFLAGS)

mqcreate1: mqcreate1.o mqutils.o
	$(CC) $(CFLAGS) -o $@ $^ $(CFLAGS) $(LINKFLAGS)

mqcreate2: mqcreate2.o mqutils.o
	$(CC) $(CFLAGS) -o $@ $^ $(CFLAGS) $(LINKFLAGS)

mqunlink: mqunlink.o mqutils.o
	$(CC) $(CFLAGS) -o $@ $^ $(CFLAGS) $(LINKFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

include $(OBJS:.o=.d)

.PHONY: clean

clean:
	rm -f *.o
	rm -f *.d
	ls | grep -v "[.][ch]" | grep -v "makefile" | xargs rm
