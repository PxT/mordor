/*
 * COMMAND5.C:
 *
 *  Additional user routines.
 *
 *  Copyright (C) 1991, 1992, 1993, 1997 Brooke Paul & Brett Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"
#include <ctype.h>
#ifdef DMALLOC
  #include "/usr/local/include/dmalloc.h"
#endif

/**********************************************************************/
/*              attack                    */
/**********************************************************************/

/* This function allows the player pointed to by the first parameter */
/* to attack a monster.                          */

int attack(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    creature    *crt_ptr;
    room        *rom_ptr;
    long        i, t;
    int     fd;


    fd = ply_ptr->fd;

    t = time(0);
    i = LT(ply_ptr, LT_ATTCK);

    if(t < i) {
        please_wait(fd, i-t);
        return(0);
    }

    if(cmnd->num < 2) {
        print(fd, "Attack what?\n");
        return(0);
    }

    rom_ptr = ply_ptr->parent_rom;

    crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
               cmnd->str[1], cmnd->val[1]);

    if(!crt_ptr) {
        cmnd->str[1][0] = up(cmnd->str[1][0]);
        crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                   cmnd->str[1], cmnd->val[1]);

        if(!crt_ptr || crt_ptr == ply_ptr || strlen(cmnd->str[1]) < 3) {
            print(fd, "You don't see that here.\n");
            return(0);
        }

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

