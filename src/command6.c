/*
 * COMMAND6.C:
 *
 *	Additional user routines.
 *
 *	Copyright (C) 1991, 1992, 1993 Brooke Paul
 *
 * $Id: command6.c,v 6.36 2001/07/25 02:55:04 develop Exp $
 *
 * $Log: command6.c,v $
 * Revision 6.36  2001/07/25 02:55:04  develop
 * fixes for thieves dropping stolen items
 * fixes for gold dropping by pkills
 *
 * Revision 6.36  2001/07/24 01:36:23  develop
 * *** empty log message ***
 *
 * Revision 6.35  2001/07/22 20:05:52  develop
 * gold theft changes
 *
 * Revision 6.34  2001/07/22 19:03:06  develop
 * first run at alllowing thieves to steal gold from other players
 *
 * Revision 6.33  2001/07/22 15:59:18  develop
 * first steps in getting gold stealing implemented
 *
 * Revision 6.32  2001/07/17 19:25:11  develop
 * *** empty log message ***
 *
 * Revision 6.31  2001/07/06 00:17:08  develop
 * RNOLEA fix
 *
 * Revision 6.30  2001/07/05 23:49:30  develop
 * RNOLEA tweaks
 *
 * Revision 6.29  2001/07/05 22:41:45  develop
 * revised RNOLEA to change output by request of MKolb (Azog)
 *
 * Revision 6.28  2001/07/05 00:06:39  develop
 * removed a del_crt_rom call in go() due to crash when having a conjure
 * follow
 *
 * Revision 6.27  2001/07/04 00:30:16  develop
 * removed count_vis_ply_and_cnj...didn't work
 *
 * Revision 6.26  2001/07/04 00:13:17  develop
 * testing count_vis_ply_and_cnj
 *
 * Revision 6.25  2001/07/03 22:47:47  develop
 * *** empty log message ***
 *
 * Revision 6.24  2001/07/03 01:32:22  develop
 * removed newline from conjure "just went to the" output
 *
 * Revision 6.23  2001/07/01 23:59:48  develop
 * RNOLEA changed for conjures
 *
 * Revision 6.22  2001/07/01 22:43:01  develop
 * added output to conjure fading when moving into an RNOLEA room
 *
 * Revision 6.21  2001/07/01 22:38:09  develop
 * changed method of keeping conjured creatures out of RNOLEA rooms
 *
 * Revision 6.20  2001/07/01 19:15:53  develop
 * fixed output when trying to take a conjured creature into RNOLEA room
 *
 * Revision 6.19  2001/07/01 19:10:58  develop
 * fixing blocking of conjure creatures in RNOLEA rooms
 *
 * Revision 6.18  2001/07/01 19:01:46  develop
 * removed ability to have conjured creatures in RNOLEAV rooms
 *
 * Revision 6.17  2001/06/06 19:36:46  develop
 * conjure added
 *
 * Revision 6.16  2001/03/17 04:15:17  develop
 * made it so thieves can steal from each other
 * "No honor among thieves"
 *
 * Revision 6.15  2001/03/09 04:36:09  develop
 * added check for spam in yell.
 *
 * Revision 6.14  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

#include "../include/mordb.h"
#include "mextern.h"

/**********************************************************************/
/*				yell				      */
/**********************************************************************/

/* This function allows a player to yell something that will be heard */
/* not only in his room, but also in all rooms adjacent to him.  In   */
/* the adjacent rooms, however, people will not know who yelled.      */

