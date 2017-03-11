/*
 * 	MAGIC8.C:
 *
 *  	Additional spell-casting routines.
 *	(C) 1994, 1995 Brooke Paul
 *
 *
 * $Id: magic8.c,v 6.19 2001/07/06 17:36:48 develop Exp $
 *
 * $Log: magic8.c,v $
 * Revision 6.19  2001/07/06 17:36:48  develop
 * removed kruft in room_damage
 *
 * Revision 6.18  2001/05/22 02:02:44  develop
 * *** empty log message ***
 *
 * Revision 6.17  2001/05/22 01:53:48  develop
 * *** empty log message ***
 *
 * Revision 6.16  2001/05/22 01:42:25  develop
 * paladins are not affected by fear.
 *
 * Revision 6.15  2001/05/19 01:27:29  develop
 * Ok, try again.
 * fixed bug in fear that allowed it to be cast
 * against lawfuls
 *
 * Revision 6.14  2001/05/19 01:22:41  develop
 * fixed error in fear that allowed it to be cast on
 * !PCHAOS players.
 *
 * Revision 6.13  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

#include "../include/mordb.h"
#include "mextern.h"

/***********************************************************************/
/*                              room_damage                             */
/***********************************************************************/

int room_damage(creature *ply_ptr, cmd *cmnd, int how, char *spellname, osp_t *osp)
{          
   	int     fd, m, dmg, victims, bns=0, invisflag;
	long	totalrealm, addrealm;
	ctag	*cp;
	ctag	*cp_tmp;

	fd = ply_ptr->fd;

	if(how == POTION) {
		output(fd, "The spell fizzles.\n");
		return(0);
	}

	if(!S_ISSET(ply_ptr, osp->splno) && (how != WAND || how != SCROLL)) {
        	output(fd, "You don't know that spell.\n");
        	return(0);
    	}  
        if(ply_ptr->level < osp->intcast - (bonus[(int)ply_ptr->intelligence] * 3)) {
            output(fd, "You are not experienced enough to cast that spell.\n");
            return(0);
	}

        if(ply_ptr->mpcur < osp->mp && how == CAST) {
           output(fd, "Not enough magic points.\n");
           return(0);
        }

	if(how == CAST)
		ply_ptr->mpcur -= osp->mp;
	if(spell_fail(ply_ptr, how)) {
                return(0);
        }

	if(ply_ptr->type == PLAYER)
		invisflag = PINVIS;
	else
		invisflag = MINVIS;

	if(F_ISSET(ply_ptr, invisflag)) {
           F_CLR(ply_ptr, invisflag);   
           output(fd, "Your invisibility fades.\n");
           broadcast_rom(fd, ply_ptr->rom_num, "%M fades into view.",
                  m1arg(ply_ptr));
        }

        if(how == CAST) switch(osp->bonus_type) {
        case 1:
            bns = bonus[(int)ply_ptr->intelligence] +
                mprofic(ply_ptr, osp->realm)/10;
            break;
        case 2:
            bns = bonus[(int)ply_ptr->intelligence] +
                mprofic(ply_ptr, osp->realm)/6;
            break;
        case 3:
            bns = bonus[(int)ply_ptr->intelligence] +
                mprofic(ply_ptr, osp->realm)/4;
            break;
        }

        if(F_ISSET(ply_ptr->parent_rom, RWATER)) {
            switch(osp->realm) {
            case WATER: bns *= 2; break;
            case FIRE:  bns = MIN(-bns, -5); break;
            }
        }
        else if(F_ISSET(ply_ptr->parent_rom, RFIRER)) {
            switch(osp->realm) {
            case FIRE:  bns *= 2; break;
            case WATER: bns = MIN(-bns, -5); break;
            }  
        }
        else if(F_ISSET(ply_ptr->parent_rom, RWINDR)) {
            switch(osp->realm) {
            case WIND:  bns *= 2; break;
            case EARTH: bns = MIN(-bns, -5); break;
           }
        }
        else if(F_ISSET(ply_ptr->parent_rom, REARTH)) {
            switch(osp->realm) {
            case EARTH: bns *= 2; break;
            case WIND:  bns = MIN(-bns, -5); break;
            }
        }

        victims = count_vis_ply(ply_ptr->parent_rom) + count_vis_mon(ply_ptr->parent_rom) - 1;
	if (victims < 1) {
	   output(fd, "There is nothing to cast on.\n");
	   return(0);
	}

        dmg = (dice(osp->ndice, osp->sdice, osp->pdice+bns));

        sprintf(g_buffer, "You cast a %d point %s spell on all enemies.\n",
                dmg, spellname);
        output(fd, g_buffer);
	if(osp->splno == STORNA)
                broadcast_rom(fd, ply_ptr->rom_num,"%M summons a tornado!", m1arg(ply_ptr));
	else if(osp->splno == SINCIN)
                broadcast_rom(fd, ply_ptr->rom_num,"%M incinerates everyone nearby!", m1arg(ply_ptr));
	else if(osp->splno == STRMOR)
                broadcast_rom(fd, ply_ptr->rom_num,"%M makes the ground shake violently!", m1arg(ply_ptr));
	else if(osp->splno == SFLOOD)
                broadcast_rom(fd, ply_ptr->rom_num,"%M summons a torrent of water!", m1arg(ply_ptr));



	dmg = dmg / victims;
        dmg = MAX(1, dmg);
        totalrealm = 0;
        
   if(!F_ISSET(ply_ptr->parent_rom, RNOKIL) && F_ISSET(ply_ptr, PCHAOS)) {
	cp = ply_ptr->parent_rom->first_ply;


        while(cp){
           cp_tmp = cp->next_tag;

           if(!in_group(ply_ptr, cp->crt) && (ply_ptr != cp->crt) && 
			F_ISSET(cp->crt, PCHAOS)) {

              sprintf(g_buffer,
                     "%%M casts a %s spell on you for %d damage.\n",
                     spellname, dmg);
              mprint(cp->crt->fd, g_buffer, m1arg(ply_ptr));

              if(F_ISSET(cp->crt, PRMAGI))
                  cp->crt->hpcur -= (dmg * 2 * MIN(50, cp->crt->piety + cp->crt->intelligence)) / 100;
              else
                 cp->crt->hpcur -= dmg;
              cp->crt->hpcur = MIN(cp->crt->hpmax, cp->crt->hpcur);

              if(cp->crt->hpcur < 1) {
                mprint(fd, "You killed %m.\n", m1arg(cp->crt));
                broadcast_rom2(fd, cp->crt->fd, ply_ptr->rom_num,
                     "%M killed %m.", m2args(ply_ptr, cp->crt));
                die(cp->crt, ply_ptr);
//                return(2);
              }
	    }           
         cp = cp_tmp;
           
        }
    } /* RNOKIL */


	cp = ply_ptr->parent_rom->first_mon;

	   while(cp){
	      cp_tmp = cp->next_tag;

              if(is_crt_killable(cp->crt, ply_ptr)) {

                 if (F_ISSET(cp->crt, MRMAGI))
                    cp->crt->hpcur -= (dmg * 2 * MIN(50, cp->crt->piety + cp->crt->intelligence))/100;
	         else
	            cp->crt->hpcur -= dmg;
	         cp->crt->hpcur = MIN(cp->crt->hpmax, cp->crt->hpcur);

                 m = MIN(cp->crt->hpcur, dmg);
                 addrealm = (m * cp->crt->experience) / MAX(1,cp->crt->hpmax);
                 addrealm = MIN(addrealm, cp->crt->experience);
                 totalrealm = totalrealm + addrealm;


                 add_enm_crt(ply_ptr->name, cp->crt);	/* adds player to monster enemy list */

                 add_enm_dmg(ply_ptr->name, cp->crt, dmg);	/* add player to monster experince list */

    	         if(cp->crt->hpcur < 1) {
     	  	    mprint(fd, "You killed %m.\n", m1arg(cp->crt));
         	    broadcast_rom2(fd, cp->crt->fd, ply_ptr->rom_num,
                         	"%M killed %m.", m2args(ply_ptr, cp->crt));
            	    die(cp->crt, ply_ptr);
//		    return(2); 
	         }

	      }	// end of killable monsters

	      cp = cp_tmp;

	   }   // end of monster while


        totalrealm = MIN(1, (totalrealm));
        ply_ptr->realm[osp->realm-1] += totalrealm;


	return(0);
}
/***********************************************************************/
/*                              room_vigor                             */
/***********************************************************************/

