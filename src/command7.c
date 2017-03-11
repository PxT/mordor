/*
 * COMMAND7.C:
 *
 *	Additional user routines.
 *
 *	Copyright (C) 1991, 1992, 1993 Brooke Paul
 *
 * $Id: command7.c,v 6.29 2001/07/31 00:39:57 develop Exp $
 *
 * $Log: command7.c,v $
 * Revision 6.29  2001/07/31 00:39:57  develop
 * fixed error in buy that didn't save_ply when purchase was made
 * and shop owner was offline
 *
 * Revision 6.28  2001/07/29 16:54:38  develop
 * added logging of player shops
 *
 * Revision 6.29  2001/07/29 16:41:37  develop
 * added price to logging of sales
 *
 * Revision 6.28  2001/07/29 16:35:40  develop
 * logging added to actions related to player shops
 *
 * Revision 6.27  2001/07/25 02:55:04  develop
 * fixes for thieves dropping stolen items
 * fixes for gold dropping by pkills
 *
 * Revision 6.27  2001/07/24 01:36:23  develop
 * *** empty log message ***
 *
 * Revision 6.26  2001/07/22 21:10:00  develop
 * appraise again
 *
 * Revision 6.25  2001/07/22 21:05:54  develop
 * another attempt to fix appraise
 *
 * Revision 6.24  2001/07/22 21:01:13  develop
 * fixing problems in appraise
 *
 * Revision 6.23  2001/07/22 19:03:06  develop
 * first run at alllowing thieves to steal gold from other players
 *
 * Revision 6.22  2001/07/21 21:32:58  develop
 * removing kruft
 *
 * Revision 6.21  2001/07/21 17:46:10  develop
 * changed player objects to use value instead of
 * special2.  set ONOSEL & ONOFIX
 *
 * Revision 6.20  2001/07/21 15:39:00  develop
 * changed error msg syntax on appraise
 *
 * Revision 6.19  2001/07/21 15:22:07  develop
 * added new  syntax to appraise to allow setting
 * of values for player run shops
 *
 * Revision 6.18  2001/07/17 19:25:11  develop
 * *** empty log message ***
 *
 * Revision 6.19  2001/07/14 22:55:38  develop
 * *** empty log message ***
 *
 * Revision 6.18  2001/07/14 21:26:44  develop
 * *** empty log message ***
 *
 * Revision 6.17  2001/06/08 15:19:13  develop
 * mod. to flee for conjure
 *
 * Revision 6.16  2001/03/09 05:14:07  develop
 * changes to weight checking
 *
 * Revision 6.15  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

#include "../include/mordb.h"
#include "mextern.h"


/**********************************************************************/
/*				flee				      */
/**********************************************************************/

/* This function allows a player to flee from an enemy.  If successful */
/* the player will drop his readied weapon and run through one of the  */
/* visible exits, losing 10% or 1000 experience, whichever is less.    */

