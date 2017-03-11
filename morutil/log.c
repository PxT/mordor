/* (c) 1996 Brooke Paul 
 * $Id: log.c,v 1.4 2001/07/17 19:19:06 develop Exp $
 *
 * $Log: log.c,v $
 * Revision 1.4  2001/07/17 19:19:06  develop
 * We used to segfault if the mordor.cf couldn't be
 * found at startup.  Now we print a message and exit.
 *
 * Revision 1.4  2001/07/17 19:09:49  develop
 * *** empty log message ***
 *
 * Revision 1.3  2001/03/08 16:05:27  develop
 * *** empty log message ***
 *
 */
#include <stdio.h>
#include <string.h>


#include "morutil.h"



char ROOMPATH[256] = "../rooms";
char MONPATH[256]  = "../objmon";
char OBJPATH[256]  = "../objmon";
char PLAYERPATH[256] = "../player";
char DOCPATH[256] = "../help";
char POSTPATH[256] = "../post";
char BINPATH[256] =  "../bin";
char LOGPATH[256] =  "../log";


static int loaded_paths_yet = FALSE;





/************************************************************************/
/* _assertlog()								*/
/*	called when an ASSERTLOG fails and NDEBUG is not defined	*/
/*	prints a message to the log file and shutsdown the game		*/
/*									*/
/*		JPF March 98						*/
/************************************************************************/
void _assertlog(char *strExp, char *strFile, unsigned int nLine )
{
	char	szBuffer[256];

	sprintf( szBuffer, "--- Assertion (%s) failed at file \'%s\' line %d ---", 
		strExp, strFile, nLine );

	elog(szBuffer);
	elog("Raising SIGABRT signal now" );

	/* raise the abort signal */
	raise(SIGABRT);

	return;
}

/****************************************************************/
/* get_config_file()						*/
/****************************************************************/
int get_config_file(char *filename)
{

	char	szBuffer[256];

	ASSERTLOG( filename );

	/* check for file existance */
	/* try the bin dir first */
	sprintf(filename, "%s/mordor.cf", BINPATH);

	/* check for file existance */
	if ( !file_exists( filename ) )
	{
		/* look in the current directory */
		strcpy( filename, "mordor.cf" );
		if ( !file_exists( filename ) )
		{
			if(loaded_paths_yet) {
				sprintf(szBuffer, "Missing \'%s\' file.\n", filename);
				elog(szBuffer);
			} 
			/* don't call elog if we can't find the ini, because that
			 * tries to load the paths again, which tries to load the ini, etc */
			else { 
				printf("Couldn't find mordor.cf file.  Exiting.\n");
				exit(0);
			}

			return(0);
		}
	}

	return(1);
}



/****************************************************************/
/* load_paths()							*/
/*		       						*/
/*	Reads the paths from the config file			*/
/*  NOTE: this can only be done once.  If it is reloaded during	*/
/*  a game, unpredicable results will happen!!!!		*/
/* JPF								*/
/****************************************************************/
int load_paths()
{
	char	filename[256];
	int		ret = 0;

	if ( get_config_file( filename ) )
	{
		GetIniString( "PathNames", "roompath", ROOMPATH, ROOMPATH, 255, filename);
		GetIniString( "PathNames", "monpath", MONPATH, MONPATH, 255, filename);
		GetIniString( "PathNames", "objpath", OBJPATH, OBJPATH, 255, filename);
		GetIniString( "PathNames", "playerpath", PLAYERPATH, PLAYERPATH, 255, filename);
		GetIniString( "PathNames", "docpath", DOCPATH, DOCPATH, 255, filename);
		GetIniString( "PathNames", "postpath", POSTPATH, POSTPATH, 255, filename);
		GetIniString( "PathNames", "binpath", BINPATH, BINPATH, 255, filename);
		GetIniString( "PathNames", "logpath", LOGPATH, LOGPATH, 255, filename);

		/* make sure we only do this once */
		loaded_paths_yet = TRUE;
	}
	else
		ret = -1;


	return(ret);
}




