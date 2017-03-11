/*
 * DM8.C:
 *
 *  DM functions
 *
 *  Copyright (C) 1994, 1999	   Brooke Paul
 *
 * $Id: dm8.c,v 6.17 2001/07/21 15:56:15 develop Exp $
 *
 * $Log: dm8.c,v $
 * Revision 6.17  2001/07/21 15:56:15  develop
 * dm_jail fix
 *
 * Revision 6.16  2001/07/21 15:53:32  develop
 * dm_jail change to work with conjure
 *
 * Revision 6.15  2001/07/17 19:28:44  develop
 * *** empty log message ***
 *
 * Revision 6.15  2001/07/14 21:26:44  develop
 * *** empty log message ***
 *
 * Revision 6.14  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

#include "../include/mordb.h"
#include "mextern.h"

int dm_familiar(creature *ply_ptr, cmd *cmnd )
{
/*
    creature  *crt_ptr;
    int fd;
	
    fd = ply_ptr->fd;
    Ply[fd].extr->alias_crt = crt_ptr;
    F_SET(ply_ptr, PALIAS);
    F_SET(crt_ptr, MDMFOL);
*/

	return(PROMPT);
}

int dm_own_ply(creature *ply_ptr, cmd *cmnd )
{
	int	crtloaded=0,fd;
	creature	*crt_ptr;


	if(ply_ptr->class < CARETAKER)
                return(PROMPT);

        fd =ply_ptr->fd;

        lowercize(cmnd->str[1], 1);
    	crt_ptr = find_who(cmnd->str[1]);

	if(!crt_ptr) {
                cmnd->str[1][0] = up(cmnd->str[1][0]);
                if(load_ply(cmnd->str[1], &crt_ptr) < 0){
                        output(fd,"Player does not exist.\n");
                        return (0);
                }
                crtloaded=1;
        }

	if(crt_ptr->class > IMMORTAL) {
			output(fd, "You can't own an immortal!\n");
                        return (0);
        }

        if(!F_ISSET(crt_ptr, PDMOWN)) {
                F_SET(crt_ptr, PDMOWN);
                sprintf(g_buffer, "You own %s.\n", crt_ptr->name);
                output(fd, g_buffer);
        }
        else {
                F_CLR(crt_ptr, PDMOWN);
                sprintf(g_buffer, "%s is no longer owned.\n", crt_ptr->name);
                output(fd, g_buffer);
        }

	if(crtloaded) {
		save_ply(crt_ptr); free_crt(crt_ptr);
                        return(PROMPT);
	}

	if(F_ISSET(crt_ptr, PINVIS)) {
                F_CLR(crt_ptr, PINVIS);
                output(crt_ptr->fd, "Your invisibility fades.\n");
                broadcast_rom(crt_ptr->fd, crt_ptr->rom_num, "%M fades into view.",
                              m1arg(crt_ptr));
        }
	return(PROMPT);

}
 /*
 *
 *     dm_dump_io:   Dumps the io fd table
 * 
 *
 */
 int dm_dump_io(creature *ply_ptr, cmd *cmnd )
 {
         int fd,q,x=0;
 
       if(ply_ptr->class < DM)
                 return(PROMPT);
 
         fd =ply_ptr->fd;
 
       sprintf(g_buffer, "\nDump of Socket Table\n");
       output(fd, g_buffer);
 
       for(q=0; q<Tablesize ; q++)
         {
               if(Ply[q].io) {
                       x+=1;
                       sprintf(g_buffer, "%s\n", Ply[q].io->address);
                       output(fd, g_buffer);
               }
       }
       sprintf(g_buffer, "%d total connections.\n", x);
       output(fd, g_buffer);
       return(PROMPT);
 }

/*
 *	dm_jail:  jails a player, and logs it
 */

