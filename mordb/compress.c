/*
 * COMPRESS.C:
 *
 *	Routines to do LZW compression on the player data.
 *	Compression code was adapted from another program
 *	and modified to perform in-memory compression.
 *
 *	Copyright (C) 1992, 1993 Brooke Paul
 * $Id: compress.c,v 1.3 2001/03/08 16:05:42 develop Exp $
 *
 * $Log: compress.c,v $
 * Revision 1.3  2001/03/08 16:05:42  develop
 * *** empty log message ***
 *
 *
 */



#include <stdio.h>

#include "../include/port.h"
#include "../include/morutil.h"


#define TABSIZE  	4096
#define STACKSIZE	TABSIZE
#define NO_PRED  	0xFFFF
#define EMPTY    	0xFFFF
#define NOT_FND  	0xFFFF
#define UEOF		((unsigned)EOF)
#define UPDATE		TRUE
#define NO_UPDATE	FALSE

static struct tabent {
	char used;
	unsigned int next;
	unsigned int predecessor;
	unsigned char follower;
} string_tab[TABSIZE];

static char		*start_inbuf, *start_outbuf;
static char		*inbuf, *outbuf;
static int		sizeof_inbuf, sizeof_outbuf;
static unsigned int	temp_inbuf;
static unsigned int	temp_outbuf;
static char 		stack[STACKSIZE];
static int		sp;



/* internal prototypes */
unsigned int h(unsigned int prev, unsigned char next);
unsigned int eolist( unsigned int i );
unsigned int hash(unsigned int prev, unsigned char next, int update );
unsigned int unhash(unsigned int prev, unsigned char next ) ;
void init_tab();
void upd_tab(unsigned int prev, unsigned int next);
unsigned int getcode();
void putcode( unsigned int code );
unsigned int readc();
unsigned int writec( int ch );
void flushout();
void push( int c );
int pop(); 






unsigned int h(unsigned int prev, unsigned char next)
{
	long i, j;

	i = (prev + next) | 0x0800;
	j = i * i;
	i = (j >> 6) & 0x0FFF;
	return(i);
}

unsigned int eolist( unsigned int i )
{
	register int t;

	while((t = string_tab[i].next))
		i = t;
	return(i);
}

unsigned int hash(unsigned int prev, unsigned char next, int update )
{
	unsigned int i, temp;
	struct tabent *ep;

	i = h(prev, next);
	if(!string_tab[i].used)
		return(i);
	else {
		i = eolist(i);
  		temp = (i + 101) & 0x0FFF; 
		ep = &string_tab[temp];
		while(ep->used) {
			temp++;
			if(temp == TABSIZE) {
				temp = 0;
				ep = string_tab;
			}
			else
				ep++;
		}
    
		if(update)
			string_tab[i].next = temp;
		return(temp);
	} 
}

unsigned int unhash(unsigned int prev, unsigned char next ) 
{
	unsigned int i;
	struct tabent *ep;

	i = h(prev, next);
    loop:
	ep = &string_tab[i];
	if((ep->predecessor == prev) && (ep->follower == next)) 
		return(i);
	if(!ep->next)
		return(NOT_FND);
	i = ep->next;
	goto loop;
}

void init_tab()
{
	int i;

	temp_outbuf = temp_inbuf = EMPTY;
	sp = 0;
	memset((char *)string_tab, 0, sizeof(string_tab));
	for(i=0; i<256; i++)
		upd_tab(NO_PRED,i);
}

void upd_tab(unsigned int prev, unsigned int next)
{
	struct tabent *ep;

	ep = &string_tab[hash(prev, (unsigned char)next, UPDATE)];
	ep->used = TRUE;
	ep->next = 0;
	ep->predecessor = prev;
	ep->follower = next;
}

