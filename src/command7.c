/*
 * COMMAND7.C:
 *
 *	Additional user routines.
 *
 *	Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"

/**********************************************************************/
/*				flee				      */
/**********************************************************************/

/* This function allows a player to flee from an enemy.  If successful */
/* the player will drop his readied weapon and run through one of the  */
/* visible exits, losing 10% or 1000 experience, whichever is less.    */

int flee(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	room	*rom_ptr;
	xtag	*xp;
	ctag	*cp, *temp;
	char 	found = 0;
	int	fd, n;
	long	i, t;

	rom_ptr = ply_ptr->parent_rom;
	fd = ply_ptr->fd;

	t = time(0);
	i = MAX(ply_ptr->lasttime[LT_ATTCK].ltime,
		ply_ptr->lasttime[LT_SPELL].ltime) + 3L;

	if(t < i && !F_ISSET(ply_ptr, PFEARS)) {
		please_wait(fd, i-t);
		return(0);
	}
        t = Time%24L;
	xp = rom_ptr->first_ext;
	if(xp) do {
		if(F_ISSET(xp->ext, XCLOSD)) continue;
		if((F_ISSET(xp->ext, XCLIMB) || F_ISSET(xp->ext, XDCLIM)) &&  
(!F_ISSET(ply_ptr, PLEVIT))) continue; 
		if(F_ISSET(xp->ext, XNAKED) && weight_ply(ply_ptr)) continue;
        if(F_ISSET(xp->ext, XFEMAL) && F_ISSET(ply_ptr, PMALES)) continue;
        if(F_ISSET(xp->ext, XMALES) && !F_ISSET(ply_ptr, PMALES)) continue; 
		if(F_ISSET(xp->ext, XFLYSP) && !F_ISSET(ply_ptr, PFLYSP))
			continue;
		if(!F_ISSET(ply_ptr, PDINVI) && F_ISSET(xp->ext, XINVIS))
			continue;
		if(F_ISSET(xp->ext, XSECRT)) continue;
		if(F_ISSET(xp->ext, XNOSEE)) continue;
	if(F_ISSET(xp->ext, XPLDGK)) 
		if (!F_ISSET(ply_ptr, PPLDGK)) continue;
		else if (BOOL(F_ISSET(xp->ext,XKNGDM)) !=  BOOL(F_ISSET(ply_ptr, PKNGDM))) 
			continue;
		if(F_ISSET(xp->ext, XNGHTO) && (t>6 && t < 20)) continue;
		if(F_ISSET(xp->ext, XDAYON) && (t<6 || t > 20))  continue;
		
    if(F_ISSET(xp->ext,XPGUAR)){
        cp = rom_ptr->first_mon;
        while(cp) {
            if(F_ISSET(cp->crt, MPGUAR) && 
            !F_ISSET(ply_ptr, PINVIS) && ply_ptr->class < CARETAKER) {
		found = 1;
		break;
            }
            cp = cp->next_tag;
        }
	if(found)
		continue;
    }         
/* if(mrand(1,100) < 70) break;     */
		if(mrand(1,100) < (65 + bonus[ply_ptr->dexterity]*5)) 
			break;
	} while(xp = xp->next_tag);

	 
        if(xp && F_ISSET(xp->ext,52) && mrand(1,5) < 2 && !F_ISSET(ply_ptr, PFEARS))
		xp = 0;
	if(!xp) {
		print(fd, "You failed to escape!\n");
		return(0);
	}

	if(ply_ptr->ready[WIELD-1] &&
	   !F_ISSET(ply_ptr->ready[WIELD-1], OCURSE)) {
		add_obj_rom(ply_ptr->ready[WIELD-1], rom_ptr);
		ply_ptr->ready[WIELD-1] = 0;
		compute_thaco(ply_ptr);
		compute_ac(ply_ptr);
		print(fd, "You drop your weapon and run like a chicken.\n");
	}
	else
		print(fd, "You run like a chicken.\n");

	F_CLR(ply_ptr, PHIDDN);

	if(F_ISSET(ply_ptr, PFEARS)) {
		ANSI(fd, RED);
		print(fd, "You flee in fear!\n");
		ANSI(fd, WHITE);
	}

	
	if(!F_ISSET(rom_ptr, RPTRAK))
		strcpy(rom_ptr->track, xp->ext->name);

	broadcast_rom(fd, rom_ptr->rom_num, "%M flees to the %s.", 
		      ply_ptr, xp->ext->name);

	if (ply_ptr->class == PALADIN)
		if (ply_ptr->level > 5) {
		n = ply_ptr->level*15;
		n = MIN(ply_ptr->experience, n);
		print(fd,"You lose %d experience for your cowardly retreat.\n",n);
		ply_ptr->experience -= n;
		lower_prof(ply_ptr,n);
	}

	load_rom(xp->ext->room, &rom_ptr);

	if(rom_ptr->lolevel > ply_ptr->level || 
	   (ply_ptr->level > rom_ptr->hilevel && rom_ptr->hilevel)) {
		print(fd, "You are thrown back by an invisible force.\n");
		broadcast_rom(fd, rom_ptr->rom_num, "%M just arrived.",ply_ptr);
		return(0);
	}

	n = count_vis_ply(rom_ptr);
	if((F_ISSET(rom_ptr, RONEPL) && n > 0) ||
	   (F_ISSET(rom_ptr, RTWOPL) && n > 1) ||
	   (F_ISSET(rom_ptr, RTHREE) && n > 2)) {
		print(fd, "The room you fled to was full!\n");
		broadcast_rom(fd, rom_ptr->rom_num, "%M just arrived.",ply_ptr);
		return(0);
	}

	del_ply_rom(ply_ptr, ply_ptr->parent_rom);
	add_ply_rom(ply_ptr, rom_ptr);

	check_traps(ply_ptr, rom_ptr);

	return(0);

}