int room_vigor(creature *ply_ptr, cmd *cmnd, int how )
{          
   	int     fd, heal;
	ctag	*cp;
	ctag	*cp_tmp;

	fd = ply_ptr->fd;
	
	if(how == POTION) {
		output(fd, "The spell fizzles.\n");
		return(0);
	}
	if(!S_ISSET(ply_ptr, SRVIGO) && (how != WAND || how != SCROLL)) {
		output(fd, "You don't know that spell.\n");
        	return(0);
    	}  
        if(ply_ptr->class != CLERIC && ply_ptr->class != BARD && ply_ptr->class < BUILDER) {
                output(fd, "Only clerics may cast that spell.\n");
				return(PROMPT);
        }
 	if(ply_ptr->mpcur < 12 && (how != WAND || how != SCROLL)) {
        	output(fd, "Not enough magic points.\n");
        			return(0);
    	}
	if(how == CAST)
		ply_ptr->mpcur-=12;
	if(spell_fail(ply_ptr, how)) {
                return(0);
        }

		cp = ply_ptr->parent_rom->first_ply;

		output(fd,"\nYou cast vigor on everyone in the room.\n");
		broadcast_rom(fd, ply_ptr->rom_num, 
			"\n%M casts vigor on everyone in the room.\n", 
			m1arg(ply_ptr));
		
		heal = mrand(1,6) + bonus[(int)ply_ptr->piety];            

		if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                	heal += mrand(1,3);
                	output(fd,"\nThe room's magical properties increase the power of your spell\n");
            	}		
		while(cp){
			cp_tmp = cp->next_tag;
			if(cp->crt->type != MONSTER) {
				if(cp->crt != ply_ptr) {
					mprint(cp->crt->fd,"\n%M casts vigor on you.\n",
						m1arg(ply_ptr));
				}
			cp->crt->hpcur += heal;
			cp->crt->hpcur = MIN(cp->crt->hpmax, cp->crt->hpcur);
			}
			cp = cp_tmp;
		}

	return(0);
}

