/*
 * COMMAND5.C:
 *
 *  Additional user routines.
 *
 *  Copyright (C) 1991, 1992, 1993 Brooke Paul
 *
 * $Id: command5.c,v 6.19 2001/07/22 20:05:52 develop Exp $
 *
 * $Log: command5.c,v $
 * Revision 6.19  2001/07/22 20:05:52  develop
 * gold theft changes
 *
 * Revision 6.18  2001/07/22 19:28:32  develop
 * crap!
 *
 * Revision 6.17  2001/07/22 19:17:17  develop
 * removed player from stolen list if they are attacked
 *
 * Revision 6.16  2001/07/22 19:03:06  develop
 * first run at alllowing thieves to steal gold from other players
 *
 * Revision 6.15  2001/06/28 04:52:33  develop
 * *** empty log message ***
 *
 * Revision 6.14  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

#include "../include/mordb.h"
#include "mextern.h"
#include <ctype.h>


/******************************************************************/
/*              attack  			                  */
/******************************************************************/

/* This function allows the player pointed to by the first parameter */
/* to attack a monster.                          */

int attack(creature *ply_ptr, cmd *cmnd )
{
    creature    *crt_ptr;
    room        *rom_ptr;
    time_t        i, t;
    int     fd;


    fd = ply_ptr->fd;

    t = time(0);
    i = LT(ply_ptr, LT_ATTCK);

	/* bypass all wait restrictions for immortals */
	if ( ply_ptr->class < BUILDER ) {
		if(t < i) {
			please_wait(fd, i-t);
			return(0);
		}
	}

    if(cmnd->num < 2) {
        output(fd, "Attack what?\n");
        return(0);
    }

    rom_ptr = ply_ptr->parent_rom;

    crt_ptr = find_crt_in_rom(ply_ptr, rom_ptr, cmnd->str[1],
			      cmnd->val[1], MON_FIRST);


    if(!crt_ptr || crt_ptr == ply_ptr ||
	(crt_ptr->type == PLAYER && strlen(cmnd->str[1]) < 3)) {
        output(fd, "You don't see that here.\n");
        return(0);
    }

    attack_crt(ply_ptr, crt_ptr);

    return(0);

}

/**********************************************************************/
/*              attack_crt                */
/**********************************************************************/

/* This function does the actual attacking.  The first parameter contains */
/* a pointer to the attacker and the second contains a pointer to the     */
/* victim.  A 1 is returned if the attack restults in death.          */

