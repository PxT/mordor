/***************************************************************************
 *
 *  Combat.c - Combat routines
 *
 *	Copyright 1994, 1999  Brooke Paul
 *
 *  combat.c modified by Charles Marchant
 *  Jan 12 1995 to replace old combat
 * $Id: combat.c,v 6.22 2001/07/22 19:17:17 develop Exp $
 *
 * $Log: combat.c,v $
 * Revision 6.22  2001/07/22 19:17:17  develop
 * removed player from stolen list if they are attacked
 *
 * Revision 6.21  2001/07/22 19:03:06  develop
 * first run at alllowing thieves to steal gold from other players
 *
 * Revision 6.20  2001/07/03 23:05:01  develop
 * more grammar fixes
 *
 * Revision 6.19  2001/07/03 22:47:47  develop
 * grammar fix in monster v. monster
 *
 * Revision 6.18  2001/07/01 01:08:23  develop
 * made it so DM Owned/Outcast can always be attacked
 *
 * Revision 6.17  2001/06/27 01:12:44  develop
 * *** empty log message ***
 *
 * Revision 6.16  2001/06/23 04:23:47  develop
 * removed meditate during combat
 *
 * Revision 6.15  2001/06/02 04:54:02  develop
 * fixed output on monsters circling with MNOPRE
 *
 * Revision 6.14  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */


#include "../include/mordb.h"
#include "mextern.h"


