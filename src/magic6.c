/*
 * MAGIC6.C:
 *
 *	Additional spell-casting routines.
 *
 *	Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"

/************************************************************************/
/*				resist_magic				*/
/************************************************************************/

/* This function allows players to cast the resist-magic spell.  It	*/
/* will allow the player to resist magical attacks from monsters	*/

int resist_magic(ply_ptr, cmnd, how)
creature	*ply_ptr;
cmd		*cmnd;
int		how;
{
	creature	*crt_ptr;
	room		*rom_ptr;
	int		fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(ply_ptr->mpcur < 12 && how == CAST) {
		print(fd, "Not enough magic points.\n");
		return(0);
	}

	if(!S_ISSET(ply_ptr, SRMAGI) && how == CAST) {
		print(fd, "You don't know that spell.\n");
		return(0);
	}
	if(spell_fail(ply_ptr)) {
                if(how == CAST)
                     ply_ptr->mpcur -= 12;
                return(0);
        }

	if(cmnd->num == 2) {
		ply_ptr->lasttime[LT_RMAGI].ltime = time(0);
		F_SET(ply_ptr, PRMAGI);
		broadcast_rom(fd, ply_ptr->rom_num, 
			"%M resists magic.", ply_ptr);
		if(how == CAST) {
			print(fd, "You are surrounded by a magical shield.\n");
			ply_ptr->mpcur -= 12;
			ply_ptr->lasttime[LT_RMAGI].interval = MAX(300, 1200 +
				bonus[ply_ptr->intelligence]*600);
if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,"The room's magical properties increase the power of your spell.\n");
            ply_ptr->lasttime[LT_RMAGI].interval += 800L;
        }                                
		}
		else {
			print(fd, "You are surrounded by a magical shield.\n");
			ply_ptr->lasttime[LT_RMAGI].interval = 1200L;
		}
		return(1);
	}
	else {

		if(how == POTION) {
			print(fd, "You can only use a potion on yourself.\n");
			return(0);
		}

		cmnd->str[2][0] = up(cmnd->str[2][0]);
		crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
				   cmnd->str[2], cmnd->val[2]);
		if(!crt_ptr) {
			print(fd, "I don't see that player here.\n");
			return(0);
		}

		F_SET(crt_ptr, PRMAGI);
		crt_ptr->lasttime[LT_RMAGI].ltime = time(0);
		broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
			"%M surrounds %m with a magical shield.",
			ply_ptr, crt_ptr);
		print(crt_ptr->fd, "%M casts resist-magic on you.\n", ply_ptr);

		if(how == CAST) {
			print(fd, "You cast a resist-magic spell on %s.\n",
				crt_ptr);
			ply_ptr->mpcur -= 12;
			crt_ptr->lasttime[LT_RMAGI].interval = MAX(300, 1200 +
				bonus[ply_ptr->intelligence]*600);
if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,"The room's magical properties increase the power of your spell.\n");
            crt_ptr->lasttime[LT_RMAGI].interval += 800L;
        }                                
		}

		else {
			print(fd, "%M is surrounded by a magic shield.\n",
				crt_ptr);
			crt_ptr->lasttime[LT_RMAGI].interval = 1200L;
		}

		return(1);
	}

}

/************************************************************************/
/*				know_alignment				*/
/************************************************************************/

/* This spell allows the caster to determine what alignment another	*/
/* creature or player is by looking at it.				*/

