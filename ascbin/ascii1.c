/* ASCII1.C  Ascii <--> Binary changer.					*/
/*  Copyright 1991 Steven Wallace.						*/
/*  Developed for use with Mordor 2.0.					*/
/*														*/
/*  Seriously rewritten by John P. Freeman for Mordor 4.x+ */
/*  Copyright 1998 John P. Freeman						*/
/* 
 *  Updated again for Mordor 6.x+
 *  (C) 2001  Paul Telford <paul@droflet.net>
 */
 

#include <stdio.h>
#include <stdlib.h>

#include "../include/port.h"
#include "../include/morutil.h"
#include "../include/mordb.h"

#include "ascbin.h"

static char	*player_file = "players.txt";
static char	*creature_file = "creatures.txt";
static char	*object_file = "objects.txt";
static char	*room_file = "rooms.txt";


char	error;


main(int argc, char *argv[])
{
	
	printf("\nascbin version 3.2 written by Steve Wallace, John P. Freeman, and Paul Telford\n\n");

	if ( argc < 3 )
	{
		usage();
		return(0);
	}

	umask (0);
	switch(*argv[1]) {

		case 'a':  
			/* ascii to binary conversion */
			if ( argv[2][0] != '-')
			{
				usage();
				return(1);
			}


			switch( argv[2][1] )
			{
				case 'p':
				case 'P':
					read_all_ply();
					break;

				case 'c':
				case 'C':
					read_all_crt();
					break;

				case 'r':
				case 'R':
					read_all_rom();
					break;

				case 'o':
				case 'O':
					read_all_obj();
					break;

				case 'a':
				case 'A':
					read_all_ply();
					read_all_crt();
					read_all_rom();
					read_all_obj();
					break;

				default:
					usage();
					break;
			}
			break;


		case 'b':  
			/* binary to ascii conversion */ 
			if ( argv[2][0] != '-' )
			{
				usage();
				return(1);
			}

			switch( argv[2][1] )
			{
				case 'p':
				case 'P':
					write_all_ply();
					break;

				case 'c':
				case 'C':
					write_all_crt();
					break;

				case 'r':
				case 'R':
					write_all_rom();
					break;

				case 'o':
				case 'O':
					write_all_obj();
					break;

				case 'a':
				case 'A':
					write_all_ply();
					write_all_crt();
					write_all_rom();
					write_all_obj();
					break;

				default:
					usage();
					break;
			}
			break;

		default:
			usage();
	}

	return(0);
}
	

void usage()
{
	printf("Usage: ascbin a -[r|p|c|o|a]\n");
	printf("       or\n");
	printf("       ascbin b -[r|p|c|o|a]\n");
	printf("       a to convert ASCII to binary\n");
	printf("       b to convert binary to ASCII\n");
	printf("       -r to convert all rooms\n");
	printf("       -p to convert all players\n");
	printf("       -c to convert all creatures\n");
	printf("       -o to convert all objects\n");
	printf("       -a to convert entire database\n");

	return;
}


void write_all_rom()
{
	HFINDFILE	hff;
	char	filename[256];
	room	*rom;
	FILE	*fp;
	int n=0;

	fp = fopen( room_file, "w");
	if ( fp != NULL ) 
	{

		hff = find_first_file(get_room_path(), filename, 256);
		if ( hff )
		{
			do 
			{
				if ( filename[0] != '.' )
				{
					if (load_rom_from_file( atoi(&filename[1]), &rom))
						printf("Unable to read binary room %d.  Continuing.\n", atoi(&filename[1]) );
					else 
					{
						n++;
						write_room(fp, rom);
						free_rom(rom);
					}
				}
			} while( find_next_file( hff, filename, 256 ));

			close_find_file(hff);
		}

		fclose(fp);
	}

	
}

void write_all_ply()
{
	HFINDFILE	hff;
	char	filename[256];
	creature	*ply;
	FILE	*fp;

	fp = fopen( player_file, "w");
	if ( fp != NULL ) 
	{

		hff = find_first_file(get_player_path(), filename, 256);
		if ( hff )
		{
			do 
			{
				if ( filename[0] != '.')
				{
					if (load_ply_from_file(filename, &ply))
						printf("Unable to read binary player %s.\n", filename);
					else 
					{
						write_player(fp, filename, ply);
						free_crt(ply);
					}
				}
			} while( find_next_file( hff, filename, 256 ));

			close_find_file(hff);
		}
		fclose(fp);
	}
	
	return;
}

