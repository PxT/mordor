/* UPDATE.C:
 *
 *  Routines to handle non-player game updates.
 *
 *  Copyright (C) 1991, 1992, 1993 Brooke Paul
 *
 * $Id: update.c,v 6.36 2001/07/17 19:28:44 develop Exp $
 *
 * $Log: update.c,v $
 * Revision 6.36  2001/07/17 19:28:44  develop
 * *** empty log message ***
 *
 * Revision 6.36  2001/07/14 21:26:44  develop
 * *** empty log message ***
 *
 * Revision 6.35  2001/07/04 15:22:17  develop
 * more adjustments to conjure lifetimes
 *
 * Revision 6.34  2001/07/04 15:12:49  develop
 * adjusting timeouts for conjured creatures
 *
 * Revision 6.33  2001/07/04 14:43:46  develop
 * playing with conjure times
 *
 * Revision 6.32  2001/07/04 02:33:03  develop
 * fixed sending a null refernce to del_conjured
 *
 * Revision 6.31  2001/07/03 21:58:04  develop
 * testing changes to smart crt's
 *
 * Revision 6.30  2001/07/03 21:47:08  develop
 * *** empty log message ***
 *
 * Revision 6.29  2001/07/03 01:20:24  develop
 * changed check on conjureds to determine if its time to fade
 *
 * Revision 6.28  2001/07/01 14:49:27  develop
 * changed MGROUP to MTEAMM and added MTEAML
 *
 * Revision 6.27  2001/07/01 06:00:31  develop
 * finished MGROUP
 *
 * Revision 6.26  2001/07/01 05:14:29  develop
 * *** empty log message ***
 *
 * Revision 6.25  2001/07/01 04:57:15  develop
 * added MGROUP
 *
 * Revision 6.24  2001/06/30 01:58:34  develop
 * added ENMLIST
 *
 * Revision 6.23  2001/06/30 01:09:05  develop
 * added struct for default enemy list
 *
 * Revision 6.22  2001/06/29 03:22:12  develop
 * added struct for default enemy lists
 *
 * Revision 6.21  2001/06/29 01:46:33  develop
 * first attempt at monster v monster default enemy lists
 *
 * Revision 6.20  2001/06/26 22:30:48  develop
 * conjure bug fixes
 *
 * Revision 6.19  2001/06/20 05:00:23  develop
 * check_conjure_vigor change
 *
 * Revision 6.18  2001/06/07 15:44:49  develop
 * minor update mod for conjureds
 *
 * Revision 6.17  2001/06/06 19:36:46  develop
 * conjure added
 *
 * Revision 6.16  2001/05/04 17:13:36  develop
 * check to make sure userid and address match in update lockout
 *
 * Revision 6.15  2001/03/15 06:17:42  develop
 * changed output on guild 'tick' to make it more in line with
 * fantasy them
 *
 * Revision 6.14  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */
#include <stdlib.h>

#include "../include/mordb.h"
#include "mextern.h"
#include "update.h"


static long last_update;
static long last_user_update;
static long last_random_update;
static long last_random_exit_update;
static long last_active_update;
static long last_time_update;
static long last_shutdown_update;
static long last_guildwar_update;
static long last_weather_update;
static long last_allcmd;
static long last_security_update;
static long last_action_update;
long        last_exit_update;
long        TX_interval = 4200;
int	    Mobilechance = 30;
short       Action_update_interval = 8;

extern csparse  Crt[CMAX];


/************************************************************************/
/*              update_game               				*/
/************************************************************************/

/* This function handles all the updates that occur while players are */
/* typing.                                */

void update_game()
{
    time_t    t;

    t = time(0);
    if(t == last_update)
        return;

    last_update = t;
    if(t - last_user_update >= 20)
		update_users(t);
    if(t - last_security_update >= 20)
		update_security(t);
    if(t - last_random_update >= Random_update_interval)
		update_random(t);
    if(t != last_active_update)
		update_active(t);
    if(t - last_time_update >= 150)
		update_time(t);
    if(t - last_weather_update >= 60)
		update_weather(t);
    if(t - last_exit_update >= TX_interval)
		update_exit(t);
    if(t - last_action_update >= Action_update_interval)
		update_action(t);
    if(last_dust_output && last_dust_output < t)
		update_dust_output(t);
    if(t - last_random_exit_update >= 21)
		update_random_exit(t);


    if(Shutdown.ltime && t - last_shutdown_update >= 30)
        if(Shutdown.ltime + Shutdown.interval <= t+500)
            update_shutdown(t);

    if(Guildwar.ltime && t - last_guildwar_update >= 30)
        if(Guildwar.ltime + Guildwar.interval <= t+500  &&
           Guildwar.ltime + Guildwar.interval > 0)
            update_guildwar(t);



	if(RECORD_ALL)
		if(t- last_allcmd >= 120)
			update_allcmd(t);
	
}

/************************************************************************/
/*              update_users						*/
/************************************************************************/

/* This function controls user updates.  Every 20 seconds it checks a */
/* user's time-out flags (such as invisibility, etc.) and it also     */
/* checks for excessive idle times.  If a user has been idle for over */
/* x mins, he is disconnected.                       */

void update_users(long t)
{
    int i;
	/* default to player timeout */
	time_t	tout;

    last_user_update = t;

    for(i=0; i<Tablesize; i++) {
        if(!Ply[i].io) 
			continue;
        if(Ply[i].ply && Ply[i].ply->class == DM) 
			tout = dm_timeout;
        else if(Ply[i].ply && Ply[i].ply->class == CARETAKER) 
			tout = ct_timeout;
        else if(Ply[i].ply && Ply[i].ply->class == BUILDER) 
			tout = bld_timeout;
		else
			tout = ply_timeout;

		/* 0 or negative timeout means no timeout */
		if ( tout > 0 )
		{
			if(t - Ply[i].io->ltime > tout && Ply[i].io->fn != waiting) 
			{
	    		scwrite(i, "\n\rTimed out.\n\r", 14);
				disconnect(i);
				continue;
			}
		}

        if(!Ply[i].ply) 
			continue;
        if(Ply[i].ply->fd < 0) 
			continue;

        if(Ply[i].ply->class < DM) 
	        update_ply(Ply[i].ply); 
    }
}

