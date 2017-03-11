/* ASCII.C  Ascii <--> Binary changer.					*/
/*  Copyright 1991 Steven Wallace.					*/
/*  Developed for use with Mordor 2.0.					*/
/*									*/

#include <stdio.h>

#include "mstruct.h"
#include "mextern.h"

#define DEBUG 0

FILE	*FP;
char	error;

int read_room();
int write_room();
int read_player();
int write_player();
int read_creature();
int write_creature();
int read_object();
int write_object();

int read_creature_basic();
int write_creature_basic();
int read_object_basic();
int write_object_basic();
int read_lasttime();
int write_lasttime();
int read_short();
int write_short();
int read_int();
int read_int();
int write_long();
int write_long();
int read_char();
int write_char();
int read_chars();
int write_chars();

int load_rom_from_file();
int save_rom_to_file();
int load_ply_from_file();
int save_ply_from_file();
int load_obj_from_file();
int save_obj_to_file();
int load_crt_from_file();
int save_crt_to_file();

void del_active(); /* dummy routine */

main(argc, argv)
int	argc;
char	*argv[];
{
	char	str[80];
	int	argnum;
	int	num;
	long	offset;
	room	*rom;
	creature *ply;
	object	*obj;
	creature *crt;

/* for (num = 0; num < argc; num++) printf("arg%d: %s\n", num, argv[num]); */

	umask (0);
	switch(*argv[1]) {

/*
 *
 *			ASCII TO BINARY CONVERSION
 *
 */ 
		case 'a':  /* ascii to binary conversion */

			/* loop through all file arguments */
			for (argnum=2; argnum < argc; argnum++) {
				error = 0;
				/* open ascii source file */
				if((FP=fopen(argv[argnum], "r"))==NULL)
					merror("1Can not open ascii file: ");
				while(!feof(FP) && !error) {
					offset = ftell(FP);
					if (EOF == fscanf(FP, "%s", str)) break;
					fseek(FP, offset, 0); /* 0=SEEK_SET ? */

	if (!strcmp(str, "#begrom")) {
		rom = (room *)malloc(sizeof(room));
		zero(rom, sizeof(room));
		if (read_room(rom))
			merror("Error reading ascii room: ");
		else if (save_rom_to_file((int)rom->rom_num, rom))
			merror("Error saving converted binary room to file: ");
		else printf("Room #%d:'%s' converted to binary.\n",
		  (int)rom->rom_num, rom->name);
		free_rom(rom);
	}
	else if (!strcmp(str, "#begPLY")) {
		ply = (creature *)malloc(sizeof(creature));
		zero(ply, sizeof(creature));
		if (read_player(str, ply))
			merror("Error reading ascii player: ");
		else if (save_ply_to_file(str, ply))
			merror("Error saving converted binary player to file:");
		else printf("Player %s converted to binary.\n", ply->name);
		free_crt(ply);
	} 
	else if (!strcmp(str, "#begobj")) {
		obj = (object *)malloc(sizeof(object));
		zero(obj, sizeof(object));
		if (read_object_basic(&num, obj))
			merror("Error reading ascii object: ");
		else if (save_obj_to_file(num, obj))
			merror("Error saving converted binary object to file: ");
		else printf("Object #%d:%s converted to binary.\n", num,
		  obj->name);
		free_obj(obj);
	}
	else if (!strcmp(str, "#begcrt")) {
		crt = (creature *)malloc(sizeof(creature));
		zero(crt, sizeof(creature));
		if (read_creature_basic(&num, crt))
			merror("Error reading ascii creature: ");
		else if (save_crt_to_file(num, crt))
			merror("Error saving converted binary creature to file: ");
		else printf("Creature #%d:%s converted to binary.\n", num,
		  crt->name);
		free_crt(crt);
	}
	else {
		merror("Unkown/illegal structure indicator.");
	}
				} /* end of while loop */

			} /* end of for loop */
			break;
/*
 *
 *			BINARY TO ASCII CONVERSION
 *
 */ 
		case 'b':  /* binary to ascii conversion */ 

			/* open ascii destination file */
			if((FP=fopen(argv[3], "w"))==NULL)
				merror("2Can not open ascii file: ");

			/* loop through all file arguments */
			else for (argnum=4; argnum < argc; argnum++) {
				error = 0;

				/* must give type of binary to convert */
				switch(*argv[2]) {
					case 'r':	/* room */
		if (load_rom_from_file(argv[argnum], &rom))
			merror("Unable to load binary file.");
		else write_room(rom);
		free_rom(rom);
					break;
					case 'p':	/* player */
		if (load_ply_from_file(argv[argnum], &ply))
			merror("Unable to load binary file.");
		else write_player(argv[argnum], ply);
		free_crt(ply);
					break;
					case 'o':	/* objects */
		for (num=0, error=0; num != OFILESIZE && !error; num++) {
			if (load_obj_from_file(num, &obj, argv[argnum]))
				merror("Unable to load object from file.");
			else write_object_basic(num, obj);
			free_obj(obj);
		}
					break;
					case 'c':	/* creatures */
		for (num=0, error=0; num != MFILESIZE && !error; num++) {
			if (load_crt_from_file(num, &crt, argv[argnum]))
				merror("Unable to load creature from file.");
			else write_creature_basic(num, crt);
			free_crt(crt);
		}
					break;
					default:
			merror("Illegal type of binary argument.", FATAL);
				}

			}
			break;
		default:
			printf("Synopsis: convert a asciisrclist...\n");
			printf("      or: convert b r,p,c,o dest srclist...\n");
	}
	close(FP);
}
	
