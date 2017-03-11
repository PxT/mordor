/*
 * CREATURE.C:
 *
 *  Routines that act on creatures.
 *
 *  Copyright (C) 1991, 1992, 1993 Brooke Paul
 *
 * $Id: creature.c,v 6.19.1.4 2001/07/26 01:40:07 develop Exp $
 *
 * $Log: creature.c,v $
 * Revision 6.19.1.4  2001/07/26 01:40:07  develop
 * fixed bug in is_stolen_crt that would crash if fd < 0
 *
 * Revision 6.19.1.3  2001/07/25 23:14:55  develop
 * added probability for OTHEFT drop
 *
 * Revision 6.19.1.2  2001/07/25 02:55:04  develop
 * fixes for thieves dropping stolen items
 * fixes for gold dropping by pkills
 *
 * Revision 6.19.1.2  2001/07/24 01:36:23  develop
 * *** empty log message ***
 *
 * Revision 6.19.1.1  2001/07/22 20:05:52  develop
 * gold theft changes
 *
 * Revision 6.19  2001/07/22 19:03:06  develop
 * first run at alllowing thieves to steal gold from other players
 *
 * Revision 6.18  2001/07/17 19:25:11  develop
 * *** empty log message ***
 *
 * Revision 6.17  2001/06/30 01:50:30  develop
 * removed experience loss when conjured creature kills a player
 *
 * Revision 6.16  2001/06/26 22:30:48  develop
 * conjure bug fixes
 *
 * Revision 6.15  2001/06/06 19:36:46  develop
 * conjure added
 *
 * Revision 6.14  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

#include "../include/mordb.h"
#include "mextern.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


/* #define OWNTROPHY	1501 */
/**********************************************************************/
/*              find_crt                  */
/**********************************************************************/

/* This function will attempt to locate a given creature within a given */
/* list.  The first parameter contains a pointer to the creature doing  */
/* the search, the second contains a tag pointer to the first tag in    */
/* the list.  The third contains the match string, and the fourth       */
/* contains the number of times to match the string.            */

creature *find_crt( creature *ply_ptr, ctag *first_ct, char *str, int val )
{
    ctag    *cp;
    int match=0, found=0;

	ASSERTLOG( ply_ptr );
	ASSERTLOG( str );

    if(!ply_ptr)
		return(0);
    if(!str)
		return(0);
    if(!first_ct)
		return(0);
	
    cp = first_ct;
    while(cp) {
        if(!cp->crt) {
		cp = cp->next_tag;
            	continue;
	}
	if(ply_ptr->class < BUILDER && cp->crt->class >= BUILDER && F_ISSET(cp->crt, PDMINV)) {
            cp = cp->next_tag;
            continue;
        }
        if(EQUAL(cp->crt, str) &&
           (F_ISSET(ply_ptr, PDINVI) ?
           1:!F_ISSET(cp->crt, MINVIS))) {
            match++;
            if(match == val) {
                found = 1;
                break;
            }
        }
        cp = cp->next_tag;
    }

    if(found)
        return(cp->crt);
    else
        return(0);
}


/**********************************************************************/
/*              find_exact_crt                  */
/**********************************************************************/

/* This function will attempt to locate a given creature within a given */
/* list.  The first parameter contains a pointer to the creature doing  */
/* the search, the second contains a tag pointer to the first tag in    */
/* the list.  The third contains the match string, and the fourth       */
/* contains the number of times to match the string.            */

creature *find_exact_crt( creature *ply_ptr, ctag *first_ct, char *str, int val )
{
    ctag    *cp;
    int match=0, found=0;

	ASSERTLOG( ply_ptr );
	ASSERTLOG( str );

    if(!ply_ptr)
		return(0);
    if(!str)
		return(0);
    if(!first_ct)
		return(0);
	
    cp = first_ct;
    while(cp) {
        if(!cp->crt) 
		{
			cp = cp->next_tag;
            continue;
		}
		if(ply_ptr->class < BUILDER && cp->crt->class >= BUILDER && F_ISSET(cp->crt, PDMINV)) {
            cp = cp->next_tag;
            continue;
		}
		if ( !strcmp(cp->crt->name, str ) &&
           (F_ISSET(ply_ptr, PDINVI) ?
           1:!F_ISSET(cp->crt, MINVIS))) {
            match++;
            if(match == val) {
                found = 1;
                break;
            }
        }
        cp = cp->next_tag;
    }

    if(found)
        return(cp->crt);
    else
        return(0);
}


/**********************************************************************/
/*              find_crt_in_rom()                  */
/**********************************************************************/

/* This function will attempt to locate a given creature within a given */
/* list.  The first parameter contains a pointer to the creature doing  */
/* the search, the second contains a pointer to room you want to search */
/* The third contains the match string, and the fourth contains the	*/
/* number of times to match the string.					*/
/* NOTE: This function will check both monsters and players in a given	*/
/* room.  Monsters are checked first, then players.			*/
/* This prevents a player named student standing in a room with students*/
/* from being in effect untouchable.					*/
creature *find_crt_in_rom( creature *ply_ptr, room *rom_ptr, const char *str, int val, int flag )
{
    ctag    *cp;
    int match=0, found=0;
	char *szTemp;

	ASSERTLOG( ply_ptr );
	ASSERTLOG( rom_ptr );
	ASSERTLOG( str );

    if(!ply_ptr)
		return(0);
    if(!str)
		return(0);
    if(!rom_ptr)
		return(0);

	/* make a copy of the string first so we dont trash it */
	szTemp = malloc( strlen(str) + 1);
	if ( szTemp == NULL )
	{
	    merror("find_crt_in_rom", FATAL);
	}
	strcpy(szTemp, str);

	/* check for monsters first */
	if ( flag == MON_FIRST )
	{
		szTemp[0] = low(szTemp[0]);
		cp = rom_ptr->first_mon;
	}
	else
	{
		szTemp[0] = up(szTemp[0]);
		cp = rom_ptr->first_ply;
	}

    while(cp) {
        if(!cp->crt) {
		cp = cp->next_tag;
            	continue;
		}
		if(ply_ptr->class < BUILDER && cp->crt->class >= BUILDER && F_ISSET(cp->crt, PDMINV)) {
            cp = cp->next_tag;
            continue;
        }
        if(EQUAL(cp->crt, szTemp) &&
           (F_ISSET(ply_ptr, PDINVI) ?
           1:!F_ISSET(cp->crt, MINVIS))) {
            match++;
            if(match == val) {
                found = 1;
                break;
            }
        }
        cp = cp->next_tag;
    }

    if(!found)
	{
		if ( flag != MON_FIRST )
		{
			/* checking for monster now */
			szTemp[0] = low(szTemp[0]);
			cp = rom_ptr->first_mon;
		}
		else
		{
			/* checking for player now */
			szTemp[0] = up(szTemp[0]);
			cp = rom_ptr->first_ply;
		}
		while(cp) {
			if(!cp->crt) {
			cp = cp->next_tag;
            		continue;
			}
			if(ply_ptr->class < BUILDER && cp->crt->class >= BUILDER && F_ISSET(cp->crt, PDMINV)) {
				cp = cp->next_tag;
				continue;
			}
			if(EQUAL(cp->crt, szTemp) &&
			   (F_ISSET(ply_ptr, PDINVI) ?
			   1:!F_ISSET(cp->crt, MINVIS))) {
				match++;
				if(match == val) {
					found = 1;
					break;
				}
			}
			cp = cp->next_tag;
		}
	}

	free(szTemp);

    if(found)
        return(cp->crt);
	else
		return(0);
}



