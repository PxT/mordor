/*
 * ROOM.C:
 *
 *	Room routines.
 *
 *	Copyright (C) 1991, 1992, 1993 Brooke Paul
 *
 * $Id: room.c,v 6.21 2001/07/25 02:55:04 develop Exp $
 *
 * $Log: room.c,v $
 * Revision 6.21  2001/07/25 02:55:04  develop
 * fixes for pkills dropping items and gold
 *
 * Revision 6.20.1.1  2001/07/24 03:10:41  develop
 * set RNOKIL in player shops
 *
 * Revision 6.20  2001/07/21 15:44:42  develop
 * small change to find_rom_owner to avoid
 * elog_broad if a player tries to set an object
 * value in an abandoned shop
 *
 * Revision 6.19  2001/07/20 12:10:00  develop
 * added pawnshops
 *
 * Revision 6.18  2001/07/20 01:22:33  develop
 * added 's to Outfitters
 *
 * Revision 6.17  2001/07/17 19:28:44  develop
 * *** empty log message ***
 *
 * Revision 6.25  2001/07/15 06:32:23  develop
 * *** empty log message ***
 *
 * Revision 6.24  2001/07/15 06:27:08  develop
 * *** empty log message ***
 *
 * Revision 6.23  2001/07/15 06:18:19  develop
 * *** empty log message ***
 *
 * Revision 6.22  2001/07/15 06:14:09  develop
 * ditto below
 *
 * Revision 6.21  2001/07/15 06:11:03  develop
 * str bug in set_rom_owner
 *
 * Revision 6.20  2001/07/15 05:56:28  develop
 * fixing bug in set_rom_owner
 *
 * Revision 6.19  2001/07/15 05:42:50  develop
 * set_rom_owner setup_shop created
 *
 * Revision 6.18  2001/07/15 04:14:35  develop
 * *** empty log message ***
 *
 * Revision 6.17  2001/07/14 21:26:44  develop
 * *** empty log message ***
 *
 * Revision 6.16  2001/07/05 00:06:39  develop
 * removed a del_crt_rom call in go() due to crash when having a conjure
 * follow
 *
 * Revision 6.15  2001/07/04 00:13:17  develop
 * testing count_vis_ply_and_cnj
 *
 * Revision 6.14  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

#include "../include/mordb.h"
#include "mextern.h"

/**********************************************************************/
/*				add_ply_rom			      */
/**********************************************************************/

/* This function is called to add a player to a room.  It inserts the */
/* player into the room's linked player list, alphabetically.  Also,  */
/* the player's room pointer is updated.			      */

void add_ply_rom(creature *ply_ptr, room *rom_ptr )
{
	ctag	*cg, *temp, *prev;

	ASSERTLOG( ply_ptr );
	ASSERTLOG( rom_ptr );

	ply_ptr->parent_rom = rom_ptr;
	ply_ptr->rom_num = rom_ptr->rom_num;
	rom_ptr->beenhere++;
	
	cg = 0;
	cg = (ctag *)malloc(sizeof(ctag));
	if(!cg)
		merror("add_ply_rom", FATAL);
	cg->crt = ply_ptr;
	cg->next_tag = 0;

	if(!F_ISSET(ply_ptr, PDMINV) && !F_ISSET(ply_ptr, PHIDDN)) {
		broadcast_rom(ply_ptr->fd, ply_ptr->rom_num, 
		              "%M just arrived.", m1arg(ply_ptr));
	}

	add_permcrt_rom(rom_ptr);
	add_permobj_rom(rom_ptr);
	check_exits(rom_ptr);

	display_rom(ply_ptr, rom_ptr);


	if(MSP && rom_ptr->env_type && F_ISSET(ply_ptr, PSOUND)) {
		sprintf(g_buffer, "zones/%d.wav\n", (int) rom_ptr->env_type);
		play_music(ply_ptr->fd, g_buffer, NULL, -1, 
					1, "room", NULL);
	}
		

	if(!rom_ptr->first_ply) {
		rom_ptr->first_ply = cg;
#ifndef MOBS_ALWAYS_ACTIVE
		{
		ctag	*cp;
		cp = rom_ptr->first_mon;
		while(cp) 
		{
			add_active(cp->crt);
			cp = cp->next_tag;
		}
		}
#endif

		return;
	}

	temp = rom_ptr->first_ply;
	if(strcmp(temp->crt->name, ply_ptr->name) > 0) {
		cg->next_tag = temp;
		rom_ptr->first_ply = cg;
		return;
	}

	while (temp) 
	{
		if(strcmp(temp->crt->name, ply_ptr->name) > 0) 
			break;
		prev = temp;
		temp = temp->next_tag;
	}
	cg->next_tag = prev->next_tag;
	prev->next_tag = cg;

}

/**********************************************************************/
/*				del_ply_rom			      */
/**********************************************************************/

/* This function removes a player from a room's linked list of players. */
/* It also resets the player's room pointer.				*/

void del_ply_rom(creature *ply_ptr, room *rom_ptr )
{
	ctag 	*temp, *prev;

	ASSERTLOG( ply_ptr );
	ASSERTLOG( rom_ptr );
	ASSERTLOG( rom_ptr->first_ply );

	ply_ptr->parent_rom = 0;
		
	if(rom_ptr->first_ply->crt == ply_ptr) {
		temp = rom_ptr->first_ply->next_tag;
		free(rom_ptr->first_ply);
		rom_ptr->first_ply = temp;

#ifndef MOBS_ALWAYS_ACTIVE
		{
		ctag 	*cp;
		if(!temp) {
			cp = rom_ptr->first_mon;
			while(cp) {
				del_active(cp->crt);
				cp = cp->next_tag;
			}
		}
		}
#endif
		return;
	}

	prev = rom_ptr->first_ply;
	temp = prev->next_tag;
	while(temp) {
		if(temp->crt == ply_ptr) {
			prev->next_tag = temp->next_tag;
			free(temp);
			return;
		}
		prev = temp;
		temp = temp->next_tag;
	}
}

