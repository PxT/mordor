/*
 * PORT.C:
 *
 *	This file should isolate the platform specific function calls.
 *
 *	Copyright (C) 1998 John P. Freeman & Brooke Paul
 * $Id: port.c,v 1.3 2001/03/08 16:05:20 develop Exp $
 *
 * $Log: port.c,v $
 * Revision 1.3  2001/03/08 16:05:20  develop
 * *** empty log message ***
 *
 *
 */


/* stuff that is platform specific belongs here */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifndef WIN32
#include <dirent.h>
#endif


#include "port.h"

/* 

 C file handles are not always the same as system handles.
 Since read() and write() take C file handles, I had to change this
 for the WIN32 socket io stuff.  Thank god, it's isolated to this file.

*/
#ifdef WIN32



/************************************************************************/
/************************************************************************/
int scwrite(int fh, const void *lpvBuffer, unsigned int nLen )
{
	DWORD dwRet; 
	WriteFile((HANDLE)fh, lpvBuffer, nLen, &dwRet, NULL); 

	return(dwRet);

}


/************************************************************************/
/************************************************************************/
int scread(int fh, void *lpvBuffer, unsigned int nLen)
{

	DWORD dwRet; 
	ReadFile((HANDLE)fh, lpvBuffer, nLen, &dwRet, NULL); 
	return(dwRet);
}


/************************************************************************/
/************************************************************************/
int scclose( int fd )
{
	return( closesocket(fd ) );
}




/************************************************************************/
/************************************************************************/
int GetIniString( char *pszSection, char *pszEntry, char *pszDefault, 
	char *pszReturnBuffer, int nBufferLen, char *pszFileName)
{
	return( GetPrivateProfileString( pszSection, pszEntry, pszDefault, 
		pszReturnBuffer, nBufferLen, pszFileName));
}


/************************************************************************/
/************************************************************************/
void init_sockets()
{

#ifdef WIN32
	int			optval = SO_SYNCHRONOUS_NONALERT;
#endif /* WIN32 */


	/* gotta initialize the winsock stuff */
	WORD wVersionRequested; 
	WSADATA wsaData; 
	int err; 
	wVersionRequested = MAKEWORD(1, 1); 

#ifdef _DEBUG
	{
	// Get current flag
	int tmpFlag;
	tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );

	// Turn on leak-checking bit
	tmpFlag |= _CRTDBG_LEAK_CHECK_DF;

	// Set flag to the new value
	_CrtSetDbgFlag( tmpFlag );
	}
#endif
	
	err = WSAStartup(wVersionRequested, &wsaData); 
	if (err != 0) 
		/* Tell the user that we couldn't find a useable */ 
		/* winsock.dll.     */ 
		return; 

#ifdef WIN32
 	err = setsockopt(INVALID_SOCKET, SOL_SOCKET, SO_OPENTYPE, (char*)&optval,sizeof(optval));
#endif /* WIN32 */

	return;
}


/************************************************************************/
/************************************************************************/
void cleanup_sockets()
{
	WSACleanup();
	return;
}


/************************************************************************/
/************************************************************************/
HFINDFILE find_first_file( char *path, char *filename, int len )
{

	HFINDFILE	hff;
	HANDLE		hFind;
	WIN32_FIND_DATA	fd;
	char	buffer[256];
	int		last;

	strcpy(buffer, path);
	last = MAX(0, strlen(buffer) - 1 );
	if ( buffer[last] != '*')
	{
		if ( buffer[last] != '\\' | buffer[last] != '/' )
			strcat( buffer, "/*.*");
		else
			strcat( buffer, "*.*");
	}


	hFind = FindFirstFile(buffer, &fd);
	if ( hFind != INVALID_HANDLE_VALUE )
	{
		hff = (HFINDFILE)hFind;
		strncpy( filename, fd.cFileName, len );
		filename[len - 1] = '\0';
	}
	else
	{
		hff = NULL;
		filename[0] = '\0';
	}


	return(hff);
}


