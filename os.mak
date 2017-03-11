# As of 12/13/00
# Known to compile on: Linux (Pentium/Itanium, Alpha, StrongARM), FreeBSD (Pentium, Alpha),
#		       HPUX, Solaris (Sparc), AIX, Windows (cygwin gcc)
# Not recently tested: Windows (MS VisualC), IRIX

# uncomment the following if you're running under IRIX
# OSFLAGS = -DIRIX -signed  

# Uncomment the following if you are running under SunOS/Solaris.  You will need
# to add '-lsocket -lnsl' to the LIBS line too.
# OSFLAGS = -DSUNOS

# Uncomment the following line if you are on HP-UX
# OSFLAGS = -DHPUX

# Uncomment if you are running on FreeBSD (I think other BSD's too?)
# OSFLAGS = -DFREEBSD

# Uncomment if you are using Cygwin's gcc under windows. (www.cygwin.com)
# OSFLAGS=-DCYGWIN

# Uncomment if you are running Linux on HP's PARISC architecture
# OSFLAGS=-DPALINUX

# Uncomment if you are running IBM's AIX
# OSFLAGS=-DAIX

# uncomment the following if you want to use the dmalloc debugger
# you will need to add '-ldmalloc' to the LIBS line too.
#OSFLAGS = -DDMALLOC  # -DDEBUG
#OSFLAGS = -DDEBUG
#OSFLAGS = -DMOBS_ALWAYS_ACTIVE


# Read the COMPILE file to see more information on the various 
# CFLAG options.

#CFLAGS2 = -DCOMPRESS -DFINGERACCT $(OSFLAGS) 
CFLAGS2 = -g -DCOMPRESS -DFINGERACCT $(OSFLAGS) 

LIBS2 = -L../lib 
CC = cc 