void write_all_obj()
{
	HFINDFILE	hff;
	char	filename[256];
	object *obj;
	int	num;
	int	index;
	FILE	*fp;

	fp = fopen( object_file, "w");
	if ( fp != NULL ) 
	{

		hff = find_first_file(get_object_path(), filename, 256);
		if ( hff )
		{
			do 
			{
				if ( filename[0] != '.' && toupper(filename[0]) == 'O' && strlen(filename) == 3)
				{
					index = OFILESIZE * atoi(&filename[1]);

					for (num=0; num < OFILESIZE ; num++) 
					{
						if (load_obj_from_file(index + num, &obj ) == 0 )
						{
							write_object(fp, index+num,obj);
							free_obj(obj);
						}
					}

				}
			} while( find_next_file( hff, filename, 256 ));

			close_find_file(hff);
		}

		fclose(fp);
	}

	return;
	
}

void write_all_crt()
{
	HFINDFILE	hff;
	char	filename[256];
	creature	*crt;
	int	num;
	int	index;
	FILE	*fp;

	fp = fopen( creature_file, "w");
	if ( fp != NULL ) 
	{

		hff = find_first_file(get_monster_path(), filename, 256);
		if ( hff )
		{
			do 
			{
				if ( filename[0] != '.' && toupper(filename[0]) == 'M' && strlen(filename) == 3)
				{
					index = MFILESIZE * atoi(&filename[1]);

					for (num=0; num != MFILESIZE; num++) {
						if (load_crt_from_file(index + num, &crt ) == 0 )
						{
							write_creature(fp, index+num,crt);
							free_crt(crt);
						}
					}
				}
			} while( find_next_file( hff, filename, 256 ));

			close_find_file(hff);
		}

		fclose(fp);
	}

	return;
	
}



void read_all_rom()
{
	char	str[256];
	room	*rom;
	long	offset;
	FILE	*fp;

	/* open ascii source file */
	fp=fopen(room_file, "r");
	if ( fp == NULL )
	{
		printf("Cannot open %s, room_file\n" );
		return;
	}


	while(!feof(fp)) 
	{
		offset = ftell(fp);
		if (EOF == fscanf(fp, "%s", str)) 
			break;
		fseek(fp, offset, 0); /* 0=SEEK_SET ? */

		if (strcmp(str, "#begrom") == 0) 
		{
			rom = (room *)malloc(sizeof(room));
			if ( rom )
			{
				zero(rom, sizeof(room));
				if (read_room(fp, rom))
					printf("Error reading ascii room. Continuing.\n");
				else if (save_rom_to_file((int)rom->rom_num, rom))
					printf("Error saving binary room %d to file", rom->rom_num);
				else 
					printf("Room #%d:'%s' converted to binary.\n",
						(int)rom->rom_num, rom->name);
				free_rom(rom);
			}
		}
	}

	return;

}

void read_all_ply()
{
	char	str[256];
	creature *ply;
	long	offset;
	FILE	*fp;

	/* open ascii source file */
	fp=fopen(player_file, "r");
	if ( fp == NULL )
	{
		printf("Cannot open %s, player_file\n" );
		return;
	}


	while(!feof(fp)) 
	{
		offset = ftell(fp);
		if (EOF == fscanf(fp, "%s", str)) 
			break;
		fseek(fp, offset, 0); /* 0=SEEK_SET ? */

		if (!strcmp(str, "#begPLY")) {
			ply = (creature *)malloc(sizeof(creature));
			if ( ply )
			{
				zero(ply, sizeof(creature));
				if (read_player(fp, str, ply))
					printf("Error reading ascii player. Continuing.\n");
				else if (save_ply_to_file(str, ply))
					printf("Error saving player %s to binary file\n", ply->name);
				else 
					printf("Player %s converted to binary.\n", ply->name);
				free_crt(ply);
			}
		} 
	}

	return;
}

