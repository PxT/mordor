/*
 * COMMAND9.C:
 *
 *	Additional user routines.
 *
 *	Copyright (C) 1991, 1992, 1993 Brooke Paul
 *
 * $Id: command9.c,v 6.14 2001/06/23 06:55:29 develop Exp $
 *
 * $Log: command9.c,v $
 * Revision 6.14  2001/06/23 06:55:29  develop
 * clerics cannot pray during battle
 *
 * Revision 6.13  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */
#include <stdlib.h>

#include "../include/mordb.h"
#include "mextern.h"



/**********************************************************************/
/*				bribe				      */
/**********************************************************************/

/* This function allows players to bribe monsters.  If they give the  */
/* monster enough money, it will leave the room.  If not, the monster */
/* keeps the money and stays.					      */

int bribe(creature *ply_ptr, cmd *cmnd )
{
	creature	*crt_ptr;
	room		*rom_ptr;
	long		amount;
	int		fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(cmnd->num < 2) {
		output(fd, "Bribe whom?\n");
		return(0);
	}

	if(cmnd->num < 3 || cmnd->str[2][0] != '$') {
		output(fd, "Syntax: bribe <monster> $<amount>\n");
		return(0);
	}

	crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[1],
			   cmnd->val[1]);

	if(!crt_ptr) {
		output(fd, "That is not here.\n");
		return(0);
	}

	amount = atol(&cmnd->str[2][1]);
	if(amount < 1 || amount > ply_ptr->gold) {
		output(fd, "Get real.\n");
		return(0);
	}

	if(amount < crt_ptr->level*895L || F_ISSET(crt_ptr, MPERMT)) {
		mprint(fd, "%M takes your money, but stays.\n", m1arg(crt_ptr));
		broadcast_rom(fd, ply_ptr->rom_num, "%M tried to bribe %m.",
			      m2args(ply_ptr, crt_ptr));
		ply_ptr->gold -= amount;
		crt_ptr->gold += amount;
	}
	else {
		mprint(fd, "%M takes your money and leaves.\n", m1arg(crt_ptr));
		broadcast_rom(fd, ply_ptr->rom_num, "%M bribed %m.",
			      m2args(ply_ptr, crt_ptr));
		ply_ptr->gold -= amount;
		del_crt_rom(crt_ptr, rom_ptr);
		free_crt(crt_ptr);

	}

	return(0);

}

/**********************************************************************/
/*				haste				      */
/**********************************************************************/

/* This function allows rangers to hasten themselves every 10 minutes */
/* or so, giving themselves 5 extra dexterity points.  The haste will  */
/* last for 4 minutes plus 1 minute for every four levels.	      */

int haste( creature *ply_ptr, cmd *cmnd )
{
	time_t	i, t;
	int	chance, fd;

	fd = ply_ptr->fd;

	if(ply_ptr->class != RANGER && ply_ptr->class < BUILDER) {
		output(fd, "Only rangers may use haste.\n");
		return(0);
	}

	if(F_ISSET(ply_ptr, PHASTE)) {
		output(fd, "You're already hastened.\n");
		return(0);
	}

	i = ply_ptr->lasttime[LT_HASTE].ltime;
	t = time(0);

	if(t-i < 600L) {
		please_wait(fd, 600L-t+i );
		return(0);
	}

	chance = MIN(85, ply_ptr->level*20+bonus[(int)ply_ptr->dexterity]);

	if(mrand(1,100) <= chance) {
		output(fd, "You feel yourself moving faster.\n");
		broadcast_rom(fd, ply_ptr->rom_num, "%M hastens.", m1arg(ply_ptr));
		F_SET(ply_ptr, PHASTE);
		ply_ptr->dexterity += 5;
		ply_ptr->lasttime[LT_HASTE].ltime = t;
		ply_ptr->lasttime[LT_HASTE].interval = 240L +
			60L * (ply_ptr->level/5);

		compute_ac(ply_ptr);
	}
	else {
		output(fd, "Your attempt to hasten failed.\n");
		broadcast_rom(fd, ply_ptr->rom_num, "%M tried to hasten.",
			      m1arg(ply_ptr));
		ply_ptr->lasttime[LT_HASTE].ltime = t - 590L;
	}

	return(0);

}
/**********************************************************************/
/*				berserk				      */
/**********************************************************************/
/* This function allows barbarians to berserk themselves every 10 minutes */
/* or so, giving themselves 3 extra dex points and some hit points.  The berserk will */
/* last for 4 minutes plus  for every four levels.		      */ 