/**********************************************************************/
/*				add_obj_rom			      */
/**********************************************************************/

/* This function adds the object pointed to by the first parameter to */
/* the object list of the room pointed to by the second parameter.    */
/* The object is added alphabetically to the room.		      */

void add_obj_rom(object	*obj_ptr, room *rom_ptr )
{
	otag	*op, *temp, *prev;

	ASSERTLOG( obj_ptr );
	ASSERTLOG( rom_ptr );
	
	obj_ptr->parent_rom = rom_ptr;
	obj_ptr->parent_obj = 0;
	obj_ptr->parent_crt = 0;

	op = 0;
	op = (otag *)malloc(sizeof(otag));
	if(!op)
		merror("add_obj_rom", FATAL);
	op->obj = obj_ptr;
	op->next_tag = 0;

	if(!rom_ptr->first_obj) {
		rom_ptr->first_obj = op;
		return;
	}

	prev = temp = rom_ptr->first_obj;
	if(strcmp(temp->obj->name, obj_ptr->name) > 0 ||
	   (!strcmp(temp->obj->name, obj_ptr->name) &&
	   temp->obj->adjustment > obj_ptr->adjustment)) {
		op->next_tag = temp;
		rom_ptr->first_obj = op;
		return;
	}

	while(temp) {
		if(strcmp(temp->obj->name, obj_ptr->name) > 0 ||
		   (!strcmp(temp->obj->name, obj_ptr->name) &&
		   temp->obj->adjustment > obj_ptr->adjustment))
			break;
		prev = temp;
		temp = temp->next_tag;
	}
	op->next_tag = prev->next_tag;
	prev->next_tag = op;

}

/**********************************************************************/
/*				del_obj_rom			      */
/**********************************************************************/

/* This function removes the object pointer to by the first parameter */
/* from the room pointed to by the second.			      */

void del_obj_rom(object	*obj_ptr, room *rom_ptr )
{
	otag 	*temp, *prev;

	ASSERTLOG( obj_ptr );
	ASSERTLOG( rom_ptr );

	obj_ptr->parent_rom = 0;
	if(rom_ptr->first_obj->obj == obj_ptr) {
		temp = rom_ptr->first_obj->next_tag;
		free(rom_ptr->first_obj);
		rom_ptr->first_obj = temp;
		return;
	}

	prev = rom_ptr->first_obj;
	temp = prev->next_tag;
	while(temp) {
		if(temp->obj == obj_ptr) {
			prev->next_tag = temp->next_tag;
			free(temp);
			return;
		}
		prev = temp;
		temp = temp->next_tag;
	}
}

/**********************************************************************/
/*				add_crt_rom			      */
/**********************************************************************/

/* This function adds the monster pointed to by the first parameter to	*/
/* the room pointed to by the second.  The third parameter determines	*/
/* how the people in the room should be notified.  If it is equal to	*/
/* zero, then no one will be notified that a monster entered the room.	*/
/* If it is non-zero, then the room will be told that "num" monsters	*/
/* of that name entered the room.					*/

void add_crt_rom(creature *crt_ptr, room *rom_ptr, int num )
{
	ctag	*cg, *temp, *prev;
	char	str[160];

	crt_ptr->parent_rom = rom_ptr;
	crt_ptr->rom_num = rom_ptr->rom_num;

	cg = 0;
	cg = (ctag *)malloc(sizeof(ctag));
	if(!cg)
		merror("add_crt_rom", FATAL);
	cg->crt = crt_ptr;
	cg->next_tag = 0;

	sprintf(str, "%%%dM just arrived.", num);
	if(!F_ISSET(crt_ptr, MINVIS) && !F_ISSET(crt_ptr, MPERMT) && num)
		broadcast_rom(crt_ptr->fd, crt_ptr->rom_num, str, m1arg(crt_ptr));

	if(!rom_ptr->first_mon) {
		rom_ptr->first_mon = cg;
		return;
	}

	temp = rom_ptr->first_mon;
	if(strcmp(temp->crt->name, crt_ptr->name) > 0) {
		cg->next_tag = temp;
		rom_ptr->first_mon = cg;
		return;
	}

	while(temp) {
		if(strcmp(temp->crt->name, crt_ptr->name) > 0) 
			break;
		prev = temp;
		temp = temp->next_tag;
	}
	cg->next_tag = prev->next_tag;
	prev->next_tag = cg;

}

/**********************************************************************/
/*				del_crt_rom			      */
/**********************************************************************/

/* This function removes the monster pointed to by the first parameter */
/* from the room pointed to by the second.			       */

void del_crt_rom(creature *crt_ptr, room *rom_ptr )
{
	ctag 	*temp, *prev;

	ASSERTLOG( crt_ptr );
	ASSERTLOG( rom_ptr );

	if(!crt_ptr) {
		elog_broad("NULL crt_ptr in del_crt_rom");
		return;
	}

	crt_ptr->parent_rom = 0;
	crt_ptr->rom_num = 0;

	if(!rom_ptr) {
		elog_broad("NULL rom_ptr in del_crt_rom");
		return;
	}

	if(!rom_ptr->first_mon) {
		elog_broad("NULL first_mon in del_crt_rom");
		return;
	}

	if(rom_ptr->first_mon->crt == crt_ptr) {
		temp = rom_ptr->first_mon->next_tag;
		free(rom_ptr->first_mon);
		rom_ptr->first_mon = temp;
		return;
	}

	prev = rom_ptr->first_mon;
	temp = prev->next_tag;
	while(temp) {
		if(temp->crt == crt_ptr) {
			prev->next_tag = temp->next_tag;
			free(temp);
			return;
		}
		prev = temp;
		temp = temp->next_tag;
	}
}