/**********************************************************************/
/*              add_enm_crt               */
/**********************************************************************/

/* This function adds a new enemy's name to the front a creature's enemy */
/* list.  The closer to the front of the list, the higher the likelihood */
/* that you will be attacked.                        */

/* NOTE:  Only call this before attack_crt, not after. */

int add_enm_crt(char *enemy, creature *crt_ptr )
{
    etag    *ep;
    int n;

	ASSERTLOG( enemy  );
	ASSERTLOG( crt_ptr );

    n = del_enm_crt(enemy, crt_ptr);

    ep = 0;
    ep = (etag *)malloc(sizeof(etag));
    if(!ep)
        merror("add_enm_crt", FATAL);
    
    strcpy(ep->enemy, enemy);
    ep->next_tag = 0;
    ep->damage = (n > -1) ? n : 0;

    if(!crt_ptr->first_enm) {
        crt_ptr->first_enm = ep;
        return(n);
    }

    ep->next_tag = crt_ptr->first_enm;
    crt_ptr->first_enm = ep;

    if(n < 0) crt_ptr->NUMHITS = 0;

    return(n);

}

/**********************************************************************/
/*              del_enm_crt               */
/**********************************************************************/

/* This function removes an enemy's name from his enemy list.         */

int del_enm_crt(char *enemy, creature *crt_ptr )
{
    etag    *ep, *prev;
    int dmg;

	ASSERTLOG( enemy  );
	ASSERTLOG( crt_ptr );

    ep = crt_ptr->first_enm;
    if(!ep) 
		return(-1);

    if(!strcmp(ep->enemy, enemy)) {
        crt_ptr->first_enm = ep->next_tag;
        dmg = ep->damage;
        free(ep);
        return(dmg);
    }

    while(ep) {
        if(!strcmp(ep->enemy, enemy)) {
            prev->next_tag = ep->next_tag;
            dmg = ep->damage;
            free(ep);
            return(dmg);
        }
        prev = ep;
        ep = ep->next_tag;
    }

    return(-1);

}

/**********************************************************************/
/*              end_enm_crt               */
/**********************************************************************/

/* This function moves an enemy within a monster's enemy list to the */
/* very end of the list.                         */

void end_enm_crt(char *enemy, creature *crt_ptr )
{
    etag    *ep, *move;

	ASSERTLOG( enemy );
	ASSERTLOG( crt_ptr );
    
    move = 0;
    move = (etag *)malloc(sizeof(etag));
    if(!move)
        merror("end_enm_crt", FATAL);

    strcpy(move->enemy, enemy);
    move->next_tag = 0;
    move->damage =  del_enm_crt(enemy, crt_ptr);

    ep = crt_ptr->first_enm;
    if(!ep) {
        crt_ptr->first_enm = move;
        return;
    }

    while(ep->next_tag)
        ep = ep->next_tag;

    ep->next_tag = move;

}

/************************************************************************/
/*              add_enm_dmg             */
/************************************************************************/

/* This function adds the amount of damage indicated by the third   */
/* argument to the enemy total for the creature pointed to by the   */
/* second argument.  The first argument contains the name of the player */
/* who hit the creature.                        */

void add_enm_dmg(char *enemy, creature *crt_ptr, int dmg )
{
    etag    *ep;

	ASSERTLOG( enemy );
	ASSERTLOG( crt_ptr );

    ep = crt_ptr->first_enm;
    while(ep) {
        if(!strcmp(enemy, ep->enemy))
            ep->damage += dmg;
        ep = ep->next_tag;
    }
}

/**********************************************************************/
/*              is_enm_crt                */
/**********************************************************************/

/* This function returns true if the name passed in the first parameter */
/* is in the enemy list of the creature pointed to by the second.       */

int is_enm_crt(char *enemy, creature *crt_ptr )
{
    etag    *ep;

	ASSERTLOG( enemy );
	ASSERTLOG( crt_ptr );

    ep = crt_ptr->first_enm;

    while(ep) {
        if(!strcmp(ep->enemy, enemy))
            return(1);
        ep = ep->next_tag;
    }

    return(0);
}

/**********************************************************************/
/*              die                                                   */
/**********************************************************************/

/* This function is called whenever a player or a monster dies.  The */
/* first parameter contains a pointer to the creature that has died. */
/* The second contains a pointer to the creature who did the dirty   */
/* deed.  Different chains of events are taken depending on whether  */
/* the deceased was a player or a monster.               */

