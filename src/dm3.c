/*
 * DM3.C:
 *
 *	DM functions
 *
 *	Copyright (C) 1991, 1992, 1993 Brooke Paul
 *
 * $Id: dm3.c,v 6.21 2001/04/29 02:30:26 develop Exp $
 *
 * $Log: dm3.c,v $
 * Revision 6.21  2001/04/29 02:30:26  develop
 * changes *lock lock to *lock creation
 *
 * Revision 6.20  2001/04/23 04:14:21  develop
 * added NOCREATE stuff
 *
 * Revision 6.19  2001/04/23 04:06:47  develop
 * added NOCREATE
 *
 * Revision 6.18  2001/04/12 05:00:57  develop
 * removed MAXSPELLS in favor of get_spell_list_size()
 *
 * Revision 6.17  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

#include "../include/mordb.h"
#include "mextern.h"

/**********************************************************************/
/*				dm_set				      */
/**********************************************************************/

/* This function allows a DM to set a variable within a currently */
/* existing data structure in the game.				  */

int dm_set(creature	*ply_ptr, cmd *cmnd )
{
	int	fd;

	fd = ply_ptr->fd;

	if(ply_ptr->class < BUILDER)
		return(PROMPT);

	if(cmnd->num < 2) {
		output(fd, "Set what?\n");
		return(0);
	}

	switch(low(cmnd->str[1][0])) {
	case 'x': 
		return(dm_set_ext(ply_ptr, cmnd));
	case 'r': 
		return(dm_set_rom(ply_ptr, cmnd));
	case 'c':
	case 'p':
	case 'm': 
		return(dm_set_crt(ply_ptr, cmnd));
	case 'i':
	case 'o': 
		return(dm_set_obj(ply_ptr, cmnd));
	default: 
		output(fd, "Invalid option.  *set <x|r|c|o> <options>\n");
		return(0);
	}

}

/**********************************************************************/
/*				dm_set_rom			      */
/**********************************************************************/

/* This function allows a DM to set a characteristic of a room. */

int dm_set_rom(creature	*ply_ptr, cmd *cmnd )
{
	room	*rom_ptr;
	int	num, fd;

	rom_ptr = ply_ptr->parent_rom;
	fd = ply_ptr->fd;

	if(cmnd->num < 3) {
		output(fd, "Syntax: *set r [xbtrfd] [<value>]\n");
		return(0);
	}


	if(ply_ptr->class < BUILDER) {
		return(0);
        }

	switch(low(cmnd->str[2][0])) {
	case 't':
		rom_ptr->traffic = (char) cmnd->val[2];
		sprintf(g_buffer, "Traffic is now %d%%.\n", (int)cmnd->val[2]);
		output(fd, g_buffer);
		return(0);
	case 'r':
		num = atoi(&cmnd->str[2][1]);
		if(num < 1 || num > 10) 
			return(PROMPT);
		rom_ptr->random[num-1] = (short) cmnd->val[2];
		sprintf(g_buffer, "Random #%d is now %d.\n", num, (int)cmnd->val[2]);
		output(fd, g_buffer);
		return(0);
	case 'f':
		num = cmnd->val[2];
		if(num < 1 || num > 64) return(PROMPT);
		if(F_ISSET(rom_ptr, num-1)) {
			F_CLR(rom_ptr, num-1);
			sprintf(g_buffer, "Room flag #%d off.\n", num);
		}
		else {
			F_SET(rom_ptr, num-1);	
			sprintf(g_buffer, "Room flag #%d on.\n", num);
		}
		output(fd, g_buffer );
		return(0);
	case 'b':
		if (low(cmnd->str[2][1]) == 'l') {
			rom_ptr->lolevel = (char) cmnd->val[2];
			sprintf(g_buffer, "Low level boundary %d\n", (int)cmnd->val[2]);
			output(fd, g_buffer);
		}
		else if (low(cmnd->str[2][1]) == 'h') {
			rom_ptr->hilevel = (char) cmnd->val[2];
			sprintf(g_buffer, "Upper level boundary %d\n", (int)cmnd->val[2]);
			output(fd, g_buffer);
		}
		return(0);
	case 'x':
		rom_ptr->trap = (char) cmnd->val[2];
		sprintf(g_buffer, "Room has trap #%d set.", (int)cmnd->val[2]);
		output(fd, g_buffer);
		return(0);
	case 'd':
		rom_ptr->death_rom = cmnd->val[2];
		sprintf(g_buffer, "Death room set to: %d.\n", (int)cmnd->val[2]);
		output(fd, g_buffer);
		return(0);
	default:
		output(fd, "Invalid option.\n");
		return(0);
	}
}

/**********************************************************************/
/*				dm_set_crt			      */
/**********************************************************************/

/* This function allows a DM to set a defining characteristic of */
/* a creature or player						 */

