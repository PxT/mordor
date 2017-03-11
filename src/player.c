/*
 * PLAYER.C:
 *
 *	Player routines.
 *
 *	Copyright (C) 1991, 1992, 1993 Brooke Paul
 *
 * $Id: player.c,v 6.18 2001/07/02 01:05:43 develop Exp $
 *
 * $Log: player.c,v $
 * Revision 6.18  2001/07/02 01:05:43  develop
 * logout crash bug...debugging
 *
 * Revision 6.17  2001/07/02 01:00:25  develop
 * fixed bug in uninit_ply that caused a crash when ply has a conjured
 * pet
 *
 * Revision 6.16  2001/06/10 13:25:38  develop
 * fixed disease effect
 *
 * Revision 6.15  2001/06/06 19:36:46  develop
 * conjure added
 *
 * Revision 6.14  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */
#include <math.h>
#include "../include/mordb.h"
#include "mextern.h"

/**********************************************************************/
/*				init_ply			      */
/**********************************************************************/

/* This function initializes a player's stats and loads up the room he */
/* should be in.  This should only be called when the player first     */
/* logs on. 							       */

void init_ply( creature	*ply_ptr )
{
	char	file[256];
	room	*rom_ptr;
	object	*obj_ptr, *obj_ptr2, *cnt_ptr;
	otag	*op, *otemp, *cop;
	time_t	t, tdiff;
	int	n, i, cantwear=0,class,room;
	int		found = 0;
	struct stat	f_stat;

	F_CLR(ply_ptr, PSPYON);
	F_CLR(ply_ptr, PREADI);
	F_CLR(ply_ptr, PSECOK);


	/* check for DMs not on the list */
	if(ply_ptr->class == DM) {
		for(i=0;i<=6;i++) {
			if(!strcmp(ply_ptr->name, dmname[i]))
			{
				found = 1;
				break;
			}
		}
		if ( !found )
		{
			ply_ptr->class = BUILDER;
			sprintf(g_buffer, "Invalid DM (%s) logged in.", ply_ptr->name);
			elog_broad(g_buffer);
		}
	}

	/* DMs on the list are DMs no matter what */
	for(i=0;i<6;i++) {
		if(!strcmp(ply_ptr->name, dmname[i])) {
			ply_ptr->class = DM;
			i=0;
			break;
		}
	}

	if(ply_ptr->class < BUILDER) {
		ply_ptr->daily[DL_BROAD].max = 5 + ply_ptr->level/2;
		ply_ptr->daily[DL_ENCHA].max = 4;
		ply_ptr->daily[DL_FHEAL].max = MAX(3, 3 + (ply_ptr->level-5)/3);
		ply_ptr->daily[DL_TRACK].max = MAX(7, 3 + (ply_ptr->level-5)/4);
		ply_ptr->daily[DL_DEFEC].max = 1;
		/* this charm really not in use any more */
		ply_ptr->daily[DL_CHARM].max = MAX(5, 3 + ply_ptr->level/4);

		ply_ptr->daily[DL_RCHRG].max = MAX(7, 3 + ply_ptr->level/4);
		ply_ptr->daily[DL_BROAE].max = 10 + ply_ptr->level/2;
		ply_ptr->daily[DL_RMGIC].max = 3;
		ply_ptr->daily[DL_TELEP].max = 3;
	}
	else {
		ply_ptr->daily[DL_BROAD].cur = 20;
/*		ply_ptr->daily[DL_BROAD].max = 200; */
		/* is only a signed char, 200 sets a negative number */
		ply_ptr->daily[DL_BROAD].max = 127;
		ply_ptr->daily[DL_DEFEC].max = 10;
		ply_ptr->daily[DL_DEFEC].cur = 1;

        }

	/* builders get auto dminvis */
	if(ply_ptr->class>=BUILDER)
		F_SET(ply_ptr, PDMINV);

	/* alcs get auto d-m */
	if(ply_ptr->class==ALCHEMIST)
		F_SET(ply_ptr, PDMAGI);

	F_SET(ply_ptr, PNOSUM);
	F_CLR(ply_ptr, PALIAS);
	if(ply_ptr->class < BUILDER)
		F_SET(ply_ptr, PPROMP);

	if(ply_ptr->gold < 0) {
		sprintf(g_buffer, "%s (%s) logged on with - gold.",
			ply_ptr->name, Ply[ply_ptr->fd].io->address);
		elog_broad( g_buffer );
		ply_ptr->gold = 0;
	}

	broadcast_login( ply_ptr, 1 );

	t = time(0);

	if(ply_ptr->class < CARETAKER) {
		sprintf(g_buffer, "%s (%s) logged on.", 
			ply_ptr->name, Ply[ply_ptr->fd].io->address);
		plog( g_buffer );
	}

	if(ply_ptr->class >= IMMORTAL )
	{
		sprintf(g_buffer, "%s  (%s@%s) logged on.", 
			ply_ptr->name, Ply[ply_ptr->fd].io->userid, 
			Ply[ply_ptr->fd].io->address);
		ilog( g_buffer );
	}

	ply_ptr->lasttime[LT_PSAVE].ltime = t;
	ply_ptr->lasttime[LT_PSAVE].interval = SAVEINTERVAL;
	ply_ptr->lasttime[LT_SECCK].ltime = t;
	ply_ptr->lasttime[LT_HEALS].ltime = t+90;

	/* new spam check */
	ply_ptr->lasttime[LT_PSEND].ltime = t;
	ply_ptr->lasttime[LT_PSEND].misc = 0;


    if(load_rom(ply_ptr->rom_num, &rom_ptr) < 0) {
		sprintf(g_buffer, "%s (%s) Attempted logon to bad or missing room (%d).", 
			ply_ptr->name, Ply[ply_ptr->fd].io->address, ply_ptr->rom_num);
		elog_broad(g_buffer ); 
		if ( load_rom(start_room_num, &rom_ptr) < 0 )
		{
			output(ply_ptr->fd, "Cant find room in database.\n");

			if(create_a_room(start_room_num, rom_ptr) < 0) {
			   merror("Problem trying to create StartRoom", FATAL);
			} else {
			   if(load_rom(ply_ptr->rom_num, &rom_ptr) < 0)
				merror("created StartRoom but couldn't load it",FATAL);
			}
			   
		}
    }

	n = count_vis_ply(rom_ptr);
	if((F_ISSET(rom_ptr, RONEPL) && n > 0) ||
	  (F_ISSET(rom_ptr, RTWOPL) && n > 1) ||
	  (F_ISSET(rom_ptr, RTHREE) && n > 2) ||
      (F_ISSET(rom_ptr, RNOLOG))) {
		if(rom_ptr->death_rom)
			room = rom_ptr->death_rom;
		else
			room = start_room_num;

		load_rom(room, &rom_ptr);
	}

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
	
	/* Beginning of new wear code */

	/* Make sure those cursed items stay on! */
	op = ply_ptr->first_obj;

	while(op) {
		otemp = op->next_tag;
		if(F_ISSET(op->obj, OCURSW)){
			obj_ptr = op->obj;
			ply_ptr->ready[obj_ptr->wearflag-1] = obj_ptr;
			F_SET(obj_ptr, OWEARS);
			equip(ply_ptr, obj_ptr);
			del_obj_crt(obj_ptr, ply_ptr);
		}
		op=otemp;
	}
	op = ply_ptr->first_obj;

	while(op) {

		otemp = op->next_tag;

		if((F_ISSET(ply_ptr, PDINVI) ? 1:!F_ISSET(op->obj, OINVIS)) && op->obj->wearflag &&
		   op->obj->wearflag != HELD && op->obj->wearflag != WIELD) {

			obj_ptr = op->obj;

			if(obj_ptr->type == ARMOR && F_ISSET(obj_ptr, ONOMAG) && ply_ptr->class == MAGE) {
				op = otemp;
				continue;
			}

		if(obj_ptr->type == ARMOR && F_ISSET(obj_ptr, ONOFEM) && !F_ISSET(ply_ptr,PMALES)) {
			op = otemp;
			continue;
		}

		if(obj_ptr->type == ARMOR && F_ISSET(obj_ptr, ONOMAL) && F_ISSET(ply_ptr,PMALES)) {
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
   		     	(objectcheck_guild(obj_ptr) != check_guild(ply_ptr))){
				op = otemp;
				continue;
	    	}              

	if(ply_ptr->class > 10)
		class = ply_ptr->class + 11;
	else
		class = ply_ptr->class;
	if(F_ISSET(obj_ptr,OCLSEL))
	if(!F_ISSET(obj_ptr,OCLSEL + class) && ( ply_ptr->class < BUILDER)){
				op = otemp;
				continue;
	}

	if(!F_ISSET(obj_ptr,OCLSEL + class) && (ply_ptr->class== MONK || ply_ptr->class == MAGE) && obj_ptr->armor >5){
		op = otemp;
		continue;
    }
    if((obj_ptr->wearflag == FINGER||obj_ptr->wearflag ==SHIELD) && ply_ptr->class == MONK){
		op=otemp;
		continue;
	}

	if(F_ISSET(obj_ptr, OGOODO) && ply_ptr->alignment < -50) {
		op = otemp;
		continue;
	}

	if(F_ISSET(obj_ptr, OEVILO) && ply_ptr->alignment > 50) {
		op = otemp;
		continue;
	}

	i = (F_ISSET(obj_ptr, OSIZE1) ? 1:0) * 2 + (F_ISSET(obj_ptr, OSIZE2) ? 1:0);

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
				if(ply_ptr->ready[NECK1-1] && !ply_ptr->ready[NECK2-1]) {
					F_SET(obj_ptr, OWEARS);
					ply_ptr->ready[NECK2-1] = obj_ptr;
					equip(ply_ptr,obj_ptr);
			
				}
				else if(!ply_ptr->ready[NECK1-1]) {	
					ply_ptr->ready[NECK1-1] = obj_ptr;
					F_SET(obj_ptr, OWEARS);
					equip(ply_ptr,obj_ptr);
				}
			}

			else if(obj_ptr->wearflag == FINGER && ply_ptr->class != MONK) {
				for(i=FINGER1; i<FINGER8+1; i++) {
					if(!ply_ptr->ready[i-1]) {
						ply_ptr->ready[i-1] = obj_ptr;
						F_SET(obj_ptr, OWEARS);
						equip(ply_ptr,obj_ptr);
						break;
					}
				}
			}

			else {
				if(!ply_ptr->ready[obj_ptr->wearflag-1] && !F_ISSET(obj_ptr, OCURSE)) {
				  ply_ptr->ready[obj_ptr->wearflag-1] = obj_ptr;
				  F_SET(obj_ptr, OWEARS);
				  equip(ply_ptr,obj_ptr);
				}
			}
			if(F_ISSET(obj_ptr, OWEARS))
				del_obj_crt(obj_ptr, ply_ptr);

		}
		op = otemp;
	}


	/* End of new wear code */
	
	compute_ac(ply_ptr);
	compute_thaco(ply_ptr);
	luck(ply_ptr);
	update_ply(ply_ptr); 

	sprintf(file, "%s/dialin", get_doc_path());

	/* Just so we know who to mail */
	/* Remove it if you are that paranoid. */
