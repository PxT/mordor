/*
 * MAGIC5.C:
 *
 *	Additional spell-casting routines.
 *
 *	Copyright (C) 1991, 1992, 1993 Brooke Paul
 *
 * $Id: magic5.c,v 6.23 2001/06/23 06:31:42 develop Exp $
 *
 * $Log: magic5.c,v $
 * Revision 6.23  2001/06/23 06:31:42  develop
 * tweaked output when conjure involved
 *
 * Revision 6.22  2001/06/23 06:28:26  develop
 * fixed error in word that caused caster to always be target
 *
 * Revision 6.21  2001/06/23 06:24:43  develop
 * fixed output in summon/teleport/word when conjure is involved
 *
 * Revision 6.20  2001/06/23 06:19:41  develop
 * fixed output in summon/teleport/word when conjure is involved
 *
 * Revision 6.19  2001/06/23 06:06:04  develop
 * adjusting summon output when conjure is involved
 *
 * Revision 6.18  2001/06/23 05:56:20  develop
 * fixed summon to allow for clearing of conjure list
 *
 * Revision 6.17  2001/06/23 05:51:08  develop
 * added check for conjure follow in summon
 *
 * Revision 6.16  2001/06/23 04:05:44  develop
 * *** empty log message ***
 *
 * Revision 6.15  2001/06/18 19:37:39  develop
 * fixed recall bug introduced with conjure mods
 *
 * Revision 6.14  2001/06/06 19:36:46  develop
 * conjure added
 *
 * Revision 6.13  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

#include "../include/mordb.h"
#include "mextern.h"

/**********************************************************************/
/*				recall				      */
/**********************************************************************/

/* This function allows a player to teleport himself or another player */
/* to room #1 (Town Square)					       */