int dm_set_crt(creature	*ply_ptr, cmd *cmnd )
{
	room		*rom_ptr;
	creature	*crt_ptr;
	int		n, num, fd, objnum, crtloaded=0;
	object		*obj_ptr;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(cmnd->num < 4) {
		output(fd, "Syntax: *set c <name> <a|con|c|dex|e|f|g|hm|h|int|l|mm|m|\n                       pie|p#|r#|str> [<value>]\n");
		return(0);
	}
	
	if(ply_ptr->class < DM) {
	    output(fd, "Check with a Dungeonmaster to get permission.\n");
        return(0);
        }

	cmnd->str[2][0] = up(cmnd->str[2][0]);
	crt_ptr = find_who(cmnd->str[2]);
	if(!crt_ptr || (ply_ptr->class<BUILDER && F_ISSET(crt_ptr, PDMINV)))
		crt_ptr = find_crt_in_rom(ply_ptr, rom_ptr,
				   cmnd->str[2], cmnd->val[2], PLY_FIRST);
	if(!crt_ptr) {
		cmnd->str[2][0] = up(cmnd->str[2][0]);
		if(load_ply(cmnd->str[2], &crt_ptr) < 0){
                	output(fd,"Player does not exist.\n");
                	return (0);
       	 	}
		crtloaded=1;
	}

	switch(low(cmnd->str[3][0])) {
	case 'a':
		if (!strcmp(cmnd->str[3], "ac") && 
			crt_ptr->type == MONSTER){
			crt_ptr->armor = (char ) cmnd->val[3];
			sprintf(crt_ptr->password, "%d", 0);
			output(fd, "Armor Class set.\n");
			return(PROMPT);

		}
		crt_ptr->alignment = (short) cmnd->val[3];
		output(fd, "Alignment set.\n");
		if(crt_ptr->type==MONSTER)
			sprintf(crt_ptr->password, "%d", 0);
		if(crtloaded) {save_ply(crt_ptr); free_crt(crt_ptr);}
		return(PROMPT);
	case 'c':
		if(!strcmp(cmnd->str[3], "con")) {
			crt_ptr->constitution = (char) cmnd->val[3];
			output(fd, "Constitution set.\n");
			if(crt_ptr->type==MONSTER)
				sprintf(crt_ptr->password, "%d", 0);
			if(crtloaded) {save_ply(crt_ptr); free_crt(crt_ptr);}
			return(PROMPT);
		}

		if(ply_ptr->class < DM) {
			if(crtloaded) free_crt(crt_ptr);
                	return(PROMPT);
		}
				
		crt_ptr->class = (char)cmnd->val[3];
		if (crt_ptr->type == PLAYER && cmnd->val[3] == DM)
        if(strcmp(crt_ptr->name, dmname[0]) &&
           strcmp(crt_ptr->name, dmname[1]) && strcmp(crt_ptr->name, dmname[2]) &&
           strcmp(crt_ptr->name, dmname[3]) && strcmp(crt_ptr->name, dmname[4]) &&
           strcmp(crt_ptr->name, dmname[5]) && strcmp(crt_ptr->name, dmname[6]))
                crt_ptr->class = BUILDER;      
		output(fd, "Class set.\n");
		if(crt_ptr->type==MONSTER)
			sprintf(crt_ptr->password, "%d", 0);
		if(crtloaded) {save_ply(crt_ptr); free_crt(crt_ptr);}
		return(PROMPT);
	case 'd':
		if(!strcmp(cmnd->str[3], "dex")) {
			crt_ptr->dexterity = (char) cmnd->val[3];
			output(fd, "Dexterity set.\n");
			if(crt_ptr->type==MONSTER)
				sprintf(crt_ptr->password, "%d", 0);
			if(crtloaded) {save_ply(crt_ptr); free_crt(crt_ptr);}
			return(PROMPT);
		} 
		else if (!strcmp(cmnd->str[3], "dn") && 
			crt_ptr->type == MONSTER){
			crt_ptr->ndice = (short) cmnd->val[3];
			output(fd, "Number of dice set.\n");
			sprintf(crt_ptr->password, "%d", 0);
			return(PROMPT);
		}
		else if (!strcmp(cmnd->str[3], "ds") && 
			crt_ptr->type == MONSTER){
			crt_ptr->sdice = (short) cmnd->val[3];
			output(fd, "Sides of dice set.\n");
			sprintf(crt_ptr->password, "%d", 0);
			return(PROMPT);
		}
		else if (!strcmp(cmnd->str[3], "dp") && 
			crt_ptr->type == MONSTER){
			crt_ptr->pdice = (short) cmnd->val[3];
			output(fd, "Plus on dice set.\n");
			sprintf(crt_ptr->password, "%d", 0);
			return(PROMPT);
		}
		break;
	case 'e':
		crt_ptr->experience = cmnd->val[3];
		sprintf(g_buffer, "%%M has %ld experience.\n", cmnd->val[3]);
		mprint(fd, g_buffer, m1arg(crt_ptr));
		if(crt_ptr->type==MONSTER)
			sprintf(crt_ptr->password, "%d", 0);
		if(crtloaded) {save_ply(crt_ptr); free_crt(crt_ptr);}
		return(0);
	case 'f':
		num = cmnd->val[3];
		if(num < 1 || num > 64) {
			if(crtloaded) {save_ply(crt_ptr); free_crt(crt_ptr);}
			return(PROMPT);
		}
		if(F_ISSET(crt_ptr, num-1)) {
			F_CLR(crt_ptr, num-1);
			sprintf(g_buffer, "%%M's flag #%d off.\n", num);
			mprint(fd, g_buffer, m1arg(crt_ptr));
		}
		else {
			F_SET(crt_ptr, num-1);
			sprintf(g_buffer, "%%M's flag #%d on.\n", num);
			mprint(fd, g_buffer, m1arg(crt_ptr));
		}

		if(crt_ptr->type==MONSTER)
			sprintf(crt_ptr->password, "%d", 0);
		if(crtloaded) {save_ply(crt_ptr); free_crt(crt_ptr);}
		return(0);
	case 'g':
		crt_ptr->gold = cmnd->val[3];
		sprintf(g_buffer, "%%M has %ld gold.\n", cmnd->val[3]);
		mprint( fd, g_buffer, m1arg(crt_ptr) );
		if(crt_ptr->type==MONSTER)
			sprintf(crt_ptr->password, "%d", 0);
		if(crtloaded) {save_ply(crt_ptr); free_crt(crt_ptr);}
		return(0);
	case 'h':
		if(cmnd->str[3][1] == 'm'){
			crt_ptr->hpmax = (short) cmnd->val[3];
			output(fd, "Hits max set.\n");
		}
		else {
			crt_ptr->hpcur = (short) cmnd->val[3];
			output(fd, "Hits set.\n");
		}
		if(crt_ptr->type==MONSTER)
			sprintf(crt_ptr->password, "%d", 0);
		if(crtloaded) {save_ply(crt_ptr); free_crt(crt_ptr);}
		return(PROMPT);
	case 'i':
		if(!strcmp(cmnd->str[3], "int")) {
			crt_ptr->intelligence = (char) cmnd->val[3];
			output(fd, "Intellegence set.\n");
			if(crt_ptr->type==MONSTER)
				sprintf(crt_ptr->password, "%d", 0);
			if(crtloaded) {save_ply(crt_ptr); free_crt(crt_ptr);}
			return(PROMPT);
		}
		if(!strcmp(cmnd->str[3], "inv")&&crt_ptr->type==MONSTER) {
			if(!strcmp(cmnd->str[4], "d")) {                                        /* MJK 2-21-00 */
 				for(n=0; n<10; n++)
 	   				crt_ptr->carry[n]=0;
				sprintf(g_buffer, "All inventory slots cleared.\n");
				output(fd, g_buffer);
				sprintf(crt_ptr->password, "%d", 0);
				return(PROMPT);
			} 
			if(cmnd->val[3] < 1) {
				obj_ptr=find_obj(ply_ptr, ply_ptr->first_obj,cmnd->str[4],1);
				if(!obj_ptr) {
					output(fd, "You are not carrying that.\n");
					return(PROMPT);
 					}
				objnum=find_obj_num(obj_ptr);
				for(n=0; n<10; n++)
 	   				crt_ptr->carry[n]=objnum;
				sprintf(g_buffer, "All slots set to object number %d.\n", objnum);
				output(fd, g_buffer);
				sprintf(crt_ptr->password, "%d", 0);
				return(PROMPT);
			} 
			if(cmnd->val[3] > 10 || cmnd->val[3] < 1) {
				output(fd, "Inventory slot number invalid.\n");
				output(fd, "Must be from 1 to 10.\n");
				return(PROMPT);
			}
			if(!strcmp(cmnd->str[4], "")) {
				crt_ptr->carry[cmnd->val[3]-1]=0;
				sprintf(g_buffer, "Inventory slot %d cleared.\n", (int)cmnd->val[3]);
				output(fd, g_buffer);
				if(crt_ptr->type==MONSTER)
					sprintf(crt_ptr->password, "%d", 0);
				return(PROMPT);
			} 
			obj_ptr=find_obj(ply_ptr, ply_ptr->first_obj,cmnd->str[4],1);
			if(!obj_ptr) {
				output(fd, "You are not carrying that.\n");
				return(PROMPT);
			}
			objnum=find_obj_num(obj_ptr);
			crt_ptr->carry[cmnd->val[3]-1]=objnum;
			sprintf(g_buffer, "Carry slot %d set to object number %d.\n", (int)cmnd->val[3], objnum);
			output(fd, g_buffer);
			if(crt_ptr->type==MONSTER)
				sprintf(crt_ptr->password, "%d", 0);
			return(PROMPT);
		}
	case 'l':
		crt_ptr->level = (char)cmnd->val[3];
		output(fd, "Level set.\n");
		if(crt_ptr->type==MONSTER)
			sprintf(crt_ptr->password, "%d", 0);
		if(crtloaded) {save_ply(crt_ptr); free_crt(crt_ptr);}
		return(PROMPT);
	case 'm':
		if(!strcmp(cmnd->str[3], "mag")&&crt_ptr->type==MONSTER) {
			if(!strcmp(cmnd->str[4], "d")) {                                        /* MJK 2-21-00 */
				for(n=0;n<get_spell_list_size();n++)
        				S_CLR(crt_ptr, n);                                                                                                          
				sprintf(g_buffer, "All magic spells cleared.\n");
				output(fd, g_buffer);
				sprintf(crt_ptr->password, "%d", 0);
				return(PROMPT);
			} 
			if(cmnd->val[3] > 0 && cmnd->val[3] <= get_spell_list_size() ) {
				objnum=cmnd->val[3];
        			S_SET(crt_ptr, objnum-1);                                                                                                          
				sprintf(g_buffer, "Magic spell %d set.\n", objnum);
				output(fd, g_buffer);
				return(PROMPT);
			}
			output(fd, "Magic spell number out of range.\n");
			if(crt_ptr->type==MONSTER)
				sprintf(crt_ptr->password, "%d", 0);
			return(PROMPT);
		}
		if(!strcmp(cmnd->str[3], "mm")) {
			crt_ptr->mpmax = (short) cmnd->val[3];
			output(fd, "Magic pts max set.\n");
		}
		if(!strcmp(cmnd->str[3], "mp")) {
			crt_ptr->mpcur = (short) cmnd->val[3];
			output(fd, "Magic pts set.\n");
		}
		if(crt_ptr->type==MONSTER)
			sprintf(crt_ptr->password, "%d", 0);
		if(crtloaded) {save_ply(crt_ptr); free_crt(crt_ptr);}
		return(PROMPT);
	case 'p':
		if(!strcmp(cmnd->str[3], "pas") && crt_ptr->type==PLAYER) {
			sprintf(crt_ptr->password, "%s", cmnd->str[4]);
			output(fd, "Password set.\n");
			if(crtloaded) {save_ply(crt_ptr); free_crt(crt_ptr);}
                        return(PROMPT);
                }
		if(!strcmp(cmnd->str[3], "pie")) {
			crt_ptr->piety = (char) cmnd->val[3];
			output(fd, "Piety set.\n");
			if(crt_ptr->type==MONSTER)
				sprintf(crt_ptr->password, "%d", 0);
			if(crtloaded) {save_ply(crt_ptr); free_crt(crt_ptr);}
			return(PROMPT);
		}
		num = atoi(&cmnd->str[3][1]);
		if(num < 0 || num > 4) {
			if(crtloaded) {save_ply(crt_ptr); free_crt(crt_ptr);}
			return(0);
		}
		crt_ptr->proficiency[num] = cmnd->val[3];

		sprintf(g_buffer, "%%M given %d shots in prof#%d.\n",  
		      (int)cmnd->val[3], num);
		mprint(fd, g_buffer, m1arg(crt_ptr)); 
		
		if(crt_ptr->type==MONSTER)
			sprintf(crt_ptr->password, "%d", 0);
		if(crtloaded) {save_ply(crt_ptr); free_crt(crt_ptr);}
		return(0);
	case 'r':
		if(!strcmp(cmnd->str[3], "roo")) {
			if(!crtloaded) {
				output(fd, "Currently logged in.  Use *teleport.\n");
				return(0);
			}
			crt_ptr->rom_num = (short) cmnd->val[3];
			output(fd, "Player moved.\n");
			save_ply(crt_ptr); 
			free_crt(crt_ptr);
			return(PROMPT);
		}
		if(!cmnd->str[3][1]) {
			crt_ptr->race = (char) cmnd->val[3];
			output(fd, "Race set.\n");
			if(crt_ptr->type==MONSTER)
				sprintf(crt_ptr->password, "%d", 0);
			if(crtloaded) {save_ply(crt_ptr); free_crt(crt_ptr);}
			return(PROMPT);
		}
		num = atoi(&cmnd->str[3][1]);
		if(num < 0 || num > 3) {
			if(crtloaded) {save_ply(crt_ptr); free_crt(crt_ptr);}
			return(PROMPT);
		}
		crt_ptr->realm[num] = cmnd->val[3];
		sprintf(g_buffer, "%%M given %d shots in realm#%d.\n", 
			(int)cmnd->val[3], num);
		mprint(fd, g_buffer, m1arg(crt_ptr));

		if(crt_ptr->type==MONSTER)
			sprintf(crt_ptr->password, "%d", 0);
		if(crtloaded) {save_ply(crt_ptr); free_crt(crt_ptr);}
		return(0);
	case 's':
		if(!strcmp(cmnd->str[3], "str")) {
			crt_ptr->strength = (char) cmnd->val[3];
			output(fd, "Strength set.\n");
			if(crt_ptr->type==MONSTER)
				sprintf(crt_ptr->password, "%d", 0);
			if(crtloaded) {save_ply(crt_ptr); free_crt(crt_ptr);}
			return(PROMPT);
		}
		break;
	case 't':
		if (!strcmp(cmnd->str[3], "thac") && 
			crt_ptr->type == MONSTER){
			crt_ptr->thaco = (char) cmnd->val[3];
			output(fd, "THAC0 set.\n");
			sprintf(crt_ptr->password, "%d", 0);
			return(PROMPT);
		}
		break;
	}
	output(fd, "Invalid option.\n");
	if(crtloaded) {save_ply(crt_ptr); free_crt(crt_ptr);}
	return(0);
}

