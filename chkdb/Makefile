include ../os.mak

CFLAGS = $(CFLAGS2)
LIBS = $(LIBS2) -lc -lmordb -lmorutil -lport    
OUTFILE = chkdb
OBJECTS = chkdb.o


$(OUTFILE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(OUTFILE) $(LIBS)
	chmod o-rwx $(OUTFILE) 
#	strip $(OUTFILE)

revision:
	@ci -l *.[ch]
	@ci -l Makefile
clean: 
	rm $(OBJECTS) core $(OUTFILE) out