/************************************************************************/
/************************************************************************/
BOOL find_next_file(HFINDFILE hff, char *filename, int len )
{

	HANDLE	hFind;
	WIN32_FIND_DATA	fd;

	BOOL	ret;

	hFind = (HANDLE)hff;

	if ( FindNextFile(hFind, &fd) )
	{
		strncpy( filename, fd.cFileName, len );
		filename[len - 1] = '\0';
		ret = TRUE;
	}
	else
	{
		filename[0] = '\0';
		ret = FALSE;
	}

	return(ret);

}



/************************************************************************/
/************************************************************************/
void close_find_file(HFINDFILE hff)
{
	 FindClose((HANDLE)hff);
	 return;
}


typedef struct tagCloseHandle
{
	HANDLE	hsockDup;
	HANDLE	hProcess;
} CLOSEHANDLE;


/************************************************************************/
/************************************************************************/
DWORD WINAPI CloseDupHandleThread(LPVOID lpvoid )
{
	CLOSEHANDLE *pch = (CLOSEHANDLE *) lpvoid;

	WaitForSingleObject(pch->hProcess, INFINITE);

	closesocket((SOCKET)pch->hsockDup);

	return(0);
}



/************************************************************************/
/************************************************************************/
int run_child(int fd, char *args[] )
{
	PROCESS_INFORMATION	pi;
	STARTUPINFO	si;
	HANDLE	hsockDup;
	int	ret = 0;
	int	i;
	char	*p;
	char	temp[10];
	char commandline[256];

	commandline[0] = '\0';

	hsockDup = (HANDLE)-1;



	/* replace the old socket with the dup */
	i = 0;
	while ( args[i] != NULL )
	{
		p = strstr(args[i], "%d");
		if ( p )
		{
			if ( hsockDup == (HANDLE)-1 )
			{
				/* Duplicate the socket OrigSock to create an inheritable copy.*/
				if (!DuplicateHandle(GetCurrentProcess(), (HANDLE)fd,
					GetCurrentProcess(), (HANDLE*)&hsockDup, GENERIC_WRITE, FALSE, 0)) 
				{
					return(0);
				} 
			}

			sprintf( temp, args[i], (int)hsockDup );
			strcat(commandline, temp );
			strcat(commandline, " " );
		}
		else
		{
			strcat(commandline, args[i] );
			strcat(commandline, " " );
		}

		i++;
	}



	/* we dont really use this because its a detached process */
	ZeroMemory( &si, sizeof(STARTUPINFO ) );
	/* but the struct size must be correct */
	si.cb = sizeof(STARTUPINFO );

	if ( CreateProcess(NULL, commandline, NULL, NULL, TRUE, 
		DETACHED_PROCESS,
		NULL, NULL, &si, &pi) )
	{
		if ( hsockDup != (HANDLE)-1 )
		{
			CLOSEHANDLE	ch;
			DWORD	dwThreadID;

			ch.hsockDup = hsockDup;
			ch.hProcess = pi.hProcess;
			CreateThread(NULL, 0, CloseDupHandleThread, &ch, 0, &dwThreadID);
		}

		/* return the pid of the new process */
		ret = (int)pi.dwProcessId;

	}

	return(ret);
	
}


#else
/* UNIX stuff I guess */

/************************************************************************/
/************************************************************************/
int scwrite(int fh, const void *lpvBuffer, unsigned int nLen )
{
	int nReturn;

	nReturn = write( fh, lpvBuffer, nLen );

	return( nReturn );
}


/************************************************************************/
/************************************************************************/
int scread(int fh, void *lpvBuffer, unsigned int nLen)
{
	return(read( fh, lpvBuffer, nLen ));
}


/************************************************************************/
/************************************************************************/
int scclose( int fd )
{
	return( close(fd ) );
}


/************************************************************************/
/************************************************************************/
void init_sockets()
{
	/* no init needed for unix */
	return;
}

/************************************************************************/
/************************************************************************/
void cleanup_sockets()
{
	/* no cleanup needed for unix */
	return;
}