int write_room(rom)
room	*rom;
{
	int	i, perm_only;
	short   n, cnt, error=0;
	xtag    *xp;
	ctag    *cp;
	otag    *op;
	struct exit_	*exit;
	
	perm_only = 0;  /* all objects, monsters */
	
/* begin of room data */
	fprintf(FP, "#begrom\n");
	
	write_short(rom->rom_num);
	write_chars(rom->name, sizeof(rom->name));
	write_char(rom->lolevel);
	write_char(rom->hilevel);
	write_char(rom->special);
	write_char(rom->trap);
	write_short(rom->trapexit);
	write_chars(rom->track, sizeof(rom->track));
	write_chars(rom->flags, sizeof(rom->flags));
	
	for (i = 0; i != 10; i++)
		write_short(rom->random[i]);
	
	write_char(rom->traffic);
	
	for (i = 0; i != 10; i++)
		write_lasttime(rom->perm_mon[i]);
	for (i = 0; i != 10; i++)
		write_lasttime(rom->perm_obj[i]);
	
	write_long(rom->beenhere);
	write_long(rom->established);
	
/* short description */
if(rom->short_desc) {
	write_short(strlen(rom->short_desc)+1); /* # of characters to read in */
	write_chars(rom->short_desc, strlen(rom->short_desc)+1);
} else {
	write_short(0);
}
	
/* long description */
if(rom->long_desc) {
	write_short(strlen(rom->long_desc)+1);  /* # of characters to read in */
	write_chars(rom->long_desc, strlen(rom->long_desc)+1);
} else {
	write_short(0);
}
	
/* object description */
if(rom->obj_desc) {
	write_short(strlen(rom->obj_desc)+1);  /* # of characters to read in */
	write_chars(rom->obj_desc, strlen(rom->obj_desc)+1);
} else {
	write_short(0);
}

/* exits */
	cnt = count_ext(rom);
	write_int(cnt);
	
	xp = rom->first_ext;
	while(xp) {
		exit = xp->ext;
/* write exit structure */
		write_chars(exit->name, sizeof(exit->name));
		write_short(exit->room);
		write_chars(exit->flags, sizeof(exit->flags));
		write_lasttime(exit->ltime);
		write_char(exit->key);
/* end exit structure */
	
		xp = xp->next_tag;
	}
	
/* monsters */
	cnt = count_mon(rom, perm_only);
	write_int(cnt);
	
	cp = rom->first_mon;
	while(cp) {
		if(!perm_only || (perm_only && cp->crt->flags[0] & 1))
			write_creature(cp->crt);
		cp = cp->next_tag;
	}
	
/* objects */
	cnt = count_ite(rom, perm_only);
	write_int(cnt);
	
	op = rom->first_obj;
	while(op) {
		if(!perm_only || (perm_only && op->obj->flags[0] & 1))
			write_object(op->obj);
		op = op->next_tag;
	}
/* end of room data */
	fprintf(FP, "#endrom\n");
}
	
