/*
 *      COMMAND14.C:
 *
 *      (C) Brooke Paul 2001
 *
 * 	$Id: command14.c,v 1.10 2001/07/29 22:11:02 develop Exp $
 *	
 *	$Log: command14.c,v $
 *	Revision 1.10  2001/07/29 22:11:02  develop
 *	added shop_cost
 *
 *	Revision 1.9  2001/07/29 21:46:26  develop
 *	changed pricing on shop and added logging
 *
 *	Revision 1.8  2001/07/29 16:54:38  develop
 *	upped price on shops to 1mil
 *
 *	Revision 1.7  2001/07/25 02:55:04  develop
 *	fixes for thieves dropping stolen items
 *	fixes for gold dropping by pkills
 *
 *	Revision 1.6  2001/07/21 15:25:52  develop
 *	commented out set_obj_price
 *	put that functionality into appraise instead
 *
 *	Revision 1.5  2001/07/21 15:22:07  develop
 *	*** empty log message ***
 *
 *	Revision 1.4  2001/07/20 12:11:35  develop
 *	!telnet
 *
 *	Revision 1.3  2001/07/20 12:10:00  develop
 *	added pawnshops
 *
 *	Revision 1.2  2001/07/18 01:43:24  develop
 *	changed cost of stores
 *
 *	Revision 1.1  2001/07/17 19:25:11  develop
 *	Initial revision
 *
 *	Revision 1.1  2001/07/15 05:42:50  develop
 *	Initial revision
 *
 *
 */


#include "../include/mordb.h"
#include "mextern.h"

int setup_shop( creature *ply_ptr, cmd *cmnd )
{
        room    *rom_ptr, *dep_ptr;
	int	n, fd;

	ASSERTLOG( ply_ptr );

	rom_ptr=ply_ptr->parent_rom;
	fd = ply_ptr->fd;

	if(!F_ISSET(rom_ptr, RPOWND)) {
		output(fd, "You can't do that here.\n");
		return(0);
	}

	if(load_rom(rom_ptr->rom_num+1, &dep_ptr) < 0) {
                output(fd, "This isn't the right place.\n");
                return(0);
        }

	if(ply_ptr->gold < shop_cost) {
		sprintf(g_buffer, "That requires $%ld.\n", shop_cost);
		output(fd, g_buffer);
		return(0);
	}

	if(ply_ptr->level < 7) {
		output(fd, "You are not high enough level to do that.\n");
		return(0);
	}

	switch(low(cmnd->str[1][0]))
	{
		case 'g':
			n = set_rom_owner(ply_ptr, GENERALSTORE);
			break;
		case 'a':
			n = set_rom_owner(ply_ptr, ARMORY);
                        break;
		case 'e':
			n = set_rom_owner(ply_ptr, EMPORIUM);
                        break;
		case 'o':
			n = set_rom_owner(ply_ptr, OUTFITTER);
			break;
		case 'p':
			n = set_rom_owner(ply_ptr, PAWNSHOP);
			break;
		case 'm':
			n = set_rom_owner(ply_ptr, MARKETPLACE);
                        break;
		default:
			output(fd, "Syntax: shop [market | emporium | pawn shop | general store | outfitter | armory ]\n");
			return(0);
	}

	if(!n) {
		elog("Shop setup error in setup_shop\n");
		output(fd, "You can't do that now.\n");
                return(0);
        }

	ply_ptr->gold -= shop_cost;
	sprintf(g_buffer, "%s created a shop (%d).\n", ply_ptr->name, rom_ptr->rom_num);
        slog(g_buffer);
	output(fd, "You are now in business. Good luck!\n");
	output(fd, "You may now enter your storeroom.\n");
	return(0);
}
