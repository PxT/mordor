/*
 * DM1.C:
 *
 *	DM functions
 *
 *	Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"
#ifdef DMALLOC
  #include "/usr/local/include/dmalloc.h"
#endif

/**********************************************************************/
/*				dm_teleport			      */
/**********************************************************************/

/* This function allows DM's to teleport to a given room number, or to */
/* a player's location.  It will also teleport a player to the DM or   */
/* one player to another.					       */
 
int dm_teleport(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	creature	*crt_ptr;
	ctag		*cp;
	creature	*crt_ptr2;
	room		*rom_ptr;

	if(ply_ptr->class < CARETAKER)
		return(PROMPT);
	if(cmnd->num < 2) {
		if(cmnd->val[0] >= RMAX) return(0);
		if(load_rom(cmnd->val[0], &rom_ptr) < 0) {
			print(ply_ptr->fd, "Error (%d)\n", cmnd->val[0]);
			return(0);
		}
/* OLD DM-FOLLOW CODE */
/*
	cp = ply_ptr->first_fol;
        while(cp) {
                if(F_ISSET(cp->crt, MDMFOL) && cp->crt->parent_rom == ply_ptr->parent_rom) {
                del_crt_rom(cp->crt, ply_ptr->parent_rom);
		broadcast_rom(ply_ptr->fd, ply_ptr->rom_num,"%M just wandered away.\n", cp->crt);
                add_crt_rom(cp->crt, rom_ptr, 1);
                add_active(cp->crt); 
                }
                cp = cp->next_tag;
        }*/
	    if(F_ISSET(ply_ptr, PALIAS)) {
		del_crt_rom(Ply[ply_ptr->fd].extr->alias_crt, ply_ptr->parent_rom);
		broadcast_rom(ply_ptr->fd, ply_ptr->rom_num,"%M just wandered away.", Ply[ply_ptr->fd].extr->alias_crt);
		add_crt_rom(Ply[ply_ptr->fd].extr->alias_crt, rom_ptr, 1);
	    }
		del_ply_rom(ply_ptr, ply_ptr->parent_rom);
		add_ply_rom(ply_ptr, rom_ptr);
	}

	else if(cmnd->num < 3) {
		lowercize(cmnd->str[1], 1);
		crt_ptr = find_who(cmnd->str[1]);
		if(!crt_ptr || crt_ptr == ply_ptr || (crt_ptr->class == DM &&
		   ply_ptr->class < DM && F_ISSET(crt_ptr, PDMINV))) {
			print(ply_ptr->fd, "%s is not on.\n", cmnd->str[1]);
			return(0);
		}
/* OLD DM-FOLLOW CODE */
/*
	cp = ply_ptr->first_fol;
        while(cp) {
                if(F_ISSET(cp->crt, MDMFOL) && cp->crt->parent_rom == ply_ptr->parent_rom) {
                del_crt_rom(cp->crt, ply_ptr->parent_rom);
		broadcast_rom(ply_ptr->fd, ply_ptr->rom_num,"%M just wandered away.\n", cp->crt);
	        add_crt_rom(cp->crt, crt_ptr->parent_rom, 1);
                }
                cp = cp->next_tag;
        } */
	    if(F_ISSET(ply_ptr, PALIAS)) {
                del_crt_rom(Ply[ply_ptr->fd].extr->alias_crt, ply_ptr->parent_rom);
                broadcast_rom(ply_ptr->fd, ply_ptr->rom_num,"%M just wandered away.", Ply[ply_ptr->fd].extr->alias_crt);                
		add_crt_rom(Ply[ply_ptr->fd].extr->alias_crt, crt_ptr->parent_rom, 1);
            }

		del_ply_rom(ply_ptr, ply_ptr->parent_rom);
		add_ply_rom(ply_ptr, crt_ptr->parent_rom);
	}

	else {
		lowercize(cmnd->str[1], 1);
		crt_ptr = find_who(cmnd->str[1]);
		if(!crt_ptr || crt_ptr == ply_ptr || (ply_ptr->class < DM &&
		   crt_ptr->class == DM && F_ISSET(crt_ptr, PDMINV))) {
			print(ply_ptr->fd, "%s is not on.\n", cmnd->str[1]);
			return(0);
		}
		lowercize(cmnd->str[2], 1);
		if(*cmnd->str[2] == '.')
			crt_ptr2 = ply_ptr;
		else
			crt_ptr2 = find_who(cmnd->str[2]);
		if(!crt_ptr2) {
			print(ply_ptr->fd, "%s is not on.\n", cmnd->str[1]);
			return(0);
		}
		del_ply_rom(crt_ptr, crt_ptr->parent_rom);
		add_ply_rom(crt_ptr, crt_ptr2->parent_rom);
	}	

	return(0);
}

