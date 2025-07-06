
wspr_beacon: wspr_beacon.c encode.o
	gcc wspr_beacon.c -o wspr_beacon

encode.o: encode.c
	gcc -c encode.c -o encode.o

clean:
	rm *.o wspr_beacon