void update_random_exit(long t)
{
	room	*rom_ptr;
	xtag	*xt;
	int i, chance;

	last_random_exit_update = t;

	for(i=0;i<Tablesize;i++) {
		if(!Ply[i].ply || !Ply[i].io)
			continue;
		if(Ply[i].ply->fd < 0)
			continue;

		rom_ptr = Ply[i].ply->parent_rom;

		check_random_exits(rom_ptr);

		xt = rom_ptr->first_ext;

		while(xt) {
			if(F_ISSET(xt->ext, XRANDM)) {
			   if(xt->ext->random.interval && (xt->ext->random.ltime + xt->ext->random.interval < t)) {
				if(F_ISSET(xt->ext, XNOSEE)) {		/* unhide exit */
                                        F_CLR(xt->ext, XNOSEE);
                                        xt->ext->random.ltime = t;
                                        if(strlen(xt->ext->rand_mesg[0]) > 0) {
                                           if(F_ISSET(xt->ext, XRANDB))
                                                broadcast(xt->ext->rand_mesg[0]);
                                           else
                                                broadcast_rom(-1,rom_ptr->rom_num,
                                                        xt->ext->rand_mesg[0],NULL);
                                        }
                                }
				else {		/* hide exit */
					F_SET(xt->ext, XNOSEE);
                                        xt->ext->random.ltime = t;
                                        if(strlen(xt->ext->rand_mesg[1]) > 0) {
                                           if(F_ISSET(xt->ext, XRANDB))
                                                broadcast(xt->ext->rand_mesg[1]);
                                           else
                                                broadcast_rom(-1,rom_ptr->rom_num,
                                                        xt->ext->rand_mesg[1],NULL);
                                        }
                                }
			   } else if(xt->ext->random.misc) {		/* no interval */
                                chance = xt->ext->random.misc;
                                if(chance > mrand(1,100)) {
                                   if(F_ISSET(xt->ext, XNOSEE)) {
                                      F_CLR(xt->ext, XNOSEE);
                                      if(xt->ext->rand_mesg[0]) {
                                           if(F_ISSET(xt->ext, XRANDB))
                                                broadcast(xt->ext->rand_mesg[0]);
                                           else
                                                broadcast_rom(-1,rom_ptr->rom_num, 
                                                        xt->ext->rand_mesg[0],NULL);
                                      }
                                         xt->ext->random.ltime = t;
                                   }
                                   else {
                                      F_SET(xt->ext, XNOSEE);
                                      if(xt->ext->rand_mesg[1]) {
                                           if(F_ISSET(xt->ext, XRANDB))
                                                broadcast(xt->ext->rand_mesg[1]);
                                           else 
                                                 broadcast_rom(-1,rom_ptr->rom_num, 
                                                        xt->ext->rand_mesg[1],NULL);
                                      }
                                      xt->ext->random.ltime = t;
                                   }
                                }
                        }

			} /* endif F_ISSET(XRANDM) */
			xt = xt->next_tag;
		}
	} /* end for */
}

/**********************************************************************/
/*              update_random                 */
/**********************************************************************/

/* This function checks all player-occupied rooms to see if random monsters */
/* have entered them.  If it is determined that random monster should enter */
/* a room, it is loaded and items it is carrying will be loaded with it.    */

void update_random(long t)
{
    creature    *crt_ptr;
    object      *obj_ptr;
    room        *rom_ptr;
    int     check[PMAX], num, m, n, i, j, k, l, total = 0;

    last_random_update = t;
    for(i=0; i<Tablesize; i++) {
        if(!Ply[i].ply || !Ply[i].io) 
			continue;
        if(Ply[i].ply->fd < 0) 
			continue;

        rom_ptr = Ply[i].ply->parent_rom;

        for(j=0; j<total; j++)
            if(check[j] == rom_ptr->rom_num) 
				break;
        if(j < total) 
			continue;

        check[total++] = rom_ptr->rom_num;

        if(mrand(1,100) > rom_ptr->traffic) 
			continue;

        n = mrand(0,9);
        if(!rom_ptr->random[n]) 
			continue;
        m = load_crt(rom_ptr->random[n], &crt_ptr);
        if(m < 0) 
			continue;

        if(F_ISSET(rom_ptr, RPLWAN))
            num = mrand(1, count_ply(rom_ptr));
        else if(crt_ptr->numwander > 1)
            num = mrand(1, crt_ptr->numwander);
        else
            num = 1;

        for(l=0; l<num; l++) {
            crt_ptr->lasttime[LT_ATTCK].ltime = 
            crt_ptr->lasttime[LT_MSCAV].ltime =
            crt_ptr->lasttime[LT_MWAND].ltime = t;

            if(crt_ptr->dexterity < 20)
                crt_ptr->lasttime[LT_ATTCK].interval = 3;
            else
                crt_ptr->lasttime[LT_ATTCK].interval = 2;

            j = mrand(1,100);
            if(j<90) 
				j=1;
            else if(j<96) 
				j=2;
            else 
				j=3;
            for(k=0; k<j; k++) {
                m = mrand(0,9);
                if(crt_ptr->carry[m]) {
					m=load_obj(crt_ptr->carry[m], &obj_ptr);
                    if(m > -1) {
						if(F_ISSET(obj_ptr, ORENCH))
							rand_enchant(obj_ptr);
						obj_ptr->value = mrand((obj_ptr->value*9)/10,(obj_ptr->value*11)/10);
						add_obj_crt(obj_ptr, crt_ptr);
                    }
                }
            }

            if(!F_ISSET(crt_ptr, MNRGLD) && crt_ptr->gold)
                crt_ptr->gold = mrand(crt_ptr->gold/10, crt_ptr->gold);

            if(!l) 
                add_crt_rom(crt_ptr, rom_ptr, num);
            else
                add_crt_rom(crt_ptr, rom_ptr, 0);

#ifndef MOBS_ALWAYS_ACTIVE
            add_active(crt_ptr);
#endif
            if(l != num-1)
                load_crt(rom_ptr->random[n], &crt_ptr);
        }
    }
}

/************************************************************************/
/*              update_active               				*/
/************************************************************************/

/* This function updates the activities of all monsters who are currently */
/* active (ie. monsters on the active list).  Usually this is reserved    */
/* for monsters in rooms that are occupied by players.            */

