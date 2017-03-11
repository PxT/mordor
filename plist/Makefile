include ../os.mak

CFLAGS = $(CFLAGS2)
LIBS = $(LIBS2) -lc -lport -lmorutil -lmordb  
OUTFILE = plist
OBJECTS = plist.o


$(OUTFILE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(OUTFILE) $(LIBS)
	chmod o-rwx $(OUTFILE) 

revision:
	@ci -l *.[ch]
	@ci -l Makefile

clean: 
	rm $(OBJECTS) core $(OUTFILE) out