/*
	if (!strcmp(Ply[ply_ptr->fd].io->address, "128.200.21.105")|| !strcmp(Ply[ply_ptr->fd].io->address, "mordor.bio.uci.edu"))
	{
		sprintf(g_buffer, "DMs here are: %s, %s, %s, %s, %s, %s.\n", dmname[0], dmname[1], dmname[2], dmname[3], dmname[4], dmname[5]);
		output(ply_ptr->fd, g_buffer);
	}
	if (ply_ptr->class == DM)
	{
		sprintf(g_buffer, "DMs here are: %s, %s, %s, %s, %s, %s.\n", dmname[0], dmname[1], dmname[2], dmname[3], dmname[4], dmname[5]);
		output(ply_ptr->fd, g_buffer);
	}
*/	

	if (ply_ptr->class < BUILDER) {
		sprintf(file, "%s/welcome", get_log_path());
		view_file(ply_ptr->fd, 1, file);
	}
	if (ply_ptr->class == BUILDER) {
		sprintf(file, "%s/welcome.build", get_log_path());
		view_file(ply_ptr->fd, 1, file);
	}
	if (ply_ptr->class == CARETAKER) {
		sprintf(file, "%s/welcome.ct", get_log_path());
		view_file(ply_ptr->fd, 1, file);
	}
	if (ply_ptr->class == DM) {
		sprintf(file, "%s/welcome.dm", get_log_path());
		view_file(ply_ptr->fd, 1, file);
	}
	sprintf(file, "%s/%s", get_post_path(), ply_ptr->name);

	if(file_exists(file)) {
	stat(file,&f_stat);

        if (f_stat.st_atime < f_stat.st_ctime) 
                output(ply_ptr->fd, "\n*** You have new mail in the post office.\n");
        else
		output(ply_ptr->fd, "\n*** You have mail in the post office.\n");

	}

	/* check for miscelanous problems */
	validate_player( ply_ptr );

}

/**********************************************************************/
/*				uninit_ply			      */
/**********************************************************************/

/* This function de-initializes a player who has left the game.  This  */
/* is called whenever a player quits or disconnects, right before save */
/* is called.							       */

void uninit_ply( creature *ply_ptr )
{
	creature	*crt_ptr;
	ctag		*cp, *prev;
	int		i;


	if(ply_ptr->parent_rom)
		del_ply_rom(ply_ptr, ply_ptr->parent_rom);

	courageous(ply_ptr);

	for(i=0;i<Tablesize;i++) {
               if(!Ply[i].ply || Ply[i].ply == ply_ptr)
                       continue;
               if(is_stolen_crt(ply_ptr->name, Ply[i].ply))
                       del_stolen_crt(ply_ptr, Ply[i].ply);
       }

        cp = ply_ptr->first_fol;
        while(cp) {
                if(cp->crt->type == MONSTER && F_ISSET(cp->crt, MCONJU)) {
			cp->crt->following = 0;
			F_CLR(cp->crt, MCONJU);
			F_CLR(cp->crt, MUNKIL);
			cp->crt->lasttime[LT_CONJU].interval = 0;
                }
        cp = cp->next_tag;
        }


	cp = ply_ptr->first_fol;
	while(cp) {
		cp->crt->following = 0;
		if(cp->crt->type == PLAYER)
		{
			sprintf(g_buffer, "You stop following %s.\n", ply_ptr->name);
			output(cp->crt->fd, g_buffer);
		}
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

		if(ply_ptr->class < BUILDER)
		{
			sprintf(g_buffer, "%s stops following you.\n", 
			      ply_ptr->name);
			output(crt_ptr->fd, g_buffer);
		}
	}

	for(i=0; i<MAXWEAR; i++)
		if(ply_ptr->ready[i]) {
			add_obj_crt(ply_ptr->ready[i], ply_ptr);
			dequip(ply_ptr,ply_ptr->ready[i]);
			ply_ptr->ready[i] = 0;
		}

	broadcast_login(ply_ptr, 0);
	
	sprintf(g_buffer, "%s logged off.", ply_ptr->name);
	if(ply_ptr->class < CARETAKER)
		plog( g_buffer );
	else
		ilog( g_buffer );
	

}

/**********************************************************************/
/*				update_ply			      */
/**********************************************************************/

/* This function checks up on all a player's time-expiring flags to see */
/* if some of them have expired.  If so, flags are set accordingly.     */

