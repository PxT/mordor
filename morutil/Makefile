include ../os.mak

CFLAGS = -c $(CFLAGS2)
LIBS = 
EXINC= morutil.h
OUTFILE = libmorutil.a
OBJECTS = log.o utils.o


all $(OUTFILE): $(OBJECTS) $(EXINC)
#	$(CC) $(CFLAGS) $(OBJECTS)
	ar -rcs $(OUTFILE) $(OBJECTS)
	cp $(OUTFILE) ../lib/
	cp $(EXINC) ../include/
	#make revision

revision:
	@ci -l *.[ch]
	@ci -l Makefile

clean: 
	rm -f $(OBJECTS) $(OUTFILE) out
 

