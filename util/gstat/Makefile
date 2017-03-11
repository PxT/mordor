CFLAGS = -DCOMPRESS

LIBS = -lm -lc
CC = cc -g 
OUTFILE = gstat

OBJECTS = gstat.o compress.o utils.o

$(OUTFILE): $(OBJECTS) 
	$(CC) $(CFLAGS) $(OFLAGS) $(OBJECTS) -o $(OUTFILE) $(LIBS)
	chmod 700 $(OUTFILE)
#	strip $(OUTFILE)

clean:
	rm *.o
	rm gstat