/**********************************************************************/
/*              		remove blindness	              */
/**********************************************************************/
 
int rm_blind(creature *ply_ptr, cmd *cmnd, int how )
{
    room        *rom_ptr;
    creature    *crt_ptr;
    int     fd;
 
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;
 
    if(ply_ptr->mpcur < 12 && how == CAST) {
        output(fd, "Not enough magic points.\n");
        return(0);
    }
 
    if(ply_ptr->class != CLERIC && ply_ptr->class != PALADIN &&
       ply_ptr->class < BUILDER && how == CAST) {
            output(fd, "Only clerics and paladins may cast that spell.\n");
            return(0);
    }                         
 
    if(!S_ISSET(ply_ptr, SRMBLD) && how == CAST) {
        output(fd, "You don't know that spell.\n");
        return(0);
    }
    if(spell_fail(ply_ptr, how)) {
                if(how==CAST)
                        ply_ptr->mpcur -= 12;
                return(0);
        }
 
    if(cmnd->num == 2) {
 
        if(how == CAST)
            ply_ptr->mpcur -= 12;
 
        if(how == CAST || how == SCROLL || how == WAND)  {
            output(fd, "Cure blindness spell cast on yourself.\n");
            output(fd, "Your see the light!.\n");
            sprintf(g_buffer, "%%M casts cure blindness on %sself.", 
                      F_ISSET(ply_ptr, PMALES) ? "him":"her");
            broadcast_rom(fd, ply_ptr->rom_num, 
                      g_buffer, m1arg(ply_ptr));
        }
        else if(how == POTION && F_ISSET(ply_ptr, PBLIND))
            output(fd, "You feel a veil lifted from your eyes.\n");
        else if(how == POTION)
            output(fd, "Nothing happens.\n");
 
        F_CLR(ply_ptr, PBLIND);
 
    }
 
    else {
 
        if(how == POTION) {
            output(fd, "You can only use a potion on yourself.\n");
            return(0);
        }
 
        crt_ptr = find_crt_in_rom(ply_ptr, rom_ptr,
                   cmnd->str[2], cmnd->val[2], PLY_FIRST);
 
        if(!crt_ptr) {
            output(fd, "That's not here.\n");
            return(0);
        }
 
        if(how == CAST) 
            ply_ptr->mpcur -= 12;
 
        F_CLR(crt_ptr, PBLIND);
 
        if(how == CAST || how == SCROLL || how == WAND) {
            mprint(fd, "You cast cure blindness on %m.\n", 
				m1arg(crt_ptr));
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "%M casts cure blindness on %m.",
                       m2args(ply_ptr, crt_ptr));
            mprint(crt_ptr->fd, "%M casts cure blindness on you.\nYou feel a veil lifted from your eyes.\n", 
				m1arg(ply_ptr));
        }
 
    }
 
    return(1);
 
}
 

/**********************************************************************/
/*                              blind                                 */
/**********************************************************************/
/* The blind  spell prevents a player or monster from seeing. The spell     *
 * results  in a -5 penalty on attacks, and an inability look at objects *
 * players, rooms, or inventory.  Also a player or monster cannot read. */

