/*
 * MAGIC2.C:
 *
 *  Additional user routines dealing with magic spells.  
 *
 *  Copyright (C) 1991, 1992, 1993 Brooke Paul
 *
 * $Id: magic2.c,v 6.13 2001/03/08 16:09:09 develop Exp $
 *
 * $Log: magic2.c,v $
 * Revision 6.13  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

#include "../include/mordb.h"
#include "mextern.h"

/**********************************************************************/
/*              vigor                     */
/**********************************************************************/

/* This function will cause the vigor spell to be cast on a player or   */
/* another monster.  It heals 1d6 hit points plus any bonuses for       */
/* intelligence.  If the healer is a cleric then there is an additional */
/* point of healing for each level of the cleric.           */

int vigor(creature *ply_ptr, cmd *cmnd, int how )
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int     fd, heal, exp;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(ply_ptr->mpcur < 2 && how == CAST) {
        output(fd, "Not enough magic points.\n");
        return(0);
    }

    if(!S_ISSET(ply_ptr, SVIGOR) && how == CAST) {
        output(fd, "You don't know that spell.\n");
        return(0);
    }
    if(ply_ptr->class == BARBARIAN) {
    	if(spell_fail(ply_ptr, how)){
        	if(how == CAST)
                	ply_ptr->mpcur -= 2;
        	return(0);
    	}  
    }
    /* Vigor self */
    if(cmnd->num == 2) {

        if(how == CAST){
            heal = bonus[(int)ply_ptr->piety] + 
            ((ply_ptr->class == CLERIC) ? ply_ptr->level * 0.8 : 0) +
            ((ply_ptr->class == PALADIN) ? ply_ptr->level * 0.6: 0) +
            ((ply_ptr->class == DRUID) ? ply_ptr->level * 0.6: 0) +
            ((ply_ptr->class == BARD) ? ply_ptr->level * 0.5: 0) +
            ((ply_ptr->class == MONK) ? ply_ptr->level * 0.4: 0) +
            ((ply_ptr->class == RANGER) ? ply_ptr->level * 0.3: 0) +
            mrand(1,6);
            ply_ptr->mpcur -= 2;
	    if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
		heal += mrand(1,3);
		output(fd,"The room's magical properties increase the power of your spell.\n");
	    }
	}
        else
            heal = mrand(1,6);

        heal = MAX(1, heal);

        ply_ptr->hpcur += MAX(1, heal);

        if(ply_ptr->hpcur > ply_ptr->hpmax)
            ply_ptr->hpcur = ply_ptr->hpmax;


        if(how == CAST || how == SCROLL) {
            output(fd, "Vigor spell cast.\n");
            sprintf(g_buffer, "%%M casts a vigor spell on %sself.", 
                      F_ISSET(ply_ptr, PMALES) ? "him":"her");
            broadcast_rom(fd, ply_ptr->rom_num, 
				g_buffer, m1arg(ply_ptr));
            return(1);
        }
        else {
            output(fd, "You feel better.\n");
            return(1);
        }
    }

    /* Cast vigor on another player or monster */
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

        if(how == CAST){
            heal = MAX(bonus[(int)ply_ptr->intelligence], 
              bonus[(int)ply_ptr->piety]) + 
            ((ply_ptr->class == CLERIC) ? ply_ptr->level * 0.8 : 0) + 
            ((ply_ptr->class == PALADIN) ? ply_ptr->level * 0.6: 0) + 
            ((ply_ptr->class == DRUID) ? ply_ptr->level * 0.6: 0) + 
            ((ply_ptr->class == BARD) ? ply_ptr->level * 0.5: 0) + 
            ((ply_ptr->class == MONK) ? ply_ptr->level * 0.4: 0) + 
            ((ply_ptr->class == RANGER) ? ply_ptr->level * 0.3: 0) + 
            mrand(1,6);
            ply_ptr->mpcur -= 2;
	    if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
		heal += mrand(1,3);
		output(fd,"The room's magical properties increase the power of your spell.\n");
	    }
	}
        else
            heal = mrand(1,6);

        heal = MAX(1, heal);

	if(GUILDEXP) 
	{
		if(ply_ptr != crt_ptr && crt_ptr->type == PLAYER && check_guild(ply_ptr) == 8) {
			exp=0;
			if((ply_ptr->class == CLERIC || ply_ptr->class == PALADIN)&& how==CAST) 
				exp=MAX(1, heal)/4;
			else 
				if(how==CAST)
					exp=MAX(1, heal)/2;

			if(exp && (crt_ptr->hpcur < crt_ptr->hpmax)) {
				ply_ptr->experience+=exp;
				sprintf(g_buffer, "You guild awards you %d experience for your deed.\n", exp);
				output(fd, g_buffer);
			}
		}
	}
	
	crt_ptr->hpcur += MAX(1, heal);

        if(crt_ptr->hpcur > crt_ptr->hpmax)
            crt_ptr->hpcur = crt_ptr->hpmax;

        if(how == CAST || how == SCROLL || how == WAND) {
            mprint(fd, "Vigor spell cast on %m.\n", m1arg(crt_ptr));
            mprint(crt_ptr->fd, "%M casts a vigor spell on you.\n",
                  m1arg(ply_ptr));
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "%M casts a vigor spell on %m.",
                       m2args(ply_ptr, crt_ptr));
            return(1);
        }
    }

    return(1);

}

