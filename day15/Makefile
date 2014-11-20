
SOLIBS += libattachable_debugger.so

SOFLAGS += -shared
LD = ${CC}

${SOLIBS}: CFLAGS += -fPIC

all: ${SOLIBS} 

libattachable_debugger.so: attachable_debugger.o

${SOLIBS}: lib%.so : %.o :
	${LD} ${SOFLAGS} -o $(@F) $^ ${SHLIBS} 

clean: 
	${RM} ${SOLIBS} *.o *.bak *~


