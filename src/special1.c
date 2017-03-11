/*
 * SPECIAL1.C:
 *
 *	User routines dealing with special routines.
 *
 *	Copyright (C) 1992, 1993 Brooke Paul
 *
 * $Id: special1.c,v 6.13 2001/03/08 16:09:09 develop Exp $
 *
 * $Log: special1.c,v $
 * Revision 6.13  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

#include "../include/mordb.h"
#include "mextern.h"


/************************************************************************/
/*				special_obj				*/
/************************************************************************/

#define SP_INVENTORY 0
#define SP_EQUIPMENT 1
#define SP_ROOM      2

int special_obj(creature *ply_ptr, cmd *cmnd, int special )
{ 
	object	*obj_ptr;
	room	*rom_ptr;
	int	fd, n, where;
	char	str[80], str2[160], match=0;
	unsigned int i;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
		   cmnd->str[1], cmnd->val[1]);
	where = SP_INVENTORY;

	if(!obj_ptr || !cmnd->val[1]) {
		for(n=0; n<MAXWEAR; n++) {
			if(!ply_ptr->ready[n]) continue;
			if(EQUAL(ply_ptr->ready[n], cmnd->str[1]))
				match++;
			else continue;
			if(match == cmnd->val[1] || !cmnd->val[1]) {
				obj_ptr = ply_ptr->ready[n];
				where = SP_EQUIPMENT;
				break;
			}
		}
	}

	if(!obj_ptr) {
		obj_ptr = find_obj(ply_ptr, rom_ptr->first_obj,
				   cmnd->str[1], cmnd->val[1]);
		where = SP_ROOM;
	}

	if(!obj_ptr) {
		output(fd, "You don't see that.\n");
		return(-1);
	}

	if(obj_ptr->special != special)
		return(-2);

	switch(obj_ptr->special) {
	case SP_MAPSC:
		strcpy(str, obj_ptr->name);
		for(i=0; i<strlen(str); i++)
			if(str[i] == ' ') str[i] = '_';
		sprintf(str2, "%s/%s", get_object_path(), str);
		view_file(fd, 1, str2);
		return(DOPROMPT);
	case SP_COMBO:
		return(combo_box(ply_ptr, obj_ptr));
	default:
		output(fd, "Nothing.\n");
	}

	return(0);
}

/************************************************************************/
/*				special_cmd				*/
/************************************************************************/

int special_cmd(creature *ply_ptr, int special, cmd	*cmnd )
{
	int n, fd;

	fd = ply_ptr->fd;

	switch(special) {
	case SP_MAPSC:
	case SP_COMBO:
		if(cmnd->num < 2) {
			output(fd, "Do that to what?\n");
			return(0);
		}
		n = special_obj(ply_ptr, cmnd, special);
		if(n == -1) return(0);
		if(n == -2) {
			output(fd, "I'd like to see you try.\n");
			return(0);
		}
		else return(n);
	default:
		output(ply_ptr->fd, "Nothing happens.\n");
		return(0);
	}
}

/************************************************************************/
/*				combo_box				*/
/************************************************************************/

int combo_box(creature *ply_ptr, object *obj_ptr )
{
	xtag	*xp;
	room	*rom_ptr;
	char	str[80];
	int	fd, dmg, i;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	str[0] = obj_ptr->sdice+'0'; str[1] = 0;

	if(obj_ptr->ndice == 1 || strlen(Ply[fd].extr->tempstr[3]) > 70)
		strcpy(Ply[fd].extr->tempstr[3], str);
	else
		strcat(Ply[fd].extr->tempstr[3], str);
 
	output(fd, "Click.\n");
	broadcast_rom(fd, ply_ptr->rom_num, "%M presses %i.", 
		m2args(ply_ptr, obj_ptr));

	if(strlen(Ply[fd].extr->tempstr[3]) >= strlen(obj_ptr->use_output)) {
		if(strcmp(Ply[fd].extr->tempstr[3], obj_ptr->use_output)) {
			dmg = mrand(20,40);
			ply_ptr->hpcur -= dmg;
			sprintf(g_buffer, "You were zapped for %d damage!\n", dmg);
			output(fd, g_buffer);
			broadcast_rom(fd, ply_ptr->rom_num,
				"%M was zapped by %i!", m2args(ply_ptr, obj_ptr));
			Ply[fd].extr->tempstr[3][0] = 0;

			if(ply_ptr->hpcur < 1) {
				output(fd, "You died.\n");
				die(ply_ptr, ply_ptr);
			}
		}

		else {
			for(i=1, xp=rom_ptr->first_ext;
				xp && i < obj_ptr->pdice;
				i++, xp = xp->next_tag) ;
			if(!xp) return(0);
			sprintf(g_buffer, "You opened the %s!\n", xp->ext->name);
			output(fd, g_buffer);
			sprintf(g_buffer, "%%M opened the %s!", xp->ext->name);
			broadcast_rom(fd, ply_ptr->rom_num, g_buffer, 
				m1arg(ply_ptr));
			F_CLR(xp->ext, XLOCKD);
			F_CLR(xp->ext, XCLOSD);
			xp->ext->ltime.ltime = time(0);
		}
	}
	return(0);
}


