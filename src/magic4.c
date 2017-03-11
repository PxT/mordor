/*
 * MAGIC4.C:
 *
 *	Additional spell-casting routines.
 *
 *	Copyright (C) 1991, 1992, 1993 Brooke Paul
 *
 * $Id: magic4.c,v 6.17 2001/06/23 06:31:42 develop Exp $
 *
 * $Log: magic4.c,v $
 * Revision 6.17  2001/06/23 06:31:42  develop
 * tweaked output when conjure involved
 *
 * Revision 6.16  2001/06/23 06:19:41  develop
 * fixed output in summon/teleport/word when conjure is involved
 *
 * Revision 6.15  2001/06/23 04:49:12  develop
 * fixed follow list bug in teleport crt_prt->first_fol
 * was there instead of ply_pre->first_fol
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
/*				detectinvis			      */
/**********************************************************************/

/* This function allows players to cast the detect-invisible spell which */
/* allows the spell-castee to see invisible items. 			 */

int detectinvis(creature *ply_ptr, cmd *cmnd, int how )
{
	creature	*crt_ptr;
	room		*rom_ptr;
	time_t		t;
	int		fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;
	t = time(0);

	if(ply_ptr->mpcur < 10 && how == CAST) {
		output(fd, "Not enough magic points.\n");
		return(0);
	}

	if(!S_ISSET(ply_ptr, SDINVI) && how == CAST) {
		output(fd, "You don't know that spell.\n");
		return(0);
	}
        if(spell_fail(ply_ptr, how)) {
                if(how==CAST)
                     ply_ptr->mpcur -= 10;
                return(0);
        }

	/* Cast detect-invisibility on self */
	if(cmnd->num == 2) {

		ply_ptr->lasttime[LT_DINVI].ltime = t;
		if(how == CAST) {
			ply_ptr->lasttime[LT_DINVI].interval = MAX(300, 1200 + 
				bonus[(int)ply_ptr->intelligence]*600);
			if(ply_ptr->class == MAGE)
				ply_ptr->lasttime[LT_DINVI].interval += 
				60*ply_ptr->level;
			ply_ptr->mpcur -= 10;
if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            output(fd,"The room's magical properties increase the power of your spell.\n");
            ply_ptr->lasttime[LT_DINVI].interval += 600L;
        }                                
		}
		else
			ply_ptr->lasttime[LT_DINVI].interval = 1200;

		if(how == CAST || how == SCROLL || how == WAND) {
			output(fd,"Detect-invisible spell cast.\n");
			sprintf(g_buffer, "%%M casts detect-invisible on %sself.", 
				      F_ISSET(ply_ptr, PMALES) ? "him":"her");
			broadcast_rom(fd, ply_ptr->rom_num, 
				      g_buffer, m1arg(ply_ptr));
		}
		else if(how == POTION)
			output(fd, "Your eyes tingle.\n");

		F_SET(ply_ptr, PDINVI);

		return(1);
	}

	/* Cast detect-invisibility on another player */
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

		crt_ptr->lasttime[LT_DINVI].ltime = t;
		if(how == CAST) {
			crt_ptr->lasttime[LT_DINVI].interval = MAX(300, 1200 + 
				bonus[(int)ply_ptr->intelligence]*600);
			if(ply_ptr->class == MAGE)
				crt_ptr->lasttime[LT_DINVI].interval += 
				60*ply_ptr->level;
			ply_ptr->mpcur -= 10;
if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            output(fd,"The room's magical properties increase the power of your spell.\n");
            crt_ptr->lasttime[LT_DINVI].interval += 600L;
        }                                
		}
		else
			crt_ptr->lasttime[LT_DINVI].interval = 1200;

		F_SET(crt_ptr, PDINVI);

		if(how == CAST || how == SCROLL || how == WAND) {
			mprint(fd, "Detect-invisible cast on %m.\n", m1arg(crt_ptr));
			mprint(crt_ptr->fd, 
			      "%M casts a detect-invisible spell on you.\n",
			      m1arg(ply_ptr));
			broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
				       "%M casts detect-invisible on %m.",
				       m2args(ply_ptr, crt_ptr));
			return(1);
		}
	}

	return(1);

}

/**********************************************************************/
/*				detectmagic			      */
/**********************************************************************/

/* This function allows players to cast the detect-magic spell which */
/* allows the spell-castee to see magic items. 			     */

