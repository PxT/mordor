/*
 * 	MAGIC8.C:
 *
 *  	Additional spell-casting routines.
 *	(C) 1994, 1995 Brooke Paul, Brett Vickers
 */

#include "mstruct.h"
#include "mextern.h"
#ifdef DMALLOC
  #include "/usr/local/include/dmalloc.h"
#endif
/***********************************************************************/
/*                              room_vigor                             */
/***********************************************************************/

int room_vigor(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd         *cmnd;
int			how;
{          
   	int     c = 0, fd, heal;
	ctag	*cp;
	ctag	*cp_tmp;

	fd = ply_ptr->fd;
	
	if(how == POTION) {
		print(fd, "The spell fizzles.\n");
		return(0);
	}
	if(!S_ISSET(ply_ptr, SRVIGO) && (how != WAND || how != SCROLL)) {
        	print(fd, "You don't know that spell.\n");
        	return(0);
    	}  
        if(ply_ptr->class != CLERIC && ply_ptr->class < CARETAKER) {
                print(fd, "Only clerics may cast that spell.\n");
				return(PROMPT);
        }
 	if(ply_ptr->mpcur < 12 && (how != WAND || how != SCROLL)) {
        	print(fd, "Not enough magic points.\n");
        			return(0);
    	}
	if(how == CAST)
		ply_ptr->mpcur-=12;
	if(spell_fail(ply_ptr, how)) {
                return(0);
        }

		cp = ply_ptr->parent_rom->first_ply;

		print(fd,"\nYou cast vigor on everyone in the room.\n");
		broadcast_rom(fd, ply_ptr->rom_num, "\n%M casts vigor on everyone in the room.\n", ply_ptr);
		
		heal = mrand(1,6) + bonus[ply_ptr->piety];            

		if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                	heal += mrand(1,3);
                	print(fd,"\nThe room's magical properties increase the power of your spell\n");
            	}		
		while(cp){
			cp_tmp = cp->next_tag;
			if(cp->crt->type != MONSTER) {
				if(cp->crt != ply_ptr) {
					print(cp->crt->fd,"\n%M casts vigor on you.\n",ply_ptr);
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
 
int rm_blind(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
{
    room        *rom_ptr;
    creature    *crt_ptr;
    int     fd;
 
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;
 
    if(ply_ptr->mpcur < 12 && how == CAST) {
        print(fd, "Not enough magic points.\n");
        return(0);
    }
 
    if(ply_ptr->class != CLERIC && ply_ptr->class != PALADIN &&
       ply_ptr->class < CARETAKER && how == CAST) {
            print(fd, "Only clerics and paladins may cast that spell.\n");
            return(0);
    }                         
 
    if(!S_ISSET(ply_ptr, SRMBLD) && how == CAST) {
        print(fd, "You don't know that spell.\n");
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
            print(fd, "Cure blindness spell cast on yourself.\n");
            print(fd, "Your see the light!.\n");
            broadcast_rom(fd, ply_ptr->rom_num, 
                      "%M casts cure blindness on %sself.", 
                      ply_ptr,
                      F_ISSET(ply_ptr, PMALES) ? "him":"her");
        }
        else if(how == POTION & F_ISSET(ply_ptr, PBLIND))
            print(fd, "You feel a veil lifted from your eyes.\n");
        else if(how == POTION)
            print(fd, "Nothing happens.\n");
 
        F_CLR(ply_ptr, PBLIND);
 
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
            cmnd->str[2][0] = low(cmnd->str[2][0]);
            crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                       cmnd->str[2], cmnd->str[2]);
 
            if(!crt_ptr) {
                print(fd, "That's not here.\n");
                return(0);
            }
        }
 
        if(how == CAST) 
            ply_ptr->mpcur -= 12;
 
        F_CLR(crt_ptr, PBLIND);
 
        if(how == CAST || how == SCROLL || how == WAND) {
            print(fd, "You cast cure blindness on %m.\n", crt_ptr);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "%M casts cure blindness on %m.",
                       ply_ptr, crt_ptr);
            print(crt_ptr->fd, "%M casts cure blindness on you.\nYou feel a veil lifted from your eyes.\n", ply_ptr);
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

int blind(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int         fd;
 
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;
 
    if(ply_ptr->mpcur < 15 && how == CAST) {
        print(fd, "Not enough magic points.\n");
        return(0);
    }
 
    if(!S_ISSET(ply_ptr, SBLIND) && how == CAST) {
        print(fd, "You don't know that spell.\n");
        return(0);
    }
 
    if(F_ISSET(ply_ptr, PINVIS)) {
        F_CLR(ply_ptr, PINVIS);
        print(fd, "Your invisibility fades.\n");
        broadcast_rom(fd, ply_ptr->rom_num, "%M fades into view.",
                  ply_ptr);
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
            print(fd, "You are blind and can no longer see.\n");
            broadcast_rom(fd, ply_ptr->rom_num, 
                      "%M casts blindness on %sself.", 
                      ply_ptr,
                      F_ISSET(ply_ptr, PMALES) ? "him":"her");
        }
        else if(how == POTION)
            print(fd, "Everything goes dark.\n");
 
    }
 
    /* blind a monster or player */
    else {
        if(how == POTION) {
            print(fd, "You can only use a potion on yourself.\n");
            return(0);
        }
 
        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                   cmnd->str[2], cmnd->val[2]);
 
        if(!crt_ptr) {
            cmnd->str[2][0] = up(cmnd->str[2][0]);
            crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                       cmnd->str[2], cmnd->val[2]);
 
            if(!crt_ptr || crt_ptr == ply_ptr || 
               strlen(cmnd->str[2]) < 3) {
                print(fd, "That's not here.\n");
                return(0);
            }
 
        }
 
        if(crt_ptr->type != PLAYER && F_ISSET(crt_ptr, MUNKIL)) {
            print(fd, "You cannot harm %s.\n",
                F_ISSET(crt_ptr, MMALES) ? "him":"her");
            return(0);
        }
 
		if (crt_ptr->type == PLAYER)
			F_SET(crt_ptr,PBLIND);
		else
			F_SET(crt_ptr,MBLIND);
 
        if(how == CAST || how == SCROLL || how == WAND) {
            print(fd, "Blindness spell cast on %s.\n", crt_ptr->name);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "%M casts a blindness spell on %m.",
                       ply_ptr, crt_ptr);
            print(crt_ptr->fd, 
                  "%M casts a blindness spell on you.\n",
                  ply_ptr);
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
 
int fear(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int         fd, dur;
 
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;
 
    if(ply_ptr->mpcur < 15 && how == CAST) {
        print(fd, "Not enough magic points.\n");
        return(0);
    }
 
    if(!S_ISSET(ply_ptr, SFEARS) && how == CAST) {
        print(fd, "You don't know that spell.\n");
        return(0);
    }
 
    if(F_ISSET(ply_ptr, PINVIS)) {
        F_CLR(ply_ptr, PINVIS);
        print(fd, "Your invisibility fades.\n");
        broadcast_rom(fd, ply_ptr->rom_num, "%M fades into view.",
                  ply_ptr);
    }
 
    if(how == CAST) {
        dur =  600 + mrand(1,30)*10  + bonus[ply_ptr->intelligence]*150;
        ply_ptr->mpcur -= 15;
	}
    else if (how == SCROLL)
        dur =  600 + mrand(1,15)*10  + bonus[ply_ptr->intelligence]*50;
    else 
        dur = 600 + mrand(1,30)*10;

	if(spell_fail(ply_ptr, how)) {
                return(0);
        }
 
    /* fear on self */
   if(cmnd->num == 2) {
        if (F_ISSET(ply_ptr,PRMAGI))
                dur /= 2;
 
        ply_ptr->lasttime[LT_FEARS].ltime = time(0);
        ply_ptr->lasttime[LT_FEARS].interval = dur;
		F_SET(ply_ptr,PFEARS);
 
        if(how == CAST || how == SCROLL || how == WAND)  {
            print(fd, "You begin to shake in terror.\n");
            broadcast_rom(fd, ply_ptr->rom_num, 
                      "%M casts fear on %sself.", 
                      ply_ptr,
                      F_ISSET(ply_ptr, PMALES) ? "him":"her");
        }
        else if(how == POTION)
            print(fd, "You begin to shake in terror.\n");
 
    }
 
    /* fear a monster or player */
    else {
        if(how == POTION) {
            print(fd, "You can only use a potion on yourself.\n");
            return(0);
        }
 
        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                   cmnd->str[2], cmnd->val[2]);
 
        if(!crt_ptr) {
            cmnd->str[2][0] = up(cmnd->str[2][0]);
            crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                       cmnd->str[2], cmnd->val[2]);
 
            if(!crt_ptr || crt_ptr == ply_ptr || 
               strlen(cmnd->str[2]) < 3) {
                print(fd, "That's not here.\n");
                return(0);
            }
 
        }
 
        if(crt_ptr->type != PLAYER && F_ISSET(crt_ptr, MUNKIL)) {
            print(fd, "You cannot harm %s.\n",
                F_ISSET(crt_ptr, MMALES) ? "him":"her");
            return(0);
        }
 
        if(crt_ptr->type != PLAYER && F_ISSET(crt_ptr, MPERMT)) {
            print(fd, "%M seems unaffected by fear.\n",crt_ptr);
            return(0);
        }

        if((crt_ptr->type == PLAYER && F_ISSET(crt_ptr, PRMAGI)) ||
           (crt_ptr->type != PLAYER && F_ISSET(crt_ptr, MRMAGI)))
            dur /= 2;
 
        crt_ptr->lasttime[LT_FEARS].ltime = time(0);
        crt_ptr->lasttime[LT_FEARS].interval = dur;
		if (crt_ptr->type == PLAYER)
			F_SET(crt_ptr,PFEARS);
		else
			F_SET(crt_ptr,MFEARS);
 
        if(how == CAST || how == SCROLL || how == WAND) {
            print(fd, "Fear spell cast on %s.\n", crt_ptr->name);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "%M casts fear on %m.",
                       ply_ptr, crt_ptr);
            print(crt_ptr->fd, 
                  "%M casts a fear spell on you.\n",
                  ply_ptr);
        }
 
        if(crt_ptr->type != PLAYER){
		add_enm_crt(ply_ptr->name, crt_ptr);
 	}
    }
 
    return(1);
 
}            


