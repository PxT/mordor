/*
 * SCREEN.C:
 *
 *	 Screen handling routines.
 *	Copyright 1993, 1999 Brooke Paul
 *
 *
 * $Id: screen.c,v 6.13 2001/03/08 16:09:09 develop Exp $
 *
 * $Log: screen.c,v $
 * Revision 6.13  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */
#include "../include/mordb.h"
#include "mextern.h"


void ANSI( int fd, int color )
{
	char	str[25];

	/* a check for debug version */
	ASSERTLOG( fd >= 0 );

	/* and some preventative code for the release version */
	if ( fd < 0  )
	{
		elog_broad("Error: ANSI() called with fd < 0");
	}
	else
	{
		if( F_ISSET(Ply[fd].ply, PANSIC ) )
		{
			sprintf(str, "%c[%dm", 27, color);
			output(fd, str);
		}
	}

	return;
}



void gotoxy(int fd, int x, int y )
{
	char	str[25];

	/* a check for debug version */
	ASSERTLOG( fd >= 0 );

	/* and some preventative code for the release version */
	if ( fd < 0  )
	{
		elog_broad("Error: gotoxy() called with fd < 0");
	}
	else
	{
		if(F_ISSET(Ply[fd].ply, PANSIC)) 
		{
			sprintf(str, "%c[%d;%df", 27, x, y);
			output(fd, str);
		}
	}
}

void scroll(int fd, int y1, int y2 )
{
	char	str[25];

	/* a check for debug version */
	ASSERTLOG( fd >= 0 );

	/* and some preventative code for the release version */
	if ( fd < 0  )
	{
		elog_broad("Error: scroll() called with fd < 0");
	}
	else
	{
		if(F_ISSET(Ply[fd].ply, PANSIC)) 
		{
			sprintf(str, "%c[%d;%dr", 27, y1, y2);
			output(fd, str);
		}
	}
}

void delline(int fd )
{
	/* a check for debug version */
	ASSERTLOG( fd >= 0 );

	/* and some preventative code for the release version */
	if ( fd < 0  )
	{
		elog_broad("Error: delline() called with fd < 0");
	}
	else
	{
		if(F_ISSET(Ply[fd].ply, PANSIC)) 
			output(fd, "\033[K");
	}
}

void clrscr(int fd )
{
	/* a check for debug version */
	ASSERTLOG( fd >= 0 );

	/* and some preventative code for the release version */
	if ( fd < 0  )
	{
		elog_broad("Error: clrscr() called with fd < 0");
	}
	else
	{
		if(F_ISSET(Ply[fd].ply, PANSIC)) 
			output(fd, "\033[2J");
	}
}
	 
void save_cursor(int fd )
{
	/* a check for debug version */
	ASSERTLOG( fd >= 0 );

	/* and some preventative code for the release version */
	if ( fd < 0  )
	{
		elog_broad("Error: save_cursor() called with fd < 0");
	}
	else
	{
		if(F_ISSET(Ply[fd].ply, PANSIC)) 
			output(fd, "\0337");
	}
}

void load_cursor( int fd )
{
	/* a check for debug version */
	ASSERTLOG( fd >= 0 );

	/* and some preventative code for the release version */
	if ( fd < 0  )
	{
		elog_broad("Error: load_cursor() called with fd < 0");
	}
	else
	{
		if(F_ISSET(Ply[fd].ply, PANSIC)) 
			output(fd, "\0338");
	}
}