int attack_crt(ply_ptr, crt_ptr)
creature    *ply_ptr;
creature    *crt_ptr;
{
    long    i, t;
    int		fd, m, n, p, addprof;

    fd = ply_ptr->fd;

    t = time(0);
    i = LT(ply_ptr, LT_ATTCK);

    if(t < i)
        return(0);
	if (crt_ptr->type == PLAYER) {
         if(is_charm_crt(ply_ptr->name, crt_ptr) && F_ISSET(ply_ptr, PCHARM)) {
                print(fd, "You like %s too much to do that.\n", crt_ptr->name);
                return(0);
         }
	else
		del_charm_crt(ply_ptr->name, crt_ptr);

    }
    F_CLR(ply_ptr, PHIDDN);
    if(F_ISSET(ply_ptr, PINVIS)) {
        F_CLR(ply_ptr, PINVIS);
        print(fd, "Your invisibility fades.\n");
        broadcast_rom(fd, ply_ptr->rom_num, "%M fades into view.",
                  ply_ptr);
    }

    ply_ptr->lasttime[LT_ATTCK].ltime = t;
    if(F_ISSET(ply_ptr, PHASTE))
        ply_ptr->lasttime[LT_ATTCK].interval = 2;
    else
        ply_ptr->lasttime[LT_ATTCK].interval = 3;

    if(F_ISSET(ply_ptr, PBLIND))
	ply_ptr->lasttime[LT_ATTCK].interval = 7;
    	
        if(crt_ptr->type == MONSTER) 
	 {
          if(F_ISSET(crt_ptr, MUNKIL)) 	
	   {
            print(fd, "You cannot harm %s.\n",
                F_ISSET(crt_ptr, MMALES) ? "him":"her");
            return(0);
            }

	if(F_ISSET(ply_ptr, PALIAS)) 
	 {
		print(fd, "You attack %m.\n", crt_ptr);
		broadcast_rom(fd, ply_ptr->rom_num, "%M attacks %m.",
                      ply_ptr, crt_ptr);
         }
	else if(add_enm_crt(ply_ptr->name, crt_ptr) < 0 ) 
	{
		/* if(is_charm_crt(crt_ptr->name, ply_ptr))
			del_charm_crt(crt_ptr, ply_ptr); */
           
	    	print(fd, "You attack %m.\n", crt_ptr);
            	broadcast_rom(fd, ply_ptr->rom_num, "%M attacks %m.",
                      ply_ptr, crt_ptr);
        }


        if(F_ISSET(crt_ptr, MMGONL)) {
            print(fd, "Your weapon has no effect on %m.\n",
                crt_ptr);
            return(0);
        }
        if(F_ISSET(crt_ptr, MENONL)) {
            if(!ply_ptr->ready[WIELD-1] || 
                ply_ptr->ready[WIELD-1]->adjustment < 1) {
                print(fd, "Your weapon has no effect on %m.\n",
                    crt_ptr);
                return(0);
            }
        }
    }
    else {
        if(F_ISSET(ply_ptr->parent_rom, RNOKIL) && ply_ptr->class < DM) {
            print(fd, "No killing allowed in this room.\n");
            return(0);
        }
 
            if((!F_ISSET(ply_ptr,PPLDGK) || !F_ISSET(crt_ptr,PPLDGK)) ||
                (BOOL(F_ISSET(ply_ptr,PKNGDM)) == BOOL(F_ISSET(crt_ptr,PKNGDM))) ||
                (! AT_WAR)) {
			
				if(ISENGARD) {
					if(!F_ISSET(ply_ptr, PCHAOS) && ply_ptr->class < DM && (ply_ptr->class != THIEF || ply_ptr->class != ASSASSIN || ply_ptr->class != PALADIN || ply_ptr->class != FIGHTER)) {
						print(fd, "Sorry, you're lawful.\n");
				        return (0);
			        }
				}
				else {
					if(!F_ISSET(ply_ptr, PCHAOS) && ply_ptr->class < DM) {      
						print(fd, "Sorry, you're lawful.\n");
			            return (0);
					}
				}

                if(!F_ISSET(crt_ptr, PCHAOS) && ply_ptr->class < DM) {
                    print(fd, "Sorry, that player is lawful.\n");
                    return (0);
                }     
	    }
        ply_ptr->lasttime[LT_ATTCK].interval += 5;
        print(crt_ptr->fd, "%M attacked you!\n", ply_ptr);
        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
                   "%M attacked %m!", ply_ptr, crt_ptr);
    }

    if(ply_ptr->ready[WIELD-1]) {
        if(ply_ptr->ready[WIELD-1]->shotscur < 1) {
            print(fd, "Your %s is broken.\n", 
                  ply_ptr->ready[WIELD-1]->name);
            broadcast_rom(fd, ply_ptr->rom_num, "%M broke %s %s.",
                ply_ptr,
                F_ISSET(ply_ptr, PMALES) ? "his":"her",
                ply_ptr->ready[WIELD-1]->name);
	    if(F_ISSET(ply_ptr->ready[WIELD-1], OTMPEN)) {
		ply_ptr->ready[WIELD-1]->pdice=0;
		F_CLR(ply_ptr->ready[WIELD-1], OTMPEN);
		F_CLR(ply_ptr->ready[WIELD-1], ORENCH);
                F_CLR(ply_ptr->ready[WIELD-1], OENCHA);
                if(F_ISSET(ply_ptr, PDMAGI))
                   print(ply_ptr->fd, "The enchantment fades on your %s.\n", ply_ptr->ready[WIELD-1]);
	    }
            add_obj_crt(ply_ptr->ready[WIELD-1], ply_ptr);
            ply_ptr->ready[WIELD-1] = 0;
            return(0);
        }
    }

    n = ply_ptr->thaco - crt_ptr->armor/10;
    
    if (F_ISSET(ply_ptr, PFEARS))
		n += 2;

    if (F_ISSET(ply_ptr, PBLIND))
		n += 5;

    if(mrand(1,20) >= n) {
        if(ply_ptr->ready[WIELD-1])
            n = mdice(ply_ptr->ready[WIELD-1]) + 
                bonus[ply_ptr->strength];
        else
            n = mdice(ply_ptr) + bonus[ply_ptr->strength];

        if(crt_ptr->class >= CARETAKER) 
            n = 0;
        n = MAX(1,n);

        if(ply_ptr->class == PALADIN) {
            if(ply_ptr->alignment < 0) {
                n /= 2;
                print(fd, "Your evilness reduces your damage.\n");
            }
            else if(ply_ptr->alignment > 250) {
                n += mrand(1,3);
                print(fd, "Your goodness increases your damage.\n");
            }
        }
	if(ply_ptr->class == MONK) {
		if(ply_ptr->ready[WIELD-1] && !F_ISSET(ply_ptr->ready[WIELD-1], OMONKO)){
			print(fd, "How can you attack well with your hands full?\n");
			n /=2;
		}
	}

        p = mod_profic(ply_ptr);
        if(mrand(1,100) <= p || (ply_ptr->ready[WIELD-1] && F_ISSET(ply_ptr->ready[WIELD-1],OALCRT))) {
            ANSI(fd, GREEN);
            print(fd, "CRITICAL HIT!\n");
            ANSI(fd, WHITE);
            broadcast_rom(fd, ply_ptr->rom_num,
                "%M made a critical hit.", ply_ptr);
            n *= mrand(3,6);
            if(ply_ptr->ready[WIELD-1] && (!F_ISSET(ply_ptr->ready[WIELD-1], ONSHAT)) 
&&((mrand(1,100) > (Ply[ply_ptr->fd].extr->luck+40) || mrand(1,100)<3) || 
(F_ISSET(ply_ptr->ready[WIELD-1],OALCRT)))) {
                print(fd, "Your %s shatters.\n",
                    ply_ptr->ready[WIELD-1]->name);
                broadcast_rom(fd, ply_ptr->rom_num,
                    "%s %s shattered.",
                    F_ISSET(ply_ptr, PMALES) ? "His":"Her",
                    ply_ptr->ready[WIELD-1]->name);
                free_obj(ply_ptr->ready[WIELD-1]);
                ply_ptr->ready[WIELD-1] = 0;
            }
        }
        else if(ply_ptr->type == PLAYER && (mrand(1,100) <= (5-p) || mrand(1,100) > Ply[ply_ptr->fd].extr->luck+ply_ptr->level*2)&& ply_ptr->ready[WIELD-1] && !F_ISSET(ply_ptr->ready[WIELD-1], OCURSE)) {
            ANSI(fd, GREEN);
            print(fd, "You FUMBLED your weapon.\n");
            ANSI(fd, WHITE);
            broadcast_rom(fd, ply_ptr->rom_num, 
                "%M fumbled %s weapon.", ply_ptr,
                F_ISSET(ply_ptr, PMALES) ? "his":"her");
            n = 0;
            add_obj_crt(ply_ptr->ready[WIELD-1], ply_ptr);
            ply_ptr->ready[WIELD-1] = 0;
            compute_thaco(ply_ptr);
        }

        print(fd, "You hit for %d damage.\n", n);
        print(crt_ptr->fd, "%M hit you for %d damage.\n",
              ply_ptr, n);
	if (ply_ptr->type == MONSTER && crt_ptr->type == MONSTER)
	    broadcast_rom2(crt_ptr->fd,fd,crt_ptr->rom_num,"%M hits %m!", ply_ptr,crt_ptr);
        if(ply_ptr->ready[WIELD-1] && !mrand(0,3))
            ply_ptr->ready[WIELD-1]->shotscur--;

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
	    else if(ply_ptr->class == MONK) { 
	    /* give blunt prof for monk barehand */
		addprof = (m * crt_ptr->experience) /
                    MAX(crt_ptr->hpmax, 1);
		addprof = MIN(addprof, crt_ptr->experience);
		ply_ptr->proficiency[2] += addprof;
	   }
	}
        if(crt_ptr->hpcur < 1) {
            print(fd, "You killed %m.\n", crt_ptr);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                      "%M killed %m.", ply_ptr, crt_ptr);
            die(crt_ptr, ply_ptr);
            return(1);
        }
        else
            check_for_flee(crt_ptr);
    }
    else {
        print(fd, "You missed.\n");
        print(crt_ptr->fd, "%M missed.\n", ply_ptr);
	if (ply_ptr->type == MONSTER && crt_ptr->type == MONSTER)
  	  broadcast_rom2(fd,crt_ptr->fd,crt_ptr->rom_num,"%M missed %m.",ply_ptr,crt_ptr);
    }

    return(0);
}