/**********************************************************************/
/*                            silence                                 */
/**********************************************************************/
/* Silence  causes a player or monster to lose their voice, makin them */
/* unable to casts spells, use scrolls, speak, yell, or broadcast */

int silence(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int         fd, dur;
 
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;
 
    if(ply_ptr->mpcur < 12 && how == CAST) {
        print(fd, "Not enough magic points.\n");
        return(0);
    }
 
    if(!S_ISSET(ply_ptr, SSILNC) && how == CAST) {
        print(fd, "You don't know that spell.\n");
        return(0);
    }
 
    if(F_ISSET(ply_ptr, PINVIS)) {
        F_CLR(ply_ptr, PINVIS);
        print(fd, "Your invisibility fades.\n");
        broadcast_rom(fd, ply_ptr->rom_num, "%M fades into view.",
                  ply_ptr);
    }
 
    if(how == CAST) {
        dur =  300 + mrand(1,30)*10  + bonus[ply_ptr->intelligence]*150;
        ply_ptr->mpcur -= 12;
	}
    else if (how == SCROLL)
        dur =  300 + mrand(1,15)*10  + bonus[ply_ptr->intelligence]*75;
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
            print(fd, "Your voice begins to fade.\n");
            broadcast_rom(fd, ply_ptr->rom_num, 
                      "%M casts silence on %sself.", 
                      ply_ptr,
                      F_ISSET(ply_ptr, PMALES) ? "him":"her");
        }
        else if(how == POTION)
            print(fd, "Your throat goes dry and you cannot speak.\n");
 
    }
 
    /* silence a monster or player */
    else {
        if(how == POTION) {
            print(fd, "You can only use a potion on yourself.\n");
            return(0);
        }
 
        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                   cmnd->str[2], cmnd->val[2]);
 
        if(!crt_ptr) {
            cmnd->str[2][0] = up(cmnd->str[2][0]);
            crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                       cmnd->str[2], cmnd->val[2]);
 
            if(!crt_ptr || crt_ptr == ply_ptr || 
               strlen(cmnd->str[2]) < 3) {
                print(fd, "That's not here.\n");
                return(0);
            }
 
        }
 
        if(crt_ptr->type != PLAYER && F_ISSET(crt_ptr, MUNKIL)) {
            print(fd, "You cannot harm %s.\n",
                F_ISSET(crt_ptr, MMALES) ? "him":"her");
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
            print(fd, "Silence casted on %s.\n", crt_ptr->name);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "%M casts a silence spell on %m.",
                       ply_ptr, crt_ptr);
            print(crt_ptr->fd, 
                  "%M casts a silence spell on you.\n",
                  ply_ptr);
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
 
