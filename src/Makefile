CFLAGS = -g -O2 -DCOMPRESS -DAUTOSHUTDOWN -DFINGERACCT -DCHECKDOUBLE -DSUICIDE
LIBS = -lm
CC = cc
OUTFILE = frp.new

OBJECTS = global.o main.o files1.o files2.o files3.o command1.o command2.o \
	  command3.o command4.o command5.o command6.o command7.o \
	  command8.o command9.o command10.o command11.o magic1.o magic2.o \
	 magic3.o magic4.o  magic5.o magic6.o magic7.o magic8.o \
	 dm1.o dm2.o dm3.o dm4.o dm5.o dm6.o io.o \
	  player.o creature.o room.o object.o update.o post.o action.o misc.o \
	  special1.o compress.o

$(OUTFILE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(OUTFILE) $(LIBS)
	chmod 700 $(OUTFILE)
#	strip $(OUTFILE)

# Need to add options to make utils here too
auth: auth.o
	$(CC) auth.o -o auth
	mv auth ../bin

clean: 
	rm $(OBJECTS)

port:
	rm -f io.o main.o;
	make

fing: finger.o
	$(CC) finger.o -o fing
	mv fing ../bin

