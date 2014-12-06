CFLAGS=-g -D_GNU_SOURCE -std=c99 -lncurses -Wall

crr: crrses.o handlers.o reservations.o rooms.o crr.o -lncurses

.PHONY: clean
clean:
	$(RM) crr *.o
