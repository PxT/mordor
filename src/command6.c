/*
 * COMMAND6.C:
 *
 *	Additional user routines.
 *
 *	Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"

/**********************************************************************/
/*				yell				      */
/**********************************************************************/

/* This function allows a player to yell something that will be heard */
/* not only in his room, but also in all rooms adjacent to him.  In   */
/* the adjacent rooms, however, people will not know who yelled.      */

int yell(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	room		*rom_ptr;
	xtag		*xp;
	char		str[300];
	int		index = -1, i, fd;
	int		len;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;
	len = strlen(cmnd->fullstr);
	for(i=0; i<len; i++) {
		if(cmnd->fullstr[i] == ' ') {
			index = i+1;
			break;
		}
	}
	str[256]=0;

	if(index == -1 || strlen(&cmnd->fullstr[index]) < 1) {
		print(fd, "Yell what?\n");
		return(0);
	}
	if(F_ISSET(ply_ptr, PSILNC)) {
		print(fd, "Your voice is too weak to yell.\n");
		return(0);
	}
	F_CLR(ply_ptr, PHIDDN);
	print(fd, "Ok.\n");

	broadcast_rom(fd, rom_ptr->rom_num, "%M yells, \"%s!\"", ply_ptr,
		      &cmnd->fullstr[index]);

	sprintf(str, "Someone yells, \"%s!\"", &cmnd->fullstr[index]);

	xp = rom_ptr->first_ext;
	while(xp) {
		if(is_rom_loaded(xp->ext->room))
			broadcast_rom(fd, xp->ext->room, "%s", str);
		xp = xp->next_tag;
	}

	return(0);

}

/**********************************************************************/
/*				go				      */
/**********************************************************************/

/* This function allows a player to go through an exit, other than one */
/* of the 6 cardinal directions.				       */