int berserk( creature *ply_ptr, cmd *cmnd )
{
	time_t	i, t;
	int	chance, fd;

	fd = ply_ptr->fd;

	if(ply_ptr->class != BARBARIAN && ply_ptr->class < BUILDER) {
		output(fd, "Only barbarians may berserk.\n");
		return(0);
	}

	if(F_ISSET(ply_ptr, PBESRK)) {
		output(fd, "You're already berserked.\n");
		return(0);
	}

	i = ply_ptr->lasttime[LT_BSRKN].ltime;
	t = time(0);

	if(t-i < 600L) {
		please_wait(fd, 600L-t+i );
		return(0); 
	}

	chance = MIN(85, ply_ptr->level*20+bonus[(int)ply_ptr->dexterity]);

	if(mrand(1,100) <= chance) {
		output(fd, "You work yourself into a berserker rage!\n");
		broadcast_rom(fd, ply_ptr->rom_num, "%M berserks.", m1arg(ply_ptr));
		F_SET(ply_ptr, PBESRK);
		ply_ptr->dexterity += 3;
		ply_ptr->hpcur += ply_ptr->level;
		ply_ptr->hpmax += ply_ptr->level;
		ply_ptr->lasttime[LT_BSRKN].ltime = t;
		ply_ptr->lasttime[LT_BSRKN].interval = 45L +
			15L * ((ply_ptr->level-1)/3);

		compute_ac(ply_ptr);
	}
	else {
		output(fd, "Your attempt to berserk failed.\n");
		broadcast_rom(fd, ply_ptr->rom_num, "%M tried to berserk.",
			      m1arg(ply_ptr));
		ply_ptr->lasttime[LT_BSRKN].ltime = t - 590L;
	}

	return(0);

}
/**********************************************************************/
/*				barkskin			      */
/**********************************************************************/

/* This function allows druids to increase their armor class every 10 minutes */
/* or so, giving themselves an additional 3.  The barkskin will */
/* last for 15 seconds plus 15 secs for every four levels.
*/

int barkskin( creature *ply_ptr, cmd *cmnd )
{
	time_t	i, t;
	int	chance, fd;

	fd = ply_ptr->fd;

	if(ply_ptr->class != DRUID && ply_ptr->class < BUILDER) {
		output(fd, "Only Druids may use barkskin.\n");
		return(0);
	}

	if(F_ISSET(ply_ptr, PBARKS)) {
		output(fd, "You already have barkskin.\n");
		return(0);
	}

	i = ply_ptr->lasttime[LT_BSRKN].ltime;
	t = time(0);

	if(t-i < 300L) {
		please_wait(fd, 300L-t+i );
		return(0);
	}

	chance = MIN(85, ply_ptr->level*20+bonus[(int)ply_ptr->dexterity]);

	if(mrand(1,100) <= chance) {
		output(fd, "Sylvan energy flows through you as your skin hardens into bark.\n");
		broadcast_rom(fd, ply_ptr->rom_num, "%M barkskins.", m1arg(ply_ptr));
		F_SET(ply_ptr, PBARKS);
                ply_ptr->lasttime[LT_BSRKN].ltime = t;
                ply_ptr->lasttime[LT_BSRKN].interval = 45L +
                        15L * ((ply_ptr->level-1)/3);
		compute_ac(ply_ptr);
	}
	else {
		output(fd, "Your attempt to barkskin failed.\n");
		broadcast_rom(fd, ply_ptr->rom_num, "%M tried to barkskin.",
			      m1arg(ply_ptr));
		ply_ptr->lasttime[LT_BSRKN].ltime = t - 590L;
	}

	return(0);

}

