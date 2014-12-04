CFLAGS+= --std=c99
TARGET_APP= thermostat example example-ta
TARGET_SO= libtimeaccel.so

thermostat.o timeaccel.o example-thermostat-client.o rvgs.o rngs.o: CFLAGS+= -Wno-unused-value 
thermostat.o timeaccel.o example-thermostat-client.o rvgs.o rngs.o: CFLAGS+= -Wno-unused-but-set-variable
thermostat.o timeaccel.o example-thermostat-client.o rvgs.o rngs.o: CFLAGS+= -Wno-unused-function

thermostat: thermostat.o rooms.o tmodel.o rvgs.o rngs.o

timeaccel.o: CFLAGS+=-fPIC
libtimeaccel.so: timeaccel.o

example: example-thermostat-client.o

example-ta: LDFLAGS+=-L.
example-ta: LIBS+=-ltimeaccel -ldl
example-ta: example-thermostat-client.o