void die(creature *crt_ptr, creature *att_ptr ) 
{
    otag        *op, *temp;
    etag        *ep;
    ctag	*cp, *prev, *cpp; /*, *fol;*/
    creature    *ply_ptr,*old_ptr;
    room        *rom_ptr;
    object      *obj_ptr, *gold_ptr, *new_obj;
    char        str[2048];
    long        profloss, total, xpv, gold=0;
    time_t	i, t;
    double	lost_fraction;
    int     	n, dr, levels = 0, expdiv, wielding=0, exp, guildscore;

    /* assume we lose profiecency */
    int		lose_prof = 1;

    ASSERTLOG( crt_ptr );
    ASSERTLOG( att_ptr );

    str[0] =0;
    if(crt_ptr->type == MONSTER) {


	remove_conjure_enemy(crt_ptr->name);

        ep = crt_ptr->first_enm;
        
	if(F_ISSET(crt_ptr, MCONJU)) 
		del_conjured(crt_ptr);

	if(att_ptr->type == MONSTER) {
		cpp = crt_ptr->parent_rom->first_mon;
		while(cpp) {
			if(strcmp(crt_ptr->name, cpp->crt->name) && is_enm_crt(crt_ptr->name, cpp->crt)) {
				del_enm_crt(crt_ptr->name, cpp->crt);
			}
			cpp = cpp->next_tag;
		}
	}


	while(ep) {
            ply_ptr = find_who(ep->enemy);
            if(ply_ptr) levels += ply_ptr->level;
		ep = ep->next_tag;
        }
	
	if(levels > 1 )
		expdiv = crt_ptr->experience / levels;

        ep = crt_ptr->first_enm;
        
	while(ep) {
            	ply_ptr = find_who(ep->enemy);
            
	   	if(ply_ptr) {
                expdiv = (crt_ptr->experience * ep->damage) /
                    MAX(crt_ptr->hpmax,1);
                expdiv = MIN(expdiv, crt_ptr->experience);
                sprintf(g_buffer, 
			"You gained %d experience for the death of %%m.\n",
			expdiv);
		mprint(ply_ptr->fd, g_buffer, m1arg(crt_ptr) );

                ply_ptr->experience += expdiv;
                ply_ptr->alignment -= crt_ptr->alignment/5;
                
		if(ply_ptr->alignment > 1000)
                    ply_ptr->alignment = 1000;
                
		if(ply_ptr->alignment < -1000)
                    ply_ptr->alignment = -1000;


                if(GUILDEXP)
                {
                        if(F_ISSET(ply_ptr,PPLDGK) && check_guild(ply_ptr) == 1 && crt_ptr->alignment > 100)
                        {
                                exp = ((crt_ptr->alignment / 5) * crt_ptr->level);
                                ply_ptr->experience+= MAX(1, exp);  
                                sprintf(g_buffer, "Your guild awards you %d experience for your deed.\n", exp);
                                output(ply_ptr->fd, g_buffer);
                        }
                        else if(F_ISSET(ply_ptr,PPLDGK) && check_guild(ply_ptr) == 7 && crt_ptr->alignment < -100)
                        {
                                exp = ((crt_ptr->alignment / 5) * crt_ptr->level) * (-1) ;
                                ply_ptr->experience+= MAX(1, exp);  
                                sprintf(g_buffer, "Your guild awards you %d experience for your deed.\n", exp);
                                output(ply_ptr->fd, g_buffer);
                        }
                

                if(F_ISSET(ply_ptr,PPLDGK) && F_ISSET(crt_ptr,MGUILD) && check_guild(ply_ptr) != check_guild(crt_ptr))
                {
			guildscore = guild_score (ply_ptr, crt_ptr);
			guildscore = guildscore * crt_ptr->level;
			cur_guilds[check_guild(ply_ptr)].score += guild_score (ply_ptr, crt_ptr);
                        sprintf(g_buffer, "You earn a score of %d for your guild.\n", guildscore);
                        output(ply_ptr->fd, g_buffer);
                }
	   	} /* endif GUILDEXP */
            } /* endif ply_ptr */
            ep = ep->next_tag;
        }


		if(!F_ISSET(crt_ptr, MTRADE)){
			sprintf(str, "%s was carrying: ", crt_str(att_ptr, crt_ptr, 0,CAP|INV));
			n = strlen(str);
			/* i = list_obj(&str[n], sizeof(str)-n, att_ptr, 
			crt_ptr->first_obj); */
			i = list_obj(&str[n], att_ptr, crt_ptr->first_obj); 
		}
        


	if(F_ISSET(crt_ptr, MPERMT))
            die_perm_crt(crt_ptr);

        op = crt_ptr->first_obj;
        
	while(op) {
            temp = op->next_tag;
            obj_ptr = op->obj;
            del_obj_crt(obj_ptr, crt_ptr);
            if(!F_ISSET(crt_ptr, MTRADE)){
		add_obj_rom(obj_ptr, crt_ptr->parent_rom);
            }
	    op = temp;
        }

        if(crt_ptr->gold) {
            if(load_obj(0, &obj_ptr) >= 0) {
            sprintf(obj_ptr->name, "%ld gold coins", crt_ptr->gold);
            if(i)
                strcat(str, ", ");
            strcat(str, obj_ptr->name);
            obj_ptr->value = crt_ptr->gold;
            add_obj_rom(obj_ptr, crt_ptr->parent_rom);
	    }
        }
	

		if(F_ISSET(crt_ptr, MDMFOL) && crt_ptr->type == MONSTER) {	
			ply_ptr = crt_ptr->following;
			cp = ply_ptr->first_fol;
			if(cp->crt == crt_ptr) {
                ply_ptr->first_fol = cp->next_tag;
                free(cp);
			}
			else {
				while(cp) {
					if(cp->crt == crt_ptr) {
                        prev->next_tag = cp->next_tag;
                        free(cp);
                        break;
					}
					prev = cp;
					cp = cp->next_tag;
				}
			}
			Ply[ply_ptr->fd].extr->alias_crt = 0;
			F_CLR(Ply[ply_ptr->fd].ply, PALIAS);
			ply_ptr->lasttime[LT_ATTCK].ltime = time(0);
				ply_ptr->lasttime[LT_ATTCK].interval = 10;
			ANSI(ply_ptr->fd, AFC_RED);
			ANSI(ply_ptr->fd, AM_BLINK);
			mprint (ply_ptr->fd, "%M's body has been destroyed.\n", 
				m1arg(crt_ptr));
			ANSI(ply_ptr->fd, AM_NORMAL);
			crt_ptr->following = 0;
		}

		if(is_charm_crt(crt_ptr->name, att_ptr))
			del_charm_crt(crt_ptr, att_ptr);
        
        del_crt_rom(crt_ptr, crt_ptr->parent_rom);
        free_crt(crt_ptr);

        if( (int)strlen(str) > n)
		{
			strcat(str, ".\n");
            output(att_ptr->fd, str);
		}
    }
    else if(crt_ptr->type == PLAYER) {

        rom_ptr = crt_ptr->parent_rom;

	if(crt_ptr->first_fol) {
		cp = crt_ptr->first_fol;
		while(cp) {
			if(cp->crt->type == MONSTER && F_ISSET(cp->crt, MCONJU)) {
				del_conjured(cp->crt);
				output(crt_ptr->fd, "Your conjured spirit has been destroyed.\n");
				broadcast_rom(-1, cp->crt->rom_num, "%M fades away.\n", m1arg(cp->crt));
				del_crt_rom(cp->crt,cp->crt->parent_rom);
				free_crt(cp->crt);
			}
			cp = cp->next_tag;
		}
	}

        i = LT(crt_ptr, LT_PLYKL);
        t = time(0);
		if(EATNDRINK){
			/* reset food and water */
			att_ptr->talk[5]=1;
			att_ptr->talk[6]=1;
			F_CLR(att_ptr, PNSUSN);
		}

  /* Keep following if plyr fell, otherwise we crash due to dangling ptr */
		if( (crt_ptr != att_ptr) && (crt_ptr->following) ) {
                                        old_ptr = crt_ptr->following;
                                        cp = old_ptr->first_fol;
                                        if(cp->crt == crt_ptr) {
                                        old_ptr->first_fol = cp->next_tag;
                                        free(cp);
                                        }
                                        else while(cp) {
                                                if(cp->crt == crt_ptr) {
                                                        prev->next_tag = cp->next_tag;
                                                        free(cp);
                                                        break;
                                                }
                                                prev = cp;
                                                cp = cp->next_tag;
                                        }
                                        crt_ptr->following = 0;
                                        sprintf(g_buffer, "You stop following %s.\n", old_ptr->name);
                                        output(crt_ptr->fd, g_buffer);
                                        if(!F_ISSET(crt_ptr, PDMINV))
                                                mprint(old_ptr->fd, "%M stops following you.\n",m1arg(crt_ptr));
                                }

        if((att_ptr->type != PLAYER || att_ptr == crt_ptr) && !F_ISSET(att_ptr, MCONJU))
	{
                crt_ptr->experience -= (int)(crt_ptr->experience/6.6);
		/* Thats 15% to you buddy... */
                
		n = crt_ptr->level - exp_to_lev(crt_ptr->experience);
		
		if (n > 1) {
			if ( crt_ptr->level < (MAXALVL+2))
				crt_ptr->experience = needed_exp[crt_ptr->level-3];
			else
				crt_ptr->experience = (long)((needed_exp[MAXALVL-1]*(crt_ptr->level-2)));
		}
        } 
        

        crt_ptr->lasttime[LT_PLYKL].ltime = 0L;
        crt_ptr->lasttime[LT_PLYKL].interval = 0L;

        if(att_ptr->type == PLAYER) 
	{
        	att_ptr->lasttime[LT_PLYKL].ltime = t;
        	att_ptr->lasttime[LT_PLYKL].interval = (long)mrand(7,14) * 86400L;

		if ( crt_ptr->name != att_ptr->name )
		{
			if(F_ISSET(att_ptr,PPLDGK) && F_ISSET(crt_ptr,PPLDGK))
			{
                        	guildscore = guild_score(att_ptr, crt_ptr);
				xpv = pk_exp(att_ptr, crt_ptr); //(crt_ptr->level*crt_ptr->level)*10;
					
				sprintf(g_buffer, "You have been bested.\nYou lose %ld experience.\n", (xpv * 5));
				output(crt_ptr->fd, g_buffer);
						
				crt_ptr->experience -= (xpv * 5);
						
				if (crt_ptr->experience <=0) 
					crt_ptr->experience=0;
						
				mprint(att_ptr->fd, "You have vanquished %m.\n", m1arg(crt_ptr));
				sprintf(g_buffer, "You gain %ld for your heroic deed.\n",xpv);
				output(att_ptr->fd, g_buffer);
						
				att_ptr->experience += xpv;
				add_prof(att_ptr,xpv);
                                		
				cur_guilds[check_guild(att_ptr)].pkilled += guildscore;
                                cur_guilds[check_guild(crt_ptr)].pkia += guildscore;
                                cur_guilds[check_guild(att_ptr)].score += guildscore * crt_ptr->level;
                                		
				sprintf(g_buffer, "You earn a pkill score of %d for your guild.\n",guildscore);
                                output(att_ptr->fd, g_buffer);
                                sprintf(g_buffer, "You incur a pkill score of -%d for your guild!\n",guildscore);
                                output(crt_ptr->fd, g_buffer);
	 		}
				
			else
			{
				/* just pkill without pledge */
				/* no exp or prof lost */
				lose_prof = 0;
			}
		}
	}
        
	else 
	{
            	del_enm_crt(crt_ptr->name, att_ptr);
		if(F_ISSET(crt_ptr, PDMOWN)) {
			F_CLR(crt_ptr, PDMOWN);
	
			if(load_obj(trophy_num, &obj_ptr) < 0) {
				merror("No trophy item for death of owned players", NONFATAL);
			}
			else 
			{
				sprintf(g_buffer, "%s's ear", crt_ptr->name);
				strncpy(obj_ptr->description, g_buffer, 80);
				strncpy(obj_ptr->name, g_buffer, 80);
				add_obj_rom(obj_ptr, crt_ptr->parent_rom);
				obj_ptr->value = crt_ptr->level * 2000;
				F_SET(obj_ptr, ONOPRE);
			}
		}
			
	}

		/* dont lose prof from non pledged pkills */
		if ( lose_prof )
		{
			for(n=0,total=0L; n<5; n++)
				total += crt_ptr->proficiency[n];
			for(n=0; n<4; n++)
				total += crt_ptr->realm[n];

			profloss = MAX(0L, total - crt_ptr->experience - 1024L);

		   /* decrement each proficiency and realm by the same percentage,
			  evenly distributing the xp loss.*/
		   lost_fraction = (double)profloss / total;
		   /* print(crt_ptr->fd, "profloss = %d, total = %d, lost_fraction = %f\n",
							   profloss,      total,      lost_fraction); */
		   if(lost_fraction > 0) {
				for(n=0;n<5;n++) {
					crt_ptr->proficiency[n] -= (long)(crt_ptr->proficiency[n] * lost_fraction);
				}
				for(n=0;n<4;n++) {
					crt_ptr->realm[n] -= (long)(crt_ptr->realm[n] * lost_fraction);
				}
				for(n=1,total=0; n<5; n++)
					if(crt_ptr->proficiency[n] > crt_ptr->proficiency[total]) 
						total = n;
				if(crt_ptr->proficiency[total] < 1024L)
					crt_ptr->proficiency[total] = 1024L;
			}
		}

        n = exp_to_lev(crt_ptr->experience);
        while(crt_ptr->level > n)
            down_level(crt_ptr);

		if (att_ptr->type == PLAYER)	// set players hp/mp to 1 after being pkilled 
		{
			crt_ptr->hpcur = 1;
        		crt_ptr->mpcur = 1;

		} 
		else {
			crt_ptr->hpcur = crt_ptr->hpmax;
        		crt_ptr->mpcur = crt_ptr->mpmax;
		}
        F_CLR(crt_ptr, PPOISN);
        F_CLR(crt_ptr, PDISEA);

        if(crt_ptr->ready[WIELD-1] &&
           !F_ISSET(crt_ptr->ready[WIELD-1], OCURSE)) {
			wielding=1;
            add_obj_rom(crt_ptr->ready[WIELD-1], 
                crt_ptr->parent_rom);
            temp_perm(crt_ptr->ready[WIELD-1]);
	    dequip(crt_ptr,crt_ptr->ready[WIELD-1]);
            crt_ptr->ready[WIELD-1] = 0;
        }

/* players no longer drop all their equip when pkilled - this helps fighters for pkilling */

	if(att_ptr->type == PLAYER)
	{
        	for(i=0; i<MAXWEAR; i++) 
		{
            	    if(crt_ptr->ready[i] &&
               		!F_ISSET(crt_ptr->ready[i], OCURSE)) 
		    {
               	        if(att_ptr->type == PLAYER)
                    	    add_obj_rom(crt_ptr->ready[i], crt_ptr->parent_rom);
                        else
                    	    add_obj_crt(crt_ptr->ready[i], crt_ptr);
                        crt_ptr->ready[i] = 0;
	    		dequip(crt_ptr,crt_ptr->ready[WIELD-1]);
               	    }
		}	


		/*  players drop gold when they die this will motivate
		lawfools to use banks */
		if(mrand(1,100) > 0) {
			if(crt_ptr->level < 7) 
				gold = (mrand(10, 50)*(crt_ptr->gold/100));
			else
				gold = (mrand(40, 80)*(crt_ptr->gold/100));

			if(gold>0) {
				crt_ptr->gold -= gold;
				if(load_obj(0, &gold_ptr) >= 0) {
                			sprintf(gold_ptr->name, "%ld gold coins", gold);
                			gold_ptr->value = gold;
					add_obj_rom(gold_ptr, crt_ptr->parent_rom);
				}
			}
		}

		/* thieves drop items that they have stolen if killed */
		if(mrand(1,100) > 0) {
			if(crt_ptr->class == THIEF) {	
				op = crt_ptr->first_obj;
        			while(op) {
               				if(F_ISSET(op->obj, OTHEFT)) {
						F_CLR(op->obj, OTHEFT);
						new_obj = duplicate_object(op->obj);
                        			add_obj_rom(new_obj, crt_ptr->parent_rom);
                        			del_obj_crt(op->obj, crt_ptr);
						break;
                			}
        			op = op->next_tag;
        			}
			}
		}
        }

        compute_ac(crt_ptr);
        compute_thaco(crt_ptr);

        if(crt_ptr == att_ptr)
		{
            sprintf(g_buffer, "### Sadly, %s died.", crt_ptr->name);
            broadcast( g_buffer );
		}
        else
		{
            sprintf(g_buffer, "### Sadly, %s was killed by %%1m.", crt_ptr->name);
            broadcast_c(g_buffer, att_ptr);
		}

		courageous(crt_ptr);


	
	if(!rom_ptr->death_rom || rom_ptr->death_rom == 0) 
		dr = death_room_num;
	else
		dr = rom_ptr->death_rom;
		
		/* if you die in jail, you stay in jail */
		if( !F_ISSET(rom_ptr, RJAILR))
		{
			del_ply_rom(crt_ptr, rom_ptr);
			if(load_rom(dr, &rom_ptr) < 0) 
				merror("load_death_room", FATAL);
			add_ply_rom(crt_ptr, rom_ptr);
		}
					

        save_ply(crt_ptr);
		if(wielding)
			output(crt_ptr->fd, "Your weapon was dropped where you died.\n");

    }
}