void update_active(long t)
{
    creature    *crt_ptr=0, *att_ptr=0, *pp=0;
    object      *obj_ptr=0;
    room        *rom_ptr=0;
    ctag        *cp=0, *cptemp, *ep, *gp;
    etag	*et;
    long        i;
    int     	n;
    time_t conj_time;

#ifdef DMALLOC
	int db;
#endif

    /* signal(SIGBUS, abort); */
    last_active_update = t;
    cp = get_first_active();
    while(cp) 
	{
#ifdef DMALLOC
		db = dmalloc_verify (cp);  
#endif /* DMALLOC */
        	if(!(cp->crt)) {
			merror("cp in active", NONFATAL);
			log_active();
			break;
		}

		crt_ptr = cp->crt;
		if(!crt_ptr) {
			merror("crt_ptr in active",NONFATAL);
 			log_active();
			break;
		}

		rom_ptr = crt_ptr->parent_rom;
		if(!rom_ptr) {
			merror("rom_ptr in active", NONFATAL);
			log_active();
			break;
		}

#ifndef MOBS_ALWAYS_ACTIVE
		/* take off the active list if nobody in the room */
		if(!rom_ptr->first_ply) {
	        cp = cp->next_tag;
			if ( !mobs_always_active )
			{
				del_active(crt_ptr);
			}
			continue;
		}
#endif
	/* set up monster v monster enemy lists */
	for (n=0; n<ENMLIST; n++) {
		if(!strcmp(monster_enmlist[n].attacker, crt_ptr->name) || !strcmp(monster_enmlist[n].attacker, "*")) {
			add_enm_crt(monster_enmlist[n].target , crt_ptr);
			end_enm_crt(monster_enmlist[n].target , crt_ptr);
		}		
	}

	/* propagate enemy lists for MTEAMM monsters */
	/* find monsters that are leaders and copy their enemy list */

	if(F_ISSET(crt_ptr, MTEAMM)) 
	{
		gp=rom_ptr->first_mon;
		while(gp) 
		{
			if ( gp->crt == crt_ptr ) 
			{
                                gp = gp->next_tag;
                                continue;
                        }

			if(F_ISSET(gp->crt, MTEAML))
			{
				et=gp->crt->first_enm;
				while(et) 
				{
					end_enm_crt(et->enemy, crt_ptr);
				et = et->next_tag;
				}
			}

		gp = gp->next_tag;
		}
	}
			
				
/* KRUFT - can be used to pick on all characters from a given site */
/*	pp=rom_ptr->first_ply;
	while(pp) {		
		strcpy(addr, Ply[pp->crt->fd].io->address);
		if(!strcmp(addr, "pluto.sfsu.edu") || !strcmp(addr, "apollo.sfsu.edu") || !strcmp(addr, "chorizo.engr.ucdavis.edu")) 
			if(mrand(1,1000)>990) {
				if(!is_enm_crt(pp->crt->name, cp->crt))
				{
					sprintf(g_buffer, "The %s attacks you!\n", cp->crt->name);
					output(pp->crt->fd, g_buffer);
				}
				add_enm_crt(pp->crt->name, cp->crt); 
			}
		pp=pp->next_tag;
	} */

        i = LT(crt_ptr, LT_ATTCK);
        if(i > t) {
            cp = cp->next_tag;
            continue;
        }

#ifdef MOBS_ALWAYS_ACTIVE
		heal_crt( crt_ptr );
#else
        i = LT(crt_ptr, LT_HEALS);
        while(i <= t && (crt_ptr->hpcur < crt_ptr->hpmax ||
              crt_ptr->mpcur < crt_ptr->mpmax)) 
		{
            crt_ptr->hpcur += MAX(1,crt_ptr->hpmax/10);
            if(crt_ptr->hpcur > crt_ptr->hpmax)
                crt_ptr->hpcur = crt_ptr->hpmax;
            crt_ptr->mpcur += MAX(1,crt_ptr->mpmax/6);
            if(crt_ptr->mpcur > crt_ptr->mpmax)
                crt_ptr->mpcur = crt_ptr->mpmax;
            i += 60L;
            crt_ptr->lasttime[LT_HEALS].ltime = t;
            crt_ptr->lasttime[LT_HEALS].interval = 60L;
        }
#endif
		if (t > LT(crt_ptr, LT_CHRMD)&&F_ISSET(crt_ptr, MCHARM))
			F_CLR(crt_ptr, MCHARM);

        crt_ptr->lasttime[LT_ATTCK].ltime = t;
        if(crt_ptr->dexterity < 20)
            crt_ptr->lasttime[LT_ATTCK].interval = 3;
        else
            crt_ptr->lasttime[LT_ATTCK].interval = 2;

        if(F_ISSET(crt_ptr, MSCAVE)) 
		{
            i = crt_ptr->lasttime[LT_MSCAV].ltime;
            if(t-i > 20 && mrand(1,100) <= 15 && 
               rom_ptr->first_obj && 
               !F_ISSET(rom_ptr->first_obj->obj, ONOTAK) &&
               !F_ISSET(rom_ptr->first_obj->obj, OSCENE) &&
               !F_ISSET(rom_ptr->first_obj->obj, OHIDDN) &&
               !F_ISSET(rom_ptr->first_obj->obj, OPERM2) &&
               !F_ISSET(rom_ptr->first_obj->obj, OPERMT)) {
                obj_ptr = rom_ptr->first_obj->obj;
                del_obj_rom(obj_ptr, rom_ptr);
                add_obj_crt(obj_ptr, crt_ptr);
                F_SET(crt_ptr, MHASSC);
                broadcast_rom(-1, crt_ptr->rom_num,
                          "%M picked up %1i.", 
                          m2args(crt_ptr, obj_ptr));
            }
            if(t-i > 20)
                crt_ptr->lasttime[LT_MSCAV].ltime = t;
        }
		if(F_ISSET(crt_ptr, MMOBIL) && !F_ISSET(crt_ptr, MPERMT) 
			&& !F_ISSET(crt_ptr, MDMFOL) && !F_ISSET(crt_ptr, MCONJU))
		{
			i = crt_ptr->lasttime[LT_MWAND].ltime;
			if(t-i > 20 && mrand(1,100) >20) 
			{
				/* we are gonna continue here no matter what */
				/* so get ready for the next monster */
				cp = cp->next_tag;
				n = mobile_crt(crt_ptr);
				if(!n)
				{
					F_CLR(crt_ptr, MMOBIL);
#ifndef MOBS_ALWAYS_ACTIVE
					del_active(crt_ptr);
#endif
				}
				continue;
			}
		}
        if(!F_ISSET(crt_ptr, MHASSC) && !F_ISSET(crt_ptr, MPERMT) 
			&& !F_ISSET(crt_ptr, MDMFOL) && !F_ISSET(crt_ptr, MCONJU)) 
		{
            i = crt_ptr->lasttime[LT_MWAND].ltime;
            if(t-i > 20 
               && mrand(1,100) <= crt_ptr->parent_rom->traffic
               && !crt_ptr->first_enm) 
			{
				if(mrand(1,100) < Mobilechance) 
				{
					F_SET(crt_ptr, MMOBIL);
					cp = cp->next_tag;
					continue;
				}
				broadcast_rom(-1, crt_ptr->rom_num, "%1M just wandered away.",
					m1arg(crt_ptr));
				del_crt_rom(crt_ptr, rom_ptr);
				cp = cp->next_tag;
				free_crt(crt_ptr); 
				continue;
			}
            if(t-i > 20)
                crt_ptr->lasttime[LT_MWAND].ltime = t;
        }

	/* Have conjured vig owner if possible */
	if( mrand(1,100) > (42 + crt_ptr->level) ) {
		check_conjure_vigor(crt_ptr);
	}

	/* This checks to see if anything in the room is attacking the
	   conjurer of a conjured pet.  If so, the conjured pet attacks it. */

	if (crt_ptr->type == MONSTER && F_ISSET(crt_ptr, MCONJU)){
		pp = crt_ptr->following;
		conj_time = time(0);
		/* Checks to see if the player logged off or if the time has
		   come for our conjured friend to depart... */
		if ( !pp || (conj_time > LT(crt_ptr, LT_CONJU)) ) {
		/* the following makes it so that pets don't fade during combat */
		/* It also seems to make them stay around for a very long time */
		/* && (conj_time - crt_ptr->lasttime[LT_ATTCK].ltime > 5)) ) { */

			broadcast_rom(crt_ptr->fd, crt_ptr->rom_num,
				"%M slowly fades away.", m1arg(crt_ptr));
			/* if !pp then you don't have a crt_ptr->following->first_fol which is required by del_conjured */
			if(pp) 
				del_conjured(crt_ptr);

			del_crt_rom(crt_ptr, crt_ptr->parent_rom);
			cp = cp->next_tag;
			free_crt(crt_ptr);
			continue;
		} else {
		   ep = rom_ptr->first_mon;
		   while (ep) {
			if ( ep->crt == crt_ptr ) {
				ep = ep->next_tag;
				continue;
			}
			if ( is_enm_crt(pp->name, ep->crt) &&
			    !is_enm_crt(ep->crt->name, crt_ptr) ) {
				add_enm_crt(ep->crt->name, crt_ptr);
				attack_mon(crt_ptr, ep->crt);
			}
		   	ep = ep->next_tag;
		   }
		}
	}



		if(!crt_ptr->first_enm && (!F_ISSET(crt_ptr, MAGGRE) && 
            !F_ISSET(crt_ptr, MGAGGR) && !F_ISSET(crt_ptr, MEAGGR))) 
		{
            cp = cp->next_tag;
            continue;

        }            

	if(crt_ptr->first_enm)
	{
    		cptemp = cp->next_tag;
   		update_combat(crt_ptr);
    		if ( !is_crt_active( crt_ptr ) )
    		{
        			cp = cptemp;
        			continue;
    		}
	}

/*	KRUFT
        if(crt_ptr->first_enm) 
		{
			cptemp = cp->next_tag;
			if(update_combat(crt_ptr))
			{
				cp = cptemp;
				continue;
			}
		}
*/
        if(F_ISSET(crt_ptr, MAGGRE) || F_ISSET(crt_ptr, MGAGGR) ||
			F_ISSET(crt_ptr, MEAGGR)) 
		{
			if (F_ISSET(crt_ptr, MAGGRE))
           		att_ptr = lowest_piety(rom_ptr, F_ISSET(crt_ptr, MDINVI) ? 1:0);
			else 
			{
				att_ptr = low_piety_alg(rom_ptr,F_ISSET(crt_ptr, MDINVI) ? 1:0,F_ISSET(crt_ptr,MGAGGR) ? -1 : 1,crt_ptr->level);
			}

            if(att_ptr) 
			{
     			if (!is_enm_crt(att_ptr->name, crt_ptr)) 
				{
					ANSI(att_ptr->fd, ATTACKCOLOR);
        			mprint(att_ptr->fd, "%M attacks you.\n", m1arg(crt_ptr));
					ANSI(att_ptr->fd, MAINTEXTCOLOR);
					broadcast_rom(att_ptr->fd, att_ptr->rom_num, 
						  "%M attacks %m.",
						  m2args(crt_ptr, att_ptr));
				}
				crt_ptr->lasttime[LT_ATTCK].interval = 3;    
     			add_enm_crt(att_ptr->name, crt_ptr); 
			}
        }


        cp = cp->next_tag;

    }

	return;
}

