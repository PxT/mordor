/*
 * COMMAND2.C:
 *
 *	Addition user command routines.
 *
 *	Copyright (C) 1991, 1992, 1993, 1997 Brooke Paul 
 *
 * $Id: command2.c,v 6.22 2001/07/29 16:54:38 develop Exp $
 *
 * $Log: command2.c,v $
 * Revision 6.22  2001/07/29 16:54:38  develop
 * upped price on shops to 1mil
 *
 * Revision 6.23  2001/07/29 16:41:37  develop
 * added price to logging of sales
 *
 * Revision 6.22  2001/07/29 16:35:40  develop
 * logging added to actions related to player shops
 *
 * Revision 6.21  2001/07/25 23:10:52  develop
 * OTHEFT infects containers
 *
 * Revision 6.20  2001/07/25 02:55:04  develop
 * fixes for thieves dropping stolen items
 * fixes for gold dropping by pkills
 *
 * Revision 6.20  2001/07/24 01:36:23  develop
 * *** empty log message ***
 *
 * Revision 6.19  2001/07/17 19:25:11  develop
 * *** empty log message ***
 *
 * Revision 6.18  2001/04/17 18:56:35  develop
 * *** empty log message ***
 *
 * Revision 6.17  2001/03/09 05:14:07  develop
 * changes to weight checking
 *
 * Revision 6.16  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

#include "../include/mordb.h"
#include "../include/version.h"
#include "mextern.h"

/**********************************************************************/
/*				look				      */
/**********************************************************************/

/* This function is called when a player tries to look at the room he */
/* is in, or at an object in the room or in his inventory.            */

int look(creature *ply_ptr, cmd	*cmnd )
{
	room		*rom_ptr;
	object		*obj_ptr;
	creature	*crt_ptr;
	char		str[2048];
	int		fd, n, match=0;

	if(cmnd->num < 2) {
		display_rom(ply_ptr, ply_ptr->parent_rom);
		return(0);
	}

	fd = ply_ptr->fd;
	if(F_ISSET(ply_ptr, PBLIND)) {
		ANSI(fd, AM_BOLD);
		ANSI(fd, BLINDCOLOR);
		output(fd, "You're blind!\n");
		ANSI(fd, MAINTEXTCOLOR);
		ANSI(fd, AM_NORMAL);
		return(0);
	}
	rom_ptr = ply_ptr->parent_rom;

	obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj, cmnd->str[1], 
			   cmnd->val[1]);

	if(!obj_ptr || !cmnd->val[1]) {
		for(n=0; n<MAXWEAR; n++) {
			if(!ply_ptr->ready[n])
				continue;
			if(EQUAL(ply_ptr->ready[n], cmnd->str[1]))
				match++;
			else continue;
			if(cmnd->val[1] == match || !cmnd->val[1]) {
				obj_ptr = ply_ptr->ready[n];
				break;
			}
		}
	}

	if(!obj_ptr)
		obj_ptr = find_obj(ply_ptr, rom_ptr->first_obj,
				   cmnd->str[1], cmnd->val[1]);

	if(obj_ptr) {

		if(obj_ptr->special) {
			n = special_obj(ply_ptr, cmnd, SP_MAPSC);
			if(n != -2) return(MAX(n, 0));
		}

		if(obj_ptr->description[0])
		{
			sprintf(g_buffer, "%s\n", obj_ptr->description);
			output(fd, g_buffer);
		}
		else
			output(fd, "You see nothing special about it.\n");

		if(obj_ptr->type <= ARMOR)
		{
			sprintf(g_buffer, "%s", obj_condition(obj_ptr));
                        output(fd, g_buffer);
		}

		if(F_ISSET(ply_ptr, PKNOWA)) {
			if(F_ISSET(obj_ptr, OGOODO))
				output(fd, "It has a blue aura.\n");
			if(F_ISSET(obj_ptr, OEVILO))
				output(fd, "It has a red aura.\n");
		}

		if(F_ISSET(obj_ptr, OCONTN)) {
			strcpy(str, "It contains: ");
			n = list_obj(&str[13], ply_ptr, obj_ptr->first_obj);
			if(n)
			{
				strcat( str, ".\n");
				output(fd, str);
			}
		}

		if(obj_ptr->type <= MISSILE) {
			mprint(fd, "%I is a ", m1arg(obj_ptr));
			switch(obj_ptr->type) {
			case SHARP: output(fd, "sharp"); break;
			case THRUST: output(fd, "thrusting"); break;
			case POLE: output(fd, "pole"); break;
			case BLUNT: output(fd, "blunt"); break;
			case MISSILE: output(fd, "missile"); break;
			}
			output(fd, " weapon.\n");
		}

		if((F_ISSET(obj_ptr, ODRUDT)  && obj_ptr->shotscur < 0) || (F_ISSET(obj_ptr, OALCHT)  && obj_ptr->shotscur < 0)) {
		   output(fd, "It looks to be a workable object.\n");
		   return(0);
		}
		   
		if(obj_ptr->type <= MISSILE || obj_ptr->type == ARMOR ||
		   obj_ptr->type == LIGHTSOURCE || obj_ptr->type == WAND ||
		   obj_ptr->type == KEY) { 
			
		//  This if added with condition descriptions to show when keys/wands
		//  are broken

		    if(obj_ptr->type == KEY || obj_ptr->type == WAND)
		    {
			if(obj_ptr->shotscur < 1)
				output(fd, "It's broken or used up.\n");
			else if(obj_ptr->shotscur <= obj_ptr->shotsmax/10)
				output(fd, "It looks about ready to break.\n");
		    }
			if(F_ISSET(obj_ptr, OTMPEN) &&(ply_ptr->class == ALCHEMIST || (ply_ptr->class ==MAGE && F_ISSET(ply_ptr, PDMAGI)) || ply_ptr->class >= BUILDER)) 
				output(fd, "It is weakly enchanted.\n");
		}

		return(0);
	}

	crt_ptr = find_crt_in_rom(ply_ptr, rom_ptr, cmnd->str[1],
			   cmnd->val[1], MON_FIRST);

	if(crt_ptr)
	{
		if ( crt_ptr->type == MONSTER) {

			mprint(fd, "You see %1m.\n", m1arg(crt_ptr));
			if(crt_ptr->description[0])
			{
				output_nl(fd, crt_ptr->description );
			}
			else
				output(fd, "There is nothing special about it.\n");
			
			sprintf(g_buffer,"%s%s", F_ISSET(crt_ptr, MMALES) ? "He":"She", 
				crt_condition(crt_ptr));
			output(fd, g_buffer);

			if(F_ISSET(ply_ptr, PKNOWA)) {
				sprintf(g_buffer, "%s ",
					F_ISSET(crt_ptr, MMALES) ? "He":"She");
				output(fd, g_buffer);

				if(crt_ptr->alignment < 0)
					output(fd, "has a red aura.\n");
				else if(crt_ptr->alignment == 0)
					output(fd, "has a grey aura.\n");
				else output(fd, "has a blue aura.\n");
			}
	
	/*		if(crt_ptr->hpcur < (crt_ptr->hpmax*3)/10)
			{
				sprintf(g_buffer, "%s has some nasty wounds.\n",
					  F_ISSET(crt_ptr, MMALES) ? "He":"She");
				output(fd, g_buffer);
			}   */
			
			if(is_enm_crt(ply_ptr->name, crt_ptr)) 
			{
				sprintf(g_buffer, "%s looks very angry at you.\n",
					  F_ISSET(crt_ptr, MMALES) ? "He":"She");
				output(fd, g_buffer);
			}
			if(crt_ptr->first_enm) {
				if(!strcmp(crt_ptr->first_enm->enemy, ply_ptr->name))
				{
					sprintf(g_buffer, "%s is attacking you.\n",
						  F_ISSET(crt_ptr, MMALES) ? "He":"She");
					output(fd, g_buffer);
				}
				else
				{
					sprintf(g_buffer, "%s is attacking %s.\n",
						  F_ISSET(crt_ptr, MMALES) ? "He":"She",
						  crt_ptr->first_enm->enemy);
					output(fd, g_buffer);
				}
			}
			consider(ply_ptr, crt_ptr);
			equip_list(fd, crt_ptr);
			return(0);
		}
		else
		{
			sprintf(g_buffer, "You see %s the %s %s.\n", crt_ptr->name,
				  get_race_adjective(crt_ptr->race), title_ply(crt_ptr));
			output(fd, g_buffer);
			if(crt_ptr->description[0])
			{
				output_nl(fd, crt_ptr->description);
			}
			
			if(crt_ptr->class < IMMORTAL) {
			   sprintf(g_buffer,"%s%s", F_ISSET(crt_ptr, MMALES) ? "He":"She",
                                crt_condition(crt_ptr));
                           output(fd, g_buffer);
			}
			
			if(F_ISSET(ply_ptr, PKNOWA) && crt_ptr->class < IMMORTAL) {
				sprintf(g_buffer, "%s ",
					F_ISSET(crt_ptr, MMALES) ? "He":"She");
				output(fd, g_buffer);
				if(crt_ptr->alignment < -100)
					output(fd, "has a red aura.\n");
				else if(crt_ptr->alignment < 101)
					output(fd, "has a grey aura.\n");
				else 
					output(fd, "has a blue aura.\n");
			}
		
		// removed with the addition of condition descriptions AK
		/*	if(crt_ptr->hpcur < (crt_ptr->hpmax*3)/10)
			{
				sprintf(g_buffer, "%s has some nasty wounds.\n",
					  F_ISSET(crt_ptr, PMALES) ? "He":"She");
				output(fd, g_buffer);
			}		*/
			
			equip_list(fd, crt_ptr);
			return(0);
		}
	}
	else
		output(fd, "You don't see that here.\n");

	return(0);

}