int blind(creature *ply_ptr, cmd *cmnd, int how )
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int         fd;
 
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;
 
    if(ply_ptr->mpcur < 15 && how == CAST) {
        output(fd, "Not enough magic points.\n");
        return(0);
    }
 
    if(!S_ISSET(ply_ptr, SBLIND) && how == CAST) {
        output(fd, "You don't know that spell.\n");
        return(0);
    }
 
    if(F_ISSET(ply_ptr, PINVIS)) {
        F_CLR(ply_ptr, PINVIS);
        output(fd, "Your invisibility fades.\n");
        broadcast_rom(fd, ply_ptr->rom_num, "%M fades into view.",
                  m1arg(ply_ptr));
    }
    if(spell_fail(ply_ptr, how)) {
                if(how==CAST)
                        ply_ptr->mpcur -= 15;
                return(0);
        }
 
    if(how == CAST) {
        ply_ptr->mpcur -= 15;
	}
    /* blind self */
   if(cmnd->num == 2) {
		F_SET(ply_ptr,PBLIND);
 
        if(how == CAST || how == SCROLL || how == WAND)  {
            output(fd, "You are blind and can no longer see.\n");
            sprintf(g_buffer, "%%M casts blindness on %sself.", 
                      F_ISSET(ply_ptr, PMALES) ? "him":"her");
            broadcast_rom(fd, ply_ptr->rom_num, 
                      g_buffer, m1arg(ply_ptr));
        }
        else if(how == POTION)
            output(fd, "Everything goes dark.\n");
 
    }
 
    /* blind a monster or player */
    else {
        if(how == POTION) {
            output(fd, "You can only use a potion on yourself.\n");
            return(0);
        }
 
        crt_ptr = find_crt_in_rom(ply_ptr, rom_ptr, cmnd->str[2], 
			cmnd->val[2], MON_FIRST);
 
		if(!crt_ptr || crt_ptr == ply_ptr || 
			(crt_ptr->type == PLAYER && strlen(cmnd->str[2]) < 3)) {
			output(fd, "That's not here.\n");
			return(0);
		}
 
 
		if(!is_crt_killable(crt_ptr, ply_ptr)) 
		{
			return(0);
		}
 
		if (crt_ptr->type == PLAYER)
			F_SET(crt_ptr,PBLIND);
		else
			F_SET(crt_ptr,MBLIND);
 
        if(how == CAST || how == SCROLL || how == WAND) {
            sprintf(g_buffer, "Blindness spell cast on %s.\n", crt_ptr->name);
            output(fd, g_buffer);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "%M casts a blindness spell on %m.",
                       m2args(ply_ptr, crt_ptr));
            mprint(crt_ptr->fd, 
                  "%M casts a blindness spell on you.\n",
                  m1arg(ply_ptr));
        }
 
        if(crt_ptr->type != PLAYER){
			add_enm_crt(ply_ptr->name, crt_ptr);
 		}	
    }
 
    return(1);
 
}            

/**********************************************************************/
/*                               fear                                 */
/**********************************************************************/
/* The fear spell causes the monster to have a high wimpy / flee   *
 * percentage and a penality of -2 on all attacks */
 