int go(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	room		*rom_ptr, *old_rom_ptr;
	creature	*crt_ptr;
	ctag		*cp, *temp;
	exit_		*ext_ptr;
	long		i, t;
	int		fd, old_rom_num, fall, dmg, n;

	rom_ptr = ply_ptr->parent_rom;
	fd = ply_ptr->fd;

	if(cmnd->num < 2) {
		print(fd, "Go where?\n");
		return(0);
	}

	ext_ptr = find_ext(ply_ptr, rom_ptr->first_ext, 
			   cmnd->str[1], cmnd->val[1]);

	if(!ext_ptr) {
		print(fd, "I don't see that exit.\n");
		return(0);
	}

	if(F_ISSET(ext_ptr, XLOCKD)) {
		print(fd, "It's locked.\n");
		return(0);
	}
	else if(F_ISSET(ext_ptr, XCLOSD)) {
		print(fd, "You have to open it first.\n");
		return(0);
	}

	if(F_ISSET(ext_ptr, XFLYSP) && !F_ISSET(ply_ptr, PFLYSP)) {
		print(fd, "You must fly to get there.\n");
		return(0);
	}

        t = Time%24L;
	if(F_ISSET(ext_ptr, XNGHTO) && (t>6 && t < 20)) {
       		print(fd, "That exit is not open during the day.\n");
        	return(0);
    	}          

	if(F_ISSET(ext_ptr, XDAYON) && (t<6 || t > 20)) {
        	print(fd, "That exit is closed for the night.\n");
        	return(0);
    	}          
 
    if(F_ISSET(ext_ptr,XPGUAR)){
        cp = rom_ptr->first_mon;
        while(cp) {
            if(F_ISSET(cp->crt, MPGUAR)) {
            if(!F_ISSET(ply_ptr, PINVIS) && ply_ptr->class < CARETAKER) {
                print(fd, "%M blocks your exit.\n", cp->crt);
                return(0);
            	}
            if(F_ISSET(cp->crt, MDINVI) && ply_ptr->class < CARETAKER) {
                print(fd, "%M blocks your exit.\n", cp->crt);
                return(0);
            	}
	    }	
            cp = cp->next_tag;
        }
    }         

if(F_ISSET(ext_ptr, XPLDGK))
	if (!F_ISSET(ply_ptr, PPLDGK)){
       		print(fd, "You do not have the proper authorization to go there.\n");
        	return(0);
	}
	else if (BOOL(F_ISSET(ext_ptr,XKNGDM)) !=  BOOL(F_ISSET(ply_ptr, PKNGDM))){
        	print(fd, "You belong to the wrong organization to go there.\n");
        	return(0);
	}

	if(F_ISSET(ext_ptr, XNAKED) && weight_ply(ply_ptr)) {
		print(fd, "You cannot bring anything through that exit.\n");
		return(0);
	}
 
	if(F_ISSET(ext_ptr, XFEMAL) && F_ISSET(ply_ptr, PMALES)){
		print(fd, "Sorry, only females are allowed to go there.\n");
		return(0); 
	}

	if(F_ISSET(ext_ptr, XMALES) && !F_ISSET(ply_ptr, PMALES)){
		print(fd, "Sorry, only males are allowed to go there.\n");
		return(0); 
	}           

	if((F_ISSET(ext_ptr, XCLIMB) || F_ISSET(ext_ptr, XREPEL)) &&
	   !F_ISSET(ply_ptr, PLEVIT)) {
		fall = (F_ISSET(ext_ptr, XDCLIM) ? 50:0) + 50 - 
		       fall_ply(ply_ptr);

		if(mrand(1,100) < fall) {
			dmg = mrand(5, 15+fall/10);
			if(ply_ptr->hpcur <= dmg){
        	           print(fd, "You fell to your death.\n");
	                   ply_ptr->hpcur=0;
			   broadcast_rom(fd, ply_ptr->rom_num, "%M died from the fall.\n", ply_ptr);
                   	   die(ply_ptr, ply_ptr);
                   	   return(0);
            		}

			ply_ptr->hpcur -= dmg;
			print(fd, "You fell and hurt yourself for %d damage.\n",
			      dmg);
			broadcast_rom(fd, ply_ptr->rom_num, "%M fell down.", 
				      ply_ptr);

			if(ply_ptr->hpcur < 1) { 
	                   print(fd, "You fell to your death.\n");
                               die(ply_ptr, ply_ptr); 
			}  
			if(F_ISSET(ext_ptr, XCLIMB))
				return(0);
		}
	}

	i = LT(ply_ptr, LT_ATTCK);
	t = time(0);

	if(t < i) {
		please_wait(fd, i-t);
		return(0);
	}

	F_CLR(ply_ptr, PHIDDN);

	cp = rom_ptr->first_mon;
	while(cp) {
		if(F_ISSET(cp->crt, MBLOCK) && 
		   is_enm_crt(ply_ptr->name, cp->crt) &&
		   !F_ISSET(ply_ptr, PINVIS) && ply_ptr->class < CARETAKER) {
			print(fd, "%M blocks your exit.\n", cp->crt);
			return(0);
		}
		cp = cp->next_tag;
	}

	if(!F_ISSET(rom_ptr, RPTRAK))
		strcpy(rom_ptr->track, ext_ptr->name);

	old_rom_num = rom_ptr->rom_num;
	old_rom_ptr = rom_ptr;

	load_rom(ext_ptr->room, &rom_ptr);
	if(rom_ptr == old_rom_ptr) {
		print(fd, "Off map in that direction.\n");
		return(0);
	}

	n = count_vis_ply(rom_ptr);

	if(rom_ptr->lolevel > ply_ptr->level && ply_ptr->class < CARETAKER) {
		print(fd, "You must be at least level %d to go that way.\n",
		      rom_ptr->lolevel);
		return(0);
	}
	else if(ply_ptr->level > rom_ptr->hilevel && rom_ptr->hilevel &&
		ply_ptr->class < CARETAKER) {
		print(fd, "Only players under level %d may go that way.\n",
		      rom_ptr->hilevel+1);
		return(0);
	}
	else if((F_ISSET(rom_ptr, RONEPL) && n > 0) ||
		(F_ISSET(rom_ptr, RTWOPL) && n > 1) ||
		(F_ISSET(rom_ptr, RTHREE) && n > 2)) {
		print(fd, "That room is full.\n");
		return(0);
	}

	if(strcmp(ext_ptr->name, "up") && strcmp(ext_ptr->name, "down") 
	   && strcmp(ext_ptr->name, "out") && !F_ISSET(ply_ptr, PDMINV)) {
		broadcast_rom(fd, old_rom_ptr->rom_num, "%M went to the %s.", 
			      ply_ptr, ext_ptr->name);
	}

	else if(!F_ISSET(ply_ptr, PDMINV)) {
		broadcast_rom(fd, old_rom_ptr->rom_num, "%M went %s.", 
			      ply_ptr, ext_ptr->name);
	}

	del_ply_rom(ply_ptr, ply_ptr->parent_rom);
	add_ply_rom(ply_ptr, rom_ptr);

	cp = ply_ptr->first_fol;
	while(cp) {
		if(cp->crt->rom_num == old_rom_num && cp->crt->type != MONSTER)
			go(cp->crt, cmnd);
		if(F_ISSET(cp->crt, MDMFOL) && cp->crt->type == MONSTER) {
                del_crt_rom(cp->crt, old_rom_ptr);
                broadcast_rom(fd, old_rom_ptr->rom_num, "%M just wandered to the %s.\n", cp->crt,ext_ptr->name);
                add_crt_rom(cp->crt, rom_ptr, 1);
                add_active(cp->crt);
                }
		cp = cp->next_tag;
	}

	if(is_rom_loaded(old_rom_num)) {
		cp = old_rom_ptr->first_mon;
		while(cp) {
			if(!F_ISSET(cp->crt, MFOLLO) || F_ISSET(cp->crt, MDMFOL)) {
				cp = cp->next_tag;
				continue;
			}
			if(!cp->crt->first_enm) {
				cp = cp->next_tag;
				continue;
			}
			if(strcmp(cp->crt->first_enm->enemy, ply_ptr->name)) {
				cp = cp->next_tag;
				continue;
			}
			if(mrand(1,20) > 10 - ply_ptr->dexterity +
			   cp->crt->dexterity) {
				cp = cp->next_tag;
				continue;
			}
			print(fd, "%M followed you.\n", cp->crt);
			broadcast_rom(fd, old_rom_num, "%M follows %m.",
				      cp->crt, ply_ptr);
			temp = cp->next_tag;
			crt_ptr = cp->crt;
			del_crt_rom(crt_ptr, old_rom_ptr);
			add_crt_rom(crt_ptr, rom_ptr, 1);
			add_active(crt_ptr);
			F_CLR(crt_ptr, MPERMT);
			cp = temp;
		}
	}

	check_traps(ply_ptr, rom_ptr);
	return(0);
}