/**********************************************************************/
/*              temp_perm                  */
/**********************************************************************/
void temp_perm(object  *obj_ptr )
{
    otag    *op;

	ASSERTLOG( obj_ptr );

    F_SET(obj_ptr, OPERM2);
    F_SET(obj_ptr, OTEMPP);
    op = obj_ptr->first_obj;
    while(op) {
        temp_perm(op->obj);
        op = op->next_tag;
    }
}

/**********************************************************************/
/*              die_perm_crt                  */
/**********************************************************************/

/* This function is called whenever a permanent monster is killed.  The */
/* room the monster was in has its permanent monster list checked to    */
/* if the monster was loaded from that room.  If it was, then the last- */
/* time field for that permanent monster is updated.            */

void die_perm_crt(creature *crt_ptr )
{
    creature    *temp_crt;
    room        *rom_ptr;
    time_t      t;
    int     i;

	ASSERTLOG( crt_ptr );

    t = time(0);

    rom_ptr = crt_ptr->parent_rom;
    
    for(i=0; i<10; i++) {
        if(!rom_ptr->perm_mon[i].misc) 
			continue;
        if(rom_ptr->perm_mon[i].ltime + rom_ptr->perm_mon[i].interval > t) 
			continue;
        if(load_crt(rom_ptr->perm_mon[i].misc, &temp_crt) < 0)
            continue;
        if(!strcmp(temp_crt->name, crt_ptr->name)) {
            rom_ptr->perm_mon[i].ltime = t;
            free_crt(temp_crt);
#ifndef MOBS_ALWAYS_ACTIVE
			del_active(crt_ptr);
#endif
            break;
        }
        free_crt(temp_crt);
    }

	if(F_ISSET(crt_ptr,MDEATH)&&!F_ISSET(crt_ptr,MFOLLO)){
	  	int     fd,n;
    	char    tmp[2048], file[256],name[80];
   
		strcpy(name, crt_ptr->name);
        for(i=0; name[i]; i++)
            if(name[i] == ' ') name[i] = '_';

    	sprintf(file,"%s/ddesc/%s_%d", get_monster_path(), name,crt_ptr->level);
    	fd = open(file,O_RDONLY | O_BINARY);
    	if (fd)
		{
		    n = read(fd,tmp,2048);
	    	close(fd);
			if ( n > 0 )
			{
    			tmp[n] = 0;
   				broadcast_rom(-1, crt_ptr->rom_num, "\n", NULL);
   				broadcast_rom(-1, crt_ptr->rom_num, tmp,  NULL);
			}
		}
	}
}

