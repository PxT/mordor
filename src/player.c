/*
 * PLAYER.C:
 *
 *	Player routines.
 *
 *	Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */
#include <math.h>
#include "mstruct.h"
#include "mextern.h"
#ifdef DMALLOC
  #include "/usr/local/include/dmalloc.h"
#endif

/**********************************************************************/
/*				init_ply			      */
/**********************************************************************/

/* This function initializes a player's stats and loads up the room he */
/* should be in.  This should only be called when the player first     */
/* logs on. 							       */

void init_ply(ply_ptr)
creature	*ply_ptr;
{
	char	file[80], str[50];
	room	*rom_ptr;
	object	*obj_ptr, *obj_ptr2, *cnt_ptr;
	otag	*op, *otemp, *cop;
	long	t, tdiff;
	int	n, wf, i, fd, found=0, cantwear=0;

	F_CLR(ply_ptr, PSPYON);
	F_CLR(ply_ptr, PREADI);
	F_CLR(ply_ptr, PSECOK);

	if(ply_ptr->class == DM && strcmp(ply_ptr->name, DMNAME) &&
	   strcmp(ply_ptr->name, DMNAME2) && strcmp(ply_ptr->name, DMNAME3) &&
	   strcmp(ply_ptr->name, DMNAME4) && strcmp(ply_ptr->name, DMNAME5) &&
	   strcmp(ply_ptr->name, DMNAME6) && strcmp(ply_ptr->name, DMNAME7))
		ply_ptr->class = CARETAKER;

	if(!strcmp(ply_ptr->name, DMNAME) || !strcmp(ply_ptr->name, DMNAME2) ||
	  !strcmp(ply_ptr->name, DMNAME3) || !strcmp(ply_ptr->name, DMNAME4) ||
	  !strcmp(ply_ptr->name, DMNAME5) || !strcmp(ply_ptr->name, DMNAME6) ||
	  !strcmp(ply_ptr->name, DMNAME7))
		ply_ptr->class = DM;

	if(ply_ptr->class < CARETAKER) {
		ply_ptr->daily[DL_BROAD].max = 5 + ply_ptr->level/2;
		ply_ptr->daily[DL_ENCHA].max = 3;
		ply_ptr->daily[DL_FHEAL].max = MAX(3, 3 + (ply_ptr->level-5)/3);
		ply_ptr->daily[DL_TRACK].max = MAX(3, 3 + (ply_ptr->level-5)/4);
		ply_ptr->daily[DL_DEFEC].max = 1;
		ply_ptr->daily[DL_CHARM].max = MAX(5, 3 + ply_ptr->level/4);
	}
	else {
                ply_ptr->daily[DL_BROAD].cur = 20;
                ply_ptr->daily[DL_BROAD].max = 20;
		ply_ptr->daily[DL_DEFEC].max = 1;
		ply_ptr->daily[DL_DEFEC].cur = 1;
        }


	F_SET(ply_ptr, PNOSUM);
	F_CLR(ply_ptr, PALIAS);
	if(ply_ptr->class < CARETAKER)
		F_SET(ply_ptr, PPROMP);

	if(!F_ISSET(ply_ptr, PDMINV))
		broadcast_login("### %s the %s %s just logged in.", ply_ptr->name,
			race_adj[ply_ptr->race], title_ply(ply_ptr));

	t = time(0);
	strcpy(str, (char *)ctime(&t));
	str[strlen(str)-1] = 0;
		loge("%s: %s (%s) logged on.\n", str, ply_ptr->name, 
			Ply[ply_ptr->fd].io->address);

	ply_ptr->lasttime[LT_PSAVE].ltime = t;
	ply_ptr->lasttime[LT_PSAVE].interval = SAVEINTERVAL;
	ply_ptr->lasttime[LT_SECCK].ltime = t;

	if(load_rom(ply_ptr->rom_num, &rom_ptr) < 0)
		load_rom(1, &rom_ptr);

	n = count_vis_ply(rom_ptr);
	if((F_ISSET(rom_ptr, RONEPL) && n > 0) ||
	  (F_ISSET(rom_ptr, RTWOPL) && n > 1) ||
	  (F_ISSET(rom_ptr, RTHREE) && n > 2) ||
      (F_ISSET(rom_ptr, RNOLOG)))
		load_rom(1, &rom_ptr);

	add_ply_rom(ply_ptr, rom_ptr);

	tdiff = t -  ply_ptr->lasttime[LT_HOURS].ltime;
	for(i=0; i<45; i++) {
		ply_ptr->lasttime[i].ltime += tdiff;
		ply_ptr->lasttime[i].ltime =
			MIN(t, ply_ptr->lasttime[i].ltime);
	}
   
    /* delete quest scrolls */
	op = ply_ptr->first_obj;
	while(op) {
		if(op->obj->type == CONTAINER) {
			cnt_ptr = op->obj;
			cop = cnt_ptr->first_obj;
			while(cop) {
				obj_ptr2 = cop->obj;
				if(obj_ptr2->questnum && obj_ptr2->type == SCROLL) {
					cop = cop->next_tag;
					cnt_ptr->shotscur--;
					del_obj_obj(obj_ptr2, cnt_ptr);
					free_obj(obj_ptr2);
				}
				else
					cop = cop->next_tag;
			}


		}
                if(op->obj->questnum && op->obj->type == SCROLL) {
                        obj_ptr = op->obj;
                        op = op->next_tag;
                        del_obj_crt(obj_ptr, ply_ptr);
                        free_obj(obj_ptr);
                        }
                else
                        op = op->next_tag;
        }

	
	/*  Bugged wear code -- no check for broken/non-suth eq */
	/*
	op = ply_ptr->first_obj;
	while(op) {
		otemp = op->next_tag;
		obj_ptr = op->obj;
		if(F_ISSET(obj_ptr, OWEARS)) {
			wf = obj_ptr->wearflag;
			if(wf == FINGER) {
				for(i=FINGER1-1; i<=FINGER8-1; i++)
					if(!ply_ptr->ready[i]) {
						ply_ptr->ready[i] = obj_ptr;
						del_obj_crt(obj_ptr, ply_ptr);
						break;
					}
			}
			else if(wf == NECK) {
				for(i=NECK1-1; i<=NECK2-1; i++)
					if(!ply_ptr->ready[i]) {
						ply_ptr->ready[i] = obj_ptr;
						del_obj_crt(obj_ptr, ply_ptr);
						break;
					}
			}
			else {
				if(!ply_ptr->ready[wf-1]) {
					ply_ptr->ready[wf-1] = obj_ptr;
					del_obj_crt(obj_ptr, ply_ptr);
				}
			}
		}
		op = otemp;
	} */
	/* End of bugged wear code */

	/* Beginning of new wear code */
	op = ply_ptr->first_obj;

	while(op) {

		otemp = op->next_tag;

		if((F_ISSET(ply_ptr, PDINVI) ?
		   1:!F_ISSET(op->obj, OINVIS)) && op->obj->wearflag &&
		   op->obj->wearflag != HELD && op->obj->wearflag != WIELD) {

			obj_ptr = op->obj;

			if(obj_ptr->type == ARMOR && F_ISSET(obj_ptr, ONOMAG) &&
			   ply_ptr->class == MAGE) {
				op = otemp;
				continue;
			}

		if(obj_ptr->type == ARMOR && F_ISSET(obj_ptr, ONOFEM) &&
		   !F_ISSET(ply_ptr,PMALES)) {
			op = otemp;
			continue;
		}

		if(obj_ptr->type == ARMOR && F_ISSET(obj_ptr, ONOMAL) &&
		   F_ISSET(ply_ptr,PMALES)) {
			op = otemp;
			continue;
		}


			if(obj_ptr->wearflag == NECK && 
			   ply_ptr->ready[NECK1-1] && 
			   ply_ptr->ready[NECK2-1]) {
				op = otemp;
				continue;
			}
	
			if(obj_ptr->wearflag == FINGER && 
			   ply_ptr->ready[FINGER1-1] &&
			   ply_ptr->ready[FINGER2-1] && 
			   ply_ptr->ready[FINGER3-1] &&
			   ply_ptr->ready[FINGER4-1] && 
			   ply_ptr->ready[FINGER5-1] && 
			   ply_ptr->ready[FINGER6-1] && 
			   ply_ptr->ready[FINGER7-1] &&
			   ply_ptr->ready[FINGER8-1]) {
				op = otemp;
				continue;
			}

			if(obj_ptr->wearflag != NECK && 
			   obj_ptr->wearflag != FINGER &&
			   ply_ptr->ready[obj_ptr->wearflag-1]) {
				op = otemp;
				continue;
			}

			if(obj_ptr->shotscur < 1) {
				op = otemp;
				continue;
			}


			if (F_ISSET(obj_ptr,OPLDGK) &&
   		     	(BOOL(F_ISSET(obj_ptr,OKNGDM)) != BOOL(F_ISSET(ply_ptr,PKNGDM)))){
				op = otemp;
				continue;
	    	}              

      if(F_ISSET(obj_ptr,OCLSEL))
	if(!F_ISSET(obj_ptr,OCLSEL + ply_ptr->class) && ( ply_ptr->class < CARETAKER)){
				op = otemp;
				continue;
	}

	if(!F_ISSET(obj_ptr,OCLSEL + ply_ptr->class) && (ply_ptr->class== MONK || ply_ptr->class == MAGE) && obj_ptr->armor >5){
                                op = otemp;
                                continue;
        }
        if((obj_ptr->wearflag == FINGER||obj_ptr->wearflag ==SHIELD) && ply_ptr->class == MONK){
                                op=otemp;
                                continue;
        }

			if(F_ISSET(obj_ptr, OGOODO) && 
			   ply_ptr->alignment < -50) {
				op = otemp;
				continue;
			}

			if(F_ISSET(obj_ptr, OEVILO) && 
			   ply_ptr->alignment > 50) {
				op = otemp;
				continue;
			}

			i = (F_ISSET(obj_ptr, OSIZE1) ? 1:0) * 2 +
				(F_ISSET(obj_ptr, OSIZE2) ? 1:0);

			switch(i) {
			case 1:
				if(ply_ptr->race != GNOME &&
				   ply_ptr->race != HOBBIT &&
				   ply_ptr->race != DWARF) cantwear = 1;
				break;
			case 2:
				if(ply_ptr->race != HUMAN &&
				   ply_ptr->race != ELF &&
				   ply_ptr->race != HALFELF &&
				   ply_ptr->race != HALFORC &&
				   ply_ptr->race != DARKELF &&
				   ply_ptr->race != GOBLIN &&
				   ply_ptr->race != ORC) cantwear = 1;
				break;
			case 3:
				if(ply_ptr->race != HALFGIANT && ply_ptr->race != OGRE && ply_ptr->race != TROLL) cantwear = 1;
				break;
			}

			if(cantwear) {
				op = otemp;
				cantwear = 0;
				continue;
			}

			if(obj_ptr->wearflag == NECK) {
				if(ply_ptr->ready[NECK1-1])
					ply_ptr->ready[NECK2-1] = obj_ptr;
				else
					ply_ptr->ready[NECK1-1] = obj_ptr;
				F_SET(obj_ptr, OWEARS);
			}

			else if(obj_ptr->wearflag == FINGER && ply_ptr->class != MONK) {
				for(i=FINGER1; i<FINGER8+1; i++) {
					if(!ply_ptr->ready[i-1]) {
						ply_ptr->ready[i-1] = obj_ptr;
						F_SET(obj_ptr, OWEARS);
						break;
					}
				}
			}

			else {
				ply_ptr->ready[obj_ptr->wearflag-1] = obj_ptr;
				F_SET(obj_ptr, OWEARS);
			}

			del_obj_crt(obj_ptr, ply_ptr);
			found = 1;

		}
		op = otemp;
	}


	/* End of new wear code */
	
	compute_ac(ply_ptr);
	compute_thaco(ply_ptr);
	luck(ply_ptr);
	update_ply(ply_ptr);

	sprintf(file, "%s/dialin", DOCPATH);

	/* Just so we know who to mail */
	/* Remove it if you are that paranoid. */
/*
	if (!strcmp(Ply[ply_ptr->fd].io->address, "128.200.21.101")|| !strcmp(Ply[ply_ptr->fd].io->address, "moria.bio.uci.edu"))
		print(ply_ptr->fd, "DMs here are: %M, %M, %M, %M, %M.\n", DMNAME1, DNAME2, DMNAME3, DMNAME4 DMAME5);
*/	

	if (ply_ptr->class < DM) {
	sprintf(file, "%s/news", LOGPATH);
	view_file(ply_ptr->fd, 1, file);
	}
	sprintf(str, "%s/%s", POSTPATH, ply_ptr->name);
	if(file_exists(str))
		print(ply_ptr->fd, "\n*** You have mail in the post office.\n");

}