/**********************************************************************/
/*				pray				      */
/**********************************************************************/

/* This function allows clerics and paladins to pray every 10 minutes */
/* to increase their piety for a duration of 5 minutes.		      */

int pray( creature *ply_ptr, cmd *cmnd )
{
	time_t	i, t;
	int	chance, fd;

	fd = ply_ptr->fd;

	if(ply_ptr->class != CLERIC && ply_ptr->class != PALADIN && 
	   ply_ptr->class < BUILDER) {
		output(fd, "Only clerics and paladins may pray.\n");
		return(0);
	}

	if(F_ISSET(ply_ptr, PPRAYD)) {
		output(fd, "You've already prayed.\n");
		return(0);
	}

	i = ply_ptr->lasttime[LT_PRAYD].ltime;
	t = time(0);

	if(t-i < 600L) {
		please_wait(fd, 600L-t+i );
		return(0);
	}

	if(LT(ply_ptr,LT_ATTCK) > t) {
	        output(ply_ptr->fd,"You cannot pray while fighting!\n");
                ply_ptr->lasttime[LT_PRAYD].ltime = t;
                ply_ptr->lasttime[LT_ATTCK].ltime = t;
                ply_ptr->lasttime[LT_PRAYD].interval = 120L;
                return(0);
        }

	chance = MIN(85, ply_ptr->level*20+bonus[(int)ply_ptr->piety]);

	if(mrand(1,100) <= chance) {
		output(fd, "You feel extremely pious.\n");
		broadcast_rom(fd, ply_ptr->rom_num, "%M prays.", m1arg(ply_ptr));
		F_SET(ply_ptr, PPRAYD);
		ply_ptr->piety += 5;
		ply_ptr->lasttime[LT_PRAYD].ltime = t;
		ply_ptr->lasttime[LT_PRAYD].interval = 300L;
	}
	else {
		output(fd, "Your prayers were not answered.\n");
		broadcast_rom(fd, ply_ptr->rom_num, "%M prays.",
			      m1arg(ply_ptr));
		ply_ptr->lasttime[LT_PRAYD].ltime = t - 590L;
	}

	return(0);

}

/**********************************************************************/
/*				prepare				      */
/**********************************************************************/

/* This function allows players to prepare themselves for traps that */
/* might be in the next room they enter.  This way, they can hope to */
/* avoid a trap that they already know is there.		     */

int prepare( creature *ply_ptr, cmd	*cmnd )
{
	time_t	i, t;
	int	fd;

	fd = ply_ptr->fd;

	if(F_ISSET(ply_ptr, PPREPA)) {
		output(fd, "You've already prepared.\n");
		return(0);
	}

	t = time(0);
	i = LT(ply_ptr, LT_PREPA);

	if(t < i) {
		please_wait(fd, i-t);
		return(0);
	}

	ply_ptr->lasttime[LT_PREPA].ltime = t;
	ply_ptr->lasttime[LT_PREPA].interval = ply_ptr->class == DM ? 0:15;
	
	output(fd, "You prepare yourself for traps.\n");
	broadcast_rom(fd, ply_ptr->rom_num, "%M prepares for traps.",
		m1arg(ply_ptr));
	F_SET(ply_ptr, PPREPA);
	if(F_ISSET(ply_ptr, PBLIND))
		F_CLR(ply_ptr, PPREPA);

	return(0);
}

/************************************************************************/
/*				use					*/
/************************************************************************/

/* This function allows a player to use an item without specifying the	*/
/* special command for its type.  Use determines which type of item	*/
/* it is, and calls the appropriate functions.				*/