/**********************************************************************/
/*              check_for_flee                */
/**********************************************************************/

/* This function will determine if a monster is about to flee to another */
/* room.  Only fleeing monsters will flee, and then only after they have */
/* less than 10% of their hit points.  Plus, they can only flee into     */
/* rooms that have already been loaded into memory.          */

void check_for_flee( creature *crt_ptr )
{
    room    *rom_ptr;
    xtag    *xp;

	ASSERTLOG( crt_ptr );

	/* We don't let perms flee anymore, since if they do the
	 * timeouts dont get set properly when they are killed */
    	if(F_ISSET(crt_ptr, MDMFOL) || F_ISSET(crt_ptr, MPERMT))
		return;
	if(!F_ISSET(crt_ptr, MFLEER))
		return;

	if(crt_ptr->hpcur > crt_ptr->hpmax/10)
        return;


    rom_ptr = crt_ptr->parent_rom;

    xp = rom_ptr->first_ext;

    while(xp) {
        if(!F_ISSET(xp->ext, XSECRT) && !F_ISSET(xp->ext, XCLOSD) && !F_ISSET(xp->ext, XNOSEE) && is_rom_loaded(xp->ext->room) && mrand(1,100) < 25) {
            sprintf(g_buffer, "%%M flees to the %s.", xp->ext->name);
            broadcast_rom(-1, rom_ptr->rom_num, 
                      g_buffer, m1arg(crt_ptr));
            del_crt_rom(crt_ptr, rom_ptr);
	    /* room is already in memory, so just fill the rom_ptr */
            load_rom(xp->ext->room, &rom_ptr);
            add_crt_rom(crt_ptr, rom_ptr, 1);
#ifndef MOBS_ALWAYS_ACTIVE
/*            if(!rom_ptr->first_ply)
                del_active(crt_ptr); */
#endif
            return;
        }
        xp = xp->next_tag;
    }
}

