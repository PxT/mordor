CFLAGS = 
LIBS =  
CC = cc
OUTFILE = chkdb
OBJECTS = utils.o ckdb.o


chkdb: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(OUTFILE) $(LIBS)
clean: 
	\rm $(OBJECTS) core $(OUTFILE) 

