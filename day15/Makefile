CFLAGS+= -lncurses

SOLIBS += libattachable_debugger.so

SOFLAGS += -shared
LD = ${CC}

${SOLIBS}: CFLAGS += -fPIC

all: ${SOLIBS} kip

kip: charcell-signal-kip.c charcell-utils.c
	$(CC) $(CFLAGS) -o kip charcell-signal-kip.c charcell-utils.c

libattachable_debugger.so: attachable_debugger.o libctor.o

${SOLIBS}: lib%.so : %.o
	${LD} ${SOFLAGS} -o $(@F) $^ ${SHLIBS} 

clean: 
	${RM} ${SOLIBS} *.o *.bak *~