/**********************************************************************/
/*				uninit_ply			      */
/**********************************************************************/

/* This function de-initializes a player who has left the game.  This  */
/* is called whenever a player quits or disconnects, right before save */
/* is called.							       */

void uninit_ply(ply_ptr)
creature	*ply_ptr;
{
	creature	*crt_ptr;
	ctag		*cp, *prev;
	int		i;


	if(ply_ptr->parent_rom)
		del_ply_rom(ply_ptr, ply_ptr->parent_rom);

	cp = ply_ptr->first_fol;
	while(cp) {
		cp->crt->following = 0;
		if(cp->crt->type == PLAYER)
			print(cp->crt->fd, "You stop following %s.\n", ply_ptr->name);
		prev = cp->next_tag;
		free(cp);
		cp = prev;
	}
	ply_ptr->first_fol = 0;
		
	if(ply_ptr->following) {
		crt_ptr = ply_ptr->following;
		cp = crt_ptr->first_fol;
		if(cp->crt == ply_ptr) {
			crt_ptr->first_fol = cp->next_tag;
			free(cp);
		}
		else while(cp) {
			if(cp->crt == ply_ptr) {
				prev->next_tag = cp->next_tag;
				free(cp);
				break;
			}
			prev = cp;
			cp = cp->next_tag;
		}
		ply_ptr->following = 0;

		if(ply_ptr->class < CARETAKER)
			print(crt_ptr->fd, "%s stops following you.\n", 
			      ply_ptr->name);
	}

	for(i=0; i<MAXWEAR; i++)
		if(ply_ptr->ready[i]) {
			add_obj_crt(ply_ptr->ready[i], ply_ptr);
			ply_ptr->ready[i] = 0;
		}

	update_ply(ply_ptr);
	if(!F_ISSET(ply_ptr, PDMINV))
		broadcast_login("### %s just logged off.", ply_ptr->name);
}

/**********************************************************************/
/*				update_ply			      */
/**********************************************************************/

/* This function checks up on all a player's time-expiring flags to see */
/* if some of them have expired.  If so, flags are set accordingly.     */