/**********************************************************************/
/*              curepoison                */
/**********************************************************************/

/* This function allows a player to cast a curepoison spell on himself, */
/* another player or a monster.  It will remove any poison that is in   */
/* that player's system.                        */

int curepoison(creature *ply_ptr, cmd *cmnd, int how )
{
    room        *rom_ptr;
    creature    *crt_ptr;
    int     fd;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(ply_ptr->mpcur < 6 && how == CAST) {
        output(fd, "Not enough magic points.\n");
        return(0);
    }

    if(!S_ISSET(ply_ptr, SCUREP) && how == CAST) {
        output(fd, "You don't know that spell.\n");
        return(0);
    }
    if(spell_fail(ply_ptr, how)) {
       if(how==CAST)
                 ply_ptr->mpcur -= 6;
        return(0);
    }

    /* Curepoison self */
    if(cmnd->num == 2) {

        if(how == CAST)
            ply_ptr->mpcur -= 6;

        if(how == CAST || how == SCROLL || how == WAND)  {
            output(fd, "Curepoison spell cast on yourself.\n");
            output(fd, "You feel much better.\n");
            sprintf(g_buffer, "%%M casts curepoison on %sself.", 
                      F_ISSET(ply_ptr, PMALES) ? "him":"her");
            broadcast_rom(fd, ply_ptr->rom_num, 
                      g_buffer, m1arg(ply_ptr));
        }
        else if(how == POTION && F_ISSET(ply_ptr, PPOISN))
            output(fd, "You feel the poison subside.\n");
        else if(how == POTION)
            output(fd, "Nothing happens.\n");

        F_CLR(ply_ptr, PPOISN);

    }

    /* Cure a monster or player */
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
            ply_ptr->mpcur -= 6;

        F_CLR(crt_ptr, PPOISN);

        if(how == CAST || how == SCROLL || how == WAND) {
            mprint(fd, "Curepoison cast on %m.\n", m1arg(crt_ptr));
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "%M casts curepoison on %m.",
                       m2args(ply_ptr, crt_ptr));
            mprint(crt_ptr->fd, "%M casts curepoison on you.\nYou feel much better.\n", 
				m1arg(ply_ptr));
        }

    }

    return(1);

}

/**********************************************************************/
/*              light                     */
/**********************************************************************/

/* This spell allows a player to cast a light spell which will illuminate */
/* any darkened room for a period of time (depending on level).       */

int light(creature *ply_ptr, cmd *cmnd, int how )
{
    int fd;

    fd = ply_ptr->fd;

    if(ply_ptr->mpcur < 5 && how == CAST) {
        output(fd, "Not enough magic points.\n");
        return(0);
    }

    if(!S_ISSET(ply_ptr, SLIGHT) && how == CAST) {
        output(fd, "You don't know that spell.\n");
        return(0);
    }
    if(how == CAST){
    	if (F_ISSET(ply_ptr->parent_rom,RPMEXT))
	output(fd,"The room's magical properties increase the power of your spell.\n");
        ply_ptr->mpcur -= 5;
    }

    F_SET(ply_ptr, PLIGHT);
    ply_ptr->lasttime[LT_LIGHT].ltime = time(0);
    ply_ptr->lasttime[LT_LIGHT].interval = 300L +
	bonus[(int)ply_ptr->intelligence]*300L +
	((F_ISSET(ply_ptr->parent_rom,RPMEXT)) ? 600L : 0);

    output(fd, "You cast a light spell.\n");
    broadcast_rom(fd, ply_ptr->rom_num, "%M casts a light spell.", 
		m1arg(ply_ptr));

    return(1);

}

