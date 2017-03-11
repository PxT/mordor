/*
 * DM1.C:
 *
 *	DM functions
 *
 *	Copyright (C) 1991, 1992, 1993 Brooke Paul
 *
 * $Id: dm1.c,v 6.16 2001/07/17 19:25:11 develop Exp $
 *
 * $Log: dm1.c,v $
 * Revision 6.16  2001/07/17 19:25:11  develop
 * *** empty log message ***
 *
 * Revision 6.16  2001/07/14 21:26:44  develop
 * *** empty log message ***
 *
 * Revision 6.15  2001/06/26 22:30:48  develop
 * conjure bug fixes
 *
 * Revision 6.14  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

#include "../include/mordb.h"
#include "mextern.h"
#include <ctype.h>


/**********************************************************************/
/*				dm_teleport			      */
/**********************************************************************/

/* This function allows DM's to teleport to a given room number, or to */
/* a player's location.  It will also teleport a player to the DM or   */
/* one player to another.					       */
 
int dm_teleport(creature *ply_ptr, cmd *cmnd )
{
	creature	*crt_ptr;
	creature	*crt_ptr2;
	room		*rom_ptr;
	ctag		*cp;

	if(ply_ptr->class < CARETAKER )
		return(PROMPT);
	if(cmnd->num < 2) {
		if(cmnd->val[0] >= RMAX) 
			return(0);
		if(load_rom(cmnd->val[0], &rom_ptr) < 0) {
			sprintf(g_buffer, "Error: Room %d does not exist or could not be loaded\n", (int)cmnd->val[0]);
			output(ply_ptr->fd, g_buffer);
			return(0);
		}
	    if(F_ISSET(ply_ptr, PALIAS)) {
			del_crt_rom(Ply[ply_ptr->fd].extr->alias_crt, ply_ptr->parent_rom);
			broadcast_rom(ply_ptr->fd, ply_ptr->rom_num,"%M just wandered away.", 
				m1arg(Ply[ply_ptr->fd].extr->alias_crt));
			add_crt_rom(Ply[ply_ptr->fd].extr->alias_crt, rom_ptr, 1);
	    }

	    cp = ply_ptr->first_fol;
	    while(cp) {
		if(cp->crt->type == MONSTER && F_ISSET(cp->crt, MCONJU)) {
			del_crt_rom(cp->crt, ply_ptr->parent_rom);
			add_crt_rom(cp->crt, rom_ptr, 1);
		}
  	        cp = cp->next_tag;
	    }	

	    if(!F_ISSET(ply_ptr, PDMINV)) {
			broadcast_rom(ply_ptr->fd, ply_ptr->rom_num, "%M disappears in a puff of smoke.", 
				m1arg(ply_ptr->name));
		}

		del_ply_rom(ply_ptr, ply_ptr->parent_rom);
		add_ply_rom(ply_ptr, rom_ptr);
	}

	else if(cmnd->num < 3) {
		lowercize(cmnd->str[1], 1);
		crt_ptr = find_who(cmnd->str[1]);
		if(!crt_ptr || crt_ptr == ply_ptr || (crt_ptr->class == DM &&
		   ply_ptr->class < DM && F_ISSET(crt_ptr, PDMINV))) 
		{
			sprintf(g_buffer, "%s is not on.\n", cmnd->str[1]);
			output(ply_ptr->fd, g_buffer);
			return(0);
		}
	    if(F_ISSET(ply_ptr, PALIAS)) {
			del_crt_rom(Ply[ply_ptr->fd].extr->alias_crt, ply_ptr->parent_rom);
			broadcast_rom(ply_ptr->fd, ply_ptr->rom_num,"%M just wandered away.", 
				m1arg(Ply[ply_ptr->fd].extr->alias_crt));
			add_crt_rom(Ply[ply_ptr->fd].extr->alias_crt, crt_ptr->parent_rom, 1);
            }
	    cp = ply_ptr->first_fol;
	    while(cp) {
		if(cp->crt->type == MONSTER && F_ISSET(cp->crt, MCONJU)) {
			del_crt_rom(cp->crt, ply_ptr->parent_rom);
			add_crt_rom(cp->crt, crt_ptr->parent_rom, 1);
		}
  	        cp = cp->next_tag;
	    }	

	    if(!F_ISSET(ply_ptr, PDMINV)) {
			broadcast_rom(ply_ptr->fd, ply_ptr->rom_num, "%M disappears in a puff of smoke.", m1arg(ply_ptr));
		}

		del_ply_rom(ply_ptr, ply_ptr->parent_rom);
		add_ply_rom(ply_ptr, crt_ptr->parent_rom);
	}

	else {
		lowercize(cmnd->str[1], 1);
		crt_ptr = find_who(cmnd->str[1]);
		if(!crt_ptr || crt_ptr == ply_ptr || (ply_ptr->class < DM &&
		   crt_ptr->class == DM && F_ISSET(crt_ptr, PDMINV))) {
			sprintf(g_buffer, "%s is not on.\n", cmnd->str[1]);
			output(ply_ptr->fd, g_buffer);
			return(0);
		}
		lowercize(cmnd->str[2], 1);
		if(*cmnd->str[2] == '.')
			crt_ptr2 = ply_ptr;
		else
			crt_ptr2 = find_who(cmnd->str[2]);
		if(!crt_ptr2) {
			sprintf(g_buffer, "%s is not on.\n", cmnd->str[1]);
			output(ply_ptr->fd, g_buffer);
			return(0);
		}
		if(!F_ISSET(ply_ptr, PDMINV)) {
			broadcast_rom(crt_ptr->fd, crt_ptr->rom_num, "%M disappears in a puff of smoke.", m1arg(crt_ptr->name));
		}
	    	if(F_ISSET(crt_ptr, PALIAS)) {
			del_crt_rom(Ply[crt_ptr->fd].extr->alias_crt, crt_ptr->parent_rom);
			broadcast_rom(crt_ptr->fd, crt_ptr->rom_num,"%M just wandered away.", 
				m1arg(Ply[crt_ptr->fd].extr->alias_crt));
			add_crt_rom(Ply[crt_ptr->fd].extr->alias_crt, crt_ptr2->parent_rom, 1);
                }
	    	cp = crt_ptr->first_fol;
	    	while(cp) {
			if(cp->crt->type == MONSTER && F_ISSET(cp->crt, MCONJU)) {
				del_crt_rom(cp->crt, crt_ptr->parent_rom);
				add_crt_rom(cp->crt, crt_ptr2->parent_rom, 1);
			}
  	        	cp = cp->next_tag;
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

int dm_send(creature *ply_ptr, cmd *cmnd )
{
	int	fd, found = 0;
	unsigned int i;

	if(ply_ptr->class < CARETAKER )
		return(PROMPT);

	fd = ply_ptr->fd;

	for(i=0; i< strlen(cmnd->fullstr); i++) {
		if(cmnd->fullstr[i] == ' ' && cmnd->fullstr[i+1] != ' ') {
			found++;
			break;
		}
	}

	if(found < 1 || strlen(&cmnd->fullstr[i+1]) < 1) {
		output(fd, "*Send what?\n");
		return(0);
	}

	output(fd, "Ok.\n");
	broadcast_wiz(ply_ptr, &cmnd->fullstr[i+1]);

	return(0);

}

/**********************************************************************/
/*				dm_purge			      */
/**********************************************************************/

/* This function allows DMs to purge a room of all its objects and    */
/* monsters.							      */

int dm_purge(creature *ply_ptr, cmd	*cmnd )
{
	ctag	*cp, *fol, *ctemp, *folprev;
	otag	*op, *otemp;
	room	*rom_ptr;
	int	fd;

	if(ply_ptr->class < BUILDER)
		return(PROMPT);

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	cp = rom_ptr->first_mon;
	rom_ptr->first_mon = 0;
	while(cp) {
		ctemp = cp->next_tag;
		if(F_ISSET (cp->crt, MDMFOL)) { 
			/* clear relevant follow lists */
            		F_CLR(cp->crt->following, PALIAS);
			Ply[cp->crt->following->fd].extr->alias_crt = 0;
            		mprint (cp->crt->following->fd, "%1M's soul was purged.\n", 
								m1arg(cp->crt));
			fol = cp->crt->following->first_fol;
            		if(fol->crt == cp->crt) { 
               			cp->crt->following->first_fol = fol->next_tag;
				free(fol); 
	    		} 
	    		else {
				while(fol) {	
					if(fol->crt == cp->crt) {
						folprev = fol->next_tag;
        					free(fol);
						break;
					}
					folprev = fol;
					fol = fol->next_tag;
				}
	    		}
		}
		if(F_ISSET(cp->crt, MCONJU)) {
            		mprint (cp->crt->following->fd, "%1M fades away.\n", 
								m1arg(cp->crt));
			del_conjured(cp->crt);
			del_crt_rom(cp->crt, cp->crt->parent_rom);
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

	output(fd, "Purged.\n");

	if(ply_ptr->class < DM)
		log_dm_command(ply_ptr->name, cmnd->fullstr);

	return(0);

}

/**********************************************************************/
/*				dm_users			      */
/**********************************************************************/

/* This function allows DM's to list all users online, displaying */
/* level, name, current room # and name, and foreign address.     */
 
int dm_users(creature *ply_ptr, cmd	*cmnd )
{
	char	*idstr;
	time_t	t;
	int		fd, i, userid = 0, fulluser = 0;
	char	str[10];

	if(ply_ptr->class < CARETAKER )
		return(PROMPT);

	if(cmnd->num > 1) {
		if(cmnd->str[1][0] == 'u') userid = 1;
		if(cmnd->str[1][0] == 'f') fulluser = 1;
	}

	t = time(0);
	fd = ply_ptr->fd;
	ANSI(fd, AFC_BLUE);
	if(fulluser)
	{
		sprintf(g_buffer, "\n%-9s %-10s %-52s", "Lev  Clas", " Player", " Email address");
		output(fd, g_buffer);
	}
	else
	{
		sprintf(g_buffer, "\n%-9s %-10s %-20s %-15s %-15s", "Lev  Clas", " Player", "Room #: Name", userid ? "UserID" : "Address", "Last command");
		output(fd, g_buffer);
	}
	output(fd, " Idle\n");
	output(fd, "-------------------------------------------------------------------------------\n");
	ANSI(fd, AFC_WHITE);
	for(i=0; i<Tablesize; i++) {
		if(!Ply[i].ply) continue;
		if(Ply[i].ply->fd < 0) continue;
		if(Ply[i].ply->class == DM && ply_ptr->class < DM &&
		   F_ISSET(Ply[i].ply, PDMINV)) continue;
		if(F_ISSET(Ply[i].ply, PDMINV) && F_ISSET(Ply[i].ply, PROBOT) &&
			(!strcmp(Ply[i].ply->name,dmname[0]) || !strcmp(Ply[i].ply->name,dmname[1])) )
			continue;

		sprintf(g_buffer, "[%2d] %-4.4s ", Ply[i].ply->level, get_class_string(Ply[i].ply->class));
		if(Ply[i].ply->class == DM)
			ANSI(fd, AFC_RED);
		else if(Ply[i].ply->class == CARETAKER)
			ANSI(fd, AFC_YELLOW);
		else if(Ply[i].ply->class == BUILDER)
			ANSI(fd, AFC_GREEN);
		output(fd, g_buffer);

		if ( F_ISSET(Ply[i].ply, PDMINV) )
		{
			strcpy(str, "+");
		}
		else if (F_ISSET(Ply[i].ply, PINVIS) )
		{
			strcpy(str, "*");
		}
		else
		{
			strcpy(str, " ");
		}

		if(!F_ISSET(Ply[i].ply, PSECOK)){
			 ANSI(fd, AFC_RED);
		}
		else {
			 ANSI(fd, AFC_YELLOW);
		}

		sprintf(g_buffer, "%s%-10.10s ", str, Ply[i].ply->name);
		output(fd, g_buffer);

		ANSI(fd, AFC_WHITE);
		if(fulluser) {
			idstr = malloc( strlen(Ply[i].io->userid) + strlen(Ply[i].io->address) + 3);
			if ( idstr )
			{
				sprintf(idstr, "%s@%s", Ply[i].io->userid,
					Ply[i].io->address);
				if(!strcmp(Ply[i].io->userid, "no_port") || !strcmp(Ply[i].io->userid, "unknown"))
					ANSI(fd, AFC_MAGENTA);

				sprintf(g_buffer, "%-51.51s ", idstr);
				output(fd, g_buffer);

				free(idstr);
			}
			else
			{
				output(fd, "malloc failed in dm_users");
			}
		}
		else {
			ANSI(fd, AFC_WHITE);
			sprintf(g_buffer, "%5hd: ", Ply[i].ply->rom_num);
			output(fd, g_buffer);
			ANSI(fd, AFC_BLUE);
			sprintf(g_buffer, "%-12.12s ", Ply[i].ply->parent_rom->name);
			output(fd, g_buffer);
			ANSI(fd, AFC_CYAN);
			sprintf(g_buffer, "%-15.15s ", userid ? Ply[i].io->userid : 
				Ply[i].io->address);
			output(fd, g_buffer);
			ANSI(fd, AFC_GREEN);
			if(Ply[i].ply->class < DM || ply_ptr->class==DM) 
				sprintf(g_buffer, "%-15.15s ", Ply[i].extr->lastcommand);
			else
                sprintf(g_buffer, "%-15.15s ", "l");
			output(fd, g_buffer );

		}
		ANSI(fd, AFC_WHITE);
		sprintf(g_buffer, "%02ld:%02ld\n", (t-Ply[i].io->ltime)/60L, 
		      (t-Ply[i].io->ltime)%60L);
		output(fd, g_buffer);
	}
	ANSI(fd, AFC_WHITE);
	ANSI(fd, AM_NORMAL);
	output(fd, "\n");

	return(0);

}

/**********************************************************************/
/*				dm_echo				      */
/**********************************************************************/

/* This function allows a DM specified by the socket descriptor in */
/* the first parameter to echo the rest of his command line to all */
/* the other people in the room.				   */

int dm_echo(creature *ply_ptr, cmd *cmnd )
{
	room		*rom_ptr;
	int		index = -1, fd;
	unsigned int i;

	if(ply_ptr->class < BUILDER)
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
		output(fd, "Echo what?\n");
		return(0);
	}

	broadcast_rom(-1, rom_ptr->rom_num, &cmnd->fullstr[index], NULL);

	return(0);

}

/**********************************************************************/
/*				dm_flushsave			      */
/**********************************************************************/

/* This function allows DM's to save all the rooms in memory back to */
/* disk in one fell swoop.  					     */

int dm_flushsave( creature *ply_ptr, cmd *cmnd )
{
	if(ply_ptr->class < DM || !(!strcmp(ply_ptr->name, dmname[1])))
		return(PROMPT);

	if(cmnd->num < 2) {
		output(ply_ptr->fd, "All rooms and contents flushed to disk.\n");
		resave_all_rom(0);
	}
	else {
		output(ply_ptr->fd, 
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

int dm_shutdown(creature *ply_ptr, cmd *cmnd )
{
	if(ply_ptr->class <= CARETAKER )
		return(PROMPT);

	output(ply_ptr->fd, "Ok.\n");

	/* book it Danno */
	log_dm_command( ply_ptr->name, cmnd->fullstr);

	Shutdown.ltime = time(0);
	Shutdown.interval = cmnd->val[0] * 60 + 1;

	return(0);

}

/*********************************************************************/
/*			shutdown_catch				     */
/*********************************************************************/

/* The purpose of this function is to force dm's to fully type *shutdown */
/* in order to shut the game down.  This prevents accidently shutdowns   */
/* caused by typos with the *s command					 */

int shutdown_catch(creature *ply_ptr, cmd *cmnd)
{
	if(ply_ptr->class <= CARETAKER)
		return(0);

	output(ply_ptr->fd, "You must fully type *shutdown to shut the game down.\n");

	return(0);
}

/**********************************************************************/
/*				dm_rmstat			      */
/**********************************************************************/

/* This function displays a room's status to the DM.		      */

int dm_rmstat( creature *ply_ptr, cmd *cmnd )
{

	if(ply_ptr->class < BUILDER)
		return(PROMPT);

	sprintf(g_buffer, "Room #%d\n", ply_ptr->parent_rom->rom_num);
	output(ply_ptr->fd, g_buffer);
	return(0);
}

/**********************************************************************/
/*				dm_flush_crtobj			      */
/**********************************************************************/

/* This function allows a DM to flush the object and creature data so */
/*  that updated data can be loaded into memory instead.	      */

int dm_flush_crtobj(creature *ply_ptr, cmd *cmnd )
{
	if(ply_ptr->class < CARETAKER)
		return(PROMPT);

	flush_obj();
	flush_crt();

	output(ply_ptr->fd, 
		"Basic object and creature data flushed from memory.\n");

	return(0);
}

/**********************************************************************/
/*				dm_reload_rom			      */
/**********************************************************************/

/* This function allows a DM to reload a room from disk.	      */

int dm_reload_rom( creature *ply_ptr, cmd *cmnd )
{
	ctag *cp;

	if(ply_ptr->class < BUILDER)
		return(PROMPT);

        cp = ply_ptr->first_fol;
        while(cp) {
                if(cp->crt->type == MONSTER && F_ISSET(cp->crt, MCONJU)) {
                        del_conjured(cp->crt);
                        del_crt_rom(cp->crt,ply_ptr->parent_rom);
                        free_crt(cp->crt);
                        output(ply_ptr->fd, "Conjured pet flushed.\n");
                }
                cp = cp->next_tag;
        }

	if(reload_rom(ply_ptr->rom_num) < 0)
		output(ply_ptr->fd, "Reload failed.\n");
	else
	{
		add_permcrt_rom(ply_ptr->parent_rom);
		output(ply_ptr->fd, "Ok.\n");
	}


	return(0);
}

/**********************************************************************/
/*				dm_save			      */
/**********************************************************************/

/* This function allows a DM to save a room back to disk.	      */

int dm_save(creature *ply_ptr, cmd *cmnd )
{
	if(ply_ptr->class < BUILDER )
		return(PROMPT);

	if(!strcmp(cmnd->str[1], "c") && (cmnd->num > 1) && ply_ptr->class > CARETAKER) {
		dm_save_crt(ply_ptr, cmnd);
		return(0);
	}
	if(!strcmp(cmnd->str[1], "o") && (cmnd->num > 1) && ply_ptr->class > CARETAKER) {
		dm_save_obj(ply_ptr, cmnd);
		return(0);
	}

	if(resave_rom(ply_ptr->rom_num) < 0)
		output(ply_ptr->fd, "Resave failed.\n");
	else
		output(ply_ptr->fd, "Room saved.\n");

	return(0);
}

/**********************************************************************/
/*				dm_create_obj			      */
/**********************************************************************/

/* This function allows a DM to create an object that will appear     */
/* in his inventory.						      */

int dm_create_obj(creature *ply_ptr, cmd *cmnd )
{
	object	*obj_ptr;

	if(ply_ptr->class < CARETAKER )
		return(PROMPT);

	if(load_obj(cmnd->val[0], &obj_ptr) < 0) {
		sprintf(g_buffer, "Error (%d)\n", (int)cmnd->val[0]);
		output(ply_ptr->fd, g_buffer);
		return(0);
	}
	if(F_ISSET(obj_ptr, ORENCH))
		rand_enchant(obj_ptr);
	sprintf(g_buffer, "%s added to your inventory.\n", obj_ptr->name);
	output(ply_ptr->fd, g_buffer);
	add_obj_crt(obj_ptr, ply_ptr);

	if(ply_ptr->class < DM)
		log_dm_command(ply_ptr->name, cmnd->fullstr);

	return(0);
}

/**********************************************************************/
/*				dm_create_crt			      */
/**********************************************************************/

/* This function allows a DM to create a creature that will appear    */
/* in the room he is located in.				      */

int dm_create_crt( creature *ply_ptr, cmd *cmnd )
{
	creature	*crt_ptr;
	object		*obj_ptr;
	room		*rom_ptr;
	int			num;
	int			total, l,j, m, k;
	time_t 		t;

	if(ply_ptr->class < CARETAKER )
		return(PROMPT);
	rom_ptr = ply_ptr->parent_rom;

	num = cmnd->val[0];
	if(num < 2) {
		num = mrand(0,9);
		num = ply_ptr->parent_rom->random[num];
		if(!num) return (0);
	}

	total = 1;
	if (cmnd->num  ==  2) {
	   if (cmnd->str[1][0] == 'n')
		total = cmnd->val[1];
	}  
	else if (cmnd->str[1][0] == 'g'){
		total = mrand(1, count_ply(rom_ptr));
		if(cmnd->val[1] == 1){
			num = mrand(0,9); 
                	num = ply_ptr->parent_rom->random[num];
                	if(!num) return (0);
		}
	}	


	if(load_crt(num, &crt_ptr) < 0) {
		sprintf(g_buffer, "Error (%d)\n", (int)cmnd->val[0]);
		output(ply_ptr->fd, g_buffer);
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

	    if(cmnd->str[1][0]=='p') {
			output(ply_ptr->fd, "Loading prototype creature.\n");
			for(k=0; k<10; k++)
				if(crt_ptr->carry[k]) {
					m=load_obj(crt_ptr->carry[k], &obj_ptr);
                        		if(m > -1) {
                        		if(F_ISSET(obj_ptr, ORENCH))
                            			rand_enchant(obj_ptr);
                        			obj_ptr->value = mrand((obj_ptr->value*9)/10, (obj_ptr->value*11)/10);
                        			add_obj_crt(obj_ptr, crt_ptr);
					}
				}
	    }		
	    else {			
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
	    }

            if(!F_ISSET(crt_ptr, MNRGLD) && crt_ptr->gold)
                crt_ptr->gold =
                mrand(crt_ptr->gold/10, crt_ptr->gold);

            if(!l) 
                add_crt_rom(crt_ptr, rom_ptr, total);
            else
                add_crt_rom(crt_ptr, rom_ptr, 0);

#ifndef MOBS_ALWAYS_ACTIVE
            add_active(crt_ptr);
#endif

	l++;
	if(l < total)
		load_crt(num, &crt_ptr);
        }


	if(ply_ptr->class < DM)
		log_dm_command(ply_ptr->name, cmnd->fullstr);

	return(DOPROMPT);
}

/**********************************************************************/
/*				dm_perm				      */
/**********************************************************************/

/* This function allows a player to make a given object sitting on the */
/* floor into a permanent object.				       */

int dm_perm(creature *ply_ptr, cmd *cmnd )
{
	creature	*crt_ptr;
	object		*obj_ptr;
	int		x=0, n=0, fd;

	if(ply_ptr->class < DM) 
		return(PROMPT);

	fd=ply_ptr->fd;

	if(cmnd->num < 2) {
		output(fd, "Syntax: *perm [o|c|t] [name|d|exit] [timeout|slot]\n");
		return(0);
	}

	switch(low(cmnd->str[1][0])) {
	case 'o':
	
	if(!strcmp(cmnd->str[2], "d") && strlen(cmnd->str[2])<2) {
		if(cmnd->val[2]>10 || cmnd->val[2]<1){
			output(fd, "Slot to delete out of range.\n");
			return(0);
		}
		ply_ptr->parent_rom->perm_obj[cmnd->val[2]-1].misc=0;
		ply_ptr->parent_rom->perm_obj[cmnd->val[2]-1].interval=0;
		sprintf(g_buffer, "Perm object slot #%d cleared in room %d.\n", 
			(int)cmnd->val[2], ply_ptr->parent_rom->rom_num);
		output(fd, g_buffer);
		return(0);
	}


	obj_ptr = find_obj(ply_ptr,ply_ptr->parent_rom->first_obj,cmnd->str[2], 1);

	if(!obj_ptr) {
		output(ply_ptr->fd, "Object not found.\n");
		return(0);
	}

	n=find_obj_num(obj_ptr);
	if(!n) {
		output(fd, "Object is not in database. Not permed.\n");
		return(0);
	}

	if(cmnd->val[2]<2)
		cmnd->val[2]= 7200;
	
	while(ply_ptr->parent_rom->perm_obj[x].misc)
		x++;
	if(x>9) {
		output(fd, "Room is already full.\n");
		return(0);
	}

	ply_ptr->parent_rom->perm_obj[x].misc=n;
	ply_ptr->parent_rom->perm_obj[x].interval=(long)cmnd->val[2];
	
	sprintf(g_buffer, "%s permed with timeout of %d.\n", obj_ptr->name, 
						(int)cmnd->val[2]);
	output(fd, g_buffer);

	return(0);
	/* perm creature */
	case 'c':

		if(!strcmp(cmnd->str[2], "d") && strlen(cmnd->str[2])<2) {
                if(cmnd->val[2]>10 || cmnd->val[2]<1) {
                        output(fd, "Slot to delete out of range.\n");
                        return(0);
				}
		
                ply_ptr->parent_rom->perm_mon[cmnd->val[2]-1].misc=0;
				ply_ptr->parent_rom->perm_mon[cmnd->val[2]-1].interval=0;
                sprintf(g_buffer, "Perm monster slot #%d cleared.\n", (int)cmnd->val[2]);
                output(fd, g_buffer);
                return(0);
        }

		crt_ptr = find_crt(ply_ptr,ply_ptr->parent_rom->first_mon, 
			cmnd->str[2], 1);
         
        if(!crt_ptr) {
                output(ply_ptr->fd, "Creature not found.\n");
                return(0);
        }        
        
		/* n=find_crt_num(crt_ptr); */
		n=atoi(crt_ptr->password);
        if(!n) {  
                output(fd, "Creature is not in database. Not permed.\n");
                return(0);
	 }
        
        if(cmnd->val[2]<2)
                cmnd->val[2]= 7200;
                 
        while(ply_ptr->parent_rom->perm_mon[x].misc)
                x++;
        if(x>9) { 
                output(fd, "Room is already full.\n");
                return(0);
	}
        
        ply_ptr->parent_rom->perm_mon[x].misc=n;
        ply_ptr->parent_rom->perm_mon[x].interval=(long)cmnd->val[2];
                 
        sprintf(g_buffer, "%s permed with timeout of %d.\n", crt_ptr->name, (int)cmnd->val[2]);
        output(fd, g_buffer);
                
        return(0);
	/* perm tracks */
	case 't':
		if(!strcmp(cmnd->str[2], "d")||cmnd->num < 3) {
 			F_CLR(ply_ptr->parent_rom, RPTRAK);               
                        output(fd, "Perm tracks deleted.\n");
                        return(0);
		}
		strcpy(ply_ptr->parent_rom->track,cmnd->str[2]); 
		F_SET(ply_ptr->parent_rom, RPTRAK);
		sprintf(g_buffer, "Perm tracks added leading %s.\n", cmnd->str[2]);
		output(fd, g_buffer);
		return(0);
	
	default:
        output(fd, "Syntax: *perm [o|c|t] [name|d|exit] [timeout|slot]\n");
		return(0);
	}
}

/**********************************************************************/
/*				dm_invis			      */
/**********************************************************************/

/* This function allows a DM to turn himself invisible.		      */

int dm_invis(creature *ply_ptr, cmd	*cmnd )
{
	if(ply_ptr->class < CARETAKER )
		return(PROMPT);

	if(F_ISSET(ply_ptr, PDMINV)) 
	{
		F_CLR(ply_ptr, PDMINV);
		output_wc(ply_ptr->fd, "Invisibility off.\n", AFC_MAGENTA);
	}
	else 
	{
		F_SET(ply_ptr, PDMINV);
		output_wc(ply_ptr->fd, "Invisibility on.\n", AFC_YELLOW);
		if ( !F_ISSET(ply_ptr, PALIAS) )
		{
			broadcast_rom(ply_ptr->fd, ply_ptr->rom_num, "%M disappears in a puff of smoke.", m1arg(ply_ptr));
		}
	}
	return(0);

}

/**********************************************************************/
/*				dm_ac				      */
/**********************************************************************/

/* This function allows a DM to take a look at his own special stats. */
/*  or another user's stats.					      */

int dm_ac(creature *ply_ptr, cmd *cmnd )
{
	creature	*crt_ptr;

	if(ply_ptr->class < CARETAKER )
		return(PROMPT);

	if(cmnd->num == 2) {
		lowercize(cmnd->str[1], 1);
		crt_ptr = find_who(cmnd->str[1]);
		if(!crt_ptr) {
			sprintf(g_buffer, "%s is not on.\n", cmnd->str[1]);
			output(ply_ptr->fd, g_buffer);
			return(0);
		}
	}
	else {
		ply_ptr->hpcur = ply_ptr->hpmax;
		ply_ptr->mpcur = ply_ptr->mpmax;
		crt_ptr = ply_ptr;
	}

	sprintf(g_buffer, "AC: %d  THAC0: %d\n",
	      crt_ptr->armor, crt_ptr->thaco);
	output(ply_ptr->fd, g_buffer);

	return(0);

}

/**********************************************************************/
/*				dm_force			      */
/**********************************************************************/

/* This function allows a DM to force another user to do a command.   */

int dm_force(creature *ply_ptr, cmd	*cmnd )
{
	creature	*crt_ptr;
	int		cfd, fd, index = 0;
	unsigned int i;
    char    	str[IBUFSIZE+1];


	if(ply_ptr->class < CARETAKER || cmnd->num < 2 )
		return(PROMPT);

	lowercize(cmnd->str[1], 1);
	crt_ptr = find_who(cmnd->str[1]);
	if(!crt_ptr) {
		sprintf(g_buffer, "%s is not on.\n", cmnd->str[1]);
		output(ply_ptr->fd, g_buffer);
		return(0);
	}


	if((crt_ptr->class == DM) && (ply_ptr->class < DM))
		return(PROMPT);

	cfd = crt_ptr->fd;
	if(!(Ply[cfd].io->fn == command && Ply[cfd].io->fnparam == 1)) {
		sprintf(g_buffer, "Can not force %s right now.\n", 
		      cmnd->str[1]);
		output(ply_ptr->fd, g_buffer);
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