/*********************************************************************/
/*			crt_condition				     */
/*********************************************************************/
/* This function returns the correct description for the condition   */
/* of the creature that is passed as the only argument                 */

char *crt_condition(creature *crt_ptr)
{
	if(crt_ptr->hpcur == crt_ptr->hpmax)
		return ply_cond_desc[0];
	else if (crt_ptr->hpcur > (crt_ptr->hpmax * 9 / 10))
		return ply_cond_desc[1];
	else if (crt_ptr->hpcur > (crt_ptr->hpmax * 8 / 10))
		return ply_cond_desc[2];
	else if (crt_ptr->hpcur > (crt_ptr->hpmax * 7 / 10))
		return ply_cond_desc[3];
	else if (crt_ptr->hpcur > (crt_ptr->hpmax * 6 / 10))
		return ply_cond_desc[4];
	else if (crt_ptr->hpcur > (crt_ptr->hpmax * 5 / 10))
		return ply_cond_desc[5];
	else if (crt_ptr->hpcur > (crt_ptr->hpmax * 4 / 10))
		return ply_cond_desc[6];
	else if (crt_ptr->hpcur > (crt_ptr->hpmax * 3 / 10))
		return ply_cond_desc[7];
	else if (crt_ptr->hpcur > (crt_ptr->hpmax * 2 / 10))
		return ply_cond_desc[8];
	else 
		return ply_cond_desc[9];
}
/**********************************************************************/
/*                          *obj_condition                            */
/**********************************************************************/