void update_ply( creature *ply_ptr )
{
	time_t 	t, ltime;
	int	item, i, dur;
	char 	ill, prot=1;
	otag	*op, *cop;

	t = time(0);
	ply_ptr->lasttime[LT_HOURS].interval +=
		(t - ply_ptr->lasttime[LT_HOURS].ltime);
	ply_ptr->lasttime[LT_HOURS].ltime = t;



if(EATNDRINK){
	if(ply_ptr->lasttime[LT_SUSTN].interval+ply_ptr->lasttime[LT_SUSTN].ltime < t) {
		ply_ptr->lasttime[LT_SUSTN].interval=ply_ptr->constitution*144-mrand(3,200);
		ply_ptr->lasttime[LT_SUSTN].ltime=t;
		switch(ply_ptr->talk[5]) {
			case 0:
				ply_ptr->talk[5]+=1;
				break;
			case 1:
				if(mrand(1,100) > 50)
					ply_ptr->talk[5]+=1;
				break;
			case 2:
				output(ply_ptr->fd, "You're getting thirsty.\n");
				ply_ptr->talk[5]+=1;
				break;
			case 3:
				F_CLR(ply_ptr, PNSUSN);
				ply_ptr->talk[5]+=1;
				break;				
			case 4:
				F_SET(ply_ptr, PNSUSN);
				output(ply_ptr->fd, "Your mouth is dry.\n");
				ply_ptr->talk[5]+=1;
				break;
			case 5:
				F_SET(ply_ptr, PNSUSN);
				ply_ptr->hpcur-=5;
				if(ply_ptr->hpcur<1) {
					ply_ptr->hpcur=0;
					die(ply_ptr,ply_ptr);
					break;
				}
				ply_ptr->talk[5]+=1;
				break;		
			case 6:
				F_SET(ply_ptr, PNSUSN);
				ply_ptr->talk[5]+=1;
				break;
			case 7:
				F_SET(ply_ptr, PNSUSN);
				output(ply_ptr->fd, "Your thirst is unbearable.\n");
				ply_ptr->hpcur-=5;  
				if(ply_ptr->hpcur<1) {
					ply_ptr->hpcur=0;
					die(ply_ptr,ply_ptr);
					break;
				}
				ply_ptr->talk[5]+=1;
				break;
			case 8: 
				F_SET(ply_ptr, PNSUSN);
				ply_ptr->hpcur-=10;
				if(ply_ptr->hpcur<1) {
					ply_ptr->hpcur=0;
					die(ply_ptr,ply_ptr);
					break;
				}
				ply_ptr->talk[5]+=1;
				break;
			case 9:
				F_SET(ply_ptr, PNSUSN);
				ANSI(ply_ptr->fd, AFC_RED);
				output(ply_ptr->fd, "You need water badly!");
				ANSI(ply_ptr->fd, AFC_WHITE);
				ply_ptr->hpcur-=20;
				if(ply_ptr->hpcur<1) {
					ply_ptr->hpcur=0;
					die(ply_ptr,ply_ptr);
					break;
				}
				ply_ptr->talk[5]=9;
				break;
			default:
				if(ply_ptr->talk[5])
					ply_ptr->talk[5]-=1;
				break;
		}
		switch(ply_ptr->talk[6]) {
			case 0:
				if(mrand(1,100) > 50)
					ply_ptr->talk[6]+=1;
				break;
			case 1:
				ply_ptr->talk[6]+=1;
				break;
			case 2:
				output(ply_ptr->fd, "You're getting hungry.\n");
				ply_ptr->talk[6]+=1;
				break;
			case 3:
				F_CLR(ply_ptr, PNSUSN);
				if(mrand(1,100) > 50)
					ply_ptr->talk[6]+=1;
				break;
			case 4:
				F_SET(ply_ptr, PNSUSN); 
				output(ply_ptr->fd, "Your stomach growls.\n");
				broadcast_rom(ply_ptr->fd, ply_ptr->rom_num, 
					"%M's stomach growls.", m1arg(ply_ptr));
				ply_ptr->talk[6]+=1;
				break;
			case 5:
				F_SET(ply_ptr, PNSUSN);
				output(ply_ptr->fd, "You better get food soon.\n");
				ply_ptr->hpcur-=5;
				if(ply_ptr->hpcur<1) {
					ply_ptr->hpcur=0;
					die(ply_ptr,ply_ptr);
					break;
				}
				ply_ptr->talk[6]+=1;
				break;
			case 6:
				F_SET(ply_ptr, PNSUSN);
				ply_ptr->talk[6]+=1;
				break;
			case 7:
				F_SET(ply_ptr, PNSUSN);
				output(ply_ptr->fd, "Your stomach begins to ache.\n");
				ply_ptr->hpcur-=5;
				if(ply_ptr->hpcur<1) {
					ply_ptr->hpcur=0;
					die(ply_ptr,ply_ptr);
					break;
				}
				ply_ptr->talk[6]+=1;
				break;
			case 8:
				F_SET(ply_ptr, PNSUSN);
				output(ply_ptr->fd, "You need food badly!.\n");
				ply_ptr->hpcur-=10;
				if(ply_ptr->hpcur<1) {
					ply_ptr->hpcur=0;
					die(ply_ptr,ply_ptr);
					break;
				}
				ply_ptr->talk[6]+=1;
				break;
			case 9:
				F_SET(ply_ptr, PNSUSN);
				ANSI(ply_ptr->fd, AFC_RED);
				output(ply_ptr->fd, "You're starving!\n");
				ANSI(ply_ptr->fd, AFC_WHITE);
                broadcast_rom(ply_ptr->fd, ply_ptr->rom_num, 
					"%M looks faint from lack of food.\n", 
					m1arg(ply_ptr));
                ply_ptr->hpcur-=20; 
                if(ply_ptr->hpcur<1) {
					ply_ptr->hpcur=0;
                    die(ply_ptr,ply_ptr);
                    break;
				}
                ply_ptr->talk[6]=9;  
                break;
			default:
				if(ply_ptr->talk[6])   
					ply_ptr->talk[6]-=1;
                break;
		}
	}
}

	/* Check for temp enchant items carried/inventory/in containers */
	if(ply_ptr->lasttime[LT_MSCAV].interval+ply_ptr->lasttime[LT_MSCAV].ltime<t){  
	  ply_ptr->lasttime[LT_MSCAV].ltime=t;
	  ply_ptr->lasttime[LT_MSCAV].interval=40;
	  op=ply_ptr->first_obj;
	  while(op) {
	  	if(op->obj->type == CONTAINER) {
			cop=op->obj->first_obj;
			while(cop) {
				if(cop->obj && F_ISSET(cop->obj, OTMPEN)) {
					dur=(int)(cop->obj->magicrealm);
                        		ltime=(int)(cop->obj->magicpower);
					if(!dur && !ltime) {
						cop->obj->pdice=0;
	        	        cop->obj->adjustment=0; 
        	        	F_CLR(cop->obj, OTMPEN);
						F_CLR(cop->obj, ORENCH);
		                F_CLR(cop->obj, OENCHA);
                		if(F_ISSET(ply_ptr, PDMAGI))
						{
                            sprintf(g_buffer, "The enchantment fades on your %s.\n", cop->obj->name);
                            output(ply_ptr->fd, g_buffer);
						}
                        break;
					}
					if(!dur) {
						ltime-=1;
						cop->obj->magicpower=(char)ltime;
					}
					else {
						dur-=1;
                        cop->obj->magicrealm=(char)dur;
					}
				}
				cop=cop->next_tag;
        	}	
		}
		if(op->obj && F_ISSET(op->obj, OTMPEN)) {
			dur=(int)(op->obj->magicrealm);
			ltime=(int)(op->obj->magicpower);
			if(!dur && !ltime) {
				op->obj->pdice=0;
				op->obj->adjustment=0;
				F_CLR(op->obj, OTMPEN);
				F_CLR(op->obj, ORENCH);
				F_CLR(op->obj, OENCHA);
				if(F_ISSET(ply_ptr, PDMAGI))
				{
					sprintf(g_buffer, "The enchantment fades on your %s.\n", op->obj->name);
					output(ply_ptr->fd, g_buffer);
				}
				break;
			}
			if(!dur) {
				ltime-=1;
				op->obj->magicpower=(char)ltime;
			}
			else {
				dur-=1;
				op->obj->magicrealm=(char)dur;
			}
		}

	  op=op->next_tag;
	  }
	/* Check held/readied eq */
	for(i=0; i<MAXWEAR; i++) {
		/* ASSUMPTION:  we will only find containers at the HELD 
		   position */
		if(i==(HELD-1) && ply_ptr->ready[i] && ply_ptr->ready[i]->type==CONTAINER) {
			cop=ply_ptr->ready[i]->first_obj;
			while(cop) {
				if(cop->obj && F_ISSET(cop->obj, OTMPEN)) {
					dur=(int)(cop->obj->magicrealm);
                    ltime=(int)(cop->obj->magicpower);
                    if(!dur && !ltime) {
						cop->obj->pdice=0;
                        cop->obj->adjustment=0;
                        F_CLR(cop->obj, OTMPEN);
                        F_CLR(cop->obj, ORENCH);
                        F_CLR(cop->obj, OENCHA);
                        if(F_ISSET(ply_ptr, PDMAGI))
						{
							sprintf(g_buffer, "The enchantment on your %s fades.\n", cop->obj->name);
							output(ply_ptr->fd, g_buffer);
						}
                        break;
                    }
                    if(!dur) {
						ltime-=1;
						cop->obj->magicpower=(char)ltime;
                    }
                    else {
						dur-=1;
						cop->obj->magicrealm=(char)dur;
                    }
				}
				cop=cop->next_tag;
            }
		}
		if(ply_ptr->ready[i]&&F_ISSET(ply_ptr->ready[i], OTMPEN)){
			ltime=(int)(ply_ptr->ready[i]->magicpower);
            dur=(int)(ply_ptr->ready[i]->magicrealm);
            if(!dur && !ltime) {
				ply_ptr->ready[i]->pdice=0;
                ply_ptr->ready[i]->adjustment=0;
				F_CLR(ply_ptr->ready[i], OTMPEN);
                F_CLR(ply_ptr->ready[i], ORENCH);
                F_CLR(ply_ptr->ready[i], OENCHA);
				if(F_ISSET(ply_ptr, PDMAGI))
				{
					sprintf(g_buffer, "The enchantment fades on your %s.\n", ply_ptr->ready[i]->name);
					output(ply_ptr->fd, g_buffer);
				}
				break;
			}

			if(!dur) {
				ltime-=1;
				ply_ptr->ready[i]->magicpower=(char)ltime;
			}
			else
				dur-=1;
			ply_ptr->ready[i]->magicrealm=(char)dur;
		}
	} 
	}		
	
	if(F_ISSET(ply_ptr, PHASTE)) {
		if(t > LT(ply_ptr, LT_HASTE)) {
			ANSI(ply_ptr->fd, AFC_GREEN);
			output(ply_ptr->fd, "You feel slower.\n");
			ANSI(ply_ptr->fd, AFC_WHITE);
			F_CLR(ply_ptr, PHASTE);
			ply_ptr->dexterity -= 5;
			compute_ac(ply_ptr);
		}
	}
	if(F_ISSET(ply_ptr, PBESRK)) {
		if(t > LT(ply_ptr, LT_BSRKN)) {
			ANSI(ply_ptr->fd, AFC_GREEN);
			output(ply_ptr->fd, "Your berserker rage subsides.\n");
                        broadcast_rom(ply_ptr->fd, ply_ptr->rom_num,"%M's berserker rage departs.",
                                m1arg(ply_ptr));
			ply_ptr->hpmax = ply_ptr->hpmax - ply_ptr->level;
			ply_ptr->hpcur = MIN(ply_ptr->hpcur, ply_ptr->hpmax);
			ply_ptr->dexterity -= 3;
			ANSI(ply_ptr->fd, AFC_WHITE);
			F_CLR(ply_ptr, PBESRK);
			compute_ac(ply_ptr);
		}
	}
	if(F_ISSET(ply_ptr, PBARKS)) {
		if(t > LT(ply_ptr, LT_BSRKN)) {
			ANSI(ply_ptr->fd, AFC_GREEN);
                        broadcast_rom(ply_ptr->fd, ply_ptr->rom_num,"%M's barkskin diminishes.",
                                m1arg(ply_ptr));
 			output(ply_ptr->fd, "Your barkskin disipates.\n");
			ANSI(ply_ptr->fd, AFC_WHITE);
			F_CLR(ply_ptr, PBARKS);
			compute_ac(ply_ptr);
		}
	}
	if(F_ISSET(ply_ptr, PPRAYD)) {
		if(t > LT(ply_ptr, LT_PRAYD)) {
			ANSI(ply_ptr->fd, AFC_YELLOW);
			output(ply_ptr->fd, "You feel less pious.\n");
			F_CLR(ply_ptr, PPRAYD);
			ANSI(ply_ptr->fd, AFC_WHITE);
			ply_ptr->piety -= 5;
		}
	}
	if(F_ISSET(ply_ptr, PINVIS)) {
		if(t > LT(ply_ptr, LT_INVIS) && ply_ptr->class < DM) {
			ANSI(ply_ptr->fd, AFC_MAGENTA);
			output(ply_ptr->fd, "Your invisibility wears off.\n");
			ANSI(ply_ptr->fd, AFC_WHITE);
			F_CLR(ply_ptr, PINVIS);
		}
	}
	if(F_ISSET(ply_ptr, PDINVI)) {
		if(t > LT(ply_ptr, LT_DINVI)  && ply_ptr->class < DM) {
			ANSI(ply_ptr->fd, AFC_MAGENTA);
			output(ply_ptr->fd, "Your detect-invis wears off.\n");
			ANSI(ply_ptr->fd, AFC_WHITE);
			F_CLR(ply_ptr, PDINVI);
		}
	}
	if(F_ISSET(ply_ptr, PDMAGI)&&ply_ptr->class!=ALCHEMIST) {
		if(t > LT(ply_ptr, LT_DMAGI) && ply_ptr->class < DM) {
			ANSI(ply_ptr->fd, AFC_MAGENTA);
			output(ply_ptr->fd, "Your detect-magic wears off.\n");
			ANSI(ply_ptr->fd, AFC_WHITE);
			F_CLR(ply_ptr, PDMAGI);
		}
	}
	if(F_ISSET(ply_ptr, PHIDDN)) {
		if(t-ply_ptr->lasttime[LT_HIDES].ltime > 300L)
			F_CLR(ply_ptr, PHIDDN);
	}
	if(F_ISSET(ply_ptr, PPROTE)) {
		if(t > LT(ply_ptr, LT_PROTE)) {
			ANSI(ply_ptr->fd, AFC_YELLOW);
			output(ply_ptr->fd, "You feel less protected.\n");
			ANSI(ply_ptr->fd, AFC_WHITE);
			F_CLR(ply_ptr, PPROTE);
			compute_ac(ply_ptr);
		}
	}
	if(F_ISSET(ply_ptr, PLEVIT)) {
		if(t > LT(ply_ptr, LT_LEVIT) && ply_ptr->class < DM) {
			ANSI(ply_ptr->fd, AFC_MAGENTA);
			output(ply_ptr->fd, "Your feet hit the ground.\n");
			ANSI(ply_ptr->fd, AFC_WHITE);
			F_CLR(ply_ptr, PLEVIT);
		}
	}
	if(F_ISSET(ply_ptr, PBLESS)) {
		if(t > LT(ply_ptr, LT_BLESS)) {
			ANSI(ply_ptr->fd, AFC_YELLOW);
			output(ply_ptr->fd, "You feel less holy.\n");
			ANSI(ply_ptr->fd, AFC_WHITE);
			F_CLR(ply_ptr, PBLESS);
			compute_thaco(ply_ptr);
		}
	}

	if(F_ISSET(ply_ptr, PRFIRE)) {
		if(t > LT(ply_ptr, LT_RFIRE)) {
			ANSI(ply_ptr->fd, AFC_YELLOW);
			output(ply_ptr->fd, "Your skin returns to normal.\n");
			ANSI(ply_ptr->fd, AFC_WHITE);
			F_CLR(ply_ptr, PRFIRE);
		}
	}


	if(F_ISSET(ply_ptr, PRCOLD)) {
		if(t > LT(ply_ptr, LT_RCOLD)) {
			ANSI(ply_ptr->fd, AM_BOLD);
			ANSI(ply_ptr->fd, AFC_YELLOW);
			output(ply_ptr->fd, "A cold chill runs through your body.\n");
			ANSI(ply_ptr->fd, AM_NORMAL);
			ANSI(ply_ptr->fd, AFC_WHITE);
			F_CLR(ply_ptr, PRCOLD);
		}
	}


	if(F_ISSET(ply_ptr, PBRWAT)) {
		if(t > LT(ply_ptr, LT_BRWAT)) {
			ANSI(ply_ptr->fd, AM_BOLD);
			ANSI(ply_ptr->fd, AFC_BLUE);
			output(ply_ptr->fd, "Your lungs contract in size.\n");
			ANSI(ply_ptr->fd, AFC_WHITE);
			ANSI(ply_ptr->fd, AM_NORMAL);
			F_CLR(ply_ptr, PBRWAT);
		}
	}

	if(F_ISSET(ply_ptr, PSSHLD)) {
		if(t > LT(ply_ptr, LT_SSHLD)) {
			ANSI(ply_ptr->fd, AM_BOLD);
			ANSI(ply_ptr->fd, AFC_GREEN);
			output(ply_ptr->fd, "Your skin softens.\n");
			ANSI(ply_ptr->fd, AFC_WHITE);
			ANSI(ply_ptr->fd, AM_NORMAL);
			F_CLR(ply_ptr, PSSHLD);
		}
	}
	if(F_ISSET(ply_ptr, PFLYSP)) {
		if(t > LT(ply_ptr, LT_FLYSP)  && ply_ptr->class < DM) {
			ANSI(ply_ptr->fd, AFC_YELLOW);
			output(ply_ptr->fd, "You can no longer fly.\n");
			ANSI(ply_ptr->fd, AFC_WHITE);
			F_CLR(ply_ptr, PFLYSP);
		}
	}
	if(F_ISSET(ply_ptr, PRMAGI)) {
		if(t > LT(ply_ptr, LT_RMAGI)) {
			ANSI(ply_ptr->fd, AM_BOLD);
			ANSI(ply_ptr->fd, AFC_MAGENTA);
			output(ply_ptr->fd,"Your magical shield dissipates.\n");
			ANSI(ply_ptr->fd, AFC_WHITE);
			ANSI(ply_ptr->fd, AM_NORMAL);
			F_CLR(ply_ptr, PRMAGI);
		}
	}
	if(F_ISSET(ply_ptr, PSILNC)) {
		if(t > LT(ply_ptr, LT_SILNC)) {
			ANSI(ply_ptr->fd, AFC_GREEN);
			output(ply_ptr->fd,"Your voice returns!\n");
			ANSI(ply_ptr->fd, AFC_WHITE);
			F_CLR(ply_ptr, PSILNC);
		}
	}
	if(F_ISSET(ply_ptr, PKNOWA)) {
		if(t > LT(ply_ptr, LT_KNOWA) && ply_ptr->class < DM) {
			ANSI(ply_ptr->fd, AFC_CYAN);
			output(ply_ptr->fd,"Your perception is diminshed.\n");
			ANSI(ply_ptr->fd, AFC_WHITE);
			F_CLR(ply_ptr, PKNOWA);
		}
	}
	if(F_ISSET(ply_ptr, PLIGHT)) {
		if(t > LT(ply_ptr, LT_LIGHT)  && ply_ptr->class < DM) {
			ANSI(ply_ptr->fd, AFC_YELLOW);
			output(ply_ptr->fd, "Your magical light fades.\n");
			ANSI(ply_ptr->fd, AFC_WHITE);
			broadcast_rom(ply_ptr->fd, ply_ptr->rom_num,"%M's magical light fades.", 
				m1arg(ply_ptr));
			F_CLR(ply_ptr, PLIGHT);
		}
	}
	if(t > LT(ply_ptr, LT_CHRMD) && F_ISSET(ply_ptr, PCHARM)) {
		ANSI(ply_ptr->fd, AFC_YELLOW);
        output(ply_ptr->fd, "Your demeanor returns to normal.\n");
        F_CLR(ply_ptr, PCHARM);
		ANSI(ply_ptr->fd, AFC_WHITE);
	}

	/* check if player is suffering from any  aliment */
	if(F_ISSET(ply_ptr, PPOISN) || F_ISSET(ply_ptr, PDISEA) ||  F_ISSET(ply_ptr, PNSUSN))
		ill = 1;
	else
		ill =0;

	if(!F_ISSET(ply_ptr->parent_rom, RPHARM) && (!ill)) 
	{
		/* call standard healing routine */
		heal_crt(ply_ptr );
	}

/* handle poison  */

	else if(!F_ISSET(ply_ptr->parent_rom, RPHARM) && ill && (i<t)) {
	   if(F_ISSET(ply_ptr, PPOISN) && ply_ptr->class != DRUID){
		ANSI(ply_ptr->fd, AM_BLINK);
		ANSI(ply_ptr->fd, POISONCOLOR);
		output(ply_ptr->fd, "Poison courses through your veins.\n");
		ply_ptr->hpcur -= MAX(1,mrand(1,4)-bonus[(int)ply_ptr->constitution]);
		ply_ptr->lasttime[LT_HEALS].ltime = t;
		ply_ptr->lasttime[LT_HEALS].interval = 65 + 5*bonus[(int)ply_ptr->constitution];
		ANSI(ply_ptr->fd, AFC_WHITE);
		ANSI(ply_ptr->fd, AM_NORMAL);
		if(ply_ptr->hpcur < 1)
			die(ply_ptr, ply_ptr);
	   }
	   else if(F_ISSET(ply_ptr, PPOISN) && ply_ptr->class == DRUID)
			F_CLR(ply_ptr, PPOISN);
		
	   if (F_ISSET(ply_ptr, PDISEA)){
		ANSI(ply_ptr->fd, AM_BLINK);
		ANSI(ply_ptr->fd, DISEASECOLOR);
		output(ply_ptr->fd, "Fever grips your mind.\n");
        ply_ptr->lasttime[LT_ATTCK].ltime = time(0);
		ANSI(ply_ptr->fd, AFC_BLUE);
		output(ply_ptr->fd, "You feel nauseous.\n");
		ply_ptr->lasttime[LT_ATTCK].interval= dice(1,6,3);
		ply_ptr->hpcur -= MAX(1,mrand(5,12)-bonus[(int)ply_ptr->constitution]);

		if(ply_ptr->hpcur > ply_ptr->hpmax) {
			ply_ptr->hpcur = ply_ptr->hpmax;
		}

		ply_ptr->lasttime[LT_HEALS].ltime = t;
		ply_ptr->lasttime[LT_HEALS].interval = 65 + 5*bonus[(int)ply_ptr->constitution];
		ANSI(ply_ptr->fd, MAINTEXTCOLOR);
		ANSI(ply_ptr->fd, AM_NORMAL);
		if(ply_ptr->hpcur < 1)
			die(ply_ptr, ply_ptr);
	   }
	}


/* handle player harm rooms */
else if(i<t) {

if (F_ISSET(ply_ptr->parent_rom, RPPOIS))
{
  ANSI(ply_ptr->fd, AM_BLINK);
  ANSI(ply_ptr->fd, POISONCOLOR);
  output(ply_ptr->fd, "The toxic air poisoned you.\n");
  ANSI(ply_ptr->fd, MAINTEXTCOLOR);
  ANSI(ply_ptr->fd, AM_NORMAL);
  F_SET(ply_ptr, PPOISN);    
}

if (F_ISSET(ply_ptr->parent_rom, RPBEFU)) 
{
	output(ply_ptr->fd, "The room starts to spin around you.\nYou feel confused.\n");
        ply_ptr->lasttime[LT_ATTCK].ltime = time(0);
        ply_ptr->lasttime[LT_ATTCK].interval= MAX(dice(2,6,0),6);
}

if (F_ISSET(ply_ptr, PPOISN)){
	ANSI(ply_ptr->fd, AM_BLINK);
	ANSI(ply_ptr->fd, AFC_RED);
  	output(ply_ptr->fd, "Poison courses through your veins.\n");
	ANSI(ply_ptr->fd, AM_NORMAL);
	ANSI(ply_ptr->fd, AFC_WHITE);
	ply_ptr->hpcur -= MAX(1,mrand(1,6)-bonus[(int)ply_ptr->constitution]);
  	if(ply_ptr->hpcur < 1) die(ply_ptr, ply_ptr);
}
if (F_ISSET(ply_ptr, PDISEA)){
			ANSI(ply_ptr->fd, AM_BLINK);
			ANSI(ply_ptr->fd, DISEASECOLOR);
        output(ply_ptr->fd, "Fever grips your mind.\n");
        ply_ptr->lasttime[LT_ATTCK].ltime = time(0);
			ANSI(ply_ptr->fd, AFC_BLUE);
        output(ply_ptr->fd, "You feel nauseous.\n");
        ply_ptr->lasttime[LT_ATTCK].interval= dice(1,6,3);
        ply_ptr->hpcur -= MAX(1,mrand(1,12)-bonus[(int)ply_ptr->constitution]);
        ply_ptr->lasttime[LT_HEALS].ltime = t;
        ply_ptr->lasttime[LT_HEALS].interval = 65 + 
                            5*bonus[(int)ply_ptr->constitution];
			ANSI(ply_ptr->fd, AM_NORMAL);
			ANSI(ply_ptr->fd, MAINTEXTCOLOR);
        if(ply_ptr->hpcur < 1)
                die(ply_ptr, ply_ptr);
}                                   

if (F_ISSET(ply_ptr->parent_rom,RPMPDR))
 	ply_ptr->mpcur -= MIN(ply_ptr->mpcur,6); 
else if (!ill) {
  ply_ptr->mpcur += MAX(1, 2+(ply_ptr->intelligence > 17 ? 1:0)+
                                ((ply_ptr->class == MAGE || ply_ptr->class ==ALCHEMIST) ? 2:0));
		ply_ptr->mpcur = MIN(ply_ptr->mpcur, ply_ptr->mpmax);
}
    
if (F_ISSET(ply_ptr->parent_rom, RFIRER) && !F_ISSET(ply_ptr,PRFIRE)) {
    	output(ply_ptr->fd, "The searing heat burns your flesh.\n");
	prot = 0;
}
else if (F_ISSET(ply_ptr->parent_rom, RWATER) && !F_ISSET(ply_ptr,PBRWAT)) {
	output(ply_ptr->fd, "Water fills your lungs.\n");
        prot = 0;
}
else if (F_ISSET(ply_ptr->parent_rom, REARTH) && !F_ISSET(ply_ptr,PSSHLD)) {
	output(ply_ptr->fd, "The earth swells up around you and smothers you.\n");
        prot = 0;
}
else if (F_ISSET(ply_ptr->parent_rom, RWINDR) && !F_ISSET(ply_ptr,PRCOLD)) {
	ANSI(ply_ptr->fd, AFC_BLUE);
	output(ply_ptr->fd, "The freezing air chills you to the bone.\n");
	ANSI(ply_ptr->fd, AFC_WHITE);
	prot = 0;
}
else  if(!F_ISSET(ply_ptr->parent_rom, RWINDR) &&
	 !F_ISSET(ply_ptr->parent_rom, REARTH) &&
	 !F_ISSET(ply_ptr->parent_rom, RFIRER) &&
	 !F_ISSET(ply_ptr->parent_rom, RWATER) &&
	 !F_ISSET(ply_ptr->parent_rom, RPPOIS) &&
	 !F_ISSET(ply_ptr->parent_rom, RPBEFU) &&
	 !F_ISSET(ply_ptr->parent_rom, RPMPDR)) {
			ANSI(ply_ptr->fd, AM_BOLD);
			ANSI(ply_ptr->fd, AFC_MAGENTA);
	output(ply_ptr->fd, "An invisible force saps your life force.\n");
			ANSI(ply_ptr->fd, AM_NORMAL);
			ANSI(ply_ptr->fd, AFC_WHITE);
	prot = 0;
}
	if (!prot) {
		ply_ptr->hpcur -= 15 - MIN(bonus[(int)ply_ptr->constitution],2);
        	if(ply_ptr->hpcur < 1) die(ply_ptr, ply_ptr);      
	}
	else if (!ill)
	{					

/*    tick bonus for hurt room - see update.c for real tick code (increase for barbs from 2 to 5 * con bonus */

		ply_ptr->hpcur += MAX(1, 3 + bonus[(int)ply_ptr->constitution] +
				(ply_ptr->class == BARBARIAN ? (5 * bonus[(int)ply_ptr->constitution]):0));
		if (F_ISSET(ply_ptr, PBESRK) ) 
			ply_ptr->hpcur = MIN(ply_ptr->hpcur, ply_ptr->hpmax + ply_ptr->level);
		else
			ply_ptr->hpcur = MIN(ply_ptr->hpcur, ply_ptr->hpmax);
	}


ply_ptr->lasttime[LT_HEALS].ltime = t;
ply_ptr->lasttime[LT_HEALS].interval = 45 +  5*bonus[(int)ply_ptr->piety];

}
/*******************************/


	if(t > LT(ply_ptr, LT_PSAVE)) {
		ply_ptr->lasttime[LT_PSAVE].ltime = t;
		/* use save_ply_cmd here to show player game was saved */
		/* normally we might call save_ply */
		save_ply_cmd(ply_ptr, 0);
	}

	item = has_light(ply_ptr);
	if(item && item != MAXWEAR+1) {
		if(ply_ptr->ready[item-1]->type == LIGHTSOURCE)
			if(--(ply_ptr->ready[item-1]->shotscur) < 1) {
				sprintf(g_buffer, "Your %s died out.\n",
				      ply_ptr->ready[item-1]->name);
				output(ply_ptr->fd, g_buffer );

				sprintf(g_buffer,"%%M's %s died out.", 
					ply_ptr->ready[item-1]->name);

				broadcast_rom(ply_ptr->fd, ply_ptr->rom_num,
					      g_buffer, m1arg(ply_ptr));
			}
	}

}