int fear(creature *ply_ptr, cmd *cmnd, int how )
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int         fd;
    xtag	*xp;
 
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;
 
    if(ply_ptr->mpcur < 15 && how == CAST) {
        output(fd, "Not enough magic points.\n");
        return(0);
    }
 
    if(!S_ISSET(ply_ptr, SFEARS) && how == CAST) {
        output(fd, "You don't know that spell.\n");
        return(0);
    }
 
    if(F_ISSET(ply_ptr, PINVIS)) {
        F_CLR(ply_ptr, PINVIS);
        output(fd, "Your invisibility fades.\n");
        broadcast_rom(fd, ply_ptr->rom_num, "%M fades into view.",
                  m1arg(ply_ptr));
    }
 
    /* fear on self */
   if(cmnd->num == 2) {
	output(fd, "Cast fear on who?\n");
	return(0);
        }
 
 
    /* fear a monster or player */
    else {
        if(how == POTION) {
            output(fd, "You can only use a potion on yourself.\n");
            return(0);
        }
 
        crt_ptr = find_crt_in_rom(ply_ptr, rom_ptr, cmnd->str[2], 
			cmnd->val[2], MON_FIRST);
 
		if(!crt_ptr || crt_ptr == ply_ptr || 
			(crt_ptr->type == PLAYER && strlen(cmnd->str[2]) < 3)) {
			output(fd, "That's not here.\n");
			return(0);
		}
		if(crt_ptr->class >= IMMORTAL) {
			output(fd, "Don't be foolish!\n");
			return(0);
		}
 
 
		if(!is_crt_killable(crt_ptr, ply_ptr)) 
		{
			return(0);
		}
 
	ply_ptr->mpcur -= 15;
 
        if(crt_ptr->type != PLAYER && F_ISSET(crt_ptr, MBLOCK)) {
            mprint(fd, "%M seems unaffected by fear.\n", m1arg(crt_ptr));
	    add_enm_crt(ply_ptr->name, crt_ptr);
            return(0);
        }

	if(crt_ptr->class == PALADIN) {
		mprint(fd, "%M seems unaffected by fear.\n", m1arg(crt_ptr));
		return(0);
	}
	
	if(crt_ptr->type == PLAYER) {
		if(!F_ISSET(ply_ptr, PCHAOS) && ply_ptr->class < DM) {
            		output(fd, "You are Lawful!\n");
            		return(0);
        	}

        	if(!F_ISSET(crt_ptr, PCHAOS) && ply_ptr->class < DM) {
            		output(fd, "That player is Lawful!\n");
            		return(0);
        	}
	}

        if((crt_ptr->type == PLAYER && F_ISSET(crt_ptr, PRMAGI)) ||
           (crt_ptr->type != PLAYER && F_ISSET(crt_ptr, MRMAGI))) {
			output(fd, "Your spell has no effect.\n");
		if(crt_ptr->type != PLAYER)
			 add_enm_crt(ply_ptr->name, crt_ptr);
			return(0);
		}
	if(spell_fail(ply_ptr, how)) {
		add_enm_crt(ply_ptr->name, crt_ptr);
		return(0);
	}
 
        if(how == CAST || how == SCROLL || how == WAND) {
            sprintf(g_buffer, "Fear spell cast on %s.\n", crt_ptr->name);
            output(fd, g_buffer);
	    if(crt_ptr->type == PLAYER) {
               broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, "%M casts fear on %m.",
                       m2args(ply_ptr, crt_ptr));
               mprint(crt_ptr->fd, "%M casts a fear spell on you.\n",
                  m1arg(ply_ptr));
            }
        }
 
	/* Cast on monster */
	if (crt_ptr->type != PLAYER) {
    	    xp = rom_ptr->first_ext;
    	    while(xp) {
            if(!F_ISSET(xp->ext, XSECRT) && !F_ISSET(xp->ext, XCLOSD) &&
           	is_rom_loaded(xp->ext->room) && mrand(1,100) < 75) {

            	sprintf(g_buffer, "%%M flees to the %s.", xp->ext->name);
            	broadcast_rom(-1, rom_ptr->rom_num, g_buffer, m1arg(crt_ptr));
            	del_crt_rom(crt_ptr, rom_ptr);

		/* is_rom_loaded used above, no need to check */
            	load_rom(xp->ext->room, &rom_ptr);
            	add_crt_rom(crt_ptr, rom_ptr, 1);

#ifndef MOBS_ALWAYS_ACTIVE
/*            if(!rom_ptr->first_ply) del_active(crt_ptr); */
#endif
		add_enm_crt(ply_ptr->name, crt_ptr);
            	return(1);
 	      }
            xp = xp->next_tag;
    	    } /* end while */
	    sprintf(g_buffer, "%s fails to escape.", crt_ptr->name);
	    broadcast_rom(-1, rom_ptr->rom_num, g_buffer,m1arg(crt_ptr));
	    add_enm_crt(ply_ptr->name, crt_ptr);

    	} else   /* cast on player */
			flee(crt_ptr, NULL);
    	}
 
    return(1);
}            


/**********************************************************************/
/*                            silence                                 */
/**********************************************************************/
/* Silence  causes a player or monster to lose their voice, makin them */
/* unable to casts spells, use scrolls, speak, yell, or broadcast */