/*
 * check_conjure_vigor(crt_ptr);
 */
int check_conjure_vigor(creature *crt_ptr)
{
	float hp;

	creature *ply_ptr;
	cmd cmnd;

	if(!F_ISSET(crt_ptr, MCONJU)) {
		return(0);
	}

	
	ply_ptr = crt_ptr->following;
	if(!ply_ptr) {
		return(0);
	}

	if(ply_ptr->hpcur == ply_ptr->hpmax) {
		return(0);
	}

	hp = ( (float)ply_ptr->hpcur / ply_ptr->hpmax);
	cmnd.num = 3;
	sprintf(cmnd.str[0], "cast");
	sprintf(cmnd.str[2], "%s", ply_ptr->name);
	cmnd.val[1]=1;
	cmnd.val[2]=1;
	if( hp < 0.6 ) {
		if(S_ISSET(crt_ptr, SMENDW)) {
			sprintf(cmnd.str[1], "mend");
			sprintf(cmnd.fullstr, "cast mend %s", ply_ptr->name);
			/* mend player */
			return(mend(crt_ptr, &cmnd, CAST));
		}
		else if(S_ISSET(crt_ptr, SVIGOR)) {
			sprintf(cmnd.str[1], "vigor");
			sprintf(cmnd.fullstr, "cast vigor %s", ply_ptr->name);
			/* vigor player */
			return(vigor(crt_ptr, &cmnd, CAST));
		}
		else {
			return(0);
		}
	} else {
		if(S_ISSET(crt_ptr, SVIGOR)) {
			sprintf(cmnd.str[1], "vigor");
			sprintf(cmnd.fullstr, "cast vigor %s", ply_ptr->name);
			/* vigor player */
			return(vigor(crt_ptr, &cmnd, CAST));
		}
		else {
			return(0);
		}
	}
	return(0);
}

		
/************************************************************************/
/*              choose_item						*/
/************************************************************************/

/* This function randomly chooses an item that the player pointed to    */
/* by the first argument is wearing.                    */

int choose_item(creature *ply_ptr )
{
    char    checklist[MAXWEAR];
    int numwear=0, i;

    for(i=0; i<MAXWEAR; i++) {
        checklist[i] = 0;
        if(i==WIELD-1 || i==HELD-1) continue;
        if(ply_ptr->ready[i])
            checklist[numwear++] = i+1;
    }

    if(!numwear) return(0);

    i = mrand(0, numwear-1);
    return(checklist[i]);
}

/************************************************************************/
/*              crt_spell						*/
/************************************************************************/

/* This function allows monsters to cast spells at players.       	*/

