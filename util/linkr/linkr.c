/*	LINKR.C
 *
 *      Links rooms together
 *
 *      Copyright (C) 1991 Steven D. Wallace
 *
 */

#include "mstruct.h"
#include "mextern.h"
#include <time.h>
#include <stdio.h>

#ifdef TC
#include <fcntl.h>
#include <stdlib.h>
#include <sys/file.h>
#endif

#ifndef TC
#include <sys/file.h>
#endif

void link_rom();
void loadit();
void getstr();
void split_string();
void zero();
void merror();

char	argread = 0;
short	deflt = 1;

void main(argc, argv)
int	argc;
char	*argv[];
{
        int 	i,x;
	short	num1, num2, diff;
	room	*rom1_ptr = 0, *rom2_ptr = 0;
	room	*rom1b_ptr = 0, *rom2b_ptr = 0;
	char	str[80];
	char	str1[80];
	char	str2[80];
	char	typestr[10];
	char	type;

	umask(0);
	typestr[0] = 0;

do {
	if (!argread && (argc > 3)) {
		num1 = atoi(argv[1]);
		num2 = atoi(argv[2]);
		strcpy(typestr, argv[3]);	
		argread = 1;
	}
	else {
		printf("\nCurrent type: %s Link rooms: ", typestr);
		gets(str);
		if (!str[0]) exit(0);
		if (str[0] == 'n') {
			diff = atoi(str+1); /* ignore 'n', get difference */
			if (diff == 0) diff = deflt;
			deflt = diff;
			num1 = num2;
			num2 += diff;
		}
		else if (str[0] == 'b') {
			diff = atoi(str+1); /* ignore 'n', get difference */
			if (diff == 0) diff = deflt;
			deflt = diff;
			num2 = num1;
			num1 -= diff;
		}
		else if (str[0] == 'u') {
		if (save_rom_to_file(num1, rom1b_ptr) < 0)
			printf("Error undoing room %d\n", num1);
		if (save_rom_to_file(num2, rom2b_ptr) < 0)
			printf("Error undoing room %d\n", num2);
		printf("\nLast link undone.\n");
		continue;
		}
		else {
			split_string(str, str1, str); /* source room = str1 */
			split_string(str, str2, str); /* dest room = str2 */
			if (*str) strcpy(typestr, str);/* typestr = (sn, we) */
			num1 = atoi(str1);
			num2 = atoi(str2);
		}
	}
/* Check type */
	if ((strcmp(typestr, "we")) && (strcmp(typestr, "ns"))) {
		printf ("Illegal link type: %s\n", typestr);
		continue;
	}	

/* Free up old room memory! */
	if (rom1_ptr) free_rom(rom1_ptr);
	if (rom2_ptr) free_rom(rom2_ptr);
	if (rom1b_ptr) free_rom(rom1b_ptr);
	if (rom2b_ptr) free_rom(rom2b_ptr);
	rom1_ptr = rom2_ptr = rom1b_ptr = rom2b_ptr = 0;

	loadit(num1, &rom1_ptr, &rom1b_ptr);
	loadit(num2, &rom2_ptr, &rom2b_ptr);

	printf("\nVia %s, linking %d(%s) <-> %d(%s).\n",
	  typestr, num1, rom1_ptr->name, num2, rom2_ptr->name);

	if (!strcmp(typestr, "we")) {
		link_rom(&rom1_ptr, num2, "east");
		link_rom(&rom2_ptr, num1, "west");
	}
	else if (!strcmp(typestr, "ns")) {
		link_rom(&rom1_ptr, num2, "south");
		link_rom(&rom2_ptr, num1, "north");
	}

/* Save rooms to disk */
	if (save_rom_to_file(num1, rom1_ptr) < 0)
		printf("Error saving room %d\n", num1);
	if (save_rom_to_file(num2, rom2_ptr) < 0)
		printf("Error saving room %d\n", num2);

} while (!argread);
	
}

void link_rom(rom_ptr, tonum, dir)
room	**rom_ptr;
short	tonum;
char	*dir;
{
	exit_	*ext;
	xtag	*next_xtag;
	xtag	*prev_xtag = 0;
	xtag	*new_xtag;
	char	found = 0;

	next_xtag = (*rom_ptr)->first_ext;

	while(next_xtag) {
		ext = next_xtag->ext;
		prev_xtag = next_xtag;
		next_xtag = next_xtag->next_tag;
		if (!strcmp(ext->name, dir)) {
			zero(ext, sizeof(exit_));
			strcpy(ext->name, dir);

			ext->room = tonum;
			return;
		}
	}
/* direction not found in current exit list, so create a new one */

/* allocate memory for exit and a new tag */
	new_xtag = (xtag *)malloc(sizeof(xtag));
	ext = (exit_ *)malloc(sizeof(exit_));

/* zero new exit and set its direction and number */
	zero(ext, sizeof(exit_));
	strcpy(ext->name, dir);
	ext->room = tonum;

	new_xtag->next_tag = 0;	/* new tag is end of tag list */
	new_xtag->ext = ext;	/* tag points to new exit */

/* old end of list point to new tag */
	if (prev_xtag)
		prev_xtag->next_tag = new_xtag;
	else
		(*rom_ptr)->first_ext = new_xtag;
}

void loadit(num, rom_ptr, romb_ptr)
short	num;
room	**rom_ptr;
room	**romb_ptr;
{
	if (load_rom_from_file(num, rom_ptr)) {
		printf("Warning:  Room %d does not exist. Creating new room.\n", num);
        	(*rom_ptr)->rom_num = num;
        	if (!(*rom_ptr)->established)
			(*rom_ptr)->established = time(0);
		*romb_ptr = (room *)malloc(sizeof(room));
		zero(*romb_ptr, sizeof(room));
	}
	else {
		load_rom_from_file(num, romb_ptr);
	}
}

void getstr(str, origstr, maxlen)
char 	*str, *origstr;
int	maxlen;
{
	char tempstr[256];

	gets(tempstr);
	if(!tempstr[0])
		strcpy(str, origstr);
	else if(!strcmp(tempstr, "@"))
		str[0] = 0;
	else
		strncpy(str, tempstr, maxlen-1);
}

void split_string(str_in, str_one, str_two)
char *str_in;
char *str_one;
char *str_two;
{
  int i=0, j=0;

  while(str_in[i] != ' ' && str_in[i] != '\n' && str_in[i] != 0)
    str_one[j++] = str_in[i++];
  str_one[j]=0; j=0;
  while(str_in[i] == ' ' || str_in[i] == '\n') i++;
  if(str_in[i])
    while(str_in[i] != 0 && str_in[i] != '\n')
      str_two[j++] = str_in[i++];
  str_two[j]=0;
}