/**********************************************************************/
/*				list				      */
/**********************************************************************/

/* This function allows a player to list the items for sale within a */
/* shoppe.							     */

int list(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	room	*rom_ptr, *dep_ptr;
	otag	*op;
	int	fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(F_ISSET(rom_ptr, RSHOPP)) {
		if(load_rom(rom_ptr->rom_num+1, &dep_ptr) < 0) {
			print(fd, "Nothing to buy.\n");
			return(0);
		}

		print(fd, "You may buy:\n");
		op = dep_ptr->first_obj;
		while(op) {
			print(fd, "   %-30s   Cost: %ld\n", 
			      obj_str(op->obj, 1, CAP), op->obj->value);
			op = op->next_tag;
		}
		print(fd, "\n");
	}

	else
		print(fd, "This is not a shoppe.\n");

	return(0);

}

/**********************************************************************/
/*				buy				      */
/**********************************************************************/

/* This function allows a player to buy something from a shoppe.      */

int buy(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	room	*rom_ptr, *dep_ptr;
	object	*obj_ptr, *obj_ptr2;
	int	fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(!F_ISSET(rom_ptr, RSHOPP)) {
		print(fd, "This is not a shoppe.\n");
		return(0);
	}

	if(cmnd->num < 2) {
		print(fd, "Buy what?\n");
		return(0);
	}

	if(load_rom(rom_ptr->rom_num+1, &dep_ptr) < 0) {
		print(fd, "Nothing to buy.\n");
		return(0);
	}

	obj_ptr = find_obj(ply_ptr, dep_ptr->first_obj,
			   cmnd->str[1], cmnd->val[1]);

	if(!obj_ptr) {
		print(fd, "That's not being sold.\n");
		return(0);
	}

	if(ply_ptr->gold < obj_ptr->value) {
		print(fd, "You don't have enough gold.\n");
		return(0);
	}
 
                if(weight_ply(ply_ptr) + weight_obj(obj_ptr) > 
                   max_weight(ply_ptr)) {
                   print(fd, "You can't carry anymore.\n");
                   return(0);
               }             

	obj_ptr2 = (object *)malloc(sizeof(object));
	if(!obj_ptr2)
		merror("buy", FATAL);

	F_CLR(ply_ptr, PHIDDN);

	*obj_ptr2 = *obj_ptr;
	F_CLR(obj_ptr2, OPERM2);
	F_CLR(obj_ptr2, OPERMT);
	F_CLR(obj_ptr2, OTEMPP);

	add_obj_crt(obj_ptr2, ply_ptr);
	ply_ptr->gold -= obj_ptr2->value;
	print(fd, "Bought.\n");
	broadcast_rom(fd, ply_ptr->rom_num, "%M bought %1i.", 
		      ply_ptr, obj_ptr2);

	return(0);

}