int attack_crt(creature *ply_ptr, creature *crt_ptr )
{
	time_t	i, t;
    	int	fd, m, n, p, addprof;

	fd = ply_ptr->fd;

    	t = time(0);
    	i = LT(ply_ptr, LT_ATTCK);

    	if(ply_ptr->class < IMMORTAL) {
    		if(t < i)
        		return(0);
    	}
	
	if (crt_ptr->type == PLAYER) {
		if(is_stolen_crt(ply_ptr->name, crt_ptr)) {
			del_stolen_crt(ply_ptr, crt_ptr);
		}
	
        	if(is_charm_crt(ply_ptr->name, crt_ptr) &&
		   F_ISSET(ply_ptr, PCHARM)) {
            		sprintf(g_buffer, "You like %s too much to do that.\n",
				 crt_ptr->name);
			output(fd, g_buffer );
            		return(0);
         	}
	 	
		else
			del_charm_crt(ply_ptr, crt_ptr);
    	}

    	if(ply_ptr->type == PLAYER)
    		F_CLR(ply_ptr, PHIDDN);
    
    	if(F_ISSET(ply_ptr, PINVIS) && ply_ptr->type==PLAYER) {
        	F_CLR(ply_ptr, PINVIS);
        	output(fd, "Your invisibility fades.\n");
        	broadcast_rom(fd, ply_ptr->rom_num, "%M fades into view.",
			      m1arg(ply_ptr));
    	}

    	ply_ptr->lasttime[LT_ATTCK].ltime = t;
    
    	if(F_ISSET(ply_ptr, PHASTE) && ply_ptr->type==PLAYER)
        	ply_ptr->lasttime[LT_ATTCK].interval = 2;
    
	else
        	ply_ptr->lasttime[LT_ATTCK].interval = 3;

    	if(F_ISSET(ply_ptr, PBLIND) && ply_ptr->type==PLAYER)
		ply_ptr->lasttime[LT_ATTCK].interval = 7;
    	
	if(crt_ptr->type == MONSTER) {
		if(!is_crt_killable(crt_ptr, ply_ptr)) {
			return(0);
		}

		if(F_ISSET(ply_ptr, PALIAS) && ply_ptr->type==PLAYER) {
			mprint(fd, "You attack %m.\n", m1arg(crt_ptr));
			broadcast_rom(fd, ply_ptr->rom_num, "%M attacks %m.",
					m2args(ply_ptr, crt_ptr));
       		}
	
		else if(add_enm_crt(ply_ptr->name, crt_ptr) < 0 ) {
			/* if(is_charm_crt(crt_ptr->name, ply_ptr))
				del_charm_crt(crt_ptr, ply_ptr); */
           
	    		mprint(fd, "You attack %m.\n", m1arg(crt_ptr));
            		broadcast_rom(fd, ply_ptr->rom_num, "%M attacks %m.",
					m2args(ply_ptr, crt_ptr));
       		}
		
		else if(ply_ptr->type==MONSTER) {
			add_enm_crt(ply_ptr->name, crt_ptr);
		} 


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
	
	} // end of if(crt_ptr->type == MONSTER)
    
	else {
		if ( !pkill_allowed(ply_ptr, crt_ptr) &&
		     ply_ptr->type != MONSTER) {
			return(0);
		}

        	ply_ptr->lasttime[LT_ATTCK].interval += 5;
       	 	mprint(crt_ptr->fd, "%M attacked you!\n", m1arg(ply_ptr));
        	broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
				"%M attacked %m!", m2args(ply_ptr, crt_ptr));
    	}

    	if(ply_ptr->ready[WIELD-1]) {
        	if(ply_ptr->ready[WIELD-1]->shotscur < 1) {
			break_weapon( ply_ptr );
            		return(0);
        	}
   	}

    	n = ply_ptr->thaco - crt_ptr->armor/10;
    
    	if(F_ISSET(ply_ptr, PBLIND))
		n += 5;

    	if(mrand(1,20) >= n) {
        	if(ply_ptr->ready[WIELD-1])
            		n = mdice(ply_ptr->ready[WIELD-1]) +
			    bonus[(int)ply_ptr->strength];
        	
		else
            		n = mdice(ply_ptr) + bonus[(int)ply_ptr->strength];

        	if(crt_ptr->class >= BUILDER) 
        		n = 0;
        
		n = MAX(1,n);

        	if(ply_ptr->class == PALADIN) {
        		if(ply_ptr->alignment < 0) {
                		n /= 2;
               			output(fd,
					"Your evilness reduces your damage.\n");
            		}
            
			else if(ply_ptr->alignment > 250) {
                		n++;
                		output(fd,
				     "Your goodness increases your damage.\n");
         		}
        	}
		if(GUILDEXP) {
			if(check_guild(ply_ptr) == 6) {
                        n += 1;
                        output(fd,
			       "Your guild expertise increases your damage.\n");
                 }
         }

		
		if(ply_ptr->class == MONK) {
			if(ply_ptr->ready[WIELD-1] &&
			  !F_ISSET(ply_ptr->ready[WIELD-1], OMONKO)) {
				output(fd, "How can you attack well with your hands full?\n");
				n /=2;
			}
/*
This is a complete functional replacement for the monk dice array.  It is a
linear function which starts monks at 1st level with 2-4 dmg, and yields
9-28 at 25th level.
*/
/*	BEGIN NEW MONK CODE	*/
			else if(!ply_ptr->ready[WIELD-1]) {
				n = mrand((2 * (1 + ((ply_ptr->level - 1)/3))),
					(ply_ptr->level+3)) 
					+ bonus[(int)ply_ptr->strength];
				if (n < 1) n = 1;
			}
/*	END NEW MONK CODE	*/
                       if(GUILDEXP) {
                               if(check_guild(ply_ptr) == 6) 
                                       n++;
                               /* Guild message has already been output above,
                               but since n was reassigned re-add the dmg
                               bonus.  */
                               
                       }

		}

        	p = mod_profic(ply_ptr);
        
		if(mrand(1,100) <= p || (ply_ptr->ready[WIELD-1] &&
		   F_ISSET(ply_ptr->ready[WIELD-1],OALCRT))) {
        		output_wc(fd, "CRITICAL HIT!\n", AFC_GREEN);
            		broadcast_rom(fd, ply_ptr->rom_num,
				      "%M made a critical hit.",m1arg(ply_ptr));
            		n *= mrand(3,6);
			
			if(ply_ptr->ready[WIELD-1] &&
			  (!F_ISSET(ply_ptr->ready[WIELD-1], ONSHAT)) &&
			  ((mrand(1,100) > (Ply[ply_ptr->fd].extr->luck+40) ||
			  mrand(1,100)<3) ||
			  (F_ISSET(ply_ptr->ready[WIELD-1],OALCRT)))) {
                		sprintf(g_buffer, "Your %s shatters.\n",
					ply_ptr->ready[WIELD-1]->name);
				output(fd, g_buffer );

				sprintf(g_buffer,  "%s %s shattered.",
					F_ISSET(ply_ptr, PMALES) ? "His":"Her",
                    			ply_ptr->ready[WIELD-1]->name);
                		broadcast_rom(fd, ply_ptr->rom_num, g_buffer,
						NULL);
				dequip(ply_ptr,ply_ptr->ready[WIELD-1]);
                		free_obj(ply_ptr->ready[WIELD-1]);
                		ply_ptr->ready[WIELD-1] = 0;
            		}
        	
		} // end of if(mrand(1,100) <= p || (ply_ptr->read[wield-1...... 
		else 
		  if(ply_ptr->type == PLAYER && (mrand(1,100) <= (5-p) || 
		     mrand(1,100) >
		     Ply[ply_ptr->fd].extr->luck+ply_ptr->level*2) && 
		     ply_ptr->ready[WIELD-1] &&
		     !F_ISSET(ply_ptr->ready[WIELD-1], OCURSE)) {
        		output_wc(fd, "You FUMBLED your weapon.\n", AFC_GREEN);
            		sprintf(g_buffer, "%%M fumbled %s weapon.",
				F_ISSET(ply_ptr, PMALES) ? "his":"her");
            		broadcast_rom(fd, ply_ptr->rom_num, g_buffer,
					m1arg(ply_ptr));
            		n = 0;
            		add_obj_crt(ply_ptr->ready[WIELD-1], ply_ptr);
			dequip(ply_ptr, ply_ptr->ready[WIELD-1]);
            		ply_ptr->ready[WIELD-1] = 0;
            		compute_thaco(ply_ptr);
       	  	}

        	sprintf(g_buffer, "You hit for %d damage.\n", n);
		output(fd, g_buffer);
        
		sprintf(g_buffer, "%%M hit you for %d damage.\n", n);
		mprint(crt_ptr->fd, g_buffer, m1arg(ply_ptr) );
	
		if(F_ISSET(crt_ptr, MNOPRE) || crt_ptr->type == PLAYER)
			sprintf(g_buffer, "%s %s %s!", ply_ptr->name,
				hit_description(n), crt_ptr->name);
		else
			sprintf(g_buffer, "%s %s the %s!", ply_ptr->name,
				hit_description(n), crt_ptr->name);

		broadcast_dam(ply_ptr->fd, crt_ptr->fd, ply_ptr->rom_num,
				g_buffer, NULL);

		if (ply_ptr->type == MONSTER && crt_ptr->type == MONSTER)
			broadcast_rom2(crt_ptr->fd,fd,crt_ptr->rom_num,
					"%M hits %m!", 
				m2args(ply_ptr,crt_ptr));

		/* handle shot reduction */
		attack_with_weapon( ply_ptr );

        	m = MIN(crt_ptr->hpcur, n);
        	crt_ptr->hpcur -= n;

        	if(crt_ptr->type != PLAYER) {
        		/* uncomment to remove charm when attack   */   
			/* if(is_charm_crt(crt_ptr->name, ply_ptr))*/
			/*	del_charm_crt(crt_ptr, ply_ptr);   */

			add_enm_dmg(ply_ptr->name, crt_ptr, m);
          	
			if(ply_ptr->ready[WIELD-1]) {
                		p = MIN(ply_ptr->ready[WIELD-1]->type, 4);
                		addprof = (m * crt_ptr->experience) /
					  MAX(crt_ptr->hpmax, 1);
                		addprof = MIN(addprof, crt_ptr->experience);
                		ply_ptr->proficiency[p] += addprof;
            		}
					
			else {
                		/* give hand prof for barehand */
                		addprof = (m * crt_ptr->experience) / MAX(crt_ptr->hpmax, 1);
                		addprof = MIN(addprof, crt_ptr->experience);

                		if(ply_ptr->class == BARBARIAN) {
                        		addprof = addprof / 5;
                        		addprof = MAX(addprof, 1);
                		} else if(ply_ptr->class != MONK) {
                        		addprof = addprof / 10;
                        		addprof = MAX(addprof, 1);
                		}

                        	ply_ptr->proficiency[HAND] += addprof;

			}
		
		} /* end of if(crt_ptr->type != PLAYER) */
        
		if(crt_ptr->hpcur < 1) {
            		mprint(fd, "You killed %m.\n", m1arg(crt_ptr));
            		broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                      		"%M killed %m.", m2args(ply_ptr, crt_ptr));
            
			die(crt_ptr, ply_ptr);
            		return(1);
        	}
        
		else {
			if (!(ply_ptr->type == PLAYER &&
			   crt_ptr->type == PLAYER)) 
				check_for_flee(crt_ptr);
        	  	
        	}
	
	} // end of if(mrand(1,20) >= n)
    
	else {
		output(fd, "You missed.\n");
        	mprint(crt_ptr->fd, "%M missed.\n", m1arg(ply_ptr));
		if (ply_ptr->type == MONSTER && crt_ptr->type == MONSTER)
  		broadcast_rom2(fd,crt_ptr->fd,crt_ptr->rom_num,
				"%M missed %m.", m2args(ply_ptr,crt_ptr) );
	}