int rm_blindness(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
{
    room        *rom_ptr;
    creature    *crt_ptr;
    int     fd;
 
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;
 
    if(ply_ptr->mpcur < 12 && how == CAST) {
        print(fd, "Not enough magic points.\n");
        return(0);
    }
 
        if(ply_ptr->class != CLERIC && ply_ptr->class != PALADIN &&
           ply_ptr->class < CARETAKER && how == CAST) {
                print(fd, "Only clerics and paladins may cast that spell.\n");
                return(0);
        }                         
 
    if(!S_ISSET(ply_ptr, SRMDIS) && how == CAST) {
        print(fd, "You don't know that spell.\n");
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
            print(fd, "Remove blindness spell cast on yourself.\n");
            print(fd, "You can see.\n");
            broadcast_rom(fd, ply_ptr->rom_num, 
                      "%M casts remove blindness on %sself.", 
                      ply_ptr,
                      F_ISSET(ply_ptr, PMALES) ? "him":"her");
        }
        else if(how == POTION & F_ISSET(ply_ptr, PBLIND))
            print(fd, "You can see.\n");
        else if(how == POTION)
            print(fd, "Nothing happens.\n");
 
        F_CLR(ply_ptr, PBLIND);
 
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
            cmnd->str[2][0] = low(cmnd->str[2][0]);
            crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                       cmnd->str[2], cmnd->val[2]);
 
            if(!crt_ptr) {
                print(fd, "That's not here.\n");
                return(0);
            }
        }
 
        if(how == CAST) 
            ply_ptr->mpcur -= 12;
 
		if (crt_ptr->type == PLAYER)
			F_CLR(crt_ptr,PBLIND);
		else
			F_CLR(crt_ptr,MBLIND);
 
        if(how == CAST || how == SCROLL || how == WAND) {
            print(fd, "You cast the remove blindness on %m.\n", crt_ptr);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "%M casts remove blindness on %m.",
                       ply_ptr, crt_ptr);
            print(crt_ptr->fd, "%M casts remove blindness on you.\nYou can see.\n", ply_ptr);
        }
 
    }
 
    return(1);
 
}