int dm_jail(creature *ply_ptr, cmd *cmnd )
{

	int fd, duration=0;
	creature *crt_ptr;
	room	 *rom_ptr;
	ctag	 *cp;

	if(ply_ptr->class < IMMORTAL)
		return(DOPROMPT);

	fd = ply_ptr->fd;

	if(cmnd->num < 2) {
		output(fd, "Syntax: *jail <player> <# days>\n");
		return(0);
	}

	lowercize(cmnd->str[1], 1);
	crt_ptr = find_who(cmnd->str[1]);
	if(!crt_ptr || crt_ptr->class >= IMMORTAL) {
		sprintf(g_buffer, "%s is not on.\n", cmnd->str[1]);
		output(fd, g_buffer);
		return(0);
	}
	
	duration = cmnd->val[1];
	if(duration < 0) {
		output(fd, "Duration must be greater than zero.  Use zero for indefinite.\n");
		return(0);
	}

	if ( load_rom(jail_room_num, &rom_ptr) < 0 ) {
		output(fd, "Failed to load jail room!\n");
		sprintf(g_buffer, "%s tried to jail %s.  Failed to load room %d.",
			ply_ptr->name, crt_ptr->name, jail_room_num);
		logn(JAIL_LOG, g_buffer);
		return(0);
	}

	F_CLR(crt_ptr, PHIDDN);
	F_CLR(crt_ptr, PINVIS);
	del_ply_rom(ply_ptr, ply_ptr->parent_rom);
	add_ply_rom(ply_ptr, rom_ptr);
	cp = ply_ptr->first_fol;
	while(cp) {
		if(cp->crt->type == MONSTER && F_ISSET(cp->crt, MCONJU)) {
			del_crt_rom(cp->crt, cp->crt->parent_rom);
			add_crt_rom(cp->crt, rom_ptr,1);
		}
		cp = cp->next_tag;
	}

	broadcast_rom(crt_ptr->fd, crt_ptr->rom_num, "%M vanishes.", 
			m1arg(crt_ptr));
	del_ply_rom(crt_ptr, crt_ptr->parent_rom);
	add_ply_rom(crt_ptr, rom_ptr);
	
	sprintf(g_buffer, "%s jailed for %d days.\n", crt_ptr->name, duration);
	output(fd, g_buffer);
	sprintf(g_buffer, "%s jailed %s for %d days.", ply_ptr->name, 
			crt_ptr->name, duration);
	logn(JAIL_LOG, g_buffer);
	
	return(0);
}

/***************************************************************************
        This function allows an immortal to toggle his or her afk flag
***************************************************************************/
            
int dm_afk(creature *ply_ptr, cmd *cmnd)
{

if(ply_ptr->class < CARETAKER)       
	return 0;
else
{               
        if (!F_ISSET(ply_ptr, PAFK))
        {
                F_SET(ply_ptr, PAFK);
                output(ply_ptr->fd, "AWAY FROM KEYBOARD MODE NOW ON!\n");
                broadcast_rom(ply_ptr->fd, ply_ptr->rom_num, "%M has just gone AFK!",
                        m1arg(ply_ptr->name));
        }
                
        else
        {
                F_CLR(ply_ptr, PAFK);
                output(ply_ptr->fd, "AWAY FROM KEYBOARD MODE NOW OFF!\n");
                broadcast_rom(ply_ptr->fd, ply_ptr->rom_num, "%M is no longer AFK!",
                        m1arg(ply_ptr->name));
        }
        return(0);
            
    } // end of else
} // end of function dm_afk

/*
*
*	dm_dump_db
*	
*	This function dumps the crt, room, or obj database to a
*	pipe delimited text file.
*
*/

int dm_dump_db ( creature *ply_ptr, cmd *cmnd ) {
	int fd;

	fd = ply_ptr->fd;
	if(ply_ptr->class < DM)
                return(PROMPT);

        if(cmnd->num < 2) {
                output(fd, "Dump what?\n");
                return(0);
        }
	switch(low(cmnd->str[1][0])) {
        case 'r': 
                return(dm_dump_rom(ply_ptr));
        case 'm': 
                return(dm_dump_crt(ply_ptr));
        case 'o': 
                return(dm_dump_obj(ply_ptr));
        default: 
                output(fd, "Invalid option.  *ascdump <r|m|o> <options>\n");
                return(0);
        }
}

