/*
 * UPDATE.C:
 *
 *  Routines to handle non-player game updates.
 *
 *  Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"
#include "update.h"
#include <stdlib.h>
#ifndef WIN32
#include <sys/signal.h>
#endif
#ifdef DMALLOC
  #include "/usr/local/include/dmalloc.h"
#endif

static long last_update;
static long last_user_update;
static long last_random_update;
static long last_active_update;
static long last_time_update;
static long last_shutdown_update;
static long last_weather_update;
static long last_allcmd;
static long last_security_update;
static long last_action_update;
static long last_active_log;

long        last_exit_update;
long        TX_interval = 4200;
short       Random_update_interval = 13;
int	    Mobilechance = 30;
short       Action_update_interval = 8;
/* This is for debugging the active list */
/* short	    Log_Active_interval = 300; */

static ctag *first_active;
extern void update_allcmd();
extern csparse  Crt[CMAX];
void update_action();

/**********************************************************************/
/*              update_game               */
/**********************************************************************/

/* This function handles all the updates that occur while players are */
/* typing.                                */

void update_game()
{
    long    t;

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
/*  This is for debugging the active list */
/*    if(t - last_active_log >= Log_Active_interval)
        log_act(t); */


    if(Shutdown.ltime && t - last_shutdown_update >= 30)
        if(Shutdown.ltime + Shutdown.interval <= t+500)
            update_shutdown(t);

#ifdef RECORD_ALL
    if(t- last_allcmd >= 120)
 	update_allcmd(t);
#endif /* RECORD_ALL */
}

/**********************************************************************/
/*              update_users                  */
/**********************************************************************/

/* This function controls user updates.  Every 20 seconds it checks a */
/* user's time-out flags (such as invisibility, etc.) and it also     */
/* checks for excessive idle times.  If a user has been idle for over */
/* 30 mins, he is disconnected.                       */

void update_users(t)
long    t;
{
    int i, tout = 300;
    last_user_update = t;

    for(i=0; i<Tablesize; i++) {
        if(!Ply[i].io) continue;
        if(Ply[i].ply && Ply[i].ply->class == DM) continue;
        if(Ply[i].ply && Ply[i].ply->class == CARETAKER) tout = 1200; 
        if(t - Ply[i].io->ltime > tout && Ply[i].io->fn != waiting) {
            write(i, "\n\rTimed out.\n\r", 14);
            disconnect(i);
            continue;
        }

        if(!Ply[i].ply) continue;
        if(Ply[i].ply->fd < 0) continue;
        update_ply(Ply[i].ply); 
    }
}

/**********************************************************************/
/*              update_random                 */
/**********************************************************************/

/* This function checks all player-occupied rooms to see if random monsters */
/* have entered them.  If it is determined that random monster should enter */
/* a room, it is loaded and items it is carrying will be loaded with it.    */

void update_random(t)
long    t;
{
    creature    *crt_ptr;
    object      *obj_ptr;
    room        *rom_ptr;
    int     check[PMAX];
    int     num, m, n, i, j, k, l, total = 0;

    last_random_update = t;
    for(i=0; i<Tablesize; i++) {
        if(!Ply[i].ply || !Ply[i].io) continue;
        if(Ply[i].ply->fd < 0) continue;

        rom_ptr = Ply[i].ply->parent_rom;

        for(j=0; j<total; j++)
            if(check[j] == rom_ptr->rom_num) break;
        if(j < total) continue;

        check[total++] = rom_ptr->rom_num;

        if(mrand(1,100) > rom_ptr->traffic) continue;

        n = mrand(0,9);
        if(!rom_ptr->random[n]) continue;
        m = load_crt(rom_ptr->random[n], &crt_ptr);
        if(m < 0) continue;

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
            if(j<90) j=1;
            else if(j<96) j=2;
            else j=3;
            for(k=0; k<j; k++) {
                m = mrand(0,9);
                if(crt_ptr->carry[m]) {
                    m=load_obj(crt_ptr->carry[m],
                        &obj_ptr);
                    if(m > -1) {
                        if(F_ISSET(obj_ptr, ORENCH))
                            rand_enchant(obj_ptr);
                        obj_ptr->value =
                        mrand((obj_ptr->value*9)/10,
                              (obj_ptr->value*11)/10);
                        add_obj_crt(obj_ptr, crt_ptr);
                    }
                }
            }

            if(!F_ISSET(crt_ptr, MNRGLD) && crt_ptr->gold)
                crt_ptr->gold =
                mrand(crt_ptr->gold/10, crt_ptr->gold);

            if(!l) 
                add_crt_rom(crt_ptr, rom_ptr, num);
            else
                add_crt_rom(crt_ptr, rom_ptr, 0);

            add_active(crt_ptr);
            if(l != num-1)
                load_crt(rom_ptr->random[n], &crt_ptr);
        }
    }
}

