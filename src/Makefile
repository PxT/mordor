include ../os.mak

CFLAGS = $(CFLAGS2) -Wall
LIBS = $(LIBS2) -lm -lc -lmordb -lmorutil -lport 
OUTFILE = mordord 
OBJECTS = main.o global.o command1.o command2.o command3.o command4.o \
          command5.o command6.o command7.o command8.o command9.o \
          command10.o command11.o command12.o command13.o access.o \
          magic1.o magic2.o magic3.o magic4.o magic5.o magic6.o \
          magic7.o magic8.o magic9.o io.o screen.o errhandl.o \
          dm1.o dm2.o dm3.o dm4.o dm5.o dm6.o dm7.o dm8.o  readcf.o \
          player.o creature.o room.o object.o update.o post.o action.o \
          misc.o special1.o memory.o combat.o logic.o talk.o bank.o \
	  msp.o guild.o room2.o player2.o command14.o dm9.o
            


all:
	@touch io.o main.o command2.o
	@rm -f io.o main.o command2.o;make full

full $(OUTFILE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(OUTFILE) $(LIBS)
	@touch *.o
	chmod 700 $(OUTFILE)
#	strip $(OUTFILE)

revision:
	@ci -l ../include/version.h
	@ci -l *.[ch]
	@ci -l Makefile

clean: 
	rm -f $(OBJECTS) $(OUTFILE) out

port:
	rm -f io.o main.o;make

version:
	@touch command2.c io.c main.c
	make full