/**********************************************************************/
/*				dm_send				      */
/**********************************************************************/

/* This function allows DMs to send messages that only they can see. */
/* It is similar to a broadcast, but there are no limits.	     */

int dm_send(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	char	str[450];
	int	i, fd, found = 0;

	if(ply_ptr->class < CARETAKER)
		return(PROMPT);

	fd = ply_ptr->fd;

	for(i=0; i<strlen(cmnd->fullstr); i++) {
		if(cmnd->fullstr[i] == ' ' && cmnd->fullstr[i+1] != ' ') {
			found++;
			break;
		}
	}

	if(found < 1 || strlen(&cmnd->fullstr[i+1]) < 1) {
		print(fd, "*Send what?\n");
		return(0);
	}

	print(fd, "Ok.\n");
	broadcast_wiz("+++ %s sent, \"%s\".", ply_ptr->name, 
		      &cmnd->fullstr[i+1]);

	return(0);

}

/**********************************************************************/
/*				dm_purge			      */
/**********************************************************************/

/* This function allows DMs to purge a room of all its objects and    */
/* monsters.							      */

int dm_purge(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	ctag	*cp, *fol, *ctemp, *folprev;
	otag	*op, *otemp;
	room	*rom_ptr;
	int	fd;

	if(ply_ptr->class < CARETAKER)
		return(PROMPT);

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	cp = rom_ptr->first_mon;
	rom_ptr->first_mon = 0;
	while(cp) {
		ctemp = cp->next_tag;
		if(F_ISSET (cp->crt, MDMFOL)) { /* clear relevant follow lists */
                	F_CLR(cp->crt->following, PALIAS);
			Ply[cp->crt->following->fd].extr->alias_crt = 0;
                	print (cp->crt->following->fd, "%1M's soul was purged.\n", cp->crt);
			fol = cp->crt->following->first_fol;
                	if(fol->crt == cp->crt) { 
                        	cp->crt->following->first_fol = fol->next_tag;
				free(fol); 
			}
			else {
			  while(fol) {	
				if(fol->crt == cp->crt) {
					folprev == fol->next_tag;
        				free(fol);
					break;
				}
				folprev = fol;
				fol = fol->next_tag;
			  }
			}
		}
			free_crt(cp->crt);
			free(cp);
		
		cp = ctemp;
	}

	op = rom_ptr->first_obj;
	rom_ptr->first_obj = 0;
	while(op) {
		otemp = op->next_tag;
		if(!F_ISSET(op->obj, OTEMPP)) {
			free_obj(op->obj);
			free(op);
		}
		op = otemp;
	}

	print(fd, "Purged.\n");
	return(0);

}

/**********************************************************************/
/*				dm_users			      */
/**********************************************************************/

/* This function allows DM's to list all users online, displaying */
/* level, name, current room # and name, and foreign address.     */
 