void read_all_obj()
{
	char	str[256];
	object	*obj;
	long	offset;
	FILE	*fp;
	int		num;

	/* open ascii source file */
	fp=fopen(object_file, "r");
	if ( fp == NULL )
	{
		printf("Cannot open %s, room_file\n" );
		return;
	}


	while(!feof(fp)) 
	{
		offset = ftell(fp);
		if (EOF == fscanf(fp, "%s", str)) 
			break;
		fseek(fp, offset, 0); /* 0=SEEK_SET ? */

		if (strcmp(str, "#begobj") == 0) 
		{
			obj = (object *)malloc(sizeof(object));
			if ( obj )
			{
				zero(obj, sizeof(object));
				num = read_object(fp, obj);
				if (num == -1)
					printf("Error reading ascii object\n");
				else if (save_obj_to_file(num, obj))
					printf("Error saving object %d to binary file\n", num);
				else 
					printf("Object #%d:%s converted to binary.\n", num,
						obj->name);
				free_obj(obj);
			}
		}
	}

}

void read_all_crt()
{
	char		str[256];
	creature	*crt;
	long		offset;
	int			num;
	FILE		*fp;

	/* open ascii source file */
	fp=fopen(creature_file, "r");
	if ( fp == NULL )
	{
		printf("Cannot open %s, creature_file\n" );
		return;
	}


	while(!feof(fp)) 
	{
		offset = ftell(fp);
		if (EOF == fscanf(fp, "%s", str)) 
			break;
		fseek(fp, offset, 0); /* 0=SEEK_SET ? */

		if (!strcmp(str, "#begcrt")) {
			crt = (creature *)malloc(sizeof(creature));
			if ( crt )
			{
				zero(crt, sizeof(creature));
				num = read_creature(fp, crt);
				if(num == -1)
					printf("Error reading ascii creature. Continuing.\n");
				else if (save_crt_to_file(num, crt))
					printf("Error saving creature %s to binary file\n", num);
				else 
					printf("Creature #%d:%s converted to binary.\n", num,
						crt->name);
				free_crt(crt);
			}
		}
	}

	return;
}


int write_room(FILE *fp, room *rom)
{
	int	i;
	char perm_only;
	short   cnt;
	xtag    *xp;
	ctag    *cp;
	otag    *op;
	struct exit_	*exit;
	
	perm_only = 0;  /* all objects, monsters */

	printf("Writing room number %d\n", rom->rom_num );

/* begin of room data */
	fprintf(fp, "#begrom\n");
	
	write_short(fp, rom->rom_num);
	write_chars(fp, rom->name, sizeof(rom->name));
	write_char(fp, rom->lolevel);
	write_char(fp, rom->hilevel);
	write_short(fp, rom->special);
	write_char(fp, rom->trap);
	write_short(fp, rom->trapexit);
	write_chars(fp, rom->track, sizeof(rom->track));
	write_chars(fp, rom->flags, sizeof(rom->flags));
	
	for (i = 0; i != 10; i++)
		write_short(fp, rom->random[i]);
	
	write_char(fp, rom->traffic);
	
	for (i = 0; i != 10; i++)
		write_lasttime(fp, rom->perm_mon[i]);
	for (i = 0; i != 10; i++)
		write_lasttime(fp, rom->perm_obj[i]);
	
	write_long(fp, rom->beenhere);
	write_long(fp, rom->established);
	write_short(fp, rom->death_rom);
	write_char(fp,rom->zone_type);
	write_char(fp,rom->zone_subtype);
	write_char(fp,rom->env_type);
	write_short(fp,rom->special1);
	write_long(fp,rom->special2);

	
	/* short description */
	if(rom->short_desc) {
		write_short(fp, (short)(strlen(rom->short_desc)+1)); /* # of characters to read in */
		write_chars(fp, rom->short_desc, strlen(rom->short_desc)+1);
	} else {
		write_short(fp, 0);
	}
		
	/* long description */
	if(rom->long_desc) {
		write_short(fp, (short)(strlen(rom->long_desc)+1));  /* # of characters to read in */
		write_chars(fp, rom->long_desc, strlen(rom->long_desc)+1);
	} else {
		write_short(fp, 0);
	}
		
	/* object description */
	if(rom->obj_desc) {
		write_short(fp, (short)(strlen(rom->obj_desc)+1));  /* # of characters to read in */
		write_chars(fp, rom->obj_desc, strlen(rom->obj_desc)+1);
	} else {
		write_short(fp, 0);
	}

	/* exits */
	cnt = count_ext(rom);
	write_int(fp, cnt);
	
	xp = rom->first_ext;
	while(xp) {
		exit = xp->ext;
/* write exit structure */
		write_chars(fp, exit->name, sizeof(exit->name));
		write_short(fp, exit->room);
		write_chars(fp, exit->flags, sizeof(exit->flags));
		write_lasttime(fp, exit->ltime);
		write_lasttime(fp, exit->random);
		write_chars(fp, exit->rand_mesg[0], sizeof(exit->rand_mesg[0]));
		write_chars(fp, exit->rand_mesg[1], sizeof(exit->rand_mesg[1]));
		write_char(fp, exit->key);
/* end exit structure */
	
		xp = xp->next_tag;
	}
	
/* monsters */
	cnt = count_mon(rom, perm_only);
	write_int(fp, cnt);
	
	cp = rom->first_mon;
	while(cp) {
		if(!perm_only || (perm_only && cp->crt->flags[0] & 1))
			write_creature(fp, -1, cp->crt);
		cp = cp->next_tag;
	}
	
/* objects */
	cnt = count_ite(rom, perm_only);
	write_int(fp, cnt);
	
	op = rom->first_obj;
	while(op) {
		if(!perm_only || (perm_only && op->obj->flags[0] & 1))
			write_object(fp, -1, op->obj);
		op = op->next_tag;
	}
/* end of room data */
	fprintf(fp, "#endrom\n");

	return(0);
}
	