int recall(creature	*ply_ptr, cmd *cmnd, int how )
{
	creature	*crt_ptr;
	room		*rom_ptr, *new_rom;
	int		fd;
	ctag		*cp;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(ply_ptr->mpcur < 25 && how == CAST) {
		output(fd, "Not enough magic points.\n");
		return(0);
	}

	if(ply_ptr->class != CLERIC && 
	   ply_ptr->class < BUILDER && how == CAST) {
		output(fd, "Only clerics may cast that spell.\n");
		return(0);
	}

	if(!S_ISSET(ply_ptr, SRECAL) && how == CAST) {
		output(fd, "You don't know that spell.\n");
		return(0);
	}

	/* Cast recall on self */
	if(cmnd->num == 2) {

		if(how == CAST)
			ply_ptr->mpcur -= 25;

		if(ply_ptr->type == PLAYER && F_ISSET(ply_ptr, PPLDGK))
		{
                	if(load_rom(cur_guilds[check_guild(ply_ptr)].hazyroom, &new_rom) < 0) 
			{
                		if(load_rom(start_room_num, &new_rom) < 0) {
                        		output(fd, "Spell failure.\n");
                        		return(0);
                		}
			}
		}
		else
		{
                	if(load_rom(start_room_num, &new_rom) < 0) {
                        	output(fd, "Spell failure.\n");
                        	return(0);
                	}
		}


		if(F_ISSET(ply_ptr, PALIAS)) 
		{
            		del_crt_rom(Ply[ply_ptr->fd].extr->alias_crt, ply_ptr->parent_rom);
	    		add_crt_rom(Ply[ply_ptr->fd].extr->alias_crt, new_rom, 1);

	    	}

		if(how == CAST || how == SCROLL || how == WAND) 
		{
			output(fd, "Word of recall spell cast.\n");
			sprintf(g_buffer, "%%M casts word of recall on %sself", F_ISSET(ply_ptr, PMALES) ? "him":"her");
			broadcast_rom(fd, ply_ptr->rom_num, g_buffer, m1arg(ply_ptr));
		}
		
		else if(how == POTION) 
		{
			output(fd, "You phase in and out of existence.\n");
			broadcast_rom(fd, ply_ptr->rom_num, "%M disappears.", m1arg(ply_ptr));
		}

          cp = ply_ptr->first_fol;
          while(cp) {
                if(cp->crt->type == MONSTER && F_ISSET(cp->crt, MCONJU)) {
                         broadcast_rom(fd, cp->crt->parent_rom->rom_num,
                                "%M fades away.", m1arg(cp->crt));
                        del_conjured(cp->crt);
                        del_crt_rom(cp->crt,cp->crt->parent_rom);
                        free_crt(cp->crt);
                }
          cp = cp->next_tag;
          }

	del_ply_rom(ply_ptr, rom_ptr);
        add_ply_rom(ply_ptr, new_rom);

	return(1);
	}

	/* Cast word of recall on another player */
	else {

		if(how == POTION) 
		{
			output(fd, "You can only use a potion on yourself.\n");
			return(0);
		}

		cmnd->str[2][0] = up(cmnd->str[2][0]);
		crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply, cmnd->str[2], cmnd->val[2]);

		if(!crt_ptr) 
		{
			output(fd, "That player is not here.\n");
			return(0);
		}

		if(how == CAST)
			ply_ptr->mpcur -= 25;


		if(how == CAST || how == SCROLL || how == WAND) 
		{
			mprint(fd, "Word of recall cast on %m.\n", m1arg(crt_ptr));
			mprint(crt_ptr->fd, "%M casts a word of recall spell on you.\n", m1arg(ply_ptr));
			broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, "%M casts word of recall on %m.",
				m2args(ply_ptr, crt_ptr));

                	if(crt_ptr->type == PLAYER && F_ISSET(crt_ptr, PPLDGK))
                	{
                        	if(load_rom(cur_guilds[check_guild(crt_ptr)].hazyroom, &new_rom) < 0)
                        	{
                                	if(load_rom(start_room_num, &new_rom) < 0) {
                                        	output(fd, "Spell failure.\n");
                                        	return(0);
                                	}
                        	}
                	}
                	else
                	{                               
                        	if(load_rom(start_room_num, &new_rom) < 0) {
                                	output(fd, "Spell failure.\n");
                                	return(0);
                        	}
                	} 


        cp = crt_ptr->first_fol;
        while(cp) {
                if(cp->crt->type == MONSTER && F_ISSET(cp->crt, MCONJU)) {
                         broadcast_rom(fd, cp->crt->parent_rom->rom_num,
                                "%M fades away.", m1arg(cp->crt));
                        del_conjured(cp->crt);
                        del_crt_rom(cp->crt, cp->crt->parent_rom);
                        free_crt(cp->crt);
                }
        cp = cp->next_tag;
        }

		del_ply_rom(crt_ptr, rom_ptr);
                add_ply_rom(crt_ptr, new_rom);
	

			return(1);
		}
	}

	return(1);

}

/**********************************************************************/
/*				summon				      */
/**********************************************************************/

/* This function allows players to cast summon spells on anyone who is */
/* in the game, taking that person to your current room.	       */

