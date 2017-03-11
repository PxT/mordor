/*
 * MISC.C:
 *
 *	Miscellaneous string, file and data structure manipulation
 *	routines.
 *
 *	Copyright (C) 1991, 1992, 1993 Brooke Paul
 *
 * $Id: misc.c,v 6.17 2001/07/06 17:31:54 develop Exp $
 *
 * $Log: misc.c,v $
 * Revision 6.17  2001/07/06 17:31:54  develop
 * changed error messsage in dice when zero is passed to it
 *
 * Revision 6.16  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

#ifdef IRIX
	#define _BSD_COMPAT
	#include <unistd.h>
	#include <sys/stat.h>
   	#include <fcntl.h>
#endif /* IRIX */

#include "../include/mordb.h"
#include "mextern.h"
#include <stdio.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>



/* Temporary (but static) data for the next several functions */

static char	xstr[5][80];
static int	xnum=0;
	


/************************************************************************/
/*				get_ply_flags()				*/
/************************************************************************/
int get_ply_flags(creature * ply_ptr )
{
	int	flags;

	ASSERTLOG( ply_ptr );

	flags = 0;

	if ( ply_ptr && ply_ptr->type == PLAYER)
	{
		if(F_ISSET(ply_ptr, PDINVI))
			flags |= INV;
		if(F_ISSET(ply_ptr, PDMAGI))
			flags |= MAG;
		if ( ply_ptr->class >= IMMORTAL )
			flags |= ISIM;
		if ( ply_ptr->class >= CARETAKER )
			flags |= ISCT;
		if ( ply_ptr->class == DM )
			flags |= ISDM;
	}


	return( flags );
}



/************************************************************************/
/*				crt_str					*/
/************************************************************************/

/* This function takes the creature given it in the first parameter, 	*/
/* and forms the appropriate singularized or pluralized version of the  */
/* creature's name using certain flags.					*/

char *crt_str(creature *viewpoint, creature	*crt, int num, int flags )
{
	char	ch;
	char	*str;
	char    pform[80];                                    
	char    sform[80];                                    
	char    pfile[80];                                    
	int     found;                                        
	FILE    *plural;     

	ASSERTLOG( viewpoint );
	ASSERTLOG( crt );

	if ( !crt )
		return("");

	str = xstr[xnum];  xnum = (xnum + 1)%5;

	/* get the flags relative to the person seeing this creature */
	flags |= get_ply_flags( viewpoint );

	if(crt->type != MONSTER) 
	{
		if(F_ISSET(crt, PALIAS) && F_ISSET(crt, PDMINV)) 
		{
			if(!F_ISSET(Ply[crt->fd].extr->alias_crt, MNOPRE)) 
			{ 
				strcpy(str, "A ");   
				strcat(str, Ply[crt->fd].extr->alias_crt->name);
			}
			else
				strcpy(str, Ply[crt->fd].extr->alias_crt->name);
		}
		else if ( ( F_ISSET(crt, PDMINV) && !(flags & ISIM ))
			|| (F_ISSET(crt, PINVIS) && !(flags & INV) ))
		{
			strcpy(str, "Someone");
		}
		else
		{
			strcpy(str, crt->name);
			if( F_ISSET(crt, PDMINV ) )
			{
				strcat( str, " (+)");
			}
			else if(F_ISSET(crt, PINVIS))
				strcat(str, " (*)");
			else if (F_ISSET(crt, PDMOWN))
				strcat(str, " (O)");
		}
		return(str);
	}


	/* else we got monsters */

	if(num == 0) {
		if(!F_ISSET(crt, MNOPRE)) {
			strcpy(str, "the ");
			strcat(str, crt->name);
		}
		else
			strcpy(str, crt->name);
	}

	else if(num == 1) {
		if(F_ISSET(crt, MNOPRE))
			strcpy(str, "");
		else {
			ch = low(crt->name[0]);
			if(ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' ||
			   ch == 'u')
				strcpy(str, "an ");
			else
				strcpy(str, "a ");
		}
		strcat(str, crt->name);
	}
	else {
		strcpy( str, int_to_text( num ));
		strcat(str, " ");
		strcat(str, crt->name);
		if(F_ISSET(crt, MTOMEN)) {
			str[strlen(str)-2] = 'e';
		}
		else if(!F_ISSET(crt, MDROPS)) {
			str[strlen(str)+1] = 0;
			str[strlen(str)+2] = 0;
			if(str[strlen(str)-1] == 's' || 
			   str[strlen(str)-1] == 'x') {
				str[strlen(str)] = 'e';
				str[strlen(str)] = 's';
			}
			else
				str[strlen(str)] = 's';
		}
	}
    if(F_ISSET(crt, MIREGP) && num > 1)
    {
        found = 0;
                    
        strcpy(pfile, get_monster_path());
        strcat(pfile,"/plurals");

         
        plural = fopen(pfile, "r");
        if(plural != NULL)
        {
            while (!found && !(feof (plural)))
            {
                fflush(plural);
				/* get singular form */
                fgets(sform, sizeof(sform), plural);    
                sform[strlen(sform)-1] = 0;
                fflush(plural);

				/* get plural form */
                fgets(pform, sizeof(pform), plural);    
                pform[strlen(pform)-1] = 0;

                if(strcmp(crt->name, sform) == 0)
                {   
                    strcpy(str, "");
					/* this if was extra dont kow why it was here */
                    if(num < 21)

						strcpy( str, int_to_text(num) );
						strcat( str, " " );
                        strcat(str, pform);
                    found = 1; 
                 }
             }
             fclose(plural);
        }
    }

	if(flags & CAP)
		str[0] = up(str[0]);
	if((flags & MAG) && (crt->type != PLAYER) && (F_ISSET(crt, MMAGIC)))
		strcat(str, " (M)");

	return(str);

}