/**********************************************************************/
/*              consider                  */
/**********************************************************************/

/* This function allows the player pointed to by the first argument to */
/* consider how difficult it would be to kill the creature in the      */
/* second parameter.                               */

void consider(creature *ply_ptr, creature *crt_ptr )
{
    char    he[5], him[5];
    int fd, diff;

	ASSERTLOG( ply_ptr );
	ASSERTLOG( crt_ptr );

    fd = ply_ptr->fd;
    diff = ply_ptr->level - crt_ptr->level;
    diff = MAX(-4, diff);
    diff = MIN(4, diff);

    sprintf(he, "%s", F_ISSET(crt_ptr, MMALES) ? "He":"She");
    sprintf(him, "%s", F_ISSET(crt_ptr, MMALES) ? "him":"her");

    switch(diff) {
        case 0:
            sprintf(g_buffer, "%s is a perfect match for you!\n", he);
            break;
        case 1:
            sprintf(g_buffer, "%s is not quite as good as you.\n", he);
            break;
        case -1:
            sprintf(g_buffer, "%s is a little better than you.\n", he);
            break;
        case 2:
            sprintf(g_buffer, "%s shouldn't be too tough to kill.\n", he);
            break;
        case -2:
            sprintf(g_buffer, "%s might be tough to kill.\n", he);
            break;
        case 3:
            sprintf(g_buffer, "%s should be easy to kill.\n", he);
            break;
        case -3:
            sprintf(g_buffer, "%s should be really hard to kill.\n", he);
            break;
        case 4:
            sprintf(g_buffer, "You could kill %s with a needle.\n", him);
            break;
        case -4:
            sprintf(g_buffer, "%s could kill you with a needle.\n", he);
            break;
    }

    output(fd, g_buffer);

}

/**********************************************************************/
/*                      is_charm_crt                                  */
/**********************************************************************/
        
/* This function returns true if the name passed in the first parameter */
/* is in the charm list of the creature pointed to by the second.       */
        