return(0);
}

/************************************************************************/
/*              who							*/
/************************************************************************/

/* This function outputs a list of all the players who are currently */
/* logged into the game.  It includes their titles and the last      */
/* command they typed.                           */

int who( creature *ply_ptr, cmd *cmnd )
{
    char    str[15];
	char	str2[15];
    int fd, i, j;

    fd = ply_ptr->fd;

	 if(F_ISSET(ply_ptr, PBLIND)){
        output_wc(fd, "You're blind!\n", BLINDCOLOR);
        return(0);
	}

	if(LASTCOMMAND)
	{
		sprintf(g_buffer, "%-23s  %-20s     %-20s\n", "Player", "Title", "Last command");
		output(fd, g_buffer );
	}
	else 
	{
	    ANSI (fd, AM_BOLD);
	    ANSI (fd, AFC_BLUE);
//		sprintf(g_buffer, "%-23s  %-20s     %-20s\n", "Player", "Title", "Race");
		sprintf(g_buffer, "%-20s  %-20s  %-12s %-20s\n", "Player", "Title", "Race", "Guild");
		output(fd, g_buffer );
	}

    output(fd, "-------------------------------------------------------------------------\n");
    ANSI(fd, AM_NORMAL);
    ANSI(fd, AFC_WHITE);
    for(i=0; i<Tablesize; i++) {
        if(!Ply[i].ply) continue;
        if(Ply[i].ply->fd == -1) continue;
        if(F_ISSET(Ply[i].ply, PDMINV) && Ply[i].ply->class == DM &&
           ply_ptr->class < BUILDER)
            continue;
        if(F_ISSET(Ply[i].ply, PDMINV) && ply_ptr->class < DM)
            continue;
	if( F_ISSET(Ply[i].ply, PDMINV) && F_ISSET(Ply[i].ply, PROBOT) &&
               (!strcmp(Ply[i].ply->name,dmname[0]) || !strcmp(Ply[i].ply->name,dmname[1])) )
            continue;
        if(F_ISSET(Ply[i].ply, PINVIS) && !F_ISSET(ply_ptr, PDINVI) &&
           ply_ptr->class < BUILDER)
            continue;
		if( F_ISSET(Ply[i].ply, PDMINV) )
		{
			strcpy(str2, "(+)" );
		}
		else if (F_ISSET(Ply[i].ply, PINVIS) )
		{
			strcpy(str2, "(*)" );
		}
		else if (F_ISSET(Ply[i].ply, PDMOWN) )
		{
			strcpy(str2, "(O)" );
		}
		else
		{
			strcpy( str2, "   ");
		}

		ANSI(fd, AFC_WHITE);
		
		sprintf(g_buffer, "%-17s%s  ", Ply[i].ply->name, str2);
		
		output(fd, g_buffer);

		ANSI(fd, AFC_GREEN);
		sprintf(g_buffer, "%-20s  ", title_ply(Ply[i].ply));
		output(fd, g_buffer);
		ANSI(fd, AFC_WHITE);

		if(LASTCOMMAND) 
		{
		    strncpy(str, Ply[i].extr->lastcommand, 14);
			for(j=0; j<15; j++)
				if(str[j] == ' ') 
				{
					str[j] = 0;
	                break;
		        }
			if(!str[0])
				output(fd, "Logged in\n");
	        else
			{
		        output_nl(fd, str);
			}
		}
		else {
			ANSI(fd, AFC_CYAN);
	        sprintf(g_buffer, "%-12s ", get_race_string(Ply[i].ply->race));
			output(fd, g_buffer );
			ANSI(fd, AFC_WHITE);
		}

 		if(F_ISSET(Ply[i].ply, PPLDGK)) 
		{   
			ANSI(fd, AFC_GREEN);
			sprintf(g_buffer, "%-20s \n", cur_guilds[check_guild(Ply[i].ply)].name);
			output(fd, g_buffer );
			ANSI(fd, AFC_WHITE);
		}
		else 
		{
			if(ply_ptr->type == CARETAKER || ply_ptr->type == DM)
				sprintf(g_buffer, "%-20s\n", "The Ancient Ones");
			else
				sprintf(g_buffer, "%-20s\n", "None");
			output(fd, g_buffer );
		}




    }
    output(fd, "\n");

    return(0);

}
 