/**********************************************************************/
/*              who                   */
/**********************************************************************/

/* This function outputs a list of all the players who are currently */
/* logged into the game.  It includes their titles and the last      */
/* command they typed.                           */

int who(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    char    str[15];
    int fd, i, j;

    fd = ply_ptr->fd;

	 if(F_ISSET(ply_ptr, PBLIND)){
                ANSI(fd, RED);
                print(fd, "You're blind!\n");
                ANSI(fd, WHITE);
                return(0);
	}

	if(LASTCOMMAND)
		print(fd, "%-23s  %-20s     %-20s\n", "Player", "Title", "Last command");
	else {
	    ANSI (fd, BOLD);
	    ANSI (fd, BLUE);
		print(fd, "%-23s  %-20s     %-20s\n", "Player", "Title", "Race");
	}

    print(fd, "-----------------------------------------------------------------\n");
    ANSI(fd, NORMAL);
    ANSI(fd, WHITE);
    for(i=0; i<Tablesize; i++) {
        if(!Ply[i].ply) continue;
        if(Ply[i].ply->fd == -1) continue;
        if(F_ISSET(Ply[i].ply, PDMINV) && Ply[i].ply->class == DM &&
           ply_ptr->class < CARETAKER)
            continue;
        if(F_ISSET(Ply[i].ply, PDMINV) && ply_ptr->class < DM)
            continue;
        if(F_ISSET(Ply[i].ply, PINVIS) && !F_ISSET(ply_ptr, PDINVI) &&
           ply_ptr->class < CARETAKER)
            continue;
		ANSI(fd, WHITE);
        print(fd, "%-20s%s  ", Ply[i].ply->name, 
              (F_ISSET(Ply[i].ply, PDMINV) ||
              F_ISSET(Ply[i].ply, PINVIS)) ? "(*)":"   ");
		ANSI(fd, GREEN);
		print(fd, "%-20s     ", title_ply(Ply[i].ply));
		ANSI(fd, WHITE);

		if(LASTCOMMAND) {
		    strncpy(str, Ply[i].extr->lastcommand, 14);
			for(j=0; j<15; j++)
				if(str[j] == ' ') {
					str[j] = 0;
	                break;
		        }
			if(!str[0])
				print(fd, "Logged in\n");
	        else
		        print(fd, "%s\n", str);
		}
		else {
			ANSI(fd, CYAN);
	        print(fd, "%-14s\n", race_str[Ply[i].ply->race]);
			ANSI(fd, WHITE);
		}

    }
    print(fd, "\n");

    return(0);

}
 