unsigned int getcode()
{
	unsigned int localbuf, returnval;

	if(temp_inbuf == EMPTY) {
		if((localbuf = readc()) == UEOF)
			return(UEOF);
		localbuf &= 0xFF;

		if((temp_inbuf = readc()) == UEOF)
			return(UEOF);
		temp_inbuf &= 0xFF;

		returnval = ((localbuf << 4) & 0xFF0) | 
			    ((temp_inbuf >> 4) & 0x00F);
		temp_inbuf &= 0x000F;
	}

	else {
		if((localbuf = readc()) == UEOF)
			return(UEOF);
		localbuf &= 0xFF;

		returnval = localbuf | ((temp_inbuf << 8) & 0xF00);
		temp_inbuf = EMPTY;
  	}

	return(returnval);
}

void putcode( unsigned int code )
{

	if(temp_outbuf == EMPTY) {
		writec((code >> 4) & 0xFF);
		temp_outbuf = code & 0x000F;
	}

	else {
		writec(((temp_outbuf << 4) & 0xFF0) | ((code >> 8) & 0x00F));
		writec(code & 0x00FF);
		temp_outbuf = EMPTY;
	}
}

unsigned int readc()
{
	if(inbuf - start_inbuf >= sizeof_inbuf)
		return(UEOF);
	else
		return((*(inbuf++) & 0xFF));
}

unsigned int writec( int ch )
{
	*outbuf = (char)ch;
	outbuf++;
	sizeof_outbuf++;
	return(0);
}

void flushout()
{
	if(temp_outbuf != EMPTY) {
		*outbuf = (temp_outbuf << 4) & 0xFF0;
		outbuf++;
		sizeof_outbuf++;
	}
}

int compress(char *in, char *out, int size)
{
	unsigned int 	c, code, localcode;
	int 		code_count = TABSIZE - 256;

	ASSERTLOG( in );
	ASSERTLOG( out );

	start_inbuf  = inbuf  = in;
	start_outbuf = outbuf = out;
	sizeof_inbuf = size;
	sizeof_outbuf = 0;

	init_tab();
	c = readc();
	code = unhash(NO_PRED, (char)c);

	while((c = readc()) != UEOF) {
		if((localcode = unhash(code, (char)c)) != NOT_FND) {
			code = localcode;
			continue;
		}

		putcode(code);
		if(code_count) {
			upd_tab(code, c);
			code_count--;
		}

		code = unhash(NO_PRED, (char)c);
	}

	putcode(code);
	flushout();

	return(sizeof_outbuf);
}


int uncompress(char	*in, char *out, int	size )
{
	unsigned int	c, tempc, code, oldcode, incode, finchar, lastchar;
	char		unknown = FALSE;
	int		code_count = TABSIZE - 256;
	struct tabent	*ep;

	ASSERTLOG( in );
	ASSERTLOG( out );

	start_inbuf  = inbuf  = in;
	start_outbuf = outbuf = out;
	sizeof_inbuf = size;
	sizeof_outbuf = 0;

	init_tab();
	code = oldcode = getcode();

	c = string_tab[code].follower;
	writec(c);
	finchar = c;

	while((code = incode = getcode()) != UEOF) {
		ep = &string_tab[code];
		if(!ep->used) {
			lastchar = finchar;
			code = oldcode;
			unknown = TRUE;
			ep = &string_tab[code];
		}

		while(ep->predecessor != NO_PRED) {
			push(ep->follower);
			code = ep->predecessor;
			ep = &string_tab[code];
		}

		finchar = ep->follower;
		writec(finchar);
		while((tempc = pop()) != EMPTY)
			writec(tempc);

		if(unknown) {
			writec(finchar = lastchar);
			unknown = FALSE;
		}
		if(code_count) {
			upd_tab(oldcode,finchar);
			code_count--;
		}
		oldcode = incode;
	}
	flushout();

	return(sizeof_outbuf);
}

void push( int c )
{
	stack[sp] = (char) c;
	sp++;
	if(sp >= STACKSIZE)
		merror("Stack overflow", FATAL );
}

int pop() 
{
	if(sp > 0) {
		sp--;
		return((int)stack[sp]);
	}
	else
		return(EMPTY);
}

