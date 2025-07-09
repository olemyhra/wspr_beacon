CC = gcc
CFLAGS1 = -Wall -Werror -O2 -g3 -std=c17 -pedantic
DEPS =  encode.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

wspr_beacon: wspr_beacon.o encode.o
	$(CC) $(CFLAGS1) wspr_beacon.o encode.o -o $@


clean:
	rm -f  wspr_beacon wspr_beacon.o encode.o