char *obj_condition(object *obj_ptr)
{
	if(obj_ptr->shotscur == obj_ptr->shotsmax)
		return obj_cond_desc[0];
	else if(obj_ptr->shotscur > (obj_ptr->shotsmax * 9 / 10))
		 return obj_cond_desc[1];
        else if(obj_ptr->shotscur > (obj_ptr->shotsmax * 8 / 10))
		 return obj_cond_desc[2];
        else if(obj_ptr->shotscur > (obj_ptr->shotsmax * 7 / 10))
		 return obj_cond_desc[3];
        else if(obj_ptr->shotscur > (obj_ptr->shotsmax * 6 / 10))
		 return obj_cond_desc[4];
        else if(obj_ptr->shotscur > (obj_ptr->shotsmax * 5 / 10))
		 return obj_cond_desc[5];
        else if(obj_ptr->shotscur > (obj_ptr->shotsmax * 4 / 10))
		 return obj_cond_desc[6];
        else if(obj_ptr->shotscur > (obj_ptr->shotsmax * 3 / 10))
		 return obj_cond_desc[7];
        else if(obj_ptr->shotscur > (obj_ptr->shotsmax * 2 / 10))
		 return obj_cond_desc[8];
        else if(obj_ptr->shotscur > 0)
		 return obj_cond_desc[9];
	else 
		return obj_cond_desc[10];
}

                
/**********************************************************************/
/*				move				      */
/**********************************************************************/

/* This function takes the player using the socket descriptor specified */
/* in the first parameter, and attempts to move him in one of the six   */
/* cardinal directions (n,s,e,w,u,d) if possible.			*/

int move(creature *ply_ptr, cmd *cmnd )
{
	char		tempstr[10];
	char		*str;

	/* find the ordinal direction */
	str = cmnd->str[0];
		
	if(!strcmp(str, "sw") || !strncmp(str, "southw", 6))
		strcpy(tempstr, "southwest");
	else if(!strcmp(str, "nw") || !strncmp(str, "northw", 6))
		strcpy(tempstr, "northwest");
	else if(!strcmp(str, "se") || !strncmp(str, "southe", 6))
		strcpy(tempstr, "southeast");
	else if(!strcmp(str, "ne") || !strncmp(str, "northe", 6))
		strcpy(tempstr, "northeast");
	else {
		switch(str[0]) {
		case 'n': strcpy(tempstr, "north"); break;
		case 's': strcpy(tempstr, "south"); break;
		case 'e': strcpy(tempstr, "east");  break;
		case 'w': strcpy(tempstr, "west");  break;
		case 'u': strcpy(tempstr, "up");    break;
		case 'd': strcpy(tempstr, "down");  break;
		case 'o':
		case 'l': strcpy(tempstr, "out");   break;
		}
	}

	/* fake a go north type thing */
	cmnd->num = 2;
	strcpy( cmnd->str[1], tempstr );
	cmnd->val[1] = 1;

	return( go( ply_ptr, cmnd) );

}


/**********************************************************************/
/*				say				      */
/**********************************************************************/

/* This function allows the player specified by the socket descriptor */
/* in the first parameter to say something to all the other people in */
/* the room.							      */

int say( creature *ply_ptr, cmd *cmnd )
{
	room	*rom_ptr;
	int		fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;


	cmnd->fullstr[255] = 0;
	clean_str( cmnd->fullstr, 1);

	if(F_ISSET(ply_ptr, PSILNC)) {
		output_wc(fd, "Your lips move but no sound comes forth.\n", SILENCECOLOR);
		return(0);
	}

	/* spam check */
	if ( check_for_spam( ply_ptr ) )
	{
		return(0);
	}

	if(strlen(cmnd->fullstr) < 1) {
		output(fd, "Say what?\n");
		return(0);
	}

	F_CLR(ply_ptr, PHIDDN);
	if(F_ISSET(ply_ptr, PLECHO))
	{
		sprintf(g_buffer, "You say, \"%s\"\n", cmnd->fullstr);
		output_wc(fd, g_buffer, ECHOCOLOR );
	}
	else
		output(fd, "Ok.\n");

	sprintf(g_buffer, "%%M says, \"%s.\"", cmnd->fullstr);
	broadcast_rom(fd, rom_ptr->rom_num, g_buffer, m1arg(ply_ptr));

	return(0);

}



/**********************************************************************/
/*				get				      */
/**********************************************************************/

/* This function allows players to get things lying on the floor or   */
/* inside another object.					      */