/**********************************************************************/
/*				openexit			      */
/**********************************************************************/

/* This function allows a player to open a door if it is not already */
/* open and if it is not locked.				     */

int openexit(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	exit_	*ext_ptr;
	room	*rom_ptr;
	int	fd;

	rom_ptr = ply_ptr->parent_rom;
	fd = ply_ptr->fd;

	if(cmnd->num < 2) {
		print(fd, "Open what?\n");
		return(0);
	}

	ext_ptr = find_ext(ply_ptr, rom_ptr->first_ext,
			   cmnd->str[1], cmnd->val[1]);

	if(!ext_ptr) {
		print(fd, "I don't see that exit.\n");
		return(0);
	}

	if(F_ISSET(ext_ptr, XLOCKD)) {
		print(fd, "It's locked.\n");
		return(0);
	}

	if(!F_ISSET(ext_ptr, XCLOSD)) {
		print(fd, "It's already open.\n");
		return(0);
	}

	F_CLR(ply_ptr, PHIDDN);

	F_CLR(ext_ptr, XCLOSD);
	ext_ptr->ltime.ltime = time(0);

	print(fd, "You open the %s.\n", ext_ptr->name);
	broadcast_rom(fd, ply_ptr->rom_num, "%M opens the %s.",
		      ply_ptr, ext_ptr->name);

	return(0);

}

/**********************************************************************/
/*				closeexit			      */
/**********************************************************************/

/* This function allows a player to close a door, if the door is not */
/* already closed, and if indeed it is a door.			     */

