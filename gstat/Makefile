include ../os.mak

CFLAGS = $(CFLAGS2) -Wall -DDEBUG
LIBS = $(LIBS2) -lc -lport -lmorutil -lmordb  
OUTFILE = gstat
OBJECTS = gstat.o


$(OUTFILE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(OUTFILE) $(LIBS)
	chmod o-rwx $(OUTFILE) 
#	strip $(OUTFILE)

clean: 
	rm -f $(OBJECTS) core $(OUTFILE) out

crier: crier.o
	$(CC) -Wall $(CFLAGS) crier.o -o crier $(LIBS)
	#chmod o-rwx $(OUTFILE) 

revision:
	@ci -l *.[ch]
	@ci -l Makefile