int summon(creature	*ply_ptr, cmd *cmnd, int how )
{
	creature	*crt_ptr;
	room		*rom_ptr;
	int		fd, n;
	ctag            *cp;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(ply_ptr->mpcur < 30 && how == CAST) {
		output(fd, "Not enough magic points.\n");
		return(0);
	}

	if(!S_ISSET(ply_ptr, SSUMMO) && how == CAST) {
		output(fd, "You don't know that spell.\n");
		return(0);
	}

	if(cmnd->num == 2) {
		output(fd, "You may not use that on yourself.\n");
		return(0);
	}

	else {

		if(how == POTION) {
			output(fd, "You can only use a potion on yourself.\n");
			return(0);
		}

		cmnd->str[2][0] = up(cmnd->str[2][0]);
		crt_ptr = find_who(cmnd->str[2]);

		if(!crt_ptr || crt_ptr == ply_ptr || F_ISSET(crt_ptr, PDMINV)) {
			output(fd, 
			      "That player is not playing (Use full names).\n");
			return(0);
		}

		if(how == CAST)
			ply_ptr->mpcur -= 30;

		n = count_vis_ply(rom_ptr);
		if(F_ISSET(rom_ptr, RNOTEL) ||
		  (F_ISSET(rom_ptr, RONEPL) && n > 0) ||
		  (F_ISSET(rom_ptr, RTWOPL) && n > 1) ||
		  (F_ISSET(rom_ptr, RTHREE) && n > 2)) {
			output(fd, "The spell fizzles.\n");
			return(0);
		}

		if(rom_ptr->lolevel > crt_ptr->level ||
		   (crt_ptr->level > rom_ptr->hilevel && rom_ptr->hilevel) ||
		   F_ISSET(crt_ptr, PNOSUM)) {
			output(fd, "The spell fizzles.\n");
			return(0);
		}

		if(F_ISSET(crt_ptr->parent_rom,RNOLEA))
		{
			sprintf(g_buffer, "Your magic can not locate %s.\n",crt_ptr->name);
			output(fd, g_buffer);
			return(0);
		}

		cp = crt_ptr->first_fol;
                while(cp) {
                  if(cp->crt->type == MONSTER && F_ISSET(cp->crt, MCONJU)) {
                         broadcast_rom(fd, cp->crt->parent_rom->rom_num,
                                "%M fades away.", m1arg(cp->crt));
                        del_conjured(cp->crt);
                        del_crt_rom(cp->crt,cp->crt->parent_rom);
                        free_crt(cp->crt);
                  }
                cp = cp->next_tag;
                }

		if(how == CAST || how == SCROLL || how == WAND) {
			mprint(fd, "You summon %m.\n", m1arg(crt_ptr));
			mprint(crt_ptr->fd, "%M summons you.\n",
			      m1arg(ply_ptr));
			broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
				       "%M summons %m.",
				       m2args(ply_ptr, crt_ptr));

			del_ply_rom(crt_ptr, crt_ptr->parent_rom);
			add_ply_rom(crt_ptr, rom_ptr);

			return(1);
		}
	}

	return(1);

}

/**********************************************************************/
/*				heal				      */
/**********************************************************************/

/* This function will cause the heal spell to be cast on a player or    */
/* another monster.  It heals all hit points damage but only works 3 	*/
/* times a day.								*/

int heal(creature *ply_ptr, cmd *cmnd, int how )
{
	creature	*crt_ptr;
	room		*rom_ptr;
	int			fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(ply_ptr->mpcur < 20 && how == CAST) {
		output(fd, "Not enough magic points.\n");
		return(0);
	}

	if(ply_ptr->class != CLERIC && ply_ptr->class != PALADIN &&
	   ply_ptr->class < BUILDER && how == CAST) {
		output(fd, "Only paladins and clerics may cast that spell.\n");
		return(0);
	}

	if(!S_ISSET(ply_ptr, SFHEAL) && how == CAST) {
		output(fd, "You don't know that spell.\n");
		return(0);
	}

	/* Heal self */
	if(cmnd->num == 2 ) {

		if(!dec_daily(&ply_ptr->daily[DL_FHEAL]) && how == CAST &&
		   ply_ptr->class < BUILDER) {
			output(fd, "You have healed enough today.\n");
			return(0);
		}

		ply_ptr->hpcur = ply_ptr->hpmax;

		if(how == CAST) 
			ply_ptr->mpcur -= 20;

		if(how == CAST || how == SCROLL) {
			output(fd, "Heal spell cast.\n");
			sprintf(g_buffer, "%%M casts a heal spell on %sself.", 
				      F_ISSET(ply_ptr, PMALES) ? "him":"her");
			broadcast_rom(fd, ply_ptr->rom_num, 
				      g_buffer, m1arg(ply_ptr));
			return(1);
		}
		else {
			output(fd, "You feel incredibly better.\n");
			return(1);
		}
	}

	/* Cast heal on another player or monster */
	else {

		if(how == POTION) {
			output(fd, "You can only use a potion on yourself.\n");
			return(0);
		}

		crt_ptr = find_crt_in_rom(ply_ptr, rom_ptr,
				   cmnd->str[2], cmnd->val[2], PLY_FIRST);


		if(!crt_ptr) {
			output(fd, "That person is not here.\n");
			return(0);
		}

		if(!dec_daily(&ply_ptr->daily[DL_FHEAL]) && how == CAST &&
		   ply_ptr->class < BUILDER && ply_ptr->type != MONSTER) {
			output(fd, "You have healed enough today.\n");
			return(0);
		}

		crt_ptr->hpcur = crt_ptr->hpmax;

		if(how == CAST) 
			ply_ptr->mpcur -= 20;
		if(how == CAST || how == SCROLL || how == WAND) {
			mprint(fd, "Heal spell cast on %m.\n", m1arg(crt_ptr));
			mprint(crt_ptr->fd, "%M casts a heal spell on you.\n",
			      m1arg(ply_ptr));
			broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
				       "%M casts a heal spell on %m.",
				       m2args(ply_ptr, crt_ptr));
			return(1);
		}
	}

	return(1);

}

