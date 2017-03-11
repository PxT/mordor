CFLAGS = -g -DCOMPRESS
LIBS = -lm 
CC = gcc

OBJECTS = ascfiles.o ascii1.o ascii2.o ascii3.o ascbin.o compress.o

ab: $(OBJECTS)
	gcc $(CFLAGS) $(OBJECTS) -o ab $(LIBS)

