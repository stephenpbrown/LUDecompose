CC=gcc
COPTS=-g -Wall -std=c99
ALL=homography hmap

all: $(ALL)

JUNK= *.o *~ *.dSYM

clean:
	-rm -rf $(JUNK)

clobber:
	-rm -rf $(JUNK) $(ALL)

homography: homography.o LUdecomp.o
	$(CC) $(COPTS) $^ -o $@

hmap: hmap.o
	$(CC) $(COPTS) $^ -o $@

LUdecomp.o: LUdecomp.c LUdecomp.h
homography.o: homography.c LUdecomp.h
hmap.o: hmap.c

.c.o:
	$(CC) -c $(COPTS) $<