int get(creature *ply_ptr, cmd *cmnd )
{
	room		*rom_ptr;
	object		*obj_ptr, *cnt_ptr;
	ctag		*cp;
	int		fd, n, match=0;

	fd = ply_ptr->fd;

	if(cmnd->num < 2) {
		output(fd, "Get what?\n");
		return(0);
	}

	rom_ptr = ply_ptr->parent_rom;
	F_CLR(ply_ptr, PHIDDN);

	if(cmnd->num == 2) {

		cp = rom_ptr->first_mon;
		while(cp) {
			if(F_ISSET(cp->crt, MGUARD))
				break;
			cp = cp->next_tag;
		}

		if(cp && ply_ptr->class < BUILDER) {
			mprint(fd, "%M won't let you take anything.\n", m1arg(cp->crt));
			return(0);
		}
		if(F_ISSET(ply_ptr, PBLIND)) {
			output_wc(fd, "You can't see to do that!\n", BLINDCOLOR);
			return(0);
		}
		if(!strcmp(cmnd->str[1], "all")) {
			if(F_ISSET(rom_ptr, RPOWND)) {
				output(fd, "You can't get all here.\n");
				return(0);	
			}
			else {
				get_all_rom(ply_ptr);
				return(0);
			}
		}

		obj_ptr = find_obj(ply_ptr, rom_ptr->first_obj,
				   cmnd->str[1], cmnd->val[1]);

		if(!obj_ptr) {
			output(fd, "That isn't here.\n");
			return(0);
		}

		if(F_ISSET(obj_ptr, ONOTAK) || F_ISSET(obj_ptr, OSCENE)) {
			output(fd, "You can't take that!\n");
			return(0);
		}

		if( weight_ply(ply_ptr) + weight_obj(obj_ptr) > max_weight(ply_ptr)) {
			output(fd, "You can't carry anymore.\n");
			return(0);
		}

		if(obj_ptr->questnum && Q_ISSET(ply_ptr, obj_ptr->questnum-1)) {
			output(fd, "You may not take that. You have already fulfilled that quest.\n");
			return(0);
		}

		if(F_ISSET(obj_ptr, OTEMPP)) {
			F_CLR(obj_ptr, OPERM2);
			F_CLR(obj_ptr, OTEMPP);
		}

		if(F_ISSET(obj_ptr, OPERMT))
			get_perm_obj(obj_ptr);

		F_CLR(obj_ptr, OHIDDN);
		del_obj_rom(obj_ptr, rom_ptr);

		if(F_ISSET(rom_ptr, RPOWND)) {
			sprintf(g_buffer, "%s removed from sale by %s in shop (%d).\n", obj_ptr->name, ply_ptr->name, rom_ptr->rom_num);                                                   slog(g_buffer);
		}
		
		if(F_ISSET(obj_ptr, OSTOLE))
			F_CLR(obj_ptr, OSTOLE);
		
		mprint(fd, "You get %1i.\n", m1arg(obj_ptr));
		if(obj_ptr->questnum && obj_ptr->shotscur>0) 
		{
			output(fd, "Quest fulfilled!  Don't drop it.\n");
			output(fd, "You won't be able to pick it up again.\n");			
			Q_SET(ply_ptr, obj_ptr->questnum-1);
			ply_ptr->experience += get_quest_exp(obj_ptr->questnum);
			sprintf(g_buffer, "%ld experience granted.\n",
				get_quest_exp(obj_ptr->questnum));
			output(fd, g_buffer );
			add_prof(ply_ptr, get_quest_exp(obj_ptr->questnum));
		}
		broadcast_rom(fd, rom_ptr->rom_num, "%M gets %1i.",
			      m2args(ply_ptr, obj_ptr));

		if(obj_ptr->type == MONEY) {
			ply_ptr->gold += obj_ptr->value;
			free_obj(obj_ptr);

			sprintf(g_buffer, "You now have %ld gold pieces.\n", 
				ply_ptr->gold);
			output(fd, g_buffer);
		}
		else
			add_obj_crt(obj_ptr, ply_ptr);
		return(0);
	}

	else {

		cnt_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
				   cmnd->str[2], cmnd->val[2]);

		if(!cnt_ptr)
			cnt_ptr = find_obj(ply_ptr, rom_ptr->first_obj,
					   cmnd->str[2], cmnd->val[2]);

		if(!cnt_ptr || !cmnd->val[2]) {
			for(n=0; n<MAXWEAR; n++) {
				if(!ply_ptr->ready[n]) continue;
				if(EQUAL(ply_ptr->ready[n], cmnd->str[2]))
					match++;
				else continue;
				if(match == cmnd->val[2] || !cmnd->val[2]) {
					cnt_ptr = ply_ptr->ready[n];
					break;
				}
			}
		}

		if(!cnt_ptr) {
			output(fd, "That isn't here.\n");
			return(0);
		}

		if(!F_ISSET(cnt_ptr, OCONTN)) {
			output(fd, "That isn't a container.\n");
			return(0);
		}

		if(!strcmp(cmnd->str[1], "all")) {
			get_all_obj(ply_ptr, cnt_ptr);
			return(0);
		}

		obj_ptr = find_obj(ply_ptr, cnt_ptr->first_obj,
				   cmnd->str[1], cmnd->val[1]);

		if(!obj_ptr) {
			output(fd, "That isn't in there.\n");
			return(0);
		}

		if(weight_ply(ply_ptr) + weight_obj(obj_ptr) >
		   max_weight(ply_ptr) && cnt_ptr->parent_rom) {
			output(fd, "You can't carry anymore.\n");
			return(0);
		}

		if(F_ISSET(obj_ptr, OPERMT))
			get_perm_obj(obj_ptr);

		cnt_ptr->shotscur--;
		del_obj_obj(obj_ptr, cnt_ptr);
		mprint(fd, "You get %1i from %1i.\n", m2args(obj_ptr, cnt_ptr));
		broadcast_rom(fd, rom_ptr->rom_num, "%M gets %1i from %1i.",
			      m3args(ply_ptr, obj_ptr, cnt_ptr));

		if(obj_ptr->type == MONEY) {
			ply_ptr->gold += obj_ptr->value;
			free_obj(obj_ptr);

			sprintf(g_buffer, "You now have %ld gold pieces.\n",
				ply_ptr->gold);
			output(fd, g_buffer);
		}
		else
			add_obj_crt(obj_ptr, ply_ptr);
	
		return(0);
	}

}

/**********************************************************************/
/*				get_all_rom			      */
/**********************************************************************/

/* This function will cause the player pointed to by the first parameter */
/* to get everything he is able to see in the room.			 */