/*
*
*	dm_dump_rom
*	
*	This function dumps the room database to a pipe-delimited text file.
*
*/

int dm_dump_rom(creature *ply_ptr) {

	return(0);

}

/*
*
*	dm_dump_crt
*	
*	This function dumps the monster database to a pipe-delimited text file.
*
*/

int dm_dump_crt(creature *ply_ptr) {
	char		str[1024], temp[80];
	creature        *crt_ptr;
	int		fd, i;
	
	if(ply_ptr->class < DM )
		return(PROMPT);

	fd = ply_ptr->fd;

	if(load_crt(1, &crt_ptr) < 0) {
		sprintf(g_buffer, "Error (1)\n");
		output(fd, g_buffer);
		return(0);
	}

	add_active(crt_ptr);

	sprintf(g_buffer, "%s|", crt_ptr->name);
	output(fd, g_buffer);
	sprintf(g_buffer, "%s|", crt_ptr->description);
	output(fd, g_buffer);
	sprintf(g_buffer, "%s|", crt_ptr->talk);
	output(fd, g_buffer);
	sprintf(g_buffer, "%s|%s|%s|",crt_ptr->key[0],crt_ptr->key[1], crt_ptr->key[2]);
	output(fd, g_buffer);
	sprintf(g_buffer, "%d|", atoi(crt_ptr->password));
	output(fd, g_buffer);

	sprintf(g_buffer, "%d|%s|",
	crt_ptr->level, get_race_string(crt_ptr->race));
	output(fd, g_buffer);
	sprintf(g_buffer, "%s|%s|%d|",
	get_class_string(crt_ptr->class),
	F_ISSET(crt_ptr, PCHAOS) ? "Chaotic":"Lawful", (int)crt_ptr->alignment);
	output(fd, g_buffer);

	sprintf(g_buffer, "%d|", (int)crt_ptr->experience);
	output(fd, g_buffer);
	sprintf(g_buffer, "%d|", (int)crt_ptr->gold);
	output(fd, g_buffer);

	if ( crt_ptr->type == PLAYER ) {
		sprintf(g_buffer, "%ld|", crt_ptr->bank_balance);
		output(fd, g_buffer);
	}

	sprintf(g_buffer, "%d|%d|", crt_ptr->hpcur, crt_ptr->hpmax);
	output(fd, g_buffer);

	sprintf(g_buffer, "%d|%d|", crt_ptr->mpcur, crt_ptr->mpmax);
	output(fd, g_buffer);
/*
	sprintf(g_buffer, "%d|", crt_ptr->clan);
	output(fd, g_buffer);
*/
	sprintf(g_buffer, "%d|", crt_ptr->armor);
	output(fd, g_buffer);

	sprintf(g_buffer, "%d|", crt_ptr->thaco);
	output(fd, g_buffer);

	sprintf(g_buffer, "%d|%d|%d|", crt_ptr->ndice, crt_ptr->sdice, crt_ptr->pdice);
	output(fd, g_buffer);

	sprintf(g_buffer, "%d|%d|%d|%d|%d|", 
		crt_ptr->strength, crt_ptr->dexterity, crt_ptr->constitution, 
		crt_ptr->intelligence, crt_ptr->piety);
	output(fd, g_buffer );
	if(F_ISSET(crt_ptr, MPERMT)) output (fd, "1|");
	else output (fd, "0|");
	if(F_ISSET(crt_ptr, MINVIS)) strcat(str, "Invis, ");
	if(F_ISSET(crt_ptr, MHIDDN)) strcat(str, "Hidden, ");
	if(F_ISSET(crt_ptr, MTOMEN)) strcat(str, "men on plural, ");
	if(F_ISSET(crt_ptr, MAGGRE)) strcat(str, "Aggr, ");
	if(F_ISSET(crt_ptr, MGAGGR)) strcat(str, "Good-Aggr, ");
	if(F_ISSET(crt_ptr, MEAGGR)) strcat(str, "Evil-Aggr, ");
	if(F_ISSET(crt_ptr, MGUARD)) strcat(str, "Guard, ");
	if(F_ISSET(crt_ptr, MBLOCK)) strcat(str, "Block, ");
       	if(F_ISSET(crt_ptr, MFOLLO)) strcat(str, "Follow, ");
       	if(F_ISSET(crt_ptr, MFLEER)) strcat(str, "Flee, ");
       	if(F_ISSET(crt_ptr, MSCAVE)) strcat(str, "Scav, ");
       	if(F_ISSET(crt_ptr, MMALES)) strcat(str, "Male, ");
       	if(F_ISSET(crt_ptr, MPOISS)) strcat(str, "Poisoner, ");
       	if(F_ISSET(crt_ptr, MUNDED)) strcat(str, "Undead, ");
       	if(F_ISSET(crt_ptr, MUNSTL)) strcat(str, "No-steal, ");
       	if(F_ISSET(crt_ptr, MPOISN)) strcat(str, "Poisoned, ");
       	if(F_ISSET(crt_ptr, MMAGIC)) strcat(str, "Magic, ");
       	if(F_ISSET(crt_ptr, MHASSC)) strcat(str, "Scavenged, ");
       	if(F_ISSET(crt_ptr, MBRETH)) {
	       	if(!F_ISSET(crt_ptr,MBRWP1) && !F_ISSET(crt_ptr,MBRWP2))
			strcat(str, "BR-fire, ");
		else if(F_ISSET(crt_ptr,MBRWP1) && !F_ISSET(crt_ptr,MBRWP2))
			strcat(str, "BR-acid, ");
		else if(!F_ISSET(crt_ptr,MBRWP1) && F_ISSET(crt_ptr,MBRWP2))
			strcat(str, "BR-frost, ");
		else
			strcat(str, "BR-gas, ");
		}
	if(F_ISSET(crt_ptr, MMGONL)) strcat(str, "Magic-only, ");
	if(F_ISSET(crt_ptr, MBLNDR)) strcat(str, "Blinder, ");
	if(F_ISSET(crt_ptr, MBLIND)) strcat(str, "Blind, ");
	if(F_ISSET(crt_ptr, MCHARM)) strcat(str, "Charmed, ");
	if(F_ISSET(crt_ptr, MMOBIL)) strcat(str, "Mobile, ");
	if(F_ISSET(crt_ptr, MSILNC)) strcat(str, "Mute, ");
	if(F_ISSET(crt_ptr, MMAGIO)) strcat(str, "Cast-percent, ");
	if(F_ISSET(crt_ptr, MRBEFD)) strcat(str, "Resist-stun, ");
	if(F_ISSET(crt_ptr, MNOCIR)) strcat(str, "No-circle, ");
	if(F_ISSET(crt_ptr, MDINVI)) strcat(str, "Detect-invis, ");
	if(F_ISSET(crt_ptr, MENONL)) strcat(str, "Enchant-only, ");
	if(F_ISSET(crt_ptr, MRMAGI)) strcat(str, "Resist-magic, ");
	if(F_ISSET(crt_ptr, MTALKS)) strcat(str, "Talks, ");
	if(F_ISSET(crt_ptr, MUNKIL)) strcat(str, "Unkillable, ");
	if(F_ISSET(crt_ptr, MNRGLD)) strcat(str, "NonrandGold, ");
	if(F_ISSET(crt_ptr, MTLKAG)) strcat(str, "Talk-aggr, ");
	if(F_ISSET(crt_ptr, MENEDR)) strcat(str, "Energy Drain, ");
	if(F_ISSET(crt_ptr, MDISEA)) strcat(str, "Disease, ");
	if(F_ISSET(crt_ptr, MDISIT)) strcat(str, "Dissolve, ");
	if(F_ISSET(crt_ptr, MPURIT)) strcat(str, "Purchase, ");
	if(F_ISSET(crt_ptr, MTRADE)) strcat(str, "Trade, ");
	if(F_ISSET(crt_ptr, MPGUAR)) strcat(str, "P-Guard, ");
	if(F_ISSET(crt_ptr, MDEATH)) strcat(str, "Death scene, ");
	if(F_ISSET(crt_ptr, MDMFOL)) strcat(str, "Possessed, ");
	if(F_ISSET(crt_ptr, MROBOT)) strcat(str, "Bot, ");
	if(F_ISSET(crt_ptr, MIREGP)) strcat(str, "Irrplural, ");
	if(F_ISSET(crt_ptr, MDROPS)) strcat(str, "No s on plural, ");
	if(F_ISSET(crt_ptr, MNOPRE)) strcat(str, "Irrplural, ");
        if(F_ISSET(crt_ptr, MGUILD) ) {
             sprintf(temp, "%s, ", cur_guilds[monstercheck_guild(crt_ptr)].name);
             strcat(str, temp);
        }       
        if(F_ISSET(crt_ptr, MPLDGK) ) strcat(str, "Pldg: ");
        if(F_ISSET(crt_ptr, MRSCND) ) strcat(str, "Rsnd: ");

	if(strlen(str) > 11) {
		str[strlen(str)-2] = '.';
		str[strlen(str)-1] = 0;
	}
	else
		strcat(str, "None.");

	output_nl(fd, str);

	if( F_ISSET(crt_ptr, MTRADE) ) {
		strcpy(str, "Trade items: ");
		for(i=0; i<5; i++) {
			sprintf(temp, "%3d ", crt_ptr->carry[i]);
			strcat(str, temp);
		}
		strcat(str, "\n             ");
		for(i=5; i<10; i++) {
			sprintf(temp, "%3d ", crt_ptr->carry[i]);
			strcat(str, temp);
		}
		output_nl(fd, str);
	}
	else {
		if( F_ISSET(crt_ptr, MPURIT) ) {
			output(fd, "\nItems for sale:\n");
		}
		else {
			output(fd, "\nRandom items dropped:\n");
		}

		for(i=0; i<10; i++) {
			sprintf(g_buffer, "   %3hd", crt_ptr->carry[i]);
			output(fd, g_buffer );
		}
		output(fd, "\n");
	}

	del_active(crt_ptr);
	return(0);
}

