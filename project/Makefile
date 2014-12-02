CFLAGS=-g -D_GNU_SOURCE -std=c99 -lcurses

crr: crrses.o handlers.o reservations.o rooms.o crr.o -lcurses

.PHONY: clean
clean:
	$(RM) crr *.o
