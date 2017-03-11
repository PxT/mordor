/*
 * MAGIC7.C:
 *
 *	(C) 1995, 1999 Brooke Paul
 *
 *  Additional spell-casting routines.
 *
 * $Id: magic7.c,v 6.14 2001/06/12 14:37:26 develop Exp $
 *
 * $Log: magic7.c,v $
 * Revision 6.14  2001/06/12 14:37:26  develop
 * removed ability to cast dispel, curse and drain
 * in RNOKILL rooms
 *
 * Revision 6.13  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

#include "../include/mordb.h"
#include "mextern.h"

/************************************************************************/
/*              resist_cold             */
/************************************************************************/

/* This function allows players to cast the resist cold spell.  It will */
/* allow the player to resist fire breathed on them by dragons and  */
/* other breathers.                         */

int resist_cold(creature *ply_ptr, cmd *cmnd, int how )
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int     fd;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(ply_ptr->mpcur < 12 && how == CAST) {
        output(fd, "Not enough magic points.\n");
        return(0);
    }

    if(!S_ISSET(ply_ptr, SRCOLD) && how == CAST) {
        output(fd, "You don't know that spell.\n");
        return(0);
    }
    if(spell_fail(ply_ptr, how)) {
                if(how==CAST)
                        ply_ptr->mpcur -= 12;
                return(0);
        }

    if(cmnd->num == 2) {
        ply_ptr->lasttime[LT_RCOLD].ltime = time(0);
        F_SET(ply_ptr, PRCOLD);
        broadcast_rom(fd, ply_ptr->rom_num, 
            "%M resists cold.", m1arg(ply_ptr));
        if(how == CAST) {
            output(fd, "You cast a resist-cold spell.\nA warm aura surrounds you.\n");
            ply_ptr->mpcur -= 12;
            ply_ptr->lasttime[LT_RCOLD].interval = MAX(300, 1200 +
                bonus[(int)ply_ptr->intelligence]*600);
if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            output(fd,"The room's magical properties increase the power of your spell.\n");
            ply_ptr->lasttime[LT_RCOLD].interval += 800L;
        }                                
        }
        else {
            output(fd, "A warm aura surrounds you.\n");
            ply_ptr->lasttime[LT_RCOLD].interval = 1200L;
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

        F_SET(crt_ptr, PRCOLD);
        crt_ptr->lasttime[LT_RCOLD].ltime = time(0);
        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
            "%M casts a resist-cold spell on %m.", 
            m2args(ply_ptr, crt_ptr));
        mprint(crt_ptr->fd, "%M casts resist-cold on you.\n", 
			m1arg(ply_ptr));

        if(how == CAST) {
            sprintf(g_buffer, "You cast a resist-cold spell on %s.\n",
                crt_ptr->name);
            output(fd, g_buffer);
            ply_ptr->mpcur -= 12;
            crt_ptr->lasttime[LT_RCOLD].interval = MAX(300, 1200 +
                bonus[(int)ply_ptr->intelligence]*600);
	if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            output(fd,"The room's magical properties increase the power of your spell.\n");
            crt_ptr->lasttime[LT_RCOLD].interval += 800L;
        }                                
        }

        else {
            mprint(fd, "%M resists cold.\n", m1arg(crt_ptr));
            crt_ptr->lasttime[LT_RCOLD].interval = 1200L;
        }

        return(1);
    }

}


/************************************************************************/
/*                          Breathe-water                               */
/************************************************************************/

/* This function allows players to cast the resist cold spell.  It will */
/* allow the player to resist fire breathed on them by dragons and  */
/* other breathers.                         */

