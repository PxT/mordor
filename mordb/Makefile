include ../os.mak

CFLAGS = -c $(CFLAGS2) -Wall
LIBS = 
EXINC= mordb.h
OUTFILE = libmordb.a
OBJECTS = array.o active.o compress.o files1.o files2.o files3.o files4.o


all $(OUTFILE): $(OBJECTS) $(EXINC)
#	$(CC) $(CFLAGS) $(OBJECTS)
	ar -rcs $(OUTFILE) $(OBJECTS)
	cp $(OUTFILE) ../lib/
	cp $(EXINC) ../include/
#	make revision

revision:
	@ci -l *.[ch]
	@ci -l Makefile

clean: 
	rm -f $(OBJECTS) $(OUTFILE) 