/************************************************************************/
/*			dm_set_ext()					*/
/************************************************************************/

int dm_set_ext(creature	*ply_ptr, cmd *cmnd )
{
	room	*rom_ptr, *rom_ptr2;
	int		fd;

	fd = ply_ptr->fd;

	if (low(cmnd->str[1][1]) == 'f')
	{
		dm_set_xflg(ply_ptr, cmnd);
		return(0);
	}

	if (low(cmnd->str[1][1]) == 'k')
	{
		if(ply_ptr->class < CARETAKER) 
		{
			output(fd, "You must be a caretaker or higher to set exit keys\n");
			return(PROMPT);
		}

        dm_set_xkey(ply_ptr, cmnd);
        return(0);
	}

	if(cmnd->num < 3) {
		output(fd, "Syntax: *set [#] x <name> <#> [. or name]\n");
		return(0);
	}

	if(cmnd->val[1] == 1)
		rom_ptr = ply_ptr->parent_rom;
	else
		if(load_rom(cmnd->val[1], &rom_ptr) < 0) {
			sprintf(g_buffer, "Room %d does not exist.\n", (int)cmnd->val[1]);
			output(fd, g_buffer);
			return(0);
		}

	cmnd->str[2][24] = 0;
	expand_exit_name(cmnd->str[2]);

	if(cmnd->val[2] == 1 && cmnd->fullstr[strlen(cmnd->fullstr) - 1] != '1') 
	{
		output(fd, "Link exit to which room?\n");
		return(0);
	}
	else if(cmnd->val[2] == 0) {
		if(del_exit(&rom_ptr, cmnd->str[2]))
		{
			sprintf(g_buffer, "Exit %s deleted.\n", cmnd->str[2]);
		}
		else
		{
			sprintf(g_buffer, "Exit %s not found.\n", cmnd->str[2]);
		}
		output(fd, g_buffer );
	}
	else if(cmnd->num > 3) {
		if(*cmnd->str[3] == '.')
			if(!opposite_exit_name(cmnd->str[2], cmnd->str[3]))
				strcpy(cmnd->str[3], cmnd->str[2]);
		link_rom(&rom_ptr, (short)cmnd->val[2], cmnd->str[2]);
		if(load_rom(cmnd->val[2], &rom_ptr2) < 0)
		{
			sprintf(g_buffer, "Room %d does not exist.\n", (int)cmnd->val[1]);
			output(fd, g_buffer);
		}
		else 
		{
			link_rom(&rom_ptr2, rom_ptr->rom_num, cmnd->str[3]);
			resave_rom(rom_ptr2->rom_num);
		}

		sprintf(g_buffer, "Room #%d linked to room #%d in %s direction, both ways.\n", rom_ptr->rom_num, (int)cmnd->val[2], cmnd->str[2]);
		output(fd, g_buffer);
	}
	else {
		link_rom(&rom_ptr, (short)cmnd->val[2], cmnd->str[2]);
		sprintf(g_buffer, "Room #%d linked to room #%d in %s direction.\n",
			rom_ptr->rom_num, (int)cmnd->val[2], cmnd->str[2]);
		output(fd, g_buffer);
	}

	resave_rom(rom_ptr->rom_num);

	log_dm_command(ply_ptr->name, cmnd->fullstr);

	return(0);
}