/**********************************************************************/
/*				up_level			      */
/**********************************************************************/

/* This function should be called whenever a player goes up a level.  */
/* It raises her hit points and magic points appropriately, and if    */
/* it is initializing a new character, it sets up the character.      */

void up_level( creature	*ply_ptr )
{
	int	index,i;


	for(i=0;i<MAXWEAR;i++) {
		if(ply_ptr->ready[i] && F_ISSET(ply_ptr->ready[i], OADDSA) && 
					ply_ptr->ready[i]->constitution != 0) 
			ply_ptr->constitution -= ply_ptr->ready[i]->constitution;
	}

	ply_ptr->level++;
	if(ply_ptr->level == 1) {
		ply_ptr->hpmax += class_stats[(int)ply_ptr->class].hpstart;
		ply_ptr->mpmax += class_stats[(int)ply_ptr->class].mpstart;

/*

Basically, if it's above 17, give them an extra point per
level.  Below 7, take one point away per level.

*/

	       	if(ply_ptr->constitution > 17) {
			ply_ptr->hpmax++;
		}
       		else if(ply_ptr->constitution < 7) {
			ply_ptr->hpmax--;
			}
		ply_ptr->hpcur = ply_ptr->hpmax;
		ply_ptr->mpcur = ply_ptr->mpmax;
		ply_ptr->ndice = class_stats[(int)ply_ptr->class].ndice;
		ply_ptr->sdice = class_stats[(int)ply_ptr->class].sdice;
		ply_ptr->pdice = class_stats[(int)ply_ptr->class].pdice;
	}
	else {
	        ply_ptr->hpmax += class_stats[(int)ply_ptr->class].hp;
        	ply_ptr->mpmax += class_stats[(int)ply_ptr->class].mp;

		index = (ply_ptr->level-2) % 10;
		switch(level_cycle[(int)ply_ptr->class][index]) {
		case STR: ply_ptr->strength++; break;
		case DEX: ply_ptr->dexterity++; break;
		case CON: ply_ptr->constitution++; break;
		case INTELLIGENCE: ply_ptr->intelligence++; break;
		case PTY: ply_ptr->piety++; break;
		}

/*

Basically, if it's above 17, give them an extra point per
level.  Below 7, take one point away per level.

*/
	       	if(ply_ptr->constitution > 17) {
			ply_ptr->hpmax++;
		}
       		else if(ply_ptr->constitution < 7) {
			ply_ptr->hpmax--;
		}

		if(F_ISSET(ply_ptr, PBESRK)) 
			ply_ptr->hpmax++;
		
	}

	/* MONK CODE new monk code with bug fixed JPF */
	set_monk_dice( ply_ptr );

	for(i=0;i<MAXWEAR;i++) {
		if(ply_ptr->ready[i] && F_ISSET(ply_ptr->ready[i], OADDSA) && 
				ply_ptr->ready[i]->constitution != 0) 
			ply_ptr->constitution += ply_ptr->ready[i]->constitution;
	}

}