void update_ply(ply_ptr)
creature	*ply_ptr;
{
	long 	t;
	int	item;
	char 	ill, prot = 1; 	/*character is prot in a pharm room */

	t = time(0);
	ply_ptr->lasttime[LT_HOURS].interval +=
		(t - ply_ptr->lasttime[LT_HOURS].ltime);
	ply_ptr->lasttime[LT_HOURS].ltime = t;

	if(F_ISSET(ply_ptr, PHASTE)) {
		if(t > LT(ply_ptr, LT_HASTE)) {
			ANSI(ply_ptr->fd, GREEN);
			print(ply_ptr->fd, "You feel slower.\n");
			ANSI(ply_ptr->fd, WHITE);
			F_CLR(ply_ptr, PHASTE);
			ply_ptr->dexterity -= 5;
		}
	}
	if(F_ISSET(ply_ptr, PPRAYD)) {
		if(t > LT(ply_ptr, LT_PRAYD)) {
			ANSI(ply_ptr->fd, YELLOW);
			print(ply_ptr->fd, "You feel less pious.\n");
			F_CLR(ply_ptr, PPRAYD);
			ANSI(ply_ptr->fd, WHITE);
			ply_ptr->piety -= 5;
		}
	}
	if(F_ISSET(ply_ptr, PINVIS)) {
		if(t > LT(ply_ptr, LT_INVIS) && ply_ptr->class < DM) {
			ANSI(ply_ptr->fd, MAGENTA);
			print(ply_ptr->fd, "Your invisibility wears off.\n");
			ANSI(ply_ptr->fd, WHITE);
			F_CLR(ply_ptr, PINVIS);
		}
	}
	if(F_ISSET(ply_ptr, PDINVI)) {
		if(t > LT(ply_ptr, LT_DINVI)  && ply_ptr->class < DM) {
			ANSI(ply_ptr->fd, MAGENTA);
			print(ply_ptr->fd, "Your detect-invis wears off.\n");
			ANSI(ply_ptr->fd, WHITE);
			F_CLR(ply_ptr, PDINVI);
		}
	}
	if(F_ISSET(ply_ptr, PDMAGI)) {
		if(t > LT(ply_ptr, LT_DMAGI) && ply_ptr->class < DM) {
			ANSI(ply_ptr->fd, MAGENTA);
			print(ply_ptr->fd, "Your detect-magic wears off.\n");
			ANSI(ply_ptr->fd, WHITE);
			F_CLR(ply_ptr, PDMAGI);
		}
	}
	if(F_ISSET(ply_ptr, PHIDDN)) {
		if(t-ply_ptr->lasttime[LT_HIDES].ltime > 300L)
			F_CLR(ply_ptr, PHIDDN);
	}
	if(F_ISSET(ply_ptr, PPROTE)) {
		if(t > LT(ply_ptr, LT_PROTE)) {
			ANSI(ply_ptr->fd, YELLOW);
			print(ply_ptr->fd, "You feel less protected.\n");
			ANSI(ply_ptr->fd, WHITE);
			F_CLR(ply_ptr, PPROTE);
			compute_ac(ply_ptr);
		}
	}
	if(F_ISSET(ply_ptr, PLEVIT)) {
		if(t > LT(ply_ptr, LT_LEVIT) && ply_ptr->class < DM) {
			ANSI(ply_ptr->fd, MAGENTA);
			print(ply_ptr->fd, "Your feet hit the ground.\n");
			ANSI(ply_ptr->fd, WHITE);
			F_CLR(ply_ptr, PLEVIT);
		}
	}
	if(F_ISSET(ply_ptr, PBLESS)) {
		if(t > LT(ply_ptr, LT_BLESS)) {
			ANSI(ply_ptr->fd, YELLOW);
			print(ply_ptr->fd, "You feel less holy.\n");
			ANSI(ply_ptr->fd, WHITE);
			F_CLR(ply_ptr, PBLESS);
			compute_thaco(ply_ptr);
		}
	}

	if(F_ISSET(ply_ptr, PRFIRE)) {
		if(t > LT(ply_ptr, LT_RFIRE)) {
			ANSI(ply_ptr->fd, YELLOW);
			print(ply_ptr->fd, "Your skin returns to normal.\n");
			ANSI(ply_ptr->fd, WHITE);
			F_CLR(ply_ptr, PRFIRE);
		}
	}


	if(F_ISSET(ply_ptr, PRCOLD)) {
		if(t > LT(ply_ptr, LT_RCOLD)) {
			ANSI(ply_ptr->fd, BOLD);
			ANSI(ply_ptr->fd, YELLOW);
			print(ply_ptr->fd, "A cold chill runs through your body.\n");
			ANSI(ply_ptr->fd, NORMAL);
			ANSI(ply_ptr->fd, WHITE);
			F_CLR(ply_ptr, PRCOLD);
		}
	}


	if(F_ISSET(ply_ptr, PBRWAT)) {
		if(t > LT(ply_ptr, LT_BRWAT)) {
			ANSI(ply_ptr->fd, BOLD);
			ANSI(ply_ptr->fd, BLUE);
			print(ply_ptr->fd, "Your lungs contract in size.\n");
			ANSI(ply_ptr->fd, WHITE);
			ANSI(ply_ptr->fd, NORMAL);
			F_CLR(ply_ptr, PBRWAT);
		}
	}

	if(F_ISSET(ply_ptr, PSSHLD)) {
		if(t > LT(ply_ptr, LT_SSHLD)) {
			ANSI(ply_ptr->fd, BOLD);
			ANSI(ply_ptr->fd, GREEN);
			print(ply_ptr->fd, "Your skin softens.\n");
			ANSI(ply_ptr->fd, WHITE);
			ANSI(ply_ptr->fd, NORMAL);
			F_CLR(ply_ptr, PSSHLD);
		}
	}
	if(F_ISSET(ply_ptr, PFLYSP)) {
		if(t > LT(ply_ptr, LT_FLYSP)  && ply_ptr->class < DM) {
			ANSI(ply_ptr->fd, YELLOW);
			print(ply_ptr->fd, "You can no longer fly.\n");
			ANSI(ply_ptr->fd, WHITE);
			F_CLR(ply_ptr, PFLYSP);
		}
	}
	if(F_ISSET(ply_ptr, PRMAGI)) {
		if(t > LT(ply_ptr, LT_RMAGI)) {
			ANSI(ply_ptr->fd, BOLD);
			ANSI(ply_ptr->fd, MAGENTA);
			print(ply_ptr->fd,
				"Your magical shield dissipates.\n");
			ANSI(ply_ptr->fd, WHITE);
			ANSI(ply_ptr->fd, NORMAL);
			F_CLR(ply_ptr, PRMAGI);
		}
	}
	if(F_ISSET(ply_ptr, PSILNC)) {
		if(t > LT(ply_ptr, LT_SILNC)) {
			ANSI(ply_ptr->fd, GREEN);
			print(ply_ptr->fd,
				"Your voice returns!\n");
			ANSI(ply_ptr->fd, WHITE);
			F_CLR(ply_ptr, PSILNC);
		}
	}
	if(F_ISSET(ply_ptr, PFEARS)) {
		if(t > LT(ply_ptr, LT_FEARS)) {
			ANSI(ply_ptr->fd, YELLOW);
			print(ply_ptr->fd,
				"You feel your courage return.\n");
			ANSI(ply_ptr->fd, WHITE);
			F_CLR(ply_ptr, PFEARS);
		}
	}
	if(F_ISSET(ply_ptr, PKNOWA)) {
		if(t > LT(ply_ptr, LT_KNOWA) && ply_ptr->class < DM) {
			ANSI(ply_ptr->fd, CYAN);
			print(ply_ptr->fd,
				"Your perception is diminshed.\n");
			ANSI(ply_ptr->fd, WHITE);
			F_CLR(ply_ptr, PKNOWA);
		}
	}
	if(F_ISSET(ply_ptr, PLIGHT)) {
		if(t > LT(ply_ptr, LT_LIGHT)  && ply_ptr->class < DM) {
			ANSI(ply_ptr->fd, YELLOW);
			print(ply_ptr->fd, "Your magical light fades.\n");
			broadcast_rom(ply_ptr->fd, ply_ptr->rom_num,
				      "%M's magical light fades.", ply_ptr);
			ANSI(ply_ptr->fd, WHITE);
			F_CLR(ply_ptr, PLIGHT);
		}
	}
	if(t > LT(ply_ptr, LT_CHRMD) && F_ISSET(ply_ptr, PCHARM)) {
                        ANSI(ply_ptr->fd, YELLOW);
                        print(ply_ptr->fd, "Your demeanor returns to normal.\n");
                        F_CLR(ply_ptr, PCHARM);
			ANSI(ply_ptr->fd, WHITE);
                }

/* check if player is suffering from any  aliment */
if(F_ISSET(ply_ptr, PPOISN) || F_ISSET(ply_ptr, PDISEA))
	ill = 1;
else
	ill =0;

/* handle normal healing for non posioned or harm room */
if(ply_ptr->parent_rom && (t > LT(ply_ptr, LT_HEALS)))
	if(!F_ISSET(ply_ptr->parent_rom, RPHARM) && (!ill)) {
		ply_ptr->hpcur += MAX(1, 3 + bonus[ply_ptr->constitution] +
				(ply_ptr->class == BARBARIAN ? 2:0));
		ply_ptr->mpcur += MAX(1, 2+(ply_ptr->intelligence > 17 ? 1:0)+
				(ply_ptr->class == MAGE ? 2:0));
		ply_ptr->lasttime[LT_HEALS].ltime = t;
		ply_ptr->lasttime[LT_HEALS].interval = 45 - 
						       5*bonus[ply_ptr->piety];
/* handle quick healing */
			if(F_ISSET(ply_ptr->parent_rom, RHEALR)) {
				ply_ptr->hpcur += 3;
				ply_ptr->mpcur += 2;
				ply_ptr->lasttime[LT_HEALS].interval /= 2;
			}

		ply_ptr->hpcur = MIN(ply_ptr->hpcur, ply_ptr->hpmax);
		ply_ptr->mpcur = MIN(ply_ptr->mpcur, ply_ptr->mpmax);
	}
/* handle poison  */

	else if(!F_ISSET(ply_ptr->parent_rom, RPHARM) && ill) {
	   if(F_ISSET(ply_ptr, PPOISN)){
		ANSI(ply_ptr->fd, BLINK);
		ANSI(ply_ptr->fd, RED);
		print(ply_ptr->fd, "Poison courses through your veins.\n");
		ply_ptr->hpcur -= MAX(1,mrand(1,4)-bonus[ply_ptr->constitution]);
		ply_ptr->lasttime[LT_HEALS].ltime = t;
		ply_ptr->lasttime[LT_HEALS].interval = 65 + 
				    5*bonus[ply_ptr->constitution];
		ANSI(ply_ptr->fd, WHITE);
		ANSI(ply_ptr->fd, NORMAL);
		if(ply_ptr->hpcur < 1)
			die(ply_ptr, ply_ptr);
	   }
	   if (F_ISSET(ply_ptr, PDISEA)){
		ANSI(ply_ptr->fd, BLINK);
		ANSI(ply_ptr->fd, RED);
		print(ply_ptr->fd, "Fever grips your mind.\n");
        	ply_ptr->lasttime[LT_ATTCK].ltime = time(0);
		ANSI(ply_ptr->fd, BLUE);
		print(ply_ptr->fd, "You feel nauseous.\n");
        	ply_ptr->lasttime[LT_ATTCK].interval= dice(1,6,3);
		ply_ptr->hpcur -= MAX(1,mrand(1,6)-bonus[ply_ptr->constitution]);
		ply_ptr->lasttime[LT_HEALS].ltime = t;
		ply_ptr->lasttime[LT_HEALS].interval = 65 + 
				    5*bonus[ply_ptr->constitution];
		ANSI(ply_ptr->fd, WHITE);
		ANSI(ply_ptr->fd, NORMAL);
		if(ply_ptr->hpcur < 1)
			die(ply_ptr, ply_ptr);
	   }
	}


/* handle player harm rooms */
else {

if (F_ISSET(ply_ptr->parent_rom, RPPOIS))
{
  ANSI(ply_ptr->fd, BLINK);
  ANSI(ply_ptr->fd, GREEN);
  print(ply_ptr->fd, "The toxic air poisoned you.\n");
  ANSI(ply_ptr->fd, WHITE);
  ANSI(ply_ptr->fd, NORMAL);
  F_SET(ply_ptr, PPOISN);    
}

if (F_ISSET(ply_ptr->parent_rom, RPBEFU)) 
{
	print(ply_ptr->fd, "The room starts to spin around you.\nYou feel confused.\n");
        ply_ptr->lasttime[LT_ATTCK].ltime = time(0);
        ply_ptr->lasttime[LT_ATTCK].interval= MAX(dice(2,6,0),6);
}

if (F_ISSET(ply_ptr, PPOISN)){
	ANSI(ply_ptr->fd, BLINK);
	ANSI(ply_ptr->fd, RED);
  	print(ply_ptr->fd, "Poison courses through your veins.\n");
	ANSI(ply_ptr->fd, NORMAL);
	ANSI(ply_ptr->fd, WHITE);
	ply_ptr->hpcur -= MAX(1,mrand(1,6)-bonus[ply_ptr->constitution]);
  	if(ply_ptr->hpcur < 1) die(ply_ptr, ply_ptr);
}
if (F_ISSET(ply_ptr, PDISEA)){
			ANSI(ply_ptr->fd, BLINK);
			ANSI(ply_ptr->fd, RED);
        print(ply_ptr->fd, "Fever grips your mind.\n");
        ply_ptr->lasttime[LT_ATTCK].ltime = time(0);
			ANSI(ply_ptr->fd, BLUE);
        print(ply_ptr->fd, "You feel nauseous.\n");
        ply_ptr->lasttime[LT_ATTCK].interval= dice(1,6,3);
        ply_ptr->hpcur -= MAX(1,mrand(1,12)-bonus[ply_ptr->constitution]);
        ply_ptr->lasttime[LT_HEALS].ltime = t;
        ply_ptr->lasttime[LT_HEALS].interval = 65 + 
                            5*bonus[ply_ptr->constitution];
			ANSI(ply_ptr->fd, NORMAL);
			ANSI(ply_ptr->fd, WHITE);
        if(ply_ptr->hpcur < 1)
                die(ply_ptr, ply_ptr);
}                                   

if (F_ISSET(ply_ptr->parent_rom,RPMPDR))
 	ply_ptr->mpcur -= MIN(ply_ptr->mpcur,6); 
else if (!ill) {
  ply_ptr->mpcur += MAX(1, 2+(ply_ptr->intelligence > 17 ? 1:0)+
                                (ply_ptr->class == MAGE ? 2:0));
		ply_ptr->mpcur = MIN(ply_ptr->mpcur, ply_ptr->mpmax);
}
    
if (F_ISSET(ply_ptr->parent_rom, RFIRER) && !F_ISSET(ply_ptr,PRFIRE)) {
    	print(ply_ptr->fd, "The searing heat burns your flesh.\n");
	prot = 0;
}
else if (F_ISSET(ply_ptr->parent_rom, RWATER) && !F_ISSET(ply_ptr,PBRWAT)) {
	print(ply_ptr->fd, "Water fills your lungs.\n");
        prot = 0;
}
else if (F_ISSET(ply_ptr->parent_rom, REARTH) && !F_ISSET(ply_ptr,PSSHLD)) {
	print(ply_ptr->fd, "The earth swells up around you and smothers you.\n");
        prot = 0;
}
else if (F_ISSET(ply_ptr->parent_rom, RWINDR) && !F_ISSET(ply_ptr,PRCOLD)) {
			ANSI(ply_ptr->fd, BLUE);
	print(ply_ptr->fd, "The freezing air chills you to the bone.\n");
			ANSI(ply_ptr->fd, WHITE);
	prot = 0;
}
else  if(!F_ISSET(ply_ptr->parent_rom, RWINDR) &&
	 !F_ISSET(ply_ptr->parent_rom, REARTH) &&
	 !F_ISSET(ply_ptr->parent_rom, RFIRER) &&
	 !F_ISSET(ply_ptr->parent_rom, RWATER) &&
	 !F_ISSET(ply_ptr->parent_rom, RPPOIS) &&
	 !F_ISSET(ply_ptr->parent_rom, RPBEFU) &&
	 !F_ISSET(ply_ptr->parent_rom, RPMPDR)) {
			ANSI(ply_ptr->fd, BOLD);
			ANSI(ply_ptr->fd, MAGENTA);
	print(ply_ptr->fd, "An invisible force saps your life force.\n");
			ANSI(ply_ptr->fd, NORMAL);
			ANSI(ply_ptr->fd, WHITE);
	prot = 0;
}
	if (!prot) {
		ply_ptr->hpcur -= 15 - MIN(bonus[ply_ptr->constitution],2);
        	if(ply_ptr->hpcur < 1) die(ply_ptr, ply_ptr);      
	}
	else if (!ill)
	{
		ply_ptr->hpcur += MAX(1, 3 + bonus[ply_ptr->constitution] +
				(ply_ptr->class == BARBARIAN ? 2:0));
		ply_ptr->hpcur = MIN(ply_ptr->hpcur, ply_ptr->hpmax);
	}

ply_ptr->lasttime[LT_HEALS].ltime = t;
ply_ptr->lasttime[LT_HEALS].interval = 45 +  5*bonus[ply_ptr->piety];

}
/*******************************/


	if(t > LT(ply_ptr, LT_PSAVE)) {
		ply_ptr->lasttime[LT_PSAVE].ltime = t;
		savegame(ply_ptr, 0);
	}

	item = has_light(ply_ptr);
	if(item && item != MAXWEAR+1) {
		if(ply_ptr->ready[item-1]->type == LIGHTSOURCE)
			if(--(ply_ptr->ready[item-1]->shotscur) < 1) {
				print(ply_ptr->fd, "Your %s died out.\n",
				      ply_ptr->ready[item-1]->name);
				broadcast_rom(ply_ptr->fd, ply_ptr->rom_num,
					      "%M's %s died out.", 
					      ply_ptr,
					      ply_ptr->ready[item-1]->name);
			}
	}

}