/**********************************************************************/
/*				sell				      */
/**********************************************************************/

/* This function will allow a player to sell an object in a pawn shoppe */

int sell(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	room	*rom_ptr;
	object	*obj_ptr;
	int	gold, fd, poorquality = 0;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(!F_ISSET(rom_ptr, RPAWNS)) {
		print(fd, "This is not a pawn shoppe.\n");
		return(0);
	}

	if(cmnd->num < 2) {
		print(fd, "Sell what?\n");
		return(0);
	}

	F_CLR(ply_ptr, PHIDDN);

	obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
			   cmnd->str[1], cmnd->val[1]);

	if(!obj_ptr) {
		print(fd, "You don't have that.\n");
		return(0);
	}

	gold = MIN(obj_ptr->value / 2, 10000);

	if((obj_ptr->type <= MISSILE || obj_ptr->type == ARMOR) &&
	   obj_ptr->shotscur <= obj_ptr->shotsmax/8)
		poorquality = 1;

	if((obj_ptr->type == WAND || obj_ptr->type == KEY) && obj_ptr->shotscur < 1)
		poorquality = 1;

	if(gold < 20 || poorquality) {
		print(fd, "The shopkeep says, \"I won't buy that crap.\"\n");
		return(0);
	}

	if(obj_ptr->type == SCROLL || obj_ptr->type == POTION) {
		print(fd, "The shopkeep won't buy potions and scrolls.\n");
		return(0);
	}

	print(fd, "The shopkeep gives you %d gold for %i.\n", gold, obj_ptr);
	broadcast_rom(fd, ply_ptr->rom_num, "%M sells %1i.", 
		      ply_ptr, obj_ptr);

	ply_ptr->gold += gold;
	del_obj_crt(obj_ptr, ply_ptr);
	free_obj(obj_ptr);

	return(0);

}

/**********************************************************************/
/*				value				      */
/**********************************************************************/

/* This function allows a player to find out the pawn-shop value of an */
/* object, if he is in the pawn shop.				       */

int value(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	room	*rom_ptr;
	object	*obj_ptr;
	long	value;
	int	fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(!F_ISSET(rom_ptr, RPAWNS) && !F_ISSET(rom_ptr, RREPAI)) {
		print(fd, "You must be in a pawn or repair shoppe.\n");
		return(0);
	}

	if(cmnd->num < 2) {
		print(fd, "Value what?\n");
		return(0);
	}

	F_CLR(ply_ptr, PHIDDN);

	obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
			   cmnd->str[1], cmnd->val[1]);

	if(!obj_ptr) {
		print(fd, "You don't have that.\n");
		return(0);
	}

	if(F_ISSET(rom_ptr, RPAWNS)) {
		value = MIN(obj_ptr->value / 2, 10000L);
		print(fd, "The shopkeep says, \"%I's worth %ld.\"\n",
		      obj_ptr, value);
	}
	else {
		value = obj_ptr->value / 2;
		print(fd, 
		      "The shopkeep says, \"%I costs %ld to be repaired.\"\n", 
		      obj_ptr, value);
	}

	broadcast_rom(fd, ply_ptr->rom_num, "%M gets %i appraised.",
		      ply_ptr, obj_ptr);

	return(0);

}

/**********************************************************************/
/*				backstab			      */
/**********************************************************************/

/* This function allows thieves and assassins to backstab a monster.  */
/* If successful, a damage multiplier is given to the player.  The    */
/* player must be successfully hidden for the backstab to work.  If   */
/* the backstab fails, then the player is forced to wait double the   */
/* normal amount of time for his next attack.			      */