/**********************************************************************/
/*				down_level			      */
/**********************************************************************/

/* This function is called when a player loses a level due to dying or */
/* for some other reason.  The appropriate stats are downgraded.       */
void down_level( creature *ply_ptr )
{
	int	index,i;

	for(i=0;i<MAXWEAR;i++) {
		if(ply_ptr->ready[i] && F_ISSET(ply_ptr->ready[i], OADDSA) && 
				ply_ptr->ready[i]->constitution != 0) 
			ply_ptr->constitution -= ply_ptr->ready[i]->constitution;
	}

	ply_ptr->level--;

	ply_ptr->hpmax -= class_stats[(int)ply_ptr->class].hp;
        ply_ptr->mpmax -= class_stats[(int)ply_ptr->class].mp;

	ply_ptr->hpcur = ply_ptr->hpmax;
	ply_ptr->mpcur = ply_ptr->mpmax;

/*

Basically, if it's above 17, give them an extra point per
level.  Below 7, take one point away per level.

*/
        if(ply_ptr->constitution > 17) {
		ply_ptr->hpmax--;
	}
        else if(ply_ptr->constitution < 7) {
		ply_ptr->hpmax++;
		}

	index = (ply_ptr->level-1) % 10;
	switch(level_cycle[(int)ply_ptr->class][index]) {
	case STR: ply_ptr->strength--; break;
	case DEX: ply_ptr->dexterity--; break;
	case CON: ply_ptr->constitution--; break;
	case INTELLIGENCE: ply_ptr->intelligence--; break;
	case PTY: ply_ptr->piety--; break;
	}

	if(F_ISSET(ply_ptr, PBESRK))
		ply_ptr->hpmax--;

	/* MONK CODE new monk code with bug fixed JPF */
	set_monk_dice( ply_ptr );

	for(i=0;i<MAXWEAR;i++) {
		if(ply_ptr->ready[i] && F_ISSET(ply_ptr->ready[i], OADDSA) && 
				ply_ptr->ready[i]->constitution != 0) 
			ply_ptr->constitution += ply_ptr->ready[i]->constitution;
	}

}