/*
*
*	dm_dump_obj
*	
*	This function dumps the object database to a pipe-delimited text file.
*
*/

int dm_dump_obj(creature *ply_ptr) {

	object	*obj_ptr;
	int	fd, count;

	fd = ply_ptr->fd;

	for (count=0; count < 10; count++){

	if(load_obj(count, &obj_ptr) < 0) {
                sprintf(g_buffer, "Error (%d)\n", count);
                output(fd, g_buffer);
                return(0);
        }

	sprintf(g_buffer, "%s|", obj_ptr->name);
	output(fd, g_buffer);
	sprintf(g_buffer, "%s|", obj_ptr->description);
	output(fd, g_buffer);
	sprintf(g_buffer, "%s|", obj_ptr->use_output);
	output(fd, g_buffer);
	sprintf(g_buffer, "%s|%s|%s|",obj_ptr->key[0],obj_ptr->key[1], obj_ptr->key[2]);
	output(fd, g_buffer);
	sprintf(g_buffer, "%d|%d|%d|", obj_ptr->ndice, obj_ptr->sdice,
		obj_ptr->pdice);
	output(fd, g_buffer);

	if(obj_ptr->adjustment) {
		sprintf(g_buffer, "%d|", obj_ptr->adjustment);
		output(fd, g_buffer);
	}
	else {
		output(fd, "0|");
	}

	sprintf(g_buffer, "%d|%d|", obj_ptr->shotscur, obj_ptr->shotsmax);
	output(fd, g_buffer);
        if(obj_ptr->wearflag == WIELD)
		output(fd, "wield|");
	else {
	        if(obj_ptr->wearflag == HELD)
			output(fd, "held|");
		else
			output(fd, "neither|");
	}

	sprintf(g_buffer, "%d|", obj_ptr->type);
	output(fd, g_buffer);
	sprintf(g_buffer, "%2.2d|", obj_ptr->armor);
	output(fd, g_buffer);
	sprintf(g_buffer, "%5.5ld|", obj_ptr->value);
	output(fd, g_buffer);
	sprintf(g_buffer, "%2.2d|", obj_ptr->weight);
	output(fd, g_buffer);
	sprintf(g_buffer, "%2.2d|", obj_ptr->magicpower);
	output(fd, g_buffer);

	if(obj_ptr->questnum) {
		sprintf(g_buffer, "%d|", obj_ptr->questnum);
		output(fd, g_buffer );
	}
	else
		output(fd, "0|");
	if(obj_ptr->special) {
		switch(obj_ptr->special) {
		  case SP_MAPSC: output(fd, "1|");break;
		  case SP_COMBO: output(fd, "2|");break;
		  case SP_HERB_HEAL: output(fd, "3|");break;
		  case SP_HERB_HARM: output(fd, "4|");break;
		  case SP_HERB_POISON: output(fd, "5|");break;
		  case SP_HERB_DISEASE: output(fd, "6|");break;
		  case SP_HERB_CURE_POISON: output(fd, "7|");break;
		  case SP_HERB_CURE_DISEASE: output(fd, "8|");break;
	 	  default: output(fd, "9|");
		}
	}
	else
		output(fd, "0|");
	if(F_ISSET(obj_ptr, OPERMT)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OHIDDN)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OINVIS)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OSOMEA)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, ODROPS)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, ONOPRE)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OCONTN)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OWTLES)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OTEMPP)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OPERM2)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, ONOMAG)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OLIGHT)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OGOODO)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OEVILO)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OENCHA)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, ONOFIX)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OCLIMB)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, ONOTAK)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OSCENE)) output(fd, "1|");
	else output(fd, "0|");