int write_object(obj)
object	*obj;
{
	int	cnt, perm_only;
	otag	*op;
	
	perm_only = 0;
	
/* begin of complete object data */
	fprintf(FP, "#begOBJ\n");
	
/* basic object format (excluding pointers) */
	write_object_basic(-1, obj);
	
/* objects carried within objects */
	cnt = count_obj(obj, perm_only);
	write_int(cnt);
	
	if(cnt > 0) {
		op = obj->first_obj;
		while(op) {
			if(!perm_only || (perm_only && op->obj->flags[0] & 1))
				write_object(op->obj);
			op = op->next_tag;
		}
	}
	
/* end of complete object */ 
	fprintf(FP, "#endOBJ\n");
}
	
int write_creature(crt)
creature *crt;
{
	int	cnt, perm_only;
	otag	*op;
	
	perm_only = 0;
	
/* begin of complete creature data */
	fprintf(FP, "#begCRT\n");
	
/* basic creature format (excluding pointers) */
	write_creature_basic(-1, crt);
	
/* objects carried by creature */
	cnt = count_inv(crt, perm_only);
	write_int(cnt);
	
	if(cnt > 0) {
		op = crt->first_obj;
		while(op) {
			if(!perm_only || (perm_only && op->obj->flags[0] & 1))
				write_object(op->obj);
			op = op->next_tag;
		}
	}
	
/* end of complete creature */ 
	fprintf(FP, "#endCRT\n");
}
	
int write_player(str, ply)
char	 *str;
creature *ply;
{
	fprintf(FP, "#begPLY\n");
	fprintf(FP, "%s\n", str);  /* write player/file name */
	write_creature(ply);
	fprintf(FP, "#endPLY\n");
}
	
int write_object_basic(index, obj)
int	index;
object	*obj;
{
	int	i;
	
/* begin of object data */
	fprintf(FP, "#begobj\n");
	write_int(index);  /* object # */
	
	write_chars(obj->name, sizeof(obj->name));
	write_chars(obj->description, sizeof(obj->description));
	for (i = 0; i != 3; i++)
		write_chars(obj->key[i], sizeof(obj->key[i]));
	write_chars(obj->use_output, sizeof(obj->use_output));
	write_long(obj->value);
	write_short(obj->weight);
	
	write_char(obj->type);
	write_char(obj->adjustment);
	
	write_short(obj->shotsmax);
	write_short(obj->shotscur);
	write_short(obj->ndice);
	write_short(obj->sdice);
	write_short(obj->pdice);
	
	write_char(obj->armor);
	write_char(obj->wearflag);
	write_char(obj->magicpower);
	write_char(obj->magicrealm);
	write_short(obj->special);
	write_chars(obj->flags, sizeof(obj->flags));
	write_char(obj->questnum);
	
/* end of object data */
	fprintf(FP, "#endobj\n");
}
	