int breathe_water(creature *ply_ptr, cmd *cmnd, int how )
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int     fd;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(ply_ptr->mpcur < 12 && how == CAST) {
        output(fd, "Not enough magic points.\n");
        return(0);
    }

    if(!S_ISSET(ply_ptr, SBRWAT) && how == CAST) {
        output(fd, "You don't know that spell.\n");
        return(0);
    }
    if(spell_fail(ply_ptr, how)) {
                if(how==CAST)
                        ply_ptr->mpcur -= 12;
                return(0);
        }

    if(cmnd->num == 2) {
        ply_ptr->lasttime[LT_BRWAT].ltime = time(0);
        F_SET(ply_ptr, PBRWAT);
        broadcast_rom(fd, ply_ptr->rom_num, 
            "%M breathes water.", m1arg(ply_ptr));
        if(how == CAST) {
            output(fd, "You cast a breathe-water spell.\nYour lungs increase in size.\n");
            ply_ptr->mpcur -= 12;
            ply_ptr->lasttime[LT_BRWAT].interval = MAX(300, 1200 +
                bonus[(int)ply_ptr->intelligence]*600);
	if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            output(fd,"The room's magical properties increase the power of your spell.\n");
            ply_ptr->lasttime[LT_BRWAT].interval += 800L;
        }                                
        }
        else {
            output(fd, "Your lungs increase in size.\n");
            ply_ptr->lasttime[LT_BRWAT].interval = 1200L;
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

        F_SET(crt_ptr, PBRWAT);
        crt_ptr->lasttime[LT_BRWAT].ltime = time(0);
        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
            "%M casts a breathe-water spell on %m.", 
            m2args(ply_ptr, crt_ptr));
        mprint(crt_ptr->fd, "%M casts breathe-water on you.\n", 
			m1arg(ply_ptr));

        if(how == CAST) {
            sprintf(g_buffer, "You cast a breathe-water spell on %s.\n",
                crt_ptr->name);
            output(fd, g_buffer);
            ply_ptr->mpcur -= 12;
            crt_ptr->lasttime[LT_BRWAT].interval = MAX(300, 1200 +
                bonus[(int)ply_ptr->intelligence]*600);
			if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
					output(fd,"The room's magical properties increase the power of your spell.\n");
					crt_ptr->lasttime[LT_BRWAT].interval += 800L;
				}                                
        }

        else {
            mprint(fd, "%M breathe-water.\n", m1arg(crt_ptr));
            crt_ptr->lasttime[LT_BRWAT].interval = 1200L;
        }

        return(1);
    }

}


/************************************************************************/
/*                      Stone-Shield                                    */
/************************************************************************/

/* This function allows players to cast the earth shield spell.  It will */
/* allow the player to resist fire breathed on them by dragons and  */
/* other breathers.                         */

