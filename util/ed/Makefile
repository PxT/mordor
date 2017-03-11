CFLAGS = -O -DCOMPRESS
LIBS = -lm 
CC = cc

OBJECTS2 = editor.o editor2.o compress.o utils.o

editor: $(OBJECTS2)
	$(CC) $(CFLAGS) $(OBJECTS2) -o editor $(LIBS)
	chmod o-rwx editor 

clean: 
	rm $(OBJECTS2) editor

