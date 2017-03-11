/* (c) 1996 Brooke Paul 
 * $Id: port.h,v 1.3 2001/03/08 16:05:20 develop Exp $
 *
 * $Log: port.h,v $
 * Revision 1.3  2001/03/08 16:05:20  develop
 * *** empty log message ***
 *
 */

#ifndef __PORT_H_
#define __PORT_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/* platform specific includes */

#ifdef FREEBSD
       #define _BSD_COMPAT
       #include <sys/types.h>
       #include <signal.h>
#endif


#ifdef SUNOS
        #include <sys/fcntl.h>
        #include <sys/filio.h>
#endif

#ifdef CYGWIN
	#include <signal.h>
#endif

#ifdef IRIX
	#define _BSD_COMPAT
	/* #include <stropts.h> */
	#include <fcntl.h>
#endif


#ifndef WIN32
	#include <sys/ioctl.h>
	#include <sys/socket.h>
	#include <sys/signal.h>
	#include <sys/wait.h>
	#include <sys/time.h>
	#include <sys/file.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <unistd.h>
	#include <ctype.h>
/*	#include <sys/io.h> */
#else
	#include <time.h>
	#include <process.h>
	#include <signal.h>
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <fcntl.h>

	/* define the size the the fd_set table */
	#define FD_SETSIZE	256
	#include <winsock.h>
	#include <io.h>
	#include <crtdbg.h>
#endif


#include <sys/stat.h>



#ifdef WIN32


/* functions that do the same thing but had named changed */
#define ioctl(a,b,c)	ioctlsocket(a,b,c)

/* no function necessary in Windows */
/* we can defne the size ourselves with FD_SETSIZE */
#define getdtablesize()	 (FD_SETSIZE)

#define index( str, ch)	strchr(str, ch)

#define kill(a, b)	

#define sleep(a)	Sleep(a * 1000 )

#else

/* UNIX specific stuff */
#ifdef IRIX
#define vfork	fork
#define _BSD_COMPAT
#endif


typedef int BOOL;

#ifndef FALSE
#define FALSE    	(0)
#endif

#ifndef TRUE
#define TRUE     	(!FALSE)
#endif

#ifndef O_BINARY    /* cygwin defines this for us */
#define O_BINARY        0
#endif

#define MAX_INI_LINE	512

/* INI Line Types */
#define ILT_BLANK		0
#define ILT_SECTION		1
#define ILT_ENTRY		2
#define	ILT_COMMENT		3

extern BOOL FindIniSection( FILE *pfile, char *pszSection );
extern BOOL GetIniEntry( FILE *pfile, char *pszEntry, char *pszReturnBuffer, int nBufferLen );
extern int GetIniLineType( char *pszLine );
extern void StripWhiteSpace( char *pszBuffer );

#ifdef NO_RENAME
extern int rename( char * oldname, char * newname );
#endif

#endif


#ifndef MIN
#define MIN(a,b)        (((a)<(b)) ? (a):(b))
#endif

#ifndef MAX
#define MAX(a,b)        (((a)>(b)) ? (a):(b))
#endif



/* File permissions */
/*#define S_IWRITE      00660 */
/*#define S_IREAD       00006 */
/* ACC is supposed to be permissions for RW for files */
#ifndef WIN32 
#define ACC             00660
#else
#define ACC             (_S_IREAD | _S_IWRITE)
#endif


#define stricmp	strcasecmp



/* PORT.H  header file for os specific stuff */
typedef void * HFINDFILE;



extern int scwrite(int fh, const void *lpvBuffer, unsigned int nLen );
extern int scread(int fh, void *lpvBuffer, unsigned int nLen);
extern int scclose( int fd );
extern void init_sockets(void);
extern void cleanup_sockets(void);
extern HFINDFILE find_first_file( char *path, char *filename, int len );
extern BOOL find_next_file(HFINDFILE hff, char *filename, int len);
extern void close_find_file(HFINDFILE hff);
extern int run_child(int fd, char *args[] );

extern int GetIniString( char *pszSection, char *pszEntry, char *pszDefault, 
	char *pszReturnBuffer, int nBufferLen, char *pszFileName);



#endif
