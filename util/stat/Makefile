CFLAGS = -g -DCOMPRESS
LIBS = -lm 
CC = cc

OBJECTS = gstat.o utils.o compress.o

OBJECTS2 = stat.o utils.o compress.o

gstat: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o playerstats $(LIBS)
	chmod o-rwx playerstats
stat:  $(OBJECTS2)
	$(CC) $(CFLAGS) $(OBJECTS2) -o roomstats $(LIBS)
	chmod o-rwx roomstats

clean: 
	rm $(OBJECTS) 