int closeexit(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	exit_	*ext_ptr;
	room	*rom_ptr;
	int	fd;

	rom_ptr = ply_ptr->parent_rom;
	fd = ply_ptr->fd;

	if(cmnd->num < 2) {
		print(fd, "Close what?\n");
		return(0);
	}

	ext_ptr = find_ext(ply_ptr, rom_ptr->first_ext,
			   cmnd->str[1], cmnd->val[1]);

	if(!ext_ptr) {
		print(fd, "I don't see that exit.\n");
		return(0);
	}

	if(F_ISSET(ext_ptr, XCLOSD)) {
		print(fd, "It's already closed.\n");
		return(0);
	}

	if(!F_ISSET(ext_ptr, XCLOSS)) {
		print(fd, "You can't close that.\n");
		return(0);
	}

	F_CLR(ply_ptr, PHIDDN);

	F_SET(ext_ptr, XCLOSD);

	print(fd, "You close the %s.\n", ext_ptr->name);
	broadcast_rom(fd, ply_ptr->rom_num, "%M closes the %s.",
		      ply_ptr, ext_ptr->name);

	return(0);

}

/**********************************************************************/
/*				unlock				      */
/**********************************************************************/

/* This function allows a player to unlock a door if he has the correct */
/* key, and it is locked.						*/

int unlock(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	room	*rom_ptr;
	object	*obj_ptr;
	exit_	*ext_ptr;
	int	fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;
	
	if(cmnd->num < 2) {
		print(fd, "Unlock what?\n");
		return(0);
	}

	ext_ptr = find_ext(ply_ptr, rom_ptr->first_ext,
			   cmnd->str[1], cmnd->val[1]);

	if(!ext_ptr) {
		print(fd, "Unlock what?\n");
		return(0);
	}

	if(!F_ISSET(ext_ptr, XLOCKD)) {
		print(fd, "It's not locked.\n");
		return(0);
	}

	if(cmnd->num < 3) {
		print(fd, "Unlock it with what?\n");
		return(0);
	}

	obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
			   cmnd->str[2], cmnd->val[2]);

	if(!obj_ptr) {
		print(fd, "You don't have that.\n");
		return(0);
	}

	if(obj_ptr->type != KEY) {
		print(fd, "That's not a key.\n");
		return(0);
	}

	if(obj_ptr->shotscur < 1) {
		print(fd, "%I is broken.\n", obj_ptr);
		return(0);
	}

	if(obj_ptr->ndice != ext_ptr->key) {
		print(fd, "Wrong key.\n");
		return(0);
	}

	F_CLR(ply_ptr, PHIDDN);

	F_CLR(ext_ptr, XLOCKD);
	ext_ptr->ltime.ltime = time(0);
	obj_ptr->shotscur--;

	if(obj_ptr->use_output[0])
		print(fd, "%s\n", obj_ptr->use_output);
	else
		print(fd, "Click.\n");
	broadcast_rom(fd, ply_ptr->rom_num, "%M unlocks the %s.",
		      ply_ptr, ext_ptr->name);

	return(0);

}

/**********************************************************************/
/*				lock				      */
/**********************************************************************/

/* This function allows a player to lock a door with the correct key. */

int lock(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	room	*rom_ptr;
	object	*obj_ptr;
	exit_	*ext_ptr;
	int	fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;
	
	if(cmnd->num < 2) {
		print(fd, "Lock what?\n");
		return(0);
	}

	ext_ptr = find_ext(ply_ptr, rom_ptr->first_ext,
			   cmnd->str[1], cmnd->val[1]);

	if(!ext_ptr) {
		print(fd, "Lock what?\n");
		return(0);
	}

	if(F_ISSET(ext_ptr, XLOCKD)) {
		print(fd, "It's already locked.\n");
		return(0);
	}

	if(cmnd->num < 3) {
		print(fd, "Lock it with what?\n");
		return(0);
	}

	obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
			   cmnd->str[2], cmnd->val[2]);

	if(!obj_ptr) {
		print(fd, "You don't have that.\n");
		return(0);
	}

	if(obj_ptr->type != KEY) {
		print(fd, "%I is not a key.\n", obj_ptr);
		return(0);
	}

	if(!F_ISSET(ext_ptr, XLOCKS)) {
		print(fd, "You can't lock it.\n");
		return(0);
	}

	if(!F_ISSET(ext_ptr, XCLOSD)) {
		print(fd, "You have to close it first.\n");
		return(0);
	}

	if(obj_ptr->shotscur < 1) {
		print(fd, "%I is broken.\n", obj_ptr);
		return(0);
	}

	if(obj_ptr->ndice != ext_ptr->key) {
		print(fd, "Wrong key.\n");
		return(0);
	}

	F_CLR(ply_ptr, PHIDDN);

	F_SET(ext_ptr, XLOCKD);

	print(fd, "Click.\n");
	broadcast_rom(fd, ply_ptr->rom_num, "%M locks the %s.",
		      ply_ptr, ext_ptr->name);

	return(0);

}