int update_combat( creature *crt_ptr )
{
   creature *att_ptr;
   room     *rom_ptr;
   etag     *ep;
   char     *enemy;
   int      n = 0,rtn = 0,p = 0,fd,t = 0;
   int		victim_is_monster = 0, circled=0, tried_circle=0;
   
	ASSERTLOG( crt_ptr );

	rom_ptr = crt_ptr->parent_rom;
	ep = crt_ptr->first_enm;
	while(1)
	{
		enemy = ep->enemy;
		if(!enemy)
			ep = ep->next_tag;
		if(!ep)
			return 0;
		if(enemy)
			break;
	}


	/* keep looking till we find one that is an exact match */
	/* and is not the monster itself */
	n = 1;
	do 
	{

		att_ptr = find_exact_crt(crt_ptr, rom_ptr->first_ply,enemy, n);
		if(!att_ptr)
			att_ptr = find_exact_crt(crt_ptr, rom_ptr->first_mon,enemy, n);
		if(!att_ptr)
		{	
#ifdef OLD_CODE
			/* with this code commented out, a monster will remember it is
			attacking a player even if he logs out and logs back in unlike
			now where he can get someone else to stadn there and attack,
			disconnect, login attack again etc. */
			if(!find_who(enemy))
				del_enm_crt(enemy, crt_ptr);
			else
#endif
				end_enm_crt(enemy, crt_ptr);
			return 0;
		}

		n++;

	} while ( att_ptr != NULL && att_ptr == crt_ptr );

  
	if ( att_ptr->type == MONSTER)
	{
		victim_is_monster = 1;
	}
 
	/* I think this check is redundanct now */
	if(att_ptr != crt_ptr) 
	{
       if(is_charm_crt(crt_ptr->name, att_ptr)&& F_ISSET(crt_ptr, MCHARM))
	       p = 1;
		crt_ptr->NUMHITS++;
		n=20;
		if(F_ISSET(crt_ptr, MMAGIO))
			n=crt_ptr->proficiency[0];
	         
		if(F_ISSET(crt_ptr, MMAGIC) && mrand(1, 100) <= n && !p) 
		{
			rtn = crt_spell(crt_ptr, att_ptr );
			if(rtn == 2) 
			{
				/* spell killed the victim */
				return 1; 
			}
			else if(rtn == 1)
			{
				/* spell hurt the victim but did not kill */
				n = 21;
				if ( !victim_is_monster )
				{
					if ( check_for_ply_flee( att_ptr ))
					{
						return(1);
					}
				}
			}
			else 
			{
				/* spell was not cast or did not hurt the victim */
      			n = crt_ptr->thaco - att_ptr->armor/10;
			}
		}
		else 
		{
			n = crt_ptr->thaco - att_ptr->armor/10;
			n = MAX(n, 1);
		}
   
   
		if(mrand(1,20) >= n && !p) 
		{
			if ( !victim_is_monster )
			{
				fd = att_ptr->fd;
				ANSI(fd, ATTACKCOLOR); 
			}
			else
			{
				fd = -1;
			}
			
			if((crt_ptr->class==BARBARIAN||crt_ptr->class==FIGHTER)
				&& (mrand(1,30)<20 &&
				crt_ptr->level > (att_ptr->level - 3))) {
			   circled = (crt_ptr->level - att_ptr->level) +
				mrand(1,4);
			   if(circled>1) {
				mprint(fd, "%M circles you.\n", m1arg(crt_ptr));
			
				if(F_ISSET(crt_ptr, MNOPRE)) { 
				  if(F_ISSET(att_ptr, MNOPRE)) {
					sprintf(g_buffer, "%s circles %s.", crt_ptr->name, att_ptr->name);
				  } else {
					sprintf(g_buffer, "%s circles the %s.", crt_ptr->name, att_ptr->name);
				  }
				} 
				else { /* !F_ISSET(crt_ptr,MNOPRE) */
				  if(F_ISSET(att_ptr, MNOPRE)) {
					sprintf(g_buffer, "The %s circles %s.", crt_ptr->name, att_ptr->name);
				  } else {
					sprintf(g_buffer, "The %s circles the %s.", crt_ptr->name, att_ptr->name);
				  }
				}
				broadcast_dam(fd, crt_ptr->fd, att_ptr->rom_num, 
					g_buffer, NULL);

				att_ptr->lasttime[LT_ATTCK].ltime = time(0);
				att_ptr->lasttime[LT_ATTCK].interval = mrand(5,8) - 
					bonus[(int)att_ptr->dexterity];
				circled=1;
			   }
			   else {
				mprint(fd, "%M tried to circle you.\n", m1arg(crt_ptr));
				if(F_ISSET(crt_ptr, MNOPRE)) {
				   if(F_ISSET(att_ptr,MNOPRE)) {
					sprintf(g_buffer, "%s tried to circle %s.", crt_ptr->name, att_ptr->name);
				   } else {
					sprintf(g_buffer, "%s tried to circle the %s.", crt_ptr->name, att_ptr->name);
				   }
				}
				else { /*!F_ISSET(crt_ptr, MNOPRE) */
				   if(F_ISSET(att_ptr,MNOPRE)) {
					sprintf(g_buffer, "The %s tried to circle %s.", crt_ptr->name, att_ptr->name);
				   } else {
					sprintf(g_buffer, "The %s tried to circle the %s.", crt_ptr->name, att_ptr->name);
				   }
				}
				broadcast_dam(fd, crt_ptr->fd, att_ptr->rom_num, 
					g_buffer, NULL);

				circled=0;
				tried_circle=1;
			   }
				
			}
		
			if(F_ISSET(crt_ptr, MBRETH) && mrand(1,30)<5)
			{
				if (F_ISSET(crt_ptr, MBRWP1) && !F_ISSET(crt_ptr, MBRWP2))
					n = bw_spit_acid(crt_ptr,att_ptr);
				else if (F_ISSET(crt_ptr, MBRWP1) && F_ISSET(crt_ptr, MBRWP2))
					n = bw_poison(crt_ptr,att_ptr);
				else if (!F_ISSET(crt_ptr, MBRWP1) && F_ISSET(crt_ptr, MBRWP2))
					n = bw_cone_frost(crt_ptr,att_ptr);
				else
					n = bw_cone_fire(crt_ptr,att_ptr);
			}
			
			else if(F_ISSET(crt_ptr, MENEDR) && mrand(1,100)< 25) 
				n = power_energy_drain(crt_ptr,att_ptr);
			else {
			    if(circled==0) {
				n = mdice(crt_ptr);		     
			    }
			}
    
            
		if(circled==0 && tried_circle==0) {
			att_ptr->hpcur -= n;

			if ( !victim_is_monster )
			{
				sprintf(g_buffer, "%%M hit you for %d damage.\n", n);
				mprint(fd, g_buffer, m1arg(crt_ptr));
				add_enm_dmg(att_ptr->name, crt_ptr,n);
			
				// added for damage descriptions 
				if(F_ISSET(crt_ptr, MNOPRE))
					sprintf(g_buffer, "%s %s %s!",crt_ptr->name , 
						hit_description(n), att_ptr->name);
				else
					sprintf(g_buffer,"The %s %s %s!", crt_ptr->name,
						hit_description(n), att_ptr->name);
				
				broadcast_dam(crt_ptr->fd, att_ptr->fd, att_ptr->rom_num, g_buffer, NULL);


			}
			
			else
			{ 
				/* Output only when monster v. monster */

				broadcast_rom(-1,crt_ptr->rom_num, "%M hits %m.", m2args(crt_ptr, att_ptr));
				
				add_enm_crt(crt_ptr->name, att_ptr);
			}
		}

			if(F_ISSET(crt_ptr, MPOISS) && mrand(1,100) <= 15) 
			{
				if ( victim_is_monster )
				{
        			broadcast_rom(-1, att_ptr->rom_num,"%M poisoned %m.", 
						m2args(crt_ptr, att_ptr));
				}
				else
				{
					ANSI(fd, POISONCOLOR);
					mprint(fd, "%M poisoned you.\n", m1arg(crt_ptr));
				}
				F_SET(att_ptr, PPOISN);
			}

			if(F_ISSET(crt_ptr, MDISEA) && mrand(1,100) <= 15) 
			{

				if ( victim_is_monster )
				{
        			broadcast_rom(-1, att_ptr->rom_num,"%M infects %m.", 
						m2args(crt_ptr, att_ptr));
				}
				else
				{
					ANSI(fd, DISEASECOLOR);
					mprint(fd, "%M infects you.\n", m1arg(crt_ptr));
				}
				F_SET(att_ptr, PDISEA);
			}
		
			if(F_ISSET(crt_ptr, MBLNDR) && mrand(1,100) <= 15) 
			{
				if ( victim_is_monster )
				{
        			broadcast_rom(-1, att_ptr->rom_num,"%M blinds %m.", 
						m2args(crt_ptr, att_ptr));
				}
				else
				{
					ANSI(fd, BLINDCOLOR);
					mprint(fd, "%M  blinds your eyes.\n", m1arg(crt_ptr));
				}
				F_SET(att_ptr, PBLIND);
			}
  
			if(F_ISSET(crt_ptr, MDISIT) && mrand(1,100) <= 15) 
				dissolve_item(att_ptr,crt_ptr);
                
			if ( !victim_is_monster )
			{
				ANSI(fd, MAINTEXTCOLOR);

				n = choose_item(att_ptr);

				if(n) 
				{
					if(--att_ptr->ready[n-1]->shotscur<1) 
					{
						sprintf(g_buffer,"Your %s fell apart.\n",att_ptr->ready[n-1]->name);
						output(fd, g_buffer);
						sprintf(g_buffer, "%%M's %s fell apart.", att_ptr->ready[n-1]->name);
						broadcast_rom(fd,att_ptr->rom_num, g_buffer,
							  m1arg(att_ptr));
						add_obj_crt(att_ptr->ready[n-1],att_ptr);
						dequip(att_ptr,att_ptr->ready[n-1]);
						att_ptr->ready[n-1] = 0;
						compute_ac(att_ptr);
					}
				}
			}
        
			if(!victim_is_monster && !F_ISSET(att_ptr, PNOAAT) && !p)
			{      
				rtn = attack_crt(att_ptr, crt_ptr);
				if(rtn) 
					return 1;
			}
			else 
			{      
				if(LT(att_ptr,LT_ATTCK) < t)
				{				   
					rtn = attack_crt(att_ptr, crt_ptr);
					att_ptr->lasttime[LT_ATTCK].ltime = t;
					if(rtn) 
						return 1;
				}
			}
                
			if(att_ptr->hpcur < 1) 
			{
				if ( !victim_is_monster )
				{
					msprint(att_ptr, g_buffer, "%M killed you.\n", m1arg(crt_ptr));
					output_wc(att_ptr->fd, g_buffer, ATTACKCOLOR); 
				}
				else
				{
					broadcast_rom(-1,att_ptr->rom_num,"%M killed %m.",
                       				m2args(crt_ptr, att_ptr));
				}
				die(att_ptr, crt_ptr);
				return 1;
			}
			else if ( !victim_is_monster )
			{
				if ( check_for_ply_flee( att_ptr ))
				{
					return(1);
				}
			}
		}
		else if(n <= 20 && !p) 
		{
			if ( !victim_is_monster )
			{
				msprint(att_ptr, g_buffer, "%M missed you.\n", 
					m1arg(crt_ptr));
				output_wc(att_ptr->fd, g_buffer, MISSEDCOLOR);
			}
			else
			{
				/* Output only when monster v. monster */
				broadcast_rom(-1, att_ptr->rom_num, "%M misses %m.", 
					m2args(crt_ptr, att_ptr));
				add_enm_crt(crt_ptr->name, att_ptr);
			}
			if(!victim_is_monster && !F_ISSET(att_ptr, PNOAAT))
			{      
				rtn = attack_crt(att_ptr, crt_ptr);
				if(rtn) 
					return 1;
			}
			else 
			{      
				if(LT(att_ptr,LT_ATTCK) < t)
				{			
					rtn = attack_crt(att_ptr, crt_ptr);
					att_ptr->lasttime[LT_ATTCK].ltime = t;
					if(rtn) 
						return 1;
				}
			}
		}
	}
  return 0;
}


