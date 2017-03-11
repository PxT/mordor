include ../os.mak

CFLAGS = -c $(CFLAGS2) -Wall
LIBS =
EXINC= port.h
OUTFILE = libport.a
OBJECTS = port.o


all $(OUTFILE): $(OBJECTS) $(EXINC)
#	$(CC) $(CFLAGS) $(OBJECTS)
	ar -rcs $(OUTFILE) $(OBJECTS)
	cp $(OUTFILE) ../lib/
	cp $(EXINC) ../include/

revision:
	@ci -l *.[ch]
	@ci -l Makefile

clean:
	rm -f $(OBJECTS) $(OUTFILE) 

