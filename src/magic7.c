/*
 * MAGIC7.C:
 *
 *  Additional spell-casting routines.
 */

#include "mstruct.h"
#include "mextern.h"
#ifdef DMALLOC
  #include "/usr/local/include/dmalloc.h"
#endif
/************************************************************************/
/*              resist_cold             */
/************************************************************************/

/* This function allows players to cast the resist cold spell.  It will */
/* allow the player to resist fire breathed on them by dragons and  */
/* other breathers.                         */

int resist_cold(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int     fd;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(ply_ptr->mpcur < 12 && how == CAST) {
        print(fd, "Not enough magic points.\n");
        return(0);
    }

    if(!S_ISSET(ply_ptr, SRCOLD) && how == CAST) {
        print(fd, "You don't know that spell.\n");
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
            "%M resists cold.", ply_ptr);
        if(how == CAST) {
            print(fd, "You cast a resist-cold spell.\nA warm aura surrounds you.\n");
            ply_ptr->mpcur -= 12;
            ply_ptr->lasttime[LT_RCOLD].interval = MAX(300, 1200 +
                bonus[ply_ptr->intelligence]*600);
if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,"The room's magical properties increase the power of your spell.\n");
            ply_ptr->lasttime[LT_RCOLD].interval += 800L;
        }                                
        }
        else {
            print(fd, "A warm aura surrounds you.\n");
            ply_ptr->lasttime[LT_RCOLD].interval = 1200L;
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

        F_SET(crt_ptr, PRCOLD);
        crt_ptr->lasttime[LT_RCOLD].ltime = time(0);
        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
            "%M casts a resist-cold spell on %m.", 
            ply_ptr, crt_ptr);
        print(crt_ptr->fd, "%M casts resist-cold on you.\n", ply_ptr);

        if(how == CAST) {
            print(fd, "You cast a resist-cold spell on %s.\n",
                crt_ptr);
            ply_ptr->mpcur -= 12;
            crt_ptr->lasttime[LT_RCOLD].interval = MAX(300, 1200 +
                bonus[ply_ptr->intelligence]*600);
	if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,"The room's magical properties increase the power of your spell.\n");
            crt_ptr->lasttime[LT_RCOLD].interval += 800L;
        }                                
        }

        else {
            print(fd, "%M resists cold.\n", crt_ptr);
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

int breathe_water(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int     fd;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(ply_ptr->mpcur < 12 && how == CAST) {
        print(fd, "Not enough magic points.\n");
        return(0);
    }

    if(!S_ISSET(ply_ptr, SBRWAT) && how == CAST) {
        print(fd, "You don't know that spell.\n");
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
            "%M breathes water.", ply_ptr);
        if(how == CAST) {
            print(fd, "You cast a breathe-water spell.\nYour lungs increase in size.\n");
            ply_ptr->mpcur -= 12;
            ply_ptr->lasttime[LT_BRWAT].interval = MAX(300, 1200 +
                bonus[ply_ptr->intelligence]*600);
	if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,"The room's magical properties increase the power of your spell.\n");
            ply_ptr->lasttime[LT_BRWAT].interval += 800L;
        }                                
        }
        else {
            print(fd, "Your lungs increase in size.\n");
            ply_ptr->lasttime[LT_BRWAT].interval = 1200L;
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

        F_SET(crt_ptr, PBRWAT);
        crt_ptr->lasttime[LT_BRWAT].ltime = time(0);
        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
            "%M casts a breathe-water spell on %m.", 
            ply_ptr, crt_ptr);
        print(crt_ptr->fd, "%M casts breathe-water on you.\n", ply_ptr);

        if(how == CAST) {
            print(fd, "You cast a breathe-water spell on %s.\n",
                crt_ptr);
            ply_ptr->mpcur -= 12;
            crt_ptr->lasttime[LT_BRWAT].interval = MAX(300, 1200 +
                bonus[ply_ptr->intelligence]*600);
	if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,"The room's magical properties increase the power of your spell.\n");
            crt_ptr->lasttime[LT_BRWAT].interval += 800L;
        }                                
        }

        else {
            print(fd, "%M breathe-water.\n", crt_ptr);
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

int earth_shield(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int     fd;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(ply_ptr->mpcur < 12 && how == CAST) {
        print(fd, "Not enough magic points.\n");
        return(0);
    }

    if(!S_ISSET(ply_ptr, SSSHLD) && how == CAST) {
        print(fd, "You don't know that spell.\n");
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
            "%M casts earth shield.", ply_ptr);
        if(how == CAST) {
            print(fd, "You cast an earth-shield spell.\nYour flesh strengthens.\n");
            ply_ptr->mpcur -= 12;
            ply_ptr->lasttime[LT_SSHLD].interval = MAX(300, 1200 +
                bonus[ply_ptr->intelligence]*600);
	if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,"The room's magical properties increase the power of your spell.\n");
            ply_ptr->lasttime[LT_SSHLD].interval += 800L;
        }                                
        }
        else {
            print(fd, "Your flesh strengthens.\n");
            ply_ptr->lasttime[LT_SSHLD].interval = 1200L;
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

        F_SET(crt_ptr, PSSHLD);
        crt_ptr->lasttime[LT_SSHLD].ltime = time(0);
        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
            "%M casts an earth-shield spell on %m.", 
            ply_ptr, crt_ptr);
        print(crt_ptr->fd, "%M casts earth-shield on you.\n", ply_ptr);

        if(how == CAST) {
            print(fd, "You cast an earth-shield spell on %s.\n",
                crt_ptr);
            ply_ptr->mpcur -= 12;
            crt_ptr->lasttime[LT_SSHLD].interval = MAX(300, 1200 +
                bonus[ply_ptr->intelligence]*600);
	if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,"The room's magical properties increase the power of your spell.\n");
            crt_ptr->lasttime[LT_SSHLD].interval += 800L;
        }                                
        }

        else {
            print(fd, "%M casts earth-shield.\n", crt_ptr);
            crt_ptr->lasttime[LT_SSHLD].interval = 1200L;
        }

        return(1);
    }

}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                  locate_person                               */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int locate_player(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int     fd, chance;
 
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(fd < 0) return(0);

    if(!S_ISSET(ply_ptr, SLOCAT) && how == CAST) {
        print(fd, "You don't know that spell.\n");
        return(0);
    }
    if(ply_ptr->mpcur < 15 && how == CAST) {
        print(fd, "Not enough magic points.\n");
        return(0);
    }


 
    if(cmnd->num < 3) {
        print(fd, "Link with whom?\n");
        return(0);
    }
 
    lowercize(cmnd->str[2], 1);
    crt_ptr = find_who(cmnd->str[2]);
 
    if(!crt_ptr || F_ISSET(crt_ptr, PDMINV) ||
       (F_ISSET(crt_ptr, PINVIS) && !F_ISSET(ply_ptr, PDINVI))) {
        print(fd, "That player is not logged on.\n");
        return(0);
    }
   if(spell_fail(ply_ptr, how)) {
                if(how==CAST)
                        ply_ptr->mpcur -= 15;
                return(0);
        }
 
        broadcast_rom(fd, ply_ptr->rom_num, 
            "%M casts clairvoyance.", ply_ptr);
        if(how == CAST) {
            print(fd, "Your mind begins to focus on %m.\n",
                crt_ptr);
            ply_ptr->mpcur -= 15;
        }

    chance =  50 + (ply_ptr->level - crt_ptr->level)*5  +
        (bonus[ply_ptr->intelligence] - bonus[crt_ptr->intelligence])*5; 

    chance += (ply_ptr->class == MAGE) ? 5 : 0;
    chance = MIN(85,chance);

    if ((crt_ptr->class < DM) && (mrand(1,100) < chance)){
        display_rom(ply_ptr,crt_ptr->parent_rom);   
    	chance += (crt_ptr->class == MAGE) ? 5 : 0;
    	chance = MIN(85,chance);

	chance = 60 + (crt_ptr->level - ply_ptr->level)*5  +
	   (bonus[crt_ptr->intelligence] - bonus[ply_ptr->intelligence])*5;
    	chance += (crt_ptr->class == MAGE) ? 5 : 0;
    	chance = MIN(85,chance);

	if  (mrand(1,100) < chance)
        	print(crt_ptr->fd, "%M temporarily sees through your eyes.\n",
            	    ply_ptr);
    }
    else{
        print(fd,"Your mind is unable to connect.\n");

	chance = 65 + (crt_ptr->level - ply_ptr->level)*5  +
	   (bonus[crt_ptr->intelligence] - bonus[ply_ptr->intelligence])*5;

	if  (mrand(1,100) < chance)
        print(crt_ptr->fd, "%M attempts to connect to your mind.\n",
            ply_ptr);

    }
 
    return(1);
}
 
 
/**********************************************************************/
/*                          drain_exp                               */
/**********************************************************************/
/* The spell drain_exp causes a player to lose a selected amout of  *
 * exp.  When a player loses exp, the player's magical realm and    *
 * weapon procifiency will reflect the change.  This spell is not   *
 * intended to be learned or casted by a player.  The 4th parameter *
 * if given, will base the exp loss on the DnS damage of the object */