/************************************************************************/
/************************************************************************/
HFINDFILE find_first_file( char *path, char *filename, int len )
{
	struct dirent  *dirp;
    DIR     *dirfd;
	HFINDFILE	hff;

	dirfd = opendir(path);
	if ( dirfd )
	{
		hff = (HFINDFILE)dirfd;
		dirp = readdir(dirfd);
		if ( dirp )
		{
			strncpy( filename, dirp->d_name, len );
			filename[len - 1] = '\0';
		}
		else
		{
			filename[0] = '\0';
		}
	}
	else
	{
		hff = NULL;
		filename[0] = '\0';
	}

	return(hff);
}


/************************************************************************/
/************************************************************************/
BOOL find_next_file(HFINDFILE hff, char *filename, int len )
{
	struct dirent  *dirp;
    DIR     *dirfd;
	BOOL	ret;

	dirfd = (DIR *)hff;

	dirp = readdir(dirfd);
	if ( dirp )
	{
		strncpy( filename, dirp->d_name, len );
		filename[len - 1] = '\0';
		ret = TRUE;
	}
	else
	{
		filename[0] = '\0';
		ret = FALSE;
	}

	return(ret);

}



/************************************************************************/
/************************************************************************/
void close_find_file(HFINDFILE hff)
{
	 closedir((DIR *)hff);
	 return;
}


/************************************************************************/
/************************************************************************/
int run_child(int fd, char *args[] )
{
	char	temp[10];
	int		i;
	char	*p;
	int		ret;

	/* replace the %d with the socket descriptor */
	i = 0;
	while ( args[i] != NULL )
	{
		p = strstr(args[i], "%d");
		if ( p )
		{
			sprintf( temp, "%d", fd );
			strcpy( p, temp );
			break;
		}
		i++;
	}

	ret = vfork();
    if(!ret)  {
		execv(args[0], args);
		exit(0);
	}

	return(ret);
}



#ifdef NO_REANAME
/************************************************************************/
/************************************************************************/
int rename( char * oldname, char * newname )
{

	link(oldfile, newfile);
	unlink(oldfile);

	return(0);
}
#endif


/************************************************************************/
/************************************************************************/
int GetIniString( char *pszSection, char *pszEntry, char *pszDefault, 
	char *pszReturnBuffer, int nBufferLen, char *pszFileName)
{  
	FILE *pfile;
	int	nReturn = 0;
		
	/* open the file */
	pfile = fopen( pszFileName, "rt" );
	
	if ( pfile )
		{
		/* find the right section */
	   if (FindIniSection( pfile, pszSection ) )
	   	{
			/* find the right entry and read the value */
	   	if ( GetIniEntry( pfile, pszEntry, pszReturnBuffer, nBufferLen ) )
	   		{
	   		/* success */
	   		nReturn = strlen( pszReturnBuffer );
	   		}
	   	}
	   fclose(pfile);
	   }

	/* if we did not find the file, section or entry...	    */
	if ( nReturn == 0 )
		{
		/* copy the default string into the buffer */
		strncpy( pszReturnBuffer, pszDefault, nBufferLen );
		nReturn = strlen( pszReturnBuffer );
		}
	   
	
	return( nReturn );		
}


/************************************************************************/
/*	FindIniSection()													*/
/************************************************************************/
BOOL FindIniSection( FILE *pfile, char *pszSection )
{
	BOOL	bStillLooking = TRUE;
	BOOL	bSuccess = FALSE;
	char *pszLine;
	int	ndx;
	
	pszLine = malloc( MAX_INI_LINE );
	if ( pszLine )
		{
		while (bStillLooking && fgets( pszLine, MAX_INI_LINE, pfile ) != NULL )
			{
			/* strip \n if there is one */
			if ( pszLine[strlen(pszLine) - 1] == '\n' )
				pszLine[strlen(pszLine) - 1] = '\0';

			switch (GetIniLineType( pszLine ) )
				{		
				case ILT_SECTION:
					/* read the section name */
					
					/* go backwards looking for the end ']' */
					ndx = strlen( pszLine ) - 1;
					while ( ndx > 0 && pszLine[ndx] != ']' )
						{
						ndx--;
						}
					if ( pszLine[ndx] == ']' )
						{
						pszLine[ndx] = '\0';

						/* skip the lead '[' */
						if ( stricmp(pszSection, pszLine + 1 ) == 0 )
							{
							/* found it */
							bStillLooking = FALSE;
							bSuccess = TRUE;
							}
						}
					break;
					
				case ILT_BLANK:
				case ILT_COMMENT:
				case ILT_ENTRY:
					/* ignore these */
					break;
				}
			}
			
		free( pszLine );
		}
	
	return( bSuccess );
}


