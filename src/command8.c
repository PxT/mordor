/*
 * COMMAND8.C:
 *
 *	Additional user routines.
 *
 *	Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"
#include <sys/time.h>

/**********************************************************************/
/*				give				      */
/**********************************************************************/

/* This function allows a player to give an item in his inventory to */
/* another player or monster.					     */

int give(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	object		*obj_ptr;
	creature	*crt_ptr;
	room		*rom_ptr;
	int		fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(cmnd->num < 3) {
		print(fd, "Give what to whom?\n");
		return(0);
	}

	if(cmnd->str[1][0] == '$') {
		give_money(ply_ptr, cmnd);
		return(0);
	}

	obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
			   cmnd->str[1], cmnd->val[1]);

	if(!obj_ptr) {
		print(fd, "You don't have that.\n");
		return(0);
	}

	F_CLR(ply_ptr, PHIDDN);

	cmnd->str[2][0] = up(cmnd->str[2][0]);
	crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
			   cmnd->str[2], cmnd->val[2]);

	if(!crt_ptr) {
		cmnd->str[2][0] = low(cmnd->str[2][0]);
		crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
				   cmnd->str[2], cmnd->val[2]);
		if(!crt_ptr) {
			print(fd, "I don't see that person here.\n");
			return(0);
		}
	}

	if(crt_ptr == ply_ptr) {
		print(fd, "You can't give something to yourself.\n");
		return(0);
	}

	if(obj_ptr->questnum) {
		print(fd, "You can't give away a quest object.\n");
		return(0);
	}

	if(crt_ptr->type == PLAYER && (weight_ply(crt_ptr) + 
	   weight_obj(obj_ptr) > max_weight(crt_ptr))) {
		print(fd, "%s can't hold anymore.\n", crt_ptr->name);
		return(0);
	}

	del_obj_crt(obj_ptr, ply_ptr);
	add_obj_crt(obj_ptr, crt_ptr);

	print(fd, "You give %1i to %m.\n", obj_ptr, crt_ptr);
	print(crt_ptr->fd, "%M gave %1i to you.\n", ply_ptr, obj_ptr);
	broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, "%M gave %1i to %m.",
		       ply_ptr, obj_ptr, crt_ptr);

	return(0);

}

/**********************************************************************/
/*				give_money			      */
/**********************************************************************/

/* This function allows a player to give gold to another player.  Gold */
/* is interpreted as gold if it is preceded by a dollar sign.	       */

void give_money(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	creature	*crt_ptr;
	room		*rom_ptr;
	long		amt;
	int		fd;

	rom_ptr = ply_ptr->parent_rom;
	fd = ply_ptr->fd;

	amt = atol(&cmnd->str[1][1]);
	if(amt < 1) {
		print(fd, "Get real.\n");
		return;
	}
	if(amt > ply_ptr->gold) {
		print(fd, "You don't have that much gold.\n");
		return;
	}

	cmnd->str[2][0] = up(cmnd->str[2][0]);
	crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
			   cmnd->str[2], cmnd->val[2]);

	if(!crt_ptr || crt_ptr == ply_ptr) {
		cmnd->str[2][0] = low(cmnd->str[2][0]);
		crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
			  cmnd->str[2], cmnd->val[2]);

		if(!crt_ptr) {
			print(fd, "That person is not here.\n");
			return;
		}
	}

	crt_ptr->gold += amt;
	ply_ptr->gold -= amt;

	print(fd, "Ok.\n");
	
	print(crt_ptr->fd, "%M gave you %ld gold pieces.\n", 
	      ply_ptr, amt);
	broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
		       "%M gave %m %ld gold pieces.", ply_ptr, crt_ptr, amt);

}

/**********************************************************************/
/*				repair				      */
/**********************************************************************/

/* This function allows a player to repair a weapon or armor if he is */
/* in a repair shoppe.  There is a chance of breakage.		      */