int drain_exp(ply_ptr, cmnd, how, obj_ptr)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
object  *obj_ptr;
{
    room        *rom_ptr;
    creature    *crt_ptr;
    int     fd;
    long    loss;
 
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;
 
    if(how == CAST && ply_ptr->class < CARETAKER) {
        print(fd, "You may not cast that spell.\n");
        return(0);
        }      

    if(how ==  SCROLL){
        print(fd, "You may not cast that spell.\n");
        return(0);
        }      

    if(!S_ISSET(ply_ptr, SDREXP) && how == CAST) {
        print(fd, "You don't know that spell.\n");
        return(0);
    }
 
    /* drain exp on self */
    if(cmnd->num == 2) {
 
        if(how == POTION || how == WAND) 
            loss= dice(ply_ptr->level, ply_ptr->level,(ply_ptr->level)*10);

        else if(how == CAST)
            loss = dice(ply_ptr->level, ply_ptr->level,1);
            loss = MIN(loss, ply_ptr->experience);

        if(how == CAST || how == WAND)  {
            print(fd, "You cast an energy drain spell on yourself.\n");
            print(fd, "You lose %d experience.\n",loss);
            broadcast_rom(fd, ply_ptr->rom_num, 
                      "%M casts energy drain on %sself.", 
                      ply_ptr,
                      F_ISSET(ply_ptr, PMALES) ? "him":"her");
        }
        else if(how == POTION){
            print(fd, "You feel your experience slipping away.\n");
            print(fd, "You lose %d experience.\n",loss);
        }
            ply_ptr->experience -= loss;    
	    lower_prof(ply_ptr,loss);
 
    }
 