int crt_spell(creature *crt_ptr, creature *att_ptr)
{
    cmd cmnd;
    int i, j, spl, c;
    int known[10], knowctr = 0;
    int (*fn)();
	char * enemy;
	int	n;
	creature *temp_ptr;

    for(i=0; i<16; i++) {
        if(!crt_ptr->spells[i]) 
			continue;
        for(j=i*8; j<=(i*8+7); j++) {
            if(knowctr > 9) 
				break;
            if(S_ISSET(crt_ptr, j))
                known[knowctr++] = j;
        }
        if(knowctr > 9) 
			break;
    }

    if(!knowctr)
        spl = 1;
    else {
        i = mrand(1, knowctr);
        spl = known[i-1];
    }

	if ((get_spell_num(spl) != SVIGOR) && 
		(get_spell_num(spl) != SMENDW) &&
		(get_spell_num(spl) != SFHEAL)){

		/* let find the exact creature that matches */
		/* we have the exact pointer */
		enemy = att_ptr->name;
		n = 1;
		do 
		{
			/* look for player first */
			temp_ptr = find_crt_in_rom(crt_ptr, crt_ptr->parent_rom, enemy, n,
				PLY_FIRST);
			if(!temp_ptr)
			{	
				/* somethign is seriuouly wrong here */
				return(0);
			}

			/* move to the next */
			n++;	

			/* keep going till we find the exact match */
		} while ( att_ptr != temp_ptr );

		/* we were one too many here */
		n--;

		/* at this point we know which number creature is the exact match */
    	strcpy(cmnd.str[2], att_ptr->name);
    	cmnd.val[2] = n;
    	cmnd.num = 3;
	}
	else {
		/* cast on self */
	 	cmnd.num = 2;
	}

    fn = get_spell_function(spl);

	if ( att_ptr->type == PLAYER )
		ANSI(att_ptr->fd, ATTACKCOLOR);

    if(fn == offensive_spell || fn == room_damage) {
        for(c=0; ospell[c].splno != get_spell_num(spl); c++)
            if(ospell[c].splno == -1) 
				return(0);
        i = (*fn)(crt_ptr, &cmnd, CAST, get_spell_name(spl),
            &ospell[c]);
    }
    else 
		i = (*fn)(crt_ptr, &cmnd, CAST);

	if ( att_ptr->type == PLAYER )
	    ANSI(att_ptr->fd, MAINTEXTCOLOR);
    return(i);
}

/************************************************************************/
/*              update_time						*/
/************************************************************************/

/* This function updates the game time in hours.  When it is 6am a sunrise */
/* message is broadcast.  When it is 8pm a sunset message is broadcast.    */

void update_time(long t)
{
    int daytime;

    last_time_update = t;

    Time++;
    daytime = (int)(Time % 24L);

    if(daytime == 6)
	{
		broadcast("\n" );
		broadcast( sunrise );
	}
    else if(daytime == 20)
	{
		broadcast("\n" );
		broadcast( sunset );
	}


}

/************************************************************************/
/*              update_shutdown						*/
/************************************************************************/

/* This function broadcasts a shutdown message every 30 seconds until */
/* shutdown is achieved.  Then it saves off all rooms and players,    */
/* and exits the game.                            */

void update_shutdown(long t)
{
    long    i;

    last_shutdown_update = t;

    i = Shutdown.ltime + Shutdown.interval;
    if(i > t) {
        if(i-t > 60)
		{
            sprintf(g_buffer, "### Game backup shutdown in %d:%02d minutes.", 
					(int)((i-t)/60L), (int)((i-t)%60L));
            broadcast( g_buffer );
		}
        else
		{
			sprintf(g_buffer, "### Game shutdown in %d seconds.", 
					(int)(i-t) );
            broadcast(g_buffer);
		}
    }
    else {
        broadcast("### Shutting down now.");
        output_buf();
	sprintf(g_buffer, "--- Shutdown: %u --- \n", (int)PORTNUM );
        elog_broad(g_buffer);
        plog(g_buffer);
        resave_all_rom(1);
        save_all_ply();

#ifdef DMALLOC
	dmalloc_log_stats();
#endif /* DMALLOC */
/* 
     char path[128];

  if(RECORD_ALL)
		sprintf(path,"%s/%s",LOGPATH,"all_cmd");
		unlink(path);
*/
	disconnect_all_ply();

	free_all_queues();


	kill(getpid(), 9);
	exit(0);
    }
}
/**********************************************************************/
/*              update_guildwar                                       */
/**********************************************************************/


void update_guildwar(long t)
{
    long    i, j;

    last_guildwar_update = t;

    i = Guildwar.ltime + Guildwar.interval;
    if(i > t) {
        if(i-t > 60)
	{
        	sprintf(g_buffer, "### The Guild war ends in %d:%02d minutes.", 
						(int)((i-t)/60L), (int)((i-t)%60L));
        	broadcast( g_buffer );
	}
        else
	{
		sprintf(g_buffer, "### The Guild war ends in %d seconds.", 
						(int)(i-t) );
		broadcast(g_buffer);
	}
    }
    else {
        broadcast("### The Guild war is finished.");
	for(j=1; j<9; j++) {
		sprintf(g_buffer, "### The %s has a final score of %d.", cur_guilds[j].name,
			cur_guilds[j].pkilled - cur_guilds[j].pkia); 
        	broadcast(g_buffer);
	}
        Guildwar.ltime = 0;
        Guildwar.interval = 0;
    }
}


/**************************************************************************/
/*			update_exit					  */
/**************************************************************************/
void update_exit(long t)
{
room    *rom_ptr;
xtag    *xp;
char    *tmp;
int i,x;

    last_exit_update = t;
    for(i=0;i<MAX_TEXIT; i++){
        if(load_rom( time_x[i].room,&rom_ptr) < 0)
            return; 
        xp = rom_ptr->first_ext;
        while(xp){
            if((time_x[i].name1) && (time_x[i].exit1 == xp->ext->room)
	      && (!strcmp(xp->ext->name,time_x[i].name1))) 
                F_SET(xp->ext,XNOSEE);

            if((time_x[i].name2) && (time_x[i].exit2 == xp->ext->room)
	      && (!strcmp(xp->ext->name,time_x[i].name2))) 
                F_CLR(xp->ext,XNOSEE);

            xp = xp->next_tag;
        }

             tmp  = time_x[i].name1;
             x = time_x[i].exit1;
             time_x[i].name1 = time_x[i].name2;
             time_x[i].exit1 = time_x[i].exit2;
             time_x[i].name2 = tmp;
             time_x[i].exit2 = x;
       
             if (!t_toggle)
               tmp = time_x[i].mess1;  
             else
               tmp = time_x[i].mess2;  

            if (tmp)
			{
                broadcast("\n");
                broadcast(tmp);
			}
            t_toggle = !t_toggle;
                    
     }
}


/************************************************************************/
/*			update_allcmd					*/
/************************************************************************/
void update_allcmd(long t)
{
    char path[128];

    last_allcmd = t;
    sprintf(path,"%s/%s", get_log_path(), "all_cmd");
    unlink(path);
}

/************************************************************************/
/*		list_act						*/
/************************************************************************/
int list_act(creature *ply_ptr, cmd	*cmnd )
{
	ctag	*cp;
	int		nCount = 0;

	if (ply_ptr->class < BUILDER)
		return(0);

	output(ply_ptr->fd,"### Active monster list ###\n");
	output(ply_ptr->fd,"Monster    -    Room Number\n");
	cp = get_first_active();
	
	while (cp)
	{
		/* check before we sprintf() */
		/* dont know why yet but this happens sometimes  */
		if ( cp->crt != NULL)
		{
			sprintf(g_buffer,"%.16s - %d.", cp->crt->name, cp->crt->parent_rom->rom_num);
		}
		else
		{
			strcpy(g_buffer,"cp->crt == NULL");
		}

		nCount++;

		output_nl(ply_ptr->fd, g_buffer);
		cp = cp->next_tag;
	}
	sprintf(g_buffer, "Total active monsters: %d.", nCount );
	output_nl(ply_ptr->fd, g_buffer );

	output_ply_buf(ply_ptr->fd);

	return(0);
}