int detectmagic(creature *ply_ptr, cmd *cmnd, int how )
{
	creature	*crt_ptr;
	room		*rom_ptr;
	time_t		t;
	int		fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;
	t = time(0);

	if(ply_ptr->mpcur < 10 && how == CAST) {
		output(fd, "Not enough magic points.\n");
		return(0);
	}

	if(!S_ISSET(ply_ptr, SDMAGI) && how == CAST) {
		output(fd, "You don't know that spell.\n");
		return(0);
	}
 	if(spell_fail(ply_ptr, how)) {
                if(how==CAST)
                     ply_ptr->mpcur -= 10;
                return(0);
        }

	/* Cast detect-magic on self */
	if(cmnd->num == 2) {

		ply_ptr->lasttime[LT_DMAGI].ltime = t;
		if(how == CAST) {
			ply_ptr->lasttime[LT_DMAGI].interval = MAX(300, 1200 + 
				bonus[(int)ply_ptr->intelligence]*600);
			if(ply_ptr->class == MAGE)
				ply_ptr->lasttime[LT_DMAGI].interval += 
				60*ply_ptr->level;
			ply_ptr->mpcur -= 10;
		if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            output(fd,"The room's magical properties increase the power of your spell.\n");
            ply_ptr->lasttime[LT_DMAGI].interval += 600L;
        }                                
		}
		else
			ply_ptr->lasttime[LT_DMAGI].interval = 1200;

		if(how == CAST || how == SCROLL || how == WAND) {
			output(fd,"Detect-magic spell cast.\n");
			sprintf(g_buffer, "%%M casts detect-magic on %sself.", 
				      F_ISSET(ply_ptr, PMALES) ? "him":"her");
			broadcast_rom(fd, ply_ptr->rom_num, 
				      g_buffer, m1arg(ply_ptr));
		}
		else if(how == POTION)
			output(fd, "Your eyes feel funny.\n");

		F_SET(ply_ptr, PDMAGI);

		return(1);
	}

	/* Cast detect-magic on another player */
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

		crt_ptr->lasttime[LT_DMAGI].ltime = t;
		if(how == CAST) {
			crt_ptr->lasttime[LT_DMAGI].interval = MAX(300, 1200 + 
				bonus[(int)ply_ptr->intelligence]*600);
			if(ply_ptr->class == MAGE)
				crt_ptr->lasttime[LT_DMAGI].interval += 
				60*ply_ptr->level;
			ply_ptr->mpcur -= 10;
if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            output(fd,"The room's magical properties increase the power of your spell.\n");
            crt_ptr->lasttime[LT_DMAGI].interval += 600L;
        }                                
		}
		else
			crt_ptr->lasttime[LT_DMAGI].interval = 1200;

		F_SET(crt_ptr, PDMAGI);

		if(how == CAST || how == SCROLL || how == WAND) {
			mprint(fd, "Detect-magic cast on %m.\n", m1arg(crt_ptr));
			mprint(crt_ptr->fd, 
			      "%M casts a detect-magic spell on you.\n",
			      m1arg(ply_ptr));
			broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
				       "%M casts detect-magic on %m.",
				       m2args(ply_ptr, crt_ptr));
			return(1);
		}
	}

	return(1);

}

/**********************************************************************/
/*				teleport			      */
/**********************************************************************/

/* This function allows a player to teleport himself or another player */
/* to another room randomly.					       */