int earth_shield(creature *ply_ptr, cmd *cmnd, int how )
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int     fd;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(ply_ptr->mpcur < 12 && how == CAST) {
        output(fd, "Not enough magic points.\n");
        return(0);
    }

    if(!S_ISSET(ply_ptr, SSSHLD) && how == CAST) {
        output(fd, "You don't know that spell.\n");
        return(0);
    }
    if(spell_fail(ply_ptr, how)) {
                if(how==CAST)
                        ply_ptr->mpcur -= 12;
                return(0);
        }

    if(cmnd->num == 2) {
        ply_ptr->lasttime[LT_SSHLD].ltime = time(0);
        F_SET(ply_ptr, PSSHLD);
        broadcast_rom(fd, ply_ptr->rom_num, 
            "%M casts earth shield.", m1arg(ply_ptr));
        if(how == CAST) {
            output(fd, "You cast an resist-earth spell.\nYour flesh strengthens.\n");
            ply_ptr->mpcur -= 12;
            ply_ptr->lasttime[LT_SSHLD].interval = MAX(300, 1200 +
                bonus[(int)ply_ptr->intelligence]*600);
	if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            output(fd,"The room's magical properties increase the power of your spell.\n");
            ply_ptr->lasttime[LT_SSHLD].interval += 800L;
        }                                
        }
        else {
            output(fd, "Your flesh strengthens.\n");
            ply_ptr->lasttime[LT_SSHLD].interval = 1200L;
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

        F_SET(crt_ptr, PSSHLD);
        crt_ptr->lasttime[LT_SSHLD].ltime = time(0);
        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
            "%M casts an resist-earth spell on %m.", 
            m2args(ply_ptr, crt_ptr));
        mprint(crt_ptr->fd, "%M casts resist-earth on you.\n", 
			m1arg(ply_ptr));

        if(how == CAST) {
            sprintf(g_buffer, "You cast an resist-earth spell on %s.\n",
                crt_ptr->name);
            output(fd, g_buffer);
            ply_ptr->mpcur -= 12;
            crt_ptr->lasttime[LT_SSHLD].interval = MAX(300, 1200 +
                bonus[(int)ply_ptr->intelligence]*600);
	if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            output(fd,"The room's magical properties increase the power of your spell.\n");
            crt_ptr->lasttime[LT_SSHLD].interval += 800L;
        }                                
        }

        else {
            mprint(fd, "%M casts resist-earth.\n", m1arg(crt_ptr));
            crt_ptr->lasttime[LT_SSHLD].interval = 1200L;
        }

        return(1);
    }

}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                  locate_person                               */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int locate_player(creature *ply_ptr, cmd *cmnd, int how )
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int     fd, chance;
 
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(fd < 0) return(0);

    if(!S_ISSET(ply_ptr, SLOCAT) && how == CAST) {
        output(fd, "You don't know that spell.\n");
        return(0);
    }
    if(ply_ptr->mpcur < 15 && how == CAST) {
        output(fd, "Not enough magic points.\n");
        return(0);
    }


 
    if(cmnd->num < 3) {
        output(fd, "Link with whom?\n");
        return(0);
    }
 
    lowercize(cmnd->str[2], 1);
    crt_ptr = find_who(cmnd->str[2]);
 
    if(!crt_ptr || F_ISSET(crt_ptr, PDMINV) ||
       (F_ISSET(crt_ptr, PINVIS) && !F_ISSET(ply_ptr, PDINVI))) {
        output(fd, "That player is not logged on.\n");
        return(0);
    }
   if(spell_fail(ply_ptr, how)) {
                if(how==CAST)
                        ply_ptr->mpcur -= 15;
                return(0);
        }
 
        broadcast_rom(fd, ply_ptr->rom_num, 
            "%M casts clairvoyance.", m1arg(ply_ptr));
        if(how == CAST) {
            mprint(fd, "Your mind begins to focus on %m.\n",
                m1arg(crt_ptr));
            ply_ptr->mpcur -= 15;
        }

    chance =  50 + (ply_ptr->level - crt_ptr->level)*5  +
        (bonus[(int)ply_ptr->intelligence] - bonus[(int)crt_ptr->intelligence])*5; 

    chance += (ply_ptr->class == MAGE) ? 5 : 0;
    chance = MIN(85,chance);

    if ((crt_ptr->class < DM) && (mrand(1,100) < chance)){
        display_rom(ply_ptr,crt_ptr->parent_rom);   
    	chance += (crt_ptr->class == MAGE) ? 5 : 0;
    	chance = MIN(85,chance);

	chance = 60 + (crt_ptr->level - ply_ptr->level)*5  +
	   (bonus[(int)crt_ptr->intelligence] - bonus[(int)ply_ptr->intelligence])*5;
    	chance += (crt_ptr->class == MAGE) ? 5 : 0;
    	chance = MIN(85,chance);

	if  (mrand(1,100) < chance)
        	mprint(crt_ptr->fd, "%M temporarily sees through your eyes.\n",
            	    m1arg(ply_ptr));
    }
    else{
        output(fd,"Your mind is unable to connect.\n");

	chance = 65 + (crt_ptr->level - ply_ptr->level)*5  +
	   (bonus[(int)crt_ptr->intelligence] - bonus[(int)ply_ptr->intelligence])*5;

	if  (mrand(1,100) < chance)
        mprint(crt_ptr->fd, "%M attempts to connect to your mind.\n",
            m1arg(ply_ptr));

    }
 
    return(1);
}
 
 
/**********************************************************************/
/*                          drain_exp                               */
/**********************************************************************/
/* The spell drain_exp causes a player to lose a selected amout of  *
 * magic points.  Success is lvl vs lvl comparison, with druids     *
 * an increased chance of success.                                  */