/**********************************************************************/
/*              protection                */
/**********************************************************************/

/* This function allows a spellcaster to cast a protection spell either */
/* on himself or on another player, improving the armor class by a      */
/* score of 10.                             */

int protection(creature *ply_ptr, cmd *cmnd, int how )
{
    creature    *crt_ptr;
    room        *rom_ptr;
    time_t        t;
    int     fd;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;
    t = time(0);

    if(ply_ptr->mpcur < 10 && how == CAST) {
        output(fd, "Not enough magic points.\n");
        return(0);
    }

    if(!S_ISSET(ply_ptr, SPROTE) && how == CAST) {
        output(fd, "You don't know that spell.\n");
        return(0);
    }
    if(spell_fail(ply_ptr, how) && ply_ptr->class != CLERIC && ply_ptr->class != PALADIN && how != POTION) {
        if(how==CAST)
              ply_ptr->mpcur -= 10;
        return(0);
    }

    /* Cast protection on self */
    if(cmnd->num == 2) {

        F_SET(ply_ptr, PPROTE);
        compute_ac(ply_ptr);

        ply_ptr->lasttime[LT_PROTE].ltime = t;
        if(how == CAST) {
            ply_ptr->lasttime[LT_PROTE].interval = MAX(300, 1200 + 
                bonus[(int)ply_ptr->intelligence]*600);
            if(ply_ptr->class == CLERIC || ply_ptr->class == 
               PALADIN)
                ply_ptr->lasttime[LT_PROTE].interval += 
                60*ply_ptr->level;
            ply_ptr->mpcur -= 10;
    	if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
	    output(fd,"The room's magical properties increase the power of your spell.\n");
	    ply_ptr->lasttime[LT_PROTE].interval += 800L;
	}
        }
        else
            ply_ptr->lasttime[LT_PROTE].interval = 1200;

        if(how == CAST || how == SCROLL || how == WAND)  {
            output(fd,"Protection spell cast.\nYou feel watched.\n");
            sprintf(g_buffer, "%%M casts a protection spell on %sself.", 
                      F_ISSET(ply_ptr, PMALES) ? "him":"her");
            broadcast_rom(fd, ply_ptr->rom_num, 
                      g_buffer, m1arg(ply_ptr));
        }
        else if(how == POTION)
            output(fd, "You feel watched.\n");

        return(1);
    }

    /* Cast protection on another player */
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

        F_SET(crt_ptr, PPROTE);
        compute_ac(crt_ptr);

        crt_ptr->lasttime[LT_PROTE].ltime = t;
        if(how == CAST) {
            crt_ptr->lasttime[LT_PROTE].interval = MAX(300, 1200 + 
                bonus[(int)ply_ptr->intelligence]*600);
            if(ply_ptr->class == CLERIC || ply_ptr->class == 
               PALADIN)
                crt_ptr->lasttime[LT_PROTE].interval += 
                60*ply_ptr->level;
            ply_ptr->mpcur -= 10;
    	if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
	    output(fd,"The room's magical properties increase the power of your spell.\n");
	    crt_ptr->lasttime[LT_PROTE].interval += 800L;
	}
        }
        else
            crt_ptr->lasttime[LT_PROTE].interval = 1200;

        if(how == CAST || how == SCROLL || how == WAND) {
            sprintf(g_buffer, "Protection cast on %s.\n", crt_ptr->name);
            output(fd, g_buffer);
            mprint(crt_ptr->fd, 
                  "%M casts a protection spell on you.\nYou feel watched.\n",
                  m1arg(ply_ptr));
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "%M casts a protection spell on %m.",
                       m2args(ply_ptr, crt_ptr));
            return(1);
        }
    }

    return(1);

}

/**********************************************************************/
/*              mend                      */
/**********************************************************************/

/* This function will cause the mend spell to be cast on a player or    */
/* another monster.  It heals 2d6 hit points plus any bonuses for       */
/* intelligence.  If the healer is a cleric then there is an additional */
/* point of healing for each level of the cleric.           */

