/*
 * PLAYER2.C:
 *
 *      Additional player routines.
 *
 *      Copyright (C) 2000, 2001 Paul Telford
 *
 * $Id: player2.c,v 6.16 2001/07/17 19:28:44 develop Exp $
 *
 * $Log: player2.c,v $
 * Revision 6.16  2001/07/17 19:28:44  develop
 * *** empty log message ***
 *
 * Revision 6.15  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

#include "../include/mordb.h"
#include "mextern.h"


/****************************************************************/
/*                              dequip                          */
/****************************************************************/
/* This should be called whenever a player dequips an object    */
/* or when it is fumbled, dissolved, etc                        */
/* Also called on logout, it is used to handle any OADDSA       */
/* bonuses that the player might have on.                       */
int dequip(creature *ply_ptr,object *obj_ptr)
{

        int i,fd;

	fd = ply_ptr->fd;

        if(!obj_ptr)
                return(-1);

        if(F_ISSET(obj_ptr,OADDSA)) {

                if(obj_ptr->strength != 0) 
                        ply_ptr->strength -= obj_ptr->strength;
                if(obj_ptr->dexterity != 0)
                        ply_ptr->dexterity -= obj_ptr->dexterity;
                if(obj_ptr->piety != 0)
                        ply_ptr->piety -= obj_ptr->piety;
                if(obj_ptr->intelligence != 0)
                        ply_ptr->intelligence -= obj_ptr->intelligence;
                if(obj_ptr->constitution != 0)
                        ply_ptr->constitution -= obj_ptr->constitution;

                for(i=0;i<128;i++) {
                        if(FS_ISSET(obj_ptr,i))
                                F_CLR(ply_ptr,i);
                }

                compute_ac(ply_ptr);
                compute_thaco(ply_ptr);

/* These are just to make lines below a little tidier */
#define l_time(a)	ply_ptr->lasttime[(a)].interval
#define l_ltime(a)	ply_ptr->lasttime[(a)].ltime

		if(equipment_sets(obj_ptr, PINVIS)) {
			l_time(LT_INVIS) -= 120;
		}
		if(equipment_sets(obj_ptr, PPROTE)) {
			l_time(LT_PROTE) -= 120;
		}
		if(equipment_sets(obj_ptr, PBLESS)) {
			l_time(LT_BLESS) -= 120;
		}
		if(equipment_sets(obj_ptr, PLIGHT)) {
			l_time(LT_LIGHT) -= 120;
		}
		if(equipment_sets(obj_ptr, PHIDDN)) {
			l_time(LT_HIDES) -= 120;
		}
		if(equipment_sets(obj_ptr, PHASTE)) {
			l_time(LT_HASTE) -= 120;
		}
		if(equipment_sets(obj_ptr, PDINVI)) {
			l_time(LT_DINVI) -= 120;
		}
		if(equipment_sets(obj_ptr, PDMAGI)) {
			l_time(LT_DMAGI) -= 120;
		}
		if(equipment_sets(obj_ptr, PPRAYD)) {
			l_time(LT_PRAYD) -= 120;
		}
		if(equipment_sets(obj_ptr, PLEVIT)) {
			l_time(LT_LEVIT) -= 120;
		}
		if(equipment_sets(obj_ptr, PRFIRE)) {
			l_time(LT_RFIRE) -= 120;
		}
		if(equipment_sets(obj_ptr, PFLYSP)) {
			l_time(LT_FLYSP) -= 120;
		}
		if(equipment_sets(obj_ptr, PRMAGI)) {
			l_time(LT_RMAGI) -= 120;
		}
		if(equipment_sets(obj_ptr, PKNOWA)) {
			l_time(LT_KNOWA) -= 120;
		}
		if(equipment_sets(obj_ptr, PRCOLD)) {
			l_time(LT_RCOLD) -= 120;
		}
		if(equipment_sets(obj_ptr, PBRWAT)) {
			l_time(LT_BRWAT) -= 120;
		}
		if(equipment_sets(obj_ptr, PSSHLD)) {
			l_time(LT_SSHLD) -= 120;
		}
		if(equipment_sets(obj_ptr, PSILNC)) {
			l_time(LT_SILNC) -= 120;
		}
		if(equipment_sets(obj_ptr, PCHARM)) {
			l_time(LT_CHRMD) -= 120;
		}
		if(equipment_sets(obj_ptr, PBESRK)) {
			l_time(LT_BSRKN) -= 120;
		}
		
#undef l_time
#undef l_ltime
                if(!F_ISSET(ply_ptr,PSAVES) && mrand(1,100) > obj_ptr->shotsmax) {
                        F_CLR(obj_ptr,OADDSA);
                        sprintf(g_buffer, "Your %s glows momentarily.\n",
                                obj_ptr->name);
                        output(ply_ptr->fd, g_buffer);
                }
        }
        return(0);
}