/**********************************************************************/
/*                              whois                                 */
/**********************************************************************/
/* The whois function displays a selected player's name, class, level *
 * title, age and gender */
      
int whois(creature *ply_ptr, cmd *cmnd )
{
    creature    *crt_ptr;
    int     fd;
 
    fd = ply_ptr->fd;
 
    if(cmnd->num < 2) {
        output(fd, "Whois who?\n");
        return(0);
    }
 
    lowercize(cmnd->str[1], 1);
    crt_ptr = find_who(cmnd->str[1]);
 
    if(!crt_ptr || F_ISSET(crt_ptr, PDMINV) || F_ISSET(ply_ptr, PBLIND) ||
       (F_ISSET(crt_ptr, PINVIS) && !F_ISSET(ply_ptr, PDINVI))) {
        output(fd, "That player is not logged on.\n");
        return(0);
    }
 
	ANSI(fd, AFC_YELLOW); 
    sprintf(g_buffer, "%-20s  %-3s %-3s [Lv]%-20s  %-3s  %-10s\n", "Player", "Cls", "Gen", "Title", "Age", "Race");
	output(fd, g_buffer);
    output_wc(fd, "------------------------------------------------------------------------\n",
		AFC_BLUE);
    sprintf(g_buffer, "%-20s  %-3.3s %-3s [%02d]%-20s  %-3d  ",
                crt_ptr->name,
                get_class_string(crt_ptr->class),
                F_ISSET(crt_ptr, PMALES) ? " M":" F",  
                crt_ptr->level,
                title_ply(crt_ptr), 
                (int)(18 + crt_ptr->lasttime[LT_HOURS].interval/86400L));
	output(fd, g_buffer );

	sprintf(g_buffer, "%-10s\n", get_race_string(crt_ptr->race));
	output(fd, g_buffer );
 	ANSI(fd, AM_NORMAL);
 	ANSI(fd, AFC_WHITE);
    return(0);
}

/**********************************************************************/
/*              search                    */
/**********************************************************************/

/* This function allows a player to search a room for hidden objects,  */
/* exits, monsters and players.                        */