int repair(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	object	*obj_ptr;
	room	*rom_ptr;
	long	cost;
	int	fd, broke;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(cmnd->num < 2) {
		print(fd, "Repair what?\n");
		return(0);
	}

	if(!F_ISSET(rom_ptr, RREPAI)) {
		print(fd, "This is not a repair shop.\n");
		return(0);
	}

	obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
			   cmnd->str[1], cmnd->val[1]);

	if(!obj_ptr) {
		print(fd, "You don't have that.\n");
		return(0);
	}

	F_CLR(ply_ptr, PHIDDN);

	if(F_ISSET(obj_ptr, ONOFIX)) {
		print(fd, "The smithy cannot repair that.\n");
		return(0);
	}

	if(obj_ptr->type > MISSILE && obj_ptr->type != ARMOR) {
		print(fd, "You can only repair weapons and armor.\n");
		return(0);
	}

	if(obj_ptr->shotscur > MAX(3, obj_ptr->shotsmax/10)) {
		print(fd, "It's not broken yet.\n");
		return(0);
	}

	cost = obj_ptr->value / 2;

	if(ply_ptr->gold < cost) {
		print(fd, "You don't have enough money.\n");
		return(0);
	}

	print(fd, "The smithy takes %ld gold pieces from you.\n", cost);
	ply_ptr->gold -= cost;

	broadcast_rom(fd, ply_ptr->rom_num, "%M has the smithy repair %1i.",
		      ply_ptr, obj_ptr);

	broke = mrand(1,100) + bonus[ply_ptr->piety];
	if((broke <= 15 && obj_ptr->shotscur < 1) ||
	   (broke <= 5 && obj_ptr->shotscur > 0)) {
		print(fd, "\"Darnitall!\" shouts the smithy, \"I broke another. Sorry %s.\"\n", F_ISSET(ply_ptr, PMALES) ? "lad":"lass");
		broadcast_rom(fd, ply_ptr->rom_num, "Oops!  He broke it.");
		if(obj_ptr->shotscur > 0) {
			print(fd, "He gives your money back.\n");
			ply_ptr->gold += cost;
		}
		del_obj_crt(obj_ptr, ply_ptr);
		free_obj(obj_ptr);
		return(0);
	}

	if((F_ISSET(obj_ptr, OENCHA) || obj_ptr->adjustment) &&
		mrand(1,50) > ply_ptr->piety) {
		print(fd, "\"It lost that nice glow,\" says the smithy.\n");
		if(obj_ptr->type == ARMOR && obj_ptr->wearflag == BODY)
			obj_ptr->armor =
				MAX(obj_ptr->armor - obj_ptr->adjustment*2, 0);
		else if(obj_ptr->type == ARMOR)
			obj_ptr->armor =
				MAX(obj_ptr->armor - obj_ptr->adjustment, 0);
		else if(obj_ptr->type <= MISSILE) {
			obj_ptr->shotsmax -= obj_ptr->adjustment*10;
			obj_ptr->pdice =
				MAX(obj_ptr->pdice - obj_ptr->adjustment, 0);
		}
		obj_ptr->adjustment = 0;
		F_CLR(obj_ptr, OENCHA);
	}

	obj_ptr->shotscur = (obj_ptr->shotsmax * mrand(5,9)) / 10;

	print(fd, "The smithy hands %i back to you, almost good as new.\n", 
		obj_ptr);

	return(0);

}

/**********************************************************************/
/*				prt_time			      */
/**********************************************************************/

/* This function outputs the time of day (realtime) to the player.    */

int prt_time(ply_ptr)
creature	*ply_ptr;
{
	char	*str;
	long	t;
	int	fd, daytime;

	fd = ply_ptr->fd;

	daytime = (int)(Time % 24L);
	print(fd, "Game-Time: %d o'clock %s.\n", daytime%12 == 0 ? 12 :
	      daytime%12, daytime > 11 ? "PM":"AM");

	t = time(0);
	str = ctime(&t);
	str[strlen(str)-1] = 0;
	
	print(fd, "Real-Time: %s (PST).\n", str);

	return(0);

}