int check_for_ply_flee(creature *ply_ptr)
{
	int	nReturn = 0;

	ASSERTLOG( ply_ptr != NULL );
	ASSERTLOG( ply_ptr->type == PLAYER );

	if(F_ISSET(ply_ptr, PWIMPY)) 
		if(ply_ptr->hpcur <= ply_ptr->WIMPYVALUE) 
		{
			flee(ply_ptr, NULL);
			nReturn = 1;
		}


	return(nReturn);
}

/***************************************************************************
 *  breath weapons 
 */

int bw_spit_acid(creature *crt_ptr, creature *vic_ptr )
{

	int fd;
	int n;

	ASSERTLOG( crt_ptr );
	ASSERTLOG( vic_ptr );

	fd = vic_ptr->fd;
	if ( vic_ptr->type == PLAYER )
		mprint(fd, "%M spits acid on you!\n", m1arg(crt_ptr));

	broadcast_rom(fd,vic_ptr->rom_num,"%M spits acid on %m!",
		m2args(crt_ptr,vic_ptr));
	n = dice(crt_ptr->level, 3, 0);
	
	return n;
}


int bw_poison(creature *crt_ptr, creature *vic_ptr )
{

	int fd;
	int n;

	ASSERTLOG( crt_ptr );
	ASSERTLOG( vic_ptr );

	fd = vic_ptr->fd;
	if ( vic_ptr->type == PLAYER )
		mprint(fd, "%M breathes poisonous gas on you!\n", m1arg(crt_ptr));

	broadcast_rom(fd, vic_ptr->rom_num, "%M breathes poisonous gas on %m!", 
		m2args(crt_ptr, vic_ptr));
	n = dice(crt_ptr->level, 2, 1);

	if ( vic_ptr->type == PLAYER )
	{
		mprint(fd, "%M poisoned you.\n", m1arg(crt_ptr));
	}
	else
	{
		broadcast_rom(-1, vic_ptr->rom_num,
		             "%M poisoned %m!", m2args(crt_ptr, vic_ptr));
	}
	F_SET(vic_ptr, PPOISN);
	return n;
}