int teleport(creature *ply_ptr, cmd	*cmnd, int how )
{
	creature	*crt_ptr;
	room		*rom_ptr, *new_rom;
	int		fd, rtn, rom_num;
	ctag		*cp;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(ply_ptr->mpcur < 30 && how == CAST) {
		output(fd, "Not enough magic points.\n");
		return(0);
	}

	if(!S_ISSET(ply_ptr, STELEP) && how == CAST) {
		output(fd, "You don't know that spell.\n");
		return(0);
	}

/*  I think a no telport room is one you cant teleport into not out of */
#ifdef CRAP
	if(F_ISSET(rom_ptr, RNOTEL)) {
                output(fd, "The spell fizzles.\n");
                if(how == CAST)
                        ply_ptr->mpcur -= 30;
                return(0);
        }
#endif

	if(!dec_daily(&ply_ptr->daily[DL_TELEP]) && ply_ptr->class < BUILDER) {
		output(fd, "You are too weak to teleport again today.\n");
		return(0);
	}
	if(spell_fail(ply_ptr, how)) {
                if(how == CAST)
                        ply_ptr->mpcur -= 30;
                return(0);
        }

	/* Cast teleport on self */
	if(cmnd->num == 2) {

		if(how == CAST)
			ply_ptr->mpcur -= 30;

		broadcast_rom(fd, ply_ptr->rom_num, 
			      "%M disappears.", m1arg(ply_ptr));
		if(how == CAST || how == SCROLL)
			output(fd, "Teleport spell cast.\n");
		else
			output(fd, "You become disoriented and find yourself in another place.\n");

		do {
			rom_num = mrand(1, RMAX-1);
			rtn = load_rom(rom_num, &new_rom);
			if(rtn > -1 && F_ISSET(new_rom, RNOTEL))
				rtn = -1;
			else if(rtn > -1 && new_rom->lolevel > ply_ptr->level)
				rtn = -1;
			else if(rtn > -1 && ply_ptr->level > new_rom->hilevel &&
				new_rom->hilevel)
				rtn = -1;
		} while(rtn < 0);

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

	/* Cast teleport on another player */
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
		
		if( ply_ptr->class < DM )
		{
			/* only a non DM can fail */
			if(ply_ptr->level < 13 ) 
			{
				output(fd, "The spell fizzles.\n");
        		if(how == CAST)
					ply_ptr->mpcur -= 30;
				return(0);	
			}
			

			if(F_ISSET(crt_ptr, PRMAGI) && (mrand(1,60)+(ply_ptr->level-crt_ptr->level)*10) > 80) {
				mprint(fd, "Your magic is too weak to teleport %m.\n", 
					m1arg(crt_ptr));
				mprint(crt_ptr->fd, "%M tried to cast teleport on you.\n", 
					m1arg(ply_ptr));
					if(how == CAST)
						ply_ptr->mpcur -= 30;
				return(0);
			}
		}

		

		if(how == CAST)
			ply_ptr->mpcur -= 30;
	
		if(how == CAST || how == SCROLL || how == WAND) {
			mprint(fd, "Teleport cast on %m.\n", m1arg(crt_ptr));
			mprint(crt_ptr->fd, 
			      "%M casts a teleport spell on you.\n",
			      m1arg(ply_ptr));
			broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
				       "%M casts teleport on %m.",
				       m2args(ply_ptr, crt_ptr));

			do {
				rom_num = mrand(1, RMAX-1);
				rtn = load_rom(rom_num, &new_rom);
				if(rtn > -1 && F_ISSET(new_rom, RNOTEL))
					rtn = -1;
				else if(rtn > -1 && 
					new_rom->lolevel > ply_ptr->level)
					rtn = -1;
				else if(rtn > -1 && 
					ply_ptr->level > new_rom->hilevel && 
					new_rom->hilevel)
					rtn = -1;
			} while(rtn < 0);


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

		del_ply_rom(crt_ptr, rom_ptr);
                add_ply_rom(crt_ptr, new_rom);


			return(1);
		}
	}

	return(1);

}

/**********************************************************************/
/*				enchant				      */
/**********************************************************************/

/* This function allows mages to enchant weapons at a cost of 25 magic */
/* points.  They can only do it 3 times a day.			       */

int enchant(creature *ply_ptr, cmd *cmnd, int how )
{
	object		*obj_ptr;
	room		*rom_ptr;
	int		fd, adj;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(how == CAST && ply_ptr->class != MAGE && 
	   ply_ptr->class < BUILDER) {
		output(fd, "Only mages may enchant.\n");
		return(0);
	}

	if(ply_ptr->mpcur < 25 && how == CAST) {
		output(fd, "Not enough magic points.\n");
		return(0);
	}

	if(!S_ISSET(ply_ptr, SENCHA) && how == CAST) {
		output(fd, "You don't know that spell.\n");
		return(0);
	}

	if(cmnd->num < 3) {
		output(fd, "Cast the spell on what?\n");
		return(0);
	}

	obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
			   cmnd->str[2], cmnd->val[2]);

	if(!obj_ptr) {
		output(fd, "You don't have that in your inventory.\n");
		return(0);
	}

	if(F_ISSET(obj_ptr, OENCHA)) {
		output(fd, "That object is already enchanted.\n");
		return(1);
	}

	if(how == CAST) {
		if(!dec_daily(&ply_ptr->daily[DL_ENCHA]) && 
		   ply_ptr->class < BUILDER) {
			output(fd, "You have enchanted enough today.\n");
			return(0);
		}
		ply_ptr->mpcur -= 25;
	}

	if((ply_ptr->class == MAGE || ply_ptr->class >= BUILDER) &&
	   how == CAST) {
		adj = (ply_ptr->level-5)/5 + 1;
		adj = MIN(3, adj);
	}
	else
		adj = 1;

	obj_ptr->adjustment = MAX(adj, obj_ptr->adjustment);

	if(obj_ptr->type == ARMOR && obj_ptr->wearflag == BODY)
		obj_ptr->armor += adj*2;
	else if(obj_ptr->type == ARMOR)
		obj_ptr->armor += adj;
	else if(obj_ptr->type <= MISSILE) {
		obj_ptr->shotsmax += adj*10;
		obj_ptr->shotscur += adj*10;
		obj_ptr->pdice += adj;
	}
	obj_ptr->value += 500*adj;

	F_SET(obj_ptr, OENCHA);

	mprint(fd, "%I begins to glow brightly.\n", m1arg(obj_ptr));
	broadcast_rom(fd, rom_ptr->rom_num, "%M enchants %1i.", 
		m2args(ply_ptr, obj_ptr));

	return(1);
}