/**********************************************************************/
/*				add_obj_crt			      */
/**********************************************************************/

/* This function adds the object pointer to by the first parameter to */
/* the inventory of the player pointed to by the second parameter.    */

void add_obj_crt(object	*obj_ptr, creature *ply_ptr )
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

void del_obj_crt(object *obj_ptr, creature *ply_ptr )
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
/*				compute_ac			      */
/**********************************************************************/

/* This function computes a player's (or a monster's) armor class by  */
/* examining its stats and the items it is holding.		      */

void compute_ac(creature *ply_ptr )
{
	int	ac, i;

	ac = 100;


	for(i=0; i<MAXWEAR; i++)
		if(ply_ptr->ready[i])
			ac -= ply_ptr->ready[i]->armor;

	ac -= 5*bonus[(int)ply_ptr->dexterity];

	if(ply_ptr->class == MONK) {
		/* cap monks at certain ac value */
		ac = MAX(-70, (100 - ply_ptr->level*6 -
			7*bonus[(int)ply_ptr->dexterity]));
	}

	if(F_ISSET(ply_ptr, PPROTE))
		ac -= 10;

	if(F_ISSET(ply_ptr, PBARKS))
		ac -= 30;

	ac = MAX(-127,MIN(127,ac));

	ply_ptr->armor = ac;
}