/************************************************************************/
/*				obj_str					*/
/************************************************************************/

/* This function examines an object's flags and its name, and returns	*/
/* the appropriate pluralized or singularized version of the name.	*/
/* In some cases it is necessary to drop the s on a plural version of	*/
/* a word.								*/

char *obj_str(creature *viewpoint, object *obj, int num, int flags )
{
	char 	ch;
	char	str2[10];
	char	*str;
	char    pform[80];
	char    sform[80];
	char    pfile[80];
	int     found;
	FILE    *plural;

	ASSERTLOG( obj );

	if ( !obj )
		return("");

	str = xstr[xnum];  xnum = (xnum + 1)%5;

	/* get the flags relative to the person seeing this creature */
	flags |= get_ply_flags( viewpoint );

	if(num == 0) {
		if(!F_ISSET(obj, ONOPRE)) {
			strcpy(str, "the ");
			strcat(str, obj->name);
		}
		else
			strcpy(str, obj->name);
	}
		
	else if(num == 1) {
		if(F_ISSET(obj, ONOPRE) || obj->type == MONEY)
			strcpy(str, "");
		else if(F_ISSET(obj, OSOMEA))
			strcpy(str, "some ");
		else {
			ch = low(obj->name[0]);
			if(ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' ||
			   ch == 'u')
				strcpy(str, "an ");
			else
				strcpy(str, "a ");
		}
		strcat(str, obj->name);
	}

	else {
		strcpy( str, int_to_text(num) );
		strcat(str, " ");

		if(F_ISSET(obj, OSOMEA))
			strcat(str, "sets of ");

		strcat(str, obj->name);
		if(!F_ISSET(obj, ODROPS) && !F_ISSET(obj, OSOMEA)) {
			str[strlen(str)+1] = 0;
			str[strlen(str)+2] = 0;
			if(str[strlen(str)-1] == 's' || 
			   str[strlen(str)-1] == 'x') {
				str[strlen(str)] = 'e';
				str[strlen(str)] = 's';
			}
			else
				str[strlen(str)] = 's';
		}
	}
       if(F_ISSET(obj, OIREGP) && num > 1)
        {
                found = 0;
                            
                strcpy(pfile, get_object_path());
                strcat(pfile,"/plurals");

 
                plural = fopen(pfile, "r");
                if(plural != NULL)
                {
                    while (!found && !(feof (plural)))
                    {
                        fflush(plural);
                        /* get singular form */
                        fgets(sform, sizeof(sform), plural);
                        sform[strlen(sform)-1] = 0;
                        fflush(plural);
                        /* get plural form */
                        fgets(pform, sizeof(pform), plural);
                        pform[strlen(pform)-1] = 0;
                                
                        if(strcmp(obj->name, sform) == 0)
                        {
                            strcpy(str, int_to_text(num) );
							strcat(str, " " );
                            strcat(str, pform);
                            found = 1;
                         }
                     }
                     fclose(plural);
                }
        }

	if(flags & CAP)
		str[0] = up(str[0]);
	if((flags & MAG) && obj->adjustment ) {
		sprintf(str2, " (%s%d)", obj->adjustment >= 0 ? "+":"",
			obj->adjustment);
		strcat(str, str2);
	}
	else if((flags & MAG) && obj->magicpower)
		strcat(str, " (M)");

	/* CT or higher see this */
	if ( flags & ISCT )
	{
		if ( F_ISSET(obj, OHIDDN) )
		{
			strcat( str, " (h)" );
		}
		if ( F_ISSET(obj, OINVIS) )
		{
			strcat( str, " (*)" );
		}
		if ( F_ISSET(obj, OSCENE) )
		{
			strcat( str, " (s)" );
		}
	}

	return(str);
}