int write_object(FILE *fp, int num, object *obj)
{
	int	cnt;
	char perm_only;
	otag	*op;
	
	perm_only = 0;

	fprintf(fp, "#begobj\n");
	
/* basic object format (excluding pointers) */
	write_object_basic(fp, num, obj);
	
	fprintf(fp, "#endobj\n");

/* objects carried within objects */
	cnt = count_obj(obj, perm_only);
	write_int(fp, cnt);
	
	if(cnt > 0) {
		op = obj->first_obj;
		while(op) {
			if(!perm_only || (perm_only && op->obj->flags[0] & 1))
				write_object(fp, -1, op->obj);
			op = op->next_tag;
		}
	}
	
/* end of complete object */ 

	return(0);
}
	
int write_creature(FILE *fp, int num, creature *crt)
{
	int	cnt;
	char perm_only;
	otag	*op;
	
	perm_only = 0;

	
	fprintf(fp, "#begcrt\n");
/* basic creature format (excluding pointers) */
	write_creature_basic(fp, num, crt);
	fprintf(fp, "#endcrt\n");
	
/* objects carried by creature */
	cnt = count_inv(crt, perm_only);
	write_int(fp, cnt);
	
	if(cnt > 0) {
		op = crt->first_obj;
		while(op) {
			if(!perm_only || (perm_only && op->obj->flags[0] & 1))
				write_object(fp, -1, op->obj);
			op = op->next_tag;
		}
	}
	
/* end of complete creature */ 

	return(0);

}
	
int write_player(FILE *fp, char *str, creature *ply)
{
	printf("Writing player %s\n", ply->name );

	fprintf(fp, "#begPLY\n");
	fprintf(fp, "%s\n", str);  /* write player/file name */
	write_creature(fp, -1, ply);
	fprintf(fp, "#endPLY\n");

	return(0);

}
	
int write_object_basic(FILE *fp, int index, object *obj)
{
	int	i;
	
/* begin of object data */
	write_int(fp, index);  /* object # */
	
	printf("Writing object %s\n", obj->name );

	write_chars(fp, obj->name, sizeof(obj->name));
	write_chars(fp, obj->description, sizeof(obj->description));
	for (i = 0; i != 3; i++)
		write_chars(fp, obj->key[i], sizeof(obj->key[i]));
	write_chars(fp, obj->use_output, sizeof(obj->use_output));
	write_long(fp, obj->value);
	write_short(fp, obj->weight);
	
	write_char(fp, obj->type);
	write_char(fp, obj->adjustment);
	
	write_short(fp, obj->shotsmax);
	write_short(fp, obj->shotscur);
	write_short(fp, obj->ndice);
	write_short(fp, obj->sdice);
	write_short(fp, obj->pdice);
	
	write_char(fp, obj->armor);
	write_char(fp, obj->wearflag);
	write_char(fp, obj->magicpower);
	write_char(fp, obj->magicrealm);
	write_short(fp, obj->special);
	write_chars(fp, obj->flags, sizeof(obj->flags));
	write_char(fp, obj->questnum);
	write_char(fp,obj->strength);
	write_char(fp,obj->dexterity);
	write_char(fp,obj->constitution);
	write_char(fp,obj->intelligence);
	write_char(fp,obj->piety);
	for (i = 0; i != 16; i++)
		write_short(fp, obj->sets_flag[i]);
	write_short(fp,obj->special1);
	write_long(fp,obj->special2);

	
/* end of object data */

	return(0);
	
}
	
