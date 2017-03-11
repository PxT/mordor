CFLAGS = -DCOMPRESS

LIBS = -lm -lc
CC = cc -g 
OUTFILE = plist

OBJECTS = plist.o compress.o utils.o

$(OUTFILE): $(OBJECTS) 
	$(CC) $(CFLAGS) $(OFLAGS) $(OBJECTS) -o $(OUTFILE) $(LIBS)
	chmod 700 $(OUTFILE)
#	strip $(OUTFILE)

clean:
	rm *.o
	rm plist