    /* energy drain a monster or player */
    else {
 
        if(how == POTION || how == WAND) 
            loss = mdice(obj_ptr);
        else if(how == CAST)
            loss = dice(ply_ptr->level, ply_ptr->level,1);

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
 
 
        loss = MIN(loss, crt_ptr->experience);
        if(how == CAST || how == WAND) {
            print(fd, "You cast energy drain on %m.\n", crt_ptr);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "%M casts energy drain on %m.",
                       ply_ptr, crt_ptr);
            print(crt_ptr->fd,
     "%M casts energy drain on you.\nYou feel your experience slipping away.\n",
         ply_ptr);
           print(crt_ptr->fd,"You lose %d experience.\n",loss);
           print(fd,"%M loses %d experience.\n",crt_ptr, loss);
        }
 
           crt_ptr->experience -= loss;    
	    lower_prof(crt_ptr,loss);
    }
 
    return(1);
 
}
               
/**********************************************************************/
/*              		remove disease				                */
/**********************************************************************/
 
int rm_disease(ply_ptr, cmnd, how)
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
            print(fd, "Remove disease spell cast on yourself.\n");
            print(fd, "Your fever subsides.\n");
            broadcast_rom(fd, ply_ptr->rom_num, 
                      "%M casts remove disease on %sself.", 
                      ply_ptr,
                      F_ISSET(ply_ptr, PMALES) ? "him":"her");
        }
        else if(how == POTION & F_ISSET(ply_ptr, PDISEA))
            print(fd, "You feel your fever subside.\n");
        else if(how == POTION)
            print(fd, "Nothing happens.\n");
 
        F_CLR(ply_ptr, PDISEA);
 
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
 
        F_CLR(crt_ptr, PDISEA);
 
        if(how == CAST || how == SCROLL || how == WAND) {
            print(fd, "You remove disease cast on %m.\n", crt_ptr);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "%M casts remove disease on %m.",
                       ply_ptr, crt_ptr);
            print(crt_ptr->fd, "%M casts remove disease on you.\nYou feel your fever subside.\n", ply_ptr);
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