int write_creature_basic(index, crt)
int		index;
creature	*crt;
{
	int	i;
	
/* begin of creature data */
	fprintf(FP, "#begcrt\n");
	write_int(index);  /* creature # */
	
	write_chars(crt->name, sizeof(crt->name));
	write_chars(crt->description, sizeof(crt->description));
	write_chars(crt->talk, sizeof(crt->talk));
	write_chars(crt->password, sizeof(crt->password));
	for (i = 0; i != 3; i++)
		write_chars(crt->key[i], sizeof(crt->key[i]));
	
	write_short(crt->fd);
	
	write_char(crt->level);
	write_char(crt->type);
	write_char(crt->class);
	write_char(crt->race);
	write_char(crt->numwander);
	
	write_short(crt->alignment);
	
	write_char(crt->strength);
	write_char(crt->dexterity);
	write_char(crt->constitution);
	write_char(crt->intelligence);
	write_char(crt->piety);
	
	write_short(crt->hpmax);
	write_short(crt->hpcur);
	write_short(crt->mpmax);
	write_short(crt->mpcur);
	
	write_char(crt->armor);
	write_char(crt->thaco);
	
	write_long(crt->experience);
	write_long(crt->gold);
	
	write_short(crt->ndice);
	write_short(crt->sdice);
	write_short(crt->pdice);

	write_short(crt->special);
	
	for (i=0; i != 5; i++)
		write_long(crt->proficiency[i]);
	for (i=0; i < 4; i++)
		write_long(crt->realm[i]);

	write_chars(crt->spells, sizeof(crt->spells));
	write_chars(crt->flags, sizeof(crt->flags));
	write_chars(crt->quests, sizeof(crt->quests));
	write_char(crt->questnum);
	
	for (i=0; i != 10; i++)
		write_short(crt->carry[i]);
	write_short(crt->rom_num);
	
	for (i=0; i != 10; i++) {
		write_char(crt->daily[i].max);
		write_char(crt->daily[i].cur);
		write_long(crt->daily[i].ltime);
	}
	
	for (i=0; i < 45; i++)
		write_lasttime(crt->lasttime[i]);
	
/* end of creature data */
	fprintf(FP, "#endcrt\n");   /* indicate end of creature ascii struct*/
}
	
/**********************************************************************/
/*                              read_room                             */
/**********************************************************************/

/* Loads a room from an already open and positioned file into the memory */
/* pointed to by the second parameter.  Also loaded are all creatures,   */
/* exits, objects and descriptions for the room.  -1 is returned upon    */
/* failure.                                                              */