void get_all_rom( creature *ply_ptr )
{
	room	*rom_ptr;
	object	*obj_ptr, *last_obj;
	otag	*op;
	char	str[2048], *str2;
	int 	fd, n = 1, found = 0, heavy = 0, dogoldmsg = 0;

	last_obj = 0; str[0] = 0;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	op = rom_ptr->first_obj;
	while(op && strlen(str)< 2040) {
		if(!F_ISSET(op->obj, OSCENE) &&
		   !F_ISSET(op->obj, ONOTAK) && !F_ISSET(op->obj, OHIDDN) && 
		   (F_ISSET(ply_ptr, PDINVI) ? 1:!F_ISSET(op->obj, OINVIS))) {
			found++;
			obj_ptr = op->obj;
			op = op->next_tag;
			if( weight_ply(ply_ptr) + weight_obj(obj_ptr) > max_weight(ply_ptr) ) {
				heavy++;
				continue;
			}
			
			if(F_ISSET(obj_ptr, OSTOLE))
				F_CLR(obj_ptr, OSTOLE);

			if(obj_ptr->questnum && 
			   Q_ISSET(ply_ptr, obj_ptr->questnum-1)) {
				heavy++;
				continue;
			}
/*
			if(obj_ptr->strength && 
			    obj_ptr->strength > ply_ptr->strength) {
				heavy++;
				continue;
			}
*/
			if(F_ISSET(obj_ptr, OTEMPP)) {
				F_CLR(obj_ptr, OPERM2);
				F_CLR(obj_ptr, OTEMPP);
			}
			if(F_ISSET(obj_ptr, OPERMT))
				get_perm_obj(obj_ptr);
			F_CLR(obj_ptr, OHIDDN);
			if(obj_ptr->questnum) {
				output(fd,"Quest fulfilled!  Don't drop it.\n");
				output(fd,"You won't be able to pick it up again.\n");		
				Q_SET(ply_ptr, obj_ptr->questnum-1);
				ply_ptr->experience += 
					get_quest_exp(obj_ptr->questnum);
				sprintf(g_buffer, "%ld experience granted.\n",
					get_quest_exp(obj_ptr->questnum));
				output(fd, g_buffer);
				add_prof(ply_ptr, get_quest_exp(obj_ptr->questnum));
			}
			del_obj_rom(obj_ptr, rom_ptr);
			if(last_obj && !strcmp(last_obj->name, obj_ptr->name) &&
			   last_obj->adjustment == obj_ptr->adjustment)
				n++;
			else if(last_obj) {
				str2 = obj_str(ply_ptr, last_obj, n, 0);
                if(strlen(str2)+strlen(str) < 2040)
				{
					strcat(str, str2);
					strcat(str, ", ");
					n=1;
				}
			}
			if(obj_ptr->type == MONEY) {
		   		str2 = obj_str(ply_ptr, obj_ptr, 1, 0);
				if(strlen(str2)+strlen(str) < 2040){
					strcat(str, str2);
					strcat(str, ", ");
				}
				ply_ptr->gold += obj_ptr->value;
				free_obj(obj_ptr);
				last_obj = 0;
				dogoldmsg = 1;
			}
			else {
				add_obj_crt(obj_ptr, ply_ptr);
				last_obj = obj_ptr;
			}
		}
		else
			op = op->next_tag;
	}

	if(found && last_obj) {
		str2 = obj_str(ply_ptr, last_obj, n, 0);
                if(strlen(str2)+strlen(str) < 2040)
			strcat(str, str2);
	}
	else if(!found) {
		output(fd, "There's nothing here.\n");
		return;
	}

	if(dogoldmsg && !last_obj)
		str[strlen(str)-2] = 0;

	if(heavy) {
		output(fd, "You weren't able to carry everything.\n");
		if(heavy == found) return;
	}

	if(!strlen(str)) return;

	sprintf(g_buffer, "%%M gets %s.", str);
	broadcast_rom(fd, rom_ptr->rom_num, g_buffer, m1arg(ply_ptr));

	sprintf(g_buffer, "You get %s.\n", str);
	output(fd, g_buffer);
	if(dogoldmsg)
	{
		sprintf(g_buffer, "You now have %ld gold pieces.\n",
			ply_ptr->gold);
		output(fd, g_buffer);
	}

}

/**********************************************************************/
/*				get_all_obj			      */
/**********************************************************************/

/* This function allows a player to get the entire contents from a 	  */
/* container object.  The player is pointed to by the first parameter and */
/* the container by the second.						  */

void get_all_obj(creature *ply_ptr, object *cnt_ptr )
{
	room	*rom_ptr;
	object	*obj_ptr, *last_obj;
	otag	*op;
	char	str[2048], *str2;
	int	fd, n = 1, found = 0, heavy = 0;

	last_obj = 0; str[0] = 0;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	op = cnt_ptr->first_obj;
	while(op) {
		if(!F_ISSET(op->obj, OSCENE) &&
		   !F_ISSET(op->obj, ONOTAK) && !F_ISSET(op->obj, OHIDDN) && 
		   (F_ISSET(ply_ptr, PDINVI) ? 1:!F_ISSET(op->obj, OINVIS))) {
			found++;
			obj_ptr = op->obj;
			op = op->next_tag;
			if(weight_ply(ply_ptr) + weight_obj(obj_ptr) >
			   max_weight(ply_ptr)) {
				heavy++;
				continue;
			}
			if(F_ISSET(obj_ptr, OTEMPP)) {
				F_CLR(obj_ptr, OPERM2);
				F_CLR(obj_ptr, OTEMPP);
			}
			if(F_ISSET(obj_ptr, OPERMT))
				get_perm_obj(obj_ptr);
			cnt_ptr->shotscur--;
			del_obj_obj(obj_ptr, cnt_ptr);
			if(last_obj && !strcmp(last_obj->name, obj_ptr->name) &&
				last_obj->adjustment == obj_ptr->adjustment)
				n++;
			else if(last_obj) {
			str2 = obj_str(ply_ptr, last_obj, n, 0);
                                if(strlen(str2)+strlen(str) < 2040){
				strcat(str, str2);
				strcat(str, ", ");
				n = 1;
				}
			}
			if(obj_ptr->type == MONEY) {
				ply_ptr->gold += obj_ptr->value;
				free_obj(obj_ptr);
				last_obj = 0;
				sprintf(g_buffer, "You now have %ld gold pieces.\n",
					ply_ptr->gold);
				output(fd, g_buffer);
			}
			else {
				add_obj_crt(obj_ptr, ply_ptr);
				last_obj = obj_ptr;
			}
		}
		else
			op = op->next_tag;
	}

	if(found && last_obj) {
	str2 = obj_str(ply_ptr, obj_ptr, n, 0);
           if(strlen(str2)+strlen(str) < 2040)
		strcat(str, str2);
	}
	else if(!found) {
		output(fd, "There's nothing in it.\n");
		return;
	}

	if(heavy) {
		output(fd, "You weren't able to carry everything.\n");
		if(heavy == found) return;
	}

	if(!strlen(str)) return;

	sprintf(g_buffer, "%%M gets %s from %%1i.",str);
	broadcast_rom(fd, rom_ptr->rom_num, g_buffer,
		      m2args(ply_ptr, cnt_ptr));

	sprintf(g_buffer, "You get %s from %%1i.\n", str);
	mprint(fd, g_buffer, m1arg(cnt_ptr));

}