int search(creature *ply_ptr, cmd *cmnd )
{
    room    *rom_ptr;
    xtag    *xp;
    otag    *op;
    ctag    *cp;
    time_t	i, t;
    int fd, chance, found = 0;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    chance = 15 + 5*bonus[(int)ply_ptr->piety] + ply_ptr->level*2;
    chance = MIN(chance, 90);
    if(ply_ptr->class == RANGER)
        chance += ply_ptr->level*8;
    if(ply_ptr->class == DRUID)
		chance += ply_ptr->level*6;
    if(F_ISSET(ply_ptr, PBLIND))
		chance = MIN(chance, 20);
    if(ply_ptr->class >= BUILDER)
        chance = 100;

    t = time(0);
    i = LT(ply_ptr, LT_SERCH);

    if(t < i) {
        please_wait(fd, i-t);
        return(0);
    }

    F_CLR(ply_ptr, PHIDDN);

    ply_ptr->lasttime[LT_SERCH].ltime = t;
	if ( ply_ptr->class == DM )
        ply_ptr->lasttime[LT_SERCH].interval = 0;
    else if(ply_ptr->class == RANGER || ply_ptr->class == DRUID)
        ply_ptr->lasttime[LT_SERCH].interval = 3;
    else
        ply_ptr->lasttime[LT_SERCH].interval = 7;

    xp = rom_ptr->first_ext;
    while(xp) {
        if(F_ISSET(xp->ext, XSECRT) && mrand(1,100) <= chance) {
           if( (F_ISSET(xp->ext, XINVIS) && F_ISSET(ply_ptr,PDINVI)) ||
               (!F_ISSET(xp->ext, XINVIS) && !F_ISSET(xp->ext, XNOSEE)) ) {
            found++;
            sprintf(g_buffer, "You found an exit: %s.\n", xp->ext->name);
			output(fd, g_buffer );
           }
	}
        xp = xp->next_tag;
    }

    op = rom_ptr->first_obj;
    while(op) {
        if(F_ISSET(op->obj, OHIDDN) && mrand(1,100) <= chance) {
          if( (F_ISSET(op->obj, OINVIS) && F_ISSET(ply_ptr,PDINVI)) ||
            !F_ISSET(op->obj, OINVIS) ){
            found++;
            mprint(fd, "You found %1i.\n", m1arg(op->obj));
          }
	}
        op = op->next_tag;
    }

    cp = rom_ptr->first_ply;
    while(cp) {
        if(F_ISSET(cp->crt, PHIDDN) && !F_ISSET(cp->crt, PDMINV) &&
           mrand(1,100) <= chance) {
          if( (F_ISSET(cp->crt, PINVIS) && F_ISSET(ply_ptr,PDINVI)) ||
              !F_ISSET(cp->crt, PINVIS) ){
            found++;
            sprintf(g_buffer, "You found %s hiding.\n", cp->crt->name);
			output(fd, g_buffer );
	  }
        }
        cp = cp->next_tag;
    }

    cp = rom_ptr->first_mon;
    while(cp) {
        if(F_ISSET(cp->crt, MHIDDN) && mrand(1,100) <= chance) {
          if( (F_ISSET(cp->crt, MINVIS) && F_ISSET(ply_ptr,PDINVI)) ||
              !F_ISSET(cp->crt, MINVIS) ){
            found++;
            mprint(fd, "You found %1m hiding.\n", m1arg(cp->crt));
	  }
        }
        cp = cp->next_tag;
    }

    broadcast_rom(fd, ply_ptr->rom_num, "%M searches the room.", 
		m1arg(ply_ptr));

    if(found)
	{
        sprintf(g_buffer, "%s found something!", 
                  F_ISSET(ply_ptr, MMALES) ? "He":"She");
        broadcast_rom(fd, ply_ptr->rom_num, g_buffer, NULL);
	}
    else
        output(fd, "You didn't find anything.\n");

    return(0);

}

/**********************************************************************/
/*              ply_suicide               */
/**********************************************************************/

/* This function is called whenever the player explicitly asks to     */
/* commit suicide.  It then calls the suicide() function which takes  */
/* over that player's input.                          */

int ply_suicide(creature *ply_ptr, cmd *cmnd )
{
    suicide(ply_ptr->fd, 1, "");
    return(DOPROMPT);
}

/**********************************************************************/
/*              suicide                   */
/**********************************************************************/

/* This function allows a player to kill himself, thus erasing his entire */
/* player file.                               */

void suicide(int fd, int param, char *str )
{
    char    file[80];

    switch(param) {
        case 1:
			ANSI(fd, AM_BOLD);
			ANSI(fd, AM_BLINK);
			ANSI(fd, AFC_RED);
            output(fd, "This will completely erase your player.\n");
            output(fd, "Are you sure (Y/N)\n");
		    ANSI(fd, AM_NORMAL);
		    ANSI(fd, AFC_WHITE);
            RETURN(fd, suicide, 2);
        case 2:
            if(low(str[0]) == 'y') {
                sprintf(g_buffer, "### %s committed suicide! We'll miss %s dearly.", Ply[fd].ply->name, F_ISSET(Ply[fd].ply, PMALES) ? "him":"her");
                broadcast( g_buffer );
                sprintf(file, "%s/%s", get_player_path(), Ply[fd].ply->name);
				if(SUICIDE) {
					sprintf(g_buffer,"%s-%d (%s@%s)", Ply[fd].ply->name, 
						Ply[fd].ply->level, Ply[fd].io->userid, 
						Ply[fd].io->address);
					logn("SUICIDE", g_buffer);
				}
				disconnect(fd);
                unlink(file);

				/* remove mail file if its exists */
				sprintf(file, "%s/%s", get_post_path(), Ply[fd].ply->name);
				unlink(file);
                return;
            }
            else {
                output(fd, "Aborted.\n");
                RETURN(fd, command, 1);
            }
    }
}