/**********************************************************************/
/*				circle				      */
/**********************************************************************/

/* This function allows fighters and barbarians to run circles about an */
/* enemy, confusing it for several seconds.				*/

int circle(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	creature	*crt_ptr;
	room		*rom_ptr;
	long		i, t;
	int		chance, delay, fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(cmnd->num < 2) {
		print(fd, "Circle whom?\n");
		return(0);
	}

	if(ply_ptr->class != FIGHTER && ply_ptr->class != BARBARIAN &&
	   ply_ptr->class < CARETAKER) {
		print(fd, "Only barbarians and fighters may circle.\n");
		return(0);
	}

	crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
			   cmnd->str[1], cmnd->val[1]);

	if(!crt_ptr) {
		cmnd->str[1][0] = up(cmnd->str[1][0]);
		crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
				   cmnd->str[1], cmnd->val[1]);

		if(!crt_ptr || crt_ptr == ply_ptr || strlen(cmnd->str[1]) < 3) {
			print(fd, "That is not here.\n");
			return(0);
		}
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
            }
	if(is_charm_crt(ply_ptr->name, crt_ptr) && F_ISSET(crt_ptr, PCHARM)) {
                print(fd, "You are too fond of %s to do that.\n", crt_ptr->name);
                return(0);
            }

	}

	i = LT(ply_ptr, LT_ATTCK);
	t = time(0);

	if(i > t) {
		please_wait(fd, i-t);
		return(0);
	}

	F_CLR(ply_ptr, PHIDDN);
	if(F_ISSET(ply_ptr, PINVIS)) {
		F_CLR(ply_ptr, PINVIS);
		print(fd, "Your invisibility fades.\n");
		broadcast_rom(fd, ply_ptr->rom_num, "%M fades into view.",
			      ply_ptr);
	}

	chance = 50 + (ply_ptr->level-crt_ptr->level)*10 +
		 (bonus[ply_ptr->dexterity] -bonus[crt_ptr->dexterity])*2;
	if (crt_ptr->type == MONSTER && F_ISSET(crt_ptr, MUNDED))
		chance -= (5 + crt_ptr->level*2);
	chance = MIN(80, chance);
	if(F_ISSET(crt_ptr, MNOCIR) || F_ISSET(ply_ptr, PBLIND))
		chance=1;
		
	if(crt_ptr->type != PLAYER) {
		if(F_ISSET(crt_ptr, MUNKIL)) {
			print(fd, "You cannot harm %s.\n",
				F_ISSET(crt_ptr, MMALES) ? "him":"her");
			return(0);
		}
		add_enm_crt(ply_ptr->name, crt_ptr);
	}

	if(mrand(1,100) <= chance) {
		if(ply_ptr->class == BARBARIAN)
			delay = mrand(6,9);
		else
			delay = mrand(6,12);
		crt_ptr->lasttime[LT_ATTCK].ltime = t;
		crt_ptr->lasttime[LT_ATTCK].interval = delay;

		print(fd, "You circle %m.\n", crt_ptr);

		print(crt_ptr->fd, "%M circles you.\n", ply_ptr);
		broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
			       "%M circles %m.", ply_ptr, crt_ptr);
		ply_ptr->lasttime[LT_ATTCK].interval = 2;
	}
	else {
		print(fd, "You failed to circle it.\n");
		print(crt_ptr->fd, "%M tried to circle you.\n", ply_ptr);
		broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
			       "%M tried to circle %m.", ply_ptr, crt_ptr);
		ply_ptr->lasttime[LT_ATTCK].interval = 3;
	}

	ply_ptr->lasttime[LT_ATTCK].ltime = t;

	return(0);

}

/**********************************************************************/
/*				bash				      */
/**********************************************************************/