/**********************************************************************/
/*				get_perm_obj			      */
/**********************************************************************/

/* This function is called whenever someone picks up a permanent item */
/* from a room.  The item's room-permanent flag is cleared, and the   */
/* inventory-permanent flag is set.  Also, the room's permanent	      */
/* time for that item is updated.				      */

void get_perm_obj( object *obj_ptr )
{
	object	*temp_obj;
	room	*rom_ptr;
	time_t	t;
	int	i;

	t = time(0);

	F_SET(obj_ptr, OPERM2);
	F_CLR(obj_ptr, OPERMT);

	rom_ptr = obj_ptr->parent_rom;
	if(!rom_ptr) return;

	if(F_ISSET(rom_ptr, RPOWND)) {
		resave_rom(rom_ptr->rom_num);
		return;
	}
	
	for(i=0; i<10; i++) {
		if(!rom_ptr->perm_obj[i].misc) continue;
		if(rom_ptr->perm_obj[i].ltime + rom_ptr->perm_obj[i].interval >
		   t) continue;
		if(load_obj(rom_ptr->perm_obj[i].misc, &temp_obj) < 0)
			continue;
		if(!strcmp(temp_obj->name, obj_ptr->name)) {
			rom_ptr->perm_obj[i].ltime = t;
			free_obj(temp_obj);
			break;
		}
		free_obj(temp_obj);
	}
}

int about( creature *ply_ptr, cmd *cmnd )
{
	int		fd;

	fd = ply_ptr->fd;

	display_credits(fd);

	return(0);

}

/**********************************************************************/
/*				inventory			      */
/**********************************************************************/

/* This function outputs the contents of a player's inventory.        */

int inventory( creature *ply_ptr, cmd *cmnd )
{
	otag	*op;
	char	*str;
	char	*str2;
	char	*str_temp;
	int		m, n, fd;
	size_t	block_size = 2048;
	size_t	str_size ;

	str_size = block_size;
	str = malloc( str_size );

	fd = ply_ptr->fd;

	if(F_ISSET(ply_ptr, PBLIND)) 
	{
		output_wc(fd, "You're blind as a bat...how can you do that?\n", BLINDCOLOR);
		return(0);
	}		

	op = ply_ptr->first_obj; 
	n=0; 
	strcpy(str, "You have: ");
	
	if(!op) 
	{
		strcat(str, "nothing.\n");
		output(fd, str);
	}
	else
	{
		while(op) 
		{
			if(F_ISSET(ply_ptr, PDINVI) ? 1:!F_ISSET(op->obj, OINVIS)) 
			{
				m=1;
				
				while(op->next_tag) 
				{
					if(!strcmp(op->next_tag->obj->name, op->obj->name) &&
						op->next_tag->obj->adjustment == op->obj->adjustment &&
						(F_ISSET(ply_ptr, PDINVI) ? 1:!F_ISSET(op->next_tag->obj, OINVIS))) 
					{
						m++;
						op = op->next_tag;
					}
					
					else
						break;
				}
				
				str2 = obj_str(ply_ptr, op->obj, m, 0);
				
				if(F_ISSET(op->obj, OSTOLE))
					strcat(str2, "(STOLEN)");

				if( (strlen(str2) + strlen(str)) < (str_size - 8) ) 
				{
					strcat(str, str2);
					strcat(str, ", ");
					n++;
				}
				
				else
				{
					/* increase the str size */
					str_size += block_size;
					str_temp = realloc( str, str_size );
					
					if ( str_temp )
					{
						/* it worked */
						str = str_temp;
						strcat(str, str2);
						strcat(str, ", ");
						n++;
					}
					
					else
					{
						/* TODO: print a warning or somethign the realloc failed */
						/* str was untouched if realloc failed so no need */
						/* to reset pointers */

						/* size did not change so adjust back */
						str_size -= block_size;
					}

				}
			}
			op = op->next_tag;
		}

		if(n) 
		{
			str[strlen(str)-2] = 0;
			strcat( str, ".\n");
			output(fd, str );
		}
		
		else
		{
			/* has invis items but non visible */
			strcat(str, "nothing.\n");
			output(fd, str);
		}
	}

	/* dont forget to clean up */
	if ( str )
	{
		free(str);
	}

	return(0);

}

/**********************************************************************/
/*				drop			   	      */
/**********************************************************************/

/* This function allows the player pointed to by the first parameter */
/* to drop an object in the room at which he is located.             */