/**********************************************************************/
/*                              whois                                 */
/**********************************************************************/
/* The whois function displays a selected player's name, class, level *
 * title, age and gender */
      
int whois(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    creature    *crt_ptr;
    int     fd;
 
    fd = ply_ptr->fd;
 
    if(cmnd->num < 2) {
        print(fd, "Whois who?\n");
        return(0);
    }
 
    lowercize(cmnd->str[1], 1);
    crt_ptr = find_who(cmnd->str[1]);
 
    if(!crt_ptr || F_ISSET(crt_ptr, PDMINV) || F_ISSET(ply_ptr, PBLIND) ||
       (F_ISSET(crt_ptr, PINVIS) && !F_ISSET(ply_ptr, PDINVI))) {
        print(fd, "That player is not logged on.\n");
        return(0);
    }
 
	ANSI(fd, YELLOW); 
       print(fd, "%-20s  %-3s %-3s [Lv]%-20s  %-3s  %-10s\n", "Player", "Cls", "Gen", "Title", "Age", "Race");
	ANSI(fd, BLUE); 
        print(fd, "------------------------------------------------------------------------\n");
	ANSI(fd, WHITE);
        print(fd, "%-20s  %-3.3s %-3s [%02d]%-20s  %-3d  ",
                crt_ptr->name,
                class_str[crt_ptr->class],
                F_ISSET(crt_ptr, PMALES) ? " M":" F",  
                crt_ptr->level,
                title_ply(crt_ptr), 
                18 + crt_ptr->lasttime[LT_HOURS].interval/86400L);
print(fd, "%-10s\n", race_str[crt_ptr->race]);
 	ANSI(fd, NORMAL);
 	ANSI(fd, WHITE);
    return(0);
}

/**********************************************************************/
/*              search                    */
/**********************************************************************/

/* This function allows a player to search a room for hidden objects,  */
/* exits, monsters and players.                        */

