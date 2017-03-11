CFLAGS = -DCOMPRESS
LIBS = -lm 
CC = cc

OBJECTS = lev.o files.o 

OBJECTS2 = reset.o files.o

lev: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o lev $(LIBS)
	chmod o-rwx lev

reset: $(OBJECTS2)
	$(CC) $(CFLAGS) $(OBJECTS2) -o reset $(LIBS)
	chmod o-rwx reset


clean: 
	rm $(OBJECTS) 