/**********************************************************************/
/*				add_permcrt_rom			      */
/**********************************************************************/

/* This function checks a room to see if any permanent monsters need to */
/* be loaded.  If so, the monsters are loaded to the room, and their    */
/* permanent flag is set.						*/

void add_permcrt_rom(room *rom_ptr )
{
	short		checklist[10];
	creature	*crt_ptr;
	object		*obj_ptr;
	ctag		*cp;
	time_t		t;
	int		h, i, j, k, l, m, n;

	t = time(0);

	for(i=0; i<10; i++)
		checklist[i] = 0;

	for(i=0; i<10; i++) {

		if(checklist[i]) 
			continue;

		/* is there a monster in this slot? */
		if(!rom_ptr->perm_mon[i].misc) 
			continue;

		/* is it time for this monster to appear */
		if(rom_ptr->perm_mon[i].ltime + rom_ptr->perm_mon[i].interval > t) 
			continue;

		/* count how many of these we are supposed to have */
		n = 1;
		for(j=i+1; j<10; j++) 
			if(rom_ptr->perm_mon[i].misc == rom_ptr->perm_mon[j].misc && (rom_ptr->perm_mon[j].ltime + rom_ptr->perm_mon[j].interval) < t) {
				n++;
				checklist[j] = 1;
			}

		if(load_crt(rom_ptr->perm_mon[i].misc, &crt_ptr) < 0)
			continue;

		/* count how many are already there */
		cp = rom_ptr->first_mon;
		m = 0;
		while(cp) 
		{
			if(F_ISSET(cp->crt, MPERMT) && 
			   !strcmp(cp->crt->name, crt_ptr->name))
				m++;
			cp = cp->next_tag;
		}

		free_crt(crt_ptr);

		for(j=0; j<n-m; j++) 
		{
			l = load_crt(rom_ptr->perm_mon[i].misc, &crt_ptr);
			if(l < 0) 
				continue;
			l = mrand(1,100);
			if(l<90) 
				l=1;
			else if(l<96) 
				l=2;
			else 
				l=3;

			for(k=0; k<l; k++) 
			{
				h = mrand(0,9);
				if(crt_ptr->carry[h]) 
				{
					h = load_obj(crt_ptr->carry[h], 
						     &obj_ptr);
					if(F_ISSET(obj_ptr, ORENCH))
						rand_enchant(obj_ptr);
					if(h > -1)
						add_obj_crt(obj_ptr, crt_ptr);
				}
			}
			if(!F_ISSET(crt_ptr, MNRGLD) && crt_ptr->gold)
				crt_ptr->gold = mrand(crt_ptr->gold/10,
					crt_ptr->gold);
			F_SET(crt_ptr, MPERMT);
			add_crt_rom(crt_ptr, rom_ptr, 0);

#ifndef MOBS_ALWAYS_ACTIVE
			if(rom_ptr->first_ply)
				add_active(crt_ptr);
#endif

		}
	}
}

/**********************************************************************/
/*				add_permobj_rom			      */
/**********************************************************************/

/* This function checks a room to see if any permanent objects need to  */
/* be loaded.  If so, the objects are loaded to the room, and their     */
/* permanent flag is set.						*/

void add_permobj_rom(room *rom_ptr )
{
	short	checklist[10];
	object	*obj_ptr;
	otag	*op;
	time_t	t;
	int	i, j, m, n;

	t = time(0);

	for(i=0; i<10; i++)
		checklist[i] = 0;

	for(i=0; i<10; i++) {

		if(checklist[i]) continue;
		if(!rom_ptr->perm_obj[i].misc) continue;
		if(rom_ptr->perm_obj[i].ltime + rom_ptr->perm_obj[i].interval >
		   t) continue;

		n = 1;
		for(j=i+1; j<10; j++) 
			if(rom_ptr->perm_obj[i].misc == 
			   rom_ptr->perm_obj[j].misc && 
			   (rom_ptr->perm_obj[j].ltime + 
			   rom_ptr->perm_obj[j].interval) < t) {
				n++;
				checklist[j] = 1;
			}

		if(load_obj(rom_ptr->perm_obj[i].misc, &obj_ptr) < 0)
			continue;

		op = rom_ptr->first_obj;
		m = 0;
		while(op) {
			if(F_ISSET(op->obj, OPERMT) && 
			   !strcmp(op->obj->name, obj_ptr->name))
				m++;
			op = op->next_tag;
		}

		free_obj(obj_ptr);

		for(j=0; j<n-m; j++) {
			if(load_obj(rom_ptr->perm_obj[i].misc, &obj_ptr) < 0)
				continue;
			if(F_ISSET(obj_ptr, ORENCH))
				rand_enchant(obj_ptr);

				
			F_SET(obj_ptr, OPERMT);
			add_obj_rom(obj_ptr, rom_ptr);
		}
	}
}


/**********************************************************************/
/*				check_exits			      */
/**********************************************************************/

/* This function checks the status of the exits in a room.  If any of */
/* the exits are closable or lockable, and the correct time interval  */
/* has occurred since the last opening/unlocking, then the doors are  */
/* re-shut/re-closed.						      */

