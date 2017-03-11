/*
	Convert Mordor DBs to ASCII and back again.
	(C) 1994, 1999 Brooke Paul
 $Id: ascbin.c,v 1.1 2001/04/01 17:54:35 develop Exp $

 $Log: ascbin.c,v $
 Revision 1.1  2001/04/01 17:54:35  develop
 Initial revision


*/
#include <stdio.h>
#include <stdlib.h>

#include "../include/mordb.h"

#include "ascbin.h"


void write_short(FILE *fp, short s)
{
	fprintf(fp, "%ld\n", (long)s);
}
	
void read_short(FILE *fp, short *s)
{
	long	temp;
	fscanf(fp, "%ld", &temp);
	*s = (short)temp;
}
	
void write_int(FILE *fp, int l)
{
	fprintf(fp, "%d\n", l);
}
	
void read_int(FILE *fp, int *l)
{
	fscanf(fp, "%d", l);
}
	
void write_long(FILE *fp, long l)
{
	fprintf(fp, "%ld\n", l);
}
	
void read_long(FILE *fp, long *l)
{
	fscanf(fp, "%ld", l);
}
	
void write_char(FILE *fp, char ch)
{
	int	i=0;
	
	i = ch;
	if (i < 0) i += 256;
	fprintf(fp, "%X\n", i);
	
}
	
void read_char(FILE *fp, char *ch)
{
	int	i;
	
	fscanf(fp, "%x", &i);
	*ch = i;
}

int write_chars(FILE *fp, char *str, int count)
{
	int	cc;
	int	i=0;

	if (count < 1) return(0);

	for(cc=0; cc != count; cc++) {
		i = str[cc];
		if (i < 0) i += 256;
		fprintf(fp, "%X ", i);
	}
	fprintf(fp, "\n");
	return(count);
}
	
void read_chars(FILE *fp, char *str, int count)
{
	int	cc;
	int	i;

	for (cc=0; cc != count; cc++) {
		fscanf(fp, "%x", &i);
		str[cc] = i; 
	}
}



void do_error( char *str )
{
	perror(str );
	perror( "/n");
	exit(1);
}