int dm_users(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	char	str[15], idstr[50];
	long	t;
	int	fd, i, j, userid = 0, fulluser = 0;

	if(ply_ptr->class < CARETAKER)
		return(PROMPT);

	if(cmnd->num > 1) {
		if(cmnd->str[1][0] == 'u') userid = 1;
		if(cmnd->str[1][0] == 'f') fulluser = 1;
	}

	t = time(0);
	fd = ply_ptr->fd;
	ANSI(fd, BLUE);
	if(fulluser){
		print(fd, "%-9s %-10s %-52s", "Lev  Clas", " Player", " Email address");
	}
	else
		print(fd, "%-9s %-10s %-20s %-15s %-15s", "Lev  Clas", " Player", "Room #: Name", userid ? "UserID" : "Address", "Last command");
	print(fd, " Idle\n");
	print(fd, "-------------------------------------------------------------------------------\n");
	ANSI(fd, WHITE);
	for(i=0; i<Tablesize; i++) {
		if(!Ply[i].ply) continue;
		if(Ply[i].ply->fd < 0) continue;
		if(Ply[i].ply->class == DM && ply_ptr->class < CARETAKER &&
		   F_ISSET(Ply[i].ply, PDMINV)) continue;
		print(fd, "[%2d] ", Ply[i].ply->level);
		print(fd, "%-4.4s ", class_str[Ply[i].ply->class]);
		if(!F_ISSET(Ply[i].ply, PSECOK)){
			 ANSI(fd, RED);
		}
		else {
			 ANSI(fd, YELLOW);
		}
		print(fd, "%s%-10.10s ", 
		      (F_ISSET(Ply[i].ply, PDMINV) || 
		      F_ISSET(Ply[i].ply, PINVIS)) ? "*":" ",
			Ply[i].ply->name);
		ANSI(fd, WHITE);
		if(fulluser) {
			sprintf(idstr, "%s@%s", Ply[i].io->userid,
				Ply[i].io->address);
			if(!strcmp(Ply[i].io->userid, "no_port") || !strcmp(Ply[i].io->userid, "unknown"))
				ANSI(fd, MAGENTA);
			print(fd, "%-51.51s ", idstr);
		}
		else {
			ANSI(fd, WHITE);
			print(fd, "%5hd: ", Ply[i].ply->rom_num);
			ANSI(fd, BLUE);
			print(fd, "%-12.12s ", Ply[i].ply->parent_rom->name);
			ANSI(fd, CYAN);
			print(fd, "%-15.15s ", userid ? Ply[i].io->userid : 
				Ply[i].io->address);
			ANSI(fd, GREEN);
			print(fd, "%-15.15s ", Ply[i].extr->lastcommand);
		}
		ANSI(fd, WHITE);
		print(fd, "%02ld:%02ld\n", (t-Ply[i].io->ltime)/60L, 
		      (t-Ply[i].io->ltime)%60L);
	}
	ANSI(fd, WHITE);
	ANSI(fd, NORMAL);
	print(fd, "\n");

	return(0);

}

/**********************************************************************/
/*				dm_echo				      */
/**********************************************************************/

/* This function allows a DM specified by the socket descriptor in */
/* the first parameter to echo the rest of his command line to all */
/* the other people in the room.				   */

int dm_echo(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	room		*rom_ptr;
	int		index = -1, i, fd;

	if(ply_ptr->class < CARETAKER)
		return(PROMPT);

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	for(i=0; i<strlen(cmnd->fullstr); i++) {
		if(cmnd->fullstr[i] == ' ') {
			index = i+1;
			break;
		}
	}

	if(index == -1 || strlen(&cmnd->fullstr[index]) < 1) {
		print(fd, "Echo what?\n");
		return(0);
	}

	broadcast_rom(-1, rom_ptr->rom_num, "%s", &cmnd->fullstr[index]);

	return(0);

}

/**********************************************************************/
/*				dm_flushsave			      */
/**********************************************************************/

/* This function allows DM's to save all the rooms in memory back to */
/* disk in one fell swoop.  					     */

int dm_flushsave(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	if(ply_ptr->class < DM || !(!strcmp(ply_ptr->name, DMNAME2)))
		return(PROMPT);

	if(cmnd->num < 2) {
		print(ply_ptr->fd, "All rooms and contents flushed to disk.\n");
		resave_all_rom(0);
	}
	else {
		print(ply_ptr->fd, 
		      "All rooms and PERM contents flushed to disk.\n");
		resave_all_rom(PERMONLY);
	}

	return(0);

}

/**********************************************************************/
/*				dm_shutdown			      */
/**********************************************************************/

/* This function allows a DM to shut down the game in a given number of */
/* minutes.							        */

int dm_shutdown(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	if(ply_ptr->class < CARETAKER)
		return(PROMPT);

	print(ply_ptr->fd, "Ok.\n");

	Shutdown.ltime = time(0);
	Shutdown.interval = cmnd->val[0] * 60 + 1;

	return(0);

}