/************************************************************************/
/*				delimit					*/
/************************************************************************/

/* This function takes a given string, and if it is greater than a given*/
/* number of characters, then it is split up into several lines.  This  */
/* is done by replacing spaces with carriage returns before the end of  */
/* the line.								*/

#define MAXLINE	77

void delimit( char *str )
{
	int 	i, j, l, len, lastspace;
	char 	str2[4096];

	str2[0] = 0;
	j = (str) ? strlen(str) : 0;
	if(j < MAXLINE)
		return;

	len = 0; lastspace = -1; l = 0;
	for(i=0; i<j; i++) {
		if(str[i] == ' ')
			lastspace = i;
		if(str[i] == '\n') {
			len = 0;
			lastspace = -1;
		}
		len++;
		if(len > MAXLINE && lastspace > -1) {
			str[lastspace] = 0;
			strcat(str2, &str[l]);
			strcat(str2, "\n  ");
			l = lastspace + 1;
			len = i - lastspace + 3;
			lastspace = -1;
		}
	}
	strcat(str2, &str[l]);
	strcpy(str, str2);
}

/************************************************************************/
/*				view_file				*/
/************************************************************************/

/* This function views a file whose name is given by the third 		*/
/* parameter. If the file is longer than 20 lines, then the user is 	*/
/* prompted to hit return to continue, thus dividing the output into 	*/
/* several pages.							*/

#define FBUF	800