/************************************************************************/
/*				dm_set_obj				*/
/************************************************************************/

int dm_set_obj( creature *ply_ptr, cmd *cmnd )
{
	room		*rom_ptr;
	creature	*crt_ptr;
	object		*obj_ptr;
	int		n, fd, match=0;
	long		num;
	char		flags[25];

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(cmnd->num < 4) {
		output(fd, "Syntax: *set o <name> [<crt>] <ad|ar|dn|ds|dp|f|m|q|sm|s|t|v|wg|wr> [<value>]\n");
		return(0);
	}

	if(ply_ptr->class < DM) {
                 output(fd, "Check with a Dungeonmaster for permission.\n");
                 return(0);
         }

	if(cmnd->num == 4) {
		strcpy(flags, cmnd->str[3]);
		num = cmnd->val[3];
		obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj, cmnd->str[2],
			cmnd->val[2]);
		if(!obj_ptr)
			obj_ptr = find_obj(ply_ptr, rom_ptr->first_obj,
				cmnd->str[2], cmnd->val[2]);
		if(!obj_ptr) {
			output(fd, "Object not found.\n");
			return(0);
		}
	}

	else if(cmnd->num == 5) {
		strcpy(flags, cmnd->str[4]);
		num = cmnd->val[4];
		cmnd->str[3][0] = up(cmnd->str[3][0]);
		crt_ptr = find_who(cmnd->str[3]);
		if(!crt_ptr) {

			crt_ptr = find_crt_in_rom(ply_ptr, rom_ptr,
				cmnd->str[3], cmnd->val[3], MON_FIRST);
		}
		if(!crt_ptr) {
			output(fd, "Creature not found.\n");
			return(0);
		}

		obj_ptr = find_obj(ply_ptr, crt_ptr->first_obj,
			cmnd->str[2], cmnd->val[2]);
		if(!obj_ptr || !cmnd->val[2]) {
			for(n=0; n<MAXWEAR; n++) {
				if(!crt_ptr->ready[n]) continue;
				if(EQUAL(crt_ptr->ready[n], cmnd->str[2]))
					match++;
				else continue;
				if(cmnd->val[1] == match || !cmnd->val[2]) {
					obj_ptr = crt_ptr->ready[n];
					break;
				}
			}
		}
		if(!obj_ptr) {
			output(fd, "Object not found.\n");
			return(0);
		}
	}

	switch(flags[0]) {
	case 'a':
		switch(flags[1]) {
		case 'd': obj_ptr->adjustment = (char) num; output(fd, "Adjustment set.\n"); return(PROMPT);
		case 'r': obj_ptr->armor = (char) num; output(fd, "Armor set.\n");return(PROMPT);
		}
		break;
	case 'c':
		obj_ptr->constitution = (short) num;
		if(num > 0)
			F_SET(obj_ptr, OADDSA);
		else
			F_CLR(obj_ptr, OADDSA);
		output(fd, "Object constitution set.\n");
		log_dm_command(ply_ptr->name, cmnd->fullstr);
		return(PROMPT);	
	case 'd':
		switch(flags[1]) {
		case 'n': obj_ptr->ndice = (short) num; output(fd, "Dice # set.\n");return(PROMPT);
		case 's': obj_ptr->sdice = (short) num; output(fd, "Dice sides set.\n");return(PROMPT);
		case 'p': obj_ptr->pdice = (short) num; output(fd, "Dice plus set.\n");return(PROMPT);
		case 'e':
			obj_ptr->dexterity = (short) num;
			if(num > 0)
				F_SET(obj_ptr, OADDSA);
			else
				F_CLR(obj_ptr, OADDSA);
			output(fd, "Object dexterity set.\n");
			log_dm_command(ply_ptr->name, cmnd->fullstr);
			return(PROMPT);	
		}
		break;
	case 'f':
		if(num < 1 || num > 128) return(PROMPT);
		if(flags[1] == 's') {
		   if(!FS_ISSET(obj_ptr, num-1)) {
			FS_SET(obj_ptr, num-1);	
			sprintf(g_buffer,"Object sets flag %d.\n",(int)num);
			output(fd,g_buffer);
			return(0);
		   }
		   else {
			FS_CLR(obj_ptr, num-1);	
			sprintf(g_buffer,"Object does not set flag %d.\n",(int)num);
			output(fd,g_buffer);
			return(0);
		   }
		}
		if(F_ISSET(obj_ptr, num-1)) {
			F_CLR(obj_ptr, num-1);
			sprintf(g_buffer, "%%I's flag #%d off.\n", (int)num);
			mprint(fd, g_buffer, m1arg(obj_ptr));
		}
		else {
			F_SET(obj_ptr, num-1);
			sprintf(g_buffer, "%%I's flag #%d on.\n", (int)num);
			mprint(fd, g_buffer, m1arg(obj_ptr));
		}
		
		return(0);
	case 'i':
		obj_ptr->intelligence = (short) num;
		if(num > 0)
			F_SET(obj_ptr, OADDSA);
		else
			F_CLR(obj_ptr, OADDSA);
		output(fd, "Object intelligence set.\n");
		log_dm_command(ply_ptr->name, cmnd->fullstr);
		return(PROMPT);	
	case 'm':
		obj_ptr->magicpower = (char) num;
		output(fd, "Magic power set.\n");
		return(PROMPT);
	case 'p':
		obj_ptr->piety = (short) num;
		if(num > 0)
			F_SET(obj_ptr, OADDSA);
		else
			F_CLR(obj_ptr, OADDSA);
		output(fd, "Object piety set.\n");
		log_dm_command(ply_ptr->name, cmnd->fullstr);
		return(PROMPT);	
	case 'q':
		if(!strcmp(ply_ptr->name, dmname[0]) || 
		   !strcmp(ply_ptr->name, dmname[1]) ||
		   !strcmp(ply_ptr->name, dmname[2])) {
			obj_ptr->questnum = (char) num;
			output(fd, "Quest number set.\n");
		}
		return(PROMPT);
	case 's':
		if(flags[1] == 'm'){
			obj_ptr->shotsmax = (short) num;
			output(fd, "Max shots set.\n");
		}
		else if(flags[1] == 'p'){
			obj_ptr->special = (short) num;
			output(fd, "Object special set.\n");
		}
		else if(flags[1] == 't'){
			obj_ptr->strength = (short) num;
			if(num > 0)
				F_SET(obj_ptr, OADDSA);
			else
				F_CLR(obj_ptr, OADDSA);
			output(fd, "Object strength set.\n");
			log_dm_command(ply_ptr->name, cmnd->fullstr);
		}
		else{
			obj_ptr->shotscur = (short) num;
			output(fd, "Current shots set.\n");
		}
		return(PROMPT);
	case 't':
		if ((num<0 || num>14) && ply_ptr->class<DM) return(PROMPT);
		switch(num) {
		case SHARP: obj_ptr->type = SHARP; output(fd, "Object is a sharp weapon.\n");return(PROMPT);
		case THRUST: obj_ptr->type = THRUST; output(fd, "Object is a thrust weapon.\n");return(PROMPT);
		case BLUNT: obj_ptr->type = BLUNT; output(fd, "Object is a blunt weapon.\n");return(PROMPT);
		case POLE: obj_ptr->type = POLE; output(fd, "Object is a pole weapon.\n");return(PROMPT);
		case MISSILE: obj_ptr->type = MISSILE; output(fd, "Object is a missile weapon.\n");return(PROMPT);
		case ARMOR: obj_ptr->type = ARMOR; output(fd, "Object is armor.\n");return(PROMPT);
		case POTION:obj_ptr->type = POTION; output(fd, "Object is a potion.\n");return(PROMPT);
        case SCROLL:obj_ptr->type = SCROLL; output(fd, "Object is a scroll.\n");return(PROMPT);
       	case WAND:obj_ptr->type = WAND; output(fd, "Object is a wand.\n");return(PROMPT);
       	case CONTAINER:obj_ptr->type = CONTAINER; output(fd, "Object is a container.\n");return(PROMPT);
       	case MONEY:obj_ptr->type = MONEY; output(fd, "Object is money.\n");return(PROMPT);               
		case KEY:obj_ptr->type = KEY; output(fd, "Object is a key.\n");return(PROMPT);
		case LIGHTSOURCE:obj_ptr->type = LIGHTSOURCE; output(fd, "Object is a lightsource.\n");return(PROMPT);
		case MISC:obj_ptr->type = MISC; output(fd, "Object is a misc item.\n");return(PROMPT);
		case HERB:obj_ptr->type = HERB; output(fd, "Object is an herb.\n"); return(PROMPT);
		case FOOD:obj_ptr->type = FOOD; output(fd, "Object is food.\n");return(PROMPT);
		case DRINK:obj_ptr->type = DRINK; output(fd, "Object is a drink.\n");return(PROMPT);
		}

		break;
	case 'v':
		obj_ptr->value = num;
		output(fd, "Value set.\n");
		return(PROMPT);
	case 'w':
		switch(flags[1]) {
		case 'g': obj_ptr->weight = (short) num; output(fd, "Weight set.\n");return(PROMPT);
		case 'r': obj_ptr->wearflag = (char) num; output(fd, "Wear location set.\n");return(PROMPT);
		}
		break;
	}

	output(fd, "Invalid option.\n");
	return(0);
}