/****************************************************************/
/*                              equip                           */
/****************************************************************/
/* This should be called whenever a player equips an object     */
/* Also called on login                                         */
int equip(creature *ply_ptr, object *obj_ptr)
{

        int i;
	time_t t;

	t = time(0);

        if(!obj_ptr)
                return(-1);

        if(F_ISSET(obj_ptr,OADDSA)) {

		if(check_illegal_stats(ply_ptr, obj_ptr) == 1) {
			F_CLR(obj_ptr, OADDSA);
			return(0);
		}
			
		if(obj_ptr->strength || obj_ptr->dexterity ||
		   obj_ptr->constitution || obj_ptr->intelligence ||
		   obj_ptr->piety) { 
		  sprintf(g_buffer, "%s used a %ds %dc %di %dp %dd %s",
			ply_ptr->name, obj_ptr->strength, obj_ptr->constitution,
			obj_ptr->intelligence, obj_ptr->piety, obj_ptr->dexterity,
			obj_ptr->name);
		  slog(g_buffer);
		}

                if(obj_ptr->strength != 0) {
                        	ply_ptr->strength += obj_ptr->strength;
		}
                if(obj_ptr->dexterity != 0) {
                        	ply_ptr->dexterity += obj_ptr->dexterity;
		}
                if(obj_ptr->piety != 0) {
                        	ply_ptr->piety += obj_ptr->piety;
		}
                if(obj_ptr->intelligence != 0) {
                        	ply_ptr->intelligence += obj_ptr->intelligence;
		}
                if(obj_ptr->constitution != 0) {
                        	ply_ptr->constitution += obj_ptr->constitution;
		}

                for(i=0;i<128;i++) {
                        if(FS_ISSET(obj_ptr,i))
                                F_SET(ply_ptr,i);
                }

                compute_ac(ply_ptr);
                compute_thaco(ply_ptr);
	
/* These are just to make lines below a little tidier */
#define l_time(a)	ply_ptr->lasttime[(a)].interval
#define l_ltime(a)	ply_ptr->lasttime[(a)].ltime

		if(equipment_sets(obj_ptr, PINVIS)) {
			l_time(LT_INVIS) += 120;
			l_ltime(LT_INVIS) = t;
		}
		if(equipment_sets(obj_ptr, PPROTE)) {
			l_time(LT_PROTE) += 120;
			l_ltime(LT_PROTE) = t;
		}
		if(equipment_sets(obj_ptr, PBLESS)) {
			l_time(LT_BLESS) += 120;
			l_ltime(LT_BLESS) = t;
		}
		if(equipment_sets(obj_ptr, PLIGHT)) {
			l_time(LT_LIGHT) += 120;
			l_ltime(LT_LIGHT) = t;
		}
		if(equipment_sets(obj_ptr, PHIDDN)) {
			l_time(LT_HIDES) += 120;
			l_ltime(LT_HIDES) = t;
		}
		if(equipment_sets(obj_ptr, PHASTE)) {
			l_time(LT_HASTE) += 120;
			l_ltime(LT_HASTE) = t;
		}
		if(equipment_sets(obj_ptr, PDINVI)) {
			l_time(LT_DINVI) += 120;
			l_ltime(LT_DINVI) = t;
		}
		if(equipment_sets(obj_ptr, PDMAGI)) {
			l_time(LT_DMAGI) += 120;
			l_ltime(LT_DMAGI) = t;
		}
		if(equipment_sets(obj_ptr, PPRAYD)) {
			l_time(LT_PRAYD) += 120;
			l_ltime(LT_PRAYD) = t;
		}
		if(equipment_sets(obj_ptr, PLEVIT)) {
			l_time(LT_LEVIT) += 120;
			l_ltime(LT_LEVIT) = t;
		}
		if(equipment_sets(obj_ptr, PRFIRE)) {
			l_time(LT_RFIRE) += 120;
			l_ltime(LT_RFIRE) = t;
		}
		if(equipment_sets(obj_ptr, PFLYSP)) {
			l_time(LT_FLYSP) += 120;
			l_ltime(LT_FLYSP) = t;
		}
		if(equipment_sets(obj_ptr, PRMAGI)) {
			l_time(LT_RMAGI) += 120;
			l_ltime(LT_RMAGI) = t;
		}
		if(equipment_sets(obj_ptr, PKNOWA)) {
			l_time(LT_KNOWA) += 120;
			l_ltime(LT_KNOWA) = t;
		}
		if(equipment_sets(obj_ptr, PRCOLD)) {
			l_time(LT_RCOLD) += 120;
			l_ltime(LT_RCOLD) = t;
		}
		if(equipment_sets(obj_ptr, PBRWAT)) {
			l_time(LT_BRWAT) += 120;
			l_ltime(LT_BRWAT) = t;
		}
		if(equipment_sets(obj_ptr, PSSHLD)) {
			l_time(LT_SSHLD) += 120;
			l_ltime(LT_SSHLD) = t;
		}
		if(equipment_sets(obj_ptr, PSILNC)) {
			l_time(LT_SILNC) += 120;
			l_ltime(LT_SILNC) = t;
		}
		if(equipment_sets(obj_ptr, PCHARM)) {
			l_time(LT_CHRMD) += 120;
			l_ltime(LT_CHRMD) = t;
		}
		if(equipment_sets(obj_ptr, PBESRK)) {
			l_time(LT_BSRKN) += 120;
			l_ltime(LT_BSRKN) = t;
		}
		
#undef l_time
#undef l_ltime

        }
        return(0);
}