int write_creature_basic(FILE *fp, int index, creature *crt)
{
	int	i;

	printf("Writing creature %s\n", crt->name );

/* begin of creature data */
	write_int(fp, index);  /* creature # */
	
	write_chars(fp, crt->name, sizeof(crt->name));
	write_chars(fp, crt->description, sizeof(crt->description));
	write_chars(fp, crt->talk, sizeof(crt->talk));
	write_chars(fp, crt->password, sizeof(crt->password));
	for (i = 0; i != 3; i++)
		write_chars(fp, crt->key[i], sizeof(crt->key[i]));
	
	write_short(fp, crt->fd);
	
	write_char(fp, crt->level);
	write_char(fp, crt->type);
	write_char(fp, crt->class);
	write_char(fp, crt->race);
	write_char(fp, crt->numwander);
	
	write_short(fp, crt->alignment);
	
	write_char(fp, crt->strength);
	write_char(fp, crt->dexterity);
	write_char(fp, crt->constitution);
	write_char(fp, crt->intelligence);
	write_char(fp, crt->piety);
	
	write_short(fp, crt->hpmax);
	write_short(fp, crt->hpcur);
	write_short(fp, crt->mpmax);
	write_short(fp, crt->mpcur);
	
	write_char(fp, crt->armor);
	write_char(fp, crt->thaco);
	
	write_long(fp, crt->experience);
	write_long(fp, crt->gold);
	
	write_short(fp, crt->ndice);
	write_short(fp, crt->sdice);
	write_short(fp, crt->pdice);

	write_short(fp, crt->special);
	
	for (i=0; i != 6; i++)
		write_long(fp, crt->proficiency[i]);
	for (i=0; i < 8; i++)
		write_long(fp, crt->realm[i]);

	write_chars(fp, crt->spells, sizeof(crt->spells));
	write_chars(fp, crt->flags, sizeof(crt->flags));
	write_chars(fp, crt->quests, sizeof(crt->quests));
	write_char(fp, crt->questnum);
	
	for (i=0; i != 10; i++)
		write_short(fp, crt->carry[i]);
	write_short(fp, crt->rom_num);
	
	for (i=0; i != 10; i++) {
		write_char(fp, crt->daily[i].max);
		write_char(fp, crt->daily[i].cur);
		write_long(fp, crt->daily[i].ltime);
	}
	
	for (i=0; i < 45; i++)
		write_lasttime(fp, crt->lasttime[i]);
	write_long(fp,crt->bank_balance);
	write_chars(fp,crt->title, sizeof(crt->title));
	for(i=0; i != 5; i++)
		write_short(fp, crt->misc_stats[i]);
	write_short(fp,crt->clanindex);
	write_long(fp,crt->clanexp);
	write_char(fp,crt->guildtype);
	write_long(fp,crt->guildexp);
	write_short(fp,crt->special1);
	write_long(fp,crt->special2);
	
/* end of creature data */

	return(0);

}
	
/**********************************************************************/
/*                              read_room                             */
/**********************************************************************/

/* Loads a room from an already open and positioned file into the memory */
/* pointed to by the second parameter.  Also loaded are all creatures,   */
/* exits, objects and descriptions for the room.  -1 is returned upon    */
/* failure.                                                              */