int use( creature *ply_ptr, cmd *cmnd )
{
	object	*obj_ptr;
	room	*rom_ptr;
	int		fd, rtn, onfloor=0;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(cmnd->num < 2) {
		output(fd, "Use what?\n");
		return(0);
	}

	if(!strcmp(cmnd->str[1], "all"))
		return(wear(ply_ptr, cmnd));

	obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj, cmnd->str[1],
			   cmnd->val[1]);
	if(!obj_ptr) {
		obj_ptr = find_obj(ply_ptr, rom_ptr->first_obj, cmnd->str[1],
				   cmnd->val[1]);
		if(!obj_ptr || !F_ISSET(obj_ptr, OUSEFL)) {
			output(fd, "Use what?\n");
			return(0);
		}
		else if(F_ISSET(obj_ptr, OUSEFL)) {
			onfloor = 1;
			cmnd->num = 2;
		}
	}

	F_CLR(ply_ptr, PHIDDN);

	switch(obj_ptr->type) {
	case SHARP:
	case THRUST:
	case BLUNT:
	case POLE:
	case MISSILE:
		rtn = ready(ply_ptr, cmnd); break;
	case ARMOR:
		rtn = wear(ply_ptr, cmnd); break;
	case POTION:
		rtn = drink(ply_ptr, cmnd); break;
	case SCROLL:
		rtn = readscroll(ply_ptr, cmnd); break;
	case WAND:
		if(onfloor) {
			cmnd->num = 2;
			obj_ptr->shotscur -= 1; 
			rtn = zap_obj(ply_ptr, obj_ptr, cmnd);
		}
		else
			rtn = zap(ply_ptr, cmnd); break;
	case KEY:
		rtn = unlock(ply_ptr, cmnd); break;
	case LIGHTSOURCE:
		rtn = hold(ply_ptr, cmnd); break;
	default:
		output(fd, "How does one use that?\n");
		rtn = 0;
	}
	return(rtn);
}

/************************************************************************/
/*				ignore					*/
/************************************************************************/

/* This function allows players to ignore the flashes of other players. */
/* If used without an argument, it lists the names of players currently */
/* being ignored.  If included with a name, the player will be added	*/
/* to the ignore list if he is not already on it.  Otherwise the player */
/* will be taken off the list.  The ignore list lasts only for the	*/
/* duration of the player's connection.					*/

int ignore( creature *ply_ptr, cmd *cmnd )
{
	int		n, fd;
	etag		*ign, *prev;
	char		outstr[1024], *name;
	creature	*crt_ptr;

	fd = ply_ptr->fd;

	if(cmnd->num == 1) {
		strcpy(outstr, "You are ignoring: ");
		ign = Ply[fd].extr->first_ignore;
		n = 0;
		while(ign) {
			strcat(outstr, ign->enemy);
			strcat(outstr, ", ");
			n = 1;
			ign = ign->next_tag;
		}
		if(!n)
			strcat(outstr, "No one.");
		else {
			outstr[strlen(outstr)-2] = '.';
			outstr[strlen(outstr)-1] = 0;
		}
		output_nl(fd, outstr);
		return(0);
	}

	cmnd->str[1][0] = up(cmnd->str[1][0]);
	name = cmnd->str[1];
	n = 0;
	ign = Ply[fd].extr->first_ignore;
	if(!ign) {}
	else if(!strcmp(ign->enemy, name)) {
		Ply[fd].extr->first_ignore = ign->next_tag;
		free(ign);
		n = 1;
	}
	else {
		prev = ign;
		ign = ign->next_tag;
		while(ign) {
			if(!strcmp(ign->enemy, name)) {
				prev->next_tag = ign->next_tag;
				free(ign);
				n = 1;
				break;
			}
			prev = ign;
			ign = ign->next_tag;
		}
	}

	if(n) {
		sprintf(g_buffer, "%s removed from your ignore list.\n", name);
		output(fd, g_buffer );
		return(0);
	}

	crt_ptr = find_who(name);

	if(crt_ptr && !F_ISSET(crt_ptr, PDMINV)) {
		ign = (etag *)malloc(sizeof(etag));
		strcpy(ign->enemy, name);
		ign->next_tag = Ply[fd].extr->first_ignore;
		Ply[fd].extr->first_ignore = ign;
		sprintf(g_buffer, "%s added to your ignore list.\n", name);
		output(fd, g_buffer );
	}
	else
		output(fd, "That player is not on.\n");

	return(0);
}