/**********************************************************************/
/*                       dm_set_xflg                                  */
/**********************************************************************/

/* This function allows a DM to set a characteristic of a exit. */

int dm_set_xflg( creature *ply_ptr, cmd *cmnd )
{
    room    *rom_ptr;
    xtag    *xp;
    int     num, fd;
    char    found=0;

    rom_ptr = ply_ptr->parent_rom;
    fd = ply_ptr->fd;

    if(cmnd->num < 3) {
        output(fd, "Syntax: *set xf <exit> [i <value>|r <value>|m <message>|<value>]\n");
        return(0);
    }

    xp = rom_ptr->first_ext;
    while(xp) {
        if(!strcmp(xp->ext->name, cmnd->str[2])){
            found = 1;
            break;
        }
        xp = xp->next_tag;
    }
 
    if(!found) {
            output(fd, "Exit not found.\n");
            return(0);
    }
	           
    if(cmnd->str[3][0] == 'i' || cmnd->str[3][0] == 'I') {
		xp->ext->random.interval = cmnd->val[3];
		sprintf(g_buffer, "%s exit interval set to %d\n",xp->ext->name,(int)cmnd->val[3]);
		output(fd, g_buffer);
    }
    else if(cmnd->str[3][0] == 'r' || cmnd->str[3][0] == 'R') {
		sprintf(g_buffer, "%s exit random set to %d\n",xp->ext->name,(int)cmnd->val[3]);
		output(fd, g_buffer);
		xp->ext->random.misc = cmnd->val[3];
    } 
    else if(cmnd->str[3][0] == 'm' || cmnd->str[3][0] == 'M') {
	if(cmnd->str[3][1] == '1') {
		clean_str(cmnd->fullstr, 4);
		if(strlen(cmnd->fullstr) < 1) {
			output(fd, "Set what?\n");
			return(0);
		}
		strcpy(xp->ext->rand_mesg[0],cmnd->fullstr);
		output(fd, "Ok.\n");
	}
	else if(cmnd->str[3][1] == '2') {
		clean_str(cmnd->fullstr, 4);
		if(strlen(cmnd->fullstr) < 1) {
			output(fd, "Set what?\n");
			return(0);
		}
		strcpy(xp->ext->rand_mesg[1], cmnd->fullstr);
		output(fd, "Ok.\n");
	}
    }
    else {
    num = cmnd->val[2];
    if(num < 1) {
		output(fd, "Invalid flag\n");
		return(PROMPT);
    }

    if(F_ISSET(xp->ext, num-1)) {
        F_CLR(xp->ext, num-1);
        sprintf(g_buffer, "%s exit flag #%d off.\n", xp->ext->name, num);
    }
    else {
        F_SET(xp->ext, num-1);
        sprintf(g_buffer, "%s exit flag #%d on.\n", xp->ext->name, num);
    }

    output(fd, g_buffer);
    }
	return(0);   
   
}