/************************************************************************/
/* get_log_path()						        */
/************************************************************************/
char *get_log_path()
{
	if ( !loaded_paths_yet )
		load_paths();

	return( LOGPATH );
}


/************************************************************************/
/* get_room_path()						        */
/************************************************************************/
char *get_room_path()
{
	if ( !loaded_paths_yet )
		load_paths();

	return( ROOMPATH );
}


/************************************************************************/
/* get_monster_path()						        */
/************************************************************************/
char *get_monster_path()
{
	if ( !loaded_paths_yet )
		load_paths();

	return( MONPATH );
}


/************************************************************************/
/* get_object_path()						        */
/************************************************************************/
char *get_object_path()
{
	if ( !loaded_paths_yet )
		load_paths();

	return( OBJPATH );
}


/************************************************************************/
/* get_player_path()						        */
/************************************************************************/
char *get_player_path()
{
	if ( !loaded_paths_yet )
		load_paths();

	return( PLAYERPATH );
}

/************************************************************************/
/* get_doc_path()						        */
/************************************************************************/
char *get_doc_path()
{
	if ( !loaded_paths_yet )
		load_paths();

	return( DOCPATH );
}


/************************************************************************/
/* get_post_path()						        */
/************************************************************************/
char *get_post_path()
{
	if ( !loaded_paths_yet )
		load_paths();

	return( POSTPATH );
}


/************************************************************************/
/* get_bin_path()						        */
/************************************************************************/
char *get_bin_path()
{
	if ( !loaded_paths_yet )
		load_paths();

	return( BINPATH );
}


/************************************************************************/
/*                              logn                                   */
/************************************************************************/
 
/* This function writes a formatted printf string to a logfile called   */
/* "name" in the log directory.                                       */
 
void logn(char *filename, char *str)
{
	char	buffer[4196];
	char    path[256];
    int     fd;

	ASSERTLOG( filename );
	ASSERTLOG( str );

    sprintf(path, "%s/%s", get_log_path(), filename);
    fd = open(path, O_RDWR | O_APPEND | O_BINARY );
    if(fd < 0) 
	{
        fd = open(path, O_RDWR | O_CREAT | O_BINARY, ACC);
        if(fd < 0) 
			return;
    }
    lseek(fd, 0L, 2);

	/* put a time stamp on everything */
	sprintf(buffer, "%s: %s\n", get_time_str(), str );
    write(fd, buffer, strlen(buffer));

    close(fd);

	return;
}




/************************************************************************/
/*				elog - logs error messages		*/
/************************************************************************/

/* This function writes a formatted printf string to a logfile called	*/
/* "error.log" in the log path directory.				*/

void elog(char *str)
{

	logn( ERROR_LOG, str);
	return;
}


/************************************************************************/
/*				plog - logs players access		*/
/************************************************************************/
/* This function writes a formatted printf string to a logfile called	*/
/* "access.log" in the log directory.					*/
void plog(char *str)
{

	logn( PLAYER_LOG, str);

	return;
}



/************************************************************************/
/*				ilog - logs immortals access		*/
/************************************************************************/
/* This function writes a formatted printf string to a logfile called	*/
/* "immoral.log" in the log directory.					*/
/*  ^^^^^^^  I think thats a typo, but its funny....  		-PxT	*/
void ilog(char *str)
{
	logn( IMMORTAL_LOG, str);

	return;
}

/************************************************************************/
/*				slog - logs status messages		*/
/************************************************************************/
/* This function writes a formatted printf string to a logfile called	*/
/* "status.log"								*/
void slog(char *str)
{
	logn( STATUS_LOG, str);

	return;
}



/************************************************************************/
/*	log_dm_command - logs immortals commands			*/
/************************************************************************/
/* This function logs the execution of a dm command in "immoral.log"	*/
/* in a consistant manner.  It is assumed the cmnd->fullstr is passed as */
/* the last argument */
void log_dm_command(char *ply_name, char *fullstr)
{
	char	buffer[2048];

	ASSERTLOG( ply_name );
	ASSERTLOG( fullstr );

	sprintf(buffer, "%s executed: %s", ply_name, fullstr );
	ilog( buffer );
	
	return;
}