int read_room(FILE *fp, room *rom)
{
    int             i;
	int		cnt;
	xtag            *xp;
	xtag            **xprev;
	exit_           *ext;
	ctag            *cp;
	ctag            **cprev;
	creature        *crt;
	otag            *op;
	otag            **oprev;
	object          *obj;
	char            *sh, *lo, *ob;
	char		str[10];

/* begin of basic structure */
	fscanf(fp, "%s", str); 
	if (strcmp(str, "#begrom"))
		return(1);
	
	read_short(fp, &rom->rom_num);
	read_chars(fp, rom->name, sizeof(rom->name));
	read_char(fp, &rom->lolevel);
	read_char(fp, &rom->hilevel);
	read_short(fp, &rom->special);
	read_char(fp, &rom->trap);
	read_short(fp, &rom->trapexit);
	read_chars(fp, rom->track, sizeof(rom->track));
	read_chars(fp, rom->flags, sizeof(rom->flags));
	
	for (i = 0; i != 10; i++)
		read_short(fp, &rom->random[i]);
	
	read_char(fp, &rom->traffic);
	
	for (i = 0; i != 10; i++)
		read_lasttime(fp, &rom->perm_mon[i]);
	for (i = 0; i != 10; i++)
		read_lasttime(fp, &rom->perm_obj[i]);
	
	read_long(fp, &rom->beenhere);
	read_long(fp, &rom->established);
	read_short(fp, &rom->death_rom);
	read_char(fp, &rom->zone_type);
	read_char(fp, &rom->zone_subtype);
	read_char(fp, &rom->env_type);
	read_short(fp,&rom->special1);
	read_long(fp,&rom->special2);

/* end basic structure */

    rom->short_desc = 0;
    rom->long_desc  = 0;
	rom->obj_desc = 0;
    rom->first_ext = 0;
    rom->first_obj = 0;
    rom->first_mon = 0;
    rom->first_ply = 0;

/* short description */
	read_int(fp, &cnt);

    if(cnt) {
		sh = (char *)malloc(cnt);
		if(sh) {
			read_chars(fp, sh, cnt);
			rom->short_desc = sh;
		}
		else
			merror("read_rom", FATAL);
        }

/* long description */
	read_int(fp, &cnt);

    if(cnt) {
        lo = (char *)malloc(cnt);
        if(lo) {
			read_chars(fp, lo, cnt);
			rom->long_desc = lo;
		}
		else
            merror("read_rom", FATAL);
		}

/* object description */
	read_int(fp, &cnt);

	if(cnt) {
		ob = (char *)malloc(cnt);
		if(ob) {
			read_chars(fp, ob, cnt);
			rom->obj_desc = ob;
		}
		else
			merror("read_rom", FATAL);
	}

/* exits */
	read_int(fp, &cnt);

    xprev = &rom->first_ext;
    while(cnt > 0) {
		cnt--;
		xp = (xtag *)malloc(sizeof(xtag));
		if(xp) {
				ext = (exit_ *)malloc(sizeof(exit_));
				if(ext) {
				/* begin exit structure */
					read_chars(fp, ext->name, sizeof(ext->name));
					read_short(fp, &ext->room);
					read_chars(fp, ext->flags, sizeof(ext->flags));
					read_lasttime(fp, &ext->ltime);
					read_lasttime(fp, &ext->random);
					read_chars(fp,ext->rand_mesg[0],sizeof(ext->rand_mesg[0]));
					read_chars(fp,ext->rand_mesg[1],sizeof(ext->rand_mesg[1]));
					read_char(fp, &ext->key);
					/* end exit structure */
                    xp->ext = ext;
                    xp->next_tag = 0;
                    *xprev = xp;
                    xprev = &xp->next_tag;
                }
                else
					merror("read_rom", FATAL);
        }
        else
			merror("read_rom", FATAL);
    }

/* monsters/creatures */
	read_int(fp, &cnt);

        cprev = &rom->first_mon;
        while(cnt > 0) {
                cnt--;
                cp = (ctag *)malloc(sizeof(ctag));
                if(cp) {
                        crt = (creature *)malloc(sizeof(creature));
                        if(crt) {
                                read_creature(fp, crt);
                                crt->parent_rom = rom;
                                cp->crt = crt;
                                cp->next_tag = 0;
                                *cprev = cp;
                                cprev = &cp->next_tag;
                        }
                        else
                                merror("read_rom", FATAL);
                }
                else
                        merror("read_rom", FATAL);
        }

/* items/objects */
	read_int(fp, &cnt);

        oprev = &rom->first_obj;
        while(cnt > 0) {
                cnt--;
                op = (otag *)malloc(sizeof(otag));
                if(op) {
                        obj = (object *)malloc(sizeof(object));
                        if(obj) {
                                read_object(fp, obj);
                                obj->parent_rom = rom;
                                op->obj = obj;
                                op->next_tag = 0;
                                *oprev = op;
                                oprev = &op->next_tag;
                        }
                        else
                                merror("read_rom", FATAL);
                }
                else
                        merror("read_rom", FATAL);
        }

	fscanf(fp, "%s", str); 
	if (strcmp(str, "#endrom"))
		return(1);

        if(error)
                return(1);
        else
                return(0);
}

int read_player(FILE *fp, char *name, creature *ply)
{
	char	 str[10];

	fscanf(fp, "%s", str); 
	if (strcmp(str, "#begPLY"))
		return(1);

	fscanf(fp, "%s\n", name);  /* write player/file name */
	read_creature(fp, ply);

	fscanf(fp, "%s", str); 
	if (strcmp(str, "#endPLY"))
		return(1);
	return(0);
}