int is_charm_crt(char *charmed, creature *crt_ptr )
{
    ctag    *cp;  
    int     fd;
    
	ASSERTLOG( charmed );
	ASSERTLOG( crt_ptr );

    fd = crt_ptr->fd;
    if(crt_ptr->type == MONSTER)
		return(0);

    cp = Ply[fd].extr->first_charm;
           
    while(cp) {
        if(!strcmp(cp->crt->name, charmed))
            return(1);
        cp = cp->next_tag;
    }

    return(0);
}                   

/************************************************************************/
/*              add_charm_crt                                           */
/************************************************************************/

/*      This function adds the creature pointed to by the first         */
/*  parameter to the charm list of the creature pointed to by the       */
/*  second parameter.                                                   */

            
int add_charm_crt(creature *crt_ptr, creature *ply_ptr )
{           
    ctag    *cp;
    int     fd, n;
    
	ASSERTLOG( crt_ptr );
	ASSERTLOG( ply_ptr );

        fd = ply_ptr->fd; 
     
        n = is_charm_crt(crt_ptr->name, ply_ptr);
        if (n)
                return(0);

        if(crt_ptr && crt_ptr->class < DM) {
		cp = 0;
                cp = (ctag *)malloc(sizeof(ctag));
                if (!cp)
                        merror("add_charm_crt", FATAL);

                cp->crt = crt_ptr;
                cp->next_tag = Ply[fd].extr->first_charm;
                Ply[fd].extr->first_charm = cp;
        }       
        return(0);
}

/************************************************************************/
/*              del_charm_crt                                           */
/************************************************************************/
/*      This function deletes the creature pointed to by the first      */
/*  parameter from the charm list of the creature pointed to by the     */
/*  second parameter.                                                   */

int del_charm_crt(creature *crt_ptr, creature *ply_ptr )
{
    ctag    *cp, *prev;
    int     fd, n;
    
	ASSERTLOG( crt_ptr );
	ASSERTLOG( ply_ptr );

        fd = ply_ptr->fd;

        n = is_charm_crt(crt_ptr->name, ply_ptr);
        if (!n)   
                return(0);

        cp = Ply[fd].extr->first_charm;

        if(cp->crt == crt_ptr) {
                Ply[fd].extr->first_charm = cp->next_tag;
                free(cp);
                return(1);
        }
        else while(cp) {
                        if(cp->crt == crt_ptr) {
                                prev->next_tag = cp->next_tag;
                                free(cp);
                                return(1);
                        }
                        prev = cp;
                        cp = cp->next_tag;
        }       
        return(0);
}

/**********************************************************************/
/*		attack_mon					      */
/**********************************************************************/
/*  This function does the attacking when it is two monsters 	      */
/*  attacking each other.					      */

int attack_mon(creature	*att_ptr, creature *atd_ptr )
{
	int	n, m;
	time_t	t;
	
	ASSERTLOG( att_ptr );
	ASSERTLOG( atd_ptr );

	if(att_ptr->type !=MONSTER || atd_ptr->type != MONSTER)
		return(0);
	
	t = time(0);
    	if(t < LT(att_ptr, LT_ATTCK))
		return(0);

	F_CLR(att_ptr, MHIDDN);
    	if(F_ISSET(att_ptr, MINVIS)) {
        	F_CLR(att_ptr, MINVIS);
        broadcast_rom(-1, att_ptr->rom_num, "%M fades into view.",
                  m1arg(att_ptr));
    	}

    	att_ptr->lasttime[LT_ATTCK].ltime = t;
	if(F_ISSET(att_ptr, MBLIND)){
		att_ptr->lasttime[LT_ATTCK].interval = 7;
    	}

        if(F_ISSET(atd_ptr, MUNKIL)) {
            return(0);
        }

        if(add_enm_crt(att_ptr->name, atd_ptr) < 0) {
            broadcast_rom(-1, att_ptr->rom_num, "%M attacks %m.",
                      m2args(att_ptr, atd_ptr));
        }


    n = att_ptr->thaco - atd_ptr->armor/10;
    

    if (F_ISSET(att_ptr, MBLIND))
		n += 5;

    if(mrand(1,20) >= n) {
            n = mdice(att_ptr) + bonus[(int) att_ptr->strength]; 
	    n = MAX(1,n);


        if(mrand(1,100) <= 5) {
            broadcast_rom(-1, att_ptr->rom_num,
                "%M made a critical hit.", m1arg(att_ptr));
            n *= mrand(3,6);
        }
        else if(mrand(1,100) <= 5) {
            sprintf(g_buffer, "%%M fumbled %s weapon.", 
                F_ISSET(att_ptr, MMALES) ? "his":"her");
            broadcast_rom(-1, att_ptr->rom_num, 
                g_buffer, m1arg(att_ptr));
        }

        m = MIN(atd_ptr->hpcur, n);
        atd_ptr->hpcur -= n;

        if(atd_ptr->hpcur < 1) {
            broadcast_rom(-1, att_ptr->rom_num,
                      "%M killed %m.", m2args(att_ptr, atd_ptr));
            die(atd_ptr, att_ptr);
            return(1);
        }
        else 
            check_for_flee(atd_ptr);
    }

    return(0);
}

/****************************************************************/
/*		mobile_crt					*/
/****************************************************************/

/*	This function provides for self-moving monsters.  If	*/
/*  the MMOBIL flag is set the creature will move around.	*/