void check_exits(room *rom_ptr )
{
	xtag	*xp;
	time_t	t;
	/*short chance; */

	ASSERTLOG( rom_ptr );

	t = time(0);

	xp = rom_ptr->first_ext;
	while(xp) {
		if(F_ISSET(xp->ext, XLOCKS) && (xp->ext->ltime.ltime + 
		   xp->ext->ltime.interval) < t) {
			F_SET(xp->ext, XLOCKD);
			F_SET(xp->ext, XCLOSD);
		}
		else if(F_ISSET(xp->ext, XCLOSS) && (xp->ext->ltime.ltime +
			 xp->ext->ltime.interval) < t)
			F_SET(xp->ext, XCLOSD);

/* Moved to update_random_exit in update.c, this should be removed soon */
/*  
		if(F_ISSET(xp->ext, XRANDM) && xp->ext->random.interval) {
			if (xp->ext->random.ltime + xp->ext->random.interval < t) {
				if(F_ISSET(xp->ext, XNOSEE)) {
					F_CLR(xp->ext, XNOSEE);
					xp->ext->random.ltime = t;
					if(xp->ext->rand_mesg[0]) {
					   if(F_ISSET(xp->ext, XRANDB))
						broadcast(xp->ext->rand_mesg[0]);
					   else
						broadcast_rom(-1,rom_ptr->rom_num,
							xp->ext->rand_mesg[0],NULL);
					}
				}
				else {
					F_SET(xp->ext, XNOSEE);
					xp->ext->random.ltime = t;
					if(xp->ext->rand_mesg[1]) {
					   if(F_ISSET(xp->ext, XRANDB))
						broadcast(xp->ext->rand_mesg[1]);
					   else
						broadcast_rom(-1,rom_ptr->rom_num,
							xp->ext->rand_mesg[1],NULL);
					}
				}
			}
		} else {  */ /* random, but no specific time */ /*
				chance = xp->ext->random.misc;
				if(chance && (chance > mrand(1,100))) {
				   if(F_ISSET(xp->ext, XNOSEE)) {
				      F_CLR(xp->ext, XNOSEE);
				      if(xp->ext->rand_mesg[0]) {
					   if(F_ISSET(xp->ext, XRANDB))
						broadcast(xp->ext->rand_mesg[0]);
					   else
				         	broadcast_rom(-1,rom_ptr->rom_num, 
							xp->ext->rand_mesg[0],NULL);
				      }
				         xp->ext->random.ltime = t;
			           }
				   else {
				      F_SET(xp->ext, XNOSEE);
				      if(xp->ext->rand_mesg[1]) {
					   if(F_ISSET(xp->ext, XRANDB))
						broadcast(xp->ext->rand_mesg[1]);
					   else	
						 broadcast_rom(-1,rom_ptr->rom_num, 
							xp->ext->rand_mesg[1],NULL);
				      }
				      xp->ext->random.ltime = t;
			           }
				}
			}
*/					
		xp = xp->next_tag;
		}

}

/**********************************************************************/
/*				display_rom			      */
/**********************************************************************/

/* This function displays the descriptions of a room, all the players */
/* in a room, all the monsters in a room, all the objects in a room,  */
/* and all the exits in a room.  That is, unless they are not visible */
/* or the room is dark.						      */