int drain_exp(creature *ply_ptr, cmd *cmnd, int how, object *obj_ptr )
{
    room        *rom_ptr;
    creature    *crt_ptr;
    int     fd, chance, mpcost;
    long    loss;
 
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;
    mpcost = 10;

    if(!S_ISSET(ply_ptr, SDREXP) && how == CAST) {
        output(fd, "You don't know that spell.\n");
        return(0);
    }
 
    if(ply_ptr->mpcur < mpcost && how == CAST) {
        output(fd, "Not enough magic points.\n");
        return(0);
    }

    if(F_ISSET(ply_ptr, PINVIS)) {
        F_CLR(ply_ptr, PINVIS);
        output(fd, "Your invisibility fades.\n");
        broadcast_rom(fd, ply_ptr->rom_num, "%M fades into view.",
                  m1arg(ply_ptr));
    }
 
    /* drain exp on self */
    if(cmnd->num == 2) {

        if(F_ISSET(ply_ptr, PRMAGI)) {
            output(fd, "Your magic shield resists the drain spell.\n");
            sprintf(g_buffer, "%%M casts energy drain on %sself.", 
                      F_ISSET(ply_ptr, PMALES) ? "him":"her");
            broadcast_rom(fd, ply_ptr->rom_num, 
                      g_buffer, m1arg(ply_ptr));
            if(how == CAST)
                ply_ptr->mpcur -= mpcost;
            return(0);
        }
        loss = ply_ptr->level;
        if(how == CAST || how == WAND)  {
            output(fd, "You cast an drain spell on yourself.\n");
            sprintf(g_buffer, "You drain %d magic points.\n",(int)loss);
            output(fd, g_buffer);
            sprintf(g_buffer, "%%M casts energy drain on %sself.", 
                      F_ISSET(ply_ptr, PMALES) ? "him":"her");
            broadcast_rom(fd, ply_ptr->rom_num, 
                      g_buffer, m1arg(ply_ptr));
        }
        else if(how == POTION){
            output(fd, "You feel your magic slipping away.\n");
            sprintf(g_buffer, "You drain %d magic points.\n",(int)loss);
            output(fd, g_buffer);
        }

        if(how == CAST)
            ply_ptr->mpcur -= mpcost;
        loss = MIN(loss, ply_ptr->mpcur);
	ply_ptr->mpcur -= loss;
    }
 
    /* energy drain a monster or player */
    else {

        if(how == POTION) {
            output(fd, "You can only use a potion on yourself.\n");
            return(0);
        }
 
        cmnd->str[2][0] = up(cmnd->str[2][0]);
        crt_ptr = find_crt_in_rom(ply_ptr, rom_ptr,
                   cmnd->str[2], cmnd->val[2], PLY_FIRST);
 
        if(!crt_ptr) {
            output(fd, "That's not here.\n");
            return(0);
        }

	if(F_ISSET(ply_ptr->parent_rom, RNOKIL)) {
                output(fd, "You can't cast that here.\n");
                return(0);
        }

	if(crt_ptr->type != PLAYER && F_ISSET(crt_ptr, MUNKIL)) {
		output(fd, "You may not do that!\n");
		return(0);
	}

        if(crt_ptr->type != PLAYER)
            add_enm_crt(ply_ptr->name, crt_ptr);

        if(F_ISSET(crt_ptr, PRMAGI)) {
            mprint(fd, "%m resists your drain spell.\n", m1arg(crt_ptr));
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "%M tries to cast drain on %m.",
                       m2args(ply_ptr, crt_ptr));
            mprint(crt_ptr->fd,
				"%M tries to cast drain on you.\nYour magic shield protects you.\n", m1arg(ply_ptr));
            if(how == CAST)
                ply_ptr->mpcur -= mpcost;
	    return(0);
        }

	chance =  50 + (ply_ptr->level - crt_ptr->level)*5  +
        (bonus[(int)ply_ptr->intelligence] - bonus[(int)crt_ptr->intelligence])*5; 
 	chance += (ply_ptr->class == DRUID) ? 30 : 0;
 	chance = MIN(85,chance);
  	if (/*(crt_ptr->class < DM) &&*/ (mrand(1,100) > chance)){
            mprint(fd, "You try to cast drain on %m.\n", m1arg(crt_ptr));
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "%M tries to cast drain on %m.",
                       m2args(ply_ptr, crt_ptr));
            mprint(crt_ptr->fd,
				"%M tries to cast drain on you.\n",
				m1arg(ply_ptr));
            if(how == CAST)
                ply_ptr->mpcur -= mpcost;
	    return(0);
	}
        else {
            loss = ply_ptr->level;
            loss = MIN(loss, crt_ptr->mpcur);
            mprint(fd, "You cast drain on %m.\n", m1arg(crt_ptr));
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "%M casts drain on %m.",
                       m2args(ply_ptr, crt_ptr));
            mprint(crt_ptr->fd,
				"%M casts drain on you.\nYou feel your magic slipping away.\n",
				m1arg(ply_ptr));
            sprintf(g_buffer,"You lose %d magic points.\n",(int)loss);
            output(crt_ptr->fd, g_buffer);

            sprintf(g_buffer,"%%M loses %d magic points.\n",(int)loss);
            mprint(fd, g_buffer, m1arg(crt_ptr));
            chance = ply_ptr->level;
            chance += (ply_ptr->class == DRUID) ? 20 : 0;
            if ((mrand(1,100)) < chance){
                mpcost = mpcost - (mrand(1,ply_ptr->level));
                mprint(fd, "You absorb some magic from %m.\n", m1arg(crt_ptr));
            }
       }
 
        if(how == CAST)
            ply_ptr->mpcur -= mpcost;
	crt_ptr->mpcur -= loss;
    }
 
    return(1);
 
}
               
/**********************************************************************/
/*              		remove disease				                */
/**********************************************************************/
 