/************************************************************************/
/*	GetIniEntry()														*/
/************************************************************************/
BOOL GetIniEntry( FILE *pfile, char *pszEntry, char *pszReturnBuffer, int nBufferLen )
{
	BOOL	bStillLooking = TRUE;
	BOOL	bSuccess = FALSE; 
	char	*pTemp;
	char *pszLine;

	pszLine = malloc( MAX_INI_LINE );
	if ( pszLine )
		{		
		while ( bStillLooking && fgets( pszLine, MAX_INI_LINE, pfile ) )
			{
			/* strip \n if there is one */
			if ( pszLine[strlen(pszLine) - 1] == '\n' )
				pszLine[strlen(pszLine) - 1] = '\0';

			switch (GetIniLineType( pszLine ) )
				{		
				case ILT_ENTRY:
					/* is this the right entry??? */
					pTemp = strtok( pszLine, "=");
					if ( pTemp )
						{
					   if ( stricmp( pTemp, pszEntry ) == 0 )
							{
							/* found it */
							bStillLooking = FALSE;
					
							/* copy the value */
							pTemp = strtok( NULL, "=\n");
							if ( pTemp )
								{
								strncpy( pszReturnBuffer, pTemp, nBufferLen );
								bSuccess = TRUE;
								}
							}
						}
					break;
				
				case ILT_SECTION:
					/* if we got to another section we did not find the right entry */
					bStillLooking = FALSE;
					break;
				
				case ILT_BLANK:
				case ILT_COMMENT:
					/* ignore these */
					break;
				}
			}
		
		/* don't forget to free the memory	 */
		free( pszLine);
		}
	
	return(bSuccess);
}


/************************************************************************/
/*	GetIniLineType()													*/
/************************************************************************/
int GetIniLineType( char *pszLine )
{
	int	nLineType;
	char	ch;
	
	/* get the first non space char */
	StripWhiteSpace( pszLine );

	/* now that we have striped the white space off... */
	/* the first char is the right one	*/
	ch = pszLine[0];
	switch ( ch )
		{
		case ';':
			nLineType = ILT_COMMENT;
			break;

		case '[':
			nLineType = ILT_SECTION;
			break;
			
		case '\n':
			nLineType = ILT_BLANK;
			break;
			
		default:
			/* anything we cannot determine becomes an entry */
			nLineType = ILT_ENTRY;
			break;
			
		}
			
	return(nLineType);
}

/************************************************************************/
/*	StripWhiteSpace()													*/
/*		removes leading and trailing white space						*/
/************************************************************************/
void StripWhiteSpace( char *pszBuffer )
{
	char *pszCopy;
	char *p;
	int	ndx;	
	
	/* make a copy of the buffer */
	pszCopy = malloc( strlen( pszBuffer ) + 1);
	if ( pszCopy )
		{
		strcpy( pszCopy, pszBuffer );

		/* remove the trailing white space first */
		ndx = strlen( pszCopy ) - 1;
		while ( ndx >= 0 && isspace( (int)pszCopy[ndx] ) )
			{
			pszCopy[ndx] = '\0';
			ndx--;
			}
		
		/* now remove the leading white space */
		p = pszCopy;
		while ( *p != '\0' && isspace( (int)*p ) )
			{
			p++;
			}
		
		/* p now points to the first non space char in pszCopy */
		/* so just copy this back into the original buffer	*/
		strcpy( pszBuffer, p);
						
		free( pszCopy);
		}
	

	return;
}



#endif