/****************************************************************/
/*		update_weather					*/
/****************************************************************/
/*								*/
void update_weather(long t)
{
int	j, wtime, n=0;

	last_weather_update = t;
	wtime = (int)(Time % 24L);	
	
	for(j=0;j<5; j++){	
	if(Weather[j].ltime + Weather[j].interval < t) {
		switch (j) {
		case 0:
			if(mrand(1,100) > 80) {
				n = mrand(0,2);
				n -= 1;
				n = Weather[j].misc + n;
				if(n < 0 || n > 4)
					n = 0;
				switch (n) {
					case 0:
						Weather[j].ltime = t;
						broadcast_wc(WEATHERCOLOR, earth_trembles);
						break;
					case 1:
						Weather[j].ltime = t;
						broadcast_wc(WEATHERCOLOR, heavy_fog);
						break;
				}
				Weather[j].misc = (short)n;
                Weather[j].interval = 3200;
			}
			break;
				
		case 1:   
			if(mrand(1,100) > 50 && wtime > 6 && wtime < 20) {
				n = mrand(0,2);
				n -= 1;
				n = Weather[j].misc + n;
				if(n < 0 || n > 4) 
					n = 0;
				switch (n) {
					case 0:
						Weather[j].ltime = t;
						broadcast_wc(WEATHERCOLOR, beautiful_day);
						break;
					case 1: 
						Weather[j].ltime = t;
						broadcast_wc(WEATHERCOLOR, bright_sun);
						break;
					case 2:
                        Weather[j].ltime = t;
						broadcast_wc(WEATHERCOLOR, bright_sun);
						break;
					case 3:
                        Weather[j].ltime = t;
		                broadcast_wc(WEATHERCOLOR, glaring_sun); 
						break;
					case 4:
                        Weather[j].ltime = t;
                        broadcast_wc(WEATHERCOLOR, heat);
						break;
				}
				Weather[j].misc = (short)n;
				Weather[j].interval = 1600;
			}
			break;
		case 2:  
			if(mrand(1,100) > 50) { 
				n = mrand(0,2);
                n -= 1;
                n = Weather[j].misc +n; 
				if(n< 0 || n> 4) 
					n = 0;
				switch (n){
					case 0:
						Weather[j].ltime = t;
						broadcast_wc(WEATHERCOLOR, still);
						break;
					case 1:
                        Weather[j].ltime = t;
						broadcast_wc(WEATHERCOLOR, light_breeze);
						break;
					case 2:
                        Weather[j].ltime = t;
						broadcast_wc(WEATHERCOLOR, strong_wind);
						break;
					case 3:
                        Weather[j].ltime = t;
						broadcast_wc(WEATHERCOLOR, wind_gusts);
						break;
					case 4:
						Weather[j].ltime = t;
						broadcast_wc(WEATHERCOLOR, gale_force);
                        break;
				}
				Weather[j].misc = (short)n;
				Weather[j].interval = 900;
			}
			break;
		case 3:  
            if(mrand(1,100) > 50) {
				n = mrand(0,2);                        	
				n -= 1;
		        n = Weather[j].misc+n; 
                if(n< 0 || n> 6) 
					n = 0;
				switch (n) {
					case 0:
						Weather[j].ltime = t;
						broadcast_wc(WEATHERCOLOR, clear_skies);
						break;
					case 1:
						Weather[j].ltime = t;
                        broadcast_wc(WEATHERCOLOR, light_clouds);
                        break;
                    case 2:
                        Weather[j].ltime = t;
                        broadcast_wc(WEATHERCOLOR, thunderheads);
                        break;
					case 3:
						Weather[j].ltime =t;
						broadcast_wc(WEATHERCOLOR, light_rain);
						break;
					case 4:
						Weather[j].ltime = t;
						broadcast_wc(WEATHERCOLOR, heavy_rain);
						break;
					case 5:
						Weather[j].ltime = t;
						broadcast_wc(WEATHERCOLOR, sheets_rain);
                        break;
        			case 6:
						Weather[j].ltime = t;
                        broadcast_wc(WEATHERCOLOR, torrent_rain);
						break;
				}
				Weather[j].misc = (short)n;
				Weather[j].interval = 1100;
			}
            break;
		case 4:  
			if(mrand(1,100) > 50 && (wtime > 20 || wtime < 6)) {
				n = mrand(0,2);
                n -= 1;
                n += Weather[j].misc;
                if(n< 0 || n> 4) 
					n = 0;
				switch (n) {
					case 0:
        				Weather[j].ltime = t;
						broadcast_wc(WEATHERCOLOR, no_moon);
						break;
					case 1:
                        Weather[j].ltime = t;
						broadcast_wc(WEATHERCOLOR, sliver_moon);
						break;
					case 2:
	                    Weather[j].ltime = t;
						broadcast_wc(WEATHERCOLOR, half_moon);
						break;
					case 3:
	                    Weather[j].ltime = t;
						broadcast_wc(WEATHERCOLOR, waxing_moon);
						break;
					case 4:
	                    Weather[j].ltime = t;
						broadcast_wc(WEATHERCOLOR, full_moon);
						break;
					}
					Weather[j].misc = (short)n;
                    Weather[j].interval = 1200;
			}
			break;
		} 		
		} 
	} 

	n = 11 - Weather[1].misc - Weather[2].misc - (Weather[3].misc - 2) + Weather[4].misc;
	if(n>25 || n < 2) 
		n=11;
	Random_update_interval = (short)n;
	return;
} 

/**********************************************************************/
/*              update_security			                      */
/**********************************************************************/

/* This function checks for locked out usernames, non-RFC 931 accounts */