/* This function allows fighters and barbarians to "bash" an opponent, */
/* doing less damage than a normal attack, but knocking the opponent   */
/* over for a few seconds, leaving him unable to attack back.	       */

int bash(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	creature	*crt_ptr;
	room		*rom_ptr;
	long		i, t;
	int		m, n, chance, fd, p, addprof;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(cmnd->num < 2) {
		print(fd, "Bash whom?\n");
		return(0);
	}

	if(ply_ptr->class != FIGHTER && ply_ptr->class != BARBARIAN &&
	   ply_ptr->class < CARETAKER) {
		print(fd, "Only barbarians and fighters may bash.\n");
		return(0);
	}

	crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
			   cmnd->str[1], cmnd->val[1]);

	if(!crt_ptr) {
		cmnd->str[1][0] = up(cmnd->str[1][0]);
		crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
				   cmnd->str[1], cmnd->val[1]);

		if(!crt_ptr || crt_ptr == ply_ptr || strlen(cmnd->str[1])<3) {
			print(fd, "That isn't here.\n");
			return(0);
		}

	}

	if(crt_ptr->type != PLAYER && is_enm_crt(ply_ptr->name, crt_ptr)) {
		print(fd, "Not while you're already fighting %s.\n",
		      F_ISSET(crt_ptr, MMALES) ? "him":"her");
		return(0);
	}
	else if(crt_ptr->type == PLAYER) {
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
            }
	if(is_charm_crt(ply_ptr->name, crt_ptr)&& F_ISSET(crt_ptr, PCHARM)) {
                print(fd, "You like %s too much to do that.\n", crt_ptr->name);
                return(0);
            }

	}

	i = LT(ply_ptr, LT_ATTCK);
	t = time(0);

	if(i > t) {
		please_wait(fd, i-t);
		return(0);
	}

	ply_ptr->lasttime[LT_ATTCK].ltime = t;
	ply_ptr->lasttime[LT_ATTCK].interval = 3;

	F_CLR(ply_ptr, PHIDDN);
	if(F_ISSET(ply_ptr, PINVIS)) {
		F_CLR(ply_ptr, PINVIS);
		print(fd, "Your invisibility fades.\n");
		broadcast_rom(fd, ply_ptr->rom_num, "%M fades into view.",
			      ply_ptr);
	}

	if(crt_ptr->type != PLAYER) {
		if(F_ISSET(crt_ptr, MUNKIL)) {
			print(fd, "You cannot harm %s.\n",
				F_ISSET(crt_ptr, MMALES) ? "him":"her");
			return(0);
		}
 if(F_ISSET(crt_ptr, MMGONL)) {
            print(fd, "Your weapon has no effect on %m.\n",
                crt_ptr);
            return(0);
        }
        if(F_ISSET(crt_ptr, MENONL)) {
            if(!ply_ptr->ready[WIELD-1] || 
                ply_ptr->ready[WIELD-1]->adjustment < 1) {
                print(fd, "Your weapon has no effect on %m.\n",
                    crt_ptr);
                return(0);
            }
        }  
		add_enm_crt(ply_ptr->name, crt_ptr);
	}

	chance = 50 + (ply_ptr->level-crt_ptr->level)*10 +
		bonus[ply_ptr->strength]*3 +
		 (bonus[ply_ptr->dexterity] -bonus[crt_ptr->dexterity])*2;
	chance += ply_ptr->class==BARBARIAN ? 5:0;
	chance = MIN(85, chance);
	if(F_ISSET(ply_ptr, PBLIND))
		chance = MIN(20, chance);

	if(mrand(1,100) <= chance) {

		if(ply_ptr->ready[WIELD-1]) {
			if(ply_ptr->ready[WIELD-1]->shotscur < 1) {
				print(fd, "Your %s is broken.\n", 
				      ply_ptr->ready[WIELD-1]->name);
				add_obj_crt(ply_ptr->ready[WIELD-1], ply_ptr);
				ply_ptr->ready[WIELD-1] = 0;
				return(0);
			}
		}

		n = ply_ptr->thaco - crt_ptr->armor/10;
		if(mrand(1,20) >= n) {

			crt_ptr->lasttime[LT_ATTCK].ltime = t;
			crt_ptr->lasttime[LT_ATTCK].interval = mrand(5,8);

			if(ply_ptr->ready[WIELD-1])
				n = mdice(ply_ptr->ready[WIELD-1]) / 2;
			else
				n = mdice(ply_ptr) / 2;

			if(ply_ptr->class == BARBARIAN && mrand(1,100) > 75)
				n = (n*3)/2;

			m = MIN(crt_ptr->hpcur, n);
			if(crt_ptr->type != PLAYER) {
				add_enm_dmg(ply_ptr->name, crt_ptr, m);
				if(ply_ptr->ready[WIELD-1]) {
					p = MIN(ply_ptr->ready[WIELD-1]->type,
						4);
					addprof = (m * crt_ptr->experience) /
						crt_ptr->hpmax;
					addprof = MIN(addprof,
						crt_ptr->experience);
					ply_ptr->proficiency[p] += addprof;
				}
			}
			crt_ptr->hpcur -= n;

			print(fd, "You bash for %d damage.\n", n);
			broadcast_rom2(fd, crt_ptr->fd, crt_ptr->rom_num, 
				       "%M bashes %m.", ply_ptr, crt_ptr);
			print(crt_ptr->fd, "%M bashed you for %d damage.\n",
			      ply_ptr, n);

			ply_ptr->lasttime[LT_ATTCK].interval = 2;

			if(crt_ptr->hpcur < 1) {
				print(fd, "You killed %m.\n", crt_ptr);
				broadcast_rom2(fd, crt_ptr->fd, 
					       ply_ptr->rom_num,
					       "%M killed %m.", ply_ptr,
					       crt_ptr);

				die(crt_ptr, ply_ptr);
			}
			else
				check_for_flee(crt_ptr);
		}
		else {
			print(fd, "Your bash failed.\n");
			print(crt_ptr->fd, "%M tried to bash you.\n", ply_ptr);
			broadcast_rom2(fd, crt_ptr->fd, crt_ptr->rom_num, 
				       "%M tried to bash %m.", ply_ptr, 
				       crt_ptr);
		}
	}

	else {
		print(fd, "Your bash failed.\n");
		print(crt_ptr->fd, "%M tried to bash you.\n", ply_ptr);
		broadcast_rom2(fd, crt_ptr->fd, crt_ptr->rom_num, 
			       "%M tried to bash %m.", ply_ptr, crt_ptr);
	}

	return(0);

}