/****************************************************************************/
/*			spell_fail					    */
/****************************************************************************/

/* This function returns 1 if the casting of a spell fails, and 0 if it is  */
/* sucessful.								    */

int spell_fail(ply_ptr, how)
creature	*ply_ptr;
int		 how;
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
                chance = ((ply_ptr->level+bonus[ply_ptr->intelligence])*5)+30; 
		chance *= Ply[fd].extr->luck/50;
		if(n>chance) {
			print(fd,"Your spell fails.\n");
			return(1);
		}
		else
			return(0);

        case BARBARIAN:
                chance = ((ply_ptr->level+bonus[ply_ptr->intelligence])*5); 
		chance *= Ply[fd].extr->luck/50;
		if(n>chance) {
                        print(fd,"Your spell fails.\n");
                        return(1);
                }
                else
                        return(0);

        case BARD:
                chance = ((ply_ptr->level+bonus[ply_ptr->intelligence])*5)+60; 
		chance *= Ply[fd].extr->luck/50;
		if(n>chance) {
                        print(fd,"Your spell fails.\n");
                        return(1);
                }
                else
                        return(0);
	case CLERIC:
		chance = ((ply_ptr->level+bonus[ply_ptr->intelligence])*5)+65; 
		chance *= Ply[fd].extr->luck/50;
		if(n>chance) {
			print(fd,"Your spell fails.\n");
			return(1);
		}
		else
			return(0);
	case FIGHTER:
		chance = ((ply_ptr->level+bonus[ply_ptr->intelligence])*5)+10; 
		chance *= Ply[fd].extr->luck/50;
		if(n>chance) {
			print(fd,"Your spell fails.\n");
			return(1);
		}
		else
			return(0);
	case MAGE:
		chance = ((ply_ptr->level+bonus[ply_ptr->intelligence])*5)+75; 
		chance *= Ply[fd].extr->luck/50;
		if(n>chance) {
			print(fd,"Your spell fails.\n");
			return(1);
		}
		else
			return(0);
	case MONK:
		chance = ((ply_ptr->level+bonus[ply_ptr->intelligence])*6)+25; 
		chance *= Ply[fd].extr->luck/50;
		if(n>chance) {
			print(fd,"Your spell fails.\n");
			return(1);
		}
		else
			return(0);
	case PALADIN:
		chance = ((ply_ptr->level+bonus[ply_ptr->intelligence])*5)+50; 
		chance *= Ply[fd].extr->luck/50;
		if(n>chance) {
			print(fd,"Your spell fails.\n");
			return(1);
		}
		else
			return(0);
	case RANGER:
		chance = ((ply_ptr->level+bonus[ply_ptr->intelligence])*4)+56; 
		chance *= Ply[fd].extr->luck/50;
		if(n>chance) {
			print(fd,"Your spell fails.\n");
			return(1);
		}
		else
			return(0);
	case THIEF:
		chance = ((ply_ptr->level+bonus[ply_ptr->intelligence])*6)+22; 
		chance *= Ply[fd].extr->luck/50;
		if(n>chance) {
			print(fd,"Your spell fails.\n");
			return(1);
		}
		else
			return(0);
	case DRUID:
		chance = ((ply_ptr->level+bonus[ply_ptr->intelligence])*5)+65;
		chance *= Ply[fd].extr->luck/50;
		if(n>chance) {
			print(fd,"Your spell fails.\n");
			return(1);
		}
		else    
			return(0);
	case ALCHEMIST:
		chance = ((ply_ptr->level+bonus[ply_ptr->intelligence])*5)+70;
		chance *= Ply[fd].extr->luck/50;
		if(n>chance) {
			print(fd,"Your spell fails.\n");
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

int fortune(ply_ptr, cmnd, how)
creature	*ply_ptr;
cmd         	*cmnd;
int		how;
{

int		fd, luk;
creature	*crt_ptr;
room		*rom_ptr;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;
	
	if(ply_ptr->class != BARD && ply_ptr->class < CARETAKER) {
		print(fd, "Only bards may do that.\n");
		return(0);
	}

	if(ply_ptr->mpcur < 12 && how == CAST) {
        	print(fd, "Not enough magic points.\n");
        	return(0);
	}

	if(!S_ISSET(ply_ptr, SFORTU) && how == CAST) {
        	print(fd, "You don't know that spell.\n");
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
            print(fd, "Fortune spell cast on yourself.\n");
	    luk = Ply[fd].extr->luck/10;
	    MAX(luk, 1);
	    switch(luk) {
		case 1:
			print(fd, "Your death will be tragic.\n.");
			break;
                case 2:
                        print(fd, "A black cat must have crossed your path.\n");
                        break;
                case 3:
                        print(fd, "If it weren't for bad luck you'd have no luck at all.\n");
                        break;
                case 4:
                        print(fd, "Your karma is imbalanced.\n");
                        break;
                case 5:
                        print(fd, "Your future is uncertain.\n");
                        break;
                case 6:
                        print(fd, "Without intervention you may find yourself in a dire situation.\n");
                        break;
                case 7:
                        print(fd, "Long range prospects look good.\n");
                        break;
                case 8:
                        print(fd, "Count your blessings, for others are less fortunate.\n");
                        break;
                case 9:
                        print(fd, "The fates have smiled upon you.\n");
                        break;
                case 10:
                        print(fd, "Your death would be tragic.\n");
                        break;
		default:
			print (fd, "You can't tell right now.\n");
		}

            broadcast_rom(fd, ply_ptr->rom_num,
                      "%M reads %s aura.",
                      ply_ptr,
                      F_ISSET(ply_ptr, PMALES) ? "his":"her");
        }
        else if(how == POTION)
            print(fd, "Nothing happens.\n"); 

 
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
            cmnd->str[2][0] = low(cmnd->str[2][0]);
            crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                       cmnd->str[2], cmnd->str[2]);
 
            if(!crt_ptr) {
                print(fd, "That's not here.\n");
                return(0);
            }
        }

	if(crt_ptr->type == MONSTER)
		luk = crt_ptr->alignment/10;
	else 
		luk = Ply[crt_ptr->fd].extr->luck/10;
	
	MAX(luk, 1);
            print(fd, "Fortune spell cast on %s.\n", crt_ptr);

            switch(luk) {
                case 1:
                        print(fd, "%M's death will be swift and certain.\n", crt_ptr);
                        break;
                case 2:
                        print(fd, "You sense %M's karma is imbalanced.\n", crt_ptr);
                        break;
                case 3:
                        print(fd, "A black cat must have crossed %M's path.\n", crt_ptr);
                        break;
                case 4:
                        print(fd, "%M's aura reeks of danger.\n", crt_ptr);
                        break;
                case 5:
                        print(fd, "Without intervention %M may end up in a dire situation.\n", crt_ptr);
		        break;
                case 6:
                        print(fd, "%M's future is uncertain.\n", crt_ptr);
                        break;
                case 7:
                        print(fd, "Long range prospects look good for %M.\n", crt_ptr);
                        break;
                case 8:
                        print(fd, "%M should count their blessings, as others are less fortunate.\n", crt_ptr);
                        break;
                case 9:
                        print(fd, "The fates smile upon %M.\n", crt_ptr);
                        break;
                case 10:
                        print(fd, "%M's death would be tragic and unexpected.\n", crt_ptr);
                        break;
                default:
                        print (fd, "You can't tell right now.\n");
                }
		broadcast_rom(fd, ply_ptr->rom_num, "%M reads %s's aura.\n", ply_ptr, crt_ptr); 
		return(0);
	}
	
}		
