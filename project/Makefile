CFLAGS=-g -D_GNU_SOURCE -std=c99 -lncurses -Wall

crr: crrses.o handlers.o reservations.o rooms.o crr.o -lncurses

tar: crr.tar.bz2

crr.tar.bz2: *.c *.h
	tar -cjf crr.tar.bz2 *.{h,c}

.PHONY: clean
clean:
	$(RM) crr *.o