/***********************************************************************
*
*
*/
int dm_set_xkey(creature *ply_ptr, cmd *cmnd )
{
    int     fd,next=0;
    char    found=0;
    room	*rom_ptr;
    xtag	*xp;

    rom_ptr = ply_ptr->parent_rom;
    fd = ply_ptr->fd;

    if(cmnd->num < 3) {
        output(fd, "Syntax: *set xk <exit> <key#>\n");
	next = next_available_key();
	sprintf(g_buffer, "Next available key is #%d\n",next);
	output(fd, g_buffer);
        return(0);
    }

        xp = rom_ptr->first_ext;
        while(xp) {
			if(!strcmp(xp->ext->name, cmnd->str[2]) &&
				!F_ISSET(xp->ext,XNOSEE)){
				found = 1;
				break;
			}   
			xp = xp->next_tag;
        }

        if(!found) {
            output(fd, "Exit not found.\n");
            return(0);
        }

	if(cmnd->val[2]>255 || cmnd->val[2]< 0) {
		output(fd, "Key number is out of range.\n");
		return(0);
	}

	xp->ext->key = (unsigned char)cmnd->val[2];
	if ( xp->ext->key == 0 )
	{
		sprintf(g_buffer, "Key removed for exit %s.\n", xp->ext->name);
	}
	else
	{
		sprintf(g_buffer, "Exit %s key set to %d.\n", xp->ext->name, xp->ext->key);
	}

	output(fd, g_buffer);

	return(0);
}

/***********************************************************************/
/***********************************************************************/

void link_rom(room	**rom_ptr, short tonum, char *dir )
{
	exit_	*ext;
	xtag	*xp, *prev = 0, *temp;

	xp = (*rom_ptr)->first_ext;

	while(xp) {
		ext = xp->ext;
		prev = xp;
		xp = xp->next_tag;
		if (!strcmp(ext->name, dir)) {
			strcpy(ext->name, dir);
			ext->room = tonum;
			return;
		}
	}

	temp = (xtag *)malloc(sizeof(xtag));
	ext = (exit_ *)malloc(sizeof(exit_));

	zero(ext, sizeof(exit_));
	strcpy(ext->name, dir);
	ext->room = tonum;

	temp->next_tag = 0;
	temp->ext = ext;

	if (prev)
		prev->next_tag = temp;
	else
		(*rom_ptr)->first_ext = temp;
}

/*********************************************************************/
/*********************************************************************/
int del_exit(room **rom_ptr, char *dir )
{
	xtag	*xp, *prev = 0;

	xp = (*rom_ptr)->first_ext;

	while(xp) {
		if (!strcmp(xp->ext->name, dir)) {
			if(prev)
				prev->next_tag = xp->next_tag;
			else
				(*rom_ptr)->first_ext = xp->next_tag;

			free(xp->ext);
			free(xp);
			return(1);
		}
		prev = xp;
		xp = xp->next_tag;
	}

	return(0);
}

