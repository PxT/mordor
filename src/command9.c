/*
 * COMMAND9.C:
 *
 *	Additional user routines.
 *
 *	Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"
#ifdef DMALLOC
  #include "/usr/local/include/dmalloc.h"
#endif

/**********************************************************************/
/*				bribe				      */
/**********************************************************************/

/* This function allows players to bribe monsters.  If they give the  */
/* monster enough money, it will leave the room.  If not, the monster */
/* keeps the money and stays.					      */

int bribe(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	creature	*crt_ptr;
	room		*rom_ptr;
	long		amount;
	int		fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(cmnd->num < 2) {
		print(fd, "Bribe whom?\n");
		return(0);
	}

	if(cmnd->num < 3 || cmnd->str[2][0] != '$') {
		print(fd, "Syntax: bribe <monster> $<amount>\n");
		return(0);
	}

	crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[1],
			   cmnd->val[1]);

	if(!crt_ptr) {
		print(fd, "That is not here.\n");
		return(0);
	}

	amount = atol(&cmnd->str[2][1]);
	if(amount < 1 || amount > ply_ptr->gold) {
		print(fd, "Get real.\n");
		return(0);
	}

	if(amount < crt_ptr->level*895L || F_ISSET(crt_ptr, MPERMT)) {
		print(fd, "%M takes your money, but stays.\n", crt_ptr);
		broadcast_rom(fd, ply_ptr->rom_num, "%M tried to bribe %m.",
			      ply_ptr, crt_ptr);
		ply_ptr->gold -= amount;
		crt_ptr->gold += amount;
	}
	else {
		print(fd, "%M takes your money and leaves.\n", crt_ptr);
		broadcast_rom(fd, ply_ptr->rom_num, "%M bribed %m.",
			      ply_ptr, crt_ptr);
		ply_ptr->gold -= amount;
		del_crt_rom(crt_ptr, rom_ptr);
		del_active(crt_ptr);
		free_crt(crt_ptr);
	}

	return(0);

}

/**********************************************************************/
/*				haste				      */
/**********************************************************************/

/* This function allows rangers to hasten themselves every 10 minutes */
/* or so, giving themselves 5 extra dexterity points.  The haste will */
/* last for 2 minutes.						      */

int haste(ply_ptr, cmnd)
creature 	*ply_ptr;
cmd		*cmnd;
{
	long	i, t;
	int	chance, fd;

	fd = ply_ptr->fd;

	if(ply_ptr->class != RANGER && ply_ptr->class < CARETAKER) {
		print(fd, "Only rangers may use haste.\n");
		return(0);
	}

	if(F_ISSET(ply_ptr, PHASTE)) {
		print(fd, "You're already hastened.\n");
		return(0);
	}

	i = ply_ptr->lasttime[LT_HASTE].ltime;
	t = time(0);

	if(t-i < 600L) {
		print(fd, "Please wait %d:%02d more minutes.\n", 
		      (600L-t+i)/60L, (600L-t+i)%60L);
		return(0);
	}

	chance = MIN(85, ply_ptr->level*20+bonus[ply_ptr->dexterity]);

	if(mrand(1,100) <= chance) {
		print(fd, "You feel yourself moving faster.\n");
		broadcast_rom(fd, ply_ptr->rom_num, "%M hastens.", ply_ptr);
		F_SET(ply_ptr, PHASTE);
		ply_ptr->dexterity += 5;
		ply_ptr->lasttime[LT_HASTE].ltime = t;
		ply_ptr->lasttime[LT_HASTE].interval = 120L +
			60L * (ply_ptr->level/5);
	}
	else {
		print(fd, "Your attempt to hasten failed.\n");
		broadcast_rom(fd, ply_ptr->rom_num, "%M tried to hasten.",
			      ply_ptr);
		ply_ptr->lasttime[LT_HASTE].ltime = t - 590L;
	}

	return(0);

}

/**********************************************************************/
/*				pray				      */
/**********************************************************************/

/* This function allows clerics and paladins to pray every 10 minutes */
/* to increase their piety for a duration of 5 minutes.		      */

int pray(ply_ptr, cmnd)
creature 	*ply_ptr;
cmd		*cmnd;
{
	long	i, t;
	int	chance, fd;

	fd = ply_ptr->fd;

	if(ply_ptr->class != CLERIC && ply_ptr->class != PALADIN && 
	   ply_ptr->class < CARETAKER) {
		print(fd, "Only clerics and paladins may pray.\n");
		return(0);
	}

	if(F_ISSET(ply_ptr, PPRAYD)) {
		print(fd, "You've already prayed.\n");
		return(0);
	}

	i = ply_ptr->lasttime[LT_PRAYD].ltime;
	t = time(0);

	if(t-i < 600L) {
		print(fd, "Please wait %d:%02d more minutes.\n", 
		      (600L-t+i)/60L, (600L-t+i)%60L);
		return(0);
	}

	chance = MIN(85, ply_ptr->level*20+bonus[ply_ptr->piety]);

	if(mrand(1,100) <= chance) {
		print(fd, "You feel extremely pious.\n");
		broadcast_rom(fd, ply_ptr->rom_num, "%M prays.", ply_ptr);
		F_SET(ply_ptr, PPRAYD);
		ply_ptr->piety += 5;
		ply_ptr->lasttime[LT_PRAYD].ltime = t;
		ply_ptr->lasttime[LT_PRAYD].interval = 300L;
	}
	else {
		print(fd, "Your prayers were not answered.\n");
		broadcast_rom(fd, ply_ptr->rom_num, "%M prays.",
			      ply_ptr);
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

int prepare(ply_ptr, cmnd)
creature 	*ply_ptr;
cmd		*cmnd;
{
	long	i, t;
	int	fd;

	fd = ply_ptr->fd;

	if(F_ISSET(ply_ptr, PPREPA)) {
		print(fd, "You've already prepared.\n");
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
	
	print(fd, "You prepare yourself for traps.\n");
	broadcast_rom(fd, ply_ptr->rom_num, "%M prepares for traps.",
		ply_ptr);
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

int use(ply_ptr, cmnd)
creature 	*ply_ptr;
cmd		*cmnd;
{
	object	*obj_ptr;
	room	*rom_ptr;
	int	fd, n, rtn, onfloor=0;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(cmnd->num < 2) {
		print(fd, "Use what?\n");
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
			print(fd, "Use what?\n");
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
		print(fd, "How does one use that?\n");
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

int ignore(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
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
		print(fd, "%s\n", outstr);
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
		print(fd, "%s removed from your ignore list.\n", name);
		return(0);
	}

	crt_ptr = find_who(name);

	if(crt_ptr && !F_ISSET(crt_ptr, PDMINV)) {
		ign = (etag *)malloc(sizeof(etag));
		strcpy(ign->enemy, name);
		ign->next_tag = Ply[fd].extr->first_ignore;
		Ply[fd].extr->first_ignore = ign;
		print(fd, "%s added to your ignore list.\n", name);
	}
	else
		print(fd, "That player is not on.\n");

	return(0);
}