/**********************************************************************/
/*				savegame			      */
/**********************************************************************/

/* This function saves a player's game.  Since items need to be un-readied */
/* before a player can be saved to a file, this function makes a duplicate */
/* of the player, unreadies everything on the duplicate, and then saves    */
/* the duplicate to the file.  Afterwards, the duplicate is freed from     */
/* memory.								   */

int savegame(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	creature	*dum_ptr;
	object		*obj[MAXWEAR];
	int		i, n = 0;

	dum_ptr = (creature *)malloc(sizeof(creature));
	if(!dum_ptr)
		merror("savegame", FATAL);

	*dum_ptr = *ply_ptr;

	for(i=0; i<MAXWEAR; i++) {
		if(dum_ptr->ready[i]) {
			obj[n++] = dum_ptr->ready[i];
			add_obj_crt(dum_ptr->ready[i], dum_ptr);
			dum_ptr->ready[i] = 0;
		}
	}

	if(!dum_ptr->name[0]) return;

	if(save_ply(dum_ptr->name, dum_ptr) < 0)
		merror("savegame", NONFATAL);

	for(i=0; i<n; i++)
		del_obj_crt(obj[i], dum_ptr);

	free(dum_ptr);

	print(ply_ptr->fd, "Player saved.\n");

	return(0);

}