/*********************************************************************/
/*********************************************************************/
void expand_exit_name( char *name )
{
	if(!strcmp(name, "n"))  strcpy(name, "north");
	else if(!strcmp(name, "s"))  strcpy(name, "south");
	else if(!strcmp(name, "e"))  strcpy(name, "east");
	else if(!strcmp(name, "w"))  strcpy(name, "west");
	else if(!strcmp(name, "sw"))  strcpy(name, "southwest");
	else if(!strcmp(name, "nw"))  strcpy(name, "northwest");
	else if(!strcmp(name, "se"))  strcpy(name, "southeast");
	else if(!strcmp(name, "ne"))  strcpy(name, "northeast");
	else if(!strcmp(name, "d")) strcpy(name, "door");
	else if(!strcmp(name, "o")) strcpy(name, "out");
	else if(!strcmp(name, "p")) strcpy(name, "passage");
	else if(!strcmp(name, "t")) strcpy(name, "trap door");
}

/*********************************************************************/
/*********************************************************************/
int opposite_exit_name(char *name, char *name2 )
{
	if(!strcmp(name, "south"))  strcpy(name2, "north");
	else if(!strcmp(name, "north"))  strcpy(name2, "south");
	else if(!strcmp(name, "west"))  strcpy(name2, "east");
	else if(!strcmp(name, "east"))  strcpy(name2, "west");
	else if(!strcmp(name, "northeast"))  strcpy(name2, "southwest");
	else if(!strcmp(name, "southeast"))  strcpy(name2, "northwest");
	else if(!strcmp(name, "northwest"))  strcpy(name2, "southeast");
	else if(!strcmp(name, "southwest"))  strcpy(name2, "northeast");
	else return(0);		/* search failed */

	return(1);		/* search successful */
}

/************************************************************************/
/*				dm_log					*/
/************************************************************************/

/* This function allows DMs to peruse the log file while in the game.	*/
/* If *log r is typed, then the log file is removed (i.e. cleared).	*/

int dm_log(creature *ply_ptr, cmd *cmnd )
{
	char fn[80];
	int	fd;

	fd = ply_ptr->fd;

	if(ply_ptr->class < CARETAKER) 
		return(PROMPT);

	if (cmnd->num < 2 )
	{
		output(fd, "syntax: *log <p|i|e|j|s|t> [-r | search string] \n");
		return(0);
	}

	switch ( tolower(cmnd->str[1][0]) )
	{
		case 'p':
			sprintf(fn, "%s/%s", get_log_path(), PLAYER_LOG);
			break;
		case 'i':
			if(ply_ptr->class < DM)
				return(PROMPT);

			sprintf(fn, "%s/%s", get_log_path(), IMMORTAL_LOG);
			break;
		case 'e':
			sprintf(fn, "%s/%s", get_log_path(), ERROR_LOG);
			break;
		case 'j':
			sprintf(fn, "%s/%s", get_log_path(), JAIL_LOG);
			break;
		case 's':
			sprintf(fn, "%s/SUICIDE", get_log_path());
			break;
		case 't':
			sprintf(fn, "%s/%s", get_log_path(),STATUS_LOG);
			break;
		default:
			output(fd, "Which log do you wish to view?\n");
			return(0);
			break;

	}


	if ( cmnd->num == 3 && !strcmp(cmnd->str[2], "-r"))
	{
		unlink(fn);
		output(fd, "Log removed.\n");
		return(0);
	}
	else if ( cmnd->num >= 3 ) {
		clean_str(cmnd->fullstr, 2);
		strcpy(Ply[fd].extr->tempstr[3], cmnd->fullstr);
	}
	else 
		strcpy(Ply[fd].extr->tempstr[3], "\0");

	view_file_reverse(fd, 1, fn);
	

	return(DOPROMPT);
}

/************************************************************************/
/*				dm_loadlockout				*/
/************************************************************************/

/* This function allows the DM to force the game to reload the lockout	*/
/* file.  Use this command when you have modified the lockout file and	*/
/* don't want to restart the game to put it into effect.		*/

int dm_loadlockout(creature	*ply_ptr, cmd *cmnd )
{
	int	i;

	if(ply_ptr-> class < DM)
		return(PROMPT);

	if(cmnd->num > 1) {
		if(!strcmp(cmnd->str[1], "creation")) {
			if(NOCREATE) {
				output_nl(ply_ptr->fd, "Character creation enabled.\n");
				NOCREATE=0;
				return(0);
			}
			else {
				output_nl(ply_ptr->fd, "Character creation disabled.\n");
				NOCREATE=1;
				return(0);
			}
		
		}
		else {
			return(dm_add_lockout(ply_ptr, cmnd));
		}
	}

	load_lockouts();
	output_nl(ply_ptr->fd, "Lockout sites read in:");
	for(i=0; i<Numlockedout; i++) {
		if ( Lockout[i].userid && Lockout[i].address)
		{
			sprintf(g_buffer, "%s@%s", Lockout[i].userid, Lockout[i].address);
			output_nl(ply_ptr->fd, g_buffer);
		}
		else
		{
			elog_broad("Bad entry in lockout file!");
		}

	}
	return(0);
}


int dm_add_lockout(creature *ply_ptr, cmd *cmnd)
{
	creature *crt_ptr;
	char buffer[80];
	char path[80];
	int fd;
	struct hostent *address;

	if(cmnd->num == 2) {
		cmnd->str[1][0] = up(cmnd->str[1][0]);
		crt_ptr = find_who(cmnd->str[1]);
		if(!crt_ptr) {
			output(ply_ptr->fd, "Use entire player name please.\n");
			return(0);
		}

		if(!strcmp(Ply[crt_ptr->fd].io->userid,"unknown") || 
			!strcmp(Ply[crt_ptr->fd].io->userid,"no_port"))
			sprintf(buffer, "all ");
		else
			sprintf(buffer, "%s ",Ply[crt_ptr->fd].io->userid);


		address = gethostbyname(Ply[crt_ptr->fd].io->address);
		if(!address) {
			output(ply_ptr->fd, "Error in lookup\n");
			return(0);
		}
	
		sprintf(g_buffer, "%s -\n", inet_ntoa( *(struct in_addr *)(address->h_addr_list[0])));

		strcat(buffer, g_buffer);
		
	sprintf(path, "%s/lockout", get_log_path());
    fd = open(path, O_RDWR | O_APPEND | O_BINARY );
    if(fd < 0)
        {
        fd = open(path, O_RDWR | O_CREAT | O_BINARY, ACC);
        if(fd < 0) {
		output(ply_ptr->fd, "Couldnt open lockout file for writing\n");
                        return(1);
	}
    }
    lseek(fd, 0L, SEEK_END);

    write(fd, buffer, strlen(buffer));

    close(fd);
	load_lockouts();
	output(ply_ptr->fd, buffer);
	log_dm_command(ply_ptr->name,cmnd->fullstr);
	disconnect(crt_ptr->fd);
		
	}
	return(0);
}