/**********************************************************************/
/*				up_level			      */
/**********************************************************************/

/* This function should be called whenever a player goes up a level.  */
/* It raises her hit points and magic points appropriately, and if    */
/* it is initializing a new character, it sets up the character.      */

void up_level(ply_ptr)
creature	*ply_ptr;
{
	int	index;

	ply_ptr->level++;
	ply_ptr->hpmax += class_stats[ply_ptr->class].hp;
	ply_ptr->mpmax += class_stats[ply_ptr->class].mp;
	if(ply_ptr->level == 1) {
		ply_ptr->hpmax = class_stats[ply_ptr->class].hpstart;
		ply_ptr->mpmax = class_stats[ply_ptr->class].mpstart;
		ply_ptr->hpcur = ply_ptr->hpmax;
		ply_ptr->mpcur = ply_ptr->mpmax;
		ply_ptr->ndice = class_stats[ply_ptr->class].ndice;
		ply_ptr->sdice = class_stats[ply_ptr->class].sdice;
		ply_ptr->pdice = class_stats[ply_ptr->class].pdice;
	}
	else {
		index = (ply_ptr->level-2) % 10;
		switch(level_cycle[ply_ptr->class][index]) {
		case STR: ply_ptr->strength++; break;
		case DEX: ply_ptr->dexterity++; break;
		case CON: ply_ptr->constitution++; break;
		case INT: ply_ptr->intelligence++; break;
		case PTY: ply_ptr->piety++; break;
		}
	/* MONK CODE */
	/* Yeah, I know this is ugly */
	/* Feel free to send a patch */
  	    if(ply_ptr->class == MONK) {
                switch(ply_ptr->level) {
                case 1:
                ply_ptr->ndice = class_stats[ply_ptr->class].ndice;
                ply_ptr->sdice = class_stats[ply_ptr->class].sdice;
                ply_ptr->pdice = class_stats[ply_ptr->class].pdice;
                break;
                case 2:
                ply_ptr->ndice = 1;
                ply_ptr->sdice = 5;
		ply_ptr->pdice = 0;
                break;
                case 3:
                ply_ptr->ndice = 1;
                ply_ptr->sdice = 5;
                ply_ptr->pdice = 1;
                break;
                case 4:
                ply_ptr->ndice = 1;
                ply_ptr->sdice = 6;
		ply_ptr->pdice = 0; 
                break;
                case 5:
                ply_ptr->ndice = 1;
                ply_ptr->sdice = 6;
                ply_ptr->pdice = 1;
                break;
                case 6:
                ply_ptr->ndice = 2;
                ply_ptr->sdice = 3;
                ply_ptr->pdice = 1;
                break;
                case 7:
                ply_ptr->ndice = 2;
                ply_ptr->sdice = 3;
                ply_ptr->pdice = 2;
                break;
                case 8:
                ply_ptr->ndice = 2;
                ply_ptr->sdice = 4;
                ply_ptr->pdice = 0;
                break;
                case 9:
                ply_ptr->ndice = 2;
                ply_ptr->sdice = 4;
                ply_ptr->pdice = 1;
                break;
                case 10:
		ply_ptr->ndice = 2;
                ply_ptr->sdice = 5;
                ply_ptr->pdice = 0;
                break;
                case 11:
                ply_ptr->ndice = 2;
                ply_ptr->sdice = 5;
                ply_ptr->pdice = 2;
                break;
                case 12:
                ply_ptr->ndice = 2;
                ply_ptr->sdice = 6;
                ply_ptr->pdice = 1;
                break;
                case 13:
                ply_ptr->ndice = 2;
                ply_ptr->sdice = 6;
                ply_ptr->pdice = 2;
                break;
                case 14:
                ply_ptr->ndice = 3;
                ply_ptr->sdice = 6;
                ply_ptr->pdice = 1;
                break;
                case 15:
                ply_ptr->ndice = 3;
                ply_ptr->sdice = 7;
                ply_ptr->pdice = 1;
                break;
                case 16:
                ply_ptr->ndice = 4;
                ply_ptr->sdice = 7;
                ply_ptr->pdice = 1;
                break;
                case 17:
                ply_ptr->ndice = 5;
                ply_ptr->sdice = 7;
                ply_ptr->pdice = 0;
                break;
                case 18:
                ply_ptr->ndice = 5;
                ply_ptr->sdice = 8;
                ply_ptr->pdice = 1;
                break;
                case 19:
		ply_ptr->ndice = 6;
                ply_ptr->sdice = 7;
                ply_ptr->pdice = 0;
                break;
                case 20:
                ply_ptr->ndice = 6;
                ply_ptr->sdice = 7;
                ply_ptr->pdice = 2;
                break;
		case 21:
		ply_ptr->ndice = 6;
                ply_ptr->sdice = 8;
                ply_ptr->pdice = 0;
                case 22:
		ply_ptr->ndice = 6;
                ply_ptr->sdice = 8;
                ply_ptr->pdice = 2;
		case 23:
		ply_ptr->ndice = 6;
                ply_ptr->sdice = 9;
                ply_ptr->pdice = 0;
		case 24:
		ply_ptr->ndice = 6;
                ply_ptr->sdice = 8;
                ply_ptr->pdice = 2;
		case 25:
		ply_ptr->ndice = 6;
                ply_ptr->sdice = 10;
                ply_ptr->pdice = 0;
		default:
                ply_ptr->ndice = 1;
                ply_ptr->sdice = 3;
                ply_ptr->pdice = 0;
                break;
		}
	    }	
	/* END MONK */
	}

}

