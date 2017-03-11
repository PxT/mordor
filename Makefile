# Note: see the file os.mak for specific options

MAINSUBDIRS = port morutil mordb mordor
ALLSUBDIRS = port morutil mordb mordor ascbin auth editor list plist gstat chkdb
VER := $(shell grep VERSION include/version.h | cut -d" " -f3 | sed 's/"//g')

all: 
	@for x in $(MAINSUBDIRS); \
	do \
		(cd $$x; echo Making $@ in:; pwd; \
		$(MAKE) $@); \
	done

clean: 
	@for x in $(MAINSUBDIRS); \
	do \
		(cd $$x; echo Making $@ in:; pwd; \
		$(MAKE) $@); \
	done
	rm -f include/mor* include/port.h lib/*

world: all ascbin editor auth list plist chkdb
	echo Done.	

mordor: libs
	(cd mordor ; make )

patch:
	../makepatch $@

libs:
	(cd port ; make)
	(cd morutil ; make)
	(cd mordb ; make)

veryclean:
	@for x in $(ALLSUBDIRS); \
	do \
		(cd $$x; echo Making $@ in:; pwd; \
		$(MAKE) clean); \
	done
	rm -f lib/* include/mor*.h include/port.h
	rm -f bin/mordor bin/list bin/plist bin/editor bin/auth bin/ascbin
	find . -name \*.rej -exec rm -f {} \;
	find . -name \*.orig -exec rm -f {} \;
	find . -name core -exec rm -f {} \;
	
editor: libs
	(cd editor ; make)

gstat: libs
	(cd gstat ; make)

ascbin: libs
	(cd ascbin ; make)

auth: libs
	(cd auth ; make)

convert:
	(cd convert ; make)

list: libs
	(cd list ; make)

plist: libs
	(cd plist ; make)

chkdb: libs
	(cd chkdb ; make)

dist: veryclean
	tar -zcvf ../mordor_full_$(VER).tgz mordor mordb morutil include gstat \
		editor auth plist list ascbin convert port os.mak Makefile

smalldist: clean
	(mv player .player ; mkdir player)
	(mv bin .bin ; mkdir bin ; cp mordor/mordor.cf bin/mordor.cf)
	(mv rooms .rooms ; mkdir rooms)	
	(mv log .log ; mkdir log)
	tar --exclude RCS -zcvf ../mordor_${VER}.tgz $(MAINSUBDIRS) os.mak Makefile player bin rooms include lib log
	rm -rf bin rooms player log
	(mv .player player ; mv .rooms rooms ; mv .bin bin ; mv .log log)