/**********************************************************************/
/*				magictrack				      */
/**********************************************************************/

/* This function allows rangers to cast the track spell which takes them */
/* to any other player in the game.					 */

int magictrack(creature	*ply_ptr, cmd *cmnd, int how )
{
	creature	*crt_ptr;
	room		*rom_ptr;
	int		fd, n;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(ply_ptr->class != DRUID && ply_ptr->class != RANGER && ply_ptr->class < BUILDER &&
	   how==CAST) {
		output(fd, "Only rangers may cast that spell.\n");
		return(0);
	}

	if(ply_ptr->mpcur < 13 && how == CAST) {
		output(fd, "Not enough magic points.\n");
		return(0);
	}

	if(!S_ISSET(ply_ptr, STRACK) && how == CAST) {
		output(fd, "You don't know that spell.\n");
		return(0);
	}

	if(cmnd->num == 2) {
		output(fd, "You may not use that on yourself.\n");
		return(0);
	}

	else {

		if(how == POTION) {
			output(fd, "You can only use a potion on yourself.\n");
			return(0);
		}

		cmnd->str[2][0] = up(cmnd->str[2][0]);
		crt_ptr = find_who(cmnd->str[2]);

		if(!crt_ptr || crt_ptr == ply_ptr || F_ISSET(crt_ptr, PDMINV)) {
			output(fd, 
			      "That player is not playing (Use full names).\n");
			return(0);
		}

		if(crt_ptr->class >= BUILDER) {
			output(fd, "You may not track that player.\n");
			return(0);
		}

		n = count_vis_ply(crt_ptr->parent_rom);
		if(F_ISSET(crt_ptr->parent_rom, RNOTEL) ||
		  (F_ISSET(crt_ptr->parent_rom, RONEPL) && n > 0) ||
		  (F_ISSET(crt_ptr->parent_rom, RTWOPL) && n > 1) ||
		  (F_ISSET(crt_ptr->parent_rom, RTHREE) && n > 2)) {
			output(fd, "The spell fizzles.\n");
			return(0);
		}

		if(crt_ptr->parent_rom->lolevel > ply_ptr->level ||
		   (ply_ptr->level > crt_ptr->parent_rom->hilevel &&
		   crt_ptr->parent_rom->hilevel)) {
			output(fd, "The spell fizzles.\n");
			return(0);
		}

		if(!dec_daily(&ply_ptr->daily[DL_TRACK]) && how == CAST &&
		   ply_ptr->class < BUILDER) {
			output(fd, "You have tracked enough today.\n");
			return(0);
		}

		if(how == CAST)
			ply_ptr->mpcur -= 13;

		if(how == CAST || how == SCROLL || how == WAND) {
			mprint(fd, "You track %m.\n", m1arg(crt_ptr));
			mprint(crt_ptr->fd, 
			      "%M has tracked you.\n", m1arg(ply_ptr));
			broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
				       "%M tracks %m.", m2args(ply_ptr, crt_ptr));

			del_ply_rom(ply_ptr, rom_ptr);
			add_ply_rom(ply_ptr, crt_ptr->parent_rom);

			return(1);
		}
	}

	return(1);

}