/*
	00=all wear, 01=small wear, 10=medium wear, 11=large wear
*/
/*
	if(F_ISSET(obj_ptr, OSIZE1) || F_ISSET(obj_ptr, OSIZE2)) {
		if (F_ISSET(obj_ptr, OSIZE1) && F_ISSET(obj_ptr, OSIZE2))
			output(fd, "large|");
		else
			if (F_ISSET(obj_ptr, OSIZE1))
				output (fd, "medium|");
			else
				output (fd, "small|");
	}
	else output(fd, "all|");
*/
	if(F_ISSET(obj_ptr, OSIZE1)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OSIZE2)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, ORENCH)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OCURSE)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OWEARS)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OUSEFL)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OCNDES)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, ONOMAL)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, ONOFEM)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, ODDICE)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OPLDGK)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OBKSTA)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OCLSEL)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OASSNO)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OBARBO)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OCLERO)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OFIGHO)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OMAGEO)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OPALAO)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, ORNGRO)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OTHIEO)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OBARDO)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OMONKO)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, ONSHAT)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OALCRT)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OLUCKY)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OCURSW)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OHBREW)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OHNGST)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OHPAST)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OHAPLY)) output(fd, "1|");
	else output(fd, "0|");
/*
	if(F_ISSET(obj_ptr, ONPREP)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, ODPREP)) output(fd, "1|");
	else output(fd, "0|");
*/
	if(F_ISSET(obj_ptr, OTMPEN)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, ODRUDO)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OALCHO)) output(fd, "1|");
	else output(fd, "0|");
	if(F_ISSET(obj_ptr, OIREGP)) output(fd, "1|");
	else output(fd, "0|");
	
	if(F_ISSET(obj_ptr, OTMPEN)) {
		sprintf(g_buffer, "%d|", ((int)obj_ptr->magicpower+(int)obj_ptr->magicrealm)*40);
		output(fd, g_buffer);
	}
	output (fd, "\n");
	}
	return(0);
}