void view_file(int fd, int param, char *str )
{
	char	buf[FBUF+1];
	int	i, l, n, ff, line;
	long	offset;

	buf[FBUF] = 0;
	switch(param) {
	case 1:
		offset = 0L;
		strcpy(Ply[fd].extr->tempstr[1], str);
		ff = open(str, O_RDONLY | O_BINARY);
		if(ff < 0) {
			output(fd, "File could not be opened.\n");
			RETURN(fd, command, 1);
		}
		line = 0;
		while(1) {
			n = read(ff, buf, FBUF);
			l = 0;
			for(i=0; i<n; i++) {
				if(buf[i] == '\n') {
					buf[i] = 0;
					line++;
					output_nl(fd, &buf[l]);
					offset += (i-l+1);
					l = i+1;
				}
				if(line > 20) 
					break;
			}
			if(line > 20) {
				sprintf(Ply[fd].extr->tempstr[0], "%lu", 
					offset);
				break;
			}
			else if(l != n) {
				output(fd, &buf[l]);
				offset += (i-l);
			}
			if(n<FBUF) 
				break;
		}
		if(n==FBUF || line>20) {
			F_SET(Ply[fd].ply, PREADI);
			output(fd, "[Hit Return, Q to Quit]: ");
			output_ply_buf(fd);
	        Ply[fd].io->intrpt &= ~1; 
		}
		if(n<FBUF && line <= 20) {
			close(ff);
			RETURN(fd, command, 1);
		}
		else {
			close(ff);
			F_SET(Ply[fd].ply, PREADI);
			RETURN(fd, view_file, 2);
		}
	case 2:
		if(str[0] != 0) {
			output(fd, "Aborted.\n");
			F_CLR(Ply[fd].ply, PREADI);
			RETURN(fd, command, 1);
		}
		offset = atol(Ply[fd].extr->tempstr[0]);
		ff = open(Ply[fd].extr->tempstr[1], O_RDONLY | O_BINARY);
		if(ff < 0) {
			output(fd, "File could not be opened.\n");
			F_CLR(Ply[fd].ply, PREADI);
			RETURN(fd, command, 1);
		}
		lseek(ff, offset, 0);
		line = 0;
		while(1) {
			n = read(ff, buf, FBUF);
			l = 0;
			for(i=0; i<n; i++) {
				if(buf[i] == '\n') {
					buf[i] = 0;
					line++;
					output_nl(fd, &buf[l]);
					offset += (i-l+1);
					l = i+1;
				}
				if(line > 20) break;
			}
			if(line > 20) {
				sprintf(Ply[fd].extr->tempstr[0], "%lu", 
					offset);
				break;
			}
			else if(l != n) {
				output(fd, &buf[l]);
				offset += (i-l);
			}
			if(n<FBUF) break;
		}
		if(n==FBUF || line > 20) {
			F_SET(Ply[fd].ply, PREADI);
			output(fd, "[Hit Return, Q to Quit]: ");
			output_ply_buf(fd);
	        Ply[fd].io->intrpt &= ~1; 
		}
		if(n<FBUF && line <= 20) {
			close(ff);
			F_CLR(Ply[fd].ply, PREADI);
			RETURN(fd, command, 1);
		}
		else {
			close(ff);
			RETURN(fd, view_file, 2);
		}
	}
}

/************************************************************************/
/*				dice					*/
/************************************************************************/

/* This function rolls n s-sided dice and adds p to the total.		*/

int dice(int n, int s, int p)
{
	int	i;
	
	if(!n) {
		merror("Zero number of dice", NONFATAL);
		return(0);
	}

	if(!s) {
		merror("Zero sides of dice", NONFATAL);
		return(0);
	}

	for(i=0; i<n; i++)
		p += mrand(1,s);

	return(p);
}

/************************************************************************/
/*				exp_to_lev				*/
/************************************************************************/

/* This function takes a given amount of experience as its first 	*/
/* argument returns the level that the experience reflects.		*/ 

int exp_to_lev(long	exp )
{
	int level = 1;
	
	while (exp >= needed_exp[level-1] && level < MAXALVL)
		level++;
	if (level == MAXALVL) {
		level = exp/(needed_exp[MAXALVL-1]);
		level++;
		level= MAX(25,level);
	}

	return(MAX(1,level));
}

/************************************************************************/
/*				dec_daily				*/
/************************************************************************/

/* This function is called whenever a daily-use item or operation is	*/
/* used or performed.  If the number of daily uses are used, up then	*/
/* a 0 is returned.  Otherwise, the number of uses is decremented and	*/
/* a 1 is returned.							*/