int drop(creature *ply_ptr, cmd	*cmnd )
{
	room		*rom_ptr;
	object		*obj_ptr, *cnt_ptr;
	int		fd, n, match=0, gold=0;
	
	fd = ply_ptr->fd;
	
	if(cmnd->num < 2) {
		output(fd, "Drop what?\n");
		return(0);
	}
	
	if(F_ISSET(ply_ptr, PALIAS))
               ply_ptr=Ply[fd].extr->alias_crt;
	
	rom_ptr = ply_ptr->parent_rom;
	F_CLR(ply_ptr, PHIDDN);

	if(cmnd->num == 2) {

		if(!strcmp(cmnd->str[1], "all")) {
			if(!F_ISSET(rom_ptr, RPOWND)) {
				drop_all_rom(ply_ptr);
			}
			else {
				output(fd, "You may not drop all here.\n");
				return(0);
			}
if(SAVEONDROP)
			if(ply_ptr->type == PLAYER) save_ply(ply_ptr);
			return(0);
		}
		if(cmnd->str[1][0] == '$' && !F_ISSET(rom_ptr, RDUMPR) && !F_ISSET(rom_ptr, RPOWND)) {
                 gold = atoi(cmnd->str[1]+1);
                  if(gold > 0 && gold <= ply_ptr->gold) {
		     load_obj(0, &obj_ptr);
                     sprintf(obj_ptr->name, "%d gold coins", gold);
                     obj_ptr->value = gold;
                     ply_ptr->gold -= gold;
                   } 
		   else { 
                     output(fd, "You don't have that much!\n");
                     return(0);
                   }
		}
		else 
		obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj, cmnd->str[1], cmnd->val[1]);
		
		if(!obj_ptr) {
			output(fd, "You don't have that.\n");
			return(0);
		}

		if(F_ISSET(rom_ptr, RPOWND)) {
			if(obj_ptr->questnum) {
				output(fd , "You may not offer to sell quest items.\n");
				return(0);
			}
			if(obj_ptr->type == CONTAINER) {
				output(fd, "You may not offer to sell that.\n");
				return(0);
			}
		}

		if(!gold)
			del_obj_crt(obj_ptr, ply_ptr);
		
		mprint(fd, "You drop %1i.\n", m1arg(obj_ptr));
		broadcast_rom(fd, rom_ptr->rom_num, "%M dropped %1i.", m2args(ply_ptr, obj_ptr));
		if(!F_ISSET(rom_ptr, RDUMPR))
		{
			add_obj_rom(obj_ptr, rom_ptr);

			if(F_ISSET(rom_ptr, RPOWND)) {
				if(!F_ISSET(obj_ptr, ONOSEL)) {
					obj_ptr->value = find_obj_value(obj_ptr);
				}
				if(!obj_ptr->value || obj_ptr->shotscur < 1) {
					del_obj_rom(obj_ptr, rom_ptr);
					free_obj(obj_ptr);
		                        ply_ptr->gold += 5;
               			         sprintf(g_buffer, "Thanks for recycling.\nYou have %-ld gold.\n", ply_ptr->gold);
                        		output(fd, g_buffer);
					if(SAVEONDROP)
				                if(ply_ptr->type == PLAYER)
                        				save_ply( ply_ptr);
					return(0);
				}
				F_SET(obj_ptr, OPERMT);
				sprintf(g_buffer, "%s offered for sale at %ld by %s from shop (%d).\n", obj_ptr->name, obj_ptr->value, ply_ptr->name, rom_ptr->rom_num);
				slog(g_buffer);
				resave_rom(rom_ptr->rom_num);
			}
			
			if(F_ISSET(obj_ptr, OSTOLE)) {
				sprintf(g_buffer, "The %s is no longer marked as being stolen.\n", obj_ptr->name);
				output(fd, g_buffer);
			}
		}	
		else {
			free_obj(obj_ptr);
			ply_ptr->gold += 5;
			sprintf(g_buffer, "Thanks for recycling.\nYou have %-ld gold.\n", ply_ptr->gold);
			output(fd, g_buffer);
		}
if(SAVEONDROP)
		if(ply_ptr->type == PLAYER) 
			save_ply( ply_ptr);
		return(0);
	}

	else {

		cnt_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
				   cmnd->str[2], cmnd->val[2]);

		if(!cnt_ptr)
			cnt_ptr = find_obj(ply_ptr, rom_ptr->first_obj,
					    cmnd->str[2], cmnd->val[2]);

		if(!cnt_ptr || !cmnd->val[2]) {
			for(n=0; n<MAXWEAR; n++) {
				if(!ply_ptr->ready[n]) continue;
				if(EQUAL(ply_ptr->ready[n], cmnd->str[2]))
					match++;
				else continue;
				if(match == cmnd->val[2] || !cmnd->val[2]) {
					cnt_ptr = ply_ptr->ready[n];
					break;
				}
			}
		}
	
		if(!cnt_ptr) {
			output(fd, "You don't see that here.\n");
			return(0);
		}

		if(!F_ISSET(cnt_ptr, OCONTN)) {
			output(fd, "That isn't a container.\n");
			return(0);
		}

		if(!strcmp(cmnd->str[1], "all")) {
			drop_all_obj(ply_ptr, cnt_ptr);
if(SAVEONDROP)
			if(ply_ptr->type == PLAYER) save_ply(ply_ptr);
			return(0);
		}

		obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
				   cmnd->str[1], cmnd->val[1]);

		if(!obj_ptr) {
			output(fd, "You don't have that.\n");
			return(0);
		}

		if(obj_ptr == cnt_ptr) {
			output(fd, "You can't put something in itself!\n");
			return(0);
		}

		if(obj_ptr->questnum) {
	               	 output(fd, "You can't do that.\n");
       	        	 return(0);
       		}	

		if(cnt_ptr->shotscur >= cnt_ptr->shotsmax) {
			mprint(fd, "%I can't hold anymore.\n", m1arg(cnt_ptr));
			return(0);
		}

		if(F_ISSET(obj_ptr, OCONTN)) {
			output(fd, "You can't put containers into containers.\n");
			return(0);
		}

                if(F_ISSET(cnt_ptr, OCNDES)) { 
		mprint(fd, "%1i is devoured by %1i!\n", m2args(obj_ptr, cnt_ptr));
		broadcast_rom(fd, rom_ptr->rom_num, "%M put %1i in %1i.",
			      m3args(ply_ptr, obj_ptr, cnt_ptr));
		del_obj_crt(obj_ptr, ply_ptr);
		free(obj_ptr);
			return(0);
		}

		del_obj_crt(obj_ptr, ply_ptr);
		add_obj_obj(obj_ptr, cnt_ptr);
		cnt_ptr->shotscur++;
		
		if(F_ISSET(obj_ptr, OSTOLE))
		{
			F_CLR(obj_ptr, OSTOLE);
			sprintf(g_buffer, "The %s is no longer marked as being stolen.\n", obj_ptr->name);
			output(fd, g_buffer);
		}

		/* trying to hide a stolen item in a bag infects the whole bag */
                if(F_ISSET(obj_ptr, OTHEFT))
                        F_SET(cnt_ptr, OTHEFT);

		
		mprint(fd, "You put %1i in %1i.\n", m2args(obj_ptr, cnt_ptr));
		broadcast_rom(fd, rom_ptr->rom_num, "%M put %1i in %1i.",
			      m3args(ply_ptr, obj_ptr, cnt_ptr));