/**********************************************************************/
/*              hide                      */
/**********************************************************************/

/* This command allows a player to try and hide himself in the shadows */
/* or it can be used to hide an object in a room.              */

int hide(creature *ply_ptr, cmd *cmnd )
{
    room    *rom_ptr;
    object  *obj_ptr;
    ctag    *cp;
    time_t    i, t;
    int fd, chance;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    i = LT(ply_ptr, LT_HIDES);
    t = time(0);

    if(i > t) {
        please_wait(fd, i-t);
        return(0);
    }

    
    cp = rom_ptr->first_mon;
    while(cp) {
	if(is_enm_crt(ply_ptr->name, cp->crt)) {
		output(fd, "How can you do that right now?\n");
		return(0);
	}
	cp = cp->next_tag;
    }

    ply_ptr->lasttime[LT_HIDES].ltime = t;
    ply_ptr->lasttime[LT_HIDES].interval = (ply_ptr->class == THIEF ||
        ply_ptr->class == ASSASSIN || ply_ptr->class == RANGER) ? 5:15;

    if(cmnd->num == 1) {

		if ( ply_ptr->class == DM )
			chance = 100;
        else if(ply_ptr->class == THIEF || ply_ptr->class == ASSASSIN)
            chance = MIN(90, 5 + 6*ply_ptr->level + 
                3*bonus[(int)ply_ptr->dexterity]);
        else if(ply_ptr->class == RANGER || ply_ptr->class == DRUID)
            chance = 5 + 10*ply_ptr->level +
                3*bonus[(int)ply_ptr->dexterity];
        else
            chance = MIN(90, 5 + 2*ply_ptr->level +
                3*bonus[(int)ply_ptr->dexterity]);


        output(fd, "You attempt to hide in the shadows.\n");

		if(F_ISSET(ply_ptr, PBLIND))
			chance = MIN(chance, 20);

		if(F_ISSET(ply_ptr, PDMOWN))
			chance=0;

	if(mrand(1,100) <= chance) {
		F_SET(ply_ptr, PHIDDN);
		output(fd, "You slip into the shadows unnoticed.\n");
	}
        else {
            F_CLR(ply_ptr, PHIDDN);
            broadcast_rom(fd, ply_ptr->rom_num,
                  "%M tries to hide in the shadows.", m1arg(ply_ptr));
        }


        return(0);

    }

    obj_ptr = find_obj(ply_ptr, rom_ptr->first_obj,
               cmnd->str[1], cmnd->val[1]);

    if(!obj_ptr) {
        output(fd, "I don't see that here.\n");
        return(0);
    }

    if(F_ISSET(obj_ptr,ONOTAK)){
		output(fd,"You can not hide that.\n");
		return (0);
	}

	if ( ply_ptr->class == DM )
		chance = 100;
    else if(ply_ptr->class == THIEF || ply_ptr->class == ASSASSIN)
        chance = MIN(90, 10 + 5*ply_ptr->level + 
            5*bonus[(int)ply_ptr->dexterity]);
    else if(ply_ptr->class == RANGER || ply_ptr->class == DRUID)
        chance = 5 + 9*ply_ptr->level +
            3*bonus[(int)ply_ptr->dexterity];
    else
        chance = MIN(90, 5 + 3*ply_ptr->level + 
            3*bonus[(int)ply_ptr->dexterity]);


    output(fd, "You attempt to hide it.\n");
    broadcast_rom(fd, ply_ptr->rom_num, "%M attempts to hide %1i.", 
              m2args(ply_ptr, obj_ptr));

    if(mrand(1,100) <= chance) {
        F_SET(obj_ptr, OHIDDN);
		mprint(fd, "You hide %1i.\n", m1arg(obj_ptr));
		}
    else
        F_CLR(obj_ptr, OHIDDN);

    return(0);

}

/************************************************************************/
/************************************************************************/

/*  Display information on creature given to player given.		*/