void display_rom(creature *ply_ptr, room *rom_ptr )
{
	xtag		*xp;
	ctag		*cp;
	creature	*crt_ptr;
	char		str[2048];
	int		fd, n=0, m, t, light = 0;
	int		ct;

	ASSERTLOG( ply_ptr );
	ASSERTLOG( rom_ptr );

	fd = ply_ptr->fd;
	ct = ply_ptr->class >= CARETAKER;

	output(fd, "\n");

	t = Time%24L;
	if(F_ISSET(ply_ptr, PBLIND) || F_ISSET(rom_ptr, RDARKR) || 
	   (F_ISSET(rom_ptr, RDARKN) && (t<6 || t>20))) {
		if(!has_light(ply_ptr)) {
			cp = rom_ptr->first_ply;
			while(cp) {
				if(has_light(cp->crt)) {
					light = 1;
					break;
				}
				cp = cp->next_tag;
			}
			if(ply_ptr->race == ELF ||
			    ply_ptr->race == DWARF ||
			    ply_ptr->race == GNOME ||
			    ply_ptr->race == TROLL ||
			    ply_ptr->race == DARKELF ||
			     ply_ptr->race == GOBLIN ||
				ply_ptr->race == ORC ||	
		    ply_ptr->class >= BUILDER)
				light = 1;
		}
		else
			light = 1;

		if(!light || F_ISSET(ply_ptr, PBLIND)) {
			if(F_ISSET(ply_ptr, PBLIND)) 
			{
				output_wc(fd, "You're blind!\n", BLINDCOLOR);
			}
			output_wc(fd, "It's too dark to see.\n", AFC_YELLOW);
			return;
		}
		
	}
	
	if(!F_ISSET(ply_ptr, PNORNM)) {
		if (ply_ptr->class >= BUILDER)
			sprintf(g_buffer, "%d - %s\n\n", rom_ptr->rom_num, rom_ptr->name);
		else
			sprintf(g_buffer, "%s\n\n", rom_ptr->name);

		output_wc(fd, g_buffer, ROOMNAMECOLOR);
	}

	if(!F_ISSET(ply_ptr, PNOSDS) && rom_ptr->short_desc)
	{
		output_nl(fd, rom_ptr->short_desc);
	}

	if(!F_ISSET(ply_ptr, PNOLDS) && rom_ptr->long_desc)
	{
		output_nl(fd, rom_ptr->long_desc);
	}

	if(!F_ISSET(ply_ptr, PNOEXT)) {
		n=0; str[0] = 0;
		if ( ct ) 
		{
			strcat(str, "All exits: ");
		}
		else
		{
			strcat(str, "Obvious exits: ");
		}

		xp = rom_ptr->first_ext;
		while(xp) {
			if(ct || (!F_ISSET(xp->ext, XSECRT) 
				&& !F_ISSET(xp->ext, XNOSEE) 
				&& (F_ISSET(ply_ptr, PDINVI) ? 1:!F_ISSET(xp->ext, XINVIS)))) 
			{
				strcat(str, xp->ext->name);

				if ( ct )
				{
					if ( F_ISSET(xp->ext, XSECRT) )
					{
						strcat( str, " (h)");
					}
					if ( F_ISSET(xp->ext, XNOSEE) )
					{
						strcat( str, " (n)" );
					}
					if ( F_ISSET(xp->ext, XINVIS) )
					{
						strcat( str, " (*)" );
					}
				}

				strcat(str, ", ");
				n++;
			}
			xp = xp->next_tag;
		}

		if(!n)
			strcat(str, "none");
		else
			str[strlen(str)-2] = 0;

		strcat(str, ".\n");
		output_wc(fd, str, EXITCOLOR);
	}

	cp = rom_ptr->first_ply; n=0; str[0]=0;
	strcat(str, "You see ");
	while(cp) {
		/* dont show yourself */
		if ( cp->crt != ply_ptr)
		{
			if( ct || ((F_ISSET(ply_ptr, PDINVI) ? 1:!F_ISSET(cp->crt, PINVIS)) &&
			   !F_ISSET(cp->crt, PHIDDN))) 
			{
				if ( !F_ISSET(cp->crt, PDMINV) 
					|| (F_ISSET(cp->crt, PDMINV) && ply_ptr->class >= cp->crt->class ))
				{
					strcat(str, cp->crt->name);
					
					if(F_ISSET(cp->crt, PAFK))
					
						strcat(str, "[AFK]");
					
					if ( ct )
					{
						if ( F_ISSET(cp->crt, PHIDDN) )
						{
							strcat( str , " (h)");
						}
						if ( F_ISSET(cp->crt, PINVIS) )
						{
							strcat( str , " (*)");
						}
					}

					strcat(str, ", ");
					n++;
				}
			}
		}
		cp = cp->next_tag;
	}
	if(n) {
		str[strlen(str)-2] = 0;

		strcat(str, ".\n");
		output_wc(fd, str, PLAYERCOLOR);
	}

	cp = rom_ptr->first_mon; n=0; str[0]=0;
	strcat(str, "You see ");
	while(cp) {
		if(ct || ((F_ISSET(ply_ptr, PDINVI) ? 1:!F_ISSET(cp->crt, MINVIS)) &&
		   !F_ISSET(cp->crt, MHIDDN))) 
		{
			m=1;
			while(cp->next_tag) {
				if(!strcmp(cp->next_tag->crt->name, cp->crt->name) &&
				   ((ct || (F_ISSET(ply_ptr, PDINVI) ? 
				   1:!F_ISSET(cp->next_tag->crt, MINVIS))) &&
				   !F_ISSET(cp->next_tag->crt, MHIDDN))) {
					m++;
					cp = cp->next_tag;
				}
				else
					break;
			}

			strcat(str, crt_str(ply_ptr, cp->crt, m, 0));

			if ( ct ) 
			{
				if ( F_ISSET(cp->crt, MHIDDN) )
				{
					strcat( str, " (h)");
				}
				if ( F_ISSET(cp->crt, MINVIS) )
				{
					strcat( str, " (*)");
				}
			}

			strcat(str, ", ");
			n++;
		}
		cp = cp->next_tag;
	}
	if(n) {
		str[strlen(str)-2] = 0;
		strcat(str, ".\n");
		output_wc(fd, str, CREATURECOLOR);
	}

	str[0]=0;  strcat(str, "You see ");
	n = list_obj(&str[8], ply_ptr, rom_ptr->first_obj);
        if(n) 
		{
			strcat(str, ".\n");
			output_wc(fd, str, ITEMCOLOR );
        }

	cp = rom_ptr->first_mon;
	while(cp) {
		if(cp->crt->first_enm) {
			crt_ptr = find_exact_crt(ply_ptr, rom_ptr->first_ply,
				cp->crt->first_enm->enemy, 1);
			ANSI(fd, ATTACKCOLOR);
			if(crt_ptr == ply_ptr)
				mprint(fd, "%M is attacking you.\n", m1arg(cp->crt));
			else if(crt_ptr)
				mprint(fd, "%M is attacking %m.\n", m2args(cp->crt,
				      crt_ptr));
			ANSI(fd, MAINTEXTCOLOR);
		}
		cp = cp->next_tag;
	}

	output(fd, "\n");

}

/**********************************************************************/
/*				find_ext			      */
/**********************************************************************/

exit_ *find_ext(creature *ply_ptr, xtag *first_xt, char *str, int val )
{
	xtag	*xp;
	int	match = 0, found = 0;

	xp = first_xt;
	while(xp) {
		if(!strncmp(xp->ext->name, str, strlen(str)) &&
		  (!F_ISSET(xp->ext, XNOSEE)) &&
		   (F_ISSET(ply_ptr, PDINVI) ? 1:!F_ISSET(xp->ext, XINVIS)))
			match++;
		if(match == val) {
			found = 1;
			break;
		}
		xp = xp->next_tag;
	}

	if(!found)
		return(0);

	return(xp->ext);

}

/**********************************************************************/
/*				check_traps			      */
/**********************************************************************/