/**********************************************************************/
/*				picklock			      */
/**********************************************************************/

/* This function is called when a thief or assassin attempts to pick a */
/* lock.  If the lock is pickable, there is a chance (depending on the */
/* player's level) that the lock will be picked.		       */

int picklock(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	room	*rom_ptr;
	exit_	*ext_ptr;
	long	i, t;
	int	fd, chance;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(ply_ptr->class != THIEF && ply_ptr->class < CARETAKER) {
		print(fd, "Only thieves may pick locks.\n");
		return(0);
	}

	if(cmnd->num < 2) {
		print(fd, "Pick what?\n");
		return(0);
	}

	ext_ptr = find_ext(ply_ptr, rom_ptr->first_ext,
			   cmnd->str[1], cmnd->val[1]);

	if(!ext_ptr) {
		print(fd, "I don't see that here.\n");
		return(0);
	}
	if(F_ISSET(ply_ptr, PBLIND)) {
		print(fd, "You can't see to pick that lock.\n");
		return(0);
	}
	if(!F_ISSET(ext_ptr, XLOCKD)) {
		print(fd, "It's not locked.\n");
		return(0);
	}

	F_CLR(ply_ptr, PHIDDN);

	i = LT(ply_ptr, LT_PICKL);
	t = time(0);

	if(t < i) {
		please_wait(fd, i-t);
		return(0);
	}

	ply_ptr->lasttime[LT_PICKL].ltime = t;
	ply_ptr->lasttime[LT_PICKL].interval = 10;

	chance = (ply_ptr->class == THIEF) ? 10*ply_ptr->level :
		 5*ply_ptr->level;
	chance += bonus[ply_ptr->dexterity]*2;

	if(F_ISSET(ext_ptr, XUNPCK))
		chance = 0;

	broadcast_rom(fd, ply_ptr->rom_num, "%M attempts to pick the %s.", 
		      ply_ptr, ext_ptr->name);

	if(mrand(1,100) <= chance) {
		print(fd, "You successfully picked the lock.\n");
		F_CLR(ext_ptr, XLOCKD);
		broadcast_rom(fd, ply_ptr->rom_num, "%s succeeded.",
			      F_ISSET(ply_ptr, PMALES) ? "He":"She");
	}
	else
		print(fd, "You failed.\n");

	return(0);

}

/**********************************************************************/
/*				steal				      */
/**********************************************************************/

/* This function allows a player to steal an item from a monster within  */
/* the game.  Only thieves may steal, and the monster must               */
/* have the object which is being stolen within his inventory.		 */

