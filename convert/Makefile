CC = gcc -w -g -DCOMPRESS -DPxT
all: 
	$(CC) -c convert.c -o convert.o
	$(CC) -c files1.c -o files1.o
	$(CC) -o convert files1.o convert.o -L ./lib -lmordb -lmorutil -lport

revision:
	@ci -l *.[ch]
	@ci -l Makefile

clean:
	\rm *.o convert
