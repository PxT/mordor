#include "mstruct.h"
#include "mextern.h"

extern FILE *FP;

int write_short(s)
short	s;
{
	fprintf(FP, "%ld\n", (long)s);
}
	
int read_short(s)
short	*s;
{
	long	temp;
	fscanf(FP, "%ld", &temp);
	*s = (short)temp;
}
	
int write_int(l)
int	l;
{
	fprintf(FP, "%d\n", l);
}
	
int read_int(l)
int	*l;
{
	fscanf(FP, "%d", l);
}
	
int write_long(l)
long	l;
{
	fprintf(FP, "%ld\n", l);
}
	
int read_long(l)
long	*l;
{
	fscanf(FP, "%ld", l);
}
	
int write_char(ch)
char	ch;
{
	int	i=0;
	
	i = ch;
	if (i < 0) i += 256;
	fprintf(FP, "%X\n", i);
	
}
	
int read_char(ch)
char	*ch;
{
int	i;
	
	fscanf(FP, "%x", &i);
	*ch = i;
}

int write_chars(str, count)
char	*str;
int	count;
{
	int	cc;
	int	i=0;

	if (count < 1) return(0);

	for(cc=0; cc != count; cc++) {
		i = str[cc];
		if (i < 0) i += 256;
		fprintf(FP, "%X ", i);
	}
	fprintf(FP, "\n");
	return(count);
}
	
int read_chars(str, count)
char	*str;
int	count;
{
	int	cc;
	int	i;

	for (cc=0; cc != count; cc++) {
		fscanf(FP, "%x", &i);
		str[cc] = i; 
	}
}