/************************************************************************/
/*              update_active               */
/************************************************************************/

/* This function updates the activities of all monsters who are currently */
/* active (ie. monsters on the active list).  Usually this is reserved    */
/* for monsters in rooms that are occupied by players.            */

void update_active(t)
long    t;
{
    creature    *crt_ptr=0, *att_ptr=0;
    object      *obj_ptr=0;
    room        *rom_ptr=0;
    ctag        *cp=0;
    char        *enemy, **file;
    long        i,z, ret_address;
    int     rtn = 0, n, fd, p=0, db, size, line;

    /* signal(SIGBUS, abort); */
    last_active_update = t;
    if(!(cp = first_active)) return;
    while(cp) {
#ifdef DMALLOC
	db = dmalloc_verify (cp);  
#endif /* DMALLOC */
        if(!(cp->crt)) {
		merror("cp in active", NONFATAL);
		log_act(t);
		break;
	}

        crt_ptr = cp->crt;
	if(!crt_ptr) {
		merror("crt_ptr in active",NONFATAL);
 		log_act(t);
		break;
	}

        rom_ptr = crt_ptr->parent_rom;
	if(!rom_ptr) {
		merror("rom_ptr in active", NONFATAL);
                log_act(t);
		break;
        }

        if(!rom_ptr->first_ply) {
            del_active(crt_ptr);
            cp = first_active;
            continue;
        }

        i = LT(crt_ptr, LT_ATTCK);
        if(i > t) {
            cp = cp->next_tag;
            continue;
        }

        i = LT(crt_ptr, LT_HEALS);
        while(i <= t && (crt_ptr->hpcur < crt_ptr->hpmax ||
              crt_ptr->mpcur < crt_ptr->mpmax)) {
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
	if (t > LT(crt_ptr, LT_CHRMD)&&F_ISSET(crt_ptr, MCHARM))
		F_CLR(crt_ptr, MCHARM);

        crt_ptr->lasttime[LT_ATTCK].ltime = t;
        if(crt_ptr->dexterity < 20)
            crt_ptr->lasttime[LT_ATTCK].interval = 3;
        else
            crt_ptr->lasttime[LT_ATTCK].interval = 2;

        if(F_ISSET(crt_ptr, MSCAVE)) {
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
                          crt_ptr, obj_ptr);
            }
            if(t-i > 20)
                crt_ptr->lasttime[LT_MSCAV].ltime = t;
        }
	if(F_ISSET(crt_ptr, MMOBIL) && !F_ISSET(crt_ptr, MPERMT) && !F_ISSET(crt_ptr, MDMFOL)){
		i = crt_ptr->lasttime[LT_MWAND].ltime;
		if(t-i > 20 && mrand(1,100) >20) {
			n = mobile_crt(crt_ptr);
			if(!n)
				F_CLR(crt_ptr, MMOBIL);
			cp =cp->next_tag;
			continue;
		}
	}
        if(!F_ISSET(crt_ptr, MHASSC) && !F_ISSET(crt_ptr, MPERMT) && !F_ISSET(crt_ptr, MDMFOL)) {
            i = crt_ptr->lasttime[LT_MWAND].ltime;
            if(t-i > 20 
               && mrand(1,100) <= crt_ptr->parent_rom->traffic
               && !crt_ptr->first_enm) {
			if(mrand(1,100) < Mobilechance) {
				F_SET(crt_ptr, MMOBIL);
				cp = cp->next_tag;
				continue;
			}
          broadcast_rom(-1, crt_ptr->rom_num,
                    "%1M just wandered away.",
                    crt_ptr);
                del_crt_rom(crt_ptr, rom_ptr);
                del_active(crt_ptr);
                free_crt(crt_ptr); 
                cp = first_active;
                continue;
	     }
            if(t-i > 20)
                crt_ptr->lasttime[LT_MWAND].ltime = t;
        }

	if(!crt_ptr->first_enm && (!F_ISSET(crt_ptr, MAGGRE) && 
            !F_ISSET(crt_ptr, MGAGGR) && !F_ISSET(crt_ptr, MEAGGR))) {
            cp = cp->next_tag;
            continue;

        }            
        if(crt_ptr->first_enm) 
          if(update_combat(crt_ptr))
          {
            cp = first_active;
            continue;
          }

        if(F_ISSET(crt_ptr, MAGGRE) || F_ISSET(crt_ptr, MGAGGR) ||
		F_ISSET(crt_ptr, MEAGGR)) {
	if (F_ISSET(crt_ptr, MAGGRE))
           	att_ptr = lowest_piety(rom_ptr, F_ISSET(crt_ptr, MDINVI) ? 1:0);
			else {
				att_ptr = low_piety_alg(rom_ptr,F_ISSET(crt_ptr, MDINVI) ? 1:0,F_ISSET(crt_ptr,MGAGGR) ? -1 : 1,crt_ptr->level);
				}
            if(!att_ptr) {
                cp = cp->next_tag;
                continue;
            }
	    if (!is_enm_crt(att_ptr, crt_ptr)) {
	            ANSI(att_ptr->fd, RED);
        	    print(att_ptr->fd, "%M attacks you.\n", crt_ptr);
		    ANSI(att_ptr->fd, WHITE);
	            broadcast_rom(att_ptr->fd, att_ptr->rom_num, 
                      "%M attacks %m.",
                      crt_ptr, att_ptr);
	     }
           crt_ptr->lasttime[LT_ATTCK].interval = 3;    
     	   add_enm_crt(att_ptr->name, crt_ptr); 
        }

        cp = cp->next_tag;

    }
}