/************************************************************************/
/*				dm_finger				*/
/************************************************************************/

/* This command allows a DM to finger the machine that a player is 	*/
/* calling from.  If a second argument is passed, then finger will	*/
/* do a finger on the person named.  The DM can also request to 	*/
/* explicitly name the site he is fingering using the @ in front of	*/
/* the address.								*/


int dm_finger( creature *ply_ptr, cmd *cmnd )
{
#ifdef FINGERACCT
	char		addr[80];
	char		name[80];
	creature	*crt_ptr;
	char		*argv[5];
	char		path[256];
	char		special[15];

	if(ply_ptr->class < CARETAKER)
		return(PROMPT);

	if(cmnd->num < 2) {
		output(ply_ptr->fd, "Finger whom?\n");
		return(0);
	}

	if(cmnd->str[1][0] == '@')
		strcpy(addr, &cmnd->str[1][1]);
	else {
		cmnd->str[1][0] = up(cmnd->str[1][0]);
		crt_ptr = find_who(cmnd->str[1]);
		if(!crt_ptr) {
			output(ply_ptr->fd, "Use entire player name please.\n");
			return(0);
		}
		strcpy(addr, Ply[crt_ptr->fd].io->address);
	}

	if(cmnd->num == 3)
		strncpy(name, cmnd->str[2], 79);
	else
		strcpy(name, "");

#ifndef WIN32
	sprintf(path, "%s/fing", get_bin_path());
#else
	sprintf(path, "%s/fing.exe", get_bin_path());
#endif

	argv[0] = path;

	/* special arg, run_child will sprintf the fd into here */
	strcpy(special, "%d" );
	argv[1] = special;
	argv[2] = addr;
	argv[3] = name;
	argv[4] = NULL;

	if ( run_child(ply_ptr->fd, argv) != 0 )
	{
		sprintf(g_buffer, "Fingering %s\n", addr);
		output(ply_ptr->fd, g_buffer);
		output(ply_ptr->fd, "Output will arrive shortly.\n");
		/* force this output first */
		output_ply_buf(ply_ptr->fd);
	}
	else
	{
		elog_broad("Error running fing");
	}


#endif

return(0);
}


/************************************************************************/
/*				dm_list					*/
/************************************************************************/

/* This function allows DMs to fork a "list" process.  List is the 	*/
/* utility program that allows one to nicely output a list of monsters,	*/
/* objects or rooms in the game.  There are many flags provided with	*/
/* the command, and they can be entered here in the same way that they	*/
/* are entered on the command line.  					*/

int dm_list(creature *ply_ptr, cmd *cmnd )
{

	int		i;
	char	*argv[7];
	char	path[256];
	char	special[15];
	char 	str[3];

	if(ply_ptr->class < CARETAKER )
		return(PROMPT);

	if(cmnd->num < 2) {
		output(ply_ptr->fd, "List what?\n");
		return(0);
	}

#ifndef WIN32
	sprintf(path, "%s/list", get_bin_path());
#else
	sprintf(path, "%s/list.exe", get_bin_path());
#endif

	for(i=cmnd->num; i<COMMANDMAX; i++)
		cmnd->str[i][0] = '\0';

	argv[0] = path;
	argv[1] = cmnd->str[1];

	/* special arg, run_child will sprintf the fd into here */
	strcpy(special, "-s%d" );
	argv[2] = special;

	if ( cmnd->str[2][0] != '\0' )
		argv[3] = cmnd->str[2];
	else
		argv[3] = NULL;

	if ( cmnd->str[3][0] != '\0' )
		argv[4] = cmnd->str[3];
	else
		argv[4] = NULL;

	if ( cmnd->str[4][0] != '\0' )
		argv[5] = cmnd->str[4];
	else
		argv[5] = NULL;

	strcpy(str, "-z");
	for(i=3;i<COMMANDMAX;i++) {
		if(argv[i] == '\0') {
			argv[i] = str;
			break;
		}
	}

	argv[6] = NULL; 
	
	output(ply_ptr->fd, "Running list.\n");
	output(ply_ptr->fd, "Output will arrive shortly.\n");
	/* force this output first */
	output_ply_buf(ply_ptr->fd);

	
	if ( !run_child(ply_ptr->fd, argv) != 0 )
	{
		output(ply_ptr->fd, "Error running list.\n");
		elog_broad("Error running list");
	}

return(0);
}





/************************************************************************/
/*				dm_info					*/
/************************************************************************/

/* This command allows the DM to view various system statistics		*/

int dm_info(creature *ply_ptr, cmd *cmnd )
{
	extern int	Rsize, Csize, Osize, Numplayers, Numwaiting;
	int		fd;

	if(ply_ptr->class < CARETAKER )
		return(PROMPT);

	fd = ply_ptr->fd;
	output(fd, "Internal Cache Queue Sizes:\n");
	sprintf(g_buffer, "   Rooms: %-5d   Monsters: %-5d   Objects: %-5d\n\n",
		Rsize, Csize, Osize);
	output(fd, g_buffer);
	sprintf(g_buffer, "Wander update: %d\n", Random_update_interval);
	output(fd, g_buffer);

	sprintf(g_buffer, "      Players: %d  Queued: %d\n\n", Numplayers, Numwaiting);
	output(fd, g_buffer);

	return(0);
}

/*
 * next_available_key
 *
 * Determines the first free key # by searching the object database
 *
 */
int next_available_key(void)
{
	int i,missing=0;
	object obj;
	short keys[256] = {0};

	for(i=0;i<OMAX;i++) {
		if(load_obj_struct_from_file(i,&obj) < 0) {
			missing++;
			if(missing > 1000) break;
			continue;
		}
		if(obj.type != 11)
			continue;

		keys[obj.ndice] = 1;
	}

	for(i=0;i<256;i++) {
		if(keys[(short)i] == 0)
			return(i);
	}
	
	return(-1);
}