int silence(creature *ply_ptr, cmd *cmnd, int how )
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int         fd, dur;
 
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;
 
    if(ply_ptr->mpcur < 12 && how == CAST) {
        output(fd, "Not enough magic points.\n");
        return(0);
    }
 
    if(!S_ISSET(ply_ptr, SSILNC) && how == CAST) {
        output(fd, "You don't know that spell.\n");
        return(0);
    }
 
    if(F_ISSET(ply_ptr, PINVIS)) {
        F_CLR(ply_ptr, PINVIS);
        output(fd, "Your invisibility fades.\n");
        broadcast_rom(fd, ply_ptr->rom_num, "%M fades into view.",
                  m1arg(ply_ptr));
    }
 
    if(how == CAST) {
        dur =  300 + mrand(1,30)*10  + bonus[(int)ply_ptr->intelligence]*150;
        ply_ptr->mpcur -= 12;
	}
    else if (how == SCROLL)
        dur =  300 + mrand(1,15)*10  + bonus[(int)ply_ptr->intelligence]*75;
    else 
        dur = 300 + mrand(1,15)*10;

        if(spell_fail(ply_ptr, how)) {
                return(0);
        }
 
    /* silence on self */
   if(cmnd->num == 2) {
        if (F_ISSET(ply_ptr,PRMAGI))
                dur /= 2;
 
        ply_ptr->lasttime[LT_SILNC].ltime = time(0);
        ply_ptr->lasttime[LT_SILNC].interval = dur;
		F_SET(ply_ptr,PSILNC);
 
        if(how == CAST || how == SCROLL || how == WAND)  {
            output(fd, "Your voice begins to fade.\n");
            sprintf(g_buffer, "%%M casts silence on %sself.", 
                      F_ISSET(ply_ptr, PMALES) ? "him":"her");

            broadcast_rom(fd, ply_ptr->rom_num, 
                      g_buffer, m1arg(ply_ptr));
        }
        else if(how == POTION)
            output(fd, "Your throat goes dry and you cannot speak.\n");
 
    }
 
    /* silence a monster or player */
    else {
        if(how == POTION) {
            output(fd, "You can only use a potion on yourself.\n");
            return(0);
        }
 
        crt_ptr = find_crt_in_rom(ply_ptr, rom_ptr, cmnd->str[2], 
			cmnd->val[2], MON_FIRST);
 
        if(!crt_ptr || crt_ptr == ply_ptr || 
               (crt_ptr->type == PLAYER && strlen(cmnd->str[2]) < 3)) {
                output(fd, "That's not here.\n");
                return(0);
        }
 
		if(!is_crt_killable(crt_ptr, ply_ptr)) 
		{
			return(0);
		}
 
        if((crt_ptr->type == PLAYER && F_ISSET(crt_ptr, PRMAGI)) ||
           (crt_ptr->type != PLAYER && F_ISSET(crt_ptr, MRMAGI)))
            dur /= 2;
 
        crt_ptr->lasttime[LT_SILNC].ltime = time(0);
        crt_ptr->lasttime[LT_SILNC].interval = dur;
		if (crt_ptr->type == PLAYER)
			F_SET(crt_ptr,PSILNC);
		else
			F_SET(crt_ptr,MSILNC);
 
        if(how == CAST || how == SCROLL || how == WAND) {
            sprintf(g_buffer, "Silence casted on %s.\n", crt_ptr->name);
            output(fd, g_buffer);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "%M casts a silence spell on %m.",
                       m2args(ply_ptr, crt_ptr));
            mprint(crt_ptr->fd, 
                  "%M casts a silence spell on you.\n",
                  m1arg(ply_ptr));
        }
 
        if(crt_ptr->type != PLAYER){
            add_enm_crt(ply_ptr->name, crt_ptr);
		} 
    }
 
    return(1);
 
}            
              
/**********************************************************************/
/*                      remove blindness                              */
/**********************************************************************/
 
int rm_blindness(creature *ply_ptr, cmd *cmnd, int how )
{
    room        *rom_ptr;
    creature    *crt_ptr;
    int     fd;
 
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;
 
    if(ply_ptr->mpcur < 12 && how == CAST) {
        output(fd, "Not enough magic points.\n");
        return(0);
    }
 
        if(ply_ptr->class != CLERIC && ply_ptr->class != PALADIN &&
           ply_ptr->class < BUILDER && how == CAST) {
                output(fd, "Only clerics and paladins may cast that spell.\n");
                return(0);
        }                         
 
    if(!S_ISSET(ply_ptr, SRMDIS) && how == CAST) {
        output(fd, "You don't know that spell.\n");
        return(0);
    }
    if(spell_fail(ply_ptr, how)) {
                if(how==CAST)
                        ply_ptr->mpcur -= 12;
                return(0);
        }

 
    if(cmnd->num == 2) {
 
        if(how == CAST)
            ply_ptr->mpcur -= 12;
 
        if(how == CAST || how == SCROLL || how == WAND)  {
            output(fd, "Remove blindness spell cast on yourself.\n");
            output(fd, "You can see.\n");

            sprintf(g_buffer, "%%M casts remove blindness on %sself.", 
                      F_ISSET(ply_ptr, PMALES) ? "him":"her");
            broadcast_rom(fd, ply_ptr->rom_num, 
                      g_buffer, m1arg(ply_ptr));
        }
        else if(how == POTION && F_ISSET(ply_ptr, PBLIND))
            output(fd, "You can see.\n");
        else if(how == POTION)
            output(fd, "Nothing happens.\n");
 
        F_CLR(ply_ptr, PBLIND);
 
    }
 
    else {
 
        if(how == POTION) {
            output(fd, "You can only use a potion on yourself.\n");
            return(0);
        }
 
        crt_ptr = find_crt_in_rom(ply_ptr, rom_ptr,
                   cmnd->str[2], cmnd->val[2], PLY_FIRST);
 
        if(!crt_ptr) {
            output(fd, "That's not here.\n");
            return(0);
        }
 
        if(how == CAST) 
            ply_ptr->mpcur -= 12;
 
		if (crt_ptr->type == PLAYER)
			F_CLR(crt_ptr,PBLIND);
		else
			F_CLR(crt_ptr,MBLIND);
 
        if(how == CAST || how == SCROLL || how == WAND) {
            mprint(fd, "You cast the remove blindness on %m.\n", 
				m1arg(crt_ptr));
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "%M casts remove blindness on %m.",
                       m2args(ply_ptr, crt_ptr));
            mprint(crt_ptr->fd, "%M casts remove blindness on you.\nYou can see.\n", 
				m1arg(ply_ptr));
        }
 
    }
 
    return(1);
 
}