int flag_list(creature *ply_ptr, cmd *cmnd )
{
	char	str[1024], temp[20];
	int		fd;

	str[0] = 0;
	temp[0] = 0;

	fd = ply_ptr->fd;

	output(fd,"Flags currently set:\n");
	if(F_ISSET(ply_ptr, PIGCLA)) 
		strcat(str, "No Class Messages\n");
	if(F_ISSET(ply_ptr, PIGGLD)) 
		strcat(str, "No Guild Messages\n");
	if(F_ISSET(ply_ptr, PNOBRD)) 
		strcat(str, "NoBroad\n");
	if(F_ISSET(ply_ptr, PNOLDS)) 
		strcat(str, "No Long Room Description\n");
	if(F_ISSET(ply_ptr, PNOSDS)) 
		strcat(str, "No Short Room Description\n");
	if(F_ISSET(ply_ptr, PNORNM)) 
		strcat(str, "No Name\n");
	if(F_ISSET(ply_ptr, PNOEXT)) 
		strcat(str, "No Exits\n");
	if(F_ISSET(ply_ptr, PLECHO)) 
		strcat(str, "Communications echo\n");
	if(F_ISSET(ply_ptr, PNLOGN)) 
		strcat(str, "No Login messages\n");
	if(F_ISSET(ply_ptr, PNOCMP)) 
		strcat(str, "Non-compact\n");
	if(F_ISSET(ply_ptr, PWIMPY)) {
		sprintf(temp, "Wimpy%d\n", ply_ptr->WIMPYVALUE);
		strcat(str, temp);
	}
	if(F_ISSET(ply_ptr, PPROMP)) 
		strcat(str, "Prompt\n");
	if(F_ISSET(ply_ptr, PANSIC)) 
		strcat(str, "Ansi output mode\n");
	if(F_ISSET(ply_ptr, PIGNOR)) 
		strcat(str, "Ignore all sends\n");
	if(F_ISSET(ply_ptr, PNOSUM)) 
		strcat(str, "Nosummon\n");
    if(F_ISSET(ply_ptr, PNOAAT)) 
		strcat(str, "No Auto Attack\n"); 
	if(F_ISSET(ply_ptr, PHASTE)) 
		strcat(str, "Haste\n");
	if(F_ISSET(ply_ptr, PPRAYD)) 
		strcat(str, "Pray\n");
	if(F_ISSET(ply_ptr, PBRIEF))
		strcat(str, "Brief\n");
	if(F_ISSET(ply_ptr, PSOUND))
		strcat(str, "Sound\n");
	if(F_ISSET(ply_ptr, PPLDGK)) {
                      sprintf(temp, "%s ", cur_guilds[check_guild(ply_ptr)].name);
                      strcat(str, temp);
	}

	output_nl(fd, str);

    output(fd, "Type help set to see a complete list of flags.\n");

    return(0);
}
/************************************************************************/
/*              set							*/
/************************************************************************/

/* This function allows a player to set certain one-bit flags.  The flags */
/* are settings for options that include brief and verbose display.  The  */
/* clear function handles the turning off of these flags.         */