int bw_cone_frost( creature *crt_ptr, creature *vic_ptr )
{

	int fd;
	int n;

	ASSERTLOG( crt_ptr );
	ASSERTLOG( vic_ptr );

	fd = vic_ptr->fd;
	if ( vic_ptr->type == PLAYER )
		mprint(fd, "%M breathes frost on you!\n", m1arg(crt_ptr));

	broadcast_rom(fd,vic_ptr->rom_num,"%M breathes frost on %m!",
		 m2args(crt_ptr,vic_ptr));

	if(vic_ptr->type == PLAYER && F_ISSET(vic_ptr, PRCOLD))
		n = dice(crt_ptr->level, 2, 0);
	else
		n = dice(crt_ptr->level, 4, 0);

	return n;
}

int bw_cone_fire( creature *crt_ptr, creature *vic_ptr )
{

	int fd;
	int n;

	ASSERTLOG( crt_ptr );
	ASSERTLOG( vic_ptr );

	fd = vic_ptr->fd;
	if ( vic_ptr->type == PLAYER )
		mprint(fd, "%M breathes fire on you!\n", m1arg(crt_ptr));

	broadcast_rom(fd, vic_ptr->rom_num,"%M breathes fire on %m!", 
		 m2args(crt_ptr, vic_ptr));

	if(vic_ptr->type == PLAYER && F_ISSET(vic_ptr, PRFIRE))
		n = dice(crt_ptr->level, 2, 0);
	else
		n = dice(crt_ptr->level, 4, 0);
	return n;
}
   