if(SAVEONDROP)
		if(ply_ptr->type == PLAYER) save_ply(ply_ptr);
		return(0);
	}

}

/**********************************************************************/
/*				drop_all_rom			      */
/**********************************************************************/

/* This function is called when a player wishes to drop his entire    */
/* inventory into the the room.					      */

void drop_all_rom( creature	*ply_ptr )
{
	object	*obj_ptr;
	room	*rom_ptr;
	otag	*op;
	char	str[2048];
	int	fd, found;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;
	/* if(F_ISSET(ply_ptr, PALIAS))
               ply_ptr=Ply[fd].extr->alias_crt; */


	found = list_obj(str, ply_ptr, ply_ptr->first_obj);

	if(!found) {
		output(fd, "You don't have anything.\n");
		return;
	}

	op = ply_ptr->first_obj;
	while(op) {
		if(F_ISSET(ply_ptr, PDINVI) ? 1:!F_ISSET(op->obj, OINVIS)) {
			obj_ptr = op->obj;
			op = op->next_tag;
			
			if(F_ISSET(obj_ptr, OSTOLE))
			{
				F_CLR(obj_ptr, OSTOLE);
				sprintf(g_buffer, "The %s is no longer marked as being stolen.\n", obj_ptr->name);
				output(fd, g_buffer);
			}
			
			del_obj_crt(obj_ptr, ply_ptr);
			if(!F_ISSET(rom_ptr, RDUMPR))
				add_obj_rom(obj_ptr, rom_ptr);
			else {
				free_obj(obj_ptr);
				ply_ptr->gold+=5;
			}
		}
		else
			op = op->next_tag;
	}

	sprintf(g_buffer, "%%M dropped %s.", str);
	broadcast_rom(fd, rom_ptr->rom_num, g_buffer, m1arg(ply_ptr));

	sprintf(g_buffer, "You drop %s.\n", str);
	output(fd, g_buffer);

	if(F_ISSET(rom_ptr, RDUMPR))
	{
		sprintf(g_buffer, 
			"Thanks for recycling.\nYou have %-ld gold.\n",
			ply_ptr->gold);
		output(fd, g_buffer);
	}
}

/**********************************************************************/
/*				drop_all_obj			      */
/**********************************************************************/

/* This function drops all the items in a player's inventory into a	 */
/* container object, if possible.  The player is pointed to by the first */
/* parameter, and the container by the second.				 */

void drop_all_obj(creature *ply_ptr, object	*cnt_ptr )
{
	object	*obj_ptr, *last_obj;
	room	*rom_ptr;
	otag	*op;
	char	str[2048], *str2;
	int	fd, n = 1, found = 0, full = 0;

	fd = ply_ptr->fd;

	last_obj = 0; str[0] = 0;
	rom_ptr = ply_ptr->parent_rom;

	op = ply_ptr->first_obj;
	while(op) {
		if((F_ISSET(ply_ptr, PDINVI) ? 1:!F_ISSET(op->obj, OINVIS)) &&
		   op->obj != cnt_ptr) {
			found++;
			obj_ptr = op->obj;
			op = op->next_tag;
			if(F_ISSET(obj_ptr, OCONTN)) {
				full++;
				continue;
			}
			if(F_ISSET(cnt_ptr, OCNDES))
			{
				full++;
				del_obj_crt(obj_ptr, ply_ptr);
				free(obj_ptr);
				continue;

			}
	
			if(cnt_ptr->shotscur >= cnt_ptr->shotsmax) {
				full++;
				continue;
			}
			cnt_ptr->shotscur++;
			del_obj_crt(obj_ptr, ply_ptr);
			add_obj_obj(obj_ptr, cnt_ptr);
			
			if(F_ISSET(obj_ptr, OSTOLE))
			{
				F_CLR(obj_ptr, OSTOLE);
				sprintf(g_buffer, "The %s is no longer marked as being stolen.\n", obj_ptr->name);
				output(fd, g_buffer);
			}

			if(last_obj && !strcmp(last_obj->name, obj_ptr->name) &&
			   last_obj->adjustment == obj_ptr->adjustment)
				n++;
			else if(last_obj) {
			str2 = obj_str(ply_ptr, last_obj, n, 0);
            if(strlen(str2)+strlen(str) < 2040){
					strcat(str, str2);
					strcat(str, ", ");
					n = 1;
				}
			}
			last_obj = obj_ptr;
		}
		else
			op = op->next_tag;
	}

	if(found && last_obj) {
		str2 = obj_str(ply_ptr, last_obj, n, 0);
                if(strlen(str2)+strlen(str) < 2040)
			strcat(str, str2);
	}
	else {
		output(fd, "You don't have anything to put into it.\n");
		return;
	}

	if(full)
		mprint(fd, "%I couldn't hold everything.\n", m1arg(cnt_ptr));

	if(full == found) 
		return;

	sprintf(g_buffer, "%%M put %s into %%1i.", str);
	broadcast_rom(fd, rom_ptr->rom_num, g_buffer, 
		m2args(ply_ptr, cnt_ptr));

	sprintf(g_buffer, "You put %s into %%1i.\n", str);
	mprint(fd, g_buffer, m1arg(cnt_ptr));

}