int steal(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	creature	*crt_ptr;
	room		*rom_ptr;
	object		*obj_ptr;
	long		i, t;
	int		fd, chance;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(cmnd->num < 2) {
		print(fd, "Steal what?\n");
		return(0);
	}

	if(cmnd->num < 3) {
		print(fd, "Steal what from whom?\n");
		return(0);
	}

	if(ply_ptr->class != THIEF && ply_ptr->class < CARETAKER) {
		print(fd, "Only thieves may steal.\n");
		return(0);
	}

	F_CLR(ply_ptr, PHIDDN);

	i = LT(ply_ptr, LT_STEAL);
	t = time(0);

	if(t < i) {
		please_wait(fd, i-t);
		return(0);
	}

	if(F_ISSET(ply_ptr, PINVIS)) {
		F_CLR(ply_ptr, PINVIS);
		print(fd, "Your invisibility fades.\n");
		broadcast_rom(fd, ply_ptr->rom_num, "%M fades into view.",
			      ply_ptr);
	}

	ply_ptr->lasttime[LT_STEAL].ltime = t;
	ply_ptr->lasttime[LT_STEAL].interval = 5;

	crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
			   cmnd->str[2], cmnd->val[2]);

	if(!crt_ptr) {
		cmnd->str[2][0] = up(cmnd->str[2][0]);
		crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
				   cmnd->str[2], cmnd->val[2]);

		if(!crt_ptr || crt_ptr == ply_ptr || F_ISSET(ply_ptr, PBLIND)) {
			print(fd, "I don't see that here.\n");
			return(0);
		}

	}

	if(crt_ptr->type == MONSTER) {
		if(F_ISSET(crt_ptr, MUNKIL)) {
			print(fd, "You cannot harm %s.\n",
				F_ISSET(crt_ptr, MMALES) ? "him":"her");
			return(0);
		}
		if(is_enm_crt(ply_ptr->name, crt_ptr)) {
			print(fd, "Not while %s's attacking you.\n",
			      F_ISSET(crt_ptr, MMALES) ? "he":"she");
			return(0);
		}
	}
	else {
		if(F_ISSET(rom_ptr, RNOKIL)) {
			print(fd, "No stealing allowed in this room.\n");
			return(0);
		}

            if((!F_ISSET(ply_ptr,PPLDGK) || !F_ISSET(crt_ptr,PPLDGK)) ||
                (BOOL(F_ISSET(ply_ptr,PKNGDM)) == BOOL(F_ISSET(crt_ptr,PKNGDM))) ||
                (! AT_WAR)) {
                if(!F_ISSET(ply_ptr, PCHAOS)) {
                    print(fd, "Sorry, you're lawful.\n");
                    return (0);
                }
                if(!F_ISSET(crt_ptr, PCHAOS)) {
                    print(fd, "Sorry, that player is lawful.\n");
                    return (0);
                }     
            }
	}
	if(F_ISSET(ply_ptr, PBLIND)) {
		print(fd, "How do you do that?  You're blind.\n");
		return(0);
	}
	obj_ptr = find_obj(ply_ptr, crt_ptr->first_obj,
			   cmnd->str[1], cmnd->val[1]);

	if(!obj_ptr) {
		print(fd, "%s doesn't have that.\n",
		      F_ISSET(crt_ptr, MMALES) ? "He":"She");
		return(0);
	}

	chance = (ply_ptr->class == THIEF) ? 4*ply_ptr->level : 
		 3*ply_ptr->level;
	chance += bonus[ply_ptr->dexterity]*3;
	if(crt_ptr->level > ply_ptr->level)
		chance -= 15*(crt_ptr->level - ply_ptr->level);
	if(obj_ptr->questnum) chance = 0;
	chance = MIN(chance, 65);

	if(obj_ptr->questnum || F_ISSET(crt_ptr, MUNSTL))
		chance = 0;
	
	if(ply_ptr->class == DM)
		chance = 100;

	if(mrand(1,100) <= chance) {
		print(fd, "You succeeded.\n");
		del_obj_crt(obj_ptr, crt_ptr);
		add_obj_crt(obj_ptr, ply_ptr);
		if(crt_ptr->type == PLAYER) {
			ply_ptr->lasttime[LT_PLYKL].ltime = t;
			ply_ptr->lasttime[LT_PLYKL].interval = 
				(long)mrand(7,10) * 86400L;
	
		}
	}

	else {
		print(fd, "You failed.\n%s", crt_ptr->type == MONSTER ? 
		      "You are attacked.\n" : "");
		broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
			      "%M tried to steal from %m.", 
			      ply_ptr, crt_ptr);

		if(crt_ptr->type == PLAYER)
			print(crt_ptr->fd, "%M tried to steal %1i from you.\n",
			      ply_ptr, obj_ptr);
		else
			add_enm_crt(ply_ptr->name, crt_ptr);
	}

	return(0);

}