/**********************************************************************/
/*                              read_object                           */
/**********************************************************************/

/* Loads the object from the open and positioned file described by fd */
/* and also loads every object which it might contain.  Returns -1 if */
/* there was an error.                                                */

int read_object(FILE *fp, object *obj)
{
        int             cnt, error=0;
        int		temp;
        otag            *op;
        otag            **prev;
        object          *tobj;
	char		str[10];

	fscanf(fp, "%s", str); 
	if (strcmp(str, "#begobj"))
		return(1);

        read_object_basic(fp, &temp, obj);

	fscanf(fp, "%s", str); 
	if (strcmp(str, "#endobj"))
		return(1);

        obj->first_obj = 0;
        obj->parent_obj = 0;
        obj->parent_rom = 0;
        obj->parent_crt = 0;
        if(obj->shotscur > obj->shotsmax)
                obj->shotscur = obj->shotsmax;

/* get # of objects to read in */
        read_int(fp, &cnt);

        prev = &obj->first_obj;
        while(cnt > 0) {
                cnt--;
                op = (otag *)malloc(sizeof(otag));
                if(op) {
                        tobj = (object *)malloc(sizeof(object));
                        if(tobj) {
                                if(read_object(fp, tobj) == -1)
                                        error = 1;
                                tobj->parent_obj = obj;
                                op->obj = tobj;
                                op->next_tag = 0;
                                *prev = op;
                                prev = &op->next_tag;
                        }
                        else
                                merror("read_obj", FATAL);
                }
                else
                        merror("read_obj", FATAL);
        }

        if(error)
                return(-11);
        else
                return(temp);
}

/**********************************************************************/
/*                              read_creature                         */
/**********************************************************************/

/* Loads a creature from an open and positioned file.  The creature is */
/* loaded at the mem location specified by the second parameter.  In   */
/* addition, all the creature's objects have memory allocated for them */
/* and are loaded as well.  Returns -1 on fail.                        */

int read_creature(FILE *fp, creature *crt)
{
        int             n, cnt, error=0;
        int		temp;
        otag            *op;
        otag            **prev;
        object          *obj;
	char		str[10];

	fscanf(fp, "%s", str);
	if (strcmp(str, "#begcrt"))
		return(1);

        read_creature_basic(fp, &temp, crt);

	fscanf(fp, "%s", str);
	if (strcmp(str, "#endcrt"))
		return(1);

        crt->first_obj = 0;
        crt->first_fol = 0;
        crt->first_enm = 0;
	crt->first_tlk = 0;
        crt->parent_rom = 0;
        crt->following = 0;
        for(n=0; n<20; n++)
                crt->ready[n] = 0;
        if(crt->mpcur > crt->mpmax)
                crt->mpcur = crt->mpmax;
        if(crt->hpcur > crt->hpmax)
                crt->hpcur = crt->hpmax;

/* get # of objects to read in */
        read_int(fp, &cnt);

        prev = &crt->first_obj;
        while(cnt > 0) {
                cnt--;
                op = (otag *)malloc(sizeof(otag));
                if(op) {
                        obj = (object *)malloc(sizeof(object));
			zero(obj, sizeof(object));
                        if(obj) {
				read_object(fp, obj);
                                obj->parent_crt = crt;
                                op->obj = obj;
                                op->next_tag = 0;
                                *prev = op;
                                prev = &op->next_tag;
                        }
                        else
                                merror("read_crt", FATAL);
                 }
                else
                        merror("read_crt", FATAL);
        }


        if(error)
                return(-1);
        else
                return(temp);
}