/************************************************************************/
/*              choose_item             */
/************************************************************************/

/* This function randomly chooses an item that the player pointed to    */
/* by the first argument is wearing.                    */

int choose_item(ply_ptr)
creature    *ply_ptr;
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

/**********************************************************************/
/*              crt_spell                 */
/**********************************************************************/

/* This function allows monsters to cast spells at players.       */

int crt_spell(crt_ptr, att_ptr)
creature    *crt_ptr;
creature    *att_ptr;
{
    cmd cmnd;
    int i, j, spl, c;
    int known[10], knowctr = 0;
    int (*fn)();

    for(i=0; i<16; i++) {
        if(!crt_ptr->spells[i]) continue;
        for(j=i*8; j<=(i*8+7); j++) {
            if(knowctr > 9) break;
            if(S_ISSET(crt_ptr, j))
                known[knowctr++] = j;
        }
        if(knowctr > 9) break;
    }

    if(!knowctr)
        spl = 1;
    else {
        i = mrand(1, knowctr);
        spl = known[i-1];
    }

	if ((spllist[spl].splno != SVIGOR) && 
		(spllist[spl].splno != SMENDW) &&
		(spllist[spl].splno != SFHEAL)){
    	strcpy(cmnd.str[2], att_ptr->name);
    	cmnd.val[2] = 1;
    	cmnd.num = 3;
	} else
	 	cmnd.num = 2;

    fn = spllist[spl].splfn;

    ANSI(att_ptr->fd, RED);
    if(fn == offensive_spell) {
        for(c=0; ospell[c].splno != spllist[spl].splno; c++)
            if(ospell[c].splno == -1) return(0);
        i = (*fn)(crt_ptr, &cmnd, CAST, spllist[spl].splstr,
            &ospell[c]);
    }
    else i = (*fn)(crt_ptr, &cmnd, CAST);
    ANSI(att_ptr->fd, WHITE);
    return(i);
}

/**********************************************************************/
/*              update_time               */
/**********************************************************************/

/* This function updates the game time in hours.  When it is 6am a sunrise */
/* message is broadcast.  When it is 8pm a sunset message is broadcast.    */

void update_time(t)
long    t;
{
    int daytime;

    last_time_update = t;

    Time++;
    daytime = (int)(Time % 24L);

    if(daytime == 6)
        broadcast("The sun rises.");
    else if(daytime == 20)
        broadcast("The sun disappears over the horizon.");

}

/**********************************************************************/
/*              update_shutdown               */
/**********************************************************************/

/* This function broadcasts a shutdown message every 30 seconds until */
/* shutdown is achieved.  Then it saves off all rooms and players,    */
/* and exits the game.                            */