int search(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    room    *rom_ptr;
    xtag    *xp;
    otag    *op;
    ctag    *cp;
    long    i, t;
    int fd, chance, found = 0;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    chance = 15 + 5*bonus[ply_ptr->piety] + ply_ptr->level*2;
    chance = MIN(chance, 90);
    if(ply_ptr->class == RANGER)
        chance += ply_ptr->level*8;
    if(ply_ptr->class == DRUID)
	chance += ply_ptr->level*6;
    if(F_ISSET(ply_ptr, PBLIND))
	chance = MIN(chance, 20);
    if(ply_ptr->class >= CARETAKER)
        chance = 100;

    t = time(0);
    i = LT(ply_ptr, LT_SERCH);

    if(t < i) {
        please_wait(fd, i-t);
        return(0);
    }

    F_CLR(ply_ptr, PHIDDN);

    ply_ptr->lasttime[LT_SERCH].ltime = t;
    if(ply_ptr->class == RANGER || ply_ptr->class == DRUID)
        ply_ptr->lasttime[LT_SERCH].interval = 3;
    else
        ply_ptr->lasttime[LT_SERCH].interval = 7;

    xp = rom_ptr->first_ext;
    while(xp) {
        if(F_ISSET(xp->ext, XSECRT) && mrand(1,100) <= chance) 
           if((F_ISSET(xp->ext, XINVIS) && F_ISSET(ply_ptr,PDINVI) ||
            !F_ISSET(xp->ext, XINVIS)) && !F_ISSET(xp->ext, XNOSEE)){
            found++;
            print(fd, "You found an exit: %s.\n", xp->ext->name);
        }
        xp = xp->next_tag;
    }

    op = rom_ptr->first_obj;
    while(op) {
        if(F_ISSET(op->obj, OHIDDN) && mrand(1,100) <= chance) 
        if(F_ISSET(op->obj, OINVIS) && F_ISSET(ply_ptr,PDINVI) ||
            !F_ISSET(op->obj, OINVIS)){
            found++;
            print(fd, "You found %1i.\n", op->obj);
        }
        op = op->next_tag;
    }

    cp = rom_ptr->first_ply;
    while(cp) {
        if(F_ISSET(cp->crt, PHIDDN) && !F_ISSET(cp->crt, PDMINV) &&
           mrand(1,100) <= chance) 
        if(F_ISSET(cp->crt, PINVIS) && F_ISSET(ply_ptr,PDINVI) ||
            !F_ISSET(cp->crt, PINVIS)){
            found++;
            print(fd, "You found %s hiding.\n", cp->crt->name);
        }
        cp = cp->next_tag;
    }

    cp = rom_ptr->first_mon;
    while(cp) {
        if(F_ISSET(cp->crt, MHIDDN) && mrand(1,100) <= chance) 
        if(F_ISSET(cp->crt, MINVIS) && F_ISSET(ply_ptr,PDINVI) ||
            !F_ISSET(cp->crt, MINVIS)){
            found++;
            print(fd, "You found %1m hiding.\n", cp->crt);
        }
        cp = cp->next_tag;
    }

    broadcast_rom(fd, ply_ptr->rom_num, "%M searches the room.", ply_ptr);

    if(found)
        broadcast_rom(fd, ply_ptr->rom_num, "%s found something!", 
                  F_ISSET(ply_ptr, MMALES) ? "He":"She");
    else
        print(fd, "You didn't find anything.\n");

    return(0);

}

/**********************************************************************/
/*              ply_suicide               */
/**********************************************************************/

/* This function is called whenever the player explicitly asks to     */
/* commit suicide.  It then calls the suicide() function which takes  */
/* over that player's input.                          */

int ply_suicide(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    suicide(ply_ptr->fd, 1, "");
    return(DOPROMPT);
}

/**********************************************************************/
/*              suicide                   */
/**********************************************************************/

/* This function allows a player to kill himself, thus erasing his entire */
/* player file.                               */

void suicide(fd, param, str)
int fd;
int param;
char    *str;
{
    char    file[80];
    long t;
    char str2[50];

    switch(param) {
        case 1:
			ANSI(fd, BOLD);
			ANSI(fd, BLINK);
			ANSI(fd, RED);
            print(fd, "This will completely erase your player.\n");
            print(fd, "Are you sure (Y/N)\n");
		    ANSI(fd, NORMAL);
		    ANSI(fd, WHITE);
            RETURN(fd, suicide, 2);
        case 2:
            if(low(str[0]) == 'y') {
                broadcast("### %s committed suicide! We'll miss %s dearly.", Ply[fd].ply->name, F_ISSET(Ply[fd].ply, PMALES) ? "him":"her");
                sprintf(file, "%s/%s", PLAYERPATH, Ply[fd].ply->name);
				if(SUICIDE) {
					t = time(0);
			        strcpy(str2, (char *)ctime(&t));
					str[strlen(str2)-1] = 0;
					logn("SUICIDE","%s-%d (%s@%s):%s\n", Ply[fd].ply->name, Ply[fd].ply->level, Ply[fd].io->userid, Ply[fd].io->address, str2);
				}
				disconnect(fd);
                unlink(file);
                return;
            }
            else {
                print(fd, "Aborted.\n");
                RETURN(fd, command, 1);
            }
    }
}