int dec_daily( struct daily *dly_ptr )
{
	time_t		t;
	struct tm	*tm, time1, time2;

	t = time(0);
	tm = localtime(&t);

	time1 = *tm;

	tm = localtime(&dly_ptr->ltime);

	time2 = *tm;

	if(time1.tm_yday != time2.tm_yday) {
		dly_ptr->cur = dly_ptr->max;
		dly_ptr->ltime = t;
	}

	if(dly_ptr->cur == 0)
		return(0);

	dly_ptr->cur--;

	return(1);
}



/************************************************************************/
/*				sort_cmds				*/
/************************************************************************/

/* This function sorts the global cmdlist structure array so that the	*/
/* command list is sorted by how often each command is used.  The most-	*/
/* used commands will end up at the top of the list.			*/

void sort_cmds()
{
	Cmdnum = 0;

	while(cmdlist[Cmdnum].cmdno != 0) Cmdnum++;

	qsort((void *)cmdlist, Cmdnum, sizeof(struct cmdstruct), (PFNCOMPARE)sort_cmp);

}

/************************************************************************/
/*				sort_cmp				*/
/************************************************************************/

/* This function is used by the quicksort routine to sort the command	*/
/* list according to how often each command has been used.		*/
int sort_cmp( struct cmdstruct *first, struct cmdstruct *second )
{
	return(strcmp(first->cmdstr, second->cmdstr));
}


/************************************************************************/
/*				load_lockouts				*/
/************************************************************************/

/* This function opens the lockout file and reads in all locked out 	*/
/* sites.								*/

void load_lockouts()
{
	FILE	*fp;
	char	str[80];
	int	i;

	if(Lockout) free(Lockout);
	Numlockedout = 0;

	sprintf(str, "%s/lockout", get_log_path());
	fp = fopen(str, "r");
	if(!fp) return;

	while(1) {
		if(fscanf(fp, "%s", str) == EOF) break; 
		if(fscanf(fp, "%s", str) == EOF) break;
		if(fscanf(fp, "%s", str) == EOF) break;
		Numlockedout++;
	}

	if(!Numlockedout) {
		fclose(fp);
		return;
	}

	Lockout = (lockout *)malloc(Numlockedout * sizeof(lockout));
	fseek(fp, 0L, 0);

	for(i=0; i<Numlockedout; i++) {
		fscanf(fp, "%s", Lockout[i].userid); 
		fscanf(fp, "%s", Lockout[i].address);
		fscanf(fp, "%s", Lockout[i].password);
		if(Lockout[i].password[0] == '-' && !Lockout[i].password[1])
			Lockout[i].password[0] = 0;
	}

	fclose(fp);
}

/************************************************************************/
/*				please_wait				*/
/************************************************************************/

void please_wait(int fd, time_t duration )
{
	if(duration == 1)
		output(fd, "Please wait 1 more second.\n");
	else
	{
		if(duration > 60) 
		{
			sprintf(g_buffer, "Please wait %ld:%02ld minutes.\n", (long)duration/60L, (long)duration%60L);
		}
		else
		{
			sprintf(g_buffer,"Please wait %ld seconds.\n", (long)duration);
		}

		output(fd, g_buffer );
	}

}
 
/*====================================================================*/
/* checks if the given str contains all digits */
int is_num(char *str )
{
  int len, i;
   len = strlen(str);
   for (i=0;i < len; i++)
      if(!isdigit((int)str[i]))
         return (0);
   return (1);
}



void **m1arg(void *arg1)
{
	static	void *argv[2];

	argv[0] = arg1;
	argv[1] = NULL;

	return(argv);
}


void **m2args(void *arg1, void *arg2)
{
	static	void *argv[3];

	argv[0] = arg1;
	argv[1] = arg2;
	argv[2] = NULL;

	return(argv);
}