int power_energy_drain( creature *crt_ptr, creature *vic_ptr)
{

	int n;
	int fd;

	ASSERTLOG( crt_ptr );
	ASSERTLOG( vic_ptr );

       fd = vic_ptr->fd;
        n = dice(crt_ptr->level, 5, (crt_ptr->level)*5);
        n = MAX(0,MIN(n,vic_ptr->experience));
        if ( vic_ptr->type == PLAYER )
                mprint(fd, "%M drains your experience!\n", m1arg(crt_ptr));

        broadcast_rom2(crt_ptr->fd, vic_ptr->fd, vic_ptr->rom_num,
                 "%M energy drains %m!", m2args(crt_ptr, vic_ptr));
 
        if ( vic_ptr->type == PLAYER )
        {
                vic_ptr->experience -= n;
                lower_prof(vic_ptr,n);
        
                sprintf(g_buffer, "%%M drains you for %d experience.\n", n);
                mprint(fd, g_buffer, m1arg(crt_ptr));
        }

	return 0;
}

/************************************************************************/
/*		pkilled_allowed()					*/
/*									*/
/* Checks all conditions necessary to be able to attack/harm a player	*/ 
/************************************************************************/
int pkill_allowed(creature *ply_ptr, creature *crt_ptr )
{

	/* assume you can and check for cases where you cant */
	int can_attack = 1;
	room	*rom_ptr;
	int		fd, diff;
	ctag *cp;


	ASSERTLOG( ply_ptr );
	ASSERTLOG( crt_ptr );

	rom_ptr = ply_ptr->parent_rom;
	fd = ply_ptr->fd;

	if(ply_ptr->class < DM && crt_ptr->type == PLAYER) 
	{
		/* DM Owned/Outcast can always be attaked */
		if (F_ISSET(crt_ptr, PDMOWN)) 
		{
			return(1);
		}

		if(F_ISSET(rom_ptr, RNOKIL)) 
		{
			output(fd, "No killing allowed in this room.\n");
			can_attack = 0;
		}
		else if ( AT_WAR && F_ISSET(ply_ptr, PPLDGK) && F_ISSET(crt_ptr,PPLDGK))
		{  
			/* at war and both pledged, can fight */
			can_attack = 1;
		}
		else
		{

			// not at war 
			if( !F_ISSET(ply_ptr, PCHAOS) && !F_ISSET(crt_ptr, PDMOWN)) 
			{      
				output(fd, "Sorry, you're lawful.\n");
				can_attack = 0;
			}
			else if( !F_ISSET(crt_ptr, PCHAOS) && !F_ISSET(crt_ptr, PDMOWN)) 
			{
				output(fd, "Sorry, that player is lawful.\n");
				can_attack = 0;
			}     

		}
	}


	if ( can_attack  && pkill_in_battle == 0 )
	{
		/* check for monsters in same room that  */
		/* have this player on their enemy list */
		cp = rom_ptr->first_mon;
		while(cp) 
		{
			if(cp->crt->first_enm) 
			{
				if( is_enm_crt(crt_ptr->name, cp->crt ))
				{
					output(ply_ptr->fd, "That is not allowed during battle.\n");
					can_attack = 0;
					break;
				}
			}
			cp = cp->next_tag;
		}
	}

if(NOBULLYS && (ply_ptr->class < CARETAKER && crt_ptr->class < CARETAKER)) {
	if ( can_attack ) {
		diff = abs(ply_ptr->level - crt_ptr->level);
		if(ply_ptr->level < 6 || crt_ptr->level < 6) {
			if(diff > 2) {
				output(ply_ptr->fd, "Why don't you pick on someone your own size?\n");
				can_attack = 0;
			}
		} else if(ply_ptr->level < 16 || crt_ptr->level < 16) {
			if(diff > 5) {
				can_attack = 0;	
				output(ply_ptr->fd, "Why don't you pick on someone your own size?\n");
			}
		} else		/* lvl's 16+ are unprotected */
			can_attack = 1;
	}
}

	return( can_attack );
}