/**********************************************************************/
/*				talk				      */
/**********************************************************************/

/* This function allows players to speak with monsters if the monster */
/* has a talk string set.					      */

int talk(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	room		*rom_ptr;
	creature	*crt_ptr;
	ttag		*tp;
	char		str[160];
	int		fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(cmnd->num < 2) {
		print(fd, "Talk to whom?\n");
		return(0);
	}

	crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
			   cmnd->str[1], cmnd->val[1]);

	if(!crt_ptr) {
		print(fd, "I don't see that here.\n");
		return(0);
	}

	F_CLR(ply_ptr, PHIDDN);

	if(cmnd->num == 2 || !F_ISSET(crt_ptr, MTALKS)) {
		broadcast_rom(fd, ply_ptr->rom_num, "%M talks with %m.", 
			ply_ptr, crt_ptr);

		if(crt_ptr->talk[0]) {
			broadcast_rom(fd, ply_ptr->rom_num,
				"%M says to %M, \"%s\".", crt_ptr, ply_ptr,
				crt_ptr->talk);
			print(fd, "%M says to you, \"%s\".\n", crt_ptr,
				crt_ptr->talk);
		}
		else
			broadcast_rom(-1, ply_ptr->rom_num,
				"%M doesn't say anything.", crt_ptr);
		if(F_ISSET(crt_ptr, MTLKAG))
			add_enm_crt(ply_ptr->name, crt_ptr);
	}

	else {
		if(!crt_ptr->first_tlk)
			if(!load_crt_tlk(crt_ptr))
				return(PROMPT);

		broadcast_rom(fd, ply_ptr->rom_num, "%M asks %m about \"%s\".",
			ply_ptr, crt_ptr, cmnd->str[2]);

		tp = crt_ptr->first_tlk;
		while(tp) {
			if(!strcmp(cmnd->str[2], tp->key)) {
				broadcast_rom(fd, ply_ptr->rom_num,
					"%M says to %M, \"%s\".", crt_ptr,
					ply_ptr, tp->response);
				print(fd, "%M says to you, \"%s\"\n",
					crt_ptr, tp->response);
				talk_action(ply_ptr, crt_ptr, tp);
				return(0);
			}
			tp = tp->next_tag;
		}
		broadcast_rom(-1, ply_ptr->rom_num, "%M shrugs.", crt_ptr);
		if(F_ISSET(crt_ptr, MTLKAG))
			add_enm_crt(ply_ptr->name, crt_ptr);
	}

	return(0);

}

/******************************************************************/
/*                      talk_action                               */
/******************************************************************/
/* The talk_action function handles a monster's actin when a     *
 * player asks the monster about a key word.  The format for the *
 * is defined in the monster's talk file.  Currently a monster   *
 * can attack, or cast spells on the player who mentions the key *
 * word or preform any of the defined social commands */