/**********************************************************************/
/*				down_level			      */
/**********************************************************************/

/* This function is called when a player loses a level due to dying or */
/* for some other reason.  The appropriate stats are downgraded.       */

void down_level(ply_ptr)
creature	*ply_ptr;
{
	int	index;

	ply_ptr->level--;
	ply_ptr->hpmax -= class_stats[ply_ptr->class].hp;
	ply_ptr->mpmax -= class_stats[ply_ptr->class].mp;
	ply_ptr->hpcur = ply_ptr->hpmax;
	ply_ptr->mpcur = ply_ptr->mpmax;
	index = (ply_ptr->level-1) % 10;
	switch(level_cycle[ply_ptr->class][index]) {
	case STR: ply_ptr->strength--; break;
	case DEX: ply_ptr->dexterity--; break;
	case CON: ply_ptr->constitution--; break;
	case INT: ply_ptr->intelligence--; break;
	case PTY: ply_ptr->piety--; break;
	}

	/* MONK CODE */
	/* Yeah, I know this is ugly */
	
  	    if(ply_ptr->class == MONK) {
                switch(ply_ptr->level) {
                case 1:
                ply_ptr->ndice = class_stats[ply_ptr->class].ndice;
                ply_ptr->sdice = class_stats[ply_ptr->class].sdice;
                ply_ptr->pdice = class_stats[ply_ptr->class].pdice;
                break;
                case 2:
                ply_ptr->ndice = 1;
                ply_ptr->sdice = 6;
		ply_ptr->pdice = 0;
                break;
                case 3:
                ply_ptr->ndice = 1;
                ply_ptr->sdice = 7;
                ply_ptr->pdice = 0;
                break;
                case 4:
                ply_ptr->ndice = 2;
                ply_ptr->sdice = 3;
		ply_ptr->pdice = 1; 
                break;
                case 5:
                ply_ptr->ndice = 2;
                ply_ptr->sdice = 3;
                ply_ptr->pdice = 2;
                break;
                case 6:
                ply_ptr->ndice = 2;
                ply_ptr->sdice = 4;
                ply_ptr->pdice = 2;
                break;
                case 7:
                ply_ptr->ndice = 2;
                ply_ptr->sdice = 5;
                ply_ptr->pdice = 1;
                break;
                case 8:
                ply_ptr->ndice = 2;
                ply_ptr->sdice = 6;
                ply_ptr->pdice = 1;
                break;
                case 9:
                ply_ptr->ndice = 2;
                ply_ptr->sdice = 7;
                ply_ptr->pdice = 1;
                break;
                case 10:
		ply_ptr->ndice = 3;
                ply_ptr->sdice = 5;
                ply_ptr->pdice = 1;
                break;
                case 11:
                ply_ptr->ndice = 4;
                ply_ptr->sdice = 5;
                ply_ptr->pdice = 0;
                break;
                case 12:
                ply_ptr->ndice = 4;
                ply_ptr->sdice = 5;
                ply_ptr->pdice = 1;
                break;
                case 13:
                ply_ptr->ndice = 4;
                ply_ptr->sdice = 6;
                ply_ptr->pdice = 0;
                break;
                case 14:
                ply_ptr->ndice = 4;
                ply_ptr->sdice = 6;
                ply_ptr->pdice = 2;
                break;
                case 15:
                ply_ptr->ndice = 3;
                ply_ptr->sdice = 8;
                ply_ptr->pdice = 2;
                break;
                case 16:
                ply_ptr->ndice = 4;
                ply_ptr->sdice = 7;
                ply_ptr->pdice = 2;
                break;
                case 17:
                ply_ptr->ndice = 5;
                ply_ptr->sdice = 7;
                ply_ptr->pdice = 0;
                break;
                case 18:
                ply_ptr->ndice = 5;
                ply_ptr->sdice = 8;
                ply_ptr->pdice = 2;
                break;
                case 19:
		ply_ptr->ndice = 6;
                ply_ptr->sdice = 7;
                ply_ptr->pdice = 0;
                break;
                case 20:
                ply_ptr->ndice = 6;
                ply_ptr->sdice = 8;
                ply_ptr->pdice = 2;
                break;
		case 21:
		ply_ptr->ndice = 6;
                ply_ptr->sdice = 8;
                ply_ptr->pdice = 3;
		case 22:
		ply_ptr->ndice = 6;
                ply_ptr->sdice = 9;
                ply_ptr->pdice = 0;
		case 23:
		ply_ptr->ndice = 6;
                ply_ptr->sdice = 9;
                ply_ptr->pdice = 1;
		case 24:
		ply_ptr->ndice = 6;
                ply_ptr->sdice = 9;
                ply_ptr->pdice = 2;
		case 25:
		ply_ptr->ndice = 6;
                ply_ptr->sdice = 9;
                ply_ptr->pdice = 3;
                default:
                ply_ptr->ndice = 1;
                ply_ptr->sdice = 3;
                ply_ptr->pdice = 0;
                break;
		}
	    }	
	/* END MONK */

}