int yell(creature *ply_ptr, cmd	*cmnd )
{
	room		*rom_ptr;
	xtag		*xp;
	char		str[300];
	int			fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	cmnd->fullstr[255]=0;
	clean_str(cmnd->fullstr, 1 );

	if(strlen(cmnd->fullstr) < 1) {
		output(fd, "Yell what?\n");
		return(0);
	}

		if(F_ISSET(ply_ptr, PSILNC)) {
			output_wc(fd, "Your voice is too weak to yell.\n", SILENCECOLOR);
			return(0);
		}

		/* spam check */
		if ( check_for_spam( ply_ptr ) )
		{
			return(0);
		}


		F_CLR(ply_ptr, PHIDDN);
		if(F_ISSET(ply_ptr, PLECHO)){
			sprintf(g_buffer,"You yell, \"%s!\"\n", cmnd->fullstr);
			output_wc(fd, g_buffer, ECHOCOLOR );
		}
		else
			output(fd, "Ok.\n");

		sprintf(g_buffer, "%%M yells, \"%s!\"", cmnd->fullstr);
		broadcast_rom(fd, rom_ptr->rom_num, g_buffer, m1arg(ply_ptr));

		sprintf(str, "Someone yells, \"%s!\"", cmnd->fullstr);

		xp = rom_ptr->first_ext;
		while(xp) {
			if(is_rom_loaded(xp->ext->room))
				broadcast_rom(fd, xp->ext->room, str, NULL);
			xp = xp->next_tag;
		}

		return(0);

	}

	/**********************************************************************/
	/*				go				      */
	/**********************************************************************/

	/* This function allows a player to go through an exit, other than one */
	/* of the 6 cardinal directions.				       */

	int go( creature *ply_ptr, cmd *cmnd)
	{
		room		*rom_ptr, *old_rom_ptr;
		creature	*crt_ptr;
		ctag		*cp, *temp;
		xtag		*xp;
		exit_		*ext_ptr = NULL;
		time_t		i, s, t;
		int		fd, old_rom_num, fall, dmg, n,x=0;
		char		strAction[50];
		int		ordinal;

		rom_ptr = ply_ptr->parent_rom;
		fd = ply_ptr->fd;

		/* was this an ordinal direction? */
		/* have to look for go and enter */
		/* I know, funky code */

		/* TODO look at this code, is may be makign a wrong assumption */

		ordinal = !((tolower(cmnd->fullstr[0]) == 'g' 
			||	(tolower(cmnd->fullstr[0]) == 'e' 
			&& tolower(cmnd->fullstr[1]) == 'n') ));

		if(cmnd->num < 2) {
			output(fd, "Go where?\n");
			return(0);
		}

		if ( ordinal )
		{
			xp = rom_ptr->first_ext;
			while(xp) {
				if(!strcmp(xp->ext->name, cmnd->str[1]) && 
					!F_ISSET(xp->ext,XNOSEE))
				{
					/* found it */
					ext_ptr = xp->ext;
					break;
				}
				xp = xp->next_tag;
			}

			if(ext_ptr == NULL ) {
				output(fd, "You can't go that way.\n");
				return(0);
			}
		}
		else
		{
			ext_ptr = find_ext(ply_ptr, rom_ptr->first_ext, 
				   cmnd->str[1], cmnd->val[1]);
		}

		if(!ext_ptr) {
			output(fd, "I don't see that exit.\n");
			return(0);
		}

		if(F_ISSET(ext_ptr, XLOCKD)) {
			if ( ply_ptr->class >= CARETAKER )
			{
				output(fd, "Locked door passed.\n");
			}
			else
			{
				output(fd, "It's locked.\n");
				return(0);
			}
		}
		else if(F_ISSET(ext_ptr, XCLOSD)) {
			if ( ply_ptr->class >= CARETAKER )
			{
				output(fd, "Closed door passed.\n");
			}
			else
			{
				output(fd, "You have to open it first.\n");
				return(0);
			}
		}

		if(F_ISSET(ext_ptr, XUNIQP)) {
			if ( ply_ptr->class >= CARETAKER ) {
				output(fd, "Unique player exit passed.\n");
			}
			else {
				x = check_ply_ext(ply_ptr->name, ext_ptr);
				if(!x) {
					output(fd, "You may not go there.\n");
					return(0);
				}
			}
		}
		if(F_ISSET(ext_ptr, XFLYSP) && !F_ISSET(ply_ptr, PFLYSP)) {
			if ( ply_ptr->class >= CARETAKER )
			{
				output(fd, "Fly exit passed.\n");
			}
			else
			{
				output(fd, "You must fly to get there.\n");
				return(0);
			}
		}

		if(F_ISSET(ext_ptr, XPLSEL)) {
			if ( !F_ISSET(ext_ptr, XPLSEL+ply_ptr->class) && ply_ptr->class < BUILDER) 
			{
				if ( ply_ptr->class >= CARETAKER )
				{
					output(fd, "Class selective exit passed.\n");
				}
				else
				{
					output(fd, "Your class prevents you from going there.\n");
					return(0);
				}
			}
		}

	    t = Time%24L;
		if(F_ISSET(ext_ptr, XNGHTO) && (t>6 && t < 20)) 
		{
			if ( ply_ptr->class >= CARETAKER )
			{
				output(fd, "Night only exit passed.\n");
			}
			else
			{
			output(fd, "That exit is not open during the day.\n");
			return(0);
			}
	    }          

		if(F_ISSET(ext_ptr, XDAYON) && (t<6 || t > 20)) 
		{
			if ( ply_ptr->class >= CARETAKER )
			{
				output(fd, "Night only exit passed.\n");
			}
			else
			{
			output(fd, "That exit is closed for the night.\n");
			return(0);
			}
	    }          
	 
	    if(F_ISSET(ext_ptr,XPGUAR)){
		cp = rom_ptr->first_mon;
		while(cp) {
		    if(F_ISSET(cp->crt, MPGUAR)) {
					if(!F_ISSET(ply_ptr, PINVIS) && ply_ptr->class < BUILDER) {
						mprint(fd, "%M blocks your exit.\n", m1arg(cp->crt));
						return(0);
			}
					if(F_ISSET(cp->crt, MDINVI) && ply_ptr->class < BUILDER) {
						mprint(fd, "%M blocks your exit.\n", m1arg(cp->crt));
						return(0);
			}
				}	
		    cp = cp->next_tag;
		}
	    }         

		if(F_ISSET(ext_ptr, XPLDGK)) {
			if ( ply_ptr->class >= CARETAKER )
			{
				output(fd, "Pledged exit passed.\n");
			}
			else
			{

				if (!F_ISSET(ply_ptr, PPLDGK))
				{
				output(fd, "You do not have proper guild authorization to go there.\n");
				return(0);
				}
				else if (exitcheck_guild(ext_ptr) !=  check_guild(ply_ptr)){
				output(fd, "You do not have proper guild authorization to go there.\n");
				return(0);
				}
			}
		}

		if(F_ISSET(ext_ptr, XNAKED) && weight_ply(ply_ptr)) 
		{
			if ( ply_ptr->class >= CARETAKER )
			{
				output(fd, "Naked exit passed.\n");
			}
			else
			{
				output(fd, "You cannot bring anything through that exit.\n");
				return(0);
			}
		}
	 
		if(F_ISSET(ext_ptr, XFEMAL) && F_ISSET(ply_ptr, PMALES))
		{
			if ( ply_ptr->class >= CARETAKER )
			{
				output(fd, "Female only exit passed.\n");
			}
			else
			{
				output(fd, "Sorry, only females are allowed to go there.\n");
				return(0); 
			}
		}

		if(F_ISSET(ext_ptr, XMALES) && !F_ISSET(ply_ptr, PMALES))
		{
			if ( ply_ptr->class >= CARETAKER )
			{
				output(fd, "Male only exit passed.\n");
			}
			else
			{
				output(fd, "Sorry, only males are allowed to go there.\n");
				return(0); 
			}
		}           

		if((F_ISSET(ext_ptr, XCLIMB) || F_ISSET(ext_ptr, XREPEL)) &&
		   !F_ISSET(ply_ptr, PLEVIT)) {
			if ( ply_ptr->class >= CARETAKER )
			{
				output(fd, "Climb only exit passed.\n");
			}
			else
			{

				fall = (F_ISSET(ext_ptr, XDCLIM) ? 50:0) + 50 - 
					   fall_ply(ply_ptr);

				if(mrand(1,100) < fall) {
					dmg = mrand(5, 15+fall/10);
					if(ply_ptr->hpcur <= dmg){
						   output(fd, "You fell to your death.\n");
							   ply_ptr->hpcur=0;
							   broadcast_rom(fd, ply_ptr->rom_num, "%M died from the fall.\n", 
								   m1arg(ply_ptr));
					   die(ply_ptr, ply_ptr);
					   return(0);
					}

					ply_ptr->hpcur -= dmg;
					sprintf(g_buffer, 
						"You fell and hurt yourself for %d damage.\n",
						dmg);
					output(fd, g_buffer );
					broadcast_rom(fd, ply_ptr->rom_num, 
						"%M fell down.", m1arg(ply_ptr));

					if(ply_ptr->hpcur < 1) { 
						output(fd, "You fell to your death.\n");
						die(ply_ptr, ply_ptr); 
					}  
					if(F_ISSET(ext_ptr, XCLIMB))
						return(0);
				}
			}
		}

		i = LT(ply_ptr, LT_ATTCK);
		s = LT(ply_ptr, LT_SPELL);
		t = time(0);

	/*
		if(t < i || t < s) {
			int p;
			p = MAX(i, s);
			please_wait(fd, p-t);
			return(0);
		}
	*/

		/* bypass for CTs and DMs */
		if ( ply_ptr->class < CARETAKER )
		{
			/* check for speed move */
			if(ply_ptr->lasttime[LT_MOVED].ltime == t) {
				if(++ply_ptr->lasttime[LT_MOVED].misc > 3) {
						please_wait(fd, 1);
						return(0);
				}
			}
			else {
					ply_ptr->lasttime[LT_MOVED].ltime = t;
					ply_ptr->lasttime[LT_MOVED].misc = 1;
			}

			/* fix walking bug here */
			if(ply_ptr->lasttime[LT_ATTCK].ltime+3 > t) {
					please_wait(fd, 3-t+ply_ptr->lasttime[LT_ATTCK].ltime);
					return(0);
			}
			/* and here */
			else if(ply_ptr->lasttime[LT_SPELL].ltime+3 > t) {
					please_wait(fd, 3-t+ply_ptr->lasttime[LT_SPELL].ltime);
					return(0);
			}
		}


		F_CLR(ply_ptr, PHIDDN);

		cp = rom_ptr->first_mon;
		while(cp) {
			if(F_ISSET(cp->crt, MBLOCK) 
				&& is_enm_crt(ply_ptr->name, cp->crt) 
				&& !F_ISSET(ply_ptr, PINVIS) 
				&& ply_ptr->class < BUILDER) {
				mprint(fd, "%M blocks your exit.\n", m1arg(cp->crt));
				return(0);
			}
			cp = cp->next_tag;
		}

		if(!F_ISSET(rom_ptr, RPTRAK))
			strcpy(rom_ptr->track, ext_ptr->name);

		old_rom_num = rom_ptr->rom_num;
		old_rom_ptr = rom_ptr;

		if ( load_rom(ext_ptr->room, &rom_ptr) < 0 )
		{
			output(fd, "Off map in that direction.\n");
			return(0);
		}

		n = count_vis_ply(rom_ptr);

		/* bypass all room restrictions for immortals */
		if ( ply_ptr->class < BUILDER )
		{
			if(rom_ptr->lolevel > ply_ptr->level ) {
				sprintf(g_buffer, "You must be at least level %d to go that way.\n",
					  rom_ptr->lolevel);
				output(fd, g_buffer );
				return(0);
			}
			else if(ply_ptr->level > rom_ptr->hilevel && rom_ptr->hilevel ) {
				sprintf(g_buffer, "Only players under level %d may go that way.\n",
					  rom_ptr->hilevel+1);
				output(fd, g_buffer);
				return(0);
			}
			else if((F_ISSET(rom_ptr, RONEPL) && n > 0) ||
				(F_ISSET(rom_ptr, RTWOPL) && n > 1) ||
				(F_ISSET(rom_ptr, RTHREE) && n > 2)) {
				output(fd, "That room is full.\n");
				return(0);
			}
		}



		if(ply_ptr->class == DM && !F_ISSET(ply_ptr, PDMINV))
		{

			strcpy(strAction, "wanders to the");
		}
		else if ( !ordinal &&
			(strcmp(ext_ptr->name, "up") && strcmp(ext_ptr->name, "down") 
			&& strcmp(ext_ptr->name, "out") ) )
		{
			/* we got here with a go x command */
			strcpy(strAction, "went to the");
		}
		else
		{
			/* we came from the move command */
			strcpy(strAction, "leaves");
		}

		if(ply_ptr->class == DM && !F_ISSET(ply_ptr, PDMINV))
		{
			sprintf(g_buffer, "%%M %s %s.", 
				      strAction, ext_ptr->name);
			broadcast_rom(fd, old_rom_ptr->rom_num, g_buffer, 
				      m1arg(ply_ptr));
		}
		else if ( F_ISSET(ply_ptr, PDMINV) &&  F_ISSET(ply_ptr, PALIAS)) {
			del_crt_rom(Ply[ply_ptr->fd].extr->alias_crt, ply_ptr->parent_rom);
		sprintf(g_buffer,"%%M just wandered to the %s.", ext_ptr->name);
		broadcast_rom(ply_ptr->fd, ply_ptr->rom_num,
				g_buffer, m1arg(Ply[ply_ptr->fd].extr->alias_crt));
		add_crt_rom(Ply[ply_ptr->fd].extr->alias_crt, rom_ptr, 1);
		}
		else if(!F_ISSET(ply_ptr, PDMINV)) {
			sprintf(g_buffer, "%%M %s %s.", 
				strAction, ext_ptr->name);
			broadcast_rom(fd, old_rom_ptr->rom_num, g_buffer, 
				      m1arg(ply_ptr));
		}


		del_ply_rom(ply_ptr, old_rom_ptr);
		add_ply_rom(ply_ptr, rom_ptr);

		cp = ply_ptr->first_fol;
		while(cp) {
			if(cp->crt->rom_num == old_rom_num && cp->crt->type != MONSTER) {
				go(cp->crt, cmnd);
			}

			if(cp->crt->type == MONSTER) {

				if(F_ISSET(cp->crt, MDMFOL)) {  /* already handled this above */
					cp = cp->next_tag;
					continue;
				}
				/* must be a conjured... */
				if(F_ISSET(rom_ptr, RNOLEA)) {
					output(fd, "Your conjure is dissipating.\n");
					cp->crt->lasttime[LT_CONJU].interval = 0;
				}
				del_crt_rom(cp->crt, old_rom_ptr);
				sprintf(g_buffer, "%%M just went to the %s.", ext_ptr->name);
				broadcast_rom(fd, old_rom_ptr->rom_num, g_buffer, m1arg(cp->crt));
				add_crt_rom(cp->crt, rom_ptr, 1);
	#ifndef MOBS_ALWAYS_ACTIVE
				add_active(cp->crt); 
	#endif
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
				/* follow level 1 chars */
				if ( ply_ptr->level < 2 )
				{
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

				/* Protect the newbie areas from aggro-dragging */
				if(F_ISSET(cp->crt,MAGGRE) && F_ISSET(ext_ptr, XPGUAR)) {
					cp = cp->next_tag;
					continue;
				}

				mprint(fd, "%M followed you.\n", m1arg(cp->crt));
				broadcast_rom(fd, old_rom_num, "%M follows %m.",
					      m2args(cp->crt, ply_ptr));
				temp = cp->next_tag;
				crt_ptr = cp->crt;

				/* prevent players from continuously creating new monsters */
				if(F_ISSET(crt_ptr, MPERMT))
					die_perm_crt(crt_ptr);

				del_crt_rom(crt_ptr, old_rom_ptr);
				add_crt_rom(crt_ptr, rom_ptr, 1);
	#ifndef MOBS_ALWAYS_ACTIVE
				add_active(crt_ptr); 
	#endif
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

	int openexit( creature *ply_ptr, cmd *cmnd )
	{
		exit_	*ext_ptr;
		room	*rom_ptr;
		int	fd;

		rom_ptr = ply_ptr->parent_rom;
		fd = ply_ptr->fd;

		if(cmnd->num < 2) {
			output(fd, "Open what?\n");
			return(0);
		}

		ext_ptr = find_ext(ply_ptr, rom_ptr->first_ext,
				   cmnd->str[1], cmnd->val[1]);

		if(!ext_ptr) {
			output(fd, "I don't see that exit.\n");
			return(0);
		}

		if(F_ISSET(ext_ptr, XLOCKD)) {
			output(fd, "It's locked.\n");
			return(0);
		}

		if(!F_ISSET(ext_ptr, XCLOSD)) {
			output(fd, "It's already open.\n");
			return(0);
		}

		F_CLR(ply_ptr, PHIDDN);

		F_CLR(ext_ptr, XCLOSD);
		ext_ptr->ltime.ltime = time(0);

		sprintf(g_buffer, "You open the %s.\n", ext_ptr->name);
		output(fd, g_buffer );
		sprintf(g_buffer, "%%M opens the %s.", ext_ptr->name);
		broadcast_rom(fd, ply_ptr->rom_num, g_buffer, m1arg(ply_ptr));

		return(0);

	}

	/**********************************************************************/
	/*				closeexit			      */
	/**********************************************************************/

	/* This function allows a player to close a door, if the door is not */
	/* already closed, and if indeed it is a door.			     */

	int closeexit( creature *ply_ptr, cmd *cmnd )
	{
		exit_	*ext_ptr;
		room	*rom_ptr;
		int	fd;

		rom_ptr = ply_ptr->parent_rom;
		fd = ply_ptr->fd;

		if(cmnd->num < 2) {
			output(fd, "Close what?\n");
			return(0);
		}

		ext_ptr = find_ext(ply_ptr, rom_ptr->first_ext,
				   cmnd->str[1], cmnd->val[1]);

		if(!ext_ptr) {
			output(fd, "I don't see that exit.\n");
			return(0);
		}

		if(F_ISSET(ext_ptr, XCLOSD)) {
			output(fd, "It's already closed.\n");
			return(0);
		}

		if(!F_ISSET(ext_ptr, XCLOSS)) {
			output(fd, "You can't close that.\n");
			return(0);
		}

		F_CLR(ply_ptr, PHIDDN);

		F_SET(ext_ptr, XCLOSD);

		sprintf(g_buffer, "You close the %s.\n", ext_ptr->name);
		output(fd, g_buffer );
		sprintf(g_buffer, "%%M closes the %s.", ext_ptr->name);
		broadcast_rom(fd, ply_ptr->rom_num, g_buffer, m1arg(ply_ptr));

		return(0);

	}

	/**********************************************************************/
	/*				unlock				      */
	/**********************************************************************/

	/* This function allows a player to unlock a door if he has the correct */
	/* key, and it is locked.						*/

	int unlock( creature *ply_ptr, cmd *cmnd )
	{
		room	*rom_ptr;
		object	*obj_ptr;
		exit_	*ext_ptr;
		int	fd;

		fd = ply_ptr->fd;
		rom_ptr = ply_ptr->parent_rom;
		
		if(cmnd->num < 2) {
			output(fd, "Unlock what?\n");
			return(0);
		}

		ext_ptr = find_ext(ply_ptr, rom_ptr->first_ext,
				   cmnd->str[1], cmnd->val[1]);

		if(!ext_ptr) {
			output(fd, "Unlock what?\n");
			return(0);
		}

		if(!F_ISSET(ext_ptr, XLOCKD)) {
			output(fd, "It's not locked.\n");
			return(0);
		}

		if(cmnd->num < 3) {
			output(fd, "Unlock it with what?\n");
			return(0);
		}

		obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
				   cmnd->str[2], cmnd->val[2]);

		if(!obj_ptr) {
			output(fd, "You don't have that.\n");
			return(0);
		}

		if(obj_ptr->type != KEY) {
			output(fd, "That's not a key.\n");
			return(0);
		}

		if(obj_ptr->shotscur < 1) {
			mprint(fd, "%I is broken.\n", m1arg(obj_ptr));
			return(0);
		}

		if(obj_ptr->ndice != ext_ptr->key) {
			output(fd, "Wrong key.\n");
			return(0);
		}

		F_CLR(ply_ptr, PHIDDN);

		F_CLR(ext_ptr, XLOCKD);
		ext_ptr->ltime.ltime = time(0);
		obj_ptr->shotscur--;

		if(obj_ptr->use_output[0])
		{
			output_nl(fd, obj_ptr->use_output);
		}
		else
			output(fd, "Click.\n");

		sprintf(g_buffer, "%%M unlocks the %s.", ext_ptr->name);
		broadcast_rom(fd, ply_ptr->rom_num, g_buffer, m1arg(ply_ptr));

		return(0);

	}

	/**********************************************************************/
	/*				lock				      */
	/**********************************************************************/

	/* This function allows a player to lock a door with the correct key. */

	int lock( creature *ply_ptr, cmd *cmnd )
	{
		room	*rom_ptr;
		object	*obj_ptr;
		exit_	*ext_ptr;
		int	fd;

		fd = ply_ptr->fd;
		rom_ptr = ply_ptr->parent_rom;
		
		if(cmnd->num < 2) {
			output(fd, "Lock what?\n");
			return(0);
		}

		ext_ptr = find_ext(ply_ptr, rom_ptr->first_ext,
				   cmnd->str[1], cmnd->val[1]);

		if(!ext_ptr) {
			output(fd, "Lock what?\n");
			return(0);
		}

		if(F_ISSET(ext_ptr, XLOCKD)) {
			output(fd, "It's already locked.\n");
			return(0);
		}

		if(cmnd->num < 3) {
			output(fd, "Lock it with what?\n");
			return(0);
		}

		obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
				   cmnd->str[2], cmnd->val[2]);

		if(!obj_ptr) {
			output(fd, "You don't have that.\n");
			return(0);
		}

		if(obj_ptr->type != KEY) {
			mprint(fd, "%I is not a key.\n", m1arg(obj_ptr));
			return(0);
		}

		if(!F_ISSET(ext_ptr, XLOCKS)) {
			output(fd, "You can't lock it.\n");
			return(0);
		}

		if(!F_ISSET(ext_ptr, XCLOSD)) {
			output(fd, "You have to close it first.\n");
			return(0);
		}

		if(obj_ptr->shotscur < 1) {
			mprint(fd, "%I is broken.\n", m1arg(obj_ptr));
			return(0);
		}

		if(obj_ptr->ndice != ext_ptr->key) {
			output(fd, "Wrong key.\n");
			return(0);
		}

		F_CLR(ply_ptr, PHIDDN);

		F_SET(ext_ptr, XLOCKD);

		output(fd, "Click.\n");
		sprintf(g_buffer, "%%M locks the %s.",ext_ptr->name);
		broadcast_rom(fd, ply_ptr->rom_num, g_buffer, m1arg(ply_ptr));

		return(0);

	}

	/**********************************************************************/
	/*				picklock			      */
	/**********************************************************************/

	/* This function is called when a thief or assassin attempts to pick a */
	/* lock.  If the lock is pickable, there is a chance (depending on the */
	/* player's level) that the lock will be picked.		       */

	int picklock(creature *ply_ptr, cmd *cmnd )
	{
		room	*rom_ptr;
		exit_	*ext_ptr;
		time_t	i, t;
		int	fd, chance;

		fd = ply_ptr->fd;
		rom_ptr = ply_ptr->parent_rom;

		if(ply_ptr->class != THIEF && ply_ptr->class != ASSASSIN && ply_ptr->class < BUILDER) {
			output(fd, "Only thieves and assassins may pick locks.\n");
			return(0);
		}

		if(cmnd->num < 2) {
			output(fd, "Pick what?\n");
			return(0);
		}
		ext_ptr = find_ext(ply_ptr, rom_ptr->first_ext,
				   cmnd->str[1], cmnd->val[1]);

		if(!ext_ptr) {
			output(fd, "I don't see that here.\n");
			return(0);
		}
		if(F_ISSET(ply_ptr, PBLIND)) {
			output_wc(fd, "You can't see to pick that lock.\n", BLINDCOLOR);
			return(0);
		}
		if(!F_ISSET(ext_ptr, XLOCKD)) {
			output(fd, "It's not locked.\n");
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
		chance += bonus[(int)ply_ptr->dexterity]*2;


		if(F_ISSET(ext_ptr, XUNPCK))
			chance = 0;

		if ( ply_ptr->class == DM )
			chance = 100;

		sprintf(g_buffer, "%%M attempts to pick the %s.", 
			      ext_ptr->name);
		broadcast_rom(fd, ply_ptr->rom_num, g_buffer, m1arg(ply_ptr));

		if(mrand(1,100) <= chance) {
			output(fd, "You successfully picked the lock.\n");
			F_CLR(ext_ptr, XLOCKD);
			sprintf(g_buffer, "%s succeeded.",
				      F_ISSET(ply_ptr, PMALES) ? "He":"She");
			broadcast_rom(fd, ply_ptr->rom_num, g_buffer, NULL );
		}
		else
			output(fd, "You failed.\n");

		return(0);

	}

	/**********************************************************************/
	/*				steal
	*/
	/**********************************************************************/

	/* This function allows a player to steal an item from a monster within  */
	/* the game.  Only thieves may steal, and the monster must               */
	/* have the object which is being stolen within his inventory.		 */

	int steal(creature *ply_ptr, cmd *cmnd )
	{
		creature	*crt_ptr;
		room		*rom_ptr;
		object		*obj_ptr;
		time_t		i, t;
		int		fd, chance;

		fd = ply_ptr->fd;
		rom_ptr = ply_ptr->parent_rom;


		if(ply_ptr->class != THIEF && ply_ptr->class < BUILDER) 
		{
			output(fd, "Only thieves may steal.\n");
			return(0);
		}

		if(cmnd->num < 2) 
		{
			output(fd, "Steal what?\n");
			return(0);
		}

		if(cmnd->num < 3) 
		{
			output(fd, "Steal what from whom?\n");
			return(0);
		}

		F_CLR(ply_ptr, PHIDDN);

		i = LT(ply_ptr, LT_STEAL);
		t = time(0);

		if ( ply_ptr->class < DM )
		{
			if(t < i) 
			{
				please_wait(fd, i-t);
				return(0);
			}
		}

		if(F_ISSET(ply_ptr, PINVIS)) 
		{
			F_CLR(ply_ptr, PINVIS);
			output(fd, "Your invisibility fades.\n");
			broadcast_rom(fd, ply_ptr->rom_num, "%M fades into view.", m1arg(ply_ptr));
		}

		ply_ptr->lasttime[LT_STEAL].ltime = t;
		ply_ptr->lasttime[LT_STEAL].interval = 5;

		if(cmnd->str[1][0] == '$') {
			steal_money(ply_ptr, cmnd);
			return(0);
		}

		crt_ptr = find_crt_in_rom(ply_ptr, rom_ptr, cmnd->str[2], cmnd->val[2], MON_FIRST);


		if(!crt_ptr || crt_ptr == ply_ptr || F_ISSET(ply_ptr, PBLIND)) 
		{
			output(fd, "I don't see that here.\n");
			return(0);
		}


		if(crt_ptr->type == MONSTER) 
		{

			if(!is_crt_killable(crt_ptr, ply_ptr)) 
			{
				return(0);
			}

			if(is_enm_crt(ply_ptr->name, crt_ptr)) 
			{
				sprintf(g_buffer, "Not while %s's attacking you.\n", F_ISSET(crt_ptr, MMALES) ? "he":"she");
				output(fd, g_buffer );
				return(0);
			}

		}
		
		else 
		{
		   if(ply_ptr->class < DM) {
			if ( !pkill_allowed(ply_ptr, crt_ptr) )
				return(0);
		   }

		}
		if(F_ISSET(ply_ptr, PBLIND)) 
		{
			output_wc(fd, "How do you do that?  You're blind.\n", BLINDCOLOR);
			return(0);
		}

		/* caculate only basic chance here */
		/* do special cases after chuck code */
		chance = theft_chance(ply_ptr, crt_ptr);

		if ( ply_ptr->class < IMMORTAL )
		{

			/* to prevent level 1 thieves from checking to see */
			/* if a perm is carrying - give them no info either way */
			if ( crt_ptr->type == MONSTER && chance <= 0 )
			{
				sprintf(g_buffer, "%s chuckles at the attempt.\n", F_ISSET(crt_ptr, MMALES) ? "He":"She");
				output(fd, g_buffer);
				return(0);
			}


			/* to prevent high level thieves from terrorizing newbies */
			/* cannot steal from chars more than 10 levels below you */
			if ( crt_ptr->type == PLAYER && ((ply_ptr->level - crt_ptr->level) > 10) ) 
			{
				output(fd, "Why don't you pick on someone your own level?\n");
				return(0);
			}
		}

		obj_ptr = find_obj(ply_ptr, crt_ptr->first_obj, cmnd->str[1], cmnd->val[1]);
		
		if(!obj_ptr) 
		{
			sprintf(g_buffer, "%s doesn't have that.\n", F_ISSET(crt_ptr, MMALES) ? "He":"She");
			output(fd, g_buffer );
			return(0);
		}

		/* check for weight problem */
		if(weight_ply(ply_ptr) + weight_obj(obj_ptr) > max_weight(ply_ptr)) 
		{
			output(fd, "You can't carry anymore.\n");
			chance = 0;
		}
		

		/* special case checks for chance */
		/* had to move this after the chuckle check */
		/* so low thieves could not tell if carrying a quest object */
		if ( obj_ptr->questnum) 
			chance = 0;

		if(crt_ptr->type == MONSTER && F_ISSET(crt_ptr, MUNSTL))
			chance = 0;
		
		if(ply_ptr->class == DM)
			chance = 100;

		if(mrand(1,100) <= chance) 
		{
			output(fd, "You succeeded.\n");
			del_obj_crt(obj_ptr, crt_ptr);
			add_obj_crt(obj_ptr, ply_ptr);
			F_SET(obj_ptr, OTHEFT);
			
			if((F_ISSET(ply_ptr, PPLDGK) && F_ISSET(crt_ptr, PPLDGK)) && ((ply_ptr->level -1) <= crt_ptr->level)) 
				F_SET(obj_ptr, OSTOLE); 
	/*		
			if ((F_ISSET(ply_ptr, PPLDGK) && crt_ptr-> type == MONSTER && F_ISSET(crt_ptr, MSTOLE))
			{
				crt_ptr->experience += ((obj_ptr->value * 0.1) * (obj_ptr->shotscur/ obj_ptr->shotsmax) / 2 );
			}
	*/
			if(crt_ptr->type == PLAYER) 
			{
				ply_ptr->lasttime[LT_PLYKL].ltime = t;
				ply_ptr->lasttime[LT_PLYKL].interval = (long)mrand(7,10) * 86400L;
			}

		}

		else 
		{
			sprintf(g_buffer, "You failed.\n%s", crt_ptr->type == MONSTER ? "You are attacked.\n" : "");
			output(fd, g_buffer );
			broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, "%M tried to steal from %m.", m2args(ply_ptr, crt_ptr));

			if(crt_ptr->type == PLAYER)
				mprint(crt_ptr->fd, "%M tried to steal %1i from you.\n", m2args(ply_ptr, obj_ptr));
			else
				add_enm_crt(ply_ptr->name, crt_ptr);
		}

		return(0);

	}

/*	steal_money() */
/* this allows players to steal money from each other */
/*	it should only be called from the steal() function */
int steal_money(creature *ply_ptr, cmd *cmnd)
	{
		creature *crt_ptr;
		room	*rom_ptr;
		int	fd, chance;
		long	amt;

		rom_ptr = ply_ptr->parent_rom;
		fd = ply_ptr->fd;

		amt = atol(&cmnd->str[1][1]);
		if(amt < 1) {
			output(fd, "How does one do that?\n");
			return(0);
		}

		if(F_ISSET(ply_ptr, PBLIND)) {
			output_wc(fd, "How do you do that?  You're blind.\n", BLINDCOLOR);
			return(0);
		}

		crt_ptr = find_crt_in_rom(ply_ptr, rom_ptr, cmnd->str[2], cmnd->val[2], PLY_FIRST);

		if(!crt_ptr || crt_ptr == ply_ptr) {
			output(fd, "I don't see that here.\n");
			return(0);
		}

		if(crt_ptr->type != PLAYER) {
			output(fd, "You can't steal that from them.\n");
			return(0);
		}

		if(ply_ptr->class < DM) {
                        if ( !pkill_allowed(ply_ptr, crt_ptr) )
                                return(0);
                }

		/* check to see if crt_ptr is on ply_ptr->extra's stolen list */
		if(is_stolen_crt(crt_ptr->name, ply_ptr)) {
				output(fd, "You've taken enough from them already.\n");
				return(0);
		}

		if(amt > crt_ptr->gold) {
			output(fd, "You failed.\n");
                        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, "%M tried to steal from %m.", m2args(ply_ptr, crt_ptr));
                        sprintf(g_buffer, "%s tried to steal gold from you.\n", ply_ptr->name);
                        output(crt_ptr->fd, g_buffer);
			return(0);
		}

		/* calculate the chance of success */
		chance = theft_chance(ply_ptr, crt_ptr);
		if(ply_ptr->class == DM)
			chance = 100;

		if(mrand(1,100) <= chance) {
		/* calculate the amount of gold that can be stolen */
			amt=amt/(mrand(1,4));
		/* transferring gold from victim to thief */
			ply_ptr->gold += amt;
			crt_ptr->gold -= amt;
		/* add crt_ptr to ply_ptr stolen list */
       		        add_stolen_crt(crt_ptr, ply_ptr);
			
			sprintf(g_buffer, "You steal %ld gold from %s.\n", amt, crt_ptr->name);
			output(fd, g_buffer);
		}
		else {
			output(fd, "You failed.\n");
			broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, "%M tried to steal from %m.", m2args(ply_ptr, crt_ptr));
			sprintf(g_buffer, "%s tried to steal gold from you.\n", ply_ptr->name);
			output(crt_ptr->fd, g_buffer);
		}

	if(SAVEONDROP) {
               if(ply_ptr->type == PLAYER) {
                        F_SET(ply_ptr, PSAVES);
                        save_ply(ply_ptr);
                        F_CLR(ply_ptr, PSAVES);
                }
  	}
return(0);
}

/* this function returns the chance that ply_ptr would steal from crt_ptr */
int theft_chance(creature *ply_ptr, creature *crt_ptr)
{

	int	chance;

	chance = (ply_ptr->class == THIEF) ? 4*ply_ptr->level :
                         3*ply_ptr->level;
        chance += bonus[(int)ply_ptr->dexterity]*3;
        if(crt_ptr->level > ply_ptr->level)
                chance -= 15*(crt_ptr->level - ply_ptr->level);
        chance = MIN(chance, 65);

	return(chance);
}