int object_send(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
{
    creature    *crt_ptr;
    object      *obj_ptr;
    room        *rom_ptr;
    int         fd, cost;
 
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(!S_ISSET(ply_ptr, STRANO) && how == CAST) {
        print(fd, "You don't know that spell.\n");
        return(0);
    }

    if(ply_ptr->class != MAGE && ply_ptr->class < CARETAKER && how == CAST) {
       print(fd, "Only mages may cast that spell.\n");
       return(0);
    }                         

    if(ply_ptr->level <5){
        print(fd, "You are not high enough level to cast that yet.\n");
        return(0);
    }
 
    if(cmnd->num < 4) {
        print(fd, "Transport what to whom?\n");
        return(0);
    }

    lowercize(cmnd->str[3], 1);
    crt_ptr = find_who(cmnd->str[3]);
 
    if(!crt_ptr || F_ISSET(crt_ptr, PDMINV) ||
       (F_ISSET(crt_ptr, PINVIS) && !F_ISSET(ply_ptr, PDINVI))) {
        print(fd, "That player is not logged on.\n");
        return(0);
    }
 
    obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
        cmnd->str[2], cmnd->val[2]);
 
    if(!obj_ptr) {
        print(fd, "You don't have that object.\n");
        return(0);
    }

	if(obj_ptr->questnum) {
                print(fd, "You can't transport a quest object.\n");
                return(0);
        }

    cost = 5 + bonus[ply_ptr->intelligence] + (ply_ptr->level - 5)*2;
    if (obj_ptr->weight > cost){
        print(fd,"%I is too heavy to transport at your current level.\n", obj_ptr);
        return(0);
    }

    cost = 8 + (obj_ptr->weight)/4;
    if(ply_ptr->mpcur < cost && how == CAST) {
        print(fd, "Not enough magic points.\n");
        return(0);
    } else if(how == CAST) {
            ply_ptr->mpcur -= cost;
        }
	if(spell_fail(ply_ptr, how)) {
                return(0);
        }

    

    if (max_weight(crt_ptr) < weight_ply(crt_ptr)+obj_ptr->weight){
        print(fd,"%M is unable to hold %1i.\n",crt_ptr,obj_ptr);
        return(0);
    }
    del_obj_crt(obj_ptr, ply_ptr);
    add_obj_crt(obj_ptr, crt_ptr);

    if(!F_ISSET(ply_ptr, PDMINV)){
	    broadcast_rom(fd, ply_ptr->rom_num, "%M transports an object to someone.\n", ply_ptr);
    }
    print(fd,"You concentrate intensely on %i as it dissappears.\n",
        obj_ptr);
    print(fd, "You sucessfully transported %1i to %m.\n", 
        obj_ptr, crt_ptr);
    if(!F_ISSET(ply_ptr, PDMINV)){
	 print(crt_ptr->fd, "%M magically sends you %1i.\n",ply_ptr, obj_ptr);
    }
    return(1);
	
}