void update_security(long t)
{
	int i, booted=0, j; /*match=0; l, fdtemp=0; */
	struct hostent *address;

	last_security_update=t;

	for(i=0; i<Tablesize; i++) {
		if(!Ply[i].ply) continue;
		if(!Ply[i].io) continue;
		if(Ply[i].ply->fd < 0) continue;
		if(F_ISSET(Ply[i].ply, PSECOK)) continue; 

/*
		if(SECURE) {
			for(l=0; l<Tablesize; l++) {
				if(!Ply[l].ply) continue;
				if(Ply[l].ply->fd < 0) continue;
				if(!strcmp(Ply[i].io->userid, Ply[l].io->userid) && i != l && strcmp(Ply[i].io->userid, "no_port") && strcmp(Ply[i].io->userid, "unknown")) {
					match +=1;
					fdtemp = l;
					break;
			       }
				if((strcmp(Ply[i].io->userid, "no_port") && strcmp(Ply[i].io->userid, "unknown")) && F_ISSET(Ply[i].ply, PAUTHD))
					F_CLR(Ply[i].ply, PAUTHD);
			}
			if(match > 0){
				output(Ply[i].ply->fd, "\n\nThe Watcher just arrived.\n");
				output(Ply[i].ply->fd, "The Watcher says, \"You may only play one character at a time.\"\n"); 
				output(Ply[i].ply->fd, "The Watcher waves goodbye.\n");
				output_ply_buf(Ply[i].ply->fd);
				disconnect(i);
				if(fdtemp) 
					disconnect(fdtemp);  
				match=0;
				fdtemp=0;
				continue;
			}
		}*/ /* SECURE */

		/* Check for username lockout */
		for(j=0; j<Numlockedout; j++) {
			if(!Ply[i].io) break;
			if(strcmp(Lockout[j].userid, Ply[i].io->userid))  
				continue;
			else 
			{
                		address = gethostbyname(Ply[i].io->address);
                		if(!address) {
					continue;
                		}

				sprintf(g_buffer, "%s", inet_ntoa( *(struct in_addr *)(address->h_addr_list[0])));
				if(!strcmp(g_buffer,Lockout[j].address)) {

				scwrite(Ply[i].ply->fd, "\n\rThe Watcher has locked your account.\n\r", 42);
				sprintf(g_buffer, "\n\rSend questions to %s.\n\r", auth_questions_email);
				output(Ply[i].ply->fd, g_buffer);

				booted +=1;
				disconnect(Ply[i].ply->fd);                        
				break;
				}
			}
		}
		if(booted) {
			booted =0;
			continue;
		}

		if(RFC1413) {
		/* Check for no_port */
		if((!strcmp(Ply[i].io->userid, "no_port") || !strcmp(Ply[i].io->userid, "unknown")) && Ply[i].ply->level > 2 && !F_ISSET(Ply[i].ply, PAUTHD)) {
			if(t-Ply[i].ply->lasttime[LT_SECCK].ltime > 60) {
				output(Ply[i].ply->fd, "\n\n\rI am unable to get authorization for your account.\n");
				sprintf(g_buffer, "Logging out now.\n\nSend any questions to %s.\n\n", register_questions_email);
				output(Ply[i].ply->fd, g_buffer);
				output_ply_buf(Ply[i].ply->fd);
				disconnect(Ply[i].ply->fd);
				continue;
			}
			else {
				if(Ply[i].ply->lasttime[LT_SECCK].interval==t){
					output(Ply[i].ply->fd, "\n\rChecking for authorization.\n");
					output(Ply[i].ply->fd, "Your time will be limited if I cannot get authorization.\n\n");
					Ply[i].ply->lasttime[LT_SECCK].interval=t+20;
				}
				continue;
			}		
		} /* no_port */
		} /* RFC1413 */

		F_SET(Ply[i].ply, PSECOK);
		continue;
	}
}

/****************************************************************************
 *  UPDATE_ACTION is the update function for logic scripts
 *  
 */

void update_action(long t)
{
    creature    *crt_ptr,*vic_ptr;
    object      *obj_ptr;
    room        *rom_ptr;
    ctag        *cp,*vcp;
    ttag        *act,*tact;
    int         i,on_cmd; 
    int         xdir;
    char        *xits[] =  { "n","ne","e","se","s","sw","w","nw","u","d" };
    cmd         cmnd;
   
   
    last_action_update = t;
    
    for(cp = get_first_active(); cp != NULL; cp = cp->next_tag)
	{
		crt_ptr = cp->crt;
		if(crt_ptr)
		{
			rom_ptr = crt_ptr->parent_rom;
			if(rom_ptr && F_ISSET(crt_ptr,MROBOT))
			{
				if(!cp->crt->first_tlk)
					load_crt_actions(cp->crt);
				else
				{
					act = cp->crt->first_tlk;
					on_cmd = act->on_cmd;
					on_cmd--;
					i = 0;
					if(on_cmd)
						while(i<on_cmd)
						{
							act = act->next_tag;
							i++;
						}
					on_cmd+=2; /* set for next command, can be altered later */
					/* proccess commands based on a higharcy */
					if(act->test_for)
					{
						switch(act->test_for)
						{
						case 'P': /* test for player */
							vic_ptr = find_crt(crt_ptr, rom_ptr->first_ply, act->response, 1);
							if(vic_ptr)
							{
								if(crt_ptr->first_tlk->target)
									free(crt_ptr->first_tlk->target);
								crt_ptr->first_tlk->target = (char *)
								calloc(1,strlen(act->response)+1);
								strcpy(crt_ptr->first_tlk->target,act->response);
								act->success = 1;
							}
							else
							{
								if(crt_ptr->first_tlk->target)
									free(crt_ptr->first_tlk->target);
								crt_ptr->first_tlk->target = 0;
								act->success = 0;
							}
							break;
						case 'C': /* test for a player with class */
						case 'R': /* test for a player with race */
							for(vcp = rom_ptr->first_ply;vcp;vcp = vcp->next_tag)
							{
								if(act->test_for == 'C')
									if(vcp->crt->class == act->arg1)
									{
										if(crt_ptr->first_tlk->target)
											free(crt_ptr->first_tlk->target);
										crt_ptr->first_tlk->target = (char *)
										calloc(1,strlen(vcp->crt->name)+1);
										strcpy(crt_ptr->first_tlk->target,vcp->crt->name);
										act->success = 1;
										break;
									}
								if(act->test_for == 'R')
									if(vcp->crt->race == act->arg1)
									{
										if(crt_ptr->first_tlk->target)
											free(crt_ptr->first_tlk->target);
										crt_ptr->first_tlk->target = (char *)
										calloc(1,strlen(vcp->crt->name)+1);
										strcpy(crt_ptr->first_tlk->target,vcp->crt->name);
										act->success = 1;
										break;
									}
								}
								if(!vcp)
								{
									if(crt_ptr->first_tlk->target)
										free(crt_ptr->first_tlk->target);
									crt_ptr->first_tlk->target = 0; 
									act->success = 0;
								}
								break;
						case 'O': /* test for object in room */
							obj_ptr = find_obj(crt_ptr, rom_ptr->first_obj, act->response, 1);
						
							if(obj_ptr)
							{
								if(crt_ptr->first_tlk->target)
									free(crt_ptr->first_tlk->target);
								crt_ptr->first_tlk->target = (char *)calloc(1,strlen(act->response)+1);
								strcpy(crt_ptr->first_tlk->target,act->response);
								act->success = 1;
								/* elog_broad(vic_ptr->name); */
							}
							else
							{
								if(crt_ptr->first_tlk->target)
									free(crt_ptr->first_tlk->target);
								crt_ptr->first_tlk->target = 0;
								act->success = 0;
							}
							break;
						case 'o': /* test for object on players */
							break;
						case 'M': /* test for monster */
							vic_ptr = find_crt(crt_ptr, rom_ptr->first_mon, act->response, 1);
							if(vic_ptr)
							{
								if(crt_ptr->first_tlk->target)
									free(crt_ptr->first_tlk->target);
								crt_ptr->first_tlk->target = (char *) calloc(1,strlen(act->response)+1);
								strcpy(crt_ptr->first_tlk->target,act->response);
								act->success = 1;
							}
							else
							{
								if(crt_ptr->first_tlk->target)
									free(crt_ptr->first_tlk->target);
								crt_ptr->first_tlk->target = 0; 
								act->success = 0;
							}
							break;
						}
		
					}
				    if(act->if_cmd)
					{ 
						/* test to see if command was successful */
						for(tact = crt_ptr->first_tlk;tact;tact = tact->next_tag)
						{
							if(tact->type == act->if_cmd)
								break;
						}
						if(tact)
						{		
							if(act->if_goto_cmd && tact->success)
								on_cmd = act->if_goto_cmd;
							if(act->not_goto_cmd && !tact->success)
								on_cmd = act->not_goto_cmd;
						}
						else
						{
							if(act->not_goto_cmd)
								on_cmd = act->not_goto_cmd;
						}
					}
					if(act->do_act) /* run a action */
					{
						act->success = 1;
						switch(act->do_act)
						{
						case 'E': /* broadcast response to room */
							broadcast_rom(-1,cp->crt->rom_num, act->response, NULL);
							break;
						case 'S': /* say to room */
							sprintf(g_buffer, "%%M says \"%s.\"", act->response);
							broadcast_rom(-1,cp->crt->rom_num, g_buffer, m1arg(crt_ptr));
							break;
						case 'A': /* attack monster in target string */
							if(crt_ptr->first_tlk->target && !crt_ptr->first_enm)
							{
								vic_ptr = find_crt(crt_ptr,rom_ptr->first_mon, 
									crt_ptr->first_tlk->target,1); 
								if(vic_ptr)
									add_enm_crt(vic_ptr->name, crt_ptr);
								broadcast_rom(-1,crt_ptr->rom_num,"%M attacks %m.", 
									m2args(crt_ptr,vic_ptr));
								attack_crt(crt_ptr,vic_ptr);
								if(crt_ptr->first_tlk->target)
									free(crt_ptr->first_tlk->target);
								crt_ptr->first_tlk->target = 0;
							}
							break;
						case 'a': /* attack player target */
							break;
						case 'c': /* cast a spell on target */
							break;
						case 'F': /* force target to do somthing */
							break;
					    case '|': /* set a flag on target */
							break;
					    case '&': /* remove flag on target */
					        break;
						case 'P': /* perform social */
							break;
					    case 'O': /* open door */
							break;
					    case 'C': /* close door */
							break;
						case 'D': /* delay action */
							break;
					    case 'G': /* go into a keyword exit */
							break;
					    case '0': /* go n */
					    case '1': /* go ne */
						case '2': /* go e */
						case '3': /* go se */
					    case '4': /* go s */
						case '5': /* go sw */
					    case '6': /* go w */
						case '7': /* go nw */
					    case '8': /* go up */
					    case '9': /* go down */
							xdir = act->do_act - '0';
							strcpy(cmnd.str[0],xits[xdir]);
							move(crt_ptr,&cmnd);
							break;			
					    }
					}
					if(act->goto_cmd) /* unconditional jump */
					{
						act->success = 1;
						cp->crt->first_tlk->on_cmd = act->goto_cmd;
					}
					else
						cp->crt->first_tlk->on_cmd = on_cmd;
				}
			}
		}
    }  
}


