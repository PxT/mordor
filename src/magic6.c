/*
 * MAGIC6.C:
 *
 *	Additional spell-casting routines.
 *
 *	Copyright (C) 1993, 1999 Brooke Paul
 *
 * $Id: magic6.c,v 6.13 2001/03/08 16:09:09 develop Exp $
 *
 * $Log: magic6.c,v $
 * Revision 6.13  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

#include "../include/mordb.h"
#include "mextern.h"

/************************************************************************/
/*				resist_magic				*/
/************************************************************************/

/* This function allows players to cast the resist-magic spell.  It	*/
/* will allow the player to resist magical attacks from monsters	*/

int resist_magic(creature *ply_ptr, cmd *cmnd, int how )
{
	creature	*crt_ptr;
	room		*rom_ptr;
	int		fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(ply_ptr->mpcur < 12 && how == CAST) {
		output(fd, "Not enough magic points.\n");
		return(0);
	}

	if(!S_ISSET(ply_ptr, SRMAGI) && how == CAST) {
		output(fd, "You don't know that spell.\n");
		return(0);
	}
        if(!dec_daily(&ply_ptr->daily[DL_RMGIC]) && how == CAST &&
                ply_ptr->class < BUILDER) {
                output(fd, "You have cast that enough today.\n");
                return(0);
        }

	if(spell_fail(ply_ptr, how)) {
                if(how == CAST)
                     ply_ptr->mpcur -= 12;
                return(0);
        }

	if(cmnd->num == 2) {
		ply_ptr->lasttime[LT_RMAGI].ltime = time(0);
		F_SET(ply_ptr, PRMAGI);
		broadcast_rom(fd, ply_ptr->rom_num, 
			"%M resists magic.", m1arg(ply_ptr));
		if(how == CAST) {
			output(fd, "You are surrounded by a magical shield.\n");
			ply_ptr->mpcur -= 12;
			ply_ptr->lasttime[LT_RMAGI].interval = MAX(300, 1200 +
				bonus[(int)ply_ptr->intelligence]*600);
if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            output(fd,"The room's magical properties increase the power of your spell.\n");
            ply_ptr->lasttime[LT_RMAGI].interval += 800L;
        }                                
		}
		else {
			output(fd, "You are surrounded by a magical shield.\n");
			ply_ptr->lasttime[LT_RMAGI].interval = 1200L;
		}
		return(1);
	}
	else {

		if(how == POTION) {
			output(fd, "You can only use a potion on yourself.\n");
			return(0);
		}

		cmnd->str[2][0] = up(cmnd->str[2][0]);
		crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
				   cmnd->str[2], cmnd->val[2]);
		if(!crt_ptr) {
			output(fd, "I don't see that player here.\n");
			return(0);
		}

		F_SET(crt_ptr, PRMAGI);
		crt_ptr->lasttime[LT_RMAGI].ltime = time(0);
		broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
			"%M surrounds %m with a magical shield.",
			m2args(ply_ptr, crt_ptr));
		mprint(crt_ptr->fd, "%M casts resist-magic on you.\n", 
			m1arg(ply_ptr));

		if(how == CAST) {
			sprintf(g_buffer, "You cast a resist-magic spell on %s.\n",
				crt_ptr->name);
			output(fd, g_buffer);
			ply_ptr->mpcur -= 12;
			crt_ptr->lasttime[LT_RMAGI].interval = MAX(300, 1200 +
				bonus[(int)ply_ptr->intelligence]*600);
if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            output(fd,"The room's magical properties increase the power of your spell.\n");
            crt_ptr->lasttime[LT_RMAGI].interval += 800L;
        }                                
		}

		else {
			mprint(fd, "%M is surrounded by a magic shield.\n",
				m1arg(crt_ptr));
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

int know_alignment(creature	*ply_ptr, cmd *cmnd, int how )
{
	creature	*crt_ptr;
	room		*rom_ptr;
	int		fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(ply_ptr->mpcur < 6 && how == CAST) {
		output(fd, "Not enough magic points.\n");
		return(0);
	}

	if(!S_ISSET(ply_ptr, SKNOWA) && how == CAST) {
		output(fd, "You don't know that spell.\n");
		return(0);
	}

	if(cmnd->num == 2) {
		ply_ptr->lasttime[LT_KNOWA].ltime = time(0);
		F_SET(ply_ptr, PKNOWA);
		broadcast_rom(fd, ply_ptr->rom_num, 
			"%M casts a know-aura spell.", m1arg(ply_ptr));
		if(how == CAST) {
			output(fd, "You become more perceptive.\n");
			ply_ptr->mpcur -= 6;
			ply_ptr->lasttime[LT_KNOWA].interval = MAX(300, 1200 +
				bonus[(int)ply_ptr->intelligence]*600);
		if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            	   output(fd,"The room's magical properties increase the power of your spell.\n");
            	   ply_ptr->lasttime[LT_KNOWA].interval += 800L;
        	}                                
		}
		else {
			output(fd, "You become more perceptive.\n");
			ply_ptr->lasttime[LT_KNOWA].interval = 1200L;
		}
		return(1);
	}
	else {

		if(how == POTION) {
			output(fd, "You can only use a potion on yourself.\n");
			return(0);
		}

		cmnd->str[2][0] = up(cmnd->str[2][0]);
		crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
				   cmnd->str[2], cmnd->val[2]);
		if(!crt_ptr) {
			output(fd, "I don't see that player here.\n");
			return(0);
		}

		F_SET(crt_ptr, PKNOWA);
		crt_ptr->lasttime[LT_KNOWA].ltime = time(0);
		broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
			"%M casts a know-aura spell on %m.",
			m2args(ply_ptr, crt_ptr));
		mprint(crt_ptr->fd, "%M casts know-aura on you.\n",
			m1arg(ply_ptr));

		if(how == CAST) {
			sprintf(g_buffer, "You cast a know-aura spell on %s.\n",
				crt_ptr->name);
			output(fd, g_buffer);
			ply_ptr->mpcur -= 6;
			crt_ptr->lasttime[LT_KNOWA].interval = MAX(300, 1200 +
				bonus[(int)ply_ptr->intelligence]*600);
		if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            	   output(fd,"The room's magical properties increase the power of your spell.\n");
            	   crt_ptr->lasttime[LT_KNOWA].interval += 800L;
        	}                                
		}

		else {
			mprint(fd, "%M becomes more perceptive.\n",
				m1arg(crt_ptr));
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

int remove_curse(creature *ply_ptr, cmd *cmnd, int how )
{
	creature	*crt_ptr;
	room		*rom_ptr;
	int		fd, i;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(ply_ptr->mpcur < 18 && how == CAST) {
		output(fd, "Not enough magic points.\n");
		return(0);
	}

	if(!S_ISSET(ply_ptr, SREMOV) && how == CAST) {
		output(fd, "You don't know that spell.\n");
		return(0);
	}
	if(spell_fail(ply_ptr, how)) {
                if(how == CAST)
                     ply_ptr->mpcur -= 18;
                return(0);
        }

	/* Cast remove-curse on self */
	if(cmnd->num == 2) {

		if(how == CAST)
			ply_ptr->mpcur -= 18;

		if(how == CAST || how == SCROLL || how == WAND) {
			output(fd, "Remove-curse spell cast.\n");
			sprintf(g_buffer, "%%M casts remove-curse on %sself.", 
				      F_ISSET(ply_ptr, PMALES) ? "him":"her");
			broadcast_rom(fd, ply_ptr->rom_num, 
				      g_buffer, m1arg(ply_ptr));
		}
		else if(how == POTION)
			output(fd, "You feel relieved of burdens.\n");

		for(i=0; i<MAXWEAR; i++)
			if(ply_ptr->ready[i]) {
				F_CLR(ply_ptr->ready[i], OCURSW);
				F_CLR(ply_ptr->ready[i], OCURSE);
			}
		return(1);
	}

	/* Cast remove-curse on another player */
	else {

		if(how == POTION) {
			output(fd, "You can only use a potion on yourself.\n");
			return(0);
		}

		cmnd->str[2][0] = up(cmnd->str[2][0]);
		crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
				   cmnd->str[2], cmnd->val[2]);

		if(!crt_ptr) {
			output(fd, "That player is not here.\n");
			return(0);
		}

		if(how == CAST)
			ply_ptr->mpcur -= 18;

		if(how == CAST || how == SCROLL || how == WAND) {
			mprint(fd, "Remove-curse cast on %m.\n", m1arg(crt_ptr));
			mprint(crt_ptr->fd, 
			      "%M casts a remove-curse spell on you.\n",
			      m1arg(ply_ptr));
			broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
				       "%M casts remove-curse on %m.",
				       m2args(ply_ptr, crt_ptr));

			for(i=0; i<MAXWEAR; i++)
				if(crt_ptr->ready[i]) {
					F_CLR(crt_ptr->ready[i], OCURSW);
					F_CLR(crt_ptr->ready[i], OCURSE);
			}


			return(1);
		}
	}

	return(1);

}