int set( creature *ply_ptr, cmd *cmnd )
{
    int fd;
    
    fd = ply_ptr->fd;

    if(cmnd->num == 1) {
		flag_list(ply_ptr,cmnd);
    	return(0);
    }


	/* CT and DM only flags */
	if (ply_ptr->class >= CARETAKER )
	{
	    if(!strcmp(cmnd->str[1], "eavesdropper")) {
			F_SET(ply_ptr, PEAVES);
			output(fd, "Eavesdropper mode enabled.\n");
			return(0);
		}
	    else if(!strcmp(cmnd->str[1], "~robot~")) {
		    F_SET(ply_ptr, PROBOT);
			output(fd, "Robot mode on.\n");
			return(0);
		}
	}


	/* flags all chars can use */
    if(!strcmp(cmnd->str[1], "classignore")) {
	F_SET(ply_ptr, PIGCLA);
	output(fd, "Class messages disabled.\n");
    }
    if(!strcmp(cmnd->str[1], "guildignore")) {
	F_SET(ply_ptr, PIGGLD);
	output(fd, "Guild messages disabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "nobroad")) {
        F_SET(ply_ptr, PNOBRD);
        output(fd, "Broadcast messages disabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "compact")) {
        F_CLR(ply_ptr, PNOCMP);
        output(fd, "Compact mode enabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "roomname")) {
        F_CLR(ply_ptr, PNORNM);
        output(fd, "Room name output enabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "short")) {
        F_CLR(ply_ptr, PNOSDS);
        output(fd, "Short descriptions enabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "long")) {
        F_CLR(ply_ptr, PNOLDS);
        output(fd, "Long descriptions enabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "hexline")) {
        F_SET(ply_ptr, PHEXLN);
        output(fd, "Hexline enabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "nologin")) {
        F_SET(ply_ptr, PNLOGN);
        output(fd, "Login messages disabled.\n");
    } 
    else if(!strcmp(cmnd->str[1], "echo")) {
        F_SET(ply_ptr, PLECHO);
        output(fd, "Communications echo enabled.\n");
    }

    else if(!strcmp(cmnd->str[1], "wimpy")) {
        F_SET(ply_ptr, PWIMPY);
        output(fd, "Wimpy mode enabled.\n");
        ply_ptr->WIMPYVALUE = (short)(cmnd->val[1] == 1L ? 10 : cmnd->val[1]);
        ply_ptr->WIMPYVALUE = MAX(ply_ptr->WIMPYVALUE, 2);
        sprintf(g_buffer, "Wimpy value set to (%d).\n", ply_ptr->WIMPYVALUE);
		output(fd, g_buffer);
    }
    else if(!strcmp(cmnd->str[1], "prompt")) {
        F_SET(ply_ptr, PPROMP);
        output(fd, "Prompt in descriptive format.\n");
    }
    else if(!strcmp(cmnd->str[1], "ansi")) {
        F_SET(ply_ptr, PANSIC);
        output(fd, "Color ANSI mode on.\n");
    }
    else if(!strcmp(cmnd->str[1], "nosummon")) {
        F_SET(ply_ptr, PNOSUM);
        output(fd, "Nosummon flag on.\n");
    }
    else if(!strcmp(cmnd->str[1], "ignore")) {
        F_SET(ply_ptr, PIGNOR);
        output(fd, "Ignore all flag on.\n");
    }
    else if(!strcmp(cmnd->str[1], "noauto")) {
        F_SET(ply_ptr, PNOAAT);
        output(fd, "Auto Attack is turned off.\n");
    }
    else if(!strcmp(cmnd->str[1], "brief")) {
	F_SET(ply_ptr, PBRIEF);
	output(fd, "Brief combat mode on.\n");
    }
    else if(!strcmp(cmnd->str[1], "sound")) {
	F_SET(ply_ptr, PSOUND);
	output(fd, "Mud Sound Protocol on.\n");
    }
    else
        output(fd, "Unknown flag.\n");

    return(0);

}

/**********************************************************************/
/*              clear                     */
/**********************************************************************/

/* Like set, this function allows a player to alter the value of a part- */
/* icular player flag.                           */

int clear(creature *ply_ptr, cmd *cmnd )
{
    int fd;
    
    fd = ply_ptr->fd;

    if(cmnd->num == 1) {
        output(fd, "Type help clear to see a list of flags.\n");
        return(0);
    }

    if(!strcmp(cmnd->str[1], "classignore")) {
	F_CLR(ply_ptr, PIGCLA);
	output(fd, "Class Messages enabled.\n");
    }
    if(!strcmp(cmnd->str[1], "guildignore")) {
	F_CLR(ply_ptr, PIGGLD);
	output(fd, "Guild Messages enabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "nobroad")) {
        F_CLR(ply_ptr, PNOBRD);
        output(fd, "Broadcast messages enabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "compact")) {
        F_SET(ply_ptr, PNOCMP);
        output(fd, "Compact mode disabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "echo")) {
        F_CLR(ply_ptr, PLECHO);
        output(fd, "Communications echo disabled.\n");
    }

    else if(!strcmp(cmnd->str[1], "nologin")) {
        F_CLR(ply_ptr, PNLOGN);
        output(fd, "Login messages enabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "roomname")) {
        F_SET(ply_ptr, PNORNM);
        output(fd, "Room name output disabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "short")) {
        F_SET(ply_ptr, PNOSDS);
        output(fd, "Short descriptions disabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "long")) {
        F_SET(ply_ptr, PNOLDS);
        output(fd, "Long descriptions disabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "hexline")) {
        F_CLR(ply_ptr, PHEXLN);
        output(fd, "Hex line disabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "wimpy")) {
        F_CLR(ply_ptr, PWIMPY);
        output(fd, "Wimpy mode disabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "eavesdropper")) {
        F_CLR(ply_ptr, PEAVES);
        output(fd, "Eavesdropper mode disabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "prompt")) {
        F_CLR(ply_ptr, PPROMP);
        output(fd, "Prompt in brief mode.\n");
    }
    else if(!strcmp(cmnd->str[1], "~robot~")) {
        F_CLR(ply_ptr, PROBOT);
        output(fd, "Robot mode off.\n");
    }
    else if(!strcmp(cmnd->str[1], "ansi")) {
        F_CLR(ply_ptr, PANSIC);
        output(fd, "Color ANSI off.\n");
    }
    else if(!strcmp(cmnd->str[1], "nosummon")) {
        F_CLR(ply_ptr, PNOSUM);
        output(fd, "Nosummon flag off.\n");
    }
    else if(!strcmp(cmnd->str[1], "ignore")) {
        F_CLR(ply_ptr, PIGNOR);
        output(fd, "Ignore all flag off.\n");
    }
    else if(!strcmp(cmnd->str[1], "noauto")) {
        F_CLR(ply_ptr, PNOAAT);
        output(fd, "Auto attack mode enabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "brief")) {
	F_CLR(ply_ptr, PBRIEF);
	output(fd, "Brief combat mode off.\n");
    }
    else if(!strcmp(cmnd->str[1], "sound")) {
	F_CLR(ply_ptr, PSOUND);
	output(fd, "Mud Sound Protocol off.\n");
    }
    else
        output(fd, "Unknown flag.\n");

    return(0);

}

/**********************************************************************/
/*              quit                      */
/**********************************************************************/

/* This function checks to see if a player is allowed to quit yet.  It  */
/* checks to make sure the player isn't in the middle of combat, and if */
/* so, the player is not allowed to quit (and 0 is returned).       */

int quit(creature *ply_ptr, cmd *cmnd )
{
    time_t    attack, spell, i, t;
    int fd;
    fd = ply_ptr->fd;

    t = time(0);
    attack = LT(ply_ptr, LT_ATTCK) + 20;
    spell = LT(ply_ptr, LT_SPELL) + 20;
	i = MAX(attack, spell );
	
    if(t < i) {
        please_wait(fd, i-t);
        return(0);
    }

    update_ply(ply_ptr);
    return(DISCONNECT);
}