/**********************************************************************/
/*              hide                      */
/**********************************************************************/

/* This command allows a player to try and hide himself in the shadows */
/* or it can be used to hide an object in a room.              */

int hide(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    room    *rom_ptr;
    object  *obj_ptr;
    long    i, t;
    int fd, chance;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    i = LT(ply_ptr, LT_HIDES);
    t = time(0);

    if(i > t) {
        please_wait(fd, i-t);
        return(0);
    }

    ply_ptr->lasttime[LT_HIDES].ltime = t;
    ply_ptr->lasttime[LT_HIDES].interval = (ply_ptr->class == THIEF ||
        ply_ptr->class == ASSASSIN || ply_ptr->class == RANGER) ? 5:15;

    if(cmnd->num == 1) {

        if(ply_ptr->class == THIEF || ply_ptr->class == ASSASSIN)
            chance = MIN(90, 5 + 6*ply_ptr->level + 
                3*bonus[ply_ptr->dexterity]);
        else if(ply_ptr->class == RANGER || ply_ptr->class == DRUID)
            chance = 5 + 10*ply_ptr->level +
                3*bonus[ply_ptr->dexterity];
        else
            chance = MIN(90, 5 + 2*ply_ptr->level +
                3*bonus[ply_ptr->dexterity]);

        print(fd, "You attempt to hide in the shadows.\n");

	if(F_ISSET(ply_ptr, PBLIND))
	    chance = MIN(chance, 20);

        if(mrand(1,100) <= chance) {
            F_SET(ply_ptr, PHIDDN);
	    print(fd, "You slip into the shadows unnoticed.\n");
	}
        else {
            F_CLR(ply_ptr, PHIDDN);
            broadcast_rom(fd, ply_ptr->rom_num,
                  "%M tries to hide in the shadows.", ply_ptr);
        }


        return(0);

    }

    obj_ptr = find_obj(ply_ptr, rom_ptr->first_obj,
               cmnd->str[1], cmnd->val[1]);

    if(!obj_ptr) {
        print(fd, "I don't see that here.\n");
        return(0);
    }

    if(F_ISSET(obj_ptr,ONOTAK)){
	print(fd,"You can not hide that.\n");
	return (0);
   }
    if(ply_ptr->class == THIEF || ply_ptr->class == ASSASSIN)
        chance = MIN(90, 10 + 5*ply_ptr->level + 
            5*bonus[ply_ptr->dexterity]);
    else if(ply_ptr->class == RANGER || ply_ptr->class == DRUID)
        chance = 5 + 9*ply_ptr->level +
            3*bonus[ply_ptr->dexterity];
    else
        chance = MIN(90, 5 + 3*ply_ptr->level + 
            3*bonus[ply_ptr->dexterity]);

    print(fd, "You attempt to hide it.\n");
    broadcast_rom(fd, ply_ptr->rom_num, "%M attempts to hide %1i.", 
              ply_ptr, obj_ptr);

    if(mrand(1,100) <= chance) {
        F_SET(obj_ptr, OHIDDN);
	print(fd, "You hide %1i.\n", obj_ptr);
    }
    else
        F_CLR(obj_ptr, OHIDDN);

    return(0);

}

/************************************************************************/
/************************************************************************/

/*  Display information on creature given to player given.		*/