/* This function checks a room to see if there are any traps and whether */
/* the player pointed to by the first parameter fell into any of them.   */

void check_traps(creature *ply_ptr, room *rom_ptr )
{
	room	*new_rom;
	ctag	*cp;
	creature *tmp_crt;
	int		dmg, fd;

	if(!rom_ptr->trap) {
		F_CLR(ply_ptr, PPREPA);
		return;
	}
	if(ply_ptr->class > BUILDER){
		output_wc(ply_ptr->fd, "Trap passed.\n", TRAPCOLOR );
		return;
	}

	switch(rom_ptr->trap){

	case TRAP_PIT:
	case TRAP_DART:
	case TRAP_BLOCK:
	case TRAP_NAKED:
	case TRAP_ALARM:
	    if(F_ISSET(ply_ptr, PPREPA) && mrand(1,20) < ply_ptr->dexterity) {
		F_CLR(ply_ptr, PPREPA);
		return;
	     }
	    F_CLR(ply_ptr, PPREPA);
	    if(mrand(1,100) < ply_ptr->dexterity)
		return;

	break;
	case TRAP_MPDAM:
	case TRAP_RMSPL:
	    if(F_ISSET(ply_ptr, PPREPA) && mrand(1,25) < 
		(ply_ptr->intelligence)) {
		F_CLR(ply_ptr, PPREPA);
		return;
	     }
	    F_CLR(ply_ptr, PPREPA);
	    if(mrand(1,100) < ply_ptr->intelligence)
		return;

	break;
	default:
		return;
	}


	fd = ply_ptr->fd;

	switch (rom_ptr->trap){
		case TRAP_PIT:
			if(!F_ISSET(ply_ptr, PLEVIT)){
				output(fd, "You fell into a pit trap!\n");
				broadcast_rom(fd, ply_ptr->rom_num, 
				"%M fell into a pit trap!", m1arg(ply_ptr));
				del_ply_rom(ply_ptr, rom_ptr);
				if(load_rom(rom_ptr->trapexit, &new_rom) < 0)
				   return;
				add_ply_rom(ply_ptr, new_rom);
				dmg = mrand(1,15);
				sprintf(g_buffer, "You lost %d hit points.\n", dmg);
				output(fd, g_buffer);
				ply_ptr->hpcur -= dmg;
				
				/* Stop following 
				if(ply_ptr->following) {
       			        	old_ptr = ply_ptr->following;
                			tp = old_ptr->first_fol;
                			if(tp->crt == ply_ptr) {
                  			old_ptr->first_fol = tp->next_tag;
                        		free(tp);
                			}
                			else while(tp) {
                        			if(tp->crt == ply_ptr) {
                                			prev->next_tag = tp->next_tag;
                                			free(tp);
                               			 	break;
                        			}
                        			prev = tp;
                        			tp = tp->next_tag;
                			}
                			ply_ptr->following = 0;
                			sprintf(g_buffer, "You stop following %s.\n", old_ptr->name);
                			output(fd, g_buffer);
                			if(!F_ISSET(ply_ptr, PDMINV))
                        			mprint(old_ptr->fd, "%M stops following you.\n",m1arg(ply_ptr));
        			} */

				if(ply_ptr->hpcur < 1)
					die(ply_ptr, ply_ptr);
			}
			break;

		case TRAP_DART:
			output(fd, "You triggered a hidden dart!\n");
			broadcast_rom(fd, ply_ptr->rom_num,
				"%M gets hit by a hidden dart.", m1arg(ply_ptr));
			dmg = mrand(1,10);
			sprintf(g_buffer, "You lost %d hit points.\n", dmg);
			output(fd, g_buffer);
			F_SET(ply_ptr, PPOISN);
			ply_ptr->hpcur -= dmg;

			/* Stop following 
                              if(ply_ptr->following) {
                                        old_ptr = ply_ptr->following;
                                        tp = old_ptr->first_fol;
                                        if(tp->crt == ply_ptr) {
                                        old_ptr->first_fol = tp->next_tag;
                                        free(tp);
                                        }
                                        else while(tp) {
                                                if(tp->crt == ply_ptr) {
                                                        prev->next_tag = tp->next_tag;
                                                        free(tp);
                                                        break;
                                                }
                                                prev = tp;
                                                tp = tp->next_tag;
                                        }
                                        ply_ptr->following = 0;
                                        sprintf(g_buffer, "You stop following %s.\n", old_ptr->name);
                                        output(fd, g_buffer);
                                        if(!F_ISSET(ply_ptr, PDMINV))
                                                mprint(old_ptr->fd, "%M stops following y
ou.\n",m1arg(ply_ptr));
                                } */

			if(ply_ptr->hpcur < 1)
				die(ply_ptr, ply_ptr);
			break;

		case TRAP_BLOCK:
			output(fd, "You triggered a falling block!\n");
			broadcast_rom(fd, ply_ptr->rom_num,
				"A large block falls on %m.", m1arg(ply_ptr));
			dmg = ply_ptr->hpmax / 2;
			sprintf(g_buffer, "You lost %d hit points.\n", dmg);
			output(fd, g_buffer);
			ply_ptr->hpcur -= dmg;

/* Stop following 
                                if(ply_ptr->following) {
                                        old_ptr = ply_ptr->following;
                                        tp = old_ptr->first_fol;
                                        if(tp->crt == ply_ptr) {
                                        old_ptr->first_fol = tp->next_tag;
                                        free(tp);
                                        }
                                        else while(tp) {
                                                if(tp->crt == ply_ptr) {
                                                        prev->next_tag = tp->next_tag;
                                                        free(tp);
                                                        break;
                                                }
                                                prev = tp;
                                                tp = tp->next_tag;
                                        }
                                        ply_ptr->following = 0;
                                        sprintf(g_buffer, "You stop following %s.\n", old_ptr->name);
                                        output(fd, g_buffer);
                                        if(!F_ISSET(ply_ptr, PDMINV))
                                                mprint(old_ptr->fd, "%M stops following y
ou.\n",m1arg(ply_ptr));
                                } */

			if(ply_ptr->hpcur < 1)
				die(ply_ptr, ply_ptr);
			break;

		case TRAP_MPDAM:
			output(fd, "You feel an exploding force in your mind!\n");
			broadcast_rom(fd, ply_ptr->rom_num,
				"An energy bolts strikes %m.", m1arg(ply_ptr));
			dmg = MIN(ply_ptr->mpcur,ply_ptr->mpmax / 2);
			sprintf(g_buffer, "You lost %d magic points.\n", dmg);
			output(fd, g_buffer);
			ply_ptr->mpcur -= dmg;
			dmg =  mrand(1,6);
			ply_ptr->hpcur -= dmg;
			sprintf(g_buffer, "You lost %d hit points.\n", dmg);
			output(fd, g_buffer);

/* Stop following
                                if(ply_ptr->following) {
                                        old_ptr = ply_ptr->following;
                                        tp = old_ptr->first_fol;
                                        if(tp->crt == ply_ptr) {
                                        old_ptr->first_fol = tp->next_tag;
                                        free(tp);
                                        }
                                        else while(tp) {
                                                if(tp->crt == ply_ptr) {
                                                        prev->next_tag = tp->next_tag;
                                                        free(tp);
                                                        break;
                                                }
                                                prev = tp;
                                                tp = tp->next_tag;
                                        }
                                        ply_ptr->following = 0;
                                        sprintf(g_buffer, "You stop following %s.\n", old_ptr->name);
                                        output(fd, g_buffer);
                                        if(!F_ISSET(ply_ptr, PDMINV))
                                                mprint(old_ptr->fd, "%M stops following y
ou.\n",m1arg(ply_ptr));
                                } */

			if(ply_ptr->hpcur < 1) die(ply_ptr, ply_ptr);
			break;

		case TRAP_RMSPL:
			output(fd, "A foul smelling charcoal cloud surrounds you.\n");
			output(fd, "Your magic begins to dissolve.\n");
			broadcast_rom(fd, ply_ptr->rom_num,
				"A charcoal cloud surrounds %m.", m1arg(ply_ptr));
		
			ply_ptr->lasttime[LT_PROTE].interval =
			ply_ptr->lasttime[LT_BLESS].interval = 
			ply_ptr->lasttime[LT_RFIRE].interval =
			ply_ptr->lasttime[LT_RCOLD].interval = 
			ply_ptr->lasttime[LT_BRWAT].interval =
			ply_ptr->lasttime[LT_SSHLD].interval =
			ply_ptr->lasttime[LT_RMAGI].interval =
			ply_ptr->lasttime[LT_LIGHT].interval =
			ply_ptr->lasttime[LT_DINVI].interval =
			ply_ptr->lasttime[LT_INVIS].interval = 
			ply_ptr->lasttime[LT_KNOWA].interval =
			ply_ptr->lasttime[LT_DMAGI].interval = 0;
			break;
                            
		case TRAP_NAKED:	
			output(fd, "You are covered in oozing green slime.\n");
			output(fd, "All your possessions dissolve away.\n");
			broadcast_rom(fd, ply_ptr->rom_num,
				"An oozing green slime envelops %m.", m1arg(ply_ptr));
			lose_all(ply_ptr);
			break;

		case TRAP_ALARM:
			output(fd, "You set off an alarm!\n");
			output(fd, "You hope there aren't any guards around.\n\n");
			broadcast_rom(fd, ply_ptr->rom_num, 
			"%M sets off an alarm!\n", m1arg(ply_ptr));
			if(load_rom(rom_ptr->trapexit, &new_rom) < 0)
			   return;
			add_permcrt_rom(new_rom);
			cp = new_rom->first_mon;
			while(cp) 
			{
				tmp_crt = cp->crt;
				cp = cp->next_tag;
				if(F_ISSET(tmp_crt, MPERMT))
				{
					if(new_rom->first_ply)
						broadcast_rom(tmp_crt->fd, tmp_crt->rom_num, 
							"%M hears an alarm and leaves to investigate.",m1arg(tmp_crt));
					else
					{
#ifndef MOBS_ALWAYS_ACTIVE
						add_active(tmp_crt); 
#endif
					}

					F_CLR(tmp_crt,MPERMT);
					F_SET(tmp_crt,MAGGRE);
                    die_perm_crt(tmp_crt);
                    del_crt_rom(tmp_crt, new_rom);
                    add_crt_rom(tmp_crt, rom_ptr, 1);
					broadcast_rom(tmp_crt->fd, tmp_crt->rom_num, 
						"%M comes to investigate the alarm.", m1arg(tmp_crt));
				}
			}
			break;
	}
	return;


}