void **m3args(void *arg1, void *arg2, void *arg3)
{
	static	void *argv[4];

	argv[0] = arg1;
	argv[1] = arg2;
	argv[2] = arg3;
	argv[3] = NULL;

	return(argv);
}


/************************************************************************/
/* clean_str()								*/
/*	called to check for modem escape codes and other things that	*/
/*	should not be sent with send or broadcast etc.			*/
/*									*/
/*		JPF March 98						*/
/************************************************************************/
void clean_str(char *str, int strip_count )
{
	char	str_buf[2048];
	char	*pnew;
	char	*porg;
	int		nPlusCount;
	int		ndx;

	pnew = str_buf;
	porg = str;
	nPlusCount = 0;


	/* strip strip_count words from the beginning */
	for ( ndx = 0; ndx < strip_count; ndx++ )
	{
		/* strip leading space */
		while ( *porg != '\0' && *porg == ' ')
			porg++;

		/* skip word */
		while ( *porg != '\0' && *porg != ' ')
			porg++;
	}

	/* strip spaces after last stripped word */
	while ( *porg != '\0' && *porg == ' ')
		porg++;

	/* copy the rest of the string in to the clean buffer */
	/* removing offensive chars */
	while ( *porg != '\0' )
	{
		switch( *porg )
		{
			case '+':
				nPlusCount++;
				if ( nPlusCount < 3 )
				{
					*(pnew++) = *porg;
				}
				break;
			default:
				nPlusCount = 0;
				*(pnew++) = *porg;
				break;
		}
		porg++;
	}

	*pnew = '\0';

	/* now copy it back into the original buffer */
	strcpy(str, str_buf );

	return;
}


/************************************************************************/
/* convert_seconds()							*/
/*	Simple routine to converts seconds to days, hours, minutes and	*/
/*	seconds								*/
/*									*/
/*	returns 0 on error, 1 on success				*/
/*		JPF May 98						*/
/************************************************************************/
int convert_seconds_to_str(long interval, char *str )
{
	int	ret_code = 0 ;
	int	days, hours, minutes, seconds;

	ASSERTLOG( str );
	ASSERTLOG( interval > 0 );

	if ( str != NULL && interval > 0 )
	{
		days = interval/86400L;
		hours = interval/3600L;
		hours %= 24;
		minutes = interval/60L;
		minutes %= 60;
		seconds = interval % 60;

		if(!days)
			sprintf(str, "%02d:%02d:%02d", hours, minutes, seconds); 
		else if (days==1)
			sprintf(str, "%d day %02d:%02d:%02d", days, hours, minutes, seconds); 
		else
			sprintf(str,"%d days %02d:%02d:%02d", days, hours, minutes, seconds); 

		ret_code = 1;
	}


	return( ret_code );
}

/************************************************************************/
/* in_group()                                                           */
/*              Simple routine to test if two players are grouped       */
/*              PxT March 2000						*/
/************************************************************************/
int in_group(creature *crt_ptr, creature *ply_ptr)
{
        ctag    *cp;
                
        if(ply_ptr->following == 0 && ply_ptr->first_fol == 0)
                        return 0;
        if(crt_ptr->following == 0 && crt_ptr->first_fol == 0)
                        return 0;
        if(ply_ptr == crt_ptr)   
                        return 1;
        if(crt_ptr->following == ply_ptr)
                        return 1;
        if(ply_ptr->following == crt_ptr)
                        return 1;
                        
        cp = crt_ptr->first_fol;
        while(cp) {
                if(cp->crt == ply_ptr)
                        return 1;
                cp = cp->next_tag;
        }
        /* if a is following b, and c is following b, then a and c are grouped */
        /* however if a is following b and b is following c, then a and c are not grouped */
        /* This should be considered a feature not a bug... */
        if(crt_ptr->following != 0) {
                cp = crt_ptr->following->first_fol;
                while(cp) {
                        if(cp->crt == ply_ptr)
                                return 1;
                        cp = cp->next_tag;
                }
        }
        return 0;
}       /* end of in_group */