int flee( creature *ply_ptr, cmd *cmnd )
{
	room	*rom_ptr;
	xtag	*xp;
	ctag	*cp;
	char 	found = 0;
	int	fd, n;
	time_t	i, t;
	int     scared = 1;
	int     **scared_of = &Ply[ply_ptr->fd].extr->scared_of;
	int     *scary;
	int     found_non_scary = 0;
	int     found_scary = 0;


	rom_ptr = ply_ptr->parent_rom;
	fd = ply_ptr->fd;

	t = time(0);
	i = MAX(ply_ptr->lasttime[LT_ATTCK].ltime,
	ply_ptr->lasttime[LT_SPELL].ltime) + 4L;

	if(t < i) 
	{
		please_wait(fd, i-t);
		return(0);
	}

	t = Time%24L;
	while(1) 
	{
		xp = rom_ptr->first_ext;
		if(xp) 
		{
			do 
			{
				found=0;
				if(F_ISSET(xp->ext, XCLOSD)) 
					continue;
				if((F_ISSET(xp->ext, XCLIMB) || F_ISSET(xp->ext, XDCLIM)) &&  
					(!F_ISSET(ply_ptr, PLEVIT))) 
					continue; 
				if(F_ISSET(xp->ext, XNAKED) && weight_ply(ply_ptr)) 
					continue;
				if(F_ISSET(xp->ext, XFEMAL) && F_ISSET(ply_ptr, PMALES)) 
					continue;
				if(F_ISSET(xp->ext, XMALES) && !F_ISSET(ply_ptr, PMALES)) 
					continue; 
				if(F_ISSET(xp->ext, XFLYSP) && !F_ISSET(ply_ptr, PFLYSP))
					continue;
				if(!F_ISSET(ply_ptr, PDINVI) && F_ISSET(xp->ext, XINVIS))
					continue;
				if(F_ISSET(xp->ext, XSECRT)) 
					continue;
				if(F_ISSET(xp->ext, XNOSEE)) 
					continue;
				if(F_ISSET(xp->ext, XUNIQP) && !check_ply_ext(ply_ptr->name, xp->ext))
					continue;
				if(F_ISSET(xp->ext, XPLDGK)) {
					if (!F_ISSET(ply_ptr, PPLDGK)) 
						continue;
					else if (exitcheck_guild(xp->ext) != check_guild(ply_ptr)) 
						continue;
				}
				if(F_ISSET(xp->ext, XNGHTO) && (t>6 && t < 20)) 
					continue;
				if(F_ISSET(xp->ext, XDAYON) && (t<6 || t > 20))  
					continue;
				if(F_ISSET(xp->ext, XPLSEL) &&!F_ISSET(xp->ext, XPLSEL+ply_ptr->class)) 
					continue;
				if(F_ISSET(xp->ext,XPGUAR))
				{
					cp = rom_ptr->first_mon;
					while(cp) 
					{
						if(F_ISSET(cp->crt, MPGUAR) && 
							!F_ISSET(ply_ptr, PINVIS) && 
							ply_ptr->class < BUILDER) 
						{
							found = 1;
							break;
						}
						cp = cp->next_tag;
					}
					if (found)
						continue;
				}

				/* check to see if the destination room is scary */
				if(scared && *scared_of) 
				{

					scary = *scared_of;
					while(*scary) 
					{
						if(*scary == xp->ext->room) 
						{
							sprintf(g_buffer, "Scared of going %s!\n", xp->ext->name);
							output(fd, g_buffer);
							found = 1;
							found_scary = 1;
							break;
						}
						scary++;
					}	
					if(found)
						continue;
				}

				found_non_scary = 1;         
				if(mrand(1,100) < (65 + bonus[(int)ply_ptr->dexterity]*5)) 
					break;
			} while( (xp = xp->next_tag) );
		}
		if(xp) 
			break; /* found an exit that is not scary, continue */
		if(found_non_scary) 
			break; /* failed to flee to all the non-scary */
		if(scared && found_scary) 
		{        /* try again not scared */
			scared = 0;
		}
		else 
			break;                   /* tried everything, give up */
	}

	if(xp && F_ISSET(xp->ext,52) && mrand(1,5) < 2)
		xp = 0;
	if(!xp) 
	{
		output(fd, "You failed to escape!\n");
		return(0);
	}

	/* update the scary list */
	scary = *scared_of;
	{
		int room = rom_ptr->rom_num;
		if(scary) 
		{
			int size = 0;
			while(*scary) 
			{
				size++;
				if(*scary == room) 
					break;
				scary++;
			}
			if(!*scary) 
			{
				*scared_of =(int*)realloc(*scared_of, (size+2)*sizeof(int));
				(*scared_of)[size] = room;
				(*scared_of)[size+1] = 0;
			}
		} 
		else 
		{
			*scared_of = (int*)malloc(sizeof(int)*2);
			(*scared_of)[0] = room;
			(*scared_of)[1] = 0;
		}
	}

	if(ply_ptr->ready[WIELD-1] &&
		!F_ISSET(ply_ptr->ready[WIELD-1], OCURSE)) 
	{
		add_obj_rom(ply_ptr->ready[WIELD-1], rom_ptr);
		dequip(ply_ptr,ply_ptr->ready[WIELD-1]);
		ply_ptr->ready[WIELD-1] = 0;
		compute_thaco(ply_ptr);
		compute_ac(ply_ptr);
		output(fd, "You drop your weapon and run like a chicken.\n");
	}
	else
		output(fd, "You run like a chicken.\n");

	F_CLR(ply_ptr, PHIDDN);



	if(!F_ISSET(rom_ptr, RPTRAK))
		strcpy(rom_ptr->track, xp->ext->name);

	sprintf(g_buffer, "%%M flees to the %s.", xp->ext->name);
	broadcast_rom(fd, rom_ptr->rom_num, g_buffer, 
	m1arg(ply_ptr));

	if (ply_ptr->class == PALADIN)
	{
		if (ply_ptr->level > 5) 
		{
			n = ply_ptr->level*15;
			n = MIN(ply_ptr->experience, n);
			sprintf(g_buffer, 
			"You lose %d experience for your cowardly retreat.\n",n);
			output(fd, g_buffer);
			ply_ptr->experience -= n;
			lower_prof(ply_ptr,n);
		}
	}

	if(load_rom(xp->ext->room, &rom_ptr) < 0) {
		output(fd, "You failed to escape.\n");
		return(0);
	}
	

	if(rom_ptr->lolevel > ply_ptr->level || 
		(ply_ptr->level > rom_ptr->hilevel && rom_ptr->hilevel)) 
	{
		output(fd, "You are thrown back by an invisible force.\n");
		broadcast_rom(fd, rom_ptr->rom_num, "%M just arrived.", 
		m1arg(ply_ptr));
		return(0);
	}

	n = count_vis_ply(rom_ptr);
	if((F_ISSET(rom_ptr, RONEPL) && n > 0) ||
		(F_ISSET(rom_ptr, RTWOPL) && n > 1) ||
		(F_ISSET(rom_ptr, RTHREE) && n > 2)) 
	{
		output(fd, "The room you fled to was full!\n");
		broadcast_rom(fd, rom_ptr->rom_num, "%M just arrived.", m1arg(ply_ptr));
		return(0);
	}
	if(F_ISSET(ply_ptr, PALIAS)) 
	{
		del_crt_rom(Ply[ply_ptr->fd].extr->alias_crt, ply_ptr->parent_rom);
		add_crt_rom(Ply[ply_ptr->fd].extr->alias_crt, rom_ptr, 1);
	}
	cp = ply_ptr->first_fol;
	while(cp) {
		if(cp->crt->type == MONSTER && F_ISSET(cp->crt, MCONJU)) {
			del_crt_rom(cp->crt, ply_ptr->parent_rom);
			add_crt_rom(cp->crt, rom_ptr,1);
		}
		cp = cp->next_tag;
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

int list( creature *ply_ptr, cmd *cmnd )
{
	room	*rom_ptr, *dep_ptr;
	otag	*op;
	char	tmpstr[80];
	int	fd, n=0;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(F_ISSET(rom_ptr, RSHOPP)) {
		if(load_rom(rom_ptr->rom_num+1, &dep_ptr) < 0) {
			output(fd, "Nothing to buy.\n");
			return(0);
		}
		add_permobj_rom(dep_ptr);
		output(fd, "You may buy:\n");
		op = dep_ptr->first_obj;
		while(op) {
			if(F_ISSET(dep_ptr, RPOWND)) {
				  n+=1;
				  sprintf(g_buffer, "   %-5s - ", obj_str(ply_ptr, op->obj, 1, CAP));
				
				  strcat(g_buffer, obj_condition(op->obj));
				  sprintf(tmpstr, "  Cost: %-ld\n", op->obj->value);
				  strcat(g_buffer, tmpstr); 
			} 
			else {
				n=1;
				sprintf(g_buffer, "   %-30s   Cost: %ld\n", 
				obj_str(ply_ptr, op->obj, 1, CAP), op->obj->value);
			}
			if(F_ISSET(op->obj, OCONTN)) 
				n-=1;
			if(!F_ISSET(op->obj, OCONTN))
				output(fd, g_buffer);
			
			op = op->next_tag;
		}
		if(!n)
                	output(fd, "Nothing.");
		output(fd, "\n");
	}

	else
		output(fd, "This is not a shoppe.\n");

	return(0);

}

/**********************************************************************/
/*				buy				      */
/**********************************************************************/

/* This function allows a player to buy something from a shoppe.      */

int buy(creature *ply_ptr, cmd *cmnd )
{
	room	*rom_ptr, *dep_ptr;
	object	*obj_ptr, *obj_ptr2;
	creature *own_ptr;
	int	fd, salestax;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(!F_ISSET(rom_ptr, RSHOPP)) {
		output(fd, "This is not a shoppe.\n");
		return(0);
	}

	if(cmnd->num < 2) {
		output(fd, "Buy what?\n");
		return(0);
	}

	if(load_rom(rom_ptr->rom_num+1, &dep_ptr) < 0) {
		output(fd, "Nothing to buy.\n");
		return(0);
	}

	obj_ptr = find_obj(ply_ptr, dep_ptr->first_obj,
			   cmnd->str[1], cmnd->val[1]);

	if(!obj_ptr || (F_ISSET(obj_ptr, OCONTN) && F_ISSET(dep_ptr, RPOWND))) 
	{
		output(fd, "That's not being sold.\n");
		return(0);
	}

	if(ply_ptr->gold < obj_ptr->value) {
		output(fd, "You don't have enough gold.\n");
		return(0);
	}
 
                if( (weight_ply(ply_ptr) + weight_obj(obj_ptr) > max_weight(ply_ptr)) ){
                   output(fd, "You can't carry anymore.\n");
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

	ply_ptr->gold -= obj_ptr->value;

	if(F_ISSET(dep_ptr, RPOWND)) {
		/* find dep_ptr owner */
		own_ptr = find_rom_owner(dep_ptr);
		if(own_ptr) {
			/* put obj_ptr2->value in owner's bank account */
			salestax = (int) (obj_ptr->value*TAXRATE);
			bank_account_modify(own_ptr, (obj_ptr->value-salestax));
			
			/* remove object from dep_ptr room */
			F_CLR(obj_ptr, OPERM2);
		        F_CLR(obj_ptr, OPERMT);
        		F_CLR(obj_ptr, OTEMPP);

			del_obj_rom(obj_ptr, dep_ptr);
			resave_rom(dep_ptr->rom_num);
			F_SET(own_ptr, PSAVES);
       	                save_ply(own_ptr);
        	        F_CLR(own_ptr, PSAVES);

			sprintf(g_buffer, "%s bought a %s for %ld from %s's shop (%d).\n", ply_ptr->name, obj_ptr->name, obj_ptr->value, own_ptr->name, rom_ptr->rom_num);
			slog(g_buffer);
			if(!find_who(own_ptr->name)) 
				free_crt(own_ptr);
			
		}	

	}
	
        add_obj_crt(obj_ptr2, ply_ptr);

	output(fd, "Bought.\n");
	broadcast_rom(fd, ply_ptr->rom_num, "%M bought %1i.", m2args(ply_ptr, obj_ptr2));
	return(0);
		

}

/**********************************************************************/
/*				sell				      */
/**********************************************************************/

/* This function will allow a player to sell an object in a pawn shoppe */

int sell( creature *ply_ptr, cmd *cmnd )
{
	room	*rom_ptr;
	object	*obj_ptr;
	int	gold, fd, poorquality = 0;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(!F_ISSET(rom_ptr, RPAWNS)) {
		output(fd, "This is not a pawn shoppe.\n");
		return(0);
	}

	if(cmnd->num < 2) {
		output(fd, "Sell what?\n");
		return(0);
	}

	F_CLR(ply_ptr, PHIDDN);

	obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
			   cmnd->str[1], cmnd->val[1]);

	if(!obj_ptr) {
		output(fd, "You don't have that.\n");
		return(0);
	}
	if(F_ISSET(obj_ptr, ONOSEL)) {
		output(fd, "You may not sell that item.\n");
		return(0);
	}
	luck (ply_ptr);	
	gold = obj_ptr->value / 4;
	gold = ((Ply[fd].extr->luck*gold)/100); 
	gold = MIN( gold, 10000);
	
	if((obj_ptr->type <= MISSILE || obj_ptr->type == ARMOR) &&
	   obj_ptr->shotscur <= obj_ptr->shotsmax/8)
		poorquality = 1;

	if((obj_ptr->type == WAND || obj_ptr->type > MISC || obj_ptr->type == KEY) && obj_ptr->shotscur < 1)
		poorquality = 1;

	if(gold < 20 || poorquality) {
		output(fd, "The shopkeep says, \"I won't buy that crap from you.\"\n");
		return(0);
	}

	if(obj_ptr->type == SCROLL || obj_ptr->type == POTION || obj_ptr->type==FOOD || obj_ptr->type==DRINK) {
		output(fd, "The shopkeep won't buy that from you.\n");
		return(0);
	}

	sprintf(g_buffer, "The shopkeep gives you %d gold for %%i.\n", gold);
	mprint(fd, g_buffer, m1arg(obj_ptr));
	broadcast_rom(fd, ply_ptr->rom_num, "%M sells %1i.", 
		      m2args(ply_ptr, obj_ptr));

        if(GUILDEXP)
        {
                   if(check_guild(ply_ptr) == 5)
                   {
                           gold += gold *.05;
                           sprintf(g_buffer, "You guild expertise earns you %d more gold.\n", gold);
                           output(fd, g_buffer);
                   }
         }


	ply_ptr->gold += gold;
	del_obj_crt(obj_ptr, ply_ptr);
	free_obj(obj_ptr);

	return(0);

}

/**********************************************************************/
/*				appraise			      */
/**********************************************************************/

/* This function allows a player to find out the pawn-shop value of an */
/* object, if he is in the pawn shop.				       */

/* it also allows players to modify the value of an object for the	*/
/* purpose of selling it to another player			*/

int appraise( creature *ply_ptr, cmd *cmnd )
{
	room	*rom_ptr, *dep_ptr;
	object	*obj_ptr;
	creature	*own_ptr;
	long	value;
	int	fd, chance;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(cmnd->num == 3) {
		if(load_rom(rom_ptr->rom_num+1, &dep_ptr) < 0) {
                	output(fd, "You can't do that here.\n");
                	return(0);
        	}

		if (!F_ISSET(dep_ptr, RPOWND)) {
			output(fd, "You must be in your shop to do that.\n");
			return(0);
		} 

	} else {	/* cmnd->num == 2 */
	  if(!F_ISSET(rom_ptr, RPAWNS) && !F_ISSET(rom_ptr, RREPAI)){
		output(fd, "You must be in a pawn or repair shoppe.\n");
		return(0);
	  }

	}
	

	if(cmnd->num < 2) {
		output(fd, "Value what?\n");
		return(0);
	}

	F_CLR(ply_ptr, PHIDDN);

	obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
			   cmnd->str[1], cmnd->val[1]);

	if(!obj_ptr) {
		output(fd, "You don't have that.\n");
		return(0);
	}

	if(cmnd->num > 2) {
		own_ptr = find_rom_owner(dep_ptr);
		if(!own_ptr || own_ptr != ply_ptr) {
			output(fd, "This does not appear to be your shop.\n");
			return(0);
		}
		if(cmnd->str[2][0] != '$') {
			output(fd, "Syntax: value <object> $amount\n");
			return(0);
		}
		value = atol(&cmnd->str[2][1]);
		// if(value < 1 || value > LONG_MAX) {
		if(value < 1 || value > 1000000) {
			output(fd, "Get real.\n");
			return(0);
		}
		if(value > obj_ptr->value)
			F_SET(obj_ptr, ONOSEL);
		if(value < obj_ptr->value)
			F_SET(obj_ptr, ONOFIX);
		obj_ptr->value = value;
		sprintf(g_buffer, "Ok, %s can now be sold in your shop for %ld gold.\n",obj_ptr->name,value);
		output(fd, g_buffer);
		return(0);
	}	

	if(F_ISSET(rom_ptr, RREPAI)) {
		value = obj_ptr->value / 2;
		sprintf(g_buffer, "The shopkeep says, \"%%I costs %ld to be repaired.\"\n", 
		      value);
		mprint(fd, g_buffer, m1arg(obj_ptr));
		broadcast_rom(fd, ply_ptr->rom_num, "%M gets %i appraised.",
		      m2args(ply_ptr, obj_ptr));
		return(0);
	}

        chance =  (4 * ply_ptr->level) + bonus[(int)ply_ptr->intelligence];
        chance = MAX(85,chance); chance = MIN(100,chance);
                
        if (mrand(1,100) > chance) {
                output(ply_ptr->fd, "The shopkeeper is unsure of its value.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "%M attempts to appraise a %i.",
                        m2args(ply_ptr, obj_ptr));
                return(0);
        }


        if((ply_ptr->class == THIEF || ply_ptr->class == ASSASSIN 
		|| ply_ptr->class == BARD || ply_ptr->class >= BUILDER)) {
		value = MIN(obj_ptr->value / 2, 10000L);
		sprintf(g_buffer, "%%I is worth %ld.\n", value);
		mprint(fd, g_buffer, m1arg(obj_ptr));
	}
        if((ply_ptr->class == MAGE || ply_ptr->class == DRUID 
		|| ply_ptr->class >= BUILDER)) {
                if (obj_ptr->adjustment > 0) {
                        sprintf(g_buffer, "The %s is a +%d item.\n", obj_ptr->name, obj_ptr->adjustment);
                        output(fd, g_buffer);
                }
                else
                        mprint(fd, "%I is not a magical item.\n", m1arg(obj_ptr));
        }
        if((ply_ptr->class == ALCHEMIST || ply_ptr->class >= BUILDER)) {
                if(obj_ptr->type == WAND || obj_ptr->type <= ARMOR) {
                        sprintf(g_buffer, "%s", obj_condition(obj_ptr));
                        output(fd, g_buffer);
                }
                else mprint(fd, "%I is not a weapon, armor, or a wand.\n", m1arg(obj_ptr));
        }
        if((ply_ptr->class == FIGHTER || ply_ptr->class == BARBARIAN 
		|| ply_ptr->class == PALADIN || ply_ptr->class == RANGER 
		|| ply_ptr->class >= BUILDER)) {
            if(obj_ptr->type < ARMOR) {
                if(obj_ptr->weight < 7)   
                        output(ply_ptr->fd, "This is a poor weapon.\n");
                if ((obj_ptr->weight >= 7) && (obj_ptr->weight <=9))
                        output(ply_ptr->fd, "This is a substandard weapon.\n");
                if ((obj_ptr->weight >= 10) && (obj_ptr->weight <=13))
                        output(ply_ptr->fd, "This is a standard weapon.\n");
                if ((obj_ptr->weight >= 14) && (obj_ptr->weight <=16))
                        output(ply_ptr->fd, "This is a quality weapon.\n");
                if ((obj_ptr->weight >= 17) && (obj_ptr->weight <=19))
                        output(ply_ptr->fd, "This is a outstanding weapon.\n");   
                if ((obj_ptr->weight >= 20) && (obj_ptr->weight <=22))
                        output(ply_ptr->fd, "This is a special weapon.\n");
                if (obj_ptr->weight > 22)
                        output(ply_ptr->fd, "This weapon is of supreme quality.\n");
                sprintf(g_buffer, "%s", obj_condition(obj_ptr));
                output(fd, g_buffer);
            }
            if(obj_ptr->type == ARMOR) {
                sprintf(g_buffer, "%s", obj_condition(obj_ptr));
                output(fd, g_buffer);
            }
            if(obj_ptr->type > ARMOR && ply_ptr->class < BUILDER)
                mprint(fd, "%I is not something you can appraise.\n", m1arg(obj_ptr));
        }
	 

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

int backstab( creature *ply_ptr, cmd *cmnd )
{
	creature	*crt_ptr;
	room		*rom_ptr;
	time_t		i, t;
	int		fd, m, n, p, addprof;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(ply_ptr->class != THIEF && ply_ptr->class != ASSASSIN &&
	   ply_ptr->class < BUILDER) {
		output(fd, "Only thieves and assassins may backstab.\n");
		return(0);
	}

	if(cmnd->num < 2 || F_ISSET(ply_ptr, PBLIND)) {
		output(fd, "Backstab what?\n");
		return(0);
	}

/*	if(!ply_ptr->ready[WIELD-1] || (ply_ptr->ready[WIELD-1]->type 
	   != SHARP && ply_ptr->ready[WIELD-1]->type != THRUST)) {
		output(fd, "Backstab requires certain sharp or thrusting weapons.\n");
		return(0);
	}
*/

	if(!ply_ptr->ready[WIELD-1] || !F_ISSET(ply_ptr->ready[WIELD-1], OBKSTA)) { 
		output(fd, "Backstab requires special backstab weapons.\n");
		return(0);
	}

	t = time(0);
	i = LT(ply_ptr, LT_ATTCK);

	if(t < i) {
		please_wait(fd, i-t);
		return(0);
	}

	crt_ptr = find_crt_in_rom(ply_ptr, rom_ptr, cmnd->str[1], cmnd->val[1],
		MON_FIRST);

	if(!crt_ptr || crt_ptr == ply_ptr || ( crt_ptr->type == PLAYER && strlen(cmnd->str[1]) < 3)) {
		output(fd, "You don't see that here.\n");
		return(0);
	}


	if(crt_ptr->type != PLAYER && is_enm_crt(ply_ptr->name, crt_ptr)) {
		sprintf(g_buffer, "Not while you're already fighting %s.\n",
		      F_ISSET(crt_ptr, MMALES) ? "him":"her");
		output(fd, g_buffer );
		return(0);
	}

	if(crt_ptr->type == PLAYER) 
	{
		if ( !pkill_allowed(ply_ptr, crt_ptr) )
		{
			return(0);
		}

		if(is_charm_crt(ply_ptr->name, crt_ptr) && F_ISSET(ply_ptr, PCHARM)){
			sprintf(g_buffer, 
				"Why would you want to backstab your good friend %s?\n", 
				crt_ptr->name);
			output(fd, g_buffer );
			return(0);
		}
	}
	else if(!is_crt_killable(crt_ptr, ply_ptr)) 
	{
		return(0);
	}

	if(is_charm_crt(crt_ptr->name, ply_ptr) && F_ISSET(ply_ptr, PCHARM)) {
		sprintf(g_buffer, "You are too fond of %s to do that.\n", crt_ptr->name);
		output(fd, g_buffer );
        return(0);
    }

	if(F_ISSET(ply_ptr, PINVIS)) {
		F_CLR(ply_ptr, PINVIS);
		output(fd, "Your invisibility fades.\n");
		broadcast_rom(fd, ply_ptr->rom_num, "%M fades into view.",
			      m1arg(ply_ptr));
	}

	ply_ptr->lasttime[LT_ATTCK].ltime = t;
	if(ply_ptr->dexterity > 18)
		ply_ptr->lasttime[LT_ATTCK].interval = 2;
	else
		ply_ptr->lasttime[LT_ATTCK].interval = 3;

	if(crt_ptr->type != PLAYER) {
		if(add_enm_crt(ply_ptr->name, crt_ptr) < 0) {
			mprint(fd, "You backstab %m.\n", m1arg(crt_ptr));
			broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
				       "%M backstabs %m.", m2args(ply_ptr, crt_ptr));
 			if(F_ISSET(crt_ptr, MMGONL)) {
            	mprint(fd, "Your weapon has no effect on %m.\n", 
					m1arg(crt_ptr));
            	return(0);
        	}
        	if(F_ISSET(crt_ptr, MENONL)) {
            	if(!ply_ptr->ready[WIELD-1] || 
                	ply_ptr->ready[WIELD-1]->adjustment < 1) {
                	mprint(fd, "Your weapon has no effect on %m.\n", 
						m1arg(crt_ptr));
                	return(0);
            }
        }  

		}
	}
	else {
		mprint(fd, "You backstab %m.\n", m1arg(crt_ptr));
		mprint(crt_ptr->fd, "%M backstabs you.\n", m1arg(ply_ptr));
		broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
			       "%M backstabs %m!", m2args(ply_ptr, crt_ptr));
	}

	if(ply_ptr->ready[WIELD-1]) {
		if(ply_ptr->ready[WIELD-1]->shotscur < 1) {
			break_weapon( ply_ptr );

			sprintf(g_buffer, "%s backstab failed.", 
				      F_ISSET(ply_ptr, PMALES) ? "His":"Her");
			broadcast_rom(fd, ply_ptr->rom_num, 
				      g_buffer, NULL ); 
			ply_ptr->lasttime[LT_ATTCK].interval *= 2;
			return(0);
		}
	}

	n = ply_ptr->thaco - crt_ptr->armor/10 + 2;
/*
	if(!F_ISSET(ply_ptr, PHIDDN))
		n = 21;

	F_CLR(ply_ptr, PHIDDN);
*/

	if(mrand(1,20) >= n) {
		if(ply_ptr->ready[WIELD-1])
			n = mdice(ply_ptr->ready[WIELD-1]);
		else
			n = mdice(ply_ptr);

		if(ply_ptr->class == THIEF) {
			if (crt_ptr->type == PLAYER)
/* Limit backstabs to x3 against players */
				n *= 2 + MAX(0.5*((ply_ptr->level-1)/4),1);
			else n *= 2 + (0.5*((ply_ptr->level-1)/4));
		}
		else if (crt_ptr->type == PLAYER)
			n *= 2 + MAX(0.5*((ply_ptr->level-1)/3),1);
		     else n *= 2 + (0.5*((ply_ptr->level-1)/3));

		if(!F_ISSET(ply_ptr, PHIDDN)) {
			n *= 0.5;
		}
		F_CLR(ply_ptr, PHIDDN);

		m = MIN(crt_ptr->hpcur, n);
		

		if(crt_ptr->type != PLAYER) {
			add_enm_dmg(ply_ptr->name, crt_ptr, m);
			if(ply_ptr->ready[WIELD-1]) {
				p = MIN(ply_ptr->ready[WIELD-1]->type, 4);
				addprof = (m * crt_ptr->experience) /
					MAX(crt_ptr->hpmax, 1);
				addprof = MIN(addprof, crt_ptr->experience);
				ply_ptr->proficiency[p] += addprof;
			}
           		else if(ply_ptr->class == MONK) { 
		            	/* give blunt prof for monk barehand */
        		        addprof = (m * crt_ptr->experience) /
                		    MAX(crt_ptr->hpmax, 1);
              			addprof = MIN(addprof, crt_ptr->experience);
                		ply_ptr->proficiency[2] += addprof;
           		}
		}
		crt_ptr->hpcur -= n;

		sprintf(g_buffer, "You hit for %d damage.\n", n);
		output(fd, g_buffer );

		sprintf(g_buffer, "%%M hit you for %d damage.\n", n);
		mprint(crt_ptr->fd, g_buffer, m1arg(ply_ptr));

		if(crt_ptr->hpcur < 1) {
			mprint(fd, "You killed %m.\n", m1arg(crt_ptr));
			broadcast_rom(fd, ply_ptr->rom_num,
				      "%M killed %m.", m2args(ply_ptr, crt_ptr));
			die(crt_ptr, ply_ptr);
		}
		else
			check_for_flee(crt_ptr);

		/* handle shot reduction */
		attack_with_weapon( ply_ptr );

		/* pkilling makes bs take longer */
		if ( crt_ptr->type == PLAYER )
		{
			ply_ptr->lasttime[LT_ATTCK].interval += 5;
		}
	}
	else {
		output(fd, "You missed.\n");
		sprintf(g_buffer, "%s backstab failed.", 
			F_ISSET(ply_ptr, PMALES) ? "His":"Her");
		broadcast_rom(fd, ply_ptr->rom_num, g_buffer, NULL); 

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

int train(creature *ply_ptr, cmd *cmnd )
{
	room	*rom_ptr;
	long	goldneeded, expneeded;
	int	fd, i, fail = 0, bit[4];

	fd = ply_ptr->fd;
	
	if(F_ISSET(ply_ptr, PBLIND)){
		output_wc(fd, "You can't...You're blind!\n", BLINDCOLOR);
		return(0);
	}
	if(!F_ISSET(ply_ptr, PSECOK)) {
		output(fd, "You are not allowed to do that.\n");
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
		output(fd, "This is not your training location.\n");
		return(0);
	}

	if(ply_ptr->level < MAXALVL)
		expneeded = needed_exp[ply_ptr->level-1];
	else
               expneeded = (long)((needed_exp[MAXALVL-1]*ply_ptr->level));   

	goldneeded = expneeded / 2L;

	if(expneeded > ply_ptr->experience) {
		sprintf(g_buffer, "You need %ld more experience.\n",
		      expneeded - ply_ptr->experience);
		output(fd, g_buffer );
		return(0);
	}

	if(goldneeded > ply_ptr->gold) {
		output(fd, "You don't have enough gold.\n");
		sprintf(g_buffer, "You need %ld gold to train.\n", goldneeded);
		output(fd, g_buffer );
		return(0);
	}

	ply_ptr->gold -= goldneeded;
	up_level(ply_ptr);

	sprintf(g_buffer, "### %s just made a level!", ply_ptr->name);
	broadcast(g_buffer);
	output(fd, "Congratulations, you made a level!\n\n");

	return(0);

}
/************************************************************************/
/*                    courageous						*/

/* Clears the scared list						*/

void courageous( creature *ply_ptr )
{
  int **scared_of;
  if(Ply[ply_ptr->fd].extr == NULL) 
	  return;
  scared_of = &Ply[ply_ptr->fd].extr->scared_of;
  if(*scared_of) {
    free(*scared_of);
    *scared_of = NULL;
  }
}