/**********************************************************************/
/*				compute_thaco			      */
/**********************************************************************/

/* This function computes a player's THAC0 by looking at his class, */
/* level, weapon adjustment and strength bonuses.		    */

void compute_thaco( creature *ply_ptr )
{
	int	thaco, n;

	n = ply_ptr->level > 20 ? 19:ply_ptr->level-1;

	thaco = thaco_list[(int)ply_ptr->class][n];
	if(ply_ptr->ready[WIELD-1])
		thaco -= ply_ptr->ready[WIELD-1]->adjustment;
	thaco -= mod_profic(ply_ptr);
 	thaco -= bonus[(int)ply_ptr->strength];

	if(F_ISSET(ply_ptr, PBLESS))
		thaco -= 1;

	ply_ptr->thaco = MAX(0, thaco);

}

/**********************************************************************/
/*				mod_profic			      */
/**********************************************************************/

/* This function returns the player's currently-used proficiency div 20 */

int mod_profic( creature *ply_ptr )
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
		ply_ptr->ready[WIELD-1]->type <= MISSILE) {
			return(profic(ply_ptr, ply_ptr->ready[WIELD-1]->type)/amt);
	}
	else {
		return(profic(ply_ptr, HAND)/amt);
	}
}

/**********************************************************************/
/*				weight_ply			      */
/**********************************************************************/

/* This function calculates the total weight that a player (or monster) */
/* is carrying in his inventory.					*/

int weight_ply( creature *ply_ptr )
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

int max_weight( creature *ply_ptr )
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