void update_shutdown(t)
long t;
{
    long    i;
     char path[128];

    last_shutdown_update = t;

    i = Shutdown.ltime + Shutdown.interval;
    if(i > t) {
        if(i-t > 60)
            broadcast("### Game backup shutdown in %d:%02d minutes.", 
                  (i-t)/60L, (i-t)%60L);
        else
            broadcast("### Game shutdown in %d seconds.", i-t);
    }
    else {
        broadcast("### Shutting down now.");
        output_buf();
        loge("--- Game shut down ---\n");
        resave_all_rom(1);
        save_all_ply();

#ifdef DMALLOC
	dmalloc_log_stats();
#endif /* DMALLOC */
/* #ifdef RECORD_ALL
	sprintf(path,"%s/%s",LOGPATH,"all_cmd");
	unlink(path);
#endif
*/	
#ifndef WIN32	
	kill(getpid(), 9);
#endif
        exit(0);
    }
}

/**********************************************************************/
/*              add_active                */
/**********************************************************************/

/* This function adds a monster to the active-monster list.  A pointer */
/* to the monster is passed in the first parameter.            */

void add_active(crt_ptr)
creature    *crt_ptr;
{
    ctag    *cp, *ct;
    long	t;

/*    del_active(crt_ptr); */
    if(!crt_ptr)
        return;
    if(is_crt_active(crt_ptr))
	return;

    ct = 0;
    ct = (ctag *)malloc(sizeof(ctag));
    if(!ct)
        merror("add_active", FATAL);

    ct->crt = crt_ptr;
    ct->next_tag = 0;

    if(!first_active)
        first_active = ct;
    else {
        ct->next_tag = first_active;
        first_active = ct;
    }

}

/**********************************************************************/
/*              del_active                */
/**********************************************************************/

/* This function removes a monster from the active-monster list.  The */
/* parameter contains a pointer to the monster which is to be removed */

void del_active(crt_ptr)
creature    *crt_ptr;
{
    ctag    *cp, *prev;
    long    t;

    if(!(cp = first_active)) return;

    if(!(is_crt_active(crt_ptr))) return;
    
    if(cp->crt == crt_ptr) {
        first_active = cp->next_tag;
        free(cp);
        return;
    }

    prev = cp;
    cp = cp->next_tag;
    while(cp) {
        if(cp->crt == crt_ptr) {
            prev->next_tag = cp->next_tag;
            free(cp);
            return;
        }
        prev = cp;
        cp = cp->next_tag;
    }

}
/*===================================================================*/
/*              update_exit              */
/*===================================================================*/
void update_exit(t)
long    t;
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
                broadcast("\n%s",tmp);
            t_toggle = !t_toggle;
                    
     }
}

/*====================================================================*/

void update_allcmd(t)
long t;
{
    char path[128];

    last_allcmd = t;
    sprintf(path,"%s/%s",LOGPATH,"all_cmd");
    unlink(path);
}

/*************************************/
int list_act(ply_ptr, cmnd)
creature *ply_ptr;
cmd	*cmnd;

{
ctag	*cp;

if (ply_ptr->class < CARETAKER)
	return(0);
	print(ply_ptr->fd,"### Active monster list ###\n");
	print(ply_ptr->fd,"Monster    -    Room Number\n");
	cp = first_active;
	
	while(cp){
	print(ply_ptr->fd,"%s - %d.\n",cp->crt->name, cp->crt->parent_rom->rom_num);
	cp = cp->next_tag;
	}
output_buf();
return;
}

/* ********* */
/* This logs the active list for debugging */
void log_act(t)
long t;
{
ctag    *cp;
char	rfile[128];
last_active_log = t;
	
	sprintf(rfile,"%s/%s",LOGPATH,"Active_list");
	unlink(rfile);

	logn("Active_list","Active monster list\n");
        cp = first_active;
        while(cp){
        logn("Active_list","%s - %d.\n",cp->crt->name, cp->crt->parent_rom->rom_num);
        cp = cp->next_tag;
        }
return;
}


