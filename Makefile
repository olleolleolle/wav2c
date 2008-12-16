CC=gcc
LD=gcc

all: wav2c

wav2c: main.o wavdata.o
	$(LD) main.o wavdata.o -o wav2c

main.o: main.c wavdata.h
	$(CC) -c main.c

wavdata.o: wavdata.c wavdata.h
	$(CC) -c wavdata.c

clean:
	rm -f *.o 

fullclean: clean
	rm -f wav2c	