int mend(creature *ply_ptr, cmd *cmnd, int how )
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int     fd, heal, exp;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(ply_ptr->mpcur < 4 && how == CAST) {
        output(fd, "Not enough magic points.\n");
        return(0);
    }

    if(!S_ISSET(ply_ptr, SMENDW) && how == CAST) {
        output(fd, "You don't know that spell.\n");
        return(0);
    }
    if(ply_ptr->class == BARBARIAN || ply_ptr->class == FIGHTER || ply_ptr->class == MONK) {
	    if(spell_fail(ply_ptr, how)) {
	        if(how==CAST)
        	        ply_ptr->mpcur -= 4;
        	return(0);
    	    }
    }
    /* Mend self */
    if(cmnd->num == 2) {

        if(how == CAST){
            heal = bonus[(int)ply_ptr->piety] + 
            ((ply_ptr->class == CLERIC) ? ply_ptr->level * 1.2 : 0) +
            ((ply_ptr->class == PALADIN) ? ply_ptr->level : 0) +
            ((ply_ptr->class == DRUID) ? ply_ptr->level : 0) +
            ((ply_ptr->class == BARD) ? ply_ptr->level * 0.8: 0) +
            ((ply_ptr->class == MONK) ? ply_ptr->level * 0.6: 0) +
            ((ply_ptr->class == RANGER) ? ply_ptr->level * 0.5: 0) +
            dice(4,3,0);
            ply_ptr->mpcur -= 4;
	    if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
		heal += mrand(1,6)+1;
		output(fd,"The room's magical properties increase the power of your spell.\n");
	    }
	}
        else
            heal = dice(4,3,0);

        heal = MAX(1, heal);

        ply_ptr->hpcur += MAX(1, heal);

        if(ply_ptr->hpcur > ply_ptr->hpmax)
            ply_ptr->hpcur = ply_ptr->hpmax;


        if(how == CAST || how == SCROLL) {
            output(fd, "Mend-wounds spell cast.\n");
            sprintf(g_buffer, "%%M casts a mend-wounds spell on %sself.", 
                      F_ISSET(ply_ptr, PMALES) ? "him":"her");
            broadcast_rom(fd, ply_ptr->rom_num, 
                      g_buffer, m1arg(ply_ptr));
            return(1);
        }
        else {
            output(fd, "You feel better.\n");
            return(1);
        }
    }

    /* Cast mend on another player or monster */
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

        if(how == CAST) {
            heal = MAX(bonus[(int)ply_ptr->intelligence],
              bonus[(int)ply_ptr->piety]) + 
            ((ply_ptr->class == CLERIC) ? ply_ptr->level * 1.2 : 0) +
            ((ply_ptr->class == PALADIN) ? ply_ptr->level : 0) +
            ((ply_ptr->class == DRUID) ? ply_ptr->level : 0) +
            ((ply_ptr->class == BARD) ? ply_ptr->level * 0.8 : 0) +
            ((ply_ptr->class == MONK) ? ply_ptr->level * 0.6: 0) +
            ((ply_ptr->class == RANGER) ? ply_ptr->level * 0.5: 0) +
            dice(4,3,0);
            ply_ptr->mpcur -= 4;
	    if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
		heal += mrand(1,6)+1;
		output(fd,"The room's magical properties increase the power of your spell.\n");
	    }
        }
        else
            heal = dice(4,3,0);

        heal = MAX(1, heal);

        if(GUILDEXP)
        {
                if(ply_ptr != crt_ptr && crt_ptr->type == PLAYER && check_guild(ply_ptr) == 8) {
                        exp=0;
                        if((ply_ptr->class == CLERIC || ply_ptr->class == PALADIN)&& how==CAST)
                                exp=MAX(1, heal)/4;
                        else
                                if(how==CAST)
                                        exp=MAX(1, heal)/2;
    
                        if(exp && (crt_ptr->hpcur < crt_ptr->hpmax)) {
                                ply_ptr->experience+=exp;
                                sprintf(g_buffer, "You guild awards you %d experience for your deed.\n", exp);           
                                output(fd, g_buffer);
                        }
                }
        }

        crt_ptr->hpcur += MAX(1, heal);

        if(crt_ptr->hpcur > crt_ptr->hpmax)
            crt_ptr->hpcur = crt_ptr->hpmax;

        if(how == CAST || how == SCROLL || how == WAND) {
            mprint(fd, "Mend-wounds spell cast on %m.\n", m1arg(crt_ptr));
            mprint(crt_ptr->fd, 
                  "%M casts a mend-wounds spell on you.\n", m1arg(ply_ptr));
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "%M casts a mend-wounds spell on %m.",
                       m2args(ply_ptr, crt_ptr));
            return(1);
        }
    }

    return(1);

}