int profic( creature *ply_ptr, int index )
{
	long	prof_array[12];
	int	i, n, prof;

	switch (ply_ptr->class){
	case FIGHTER:
		prof_array[0] = 0L;			prof_array[1] = 768L;
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
		prof_array[0] = 0L;			prof_array[1] = 768L;
		prof_array[2] = 1024L;		prof_array[3] = 1800L;
		prof_array[4] = 2388L;		prof_array[5] = 20000L;
		prof_array[6] = 37768L;		prof_array[7] = 205000L;
		prof_array[8] = 325610L;	prof_array[9] = 895000L;
		prof_array[10] = 1549760L;
		prof_array[11] = 500000000L;
		break;

	case BARD:
	case THIEF:
	case ASSASSIN:
	case BUILDER:
		prof_array[0] = 0L;			prof_array[1] = 768L;
		prof_array[2] = 1024L;		prof_array[3] = 1800L;
		prof_array[4] = 2388L;		prof_array[5] = 20000L;
		prof_array[6] = 42768L;		prof_array[7] = 220000L;
		prof_array[8] = 355610L;	prof_array[9] = 950000L;
		prof_array[10] = 1949760L;
		prof_array[11] = 500000000L;
		break;

	case ALCHEMIST:
	case DRUID:
	case CLERIC:
		prof_array[0] = 0L;         prof_array[1] = 768L;
        prof_array[2] = 1024L;      prof_array[3] = 1800L;
        prof_array[4] = 2388L;      prof_array[5] = 22000L;
        prof_array[6] = 47768L;     prof_array[7] = 215000L;
        prof_array[8] = 370610L;    prof_array[9] = 975000L;
        prof_array[10] = 2044760L;
        prof_array[11] = 500000000L;
        break;

	case MAGE:
		prof_array[0] = 0L;			prof_array[1] = 768L;
		prof_array[2] = 1024L;		prof_array[3] = 1800L;
		prof_array[4] = 2388L;		prof_array[5] = 25000L;
		prof_array[6] = 50768L;		prof_array[7] = 215000L;
		prof_array[8] = 385610L;	prof_array[9] = 1005000L;
		prof_array[10] = 2344760L;
		prof_array[11] = 500000000L;
		break;
	default:
		return 0;
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

int mprofic( creature *ply_ptr, int	index )
{
	long	prof_array[12];
	int	i, n, prof;

        switch(ply_ptr->class){
        case MAGE:
		case ALCHEMIST:
			prof_array[0] = 0L;        prof_array[1] = 1024L;
            prof_array[2] = 2048L;     prof_array[3] = 4096L;
            prof_array[4] = 8192L;     prof_array[5] = 16384L;
            prof_array[6] = 35768L;    prof_array[7] = 85536L;
            prof_array[8] = 140000L;   prof_array[9] = 459410L;
            prof_array[10] = 2073306L; prof_array[11] = 500000000L;
            break;
        case CLERIC:
		case DRUID:
		case BARD:
			prof_array[0] = 0L;        prof_array[1] = 1024L;
			prof_array[2] = 4092L;     prof_array[3] = 8192L;
		    prof_array[4] = 16384L;    prof_array[5] = 32768L;
	        prof_array[6] = 70536L;    prof_array[7] = 119000L;
			prof_array[8] = 226410L;   prof_array[9] = 709410L;
		    prof_array[10] = 2973307L; prof_array[11] = 500000000L;
	        break;
        case PALADIN:
        case MONK:
		case RANGER:    
            prof_array[0] = 0L;        prof_array[1] = 1024L;
            prof_array[2] = 8192L;     prof_array[3] = 16384L;
            prof_array[4] = 32768L;    prof_array[5] = 65536L;
            prof_array[6] = 105000L;   prof_array[7] = 165410L;
            prof_array[8] = 287306L;   prof_array[9] = 809410L;
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

int fall_ply( creature *ply_ptr )
{
	int	fall, j;

	fall = bonus[(int)ply_ptr->dexterity]*5;
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

creature *find_who( char *name )
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

creature *lowest_piety(room	*rom_ptr, int invis )
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

int has_light( creature *crt_ptr )
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
/* NOTE: we cannot use any functions that will call print() in here	*/
/* or we will end up making a prompt that keeps displaying forever	*/
/************************************************************************/

/* This function returns the prompt that the player should be seeing	*/

void ply_prompt( creature *ply_ptr )
{
	char prompt[40];
	char	temp[25];
	int fd;

	fd = ply_ptr->fd;

	/* maybe still logging in */
	if ( fd < 0 )
		return;


	/* start with nothing */
	prompt[0] = '\0';


	if( F_ISSET(ply_ptr, PSPYON) || F_ISSET(ply_ptr, PREADI))
	{
		/* no prompt in this situation */
		return;
	}

	if( F_ISSET(Ply[fd].ply, PANSIC ) )
	{
		sprintf(temp, "%c[%dm", 27, PROMPTCOLOR);
	}
	else
	{
		temp[0] = '\0';
	}


	if(F_ISSET(ply_ptr, PPROMP) || F_ISSET(ply_ptr, PALIAS))
	{
		if(F_ISSET(ply_ptr, PALIAS) && Ply[fd].extr->alias_crt) {
			if(F_ISSET(ply_ptr, PAFK)) {
				sprintf(prompt, "%s(%d H %d M):[AFK] ", temp, 
					Ply[fd].extr->alias_crt->hpcur, Ply[fd].extr->alias_crt->mpcur);
			}
			
			else {
				sprintf(prompt, "%s(%d H %d M): ", temp, Ply[fd].extr->alias_crt->hpcur,
					Ply[fd].extr->alias_crt->mpcur);
			}
		}
		else {
			if(F_ISSET(ply_ptr, PAFK)) {
				sprintf(prompt, "%s(%d H %d M):[AFK] ", temp, ply_ptr->hpcur,
				ply_ptr->mpcur);
			}
			else {
				sprintf(prompt, "%s(%d H %d M): ", temp, ply_ptr->hpcur, ply_ptr->mpcur);
			}
		}
	}
	else {
		sprintf( prompt, "%s:", temp );
	}

	/* resotre normal color */
	if( F_ISSET(Ply[fd].ply, PANSIC ) )
	{
		sprintf(temp, "%c[%dm", 27, MAINTEXTCOLOR);
		strcat( prompt, temp );
	}

	spy_write(fd, prompt, strlen(prompt) );
	
	return ;
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

creature *low_piety_alg(room *rom_ptr, int invis, int alg, int lvl )
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

int luck( creature *ply_ptr )
{
	int		num, alg, con, smrt;
	
	if(!ply_ptr)
		return(0);
	if(ply_ptr->type != PLAYER)
		return(0);

	alg = abs(ply_ptr->alignment);
	alg = alg+1;
    alg = (int)(alg/10);

	if(!alg || (ply_ptr->class == PALADIN && ply_ptr->alignment > 0))
		alg = 1;
	con = ply_ptr->constitution;
	smrt = ply_ptr->intelligence;

	num = 100*(smrt+con);
	num /= alg;

	if(ply_ptr->ready[HELD-1] && F_ISSET(ply_ptr->ready[HELD-1], OLUCKY)) 
		num += ply_ptr->ready[HELD-1]->ndice;

	if(num>99)
		num=99;
	if(num<1)
		num=1;
	
	Ply[ply_ptr->fd].extr->luck = num;

	return(num);
}



/****************************************************************/
/*			set_monk_dice				*/
/****************************************************************/
void set_monk_dice(creature *ply_ptr)
{
	int	nLevel;

	ASSERTLOG( ply_ptr );

	/* reset monk dice? */
    if(ply_ptr->class == MONK) {
		nLevel = MAX(0, MIN(ply_ptr->level, 25));

        ply_ptr->ndice = monk_dice[nLevel].nDice;
        ply_ptr->sdice = monk_dice[nLevel].nSides;
        ply_ptr->pdice = monk_dice[nLevel].nPlus;
	}

	return;
}




/****************************************************************/
/*			validate_player				*/
/*	Heres where we check that everythign is kosher		*/
/****************************************************************/
void validate_player(creature *ply_ptr)
{

	int i;

	ASSERTLOG( ply_ptr );


	/* only players and builders are affected */
	if( ply_ptr->class < CARETAKER )
	{
		/* players cant set eavesdropper flag anymore */
		if ( F_ISSET(ply_ptr, PEAVES ) )
		{
			F_CLR(ply_ptr, PEAVES );
		}

		/* reset monk dice? */
		/* to fix old bugs or to reset the dice table */
		set_monk_dice( ply_ptr );


		/* other things that could be done */
		/* check for illegal spells or items */
		/* check for boosted stats */
		/* fix things like the monk bug */
		/* etc */

	}

	for(i=get_spell_list_size()-1;i<128;i++)
		S_CLR(ply_ptr, i);

	/* sanitize new structures (just in case) */
		
	if(!ply_ptr->bank_balance || ply_ptr->bank_balance < 0)
		ply_ptr->bank_balance = 0;

	/* check for illegal pledges */
	/* if(ISENGARD) {
		if(!(ply_ptr->class == FIGHTER || ply_ptr->class == ASSASSIN || 
			ply_ptr->class == THIEF || ply_ptr->class == PALADIN) 
			&& F_ISSET(ply_ptr, PPLDGK)) 
		{
			F_CLR(ply_ptr, PPLDGK);
			sprintf(g_buffer, "Cleared illegal PPLDGK flag on %s.", ply_ptr->name);
			elog_broad( g_buffer );
		}
	} 
	*/
	F_CLR(ply_ptr, PROBOT);

	return;
}


/****************************************************************/
/*			check_for_spam				*/
/*	RETURNS: 1 if spam 0 if no spam				*/
/****************************************************************/
int check_for_spam( creature *ply_ptr )
{

	time_t		t;

	t = time(0);
	if ( ply_ptr->class < CARETAKER )
	{
		if(ply_ptr->lasttime[LT_PSEND].ltime == t) 
		{
			/* 4 in one second is spam */
			if(++ply_ptr->lasttime[LT_PSEND].misc > 3) 
			{
				silence_spammer( ply_ptr );
				return(1);
			}
		}
		else if ( ply_ptr->lasttime[LT_PSEND].ltime + 1 == t )
		{
			/* 6 in 2 seconds is spam */
			if(++ply_ptr->lasttime[LT_PSEND].misc > 5) 
			{
				silence_spammer( ply_ptr );
				return(1);
			}
		}
		else if ( ply_ptr->lasttime[LT_PSEND].ltime + 2 == t )
		{
			/* 7 in 3 seconds is spam */
			if(++ply_ptr->lasttime[LT_PSEND].misc > 6 ) 
			{
				silence_spammer( ply_ptr );
				return(1);
			}
		}
		else
		{
			/* reset spam counter */
			ply_ptr->lasttime[LT_PSEND].ltime = t;
			ply_ptr->lasttime[LT_PSEND].misc = 1;
		}
	}


	return(0 );
}


/**********************************************************************/
/*				silence_spammer			      */
/**********************************************************************/
void silence_spammer( creature *ply_ptr )
{
	ASSERTLOG( ply_ptr );
	
	if ( ply_ptr )
	{
		if( F_ISSET( ply_ptr, PSILNC ) )
		{
			/* already spamming */
			output_wc( ply_ptr->fd, 
				"You have been given an additional 5 minutes of silence for spamming again!\n", 
				AFC_RED);

			ply_ptr->lasttime[LT_SILNC].interval += 300;
		}
		else
		{
			/* first spam */
			ply_ptr->lasttime[LT_SILNC].ltime = time(0);

			/* 2 minutes for spammers */
			ply_ptr->lasttime[LT_SILNC].interval = 120;

			F_SET(ply_ptr,PSILNC);

			output_wc( ply_ptr->fd, 
				"You have been silenced for 2 minutes for spamming!\n", 
				AFC_RED);

			sprintf(g_buffer, "%s has been silenced for spamming!\n",
				ply_ptr->name);

			broadcast_rom(ply_ptr->fd, ply_ptr->rom_num,
				g_buffer, NULL);
		}
	}

	return;

}

/**********************************************************************/
/*				save_all_ply			      */
/**********************************************************************/

/* This function saves all players currently in memory.		      */

void save_all_ply()
{
	int i;

	for(i=0; i<Tablesize; i++) {
		if(Ply[i].ply && Ply[i].io && Ply[i].ply->name[0])
			save_ply(Ply[i].ply );
	}
}