void update_dust_output(long t)
{
        last_dust_output=0;
        broadcast("Ominous thunder rumbles in the distance.");
}


void crash( int sig )
{
	char path[256];

	broadcast("### Quick shutdown now!");
	output_buf();

	elog_broad("--- !CRASH ! Game closed ---");

	switch( sig )
	{
		case SIGABRT:
			elog_broad("--- SIGABORT received  ---");
			break;
		case SIGFPE:
			elog_broad("--- SIGFPE received  ---");
			break;
		case SIGILL:
			elog_broad("--- SIGILL received  ---");
			break;
		case SIGSEGV:
			elog_broad("--- SIGSEGV received  ---");
			break;
		default:
			elog_broad("--- Unknown signal received  ---");
			break;
	}
 
	/* resave_all_rom(1); */
	save_all_ply();

#ifdef DMALLOC 
        dmalloc_shutdown();
#endif /* DMALLOC */
        
	if(RECORD_ALL) {
        sprintf(path,"%s/%s",get_log_path(),"all_cmd");
        unlink(path);
	}
        
	kill(getpid(), 9);

	exit(0);
}



/************************************************************************/
/* heal_crt()								*/
/*		Can be used to heal both players and monsters.		*/
/************************************************************************/
void heal_crt( creature * crt_ptr )
{
	time_t	i, t;

	t = time(0);

	i=LT(crt_ptr, LT_HEALS);

	if(i < t) 
	{
/*   tick code - increase for barbs from 2 points to 5 * con bonus  */

               if(GUILDEXP)
               {
                       if(crt_ptr->type == PLAYER && check_guild(crt_ptr) == 4)
                       {
			     if (crt_ptr->hpcur < crt_ptr->hpmax)  
				if(F_ISSET(crt_ptr,PNOCMP))
                               	   output(crt_ptr->fd,  "Your guild expertise allows you to heal more quickly.\n");
                                 crt_ptr->hpcur += 2;
			     
		       }

                       if(crt_ptr->type == PLAYER && check_guild(crt_ptr) == 2)
                       {
			   if (crt_ptr->mpcur < crt_ptr->mpmax) 
				if(F_ISSET(crt_ptr, PNOCMP))
                               	   output(crt_ptr->fd,  "Your guild expertise allows you to recover more quickly.\n");
                           	crt_ptr->mpcur += 1; 
			   
                       }
               }


                crt_ptr->hpcur += MAX(1, 3 + bonus[(int)crt_ptr->constitution] +
                              (crt_ptr->class == BARBARIAN ? (3 * bonus[(int)crt_ptr->constitution]):0));

		crt_ptr->mpcur += MAX(1, 2+(crt_ptr->intelligence > 17 ? 1:0)+
				((crt_ptr->class == MAGE || crt_ptr->class == ALCHEMIST) ? 2:0));

		crt_ptr->lasttime[LT_HEALS].ltime = t;
		crt_ptr->lasttime[LT_HEALS].interval = 45 - 5*bonus[(int)crt_ptr->piety];

		/* handle quick healing */
		if(F_ISSET(crt_ptr->parent_rom, RHEALR)) 
		{
			crt_ptr->hpcur += 3;
			crt_ptr->mpcur += 2;
			crt_ptr->lasttime[LT_HEALS].interval /= 2;
		}

              if (F_ISSET(crt_ptr, PBESRK) ) 
                      crt_ptr->hpcur = MIN(crt_ptr->hpcur, crt_ptr->hpmax + crt_ptr->level);
              else
                      crt_ptr->hpcur = MIN(crt_ptr->hpcur, crt_ptr->hpmax);

		crt_ptr->mpcur = MIN(crt_ptr->mpcur, crt_ptr->mpmax);
	}

	return;
}