/****************************************************************/
/*                              equipment_sets                  */
/****************************************************************/
/* Checks the object given in the first structure to see if it  */
/* sets the flag given as 2nd argument  			*/
int equipment_sets(object *obj_ptr, int flag)
{

	if(obj_ptr && FS_ISSET(obj_ptr, flag))
			return(1);
	
	
	return(0);
}

int check_illegal_stats(creature *ply_ptr, object *obj_ptr)
{

	if(!F_ISSET(obj_ptr, OADDSA))
		return(0);

	if(ply_ptr->strength + obj_ptr->strength > 31)
		return(1);
	if(ply_ptr->constitution + obj_ptr->constitution > 31)
		return(1);
	if(ply_ptr->dexterity + obj_ptr->dexterity > 31)
		return(1);
	if(ply_ptr->piety + obj_ptr->piety > 31)
		return(1);
	if(ply_ptr->intelligence + obj_ptr->intelligence > 31)
		return(1);

	if(ply_ptr->intelligence + obj_ptr->intelligence < 0)
		return(1);
	if(ply_ptr->piety + obj_ptr->piety < 0)
		return(1);
	if(ply_ptr->dexterity + obj_ptr->dexterity < 0)
		return(1);
	if(ply_ptr->constitution + obj_ptr->constitution < 0)
		return(1);
	if(ply_ptr->strength + obj_ptr->strength < 0)
		return(1);

	return(0);
}

/* this returns 1 if the player name is allowed through the exit */
int check_ply_ext(char *name, exit_ *ext_ptr)
{
	room		*rom_ptr;
	creature	*own_ptr;

	if ( load_rom(ext_ptr->room, &rom_ptr) < 0 )
        {
                return(0);
        }

	own_ptr = find_rom_owner(rom_ptr);

	if(!strcmp(own_ptr->name, name))
		return(1);
	else
		return(0);
}