/**********************************************************************/
/*				count_ply			      */
/**********************************************************************/

/* This function counts the number of (non-DM-invisible) players in a */
/* room and returns that number.				      */

int count_vis_ply(room *rom_ptr )
{
	ctag	*cp;
	int	num = 0;

	ASSERTLOG( rom_ptr );

	cp = rom_ptr->first_ply;
	while(cp) {
		if(!F_ISSET(cp->crt, PDMINV)) num++;
		cp = cp->next_tag;
	}

	return num;
}

/* counts monsters in the room and returns the number */
int count_vis_mon(room *rom_ptr )
{
        ctag    *cp;
        int     num = 0;

        ASSERTLOG( rom_ptr );

        cp = rom_ptr->first_mon;
        while(cp) {
                num++;
                cp = cp->next_tag;
        }

        return num;
}

/* counts players and conjured creatures in the room and returns number */
int count_vis_ply_and_cnj(room *rom_ptr )
{
        ctag    *cp;
        int     num = 0;

        ASSERTLOG( rom_ptr );

        cp = rom_ptr->first_mon;
	while(cp) {
		if(F_ISSET(cp->crt, MCONJU))
                	num++;
                cp = cp->next_tag;
        }

	cp = rom_ptr->first_ply;
        while(cp) {
                if(!F_ISSET(cp->crt, PDMINV)) num++;
                cp = cp->next_tag;
        }

        return num;
}

