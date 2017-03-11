/*
 * SCREEN.C:
 *
 *	 Screen handling routines.
 *
 *
 */
#include "mstruct.h"
#include "mextern.h"

void setcolor(fd, color)
short	fd;
int	color;
{
	if(fd > -1)
		if(F_ISSET(Ply[fd].ply, PANSIC)) 
			print(fd, "%c[%dm", 27, color);
}

void gotoxy(fd, x, y)
short	fd;
int	x;
int	y;
{
	if(fd > -1) 
		if(F_ISSET(Ply[fd].ply, PANSIC)) 
			print(fd, "%c[%d;%df", 27, x, y);
}

void scroll(fd, y1, y2)
short	fd;
int	y1;
int	y2;
{
	if(fd > -1) 
		if(F_ISSET(Ply[fd].ply, PANSIC)) 
			print(fd, "%c[%d;%dr", 27, y1, y2);
}

void delline(fd)
short	fd;
{
	if(fd > -1)
		if(F_ISSET(Ply[fd].ply, PANSIC)) 
			print(fd, "\033[K");
}

void clrscr(fd)
short	fd;
{
	if(fd > -1)
		if(F_ISSET(Ply[fd].ply, PANSIC)) 
			print(fd, "\033[2J");
}
	 
void save_cursor(fd)
short	fd;
{
	if(fd > -1) 
		if(F_ISSET(Ply[fd].ply, PANSIC)) 
			print(fd, "\0337");
}

void load_cursor(fd)
short	fd;
{
	if(fd > -1) 
		if(F_ISSET(Ply[fd].ply, PANSIC)) 
			print(fd, "\0338");
}