/**********************************************************************/
/*				dm_rmstat			      */
/**********************************************************************/

/* This function displays a room's status to the DM.		      */

int dm_rmstat(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{

	if(ply_ptr->class < CARETAKER)
		return(PROMPT);

	print(ply_ptr->fd, "Room #%d\n", ply_ptr->parent_rom->rom_num);
	return(0);
}

/**********************************************************************/
/*				dm_flush_crtobj			      */
/**********************************************************************/

/* This function allows a DM to flush the object and creature data so */
/*  that updated data can be loaded into memory instead.	      */

int dm_flush_crtobj(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	if(ply_ptr->class < CARETAKER)
		return(PROMPT);

	flush_obj();
	flush_crt();

	print(ply_ptr->fd, 
		"Basic object and creature data flushed from memory.\n");

	return(0);
}

/**********************************************************************/
/*				dm_reload_rom			      */
/**********************************************************************/

/* This function allows a DM to reload a room from disk.	      */

int dm_reload_rom(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	if(ply_ptr->class < CARETAKER)
		return(PROMPT);

	if(reload_rom(ply_ptr->rom_num) < 0)
		print(ply_ptr->fd, "Reload failed.\n");
	else
		print(ply_ptr->fd, "Ok.\n");

	return(0);
}

/**********************************************************************/
/*				dm_resave			      */
/**********************************************************************/

/* This function allows a DM to save a room back to disk.	      */

int dm_resave(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	if(ply_ptr->class < CARETAKER)
		return(PROMPT);

	if(resave_rom(ply_ptr->rom_num) < 0)
		print(ply_ptr->fd, "Resave failed.\n");
	else
		print(ply_ptr->fd, "Ok.\n");

	return(0);
}

/**********************************************************************/
/*				dm_create_obj			      */
/**********************************************************************/

/* This function allows a DM to create an object that will appear     */
/* in his inventory.						      */

int dm_create_obj(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	object	*obj_ptr;

	if(ply_ptr->class < CARETAKER)
		return(PROMPT);

	if(load_obj(cmnd->val[0], &obj_ptr) < 0) {
		print(ply_ptr->fd, "Error (%d)\n", cmnd->val[0]);
		return(0);
	}
	if(F_ISSET(obj_ptr, ORENCH))
		rand_enchant(obj_ptr);
	print(ply_ptr->fd, "%s added to your inventory.\n", obj_ptr->name);
	add_obj_crt(obj_ptr, ply_ptr);
	return(0);
}

/**********************************************************************/
/*				dm_create_crt			      */
/**********************************************************************/

/* This function allows a DM to create a creature that will appear    */
/* in the room he is located in.				      */

int dm_create_crt(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	creature	*crt_ptr;
	object		*obj_ptr;
	room		*rom_ptr;
	int		item, num;
	int		total, l,j, m, k, n;
	long 		t;

	if(ply_ptr->class < CARETAKER)
		return(PROMPT);
	rom_ptr = ply_ptr->parent_rom;

	num = cmnd->val[0];
	if(num < 2) {
		num = mrand(0,9);
		num = ply_ptr->parent_rom->random[num];
		if(!num) return (0);
	}

	total = 1;
	if (cmnd->num  ==  2)
	   if (cmnd->str[1][0] == 'n')
		total = cmnd->val[1];
	else if (cmnd->str[1][0] == 'g'){
		total = mrand(1, count_ply(rom_ptr));
		if(cmnd->val[1] == 1){
			num = mrand(0,9); 
                	num = ply_ptr->parent_rom->random[num];
                	if(!num) return (0);
		}
	}	


	if(load_crt(num, &crt_ptr) < 0) {
		print(ply_ptr->fd, "Error (%d)\n", cmnd->val[0]);
		return(0);
	}
	t = time(0);
        for(l=0; l<total;) {
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
                add_crt_rom(crt_ptr, rom_ptr, total);
            else
                add_crt_rom(crt_ptr, rom_ptr, 0);

            add_active(crt_ptr);
	l++;
	if(l < total)
		load_crt(num, &crt_ptr);
        }

	return(0);
}

/**********************************************************************/
/*				dm_perm				      */
/**********************************************************************/

/* This function allows a player to make a given object sitting on the */
/* floor into a permanent object.				       */

int dm_perm(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	object	*obj_ptr;

	if(ply_ptr->class < DM) 
		return(PROMPT);

	obj_ptr = find_obj(ply_ptr, 
			   ply_ptr->parent_rom->first_obj, 
			   cmnd->str[1], cmnd->val[1]);

	if(!obj_ptr)
		print(ply_ptr->fd, "Failed.\n");
	else {
		F_SET(obj_ptr, OPERM2);
		F_SET(obj_ptr, OTEMPP);
		print(ply_ptr->fd, "Done.\n");
	}
	return(0);
}

/**********************************************************************/
/*				dm_invis			      */
/**********************************************************************/

/* This function allows a DM to turn himself invisible.		      */

int dm_invis(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	if(ply_ptr->class < CARETAKER)
		return(PROMPT);

	if(F_ISSET(ply_ptr, PDMINV)) {
		F_CLR(ply_ptr, PDMINV);
		ANSI(ply_ptr->fd, MAGENTA);
		print(ply_ptr->fd, "Invisibility off.\n");
	}
	else {
		F_SET(ply_ptr, PDMINV);
		ANSI(ply_ptr->fd, YELLOW);
		print(ply_ptr->fd, "Invisibility on.\n");
	}
	ANSI(ply_ptr->fd, WHITE);
	return(0);

}

/**********************************************************************/
/*				dm_ac				      */
/**********************************************************************/

/* This function allows a DM to take a look at his own special stats. */
/*  or another user's stats.					      */

int dm_ac(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	creature	*crt_ptr;

	if(ply_ptr->class < CARETAKER)
		return(PROMPT);

	if(cmnd->num == 2) {
		lowercize(cmnd->str[1], 1);
		crt_ptr = find_who(cmnd->str[1]);
		if(!crt_ptr) {
			print(ply_ptr->fd, "%s is not on.\n", cmnd->str[1]);
			return(0);
		}
	}
	else {
		ply_ptr->hpcur = ply_ptr->hpmax;
		ply_ptr->mpcur = ply_ptr->mpmax;
		crt_ptr = ply_ptr;
	}

	print(ply_ptr->fd, "AC: %d  THAC0: %d\n",
	      crt_ptr->armor, crt_ptr->thaco);

	return(0);

}

/**********************************************************************/
/*				dm_force			      */
/**********************************************************************/

/* This function allows a DM to force another user to do a command.   */

int dm_force(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	creature	*crt_ptr;
	int		i, cfd, fd, index = 0;
        char    	str[IBUFSIZE+1];

	if(ply_ptr->class < CARETAKER || cmnd->num < 2)
		return(PROMPT);

	lowercize(cmnd->str[1], 1);
	crt_ptr = find_who(cmnd->str[1]);
	if(!crt_ptr) {
		print(ply_ptr->fd, "%s is not on.\n", cmnd->str[1]);
		return(0);
	}


	if((crt_ptr->class == DM) && (ply_ptr->class < DM))
		return(PROMPT);

	cfd = crt_ptr->fd;
	if(!(Ply[cfd].io->fn == command && Ply[cfd].io->fnparam == 1)) {
		print(ply_ptr->fd, "Can not force %s right now.\n", 
		      cmnd->str[1]);
		return(0);
	}

	fd = ply_ptr->fd;

	for(i=0; i<strlen(cmnd->fullstr); i++)
		if(cmnd->fullstr[i] == ' ') {
			index = i+1;
			break;
		}
	for(i=index; i<strlen(cmnd->fullstr); i++)
		if(cmnd->fullstr[i] != ' ') {
			index = i+1;
			break;
		}
	for(i=index; i<strlen(cmnd->fullstr); i++)
		if(cmnd->fullstr[i] == ' ') {
			index = i+1;
			break;
		}
	for(i=index; i<strlen(cmnd->fullstr); i++)
		if(cmnd->fullstr[i] != ' ') {
			index = i;
			break;
		}
	strcpy(str, &cmnd->fullstr[index]);

	command(cfd, 1, str);

	return(PROMPT);

}