/**********************************************************************/
/*				levitate			      */
/**********************************************************************/

/* This function allows players to cast the levitate spell, thus allowing */
/* them to levitate over traps or up mountain cliffs.			  */

int levitate(creature *ply_ptr, cmd *cmnd, int how )
{
	creature	*crt_ptr;
	room		*rom_ptr;
	int		fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(ply_ptr->mpcur < 10 && how == CAST) {
		output(fd, "Not enough magic points.\n");
		return(0);
	}

	if(!S_ISSET(ply_ptr, SLEVIT) && how == CAST) {
		output(fd, "You don't know that spell.\n");
		return(0);
	}
	if(spell_fail(ply_ptr, how)) {
                if(how == CAST)
                     ply_ptr->mpcur -= 10;
                return(0);
        }

	if(cmnd->num == 2) {
		ply_ptr->lasttime[LT_LEVIT].ltime = time(0);
		F_SET(ply_ptr, PLEVIT);
		broadcast_rom(fd, ply_ptr->rom_num, 
			"%M levitates off the floor.", m1arg(ply_ptr));
		if(how == CAST) {
			output(fd, "You cast a levitation spell.\n");
			ply_ptr->mpcur -= 10;
			ply_ptr->lasttime[LT_LEVIT].interval = 2400L +
				bonus[(int)ply_ptr->intelligence]*600L;
if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            output(fd,"The room's magical properties increase the power of your spell.\n");
            ply_ptr->lasttime[LT_LEVIT].interval += 800L;
        }                                
		}
		else {
			output(fd, "You begin to float.\n");
			ply_ptr->lasttime[LT_LEVIT].interval = 1200L;
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

		F_SET(crt_ptr, PLEVIT);
		crt_ptr->lasttime[LT_LEVIT].ltime = time(0);
		broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
			"%M casts a levitate spell on %m.", 
			m2args(ply_ptr, crt_ptr));
		mprint(crt_ptr->fd, "%M casts levitate on you.\n", 
			m1arg(ply_ptr));

		if(how == CAST) {
			sprintf(g_buffer, "You cast a levitate spell on %s.\n",crt_ptr->name);
			output(fd, g_buffer);
			ply_ptr->mpcur -= 10;
			crt_ptr->lasttime[LT_LEVIT].interval = 2400L +
				bonus[(int)ply_ptr->intelligence]*600L;
		if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            	  output(fd,"The room's magical properties increase the power of your spell.\n");
            	  crt_ptr->lasttime[LT_LEVIT].interval += 800L;
        	}                                
		}

		else {
			mprint(fd, "%M begins to float.\n", m1arg(crt_ptr));
			crt_ptr->lasttime[LT_LEVIT].interval = 1200L;
		}

		return(1);
	}

}

/************************************************************************/
/*				resist_fire				*/
/************************************************************************/

/* This function allows players to cast the resist fire spell.  It will	*/
/* allow the player to resist fire breathed on them by dragons and 	*/
/* other breathers.							*/