int backstab(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	creature	*crt_ptr;
	room		*rom_ptr;
	long		i, t;
	int		fd, m, n, p, addprof;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(ply_ptr->class != THIEF && ply_ptr->class != ASSASSIN &&
	   ply_ptr->class < CARETAKER) {
		print(fd, "Only thieves and assassins may backstab.\n");
		return(0);
	}

	if(cmnd->num < 2 || F_ISSET(ply_ptr, PBLIND)) {
		print(fd, "Backstab what?\n");
		return(0);
	}

	if(!ply_ptr->ready[WIELD-1] || (ply_ptr->ready[WIELD-1]->type 
	   != SHARP && ply_ptr->ready[WIELD-1]->type != THRUST)) {
		print(fd, "Backstab requires sharp or thrusting weapons.\n");
		return(0);
	}

	t = time(0);
	i = LT(ply_ptr, LT_ATTCK);

	if(t < i) {
		please_wait(fd, i-t);
		return(0);
	}

	crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
			   cmnd->str[1], cmnd->val[1]);

	if(!crt_ptr) {
		cmnd->str[1][0] = up(cmnd->str[1][0]);
		crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
				   cmnd->str[1], cmnd->val[1]);

		if(!crt_ptr || crt_ptr == ply_ptr || strlen(cmnd->str[1]) < 3) {
			print(fd, "You don't see that here.\n");
			return(0);
		}

	}

	if(crt_ptr->type != PLAYER && is_enm_crt(ply_ptr->name, crt_ptr)) {
		print(fd, "Not while you're already fighting %s.\n",
		      F_ISSET(crt_ptr, MMALES) ? "him":"her");
		return(0);
	}

	if(crt_ptr->type == PLAYER) {
		if(F_ISSET(rom_ptr, RNOKIL)) {
			print(fd, "No killing allowed in this room.\n");
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
		if(is_charm_crt(ply_ptr->name, crt_ptr) && F_ISSET(ply_ptr, PCHARM)){
			print (fd, "Why would you want to backstab your good friend %s?\n", crt_ptr->name);
			return(0);
		}
            }

		print(crt_ptr->fd, "%M backstabs you.\n", ply_ptr);
	}
	else if(F_ISSET(crt_ptr, MUNKIL)) {
		print(fd, "You cannot harm %s.\n",
			F_ISSET(crt_ptr, MMALES) ? "him":"her");
		return(0);
	}
	if(is_charm_crt(crt_ptr->name, ply_ptr) && F_ISSET(ply_ptr, PCHARM)) {
                print(fd, "You are too fond of %s to do that.\n", crt_ptr->name);
                return(0);
        }

	if(F_ISSET(ply_ptr, PINVIS)) {
		F_CLR(ply_ptr, PINVIS);
		print(fd, "Your invisibility fades.\n");
		broadcast_rom(fd, ply_ptr->rom_num, "%M fades into view.",
			      ply_ptr);
	}

	ply_ptr->lasttime[LT_ATTCK].ltime = t;
	if(ply_ptr->dexterity > 18)
		ply_ptr->lasttime[LT_ATTCK].interval = 2;
	else
		ply_ptr->lasttime[LT_ATTCK].interval = 3;

	if(crt_ptr->type != PLAYER) {
		if(add_enm_crt(ply_ptr->name, crt_ptr) < 0) {
			print(fd, "You backstab %m.\n", crt_ptr);
			broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
				       "%M backstabs %m.", ply_ptr, crt_ptr);
 			if(F_ISSET(crt_ptr, MMGONL)) {
            	print(fd, "Your weapon has no effect on %m.\n", crt_ptr);
            	return(0);
        	}
        	if(F_ISSET(crt_ptr, MENONL)) {
            	if(!ply_ptr->ready[WIELD-1] || 
                	ply_ptr->ready[WIELD-1]->adjustment < 1) {
                	print(fd, "Your weapon has no effect on %m.\n", crt_ptr);
                	return(0);
            }
        }  

		}
	}
	else {
		print(fd, "You backstab %m.\n", crt_ptr);
		print(crt_ptr->fd, "%M backstabs you.\n", ply_ptr);
		broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
			       "%M backstabs %m!", ply_ptr, crt_ptr);
	}

	if(ply_ptr->ready[WIELD-1]) {
		if(ply_ptr->ready[WIELD-1]->shotscur < 1) {
			print(fd, "Your %s is broken.\n", 
			      ply_ptr->ready[WIELD-1]->name);
			add_obj_crt(ply_ptr->ready[WIELD-1], ply_ptr);
			ply_ptr->ready[WIELD-1] = 0;
			broadcast_rom(fd, ply_ptr->rom_num, 
				      "%s backstab failed.", 
				      F_ISSET(ply_ptr, PMALES) ? "His":"Her");
			ply_ptr->lasttime[LT_ATTCK].interval *= 2;
			return(0);
		}
	}

	n = ply_ptr->thaco - crt_ptr->armor/10 + 2;
	if(!F_ISSET(ply_ptr, PHIDDN))
		n = 21;

	F_CLR(ply_ptr, PHIDDN);

	if(mrand(1,20) >= n) {
		if(ply_ptr->ready[WIELD-1])
			n = mdice(ply_ptr->ready[WIELD-1]);
		else
			n = mdice(ply_ptr);

		if(ply_ptr->class == THIEF)
			n *= (mrand(20,35) / 10);
		else
			n *= 3;

		m = MIN(crt_ptr->hpcur, n);
		if(crt_ptr->type != PLAYER) {
			add_enm_dmg(ply_ptr->name, crt_ptr, m);
			if(ply_ptr->ready[WIELD-1]) {
				p = MIN(ply_ptr->ready[WIELD-1]->type, 4);
				addprof = (m * crt_ptr->experience) /
					crt_ptr->hpmax;
				addprof = MIN(addprof, crt_ptr->experience);
				ply_ptr->proficiency[p] += addprof;
			}
		}
		crt_ptr->hpcur -= n;

		print(fd, "You hit for %d damage.\n", n);
		print(crt_ptr->fd, "%M hit you for %d damage.\n", ply_ptr, n);

		if(crt_ptr->hpcur < 1) {
			print(fd, "You killed %m.\n", crt_ptr);
			broadcast_rom(fd, ply_ptr->rom_num,
				      "%M killed %m.", ply_ptr, crt_ptr);
			if(ply_ptr->ready[WIELD-1])
				ply_ptr->ready[WIELD-1]->shotscur--;
			die(crt_ptr, ply_ptr);
		}
		else
			check_for_flee(crt_ptr);
	}
	else {
		print(fd, "You missed.\n");
		broadcast_rom(fd, ply_ptr->rom_num, "%s backstab failed.", 
			      F_ISSET(ply_ptr, PMALES) ? "His":"Her");
		ply_ptr->lasttime[LT_ATTCK].interval *= 3;
	}

	return(0);

}