int flag_list(ply_ptr, cmnd)
creature	*ply_ptr;
cmd			*cmnd;
{
	char		str[1024], temp[20];
	int		i, fd;

	str[0] = 0;
	temp[0] = 0;

	fd = ply_ptr->fd;

		print(fd,"Flags currently set:\n");
		if(F_ISSET(ply_ptr, PIGCLA)) strcat(str, "No Class Messages\n");
		if(F_ISSET(ply_ptr, PNOBRD)) strcat(str, "NoBroad\n");
		if(F_ISSET(ply_ptr, PNOLDS)) strcat(str, "No Long Room Description\n");
		if(F_ISSET(ply_ptr, PNOSDS)) strcat(str, "No Short Room Description\n");
		if(F_ISSET(ply_ptr, PNORNM)) strcat(str, "No Name\n");
		if(F_ISSET(ply_ptr, PNOEXT)) strcat(str, "No Exits\n");
		if(F_ISSET(ply_ptr, PLECHO)) strcat(str, "Communications echo\n");
		if(F_ISSET(ply_ptr, PNLOGN)) strcat(str, "No Login messages\n");
		if(F_ISSET(ply_ptr, PNOCMP)) strcat(str, "Non-compact\n");
		if(F_ISSET(ply_ptr, PWIMPY)) {
			sprintf(temp, "Wimpy%d\n", ply_ptr->WIMPYVALUE);
			strcat(str, temp);
		}
		if(F_ISSET(ply_ptr, PPROMP)) strcat(str, "Prompt\n");
		if(F_ISSET(ply_ptr, PANSIC)) strcat(str, "Ansi output mode\n");
		if(F_ISSET(ply_ptr, PIGNOR)) strcat(str, "Ignore all sends\n");
		if(F_ISSET(ply_ptr, PNOSUM)) strcat(str, "Nosummon\n");
        	if(F_ISSET(ply_ptr, PNOAAT)) strcat(str, "No Auto Attack\n"); 
		if(F_ISSET(ply_ptr, PHASTE)) strcat(str, "Haste\n");
		if(F_ISSET(ply_ptr, PPRAYD)) strcat(str, "Pray\n");
		if(F_ISSET(ply_ptr, PPLDGK))
			if(F_ISSET(ply_ptr, PKNGDM)) strcat(str, "Organization 1\n");
			else strcat(str, "Organization 0\n");

		print(fd,"%s\n",str);

        print(fd, "Type help set to see a complete list of flags.\n");
    return(0);
}
/**********************************************************************/
/*              set                   */
/**********************************************************************/

/* This function allows a player to set certain one-bit flags.  The flags */
/* are settings for options that include brief and verbose display.  The  */
/* clear function handles the turning off of these flags.         */