int read_room(rom)
room    *rom;
{
        int             i, error=0;
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
	fscanf(FP, "%s", str); 
	if (strcmp(str, "#begrom"))
		return(1);
	
	read_short(&rom->rom_num);
	read_chars(rom->name, sizeof(rom->name));
	read_char(&rom->lolevel);
	read_char(&rom->hilevel);
	read_short(&rom->special);
	read_char(&rom->trap);
	read_short(&rom->trapexit);
	read_chars(rom->track, sizeof(rom->track));
	read_chars(rom->flags, sizeof(rom->flags));
	
	for (i = 0; i != 10; i++)
		read_short(&rom->random[i]);
	
	read_char(&rom->traffic);
	
	for (i = 0; i != 10; i++)
		read_lasttime(&rom->perm_mon[i]);
	for (i = 0; i != 10; i++)
		read_lasttime(&rom->perm_obj[i]);
	
	read_long(&rom->beenhere);
	read_long(&rom->established);

/* end basic structure */

        rom->short_desc = 0;
        rom->long_desc  = 0;
	rom->obj_desc = 0;
        rom->first_ext = 0;
        rom->first_obj = 0;
        rom->first_mon = 0;
        rom->first_ply = 0;

/* short description */
	read_int(&cnt);

        if(cnt) {
                sh = (char *)malloc(cnt);
                if(sh) {
			read_chars(sh, cnt);
                        rom->short_desc = sh;
                }
                else
                        merror("read_rom", FATAL);
        }

/* long description */
	read_int(&cnt);

        if(cnt) {
                lo = (char *)malloc(cnt);
                if(lo) {
			read_chars(lo, cnt);
                        rom->long_desc = lo;
                }
                else
                        merror("read_rom", FATAL);
        }

/* object description */
	read_int(&cnt);

        if(cnt) {
                ob = (char *)malloc(cnt);
                if(ob) {
			read_chars(ob, cnt);
                        rom->obj_desc = ob;
                }
                else
                        merror("read_rom", FATAL);
        }

/* exits */
	read_int(&cnt);

        xprev = &rom->first_ext;
        while(cnt > 0) {
                cnt--;
                xp = (xtag *)malloc(sizeof(xtag));
                if(xp) {
                        ext = (exit_ *)malloc(sizeof(exit_));
                        if(ext) {
		/* begin exit structure */
			read_chars(ext->name, sizeof(ext->name));
			read_short(&ext->room);
			read_chars(ext->flags, sizeof(ext->flags));
			read_lasttime(&ext->ltime);
			read_char(&ext->key);
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
	read_int(&cnt);

        cprev = &rom->first_mon;
        while(cnt > 0) {
                cnt--;
                cp = (ctag *)malloc(sizeof(ctag));
                if(cp) {
                        crt = (creature *)malloc(sizeof(creature));
                        if(crt) {
                                if(read_creature(crt) < 0)
                                        error = 1;
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
	read_int(&cnt);

        oprev = &rom->first_obj;
        while(cnt > 0) {
                cnt--;
                op = (otag *)malloc(sizeof(otag));
                if(op) {
                        obj = (object *)malloc(sizeof(object));
                        if(obj) {
                                if(read_object(obj) < 0)
                                        error = 1;
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

	fscanf(FP, "%s", str); 
	if (strcmp(str, "#endrom"))
		return(1);

        if(error)
                return(1);
        else
                return(0);
}

int read_player(name, ply)
char	 	*name;
creature	*ply;
{
	char	 str[10];

	fscanf(FP, "%s", str); 
	if (strcmp(str, "#begPLY"))
		return(1);

	fscanf(FP, "%s\n", name);  /* write player/file name */
	if(read_creature(ply))
		return(1);

	fscanf(FP, "%s", str); 
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

int read_object(obj)
object  *obj;
{
        int             cnt, error=0;
        int		temp;
        otag            *op;
        otag            **prev;
        object          *tobj;
	char		str[10];

	fscanf(FP, "%s", str); 
	if (strcmp(str, "#begOBJ"))
		return(1);

        read_object_basic(&temp, obj);

        obj->first_obj = 0;
        obj->parent_obj = 0;
        obj->parent_rom = 0;
        obj->parent_crt = 0;
        if(obj->shotscur > obj->shotsmax)
                obj->shotscur = obj->shotsmax;

/* get # of objects to read in */
        read_int(&cnt);

        prev = &obj->first_obj;
        while(cnt > 0) {
                cnt--;
                op = (otag *)malloc(sizeof(otag));
                if(op) {
                        tobj = (object *)malloc(sizeof(object));
                        if(tobj) {
                                if(read_object(tobj))
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
	fscanf(FP, "%s", str); 
	if (strcmp(str, "#endOBJ"))
		return(1);

        if(error)
                return(1);
        else
                return(0);
}

/**********************************************************************/
/*                              read_creature                         */
/**********************************************************************/

/* Loads a creature from an open and positioned file.  The creature is */
/* loaded at the mem location specified by the second parameter.  In   */
/* addition, all the creature's objects have memory allocated for them */
/* and are loaded as well.  Returns -1 on fail.                        */

int read_creature(crt)
creature        *crt;
{
        int             n, cnt, error=0;
        int		temp;
        otag            *op;
        otag            **prev;
        object          *obj;
	char		str[10];

	fscanf(FP, "%s", str);
	if (strcmp(str, "#begCRT"))
		return(1);

        read_creature_basic(&temp, crt);

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
        read_int(&cnt);

        prev = &crt->first_obj;
        while(cnt > 0) {
                cnt--;
                op = (otag *)malloc(sizeof(otag));
                if(op) {
                        obj = (object *)malloc(sizeof(object));
			zero(obj, sizeof(object));
                        if(obj) {
				if (read_object(obj)) return(1);
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

	fscanf(FP, "%s", str);
	if (strcmp(str, "#endCRT"))
		return(1);

        if(error)
                return(1);
        else
                return(0);
}

int read_object_basic(index, obj)
int	*index;
object	*obj;
{
	int	i;
	char	str[10];
	
/* begin of object data */
	fscanf(FP, "%s", str); 
	if (strcmp(str, "#begobj"))
		return(1);
	read_int(index);  /* object # */
	
	read_chars(obj->name, sizeof(obj->name));
	read_chars(obj->description, sizeof(obj->description));
	for (i = 0; i != 3; i++)
		read_chars(obj->key[i], sizeof(obj->key[i]));
	read_chars(obj->use_output, sizeof(obj->use_output));
	read_long(&obj->value);
	read_short(&obj->weight);
	
	read_char(&obj->type);
	read_char(&obj->adjustment);
	
	read_short(&obj->shotsmax);
	read_short(&obj->shotscur);
	read_short(&obj->ndice);
	read_short(&obj->sdice);
	read_short(&obj->pdice);
	
	read_char(&obj->armor);
	read_char(&obj->wearflag);
	read_char(&obj->magicpower);
	read_char(&obj->magicrealm);
	read_short(&obj->special);
	read_chars(obj->flags, sizeof(obj->flags));
	read_char(&obj->questnum);
	
/* end of object data */
	fscanf(FP, "%s", str); 
	if (strcmp(str, "#endobj"))
		return(1);
	return(0);
}
	
int read_creature_basic(index, crt)
int		*index;
creature	*crt;
{
	int	i;
	char	str[10];
	
/* begin of creature data */
	fscanf(FP, "%s", str); 
	if (strcmp(str, "#begcrt"))
		return(1);

	read_int(index);  /* creature # */
	
	read_chars(crt->name, sizeof(crt->name));
	read_chars(crt->description, sizeof(crt->description));
	read_chars(crt->talk, sizeof(crt->talk));
	read_chars(crt->password, sizeof(crt->password));
	for (i = 0; i != 3; i++)
		read_chars(crt->key[i], sizeof(crt->key[i]));
	
	read_short(&crt->fd);
	
	read_char(&crt->level);
	read_char(&crt->type);
	read_char(&crt->class);
	read_char(&crt->race);
	read_char(&crt->numwander);
	
	read_short(&crt->alignment);
	
	read_char(&crt->strength);
	read_char(&crt->dexterity);
	read_char(&crt->constitution);
	read_char(&crt->intelligence);
	read_char(&crt->piety);
	
	read_short(&crt->hpmax);
	read_short(&crt->hpcur);
	read_short(&crt->mpmax);
	read_short(&crt->mpcur);
	
	read_char(&crt->armor);
	read_char(&crt->thaco);
	
	read_long(&crt->experience);
	read_long(&crt->gold);
	
	read_short(&crt->ndice);
	read_short(&crt->sdice);
	read_short(&crt->pdice);

	read_short(&crt->special);
	
	for (i=0; i != 5; i++)
		read_long(&crt->proficiency[i]);
	for (i=0; i != 4; i++)
		read_long(&crt->realm[i]);

	read_chars(crt->spells, sizeof(crt->spells));
	read_chars(crt->flags, sizeof(crt->flags));
	read_chars(crt->quests, sizeof(crt->quests));
	read_char(&crt->questnum);
	
	for (i=0; i != 10; i++)
		read_short(&crt->carry[i]);
	read_short(&crt->rom_num);
	
	for (i=0; i != 10; i++) {
		read_char(&crt->daily[i].max);
		read_char(&crt->daily[i].cur);
		read_long(&crt->daily[i].ltime);
	}

	for (i=0; i < 45; i++)
		read_lasttime(&crt->lasttime[i]);
	
/* end of creature data */
	fscanf(FP, "%s", str); 
	if (strcmp(str, "#endcrt"))
		return(1);
	return(0);
}
	
int write_lasttime(lt)
struct lasttime	lt;
{
	write_long(lt.interval);
	write_long(lt.ltime);
	write_short(lt.misc);
}
	
int read_lasttime(lt)
struct lasttime	*lt;
{	
	read_long(&lt->interval);
	read_long(&lt->ltime);
	read_short(&lt->misc);
}

void zero(ptr, size)
void 	*ptr;
int	size;
{
	char 	*chptr;
	int	i;

        chptr = (char *)ptr;
	for(i=0; i<size; i++) 
		chptr[i] = 0;
}

void merror(str, errtype)
char    *str;
char    errtype;
{
        fprintf(stderr, "Error occured in %s.\n", str);
        perror("Error: ");
        if(errtype == FATAL) {
		close(FP);
                exit(-1);
	}
	error = 1;
}

void del_active(crt)
creature *crt;
{ return; }

