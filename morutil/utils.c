/***************************************************************************
*  Mordor Database Utilities:
*	These functions allow for read/write access to Mordor MUD
*  database files.
*
*	(c) 1996 Brooke Paul
* $Id: utils.c,v 1.3 2001/07/22 22:38:32 develop Exp $
*
* $Log: utils.c,v $
* Revision 1.3  2001/07/22 22:38:32  develop
* added LONG_MAX
*
* Revision 1.3  2001/07/22 20:27:07  develop
* added time.h include
*
* Revision 1.2  2001/03/08 16:05:27  develop
* *** empty log message ***
*
*
*	void	merror(), zero(), posit(), spin(), message();
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "morutil.h"



/************************************************************************/
/*				new_merror				*/
/************************************************************************/

/* merror is called whenever an error message should be output to the	*/
/* log file.  If the error is fatal, then the program is aborted	*/

void new_merror(char *str, char errtype, char *file, const int line )
{
	long t;
	char bugstr[512];

	ASSERTLOG( file != NULL);

	t = time(0);
	sprintf(bugstr, "Error occured in %s in file %s line %d", str, 
		file, line);
	elog(bugstr);
	if(errtype == FATAL)
		exit(-1);
}

/************************************************************************/
/*				lowercize				*/
/************************************************************************/

/* This function takes the string passed in as the first parameter and	*/
/* converts it to lowercase.  If the flag in the second parameter is	*/
/* TRUE, then the first letter is capitalized.		*/

void lowercize(char	*str, int flag )
{
	unsigned int 	i;

	for(i=0; i< strlen(str); i++)
	{
		if ( i == 0 && flag )
		{
			str[0] = up(str[0]);
		}
		else
		{
			str[i] = low(str[i]);
		}
	}

	return;
}


/************************************************************************/
/*				zero					*/
/************************************************************************/

/* This function zeroes a block of bytes at the given pointer and the */
/* given length.						      */

void zero( void	*ptr, int size )
{
	char 	*chptr;
	int	i;

    chptr = (char *)ptr;
	for(i=0; i<size; i++) 
		chptr[i] = 0;
}



void posit(int x, int y)
{
        printf("%c[%d;%df", 27, x, y);
}

void clearscreen()
{
        printf("%c[2J",27);
}


void savepos()
{
	printf("%c[s",27);
}

void restorepos()
{
	printf("%c[u",27);
}

void spin(int x)
{
	switch(x) {    
		case 1:
			posit(8,10); printf("| \n");
            break;
        case 2:
			posit(8,10); printf("/ \n");
            break;
		case 3:
			posit(8,10); printf("\\ \n");
			break;
		default:
			posit(8,10); printf("- \n");
			break;
	}
}

void message(char *str)

{
	static int count=0;
    int len, loc;


	if(strlen(str)<79) 
	{
		len=79-strlen(str);
		while(len) {
			strcat(str, " ");
			len -= 1;
		}
	}
	count++;
	if(count>10)
		count=-1;
	loc = count+13;
	posit(loc, 10);
	printf("%s", str);
}



/************************************************************************/
/*				file_exists				*/
/************************************************************************/

/* This function returns 1 if the filename specified by the first par-	*/
/* ameter exists, 0 if it doesn't.					*/

int file_exists( char *filename )
{
	int ff;

	ff = open(filename, O_RDONLY | O_BINARY);
	if(ff > -1) {
		close(ff);
		return(1);
	}
	else
		return(0);
}



/****************************************************************/
/* convert_to_bool()											*/
/* JPF															*
/****************************************************************/
int convert_to_bool(char *buf)
{
	int	nReturn = 0;

	lowercize(buf, 0);

	if( strcmp(buf,"yes") == 0 || strcmp(buf, "1") == 0)
		nReturn = 1;

	return(nReturn );
}


/****************************************************************/
/* convert_to_color()											*/
/* JPF															*/
/****************************************************************/
int convert_to_color(char *buf)
{

	/* default to WHITE */
	int	color = AFC_WHITE;

	lowercize(buf, 0);

    if(!strcmp(buf, "black")) 
		color=AFC_BLACK;
    else if(!strcmp(buf, "red")) 
		color=AFC_RED;
    else if(!strcmp(buf, "green")) 
		color=AFC_GREEN;
    else if(!strcmp(buf, "yellow")) 
		color=AFC_YELLOW;
    else if(!strcmp(buf, "blue")) 
		color=AFC_BLUE;
    else if(!strcmp(buf, "magenta")) 
		color=AFC_MAGENTA;
    else if(!strcmp(buf, "cyan")) 
		color=AFC_CYAN;
    else if(!strcmp(buf, "white")) 
		color=AFC_WHITE;


	return( color );
}


/****************************************************************/
/* get_time_str()												*/
/*		gets the result of ctime but removes the trailing		*/
/*		newline													*/
/* Returns: the a static pointer to time string					*/
/****************************************************************/
char *get_time_str()
{
	time_t	t;
	static char timestr[40];

	t = time(0);
	strcpy(timestr, ctime(&t));

	/* strip the newline at the end */
	timestr[strlen(timestr)-1] = 0;

	return(timestr);
}