/************************************************************************/
/* break_weapon()							*/
/************************************************************************/
void break_weapon(creature *ply_ptr)
{

	int	fd;

	ASSERTLOG( ply_ptr );

	fd = ply_ptr->fd;

	sprintf(g_buffer, "Your %s is broken.\n", ply_ptr->ready[WIELD-1]->name);
	output(fd, g_buffer );
    sprintf(g_buffer, "%%M broke %s %s.", 
		F_ISSET(ply_ptr, PMALES) ? "his":"her",
		ply_ptr->ready[WIELD-1]->name);
    broadcast_rom(fd, ply_ptr->rom_num, g_buffer,
        m1arg(ply_ptr));

	if(F_ISSET(ply_ptr->ready[WIELD-1], OTMPEN)) 
	{
       	ply_ptr->ready[WIELD-1]->pdice=0;
       	F_CLR(ply_ptr->ready[WIELD-1], OTMPEN);
       	F_CLR(ply_ptr->ready[WIELD-1], ORENCH);
       	F_CLR(ply_ptr->ready[WIELD-1], OENCHA);
       	if(F_ISSET(ply_ptr, PDMAGI))
		{
        	sprintf(g_buffer, "The enchantment fades on your %s.\n", 
				ply_ptr->ready[WIELD-1]->name);
			output(ply_ptr->fd, g_buffer );
		}
    }

	add_obj_crt(ply_ptr->ready[WIELD-1], ply_ptr);
	dequip(ply_ptr,ply_ptr->ready[WIELD-1]);
	ply_ptr->ready[WIELD-1] = 0;

	return;

}
 
/************************************************************************/
/* attack_with_weapon()							*/
/************************************************************************/
void attack_with_weapon( creature * ply_ptr )
{
	if(ply_ptr->ready[WIELD-1] && !mrand(0,3))
		ply_ptr->ready[WIELD-1]->shotscur--;

	return;
}


/************************************************************************/
/* do_damage()								*/
/*									*/
/* This function shoul be called whenever damage is done to a player	*/
/* or monster.  The att_ptr is the attacker and is allowed to be NULL	*/
/* for player harm rooms and poison and stuff.				*/
/************************************************************************/
int do_damage(creature *vic_ptr, creature *att_ptr, int dmg)
{
	int	fd;

	ASSERTLOG(vic_ptr);
	ASSERTLOG(vic_ptr->type == PLAYER );
	ASSERTLOG( dmg >= 0 );


	if ( vic_ptr == NULL )
	{
		elog_broad("do_damage() called with a NULL victim pointer");
		return(0);
	}

	fd = vic_ptr->fd;

	vic_ptr->hpcur -= dmg;

	if(vic_ptr->hpcur < 1) 
	{
		if ( att_ptr )
		{
			msprint(vic_ptr, g_buffer, "%M killed you.\n", m1arg(att_ptr));
			output_wc(fd, g_buffer, ATTACKCOLOR);

			if(vic_ptr->type == MONSTER && att_ptr->type == MONSTER) {
				broadcast_rom2(att_ptr->fd, vic_ptr->fd, vic_ptr->rom_num,"%M killed %m.",
				   m2args(att_ptr, vic_ptr));
			}
		}
		else
		{
			ANSI(fd, ATTACKCOLOR);
			output(fd, "You died.\n" );
			ANSI(fd, MAINTEXTCOLOR);

			/* for the die function */
			att_ptr = vic_ptr;
		}

		die(vic_ptr, att_ptr);
	}
	else if(F_ISSET(vic_ptr, PWIMPY)) 
	{
		if(vic_ptr->hpcur <= vic_ptr->WIMPYVALUE) 
		{
			flee(vic_ptr, NULL);
		}
	}


	return(TRUE);

}


/******************************************************************/
/*                  hit_description                               */
/******************************************************************/

char *hit_description(int damage)
{
	if(damage < 3)
		return dam_desc[0];
	else if(damage < 6)
		return dam_desc[1];
	else if(damage < 10)
		return dam_desc[2];
	else if(damage < 13)
		return dam_desc[3];
	else if(damage < 16)
		return dam_desc[4];
	else if(damage < 21)
		return dam_desc[5];
	else if(damage < 26)
		return dam_desc[6];
	else if(damage < 32)
		return dam_desc[7];
	else if(damage < 37)
		return dam_desc[8];
	else if(damage < 42)
		return dam_desc[9];
	else if(damage < 47)
		return dam_desc[10];
	else if(damage < 52)
		return dam_desc[11];
	else
		return dam_desc[12];
}