/**********************************************************************/
/*				add_obj_crt			      */
/**********************************************************************/

/* This function adds the object pointer to by the first parameter to */
/* the inventory of the player pointed to by the second parameter.    */

void add_obj_crt(obj_ptr, ply_ptr)
object		*obj_ptr;
creature	*ply_ptr;
{
	otag	*op, *temp, *prev;

	obj_ptr->parent_crt = ply_ptr;
	obj_ptr->parent_obj = 0;
	obj_ptr->parent_rom = 0;

	op = (otag *)malloc(sizeof(otag));
	if(!op)
		merror("add_obj_crt", FATAL);
	op->obj = obj_ptr;
	op->next_tag = 0;

	if(!ply_ptr->first_obj) {
		ply_ptr->first_obj = op;
		return;
	}

	temp = ply_ptr->first_obj;
	if(strcmp(temp->obj->name, obj_ptr->name) > 0 ||
	   (!strcmp(temp->obj->name, obj_ptr->name) &&
	   temp->obj->adjustment > obj_ptr->adjustment)) {
		op->next_tag = temp;
		ply_ptr->first_obj = op;
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
/*				del_obj_crt			      */
/**********************************************************************/

/* This function removes the object pointer to by the first parameter */
/* from the player pointed to by the second.			      */

void del_obj_crt(obj_ptr, ply_ptr)
object		*obj_ptr;
creature	*ply_ptr;
{
	otag 	*temp, *prev;

	if(!obj_ptr->parent_crt) {
		ply_ptr->ready[obj_ptr->wearflag-1] = 0;
		return;
	}

	obj_ptr->parent_crt = 0;
	if(ply_ptr->first_obj->obj == obj_ptr) {
		temp = ply_ptr->first_obj->next_tag;
		free(ply_ptr->first_obj);
		ply_ptr->first_obj = temp;
		return;
	}

	prev = ply_ptr->first_obj;
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
/*				title_ply			      */
/**********************************************************************/

/* This function returns a string with the player's character title in */
/* it.  The title is determined by looking at the player's class and   */
/* level.							       */

char *title_ply(ply_ptr)
creature	*ply_ptr;
{
	int	titlnum;

	titlnum = (ply_ptr->level-1)/3;
	if(titlnum > 7)
		titlnum = 7;
	return(lev_title[ply_ptr->class][titlnum]);
}

/**********************************************************************/
/*				compute_ac			      */
/**********************************************************************/

/* This function computes a player's (or a monster's) armor class by  */
/* examining its stats and the items it is holding.		      */

void compute_ac(ply_ptr)
creature	*ply_ptr;
{
	int	ac, i;

	ac = 100;

	ac -= 5*bonus[ply_ptr->dexterity];

	for(i=0; i<MAXWEAR; i++)
		if(ply_ptr->ready[i])
			ac -= ply_ptr->ready[i]->armor;

	if(ply_ptr->class == MONK)
               ac = 100-ply_ptr->level*7;

	if(F_ISSET(ply_ptr, PPROTE))
		ac -= 10;

	ac = MAX(-127,MIN(127,ac));

	ply_ptr->armor = ac;
}

/**********************************************************************/
/*				compute_thaco			      */
/**********************************************************************/

/* This function computes a player's THAC0 by looking at his class, */
/* level, weapon adjustment and strength bonuses.		    */

void compute_thaco(ply_ptr)
creature	*ply_ptr;
{
	int	thaco, n;

	n = ply_ptr->level > 20 ? 19:ply_ptr->level-1;

	thaco = thaco_list[ply_ptr->class][n];
	if(ply_ptr->ready[WIELD-1])
		thaco -= ply_ptr->ready[WIELD-1]->adjustment;
	thaco -= mod_profic(ply_ptr);
 	thaco -= bonus[ply_ptr->strength];

	if(F_ISSET(ply_ptr, PBLESS))
		thaco -= 1;

	ply_ptr->thaco = MAX(0, thaco);

}

/**********************************************************************/
/*				mod_profic			      */
/**********************************************************************/

/* This function returns the player's currently-used proficiency div 20 */

int mod_profic(ply_ptr)
creature	*ply_ptr;
{
	int	amt;
	switch(ply_ptr->class) {
		case FIGHTER:
		case BARBARIAN:
			amt = 20;
			break;
		case RANGER:
		case PALADIN:
			amt = 25;
			break;
		case THIEF:
		case ASSASSIN:
		case MONK:
		case CLERIC:
			amt = 30;
			break;
		default:
			amt = 40;
			break;
	}

	if(ply_ptr->ready[WIELD-1] && 
		ply_ptr->ready[WIELD-1]->type <= MISSILE)
		return(profic(ply_ptr, ply_ptr->ready[WIELD-1]->type)/amt);
	else
		return(profic(ply_ptr, BLUNT)/amt);
}

/**********************************************************************/
/*				weight_ply			      */
/**********************************************************************/

/* This function calculates the total weight that a player (or monster) */
/* is carrying in his inventory.					*/

int weight_ply(ply_ptr)
creature	*ply_ptr;
{
	int	i, n = 0;
	otag	*op;

	op = ply_ptr->first_obj;
	while(op) {
		if(!F_ISSET(op->obj, OWTLES))
			n += weight_obj(op->obj);
		op = op->next_tag;
	}

	for(i=0; i<MAXWEAR; i++)
		if(ply_ptr->ready[i])
			n += weight_obj(ply_ptr->ready[i]);

	return(n);

}

/**********************************************************************/
/*				max_weight			      */
/**********************************************************************/

/* This function returns the maximum weight a player can be allowed to */
/* hold in his inventory.					       */

int max_weight(ply_ptr)
creature	*ply_ptr;
{
	int	n;

	n = 20 + ply_ptr->strength*10;
	if(ply_ptr->class == BARBARIAN)
		n += ply_ptr->level*10;

	return(n);
}
	
/**********************************************************************/
/*				profic				      */
/**********************************************************************/

/* This function determines a weapons proficiency percentage.  The first */
/* parameter contains a pointer to the player whose percentage is being  */
/* determined.  The second is an integer containing the proficiency      */
/* number.								 */

int profic(ply_ptr, index)
creature	*ply_ptr;
int		index;
{
	long	prof_array[12];
	int	i, n, prof;

	switch (ply_ptr->class){
	case FIGHTER:
		prof_array[0] = 0L;		prof_array[1] = 768L;
		prof_array[2] = 1024L;		prof_array[3] = 1440L;
		prof_array[4] = 1910L;		prof_array[5] = 16000L;
		prof_array[6] = 31214L;		prof_array[7] = 167000L;
		prof_array[8] = 268488L;	prof_array[9] = 695000L;
		prof_array[10] = 934808L;
		prof_array[11] = 500000000L;
		break;

	case BARBARIAN: 
	case PALADIN: 
	case RANGER:
	case MONK:
	case DM:
	case CARETAKER:
		prof_array[0] = 0L;		prof_array[1] = 768L;
		prof_array[2] = 1024L;		prof_array[3] = 1800L;
		prof_array[4] = 2388L;		prof_array[5] = 20000L;
		prof_array[6] = 37768L;		prof_array[7] = 205000L;
		prof_array[8] = 325610L;	prof_array[9] = 895000L;
		prof_array[10] = 1549760L;
		prof_array[11] = 500000000L;
		break;

	case CLERIC:
	case BARD:
	case THIEF:
	case ASSASSIN:
		prof_array[0] = 0L;		prof_array[1] = 768L;
		prof_array[2] = 1024L;		prof_array[3] = 1800L;
		prof_array[4] = 2388L;		prof_array[5] = 20000L;
		prof_array[6] = 42768L;		prof_array[7] = 220000L;
		prof_array[8] = 355610L;	prof_array[9] = 950000L;
		prof_array[10] = 1949760L;
		prof_array[11] = 500000000L;
		break;

	case MAGE:
		prof_array[0] = 0L;		prof_array[1] = 768L;
		prof_array[2] = 1024L;		prof_array[3] = 1800L;
		prof_array[4] = 2388L;		prof_array[5] = 25000L;
		prof_array[6] = 50768L;		prof_array[7] = 215000L;
		prof_array[8] = 385610L;	prof_array[9] = 1005000L;
		prof_array[10] = 2344760L;
		prof_array[11] = 500000000L;
		break;
	}
	
	n = ply_ptr->proficiency[index];
	for(i=0; i<11; i++)
		if(n < prof_array[i+1]) {
			prof = 10*i;
			break;
		}

	prof += ((n - prof_array[i])*10) / (prof_array[i+1] - prof_array[i]);

	return(prof);
}

/************************************************************************/
/*				mprofic					*/
/************************************************************************/

/* This function returns the magical realm proficiency as a percentage	*/

int mprofic(ply_ptr, index)
creature	*ply_ptr;
int		index;
{
	long	prof_array[12];
	int	i, n, prof;

        switch(ply_ptr->class){
        case MAGE:
                prof_array[0] = 0L;       prof_array[1] = 1024L;
                prof_array[2] = 2048L;    prof_array[3] = 4096L;
                prof_array[4] = 8192L;    prof_array[5] = 16384L;
                prof_array[6] = 35768L;   prof_array[7] = 85536L;
                prof_array[8] = 140000L;  prof_array[9] = 459410L;
                prof_array[10] = 2073306L; prof_array[11] = 500000000L;
                break;
        case CLERIC:
                prof_array[0] = 0L;       prof_array[1] = 1024L;
                prof_array[2] = 4092L;    prof_array[3] = 8192L;
                prof_array[4] = 16384L;   prof_array[5] = 32768L;
                prof_array[6] = 70536L;   prof_array[7] = 119000L;
                prof_array[8] = 226410L;  prof_array[9] = 709410L;
                prof_array[10] = 2973307L; prof_array[11] = 500000000L;
                break;
        case PALADIN:
        case MONK:
	case RANGER:    
                prof_array[0] = 0L;       prof_array[1] = 1024L;
                prof_array[2] = 8192L;    prof_array[3] = 16384L;
                prof_array[4] = 32768L;   prof_array[5] = 65536L;
                prof_array[6] = 105000L;  prof_array[7] = 165410L;
                prof_array[8] = 287306L;  prof_array[9] = 809410L;
                prof_array[10] = 3538232L; prof_array[11] = 500000000L;
                break;  
        default:
                prof_array[0] = 0L;       prof_array[1] = 1024L;
                prof_array[2] = 40000L;   prof_array[3] = 80000L;
                prof_array[4] = 120000L;  prof_array[5] = 160000L;
                prof_array[6] = 205000L;  prof_array[7] = 222000L;
                prof_array[8] = 380000L;  prof_array[9] = 965410L;
                prof_array[10] = 5495000; prof_array[11] = 500000000L;
                break;
        } 

	n = ply_ptr->realm[index-1];
	for(i=0; i<11; i++)
		if(n < prof_array[i+1]) {
			prof = 10*i;
			break;
		}

	prof += ((n - prof_array[i])*10) / (prof_array[i+1] - prof_array[i]);

	return(prof);
}

/**********************************************************************/
/*				fall_ply			      */
/**********************************************************************/

/* This function computes a player's bonus (or susceptibility) to falling */
/* while climbing.							  */

int fall_ply(ply_ptr)
creature	*ply_ptr;
{
	int	fall, j;

	fall = bonus[ply_ptr->dexterity]*5;
	for(j=0; j<MAXWEAR; j++)
		if(ply_ptr->ready[j])
			if(F_ISSET(ply_ptr->ready[j], OCLIMB))
				fall += ply_ptr->ready[j]->pdice*3;
	return(fall);
}

/**********************************************************************/
/*				find_who			      */
/**********************************************************************/

/* This function searches through the players who are currently logged */
/* on for a given player name.  If that player is on, a pointer to him */
/* is returned.							       */

creature *find_who(name)
char	*name;
{
	int i;

	for(i=0; i<Tablesize; i++) {
		if(!Ply[i].ply || !Ply[i].io || Ply[i].ply->fd < 0) continue;
		if(!strcmp(Ply[i].ply->name, name))
			return(Ply[i].ply);
	}

	return(0);

}

/**********************************************************************/
/*				lowest_piety			      */
/**********************************************************************/

/* This function finds the player with the lowest piety in a given room. */
/* The pointer to that player is returned.  In the case of a tie, one of */
/* them is randomly chosen.						 */

creature *lowest_piety(rom_ptr, invis)
room	*rom_ptr;
int	invis;
{
	creature	*ply_ptr = 0;
	ctag		*cp;
	int		totalpiety=0, pick=0;

	cp = rom_ptr->first_ply;
	if(!cp)
		return(0);

	while(cp) {
		if(F_ISSET(cp->crt, PHIDDN) ||
		   (F_ISSET(cp->crt, PINVIS) && !invis) ||
		   F_ISSET(cp->crt, PDMINV)) {
			cp = cp->next_tag;
			continue;
		}
		totalpiety += MAX(1, (25 - cp->crt->piety));
		cp = cp->next_tag;
	}

	if(!totalpiety)
		return(0);
	pick = mrand(1, totalpiety);

	cp = rom_ptr->first_ply;
	totalpiety = 0;
	while(cp) {
		if(F_ISSET(cp->crt, PHIDDN) ||
		   (F_ISSET(cp->crt, PINVIS) && !invis) ||
		   F_ISSET(cp->crt, PDMINV)) {
			cp = cp->next_tag;
			continue;
		}
		totalpiety += MAX(1, (25 - cp->crt->piety));
		if(totalpiety >= pick) {
			ply_ptr = cp->crt;
			break;
		}
		cp = cp->next_tag;
	}

	return(ply_ptr);
}

/**********************************************************************/
/*				has_light			      */
/**********************************************************************/

/* This function returns true if the player in the first parameter is */
/* holding or wearing anything that generates light.		      */

int has_light(crt_ptr)
creature	*crt_ptr;
{
	int	i, light = 0;

	for(i=0; i<MAXWEAR; i++) {
		if(!crt_ptr->ready[i]) continue;
		if(F_ISSET(crt_ptr->ready[i], OLIGHT)) {
			if((crt_ptr->ready[i]->type == LIGHTSOURCE &&
			   crt_ptr->ready[i]->shotscur > 0) ||
			   crt_ptr->ready[i]->type != LIGHTSOURCE) {
				light = 1;
				break;
			}
		}
	}

	if(F_ISSET(crt_ptr, PLIGHT)) {
		light = 1;
		i = MAXWEAR;
	}

	if(light) 
		return(i+1);
	else 
		return(0);

}

/************************************************************************/
/*				ply_prompt				*/
/************************************************************************/

/* This function returns the prompt that the player should be seeing	*/

char *ply_prompt(ply_ptr)
creature	*ply_ptr;
{
	static char prompt[40];
	int fd;

	fd = ply_ptr->fd;

	if(fd < 0 || F_ISSET(ply_ptr, PSPYON) || F_ISSET(ply_ptr, PREADI))
		prompt[0] = 0;

	else if(F_ISSET(ply_ptr, PPROMP) || F_ISSET(ply_ptr, PALIAS))
		if(F_ISSET(ply_ptr, PALIAS))
			sprintf(prompt, "(%d H %d M): ", Ply[fd].extr->alias_crt->hpcur,
        	            Ply[fd].extr->alias_crt->mpcur);
		else

			sprintf(prompt, "(%d H %d M): ", ply_ptr->hpcur,
			    ply_ptr->mpcur);

	else
		strcpy(prompt, ": ");

	return prompt;
}

 
/**********************************************************************/
/*                            low_piety_alg                          */
/**********************************************************************/
/* This function is a varation on the lowest piety function.  The    *
 * searchs the given player list, totally up all the player's piety  *
 * (30 - ply piety), and then randomly picking a player from the     *
 * based on the player's piety.  Players with lower piety have a     *
 * greater chance of being attacked.  The alg, parameter tells the   *
 * whether to ignore a given player alignemnt (alg=1, only consider  *
 * good players, -1 only consider evil players).  The invis param    *
 * tells if the monster can detect invisible. */

creature *low_piety_alg(rom_ptr, invis, alg,lvl)
room    *rom_ptr;
int     invis;
int		alg;
int  	lvl;
{
    creature    *ply_ptr = 0;
    ctag        *cp;
    int        total, pick;

	if(!rom_ptr)
		return(NULL);
 
        cp = rom_ptr->first_ply;
        total = 0;

       if(!cp)
                return(NULL);
 
        while(cp) {
            if(F_ISSET(cp->crt, PHIDDN) || 
				(F_ISSET(cp->crt, PINVIS) && !invis) || 
				F_ISSET(cp->crt, PDMINV) ||
				(cp->crt->level < lvl) ||
				((alg == 1) && (cp->crt->alignment > -100)) ||
				((alg == -1) && (cp->crt->alignment < 100))) {
	                cp = cp->next_tag;
                    continue;
         	}
            total += MAX(1, (30 - cp->crt->piety));
            cp = cp->next_tag;
        }
 
        if(!total)
                return(NULL);

        pick = mrand(1, total);
 
        cp = rom_ptr->first_ply;
        total = 0;
        while(cp) {
            if(F_ISSET(cp->crt, PHIDDN) ||
               (F_ISSET(cp->crt, PINVIS) && !invis) ||
				F_ISSET(cp->crt, PDMINV) ||
				((alg == 1) && (cp->crt->alignment > -100)) ||
				((alg == -1) && (cp->crt->alignment < 100))) {
                    cp = cp->next_tag;
                    continue;
                }

            total += MAX(1, (30 - cp->crt->piety));
            if(total >= pick) {
                    ply_ptr = cp->crt;
                    break;
            }
            cp = cp->next_tag;
        }
 
        return(ply_ptr);
}
                        
/****************************************************************/
/*			luck					*/
/****************************************************************/

/* This sets the luck value for a given player			*/

int luck(ply_ptr)
creature	*ply_ptr;
{
	int	fd, num, gld, alg, con, smrt;
	extra	*extr;
	
	if(!ply_ptr)
		return(0);
	if(ply_ptr->type != PLAYER)
		return(0);

	gld = ply_ptr->gold;
	alg = abs(ply_ptr->alignment);
	alg = alg+1;
#ifdef WIN32
	alg = (int)(alg/10);
#else
	alg = rint(alg/10);
#endif
	if(!alg)
		alg = 1;
	con = ply_ptr->constitution;
	smrt = ply_ptr->intelligence;

	num = 100*(smrt+con);
	num /= alg;
	num -= gld/50000;

	if(ply_ptr->ready[HELD-1] && F_ISSET(ply_ptr->ready[HELD-1], OLUCKY)) 
		num += ply_ptr->ready[HELD-1]->ndice;

	if(num>99)
		num=99;
	if(num<1)
		num=1;
	
	Ply[ply_ptr->fd].extr->luck = num;

	return(num);
}