/**********************************************************************/
/*				train				      */
/**********************************************************************/

/* This function allows a player to train if he is in the correct     */
/* training location and has enough gold and experience.  If so, the  */
/* character goes up a level.					      */

int train(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	room	*rom_ptr;
	long	goldneeded, expneeded, lv;
	int	fd, i, fail = 0, bit[4];

	fd = ply_ptr->fd;
	
	if(F_ISSET(ply_ptr, PBLIND)){
		ANSI(fd, RED);
		print(fd, "You can't...You're blind!\n");
		ANSI(fd, WHITE);
		return(0);
	}

	rom_ptr = ply_ptr->parent_rom;

	bit[0] = ply_ptr->class & 1;
	bit[1] = ply_ptr->class & 2;
	bit[2] = ply_ptr->class & 4;
	bit[3] = ply_ptr->class & 8;

	for(i=0; i<4; i++) {
		if(!((bit[i] && F_ISSET(rom_ptr, RTRAIN+3-i)) ||
		   (!bit[i] && !F_ISSET(rom_ptr, RTRAIN+3-i))))
			fail = 1;
	}

	if(fail) {
		print(fd, "This is not your training location.\n");
		return(0);
	}

	if(ply_ptr->level < MAXALVL)
		expneeded = needed_exp[ply_ptr->level-1];
	else
               expneeded = (long)((needed_exp[MAXALVL-1]*ply_ptr->level));   

	goldneeded = expneeded / 2L;

	if(expneeded > ply_ptr->experience) {
		print(fd, "You need %ld more experience.\n",
		      expneeded - ply_ptr->experience);
		return(0);
	}

	if(goldneeded > ply_ptr->gold) {
		print(fd, "You don't have enough gold.\n");
		print(fd, "You need %ld gold to train.\n", goldneeded);
		return(0);
	}

	ply_ptr->gold -= goldneeded;
	up_level(ply_ptr);

	broadcast("### %s just made a level!", ply_ptr->name);
	print(fd, "Congratulations, you made a level!\n\n");

	return(0);

}