void talk_action(ply_ptr, crt_ptr,tt)
creature	*ply_ptr;
creature	*crt_ptr;
ttag		*tt;
{
cmd		cm;
int 	i, n,splno =0;
object  *obj_ptr;
int		(*fn)();

for (i = 0; i < COMMANDMAX;i++){
	cm.str[i][0] = 0;
	cm.str[i][24] = 0;
	cm.val[i]	= 0;
}
	cm.fullstr[0] = 0;
	cm.num = 0;

switch(tt->type){
	case 1: 	/*attack */
		add_enm_crt(ply_ptr->name, crt_ptr);
		broadcast_rom(ply_ptr->fd, ply_ptr->rom_num,
					"%M attacks %M\n",crt_ptr,ply_ptr);
		print(ply_ptr->fd,"%M attacks you.\n",crt_ptr);
		break;

	case 2:				/*action command */
		if(action){
			strncpy(cm.str[0],tt->action,25);
			strcat(cm.fullstr,tt->action);
			cm.val[0]  = 1;
			cm.num = 1;

			if(tt->target)
				if(!strcmp(tt->target,"PLAYER")){
					strcpy(cm.str[1],ply_ptr->name);
					strcat(cm.fullstr," ");
					strcat(cm.fullstr,ply_ptr->name);
					cm.val[1]  = 1;
					cm.num = 2;
				}
			action(crt_ptr,&cm);
		}
		break;
	case 3:			/*cast */
		if(tt->action){
			n =  crt_ptr->mpcur; 
			strcpy(cm.str[0],"cast");
			strncpy(cm.str[1],tt->action,25);
			strcpy(cm.str[2],ply_ptr->name);
			cm.val[0]  = cm.val[1]  = cm.val[2]  = 1;
			cm.num = 3;
			sprintf(cm.fullstr,"cast %s %s",tt->action,ply_ptr->name);

		    i = 0;
			do {
		   		if(!strcmp(tt->action, spllist[i].splstr)) {
           			splno = i;
           	 		break;
        		}
       	 		i++;
    		} while(spllist[i].splno != -1);

			if(spllist[i].splno == -1)
				return;
		 	fn = spllist[splno].splfn;
			if(fn == offensive_spell) {
				for(i=0; ospell[i].splno != spllist[splno].splno; i++)
					if(ospell[i].splno == -1) return;
				 (*fn)(crt_ptr, &cm, CAST, spllist[splno].splstr,
            &ospell[i]);
    		}
			else if(is_enm_crt(ply_ptr->name,crt_ptr)){
				print(ply_ptr->fd,"%M refuses to cast any spells on you.\n",
					crt_ptr);
				return;
			}	
			else
			  (*fn)(crt_ptr, &cm, CAST);
			
			if (spllist[i].splno  != SRESTO && crt_ptr->mpcur == n)
				print(ply_ptr->fd,
			"%M appoligies that %s can not currently cast that spell on you.\n",
			crt_ptr, (F_ISSET(crt_ptr,MMALES)) ? "he" : "she");
		}
		break;
	case 4:
		i = atoi(tt->action);
		if (i > 0){
			n=load_obj(i, &obj_ptr);
            if(n > -1) {
                if(F_ISSET(obj_ptr, ORENCH))
                    rand_enchant(obj_ptr);
 
                if(weight_ply(ply_ptr) + weight_obj(obj_ptr) > 
                   max_weight(ply_ptr)) {
                   print(ply_ptr->fd, "You can't hold anymore.\n");
				   break;
               }    

	            if(obj_ptr->questnum && Q_ISSET(ply_ptr, obj_ptr->questnum-1)) {
                    print(ply_ptr->fd, "You may not get that. %s.\n",
                          "You have already fulfilled that quest");
					break;
                } 
                
	     		if(obj_ptr->questnum) {
   	                 print(ply_ptr->fd, "Quest fulfilled!  Don't drop it.\n");
   	                 Q_SET(ply_ptr, obj_ptr->questnum-1);
   	                 ply_ptr->experience += quest_exp[obj_ptr->questnum-1];
   	                 print(ply_ptr->fd, "%ld experience granted.\n",
                            quest_exp[obj_ptr->questnum-1]);
   	                 add_prof(ply_ptr,quest_exp[obj_ptr->questnum-1]);
   	             	} 
                add_obj_crt(obj_ptr, ply_ptr);
				print(ply_ptr->fd,"%M gives you %i\n",crt_ptr, obj_ptr);
				broadcast_rom(ply_ptr->fd, ply_ptr->rom_num,
					"%M gives %m %i\n", crt_ptr,ply_ptr,obj_ptr);
			}
			else
				print(ply_ptr->fd,"%M has nothing to give you.\n",crt_ptr);
        }  
		break;
	default:
		break;
	}
return;
}