/*	find_rom_owner							 */
/* This finds and returns a pointer to the player that owns a given room.*/
/* This function assumes that the first string in the rom_ptr->name is 	 */
/*	owner's name.							 */

creature *find_rom_owner(room *rom_ptr)
{
	int		i, j;
	char		str[80];
	creature	*crt_ptr;


	ASSERTLOG( rom_ptr );

	if(!F_ISSET(rom_ptr, RPOWND)) {
		return(0);
	}

	strncpy(str, rom_ptr->name, 79);

	/* parse out first string from rom name (should be owner's name) */
	j = strlen(str);
	i = 0;

	for(i=0; i<=j; i++) {

		/* look for first non space or comment  */
		if (str[i] == ' '  || str[i] == '#' )
                        continue;

		/* ok we're at first non space */
		/* now find the end of the string */
		/* expect one of 3 conditions:
			string terms
			find apostrophe (as in Foo's)
			find a hashmark comment
						  */
		while ( str[i] != '\0' && str[i] != ' ' && str[i] != '\'' && str[i] != '#') {
          	      i++;
                }

                /* terminate the string */
                str[i] = '\0';
	}

	/* str should now equal the owner's name */
	/* find the owner */
	str[0] = up(str[0]);
	if(!strcmp(str, "An")) { /* An abandoned shop */
		return(0);	 /* added to avoid elog when you value in */
	}			 /* an abandoned */
	crt_ptr = find_who(str);
	if(!crt_ptr) {
		if(load_ply(str, &crt_ptr) < 0){
			/* do an autoreset of room here since owner is AWOL? */
			sprintf(g_buffer, "Room owner %s does not exist for room %d.\n", str, rom_ptr->rom_num);
                        elog_broad(g_buffer);
                        return (0);
                }
	}

	return(crt_ptr);
}

/* This sets up a pair of rooms to be used by a player to sell items */
int set_rom_owner(creature *ply_ptr, int type)
{
	room	*rom_ptr, *dep_ptr;
	object	*obj_ptr;
	xtag	*xp;
	int 	n;

	ASSERTLOG( ply_ptr );

	rom_ptr = ply_ptr->parent_rom;

	if(!rom_ptr)
		return(0);

	/* create an entrace for the owner */
	if(load_rom(rom_ptr->rom_num+1, &dep_ptr) < 0) {
                return(0);
        }

	/* set the storeroom's name.  This is how we tell ownership */
	sprintf(dep_ptr->name, "%s%s", ply_ptr->name, "'s storeroom");

	/* now set the name of the shop itself */
	switch (type)
	{
		case ARMORY:
			sprintf(rom_ptr->name, "%s%s", ply_ptr->name, "'s Armory");
                        break;
		case OUTFITTER:
			sprintf(rom_ptr->name, "%s%s", ply_ptr->name, "'s Outfitters");
			break;
		case EMPORIUM:
			sprintf(rom_ptr->name, "%s%s", ply_ptr->name, "'s Emporium");
                        break;
		case GENERALSTORE:
			sprintf(rom_ptr->name, "%s%s", ply_ptr->name, "'s General Store");
                        break;
		case PAWNSHOP:
			sprintf(rom_ptr->name, "%s%s", ply_ptr->name, "'s Pawn Shop");
			break;
		case MARKETPLACE:
                        sprintf(rom_ptr->name, "%s%s", ply_ptr->name, "'s Marketplace");
			break;
		default:
			sprintf(rom_ptr->name, "%s%s", ply_ptr->name, "'s General Store");
                        break;

	}

	/* link the storeroom and the shop */
	link_rom(&rom_ptr, dep_ptr->rom_num, "storeroom");
	link_rom(&dep_ptr, rom_ptr->rom_num, "out");

	/* find the storeroom exit and set it to be unique */
	xp = rom_ptr->first_ext;
	while(xp) {
		if(!strcmp(xp->ext->name, "storeroom")){
            	F_SET(xp->ext, XUNIQP);
            	break;
        	}
        xp = xp->next_tag;
	}

	/* put a storage chest in the storeroom */
	n=load_obj(CHEST, &obj_ptr);
	if(n>-1) {
		add_obj_rom(obj_ptr, dep_ptr);	
		F_SET(obj_ptr, OPERMT);
	}
	else {
		elog("Unable to create storage object in storeroom.\n");
	}

	F_SET(dep_ptr, RPOWND);
	F_SET(dep_ptr, RNOTEL);
	F_SET(dep_ptr, RONEPL);
	F_SET(dep_ptr, RNDOOR);
	F_CLR(rom_ptr, RPOWND);
	F_SET(rom_ptr, RSHOPP);
	F_SET(rom_ptr, RNOKIL);
	
	resave_rom(rom_ptr->rom_num);
	resave_rom(dep_ptr->rom_num);

	return(1);
}