int read_object_basic(FILE *fp, int *index, object *obj)
{
	int	i;
	char	str[10];
	
/* begin of object data */
	read_int(fp, index);  /* object # */
	
	read_chars(fp, obj->name, sizeof(obj->name));
	read_chars(fp, obj->description, sizeof(obj->description));
	for (i = 0; i != 3; i++)
		read_chars(fp, obj->key[i], sizeof(obj->key[i]));
	read_chars(fp, obj->use_output, sizeof(obj->use_output));
	read_long(fp, &obj->value);
	read_short(fp, &obj->weight);
	
	read_char(fp, &obj->type);
	read_char(fp, &obj->adjustment);
	
	read_short(fp, &obj->shotsmax);
	read_short(fp, &obj->shotscur);
	read_short(fp, &obj->ndice);
	read_short(fp, &obj->sdice);
	read_short(fp, &obj->pdice);
	
	read_char(fp, &obj->armor);
	read_char(fp, &obj->wearflag);
	read_char(fp, &obj->magicpower);
	read_char(fp, &obj->magicrealm);
	read_short(fp, &obj->special);
	read_chars(fp, obj->flags, sizeof(obj->flags));
	read_char(fp, &obj->questnum);
	read_char(fp,&obj->strength);
	read_char(fp,&obj->dexterity);
	read_char(fp,&obj->constitution);
	read_char(fp,&obj->intelligence);
	read_char(fp,&obj->piety);
	for (i = 0; i != 16; i++)
		read_short(fp, &obj->sets_flag[i]);
	read_short(fp,&obj->special1);
	read_long(fp,&obj->special2);
	
/* end of object data */
	return(0);
}
	
int read_creature_basic(FILE *fp, int *index, creature *crt)
{
	int	i;
	char	str[10];
	
/* begin of creature data */

	read_int(fp, index);  /* creature # */
	
	read_chars(fp, crt->name, sizeof(crt->name));
	read_chars(fp, crt->description, sizeof(crt->description));
	read_chars(fp, crt->talk, sizeof(crt->talk));
	read_chars(fp, crt->password, sizeof(crt->password));
	for (i = 0; i != 3; i++)
		read_chars(fp, crt->key[i], sizeof(crt->key[i]));
	
	read_short(fp, &crt->fd);
	
	read_char(fp, &crt->level);
	read_char(fp, &crt->type);
	read_char(fp, &crt->class);
	read_char(fp, &crt->race);
	read_char(fp, &crt->numwander);
	
	read_short(fp, &crt->alignment);
	
	read_char(fp, &crt->strength);
	read_char(fp, &crt->dexterity);
	read_char(fp, &crt->constitution);
	read_char(fp, &crt->intelligence);
	read_char(fp, &crt->piety);
	
	read_short(fp, &crt->hpmax);
	read_short(fp, &crt->hpcur);
	read_short(fp, &crt->mpmax);
	read_short(fp, &crt->mpcur);
	
	read_char(fp, &crt->armor);
	read_char(fp, &crt->thaco);
	
	read_long(fp, &crt->experience);
	read_long(fp, &crt->gold);
	
	read_short(fp, &crt->ndice);
	read_short(fp, &crt->sdice);
	read_short(fp, &crt->pdice);

	read_short(fp, &crt->special);
	
	for (i=0; i != 6; i++)
		read_long(fp, &crt->proficiency[i]);
	for (i=0; i != 8; i++)
		read_long(fp, &crt->realm[i]);

	read_chars(fp, crt->spells, sizeof(crt->spells));
	read_chars(fp, crt->flags, sizeof(crt->flags));
	read_chars(fp, crt->quests, sizeof(crt->quests));
	read_char(fp, &crt->questnum);
	
	for (i=0; i != 10; i++)
		read_short(fp, &crt->carry[i]);
	read_short(fp, &crt->rom_num);
	
	for (i=0; i != 10; i++) {
		read_char(fp, &crt->daily[i].max);
		read_char(fp, &crt->daily[i].cur);
		read_long(fp, &crt->daily[i].ltime);
	}

	for (i=0; i < 45; i++)
		read_lasttime(fp, &crt->lasttime[i]);
	read_long(fp, &crt->bank_balance);
	read_chars(fp,crt->title, sizeof(crt->title));
	for(i=0;i!=5;i++)
		read_short(fp, &crt->misc_stats[i]);
	read_short(fp, &crt->clanindex);
	read_long(fp, &crt->clanexp);
	read_char(fp, &crt->guildtype);
	read_long(fp, &crt->guildexp);
	read_short(fp, &crt->special1);
	read_long(fp, &crt->special2);
	
/* end of creature data */
	return(0);
}
	
void write_lasttime(FILE *fp, struct lasttime lt)
{
	write_long(fp, lt.interval);
	write_long(fp, lt.ltime);
	write_short(fp, lt.misc);
}
	
void read_lasttime(FILE *fp, struct lasttime *lt)
{	
	read_long(fp, &lt->interval);
	read_long(fp, &lt->ltime);
	read_short(fp, &lt->misc);
}