int mobile_crt(creature	*crt_ptr )
{
	int		n=0, d=0, choice=0;
	room	*rom_ptr, *new_rom;
	xtag	*xp, *xptemp;

	ASSERTLOG( crt_ptr );

	if(crt_ptr->type != MONSTER || !F_ISSET(crt_ptr, MMOBIL) || F_ISSET(crt_ptr, MPERMT) || F_ISSET(crt_ptr, MPGUAR))
		return(0);

	if(crt_ptr->first_enm)
		return(0);

	rom_ptr = crt_ptr->parent_rom;
	xptemp = rom_ptr->first_ext;
	while(xptemp) 
	{ 
		/* Count up exits */
		if(!F_ISSET(xptemp->ext, XSECRT) && !F_ISSET(xptemp->ext, XNOSEE) && !F_ISSET(xptemp->ext, XLOCKD) && !F_ISSET(xptemp->ext, XPGUAR) && is_rom_loaded(xptemp->ext->room)) 
			d += 1;
		xptemp=xptemp->next_tag;
	}

	if(!d)
		return(0);

	choice = mrand(1,d);
	xp = rom_ptr->first_ext;
    	while(xp) 
		{  
			if(!F_ISSET(xp->ext, XSECRT) && !F_ISSET(xp->ext, XNOSEE) && !F_ISSET(xp->ext, XLOCKD) && !F_ISSET(xp->ext, XPGUAR) && is_rom_loaded(xp->ext->room)) {
				choice -= 1;
			if(!choice) 
			{
				if(F_ISSET(xp->ext, XCLOSD) && !F_ISSET(xp->ext, XLOCKD)) 
				{
					sprintf(g_buffer, "%%M just opened the %s.", xp->ext->name);
					broadcast_rom(-1, rom_ptr->rom_num, g_buffer, m1arg(crt_ptr));
					F_CLR(xp->ext, XCLOSD);
				}
				sprintf(g_buffer, "%%M just wandered to the %s.", xp->ext->name);
				broadcast_rom(-1, rom_ptr->rom_num, g_buffer, m1arg(crt_ptr));
				del_crt_rom(crt_ptr, rom_ptr);
				/* is_rom_loaded used above, no need to chck */
				load_rom(xp->ext->room, &new_rom); 
				add_crt_rom(crt_ptr, new_rom , 1);
				n = 1;
				crt_ptr->lasttime[LT_MWAND].ltime=time(0);
				crt_ptr->lasttime[LT_MWAND].interval=mrand(5,60);
				break;
			}
        }
        xp = xp->next_tag;
    }
	if(mrand(1,100)>80)
		F_CLR(crt_ptr, MMOBIL);

 return(n);
}



/****************************************************************/
/*		is_crt_killable()				*/
/*	RETURNS: 1 if it is and 0 if not			*/
/****************************************************************/
int is_crt_killable( creature *crt_ptr, creature *att_ptr )
{

    if(crt_ptr->type != PLAYER && F_ISSET(crt_ptr, MUNKIL)) 
	{
        sprintf(g_buffer, "You cannot harm %s.\n",
            F_ISSET(crt_ptr, MMALES) ? "him":"her");
        output(att_ptr->fd, g_buffer);
        return(0);
    }

    if(crt_ptr->type == MONSTER && F_ISSET(crt_ptr, MCONJU) && crt_ptr->following == att_ptr) {
        sprintf(g_buffer, "You cannot harm %s.\n",
            F_ISSET(crt_ptr, MMALES) ? "him":"her");
        output(att_ptr->fd, g_buffer);
        return(0);
    }	

	return(1);
}

/************************************************************************/
/* 		remove_conjure_enemy					*/
/************************************************************************/
/* Should be called when a creature dies to remove their name from the  */
/* enemy lists of any conjure creatures.  This will "over-remove" 	*/
/* In other words, if you pass in "militiaman" then _all_ conjureds will*/
/* stop attacking _all_ militimen which may not be what you wanted	*/
/* but thats the price we pay for using strings as the enemy lists	*/
void remove_conjure_enemy(char *enemy)
{
	int i;
	ctag *cp;

	for(i=0;i<Tablesize;i++) {
		if(!Ply[i].ply)
			continue;

		if(	Ply[i].ply->class < IMMORTAL && 
			(Ply[i].ply->class != DRUID ||
			Ply[i].ply->class != ALCHEMIST ||
			Ply[i].ply->class != MAGE ||
			Ply[i].ply->class != BARD) ) 
				continue;
		else {
			cp = Ply[i].ply->first_fol;
			while(cp) {
				if(cp->crt->type == MONSTER && F_ISSET(cp->crt, MCONJU)) {
					if(is_enm_crt(enemy,cp->crt))
						del_enm_crt(enemy, cp->crt);
				}

			cp = cp->next_tag;
			}
		}
	}

}	

/* add_stolen_crt, del_stolen_crt, and is_stolen_crt manage the 
ply_ptr->extr->first_stolen list.  */

int add_stolen_crt(creature *crt_ptr, creature *ply_ptr )
{
	ctag	*cp;
	int	fd;


        ASSERTLOG( crt_ptr );
        ASSERTLOG( ply_ptr );

        fd = ply_ptr->fd;

        if(is_stolen_crt(crt_ptr->name, ply_ptr))
                return(0);

        if(crt_ptr && crt_ptr->class < DM) {
                cp = (ctag *)malloc(sizeof(ctag));
                if (!cp)
                        merror("add_stolen_crt", FATAL);

                cp->crt = crt_ptr;
                cp->next_tag = Ply[fd].extr->first_stolen;
                Ply[fd].extr->first_stolen = cp;
        }
        return(0);

}

int del_stolen_crt(creature *crt_ptr, creature *ply_ptr )
{
	ctag    *cp, *prev;
    	int     fd;

        ASSERTLOG( crt_ptr );
        ASSERTLOG( ply_ptr );

        fd = ply_ptr->fd;

        if (!is_stolen_crt(crt_ptr->name, ply_ptr))
                return(0);

        cp = Ply[fd].extr->first_stolen;

        if(cp->crt == crt_ptr) {
                Ply[fd].extr->first_stolen = cp->next_tag;
                free(cp);
                return(1);
        }
        else while(cp) {
                        if(cp->crt == crt_ptr) {
                                prev->next_tag = cp->next_tag;
                                free(cp);
                                return(1);
                        }
                        prev = cp;
                        cp = cp->next_tag;
        }
        return(0);
}

int is_stolen_crt(char *stolen, creature *crt_ptr )
{
        ctag    *cp;
        int     fd;

        ASSERTLOG( stolen );
        ASSERTLOG( crt_ptr );

    fd = crt_ptr->fd;
    if(crt_ptr->type == MONSTER || fd < 0)
                return(0);

    cp = Ply[fd].extr->first_stolen;

    while(cp) {
        if(!strcmp(cp->crt->name, stolen))
            return(1);
    cp = cp->next_tag;
    }

    return(0);
}