/****************************************************************************/
/*			spell_fail					    */
/****************************************************************************/

/* This function returns 1 if the casting of a spell fails, and 0 if it is  */
/* sucessful.								    */

int spell_fail( creature *ply_ptr, int how )
{

int	chance, fd, n;

	if(how == POTION)
		return(0);

	if(ply_ptr->type != PLAYER)
		return(0);

	fd=ply_ptr->fd;
	n = mrand(1,100);
	luck(ply_ptr);

switch(ply_ptr->class) {

        case ASSASSIN:
                chance = ((ply_ptr->level+bonus[(int)ply_ptr->intelligence])*5)+30; 
		chance *= Ply[fd].extr->luck/50;
		if(n>chance) {
			output(fd,"Your spell fails.\n");
			return(1);
		}
		else
			return(0);

        case BARBARIAN:
                chance = ((ply_ptr->level+bonus[(int)ply_ptr->intelligence])*5); 
		chance *= Ply[fd].extr->luck/50;
		if(n>chance) {
                        output(fd,"Your spell fails.\n");
                        return(1);
                }
                else
                        return(0);

        case BARD:
                chance = ((ply_ptr->level+bonus[(int)ply_ptr->intelligence])*5)+60; 
		chance *= Ply[fd].extr->luck/50;
		if(n>chance) {
                        output(fd,"Your spell fails.\n");
                        return(1);
                }
                else
                        return(0);
	case CLERIC:
		chance = ((ply_ptr->level+bonus[(int)ply_ptr->intelligence])*5)+65; 
		chance *= Ply[fd].extr->luck/50;
		if(n>chance) {
			output(fd,"Your spell fails.\n");
			return(1);
		}
		else
			return(0);
	case FIGHTER:
		chance = ((ply_ptr->level+bonus[(int)ply_ptr->intelligence])*5)+10; 
		chance *= Ply[fd].extr->luck/50;
		if(n>chance) {
			output(fd,"Your spell fails.\n");
			return(1);
		}
		else
			return(0);
	case MAGE:
		chance = ((ply_ptr->level+bonus[(int)ply_ptr->intelligence])*5)+75; 
		chance *= Ply[fd].extr->luck/50;
		if(n>chance) {
			output(fd,"Your spell fails.\n");
			return(1);
		}
		else
			return(0);
	case MONK:
		chance = ((ply_ptr->level+bonus[(int)ply_ptr->intelligence])*6)+25; 
		chance *= Ply[fd].extr->luck/50;
		if(n>chance) {
			output(fd,"Your spell fails.\n");
			return(1);
		}
		else
			return(0);
	case PALADIN:
		chance = ((ply_ptr->level+bonus[(int)ply_ptr->intelligence])*5)+50; 
		chance *= Ply[fd].extr->luck/50;
		if(n>chance) {
			output(fd,"Your spell fails.\n");
			return(1);
		}
		else
			return(0);
	case RANGER:
		chance = ((ply_ptr->level+bonus[(int)ply_ptr->intelligence])*4)+56; 
		chance *= Ply[fd].extr->luck/50;
		if(n>chance) {
			output(fd,"Your spell fails.\n");
			return(1);
		}
		else
			return(0);
	case THIEF:
		chance = ((ply_ptr->level+bonus[(int)ply_ptr->intelligence])*6)+22; 
		chance *= Ply[fd].extr->luck/50;
		if(n>chance) {
			output(fd,"Your spell fails.\n");
			return(1);
		}
		else
			return(0);
	case DRUID:
		chance = ((ply_ptr->level+bonus[(int)ply_ptr->intelligence])*5)+65;
		chance *= Ply[fd].extr->luck/50;
		if(n>chance) {
			output(fd,"Your spell fails.\n");
			return(1);
		}
		else    
			return(0);
	case ALCHEMIST:
		chance = ((ply_ptr->level+bonus[(int)ply_ptr->intelligence])*5)+70;
		chance *= Ply[fd].extr->luck/50;
		if(n>chance) {
			output(fd,"Your spell fails.\n");
			return(1);
		}   
		else
			return(0);

	default:
		return(0);
    }
}

/****************************************************************************/
/*                      fortune                                            */
/****************************************************************************/
        
/* This allows bards to tell the luck of a given player.		*/