int rm_disease(creature *ply_ptr, cmd *cmnd, int how )
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
            output(fd, "Remove disease spell cast on yourself.\n");
            output(fd, "Your fever subsides.\n");
            sprintf(g_buffer, "%%M casts remove disease on %sself.", 
                      F_ISSET(ply_ptr, PMALES) ? "him":"her");
            broadcast_rom(fd, ply_ptr->rom_num, 
                      g_buffer, m1arg(ply_ptr));
        }
        else if(how == POTION && F_ISSET(ply_ptr, PDISEA))
            output(fd, "You feel your fever subside.\n");
        else if(how == POTION)
            output(fd, "Nothing happens.\n");
 
        F_CLR(ply_ptr, PDISEA);
 
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
 
        F_CLR(crt_ptr, PDISEA);
 
        if(how == CAST || how == SCROLL || how == WAND) {
            mprint(fd, "You remove disease cast on %m.\n", 
				m1arg(crt_ptr));
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "%M casts remove disease on %m.",
                       m2args(ply_ptr, crt_ptr));
            mprint(crt_ptr->fd, "%M casts remove disease on you.\nYou feel your fever subside.\n", 
				m1arg(ply_ptr));
        }
 
    }
 
    return(1);
 
}
 


/************************************************************************/
/*                           object_send                               */
/************************************************************************/
/* The send object spell allows a mage  or DM/Caretaker to magically     *
 * transport an object to another login (detectable) player.  The maxium *
 * weight of the object being transported is based on the player's       *
 * level and intellignece. */

int object_send(creature *ply_ptr, cmd *cmnd, int how )
{
    creature    *crt_ptr;
    object      *obj_ptr;
    room        *rom_ptr;
    int         fd, cost;
 
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(!S_ISSET(ply_ptr, STRANO) && how == CAST) {
        output(fd, "You don't know that spell.\n");
        return(0);
    }

    if(ply_ptr->class != MAGE && ply_ptr->class < BUILDER && how == CAST) {
       output(fd, "Only mages may cast that spell.\n");
       return(0);
    }                         

    if(ply_ptr->level <5){
        output(fd, "You are not high enough level to cast that yet.\n");
        return(0);
    }
 
    if(cmnd->num < 4) {
        output(fd, "Transport what to whom?\n");
        return(0);
    }

    lowercize(cmnd->str[3], 1);
    crt_ptr = find_who(cmnd->str[3]);
 
    if(!crt_ptr || F_ISSET(crt_ptr, PDMINV) ||
       (F_ISSET(crt_ptr, PINVIS) && !F_ISSET(ply_ptr, PDINVI))) {
        output(fd, "That player is not logged on.\n");
        return(0);
    }
 
    obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
        cmnd->str[2], cmnd->val[2]);
 
    if(!obj_ptr) {
        output(fd, "You don't have that object.\n");
        return(0);
    }

	if(obj_ptr->questnum) {
                output(fd, "You can't transport a quest object.\n");
                return(0);
        }

    cost = 5 + bonus[(int)ply_ptr->intelligence] + (ply_ptr->level - 5)*2;
    if (obj_ptr->weight > cost){
        mprint(fd,"%I is too heavy to transport at your current level.\n", 
			m1arg(obj_ptr));
        return(0);
    }

    cost = 8 + (obj_ptr->weight)/4;
    if(ply_ptr->mpcur < cost && how == CAST) {
        output(fd, "Not enough magic points.\n");
        return(0);
    } else if(how == CAST) {
            ply_ptr->mpcur -= cost;
        }
	if(spell_fail(ply_ptr, how)) {
                return(0);
        }

    

    if (max_weight(crt_ptr) < weight_ply(crt_ptr)+obj_ptr->weight){
        mprint(fd,"%M is unable to hold %1i.\n",
			m2args(crt_ptr,obj_ptr));
        return(0);
    }
    del_obj_crt(obj_ptr, ply_ptr);
    add_obj_crt(obj_ptr, crt_ptr);

    if(!F_ISSET(ply_ptr, PDMINV)){
	    broadcast_rom(fd, ply_ptr->rom_num, "%M transports an object to someone.\n", 
			m1arg(ply_ptr));
    }
    mprint(fd,"You concentrate intensely on %i as it dissappears.\n",
        m1arg(obj_ptr));
    mprint(fd, "You sucessfully transported %1i to %m.\n", 
        m2args(obj_ptr, crt_ptr));
    if(!F_ISSET(ply_ptr, PDMINV)){
		mprint(crt_ptr->fd, "%M magically sends you %1i.\n",
			m2args(ply_ptr, obj_ptr));
    }
    return(1);
	
}