int know_alignment(ply_ptr, cmnd, how)
creature	*ply_ptr;
cmd		*cmnd;
int		how;
{
	creature	*crt_ptr;
	room		*rom_ptr;
	int		fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(ply_ptr->mpcur < 6 && how == CAST) {
		print(fd, "Not enough magic points.\n");
		return(0);
	}

	if(!S_ISSET(ply_ptr, SKNOWA) && how == CAST) {
		print(fd, "You don't know that spell.\n");
		return(0);
	}

	if(cmnd->num == 2) {
		ply_ptr->lasttime[LT_KNOWA].ltime = time(0);
		F_SET(ply_ptr, PKNOWA);
		broadcast_rom(fd, ply_ptr->rom_num, 
			"%M casts a know-aura spell.", ply_ptr);
		if(how == CAST) {
			print(fd, "You become more perceptive.\n");
			ply_ptr->mpcur -= 6;
			ply_ptr->lasttime[LT_KNOWA].interval = MAX(300, 1200 +
				bonus[ply_ptr->intelligence]*600);
		if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            	   print(fd,"The room's magical properties increase the power of your spell.\n");
            	   ply_ptr->lasttime[LT_KNOWA].interval += 800L;
        	}                                
		}
		else {
			print(fd, "You become more perceptive.\n");
			ply_ptr->lasttime[LT_KNOWA].interval = 1200L;
		}
		return(1);
	}
	else {

		if(how == POTION) {
			print(fd, "You can only use a potion on yourself.\n");
			return(0);
		}

		cmnd->str[2][0] = up(cmnd->str[2][0]);
		crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
				   cmnd->str[2], cmnd->val[2]);
		if(!crt_ptr) {
			print(fd, "I don't see that player here.\n");
			return(0);
		}

		F_SET(crt_ptr, PKNOWA);
		crt_ptr->lasttime[LT_KNOWA].ltime = time(0);
		broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
			"%M casts a know-aura spell on %m.",
			ply_ptr, crt_ptr);
		print(crt_ptr->fd, "%M casts know-aura on you.\n",
			ply_ptr);

		if(how == CAST) {
			print(fd, "You cast a know-aura spell on %s.\n",
				crt_ptr);
			ply_ptr->mpcur -= 6;
			crt_ptr->lasttime[LT_KNOWA].interval = MAX(300, 1200 +
				bonus[ply_ptr->intelligence]*600);
		if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            	   print(fd,"The room's magical properties increase the power of your spell.\n");
            	   crt_ptr->lasttime[LT_KNOWA].interval += 800L;
        	}                                
		}

		else {
			print(fd, "%M becomes more perceptive.\n",
				crt_ptr);
			crt_ptr->lasttime[LT_KNOWA].interval = 1200L;
		}

		return(1);
	}

}

/**********************************************************************/
/*			remove-curse				      */
/**********************************************************************/

/* This function allows a player to remove a curse on all the items	*/
/* in his inventory or on another player's inventory			*/

int remove_curse(ply_ptr, cmnd, how)
creature	*ply_ptr;
cmd		*cmnd;
int		how;
{
	creature	*crt_ptr;
	room		*rom_ptr, *new_rom;
	int		fd, i;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(ply_ptr->mpcur < 18 && how == CAST) {
		print(fd, "Not enough magic points.\n");
		return(0);
	}

	if(!S_ISSET(ply_ptr, SREMOV) && how == CAST) {
		print(fd, "You don't know that spell.\n");
		return(0);
	}
        if(spell_fail(ply_ptr)) {
                if(how == CAST)
                     ply_ptr->mpcur -= 18;
                return(0);
        }


	/* Cast remove-curse on self */
	if(cmnd->num == 2) {

		if(how == CAST)
			ply_ptr->mpcur -= 18;

		if(how == CAST || how == SCROLL || how == WAND) {
			print(fd, "Remove-curse spell cast.\n");
			broadcast_rom(fd, ply_ptr->rom_num, 
				      "%M casts remove-curse on %sself.", 
				      ply_ptr,
				      F_ISSET(ply_ptr, PMALES) ? "him":"her");
		}
		else if(how == POTION)
			print(fd, "You feel relieved of burdens.\n");

		for(i=0; i<MAXWEAR; i++)
			if(ply_ptr->ready[i])
				F_CLR(ply_ptr->ready[i], OCURSE);

		return(1);
	}

	/* Cast remove-curse on another player */
	else {

		if(how == POTION) {
			print(fd, "You can only use a potion on yourself.\n");
			return(0);
		}

		cmnd->str[2][0] = up(cmnd->str[2][0]);
		crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
				   cmnd->str[2], cmnd->val[2]);

		if(!crt_ptr) {
			print(fd, "That player is not here.\n");
			return(0);
		}

		if(how == CAST)
			ply_ptr->mpcur -= 18;

		if(how == CAST || how == SCROLL || how == WAND) {
			print(fd, "Remove-curse cast on %m.\n", crt_ptr);
			print(crt_ptr->fd, 
			      "%M casts a remove-curse spell on you.\n",
			      ply_ptr);
			broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
				       "%M casts remove-curse on %m.",
				       ply_ptr, crt_ptr);

			for(i=0; i<MAXWEAR; i++)
				if(crt_ptr->ready[i])
					F_CLR(crt_ptr->ready[i], OCURSE);

			return(1);
		}
	}

	return(1);

}