/****************************************************************/
/*		update_weather					*/
/****************************************************************/
/*								*/
void update_weather(t)
long	t;
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
					broadcast("The earth trembles under your feet.");
					break;
					case 1:
					Weather[j].ltime = t;
					broadcast("A heavy fog blankets the earth.");
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
					broadcast("It's a beautiful day today.");
					break;
					case 1: 
					Weather[j].ltime = t;
					broadcast("The sun shines brightly across the land.");
					break;
					case 2:
                                        Weather[j].ltime = t;
					broadcast("The sun shines brightly across the land.");
					break;
					case3:
                                        Weather[j].ltime = t;
                                        broadcast("The glaring sun beats down upon the inhabitants of Isengard."); 
					break;
					case4:
                                        Weather[j].ltime = t;
                                        broadcast("The heat today is unbearable.");
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
					broadcast("The air is still and quiet.");
					break;
					case 1:
                                        Weather[j].ltime = t;
					broadcast("A light breeze blows from the south.");
					break;
					case 2:
                                        Weather[j].ltime = t;
					broadcast("A strong wind blows across the land.");
					break;
					case 3:
                                        Weather[j].ltime = t;
					broadcast("The wind gusts, blowing debris through the streets.");
					break;
					case 4:
                                        Weather[j].ltime = t;
					broadcast("Gale force winds blow in from the Sea of Tranquility.");
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
                                        broadcast("Clear, blue skies cover the land of Isengard.");
                                	break;
                                	case 1:
                                        Weather[j].ltime = t;
                                        broadcast("Light clouds appear over the Mountains of Saeros.");
                                	break;
                                	case 2:
                                        Weather[j].ltime = t;
                                        broadcast("Thunderheads roll in from the east.");
                                	break;
					case 3:
					Weather[j].ltime =t;
					broadcast("A light rain falls quietly across Isengard.");
                                	break;
					case 4:
					Weather[j].ltime = t;
					broadcast("A heavy rain begins to fall.");
					break;
					case 5:
                                        Weather[j].ltime = t;
                                        broadcast("Sheets of rain pour down from the skies.");
                                	break;
        				case 6:
                                        Weather[j].ltime = t;
                        		broadcast("A torrent soaks the ground.");
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
						broadcast("The sky is dark as pitch.");
						break;
					case 1:
                                        	Weather[j].ltime = t;
						broadcast("A sliver of silver can be seen in the night sky.");
						break;
					case 2:
	                                        Weather[j].ltime = t;
						broadcast("Half a moon lights the evening skies.");
						break;
					case 3:
	                                        Weather[j].ltime = t;
						broadcast("The night sky is lit by the waxing moon.");
						break;
					case 4:
	                                        Weather[j].ltime = t;
						broadcast("The full moon shines across the land.");
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

void update_security(t)
long	t;
{
int i, booted=0, match=0, j, l, fdtemp=0;
last_security_update=t;


        for(i=0; i<Tablesize; i++) {
                if(!Ply[i].ply) continue;
		if(!Ply[i].io) continue;
                if(Ply[i].ply->fd < 1) continue;
		if(F_ISSET(Ply[i].ply, PSECOK)) continue; 
#ifdef SECURE
		
		for(l=0; l<Tablesize; l++) {
                if(!Ply[l].ply) continue;
                if(Ply[l].ply->fd < 1) continue;
			if(!strcmp(Ply[i].io->userid, Ply[l].io->userid) && i != l && strcmp(Ply[i].io->userid, "no_port") && strcmp(Ply[i].io->userid, "unknown")) {
                        	match +=1;
				fdtemp = l;
				break;
                 	}
			if((strcmp(Ply[i].io->userid, "no_port") && strcmp(Ply[i].io->userid, "unknown")) && F_ISSET(Ply[i].ply, PAUTHD))
				F_CLR(Ply[i].ply, PAUTHD);
		}
		if(match > 0){
                        print(Ply[i].ply->fd, "\n\nThe Watcher just arrived.\n");
                        print(Ply[i].ply->fd, "The Watcher says, \"You may only play one character at a time.\"\n"); 
                        print(Ply[i].ply->fd, "The Watcher waves goodbye.\n");
			output_buf();
			disconnect(i);
			if(fdtemp) disconnect(fdtemp);  
			match=0;
			fdtemp=0;
			continue;
		 }
#endif /* SECURE */
	        /* Check for username lockout */
		for(j=0; j<Numlockedout; j++) {
		if(!Ply[i].io) break;
                if(strcmp(Lockout[j].userid, Ply[i].io->userid))  
                        continue;
	                else {
                        write(Ply[i].ply->fd, "\n\rThe Watcher has locked your account.\n\r", 42);
                        write(Ply[i].ply->fd, "\n\rSend questions to auth@moria.bio.uci.edu.\n\r", 46);
			booted +=1;
			disconnect(Ply[i].ply->fd);                        
	        	break;
			}
        	}
        	if(booted) {
		 	booted =0;
			 continue;
		}
#ifdef SECURE
		/* Check for no_port */
		if((!strcmp(Ply[i].io->userid, "no_port") || !strcmp(Ply[i].io->userid, "unknown")) && Ply[i].ply->level > 2 && !F_ISSET(Ply[i].ply, PAUTHD))
        		if(t-Ply[i].ply->lasttime[LT_SECCK].ltime > 60) {
				print(Ply[i].ply->fd, "\n\n\rI am unable to get authorization for your account.\n");
				print(Ply[i].ply->fd, "Logging out now.\n\nSend any questions to register@moria.bio.uci.edu.\n\n");
				output_buf();
				disconnect(Ply[i].ply->fd);
				continue;
			}
			else {
				if(Ply[i].ply->lasttime[LT_SECCK].interval=t){
				print(Ply[i].ply->fd, "\n\rChecking for authorization.\n");
				print(Ply[i].ply->fd, "Your time will be limited if I cannot get authorization.\n\n");
				Ply[i].ply->lasttime[LT_SECCK].interval=t+20;
				}
				continue;
			}		
#endif /* SECURE */

		/* passed security check */
		/* if(!F_ISSET(Ply[i].ply, PSECOK)) 
			print(Ply[i].ply->fd, "\n\rWelcome to Isengard!\n"); */
		F_SET(Ply[i].ply, PSECOK);
		continue;
	}
}