int resist_fire(creature *ply_ptr, cmd *cmnd, int how )
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

	if(!S_ISSET(ply_ptr, SRFIRE) && how == CAST) {
		output(fd, "You don't know that spell.\n");
		return(0);
	}
	if(spell_fail(ply_ptr, how)) {
                if(how == CAST)
                     ply_ptr->mpcur -= 12;
                return(0);
        }

	if(cmnd->num == 2) {
		ply_ptr->lasttime[LT_RFIRE].ltime = time(0);
		F_SET(ply_ptr, PRFIRE);
		broadcast_rom(fd, ply_ptr->rom_num, 
			"%M resists fire.", m1arg(ply_ptr));
		if(how == CAST) {
			output(fd, "You cast a resist-fire spell.\nYour skin toughens.\n");
			ply_ptr->mpcur -= 12;
			ply_ptr->lasttime[LT_RFIRE].interval = MAX(300, 1200 +
				bonus[(int)ply_ptr->intelligence]*600);
		if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            	  output(fd,"The room's magical properties increase the power of your spell.\n");
            	  ply_ptr->lasttime[LT_RFIRE].interval += 800L;
        	}                                
		}
		else {
			output(fd, "You feel your skin toughen.\n");
			ply_ptr->lasttime[LT_RFIRE].interval = 1200L;
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

		F_SET(crt_ptr, PRFIRE);
		crt_ptr->lasttime[LT_RFIRE].ltime = time(0);
		broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
			"%M casts a resist-fire spell on %m.", 
			m2args(ply_ptr, crt_ptr));
		mprint(crt_ptr->fd, "%M casts resist-fire on you.\n", 
			m1arg(ply_ptr));

		if(how == CAST) {
			sprintf(g_buffer, "You cast a resist-fire spell on %s.\n",
				crt_ptr->name);
			output(fd, g_buffer);
			ply_ptr->mpcur -= 12;
			crt_ptr->lasttime[LT_RFIRE].interval = MAX(300, 1200 +
				bonus[(int)ply_ptr->intelligence]*600);
		if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            	  output(fd,"The room's magical properties increase the power of your spell.\n");
            	  crt_ptr->lasttime[LT_RFIRE].interval += 800L;
        	}                                
		}

		else {
			mprint(fd, "%M resists fire.\n", m1arg(crt_ptr));
			crt_ptr->lasttime[LT_RFIRE].interval = 1200L;
		}

		return(1);
	}

}


/************************************************************************/
/*				fly					*/
/************************************************************************/

/* This function allows players to cast the fly spell.  It will		*/
/* allow the player to fly to areas that are otherwise unreachable	*/
/* by foot.								*/

int fly(creature *ply_ptr, cmd *cmnd, int how )
{
	creature	*crt_ptr;
	room		*rom_ptr;
	int		fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(ply_ptr->mpcur < 15 && how == CAST) {
		output(fd, "Not enough magic points.\n");
		return(0);
	}

	if(!S_ISSET(ply_ptr, SFLYSP) && how == CAST) {
		output(fd, "You don't know that spell.\n");
		return(0);
	}
	if(spell_fail(ply_ptr, how)) {
                if(how == CAST)
                     ply_ptr->mpcur -= 15;
                return(0);
        }

	if(cmnd->num == 2) {
		ply_ptr->lasttime[LT_FLYSP].ltime = time(0);
		F_SET(ply_ptr, PFLYSP);
		broadcast_rom(fd, ply_ptr->rom_num, 
			"%M begins to fly.", m1arg(ply_ptr));
		if(how == CAST) {
			output(fd, "You can fly!\n");
			ply_ptr->mpcur -= 15;
			ply_ptr->lasttime[LT_FLYSP].interval = MAX(300, 1200 +
				bonus[(int)ply_ptr->intelligence]*600);
		if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            	  output(fd,"The room's magical properties increase the power of your spell.\n");
            	  ply_ptr->lasttime[LT_FLYSP].interval += 600L;
        	}                                
		}
		else {
			output(fd, "You can fly!\n");
			ply_ptr->lasttime[LT_FLYSP].interval = 1200L;
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

		F_SET(crt_ptr, PFLYSP);
		crt_ptr->lasttime[LT_FLYSP].ltime = time(0);
		broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
			"%M casts a fly spell on %m.", 
			m2args(ply_ptr, crt_ptr));
		mprint(crt_ptr->fd, "%M casts fly on you.\n", m1arg(ply_ptr));

		if(how == CAST) {
			sprintf(g_buffer, "You cast a fly spell on %s.\n",
				crt_ptr->name);
			output(fd, g_buffer);
			ply_ptr->mpcur -= 15;
			crt_ptr->lasttime[LT_FLYSP].interval = MAX(300, 1200 +
				bonus[(int)ply_ptr->intelligence]*600);
		if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            	  output(fd,"The room's magical properties increase the power of your spell.\n");
            	  crt_ptr->lasttime[LT_FLYSP].interval += 600L;
        	}                                
		}

		else {
			mprint(fd, "%M can fly.\n", m1arg(crt_ptr));
			crt_ptr->lasttime[LT_FLYSP].interval = 1200L;
		}

		return(1);
	}

}

