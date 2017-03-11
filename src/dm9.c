/*
 *      DM9.C:
 *
 *      (C) Brooke Paul 2001
 *
 *	$Id: dm9.c,v 1.13 2001/07/25 23:10:52 develop Exp $
 *
 *	$Log: dm9.c,v $
 *	Revision 1.13  2001/07/25 23:10:52  develop
 *	fix for *stolen
 *
 *	Revision 1.12  2001/07/25 02:55:04  develop
 *	fixes for thieves dropping stolen items
 *	fixes for gold dropping by pkills
 *
 *	Revision 1.12  2001/07/24 01:36:23  develop
 *	*** empty log message ***
 *
 *	Revision 1.11  2001/07/22 20:42:54  develop
 *	added dm_stolen
 *
 *	Revision 1.10  2001/07/22 20:05:52  develop
 *	gold theft changes
 *
 *	Revision 1.9  2001/07/21 23:19:23  develop
 *	set log_dm_command for dm_reset_shop
 *
 *	Revision 1.8  2001/07/20 01:32:05  develop
 *	./mordord -r
 *	fixed output
 *
 *	Revision 1.7  2001/07/20 01:28:23  develop
 *	fixed output for failed dm_reset_shop
 *
 *	Revision 1.6  2001/07/20 00:25:38  develop
 *	more fixes for confused dms
 *
 *	Revision 1.5  2001/07/20 00:23:42  develop
 *	kwatch
 *
 *	Revision 1.4  2001/07/20 00:18:07  develop
 *	fixed *shop code to account for DM error
 *
 *	Revision 1.3  2001/07/19 17:03:08  develop
 *	*** empty log message ***
 *
 *	Revision 1.2  2001/07/18 01:51:42  develop
 *	fixing crash bug in dm_reset_shop
 *
 *	Revision 1.1  2001/07/18 01:43:24  develop
 *	Initial revision
 *
 *
 *
 */

#include "../include/mordb.h"
#include "mextern.h"

/* This command allows a DM to easily create or reset a player created shop
 */
int dm_reset_shop(creature *ply_ptr, cmd *cmnd)
{
	room	*rom_ptr, *dep_ptr;
	xtag	*xp;
	otag	*op, *otemp;
	int	fd,n;

	ASSERTLOG(ply_ptr);

	fd=ply_ptr->fd;
	rom_ptr=ply_ptr->parent_rom;

	if(ply_ptr->class < CARETAKER) 
		return(0);

	log_dm_command(ply_ptr->name, cmnd->fullstr);

	if(load_rom(rom_ptr->rom_num+1, &dep_ptr) < 0) {
		output(fd, "Error loading storeroom.\n");
		return(0);
	}

	/* create a new player shop from scratch */
	if(!F_ISSET(dep_ptr, RPOWND) && !F_ISSET(rom_ptr, RPOWND)) {
		xp = dep_ptr->first_ext;
		if(xp) {
			sprintf(g_buffer, "Room %d has exits.  Please remove and try again.\n", dep_ptr->rom_num);	
			output(fd, g_buffer);
			return(0);
		}
		/* set it to be ready for next tenant */
		F_SET(rom_ptr, RPOWND);
       		sprintf(dep_ptr->name, "%s", "Storeroom");
        	sprintf(rom_ptr->name, "%s", "An abandoned shop");
        	sprintf(g_buffer, "Room %d set as shop with %d as storeroom.\n", rom_ptr->rom_num, dep_ptr->rom_num);
        	output(fd, g_buffer);
		resave_rom(rom_ptr->rom_num);
	        resave_rom(dep_ptr->rom_num);
       	 	reload_rom(dep_ptr->rom_num);
        	reload_rom(rom_ptr->rom_num);
		return(0);
	}
	
	/* looks like an existing shop */
	if(F_ISSET(dep_ptr, RPOWND)) {

		output(fd, "Destroying existing shop...I hope you know what you are doing.\n");
		F_CLR(dep_ptr, RPOWND);
		F_CLR(rom_ptr, RSHOPP);

		/* delete any objects in the storeroom */
		op = dep_ptr->first_obj;
	        dep_ptr->first_obj = 0;
       		while(op) {
                	otemp = op->next_tag;
                        free_obj(op->obj);
                        free(op);
                op = otemp;
        	}

		/* delete the storeroom entrance and exit */
		sprintf(g_buffer, "out");
		n = del_exit(&dep_ptr, g_buffer);
		if(!n) 
			output(fd, "Error: storeroom exit not found.\n");
		n=0;
		sprintf(g_buffer, "storeroom");
		n = del_exit(&rom_ptr, g_buffer);
		if(!n)
                        output(fd, "Error: storeroom entrance not found.\n");

		/* set it to be ready for next tenant */
        	F_SET(rom_ptr, RPOWND);
        	sprintf(dep_ptr->name, "%s", "Storeroom");
        	sprintf(rom_ptr->name, "%s", "An abandoned shop");
        	sprintf(g_buffer, "Room %d set as shop with %d as storeroom.\n", rom_ptr->rom_num, dep_ptr->rom_num);
        	output(fd, g_buffer);
        	resave_rom(rom_ptr->rom_num);
        	resave_rom(dep_ptr->rom_num);
        	reload_rom(dep_ptr->rom_num);
        	reload_rom(rom_ptr->rom_num);
        	return(0);

	}
		/* looks like a clueless DM */
		output(fd, "You have tried to create a shop where one already exists,\n");
		output(fd, "or are executing this command without understanding how\n");
		output(fd, "shops work. You might want to ask for help before trying again.\n");
		return(0);
}

/* this allows a DM to see a given player's stolen list */
int dm_stolen(creature *ply_ptr, cmd *cmnd)
{

	int		fd, n=0;
	ctag		*cp;
	room		*rom_ptr;
	creature	*crt_ptr;

	fd=ply_ptr->fd;
        rom_ptr=ply_ptr->parent_rom;

        if(ply_ptr->class < CARETAKER)
                return(0);

	 crt_ptr = find_crt_in_rom(ply_ptr, rom_ptr, cmnd->str[1], cmnd->val[1], PLY_FIRST);

	if(!crt_ptr) {
		cmnd->str[2][0] = up(cmnd->str[2][0]);
        	crt_ptr = find_who(cmnd->str[2]);
	}

	if(!crt_ptr) {
		output(fd, "Player not found.\n");
		return(0);
	}

	if(crt_ptr->type != PLAYER) {
		output(fd, "This command only works on players.\n");
                return(0);
        }

	output(fd, "Stolen list:\n");
	cp = Ply[crt_ptr->fd].extr->first_stolen;
	while(cp) {
		n=1;
		sprintf(g_buffer, "%s\n", cp->crt->name);
		output(fd, g_buffer);
	cp = cp->next_tag;
	}

	if(!n)
		output(fd, "None.\n");

	return(0);
}