/****************************************************************************
 *  UPDATE_ACTION is the update function for logic scripts
 *  
 */

void update_action(t)
long    t;
{
    creature    *crt_ptr,*vic_ptr;
    object      *obj_ptr;
    room        *rom_ptr;
    ctag        *cp,*vcp;
    ttag        *act,*tact;
    int         i,on_cmd,check; 
    int         xdir;
    char        *xits[] =  { "n","ne","e","se","s","sw","w","nw","u","d" };
    cmd         cmnd;
   
   
    last_action_update = t;
    
    for(cp = first_active;cp;cp = cp->next_tag)
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
			vic_ptr = find_crt(crt_ptr, rom_ptr->first_ply,
                                           act->response, 1);
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
			obj_ptr = find_obj(crt_ptr, rom_ptr->first_obj,
                                           act->response, 1);
			
			if(obj_ptr)
			  {
			    if(crt_ptr->first_tlk->target)
			          free(crt_ptr->first_tlk->target);
			    crt_ptr->first_tlk->target = (char *)
			          calloc(1,strlen(act->response)+1);
			    strcpy(crt_ptr->first_tlk->target,act->response);
			    act->success = 1;
			    /* loge(vic_ptr->name); */
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
		        vic_ptr = find_crt(crt_ptr, rom_ptr->first_mon,
                                           act->response, 1);
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
			broadcast_rom(-1,cp->crt->rom_num,"%s",act->response);
			break;
		      case 'S': /* say to room */
			broadcast_rom(-1,cp->crt->rom_num,"%M says \"%s.\"",
				      crt_ptr,act->response);
			break;
		      case 'A': /* attack monster in target string */
			if(crt_ptr->first_tlk->target && !crt_ptr->first_enm)
			  {
			    vic_ptr = find_crt(crt_ptr,rom_ptr->first_mon,
					       crt_ptr->first_tlk->target,1); 
			    if(vic_ptr)
			      add_enm_crt(vic_ptr->name, crt_ptr);
			      broadcast_rom(-1,crt_ptr->rom_num,"%M attacks %s.",crt_ptr,vic_ptr);
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

/*********************************************************************/
/*			is_crt_active				     */
/*********************************************************************/
/*	This function returns 1 if the parameter passed is in the    */
/*	active list.						     */

int is_crt_active(crt_ptr)
creature    *crt_ptr;
{
	ctag	*cp=0;
	int	  n=0;

	if(!(cp = first_active)) 
		return(0);

	while (cp) {
		if(cp->crt == crt_ptr){
			n = 1;	
			break;
		}
	cp = cp->next_tag;
	}
	return(n);
}		