/************************************************************************/
/* void view_file_reverse(int fd, int param, char *str)			*/
/* 									*/
/* displays a file, line by line starting with the last			*/
/* similar to unix 'tac' command					*/
/* 									*/
/* 									*/
/************************************************************************/

/* The maximum size (in bytes) of a screenful of data 	*/
/* 80 characters+newline * 20 lines +1 for luck 	*/
#define TACBUF ( (81 * 20 * sizeof(char)) + 1 )

void view_file_reverse(int fd, int param, char *str)
{

off_t oldpos;
off_t newpos;
off_t temppos;
int i,more_file=1,count,amount=1621;
char string[1622];
char search[80];
long offset;

FILE *ff;


	if(strlen(Ply[fd].extr->tempstr[3]) > 0)
		strcpy(search, Ply[fd].extr->tempstr[3]);
	else
		strcpy(search, "\0");
	
    switch(param) {
	case 1:

	strcpy(Ply[fd].extr->tempstr[1], str);
	if ((ff = fopen(str, "r")) == NULL){
		output(fd, "error opening file\n");
		RETURN(fd, command, 1);
	}



	fseek(ff, 0L, SEEK_END);
	oldpos = ftell(ff);
	if(oldpos < 1) {
		output(fd, "Error opening file\n");
		RETURN(fd, command, 1);
	}
	break;

	case 2:

		if(str[0] != 0) {
			output(fd, "Aborted.\n");
			F_CLR(Ply[fd].ply, PREADI);
			RETURN(fd, command, 1);
		}

	if ((ff = fopen(Ply[fd].extr->tempstr[1], "r")) == NULL){
		output(fd, "error opening file\n");
		F_CLR(Ply[fd].ply, PREADI);
		RETURN(fd, command, 1);
	}

	offset = atol(Ply[fd].extr->tempstr[0]);
	fseek(ff, offset, SEEK_SET);
	oldpos = ftell(ff);
	if(oldpos < 1) {
		output(fd, "Error opening file\n");
		RETURN(fd, command, 1);
	}

    } /* end switch */

nomatch:
	temppos = oldpos - TACBUF; 
	if(temppos > 0)
		fseek(ff, temppos, SEEK_SET);
	else {
		fseek(ff, 0L, SEEK_SET);
		amount = oldpos;
	}

	newpos = ftell(ff);


	fread(string, amount,1, ff);
	string[amount] = '\0';
	i = strlen(string);
	i--;

	count = 0;
	while(count < 21 && i > 0) {
		if(string[i] == '\n') {
			if((strlen(search) > 0 && strstr(&string[i], search))
				|| search[0] == '\0') {
				sprintf(g_buffer, "%s", &string[i]);
				output(fd, g_buffer);
				count++;
			}
			string[i]='\0';
		}
		i--;
	}

	oldpos = newpos + i + 2;
	if(oldpos < 3)
		more_file =  0;

	sprintf(Ply[fd].extr->tempstr[0], "%lu", (long) oldpos);
		
	
	if(more_file && count == 0) 
		goto nomatch;		/* didnt find a match within a screenful */
	else if(more_file) {
                output(fd, "\n[Hit Return, Q to Quit]: ");
                output_ply_buf(fd);
                Ply[fd].io->intrpt &= ~1; 

		fclose(ff);
		F_SET(Ply[fd].ply, PREADI);
		RETURN(fd, view_file_reverse, 2);
	} else {
		if((strlen(search) > 0 && strstr(string, search))
				 || search[0] == '\0') {
			sprintf(g_buffer, "\n%s\n", string);
			output(fd, g_buffer);
		}
		fclose(ff);
		F_CLR(Ply[fd].ply, PREADI);
		RETURN(fd, command, 1);
	}

}