int set(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    int fd;
    
    fd = ply_ptr->fd;

    if(cmnd->num == 1) {
		flag_list(ply_ptr,cmnd);
    	return(0);
    }

    if(!strcmp(cmnd->str[1], "classignore")) {
	F_SET(ply_ptr, PIGCLA);
	print(fd, "Class messages disabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "nobroad")) {
        F_SET(ply_ptr, PNOBRD);
        print(fd, "Broadcast messages disabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "compact")) {
        F_CLR(ply_ptr, PNOCMP);
        print(fd, "Compact mode enabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "roomname")) {
        F_CLR(ply_ptr, PNORNM);
        print(fd, "Room name output enabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "short")) {
        F_CLR(ply_ptr, PNOSDS);
        print(fd, "Short descriptions enabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "long")) {
        F_CLR(ply_ptr, PNOLDS);
        print(fd, "Long descriptions enabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "hexline")) {
        F_SET(ply_ptr, PHEXLN);
        print(fd, "Hexline enabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "nologin")) {
        F_SET(ply_ptr, PNLOGN);
        print(fd, "Login messages disabled.\n");
    } 
    else if(!strcmp(cmnd->str[1], "echo")) {
        F_SET(ply_ptr, PLECHO);
        print(fd, "Communications echo enabled.\n");
    }

    else if(!strcmp(cmnd->str[1], "wimpy")) {
        F_SET(ply_ptr, PWIMPY);
        print(fd, "Wimpy mode enabled.\n");
        ply_ptr->WIMPYVALUE = cmnd->val[1] == 1L ? 10 : cmnd->val[1];
        ply_ptr->WIMPYVALUE = MAX(ply_ptr->WIMPYVALUE, 2);
        print(fd, "Wimpy value set to (%d).\n", ply_ptr->WIMPYVALUE);
    }
    else if(!strcmp(cmnd->str[1], "eavesdropper")) {
        F_SET(ply_ptr, PEAVES);
        print(fd, "Eavesdropper mode enabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "prompt")) {
        F_SET(ply_ptr, PPROMP);
        print(fd, "Prompt in descriptive format.\n");
    }
    else if(!strcmp(cmnd->str[1], "~robot~")) {
        F_SET(ply_ptr, PROBOT);
        print(fd, "Robot mode on.\n");
    }
    else if(!strcmp(cmnd->str[1], "ansi")) {
        F_SET(ply_ptr, PANSIC);
        print(fd, "Color ANSI mode on.\n");
    }
    else if(!strcmp(cmnd->str[1], "nosummon")) {
        F_SET(ply_ptr, PNOSUM);
        print(fd, "Nosummon flag on.\n");
    }
    else if(!strcmp(cmnd->str[1], "ignore")) {
        F_SET(ply_ptr, PIGNOR);
        print(fd, "Ignore all flag on.\n");
    }
    else if(!strcmp(cmnd->str[1], "noauto")) {
        F_SET(ply_ptr, PNOAAT);
        print(fd, "Auto Attack is turned off.\n");
    }
    else
        print(fd, "Unknown flag.\n");

    return(0);

}

/**********************************************************************/
/*              clear                     */
/**********************************************************************/

/* Like set, this function allows a player to alter the value of a part- */
/* icular player flag.                           */

int clear(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    int fd;
    
    fd = ply_ptr->fd;

    if(cmnd->num == 1) {
        print(fd, "Type help clear to see a list of flags.\n");
        return(0);
    }

    if(!strcmp(cmnd->str[1], "classignore")) {
	F_CLR(ply_ptr, PIGCLA);
	print(fd, "Class Messages enabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "nobroad")) {
        F_CLR(ply_ptr, PNOBRD);
        print(fd, "Broadcast messages enabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "compact")) {
        F_SET(ply_ptr, PNOCMP);
        print(fd, "Compact mode disabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "echo")) {
        F_CLR(ply_ptr, PLECHO);
        print(fd, "Communications echo disabled.\n");
    }

    else if(!strcmp(cmnd->str[1], "nologin")) {
        F_CLR(ply_ptr, PNLOGN);
        print(fd, "Login messages enabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "roomname")) {
        F_SET(ply_ptr, PNORNM);
        print(fd, "Room name output disabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "short")) {
        F_SET(ply_ptr, PNOSDS);
        print(fd, "Short descriptions disabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "long")) {
        F_SET(ply_ptr, PNOLDS);
        print(fd, "Long descriptions disabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "hexline")) {
        F_CLR(ply_ptr, PHEXLN);
        print(fd, "Hex line disabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "wimpy")) {
        F_CLR(ply_ptr, PWIMPY);
        print(fd, "Wimpy mode disabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "eavesdropper")) {
        F_CLR(ply_ptr, PEAVES);
        print(fd, "Eavesdropper mode disabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "prompt")) {
        F_CLR(ply_ptr, PPROMP);
        print(fd, "Prompt in brief mode.\n");
    }
    else if(!strcmp(cmnd->str[1], "~robot~")) {
        F_CLR(ply_ptr, PROBOT);
        print(fd, "Robot mode off.\n");
    }
    else if(!strcmp(cmnd->str[1], "ansi")) {
        F_CLR(ply_ptr, PANSIC);
        print(fd, "Color ANSI off.\n");
    }
    else if(!strcmp(cmnd->str[1], "nosummon")) {
        F_CLR(ply_ptr, PNOSUM);
        print(fd, "Nosummon flag off.\n");
    }
    else if(!strcmp(cmnd->str[1], "ignore")) {
        F_CLR(ply_ptr, PIGNOR);
        print(fd, "Ignore all flag off.\n");
    }
    else if(!strcmp(cmnd->str[1], "noauto")) {
        F_CLR(ply_ptr, PNOAAT);
        print(fd, "Auto attack mode enabled.\n");
    }
    else
        print(fd, "Unknown flag.\n");

    return(0);

}

/**********************************************************************/
/*              quit                      */
/**********************************************************************/

/* This function checks to see if a player is allowed to quit yet.  It  */
/* checks to make sure the player isn't in the middle of combat, and if */
/* so, the player is not allowed to quit (and 0 is returned).       */

int quit(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    long    i, t;
    int fd;
    fd = ply_ptr->fd;

    t = time(0);
    i = LT(ply_ptr, LT_ATTCK) + 20;
	
    if(t < i) {
        please_wait(fd, i-t);
        return(0);
    }
    i = LT(ply_ptr, LT_SPELL) + 20;
    
    if(t < i) {
        please_wait(fd, i-t);
        return(0);
    }   

    update_ply(ply_ptr);
    return(DISCONNECT);
}