int fortune(creature *ply_ptr, cmd *cmnd, int how )
{

int		fd, luk;
creature	*crt_ptr;
room		*rom_ptr;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;
	
	if(ply_ptr->class != BARD && ply_ptr->class < BUILDER) {
		output(fd, "Only bards may do that.\n");
		return(0);
	}

	if(ply_ptr->mpcur < 12 && how == CAST) {
        	output(fd, "Not enough magic points.\n");
        	return(0);
	}

	if(!S_ISSET(ply_ptr, SFORTU) && how == CAST) {
        	output(fd, "You don't know that spell.\n");
        	return(0);
    }
    	
	if(spell_fail(ply_ptr, how)) {
		if(how==CAST)
				ply_ptr->mpcur -= 12;
		return(0);
    }       

    if(cmnd->num == 2) {

		if(how == CAST)
            ply_ptr->mpcur -= 12;

        if(how == CAST || how == SCROLL || how == WAND)  {
            output(fd, "Fortune spell cast on yourself.\n");
			luk = Ply[fd].extr->luck/10;
			luk = MAX(luk, 1);
			switch(luk) {
				case 1:
					output(fd, "Your death will be tragic.\n.");
					break;
				case 2:
						output(fd, "A black cat must have crossed your path.\n");
						break;
				case 3:
						output(fd, "If it weren't for bad luck you'd have no luck at all.\n");
						break;
				case 4:
						output(fd, "Your karma is imbalanced.\n");
						break;
				case 5:
						output(fd, "Your future is uncertain.\n");
						break;
				case 6:
						output(fd, "Without intervention you may find yourself in a dire situation.\n");
						break;
				case 7:
						output(fd, "Long range prospects look good.\n");
						break;
				case 8:
						output(fd, "Count your blessings, for others are less fortunate.\n");
						break;
				case 9:
						output(fd, "The fates have smiled upon you.\n");
						break;
				case 10:
						output(fd, "Your death would be tragic.\n");
						break;
				default:
					output (fd, "You can't tell right now.\n");
			}

            sprintf(g_buffer, "%%M reads %s aura.",
	            F_ISSET(ply_ptr, PMALES) ? "his":"her");
            broadcast_rom(fd, ply_ptr->rom_num,
                      g_buffer, m1arg(ply_ptr));

        }
        else if(how == POTION)
            output(fd, "Nothing happens.\n"); 

 
    }
	else {
       
        if(how == POTION) {
            output(fd, "You can only use a potion on yourself.\n");
            return(0);
        }                       

        crt_ptr = find_crt_in_rom(ply_ptr, rom_ptr,
                   cmnd->str[2], cmnd->val[2], PLY_FIRST);
    
        if(!crt_ptr) {
            output(fd, "That's not here.\n");
            return(0);
        }

		if(crt_ptr->type == MONSTER)
			luk = crt_ptr->alignment/10;
		else 
			luk = Ply[crt_ptr->fd].extr->luck/10;
		
		luk = MAX(luk, 1);
        sprintf(g_buffer, "Fortune spell cast on %s.\n", crt_ptr->name);
        output(fd, g_buffer);

        switch(luk) {
            case 1:
                    mprint(fd, "%M's death will be swift and certain.\n", m1arg(crt_ptr));
                    break;
            case 2:
                    mprint(fd, "You sense %M's karma is imbalanced.\n", m1arg(crt_ptr));
                    break;
            case 3:
                    mprint(fd, "A black cat must have crossed %M's path.\n", m1arg(crt_ptr));
                    break;
            case 4:
                    mprint(fd, "%M's aura reeks of danger.\n", m1arg(crt_ptr));
                    break;
            case 5:
                    mprint(fd, "Without intervention %M may end up in a dire situation.\n", m1arg(crt_ptr));
					break;
            case 6:
                    mprint(fd, "%M's future is uncertain.\n", m1arg(crt_ptr));
                    break;
            case 7:
                    mprint(fd, "Long range prospects look good for %M.\n", m1arg(crt_ptr));
                    break;
            case 8:
                    mprint(fd, "%M should count their blessings, as others are less fortunate.\n", m1arg(crt_ptr));
                    break;
            case 9:
                    mprint(fd, "The fates smile upon %M.\n", m1arg(crt_ptr));
                    break;
            case 10:
                    mprint(fd, "%M's death would be tragic and unexpected.\n", m1arg(crt_ptr));
                    break;
            default:
                    output (fd, "You can't tell right now.\n");
            }
		sprintf(g_buffer, "%%M reads %s's aura.\n", crt_ptr->name); 
		broadcast_rom(fd, ply_ptr->rom_num, g_buffer, m1arg(ply_ptr)); 
		return(0);
	}
	

	return(0);
}		
