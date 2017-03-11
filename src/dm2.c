/*
 * DM2.C:
 *
 *	DM functions
 *
 *	Copyright (C) 1991, 1992, 1993 Brooke Paul
 *
 * $Id: dm2.c,v 6.28 2001/07/25 02:55:04 develop Exp $
 *
 * $Log: dm2.c,v $
 * Revision 6.28  2001/07/25 02:55:04  develop
 * fixes for thieves dropping stolen items
 * fixes for gold dropping by pkills
 *
 * Revision 6.28  2001/07/24 01:36:23  develop
 * *** empty log message ***
 *
 * Revision 6.27  2001/07/21 17:46:10  develop
 * changed player objects to use value instead of
 * special2.  set ONOSEL & ONOFIX
 *
 * Revision 6.26  2001/07/17 19:28:44  develop
 * *** empty log message ***
 *
 * Revision 6.26  2001/07/14 22:55:38  develop
 * added dm_stat_rom for XUNIQP and RPSHOP
 *
 * Revision 6.25  2001/07/04 15:02:04  develop
 * *** empty log message ***
 *
 * Revision 6.24  2001/07/04 14:33:36  develop
 * dm_stat for conjure time yet again
 *
 * Revision 6.23  2001/07/04 14:19:35  develop
 * toying with conjure time output in dm_stat
 *
 * Revision 6.22  2001/07/04 14:12:50  develop
 * dm_stat for conjure change
 *
 * Revision 6.21  2001/07/04 14:09:21  develop
 * added dm_stat check for conjure time remaining
 *
 * Revision 6.20  2001/07/01 14:49:27  develop
 * changed MGROUP to MTEAMM and added MTEAML
 *
 * Revision 6.19  2001/07/01 04:57:15  develop
 * added MGROUP
 *
 * Revision 6.18  2001/04/12 05:00:57  develop
 * small fix to stat_crt
 *
 * Revision 6.17  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

#include "../include/mordb.h"
#include "mextern.h"


/************************************************************************/
/*				dm_stat					*/
/************************************************************************/

/*  This function will allow a DM to display information on an object	*/
/*  creature, player, or room.						*/

int dm_stat(creature *ply_ptr, cmd *cmnd )
{
	room		*rom_ptr;
	object		*obj_ptr;
	creature	*crt_ptr;
	creature	*ply_ptr2;
	int		fd, i, j;

	if(ply_ptr->class < BUILDER )
		return(PROMPT);

	fd = ply_ptr->fd;

	/* Give stats on room DM is in or specified room # */
	if(cmnd->num < 2) {
		if(cmnd->val[0] >= RMAX) return(0);
		if(cmnd->val[0] == 1)
			rom_ptr = ply_ptr->parent_rom;
		else {
			if(load_rom(cmnd->val[0], &rom_ptr) < 0) {
				sprintf(g_buffer, "Error (%d)\n", (int)cmnd->val[0]);
				output(ply_ptr->fd, g_buffer);
				return(0);
			}
		}

		stat_rom(ply_ptr, rom_ptr);
		return(0);
	}

	/*  Use player reference through 2nd parameter or default to DM */
 	if(cmnd->num < 3)
		ply_ptr2 = ply_ptr;
	else { 
		ply_ptr2 = find_crt_in_rom(ply_ptr, ply_ptr->parent_rom,
				    cmnd->str[2], cmnd->val[2], MON_FIRST);
		if(!ply_ptr2)
			ply_ptr2 = find_who(cmnd->str[2]);
		if(!ply_ptr2 || (ply_ptr->class < DM && F_ISSET(ply_ptr2, PDMINV)))
			ply_ptr2 = ply_ptr;
	} 

	rom_ptr = ply_ptr2->parent_rom;

	/* Give info on object, if found */
	obj_ptr = find_obj(ply_ptr2, ply_ptr2->first_obj, cmnd->str[1], 
			   cmnd->val[1]);
	if(!obj_ptr) {
		for(i=0,j=0; i<MAXWEAR; i++) {
			if(EQUAL(ply_ptr2->ready[i], cmnd->str[1])) {
				j++;
				if(j == cmnd->val[1]) {
					obj_ptr = ply_ptr2->ready[i];
					break;
				}
			}
		}
	}
	if(!obj_ptr)
		obj_ptr = find_obj(ply_ptr2, rom_ptr->first_obj,
				   cmnd->str[1], cmnd->val[1]);

	if(obj_ptr) {
		stat_obj(ply_ptr, obj_ptr);
		return(0);
	}

	/*  Search for creature or player to get info on */
	crt_ptr = find_crt_in_rom(ply_ptr, rom_ptr, cmnd->str[1],
			   cmnd->val[1], MON_FIRST);

	if(!crt_ptr) {
		cmnd->str[1][0] = up(cmnd->str[1][0]);
		crt_ptr = find_who(cmnd->str[1]);
	}
	if(crt_ptr && !(ply_ptr->class<BUILDER && F_ISSET(crt_ptr, PDMINV))) {
		stat_crt(ply_ptr, crt_ptr);
		return(0);
	}

	else
		output(fd, "Unable to locate.\n");

	return(0);

}

/************************************************************************/
/*				stat_rom				*/
/************************************************************************/

/*  Display information on room given to player given.			*/

int stat_rom(creature *ply_ptr, room *rom_ptr )
{
	int		i, fd;
	char		str[1024], key[1024], temp[1024];
	xtag		*next_xtag;
	exit_		*ext;

	fd = ply_ptr->fd;

	sprintf(g_buffer, "Room #: %d\n", rom_ptr->rom_num);
	output(fd, g_buffer);
	sprintf(g_buffer, "Name: %s\n", rom_ptr->name);
	output(fd, g_buffer);

	sprintf(g_buffer, "Death Room: %d\n", rom_ptr->death_rom);
	output(fd, g_buffer);

	sprintf(g_buffer, "Traffic: %d%%\n", rom_ptr->traffic);
	output(fd, g_buffer);
	output(fd, "Random monsters:\n");
	for(i=0; i<10; i++) {
		sprintf(g_buffer, "   %3hd", rom_ptr->random[i]);
		output(fd, g_buffer );
	}
	output(fd, "\nPerm objects:\n");
	for(i=0; i<10; i++) {
		sprintf(g_buffer, "   %3hd", rom_ptr->perm_obj[i].misc);
		output(fd, g_buffer );
	}
	output(fd, "\n");
	for(i=0; i<10; i++) {
		sprintf(g_buffer, "   %3hd", (int)rom_ptr->perm_obj[i].interval);
		output(fd, g_buffer);
	}

	output(fd, "\nPerm monsters:\n");
	for(i=0; i<10; i++) {
		sprintf(g_buffer, "   %3hd", rom_ptr->perm_mon[i].misc);
		output(fd, g_buffer);
	}
	output(fd, "\n");
	for(i=0; i<10; i++) {
		sprintf(g_buffer, "   %3hd", (int)rom_ptr->perm_mon[i].interval);
		output(fd, g_buffer);
	}

	output(fd, "\n");
	if(rom_ptr->track && F_ISSET(rom_ptr, RPTRAK)) {
		sprintf(g_buffer, "Perm Tracks: %s.\n", rom_ptr->track);
		output(fd, g_buffer);
	}

	if (rom_ptr->lolevel || rom_ptr->hilevel){
		output(fd, "Level Boundary: ");
		if (rom_ptr->lolevel) {
			sprintf(g_buffer,"%d+ level  ",rom_ptr->lolevel);
			output(fd, g_buffer);
		}
		if (rom_ptr->hilevel) {
			sprintf(g_buffer,"%d- level  ",rom_ptr->hilevel);
			output(fd, g_buffer);
		}
		output(fd,"\n");
	}

	if (rom_ptr->trap) {
		output(fd, "Trap type: ");	
		switch(rom_ptr->trap){
		   case TRAP_PIT:
			sprintf(g_buffer,"Pit Trap (exit rm %d)\n",
				rom_ptr->trapexit);
			output(fd, g_buffer );
		    break;
		   case TRAP_DART:
			output(fd,"Poison Dart Trap\n");
		    break;
		   case TRAP_BLOCK:
			output(fd,"Falling Block Trap\n");
		    break;
		   case TRAP_MPDAM:
			output(fd,"MP Damage Trap\n");
		    break;
		   case TRAP_RMSPL:
			output(fd,"Negate Spell Trap\n");
		    break;
		   case TRAP_NAKED:
			output(fd,"Naked Trap\n");
		    break;
		   case TRAP_ALARM:
			sprintf(g_buffer," Alarm Trap (guard rm %d)\n",
				rom_ptr->trapexit);
			output(fd, g_buffer);
		    break;
		   default:
			output(fd,"invalid trap #\n");
		   break;
		}
	}
	strcpy(str, "Flags set: ");
	if(F_ISSET(rom_ptr, RSHOPP)) strcat(str, "Shoppe, ");
	if(F_ISSET(rom_ptr, RDUMPR)) strcat(str, "DumpRoom, ");
	if(F_ISSET(rom_ptr, RPOWND)) strcat(str, "PlyShoppe, ");
	if(F_ISSET(rom_ptr, RPAWNS)) strcat(str, "PawnShop, ");
	if(F_ISSET(rom_ptr, RTRAIN)) strcat(str, "Train, ");
	if(F_ISSET(rom_ptr, RREPAI)) strcat(str, "Repair, ");
	if(F_ISSET(rom_ptr, RDARKR)) strcat(str, "DarkAlways, ");
	if(F_ISSET(rom_ptr, RDARKN)) strcat(str, "DarkNight, ");
	if(F_ISSET(rom_ptr, RPOSTO)) strcat(str, "PostOffice, ");
	if(F_ISSET(rom_ptr, RNOKIL)) strcat(str, "NoPlyKill, ");
	if(F_ISSET(rom_ptr, RNOTEL)) strcat(str, "NoTeleport, ");
	if(F_ISSET(rom_ptr, RHEALR)) strcat(str, "HealFast, ");
	if(F_ISSET(rom_ptr, RONEPL)) strcat(str, "OnePlayer, ");
	if(F_ISSET(rom_ptr, RTWOPL)) strcat(str, "TwoPlayer, ");
	if(F_ISSET(rom_ptr, RTHREE)) strcat(str, "ThreePlyr, ");
	if(F_ISSET(rom_ptr, RNOMAG)) strcat(str, "NoMagic, ");
	if(F_ISSET(rom_ptr, RPTRAK)) strcat(str, "PermTrack, ");
	if(F_ISSET(rom_ptr, REARTH)) strcat(str, "Earth, ");
	if(F_ISSET(rom_ptr, RWINDR)) strcat(str, "Wind, ");
	if(F_ISSET(rom_ptr, RFIRER)) strcat(str, "Fire, ");
	if(F_ISSET(rom_ptr, RWATER)) strcat(str, "Water, ");
	if(F_ISSET(rom_ptr, RPLWAN)) strcat(str, "Groupwander, ");
	if(F_ISSET(rom_ptr, RPHARM)) strcat(str, "PHarm, ");
	if(F_ISSET(rom_ptr, RPPOIS)) strcat(str, "P-Poision, ");
	if(F_ISSET(rom_ptr, RPMPDR)) strcat(str, "P-mp Drain, ");
	if(F_ISSET(rom_ptr, RPBEFU)) strcat(str, "Confusion, ");
	if(F_ISSET(rom_ptr, RNOLEA)) strcat(str, "No Summon, ");
	if(F_ISSET(rom_ptr, RNOPOT)) strcat(str, "No Potion, ");
	if(F_ISSET(rom_ptr, RPMEXT)) strcat(str, "Pmagic, ");
	if(F_ISSET(rom_ptr, RNOLOG)) strcat(str, "NoLog, ");
	if(F_ISSET(rom_ptr, RELECT)) strcat(str, "Elect, ");
	if(F_ISSET(rom_ptr, RNDOOR)) strcat(str, "InDoors, ");
	if(F_ISSET(rom_ptr, RJAILR)) strcat(str, "Jail, ");
	if(F_ISSET(rom_ptr, RBANKR)) strcat(str, "Bank, ");
	if(F_ISSET(rom_ptr, RPLDGK)) strcat(str, "Guild Room: ");
	if(F_ISSET(rom_ptr, RPLDGK) ) {
             sprintf(temp, "%s ", cur_guilds[roomcheck_guild(rom_ptr)].name);
             strcat(str, temp);
             if( F_ISSET(rom_ptr, RGILDA))
                strcat(str, "(1,");
               else
                strcat(str, "(0,");
             if( F_ISSET(rom_ptr, RGILDB))
                strcat(str, "1,");
               else
                strcat(str, "0,");
             if( F_ISSET(rom_ptr, RGILDC))
                strcat(str, "1), ");
               else
                strcat(str, "0), ");
         }
	if(F_ISSET(rom_ptr, RRSCND)) strcat(str, "Pldg/Rscnd, ");
/* 
	*** Begin old guild output ***
	{
		sprintf(temp, "%s ", cur_guilds[roomcheck_guild(crt_ptr)].name);
		strcat(str, temp);

		if( (F_ISSET(rom_ptr,RGILDA)) &&
		    (!F_ISSET(rom_ptr, RGILDB)) && (!F_ISSET(rom_ptr, RGILDC))) 
			strcat(str, "Guild 1, ");
			if( (!F_ISSET(rom_ptr, RGILDA)) &&
			    (F_ISSET(rom_ptr, RGILDB)) &&
			    (!F_ISSET(rom_ptr, RGILDC)) ) 
				strcat(str, "Guild 2, ");
			if( (!F_ISSET(rom_ptr, RGILDA)) &&
			    (!F_ISSET(rom_ptr, RGILDB)) &&
			    (F_ISSET(rom_ptr, RGILDC)) ) 
				strcat(str, "Guild 3, ");
			if( (F_ISSET(rom_ptr, RGILDA)) &&
			    (F_ISSET(rom_ptr, RGILDB)) &&
			    (!F_ISSET(rom_ptr, RGILDC)) )
				strcat(str, "Guild 4, ");
			if( (F_ISSET(rom_ptr, RGILDA)) &&
			    (!F_ISSET(rom_ptr, RGILDB)) &&
			    (F_ISSET(rom_ptr, RGILDC)) ) 
				strcat(str, "Guild 5, ");
			if( (!F_ISSET(rom_ptr, RGILDA)) &&
			    (F_ISSET(rom_ptr, RGILDB)) &&
			    (F_ISSET(rom_ptr, RGILDC)) ) 
				strcat(str, "Guild 6, ");
			if( (!F_ISSET(rom_ptr, RGILDA)) &&
			    (!F_ISSET(rom_ptr, RGILDB)) &&
			    (!F_ISSET(rom_ptr, RGILDC)) ) 
				strcat(str, "Guild 8, ");
		}
	*** End of old guild output ***
*/


	if(strlen(str) > 13) {
		str[strlen(str)-2] = '.';
		str[strlen(str)-1] = 0;
	}
	else
		strcat(str, "None.");

	output_nl(fd, str);

	output(fd, "Exits:\n");
	next_xtag = rom_ptr->first_ext;
	while(next_xtag) {
		ext = next_xtag->ext;
		sprintf(g_buffer, "  %s: %d", ext->name, ext->room);
		output(fd, g_buffer);

		*str = 0;
		if(F_ISSET(ext, XSECRT)) strcat(str, "Secret, ");
		if(F_ISSET(ext, XINVIS)) strcat(str, "Invisible, ");
		if(F_ISSET(ext, XCLOSD)) strcat(str, "Closed, ");
		if(F_ISSET(ext, XLOCKD)) strcat(str, "Locked, ");
		if(F_ISSET(ext, XLOCKS)) strcat(str, "Lockable ");
		if (F_ISSET(ext, XLOCKD) || F_ISSET(ext, XLOCKS)) {
		    sprintf(key, "Key (%d), ", (unsigned int)ext->key);
	            strcat(str, key);
        	}
		if(F_ISSET(ext, XCLOSS)) strcat(str, "Closable, ");
		if(F_ISSET(ext, XUNPCK)) strcat(str, "Un-pick, ");
		if(F_ISSET(ext, XNAKED)) strcat(str, "Naked, ");
		if(F_ISSET(ext, XCLIMB)) strcat(str, "ClimbUp, ");
		if(F_ISSET(ext, XREPEL)) strcat(str, "ClimbRepel, ");
		if(F_ISSET(ext, XDCLIM)) strcat(str, "HardClimb, ");
		if(F_ISSET(ext, XFLYSP)) strcat(str, "Fly, ");
		if(F_ISSET(ext, XFEMAL)) strcat(str, "Female, ");
		if(F_ISSET(ext, XMALES)) strcat(str, "Male, ");
		if(F_ISSET(ext, XNGHTO)) strcat(str, "Night, ");
		if(F_ISSET(ext, XDAYON)) strcat(str, "Day, ");
		if(F_ISSET(ext, XUNIQP)) strcat(str, "PlySelective, ");
		if(F_ISSET(ext, XPLSEL)) {
			strcat(str ,"Cls-sel: ");
			if (F_ISSET(ext, XPASSN)) strcat(str, "Assn, ");
			if (F_ISSET(ext, XPBARB)) strcat(str, "Barb, ");
			if (F_ISSET(ext, XPBARD)) strcat(str, "Bard, ");
			if (F_ISSET(ext, XPCLER)) strcat(str, "Cler, ");
			if (F_ISSET(ext, XPFGHT)) strcat(str, "Fght, ");
			if (F_ISSET(ext, XPMAGE)) strcat(str, "Mage, ");
			if (F_ISSET(ext, XPPALA)) strcat(str, "Paly, ");
			if (F_ISSET(ext, XPRNGR)) strcat(str, "Rngr, ");
			if (F_ISSET(ext, XPTHEF)) strcat(str, "Thef, ");
			if (F_ISSET(ext, XPDRUD)) strcat(str, "Drud, ");
			if (F_ISSET(ext, XPALCH)) strcat(str, "Alch, ");
			if (F_ISSET(ext, XBUILD)) strcat(str, "Bldr, ");
		}
		if(F_ISSET(ext, XNOSEE)) strcat(str, "No-See, ");
		if(F_ISSET(ext, XPGUAR)) strcat(str, "P-Guard, ");
		if(F_ISSET(ext, XPLDGK)) {
	            	sprintf(temp, "%s ",
				cur_guilds[exitcheck_guild(ext)].name);
			strcat(str, temp);
            	if( F_ISSET(ext, XGILDA))
                	strcat(str, "(1,");
               	else
                	strcat(str, "(0,");
             	if( F_ISSET(ext, XGILDB))
                	strcat(str, "1,");
               	else
               	 	strcat(str, "0,");
             	if( F_ISSET(ext, XGILDC))
                	strcat(str, "1), ");
               	else
                	strcat(str, "0), ");
  	        }
		if(F_ISSET(ext, XRANDM)) { 
			sprintf(g_buffer, "Random(%d, %d), ", (int)ext->random.interval, 
					(int)ext->random.misc);
			strcat(str, g_buffer);
		}
		if(F_ISSET(ext, XRANDB))
			strcat(str, "BroadRandom, ");
			

		if(*str) {
			str[strlen(str)-2] = '.';
			str[strlen(str)-1] = 0;
			sprintf(g_buffer, ", Flags: %s\n", str);
			output(fd, g_buffer);
		}
		else
			output(fd, ".\n");
		
		next_xtag = next_xtag->next_tag;
	}
	return(0);
}

/************************************************************************/
/*				stat_crt				*/
/************************************************************************/

/*  Display information on creature given to player given.		*/

int stat_crt(creature *ply_ptr, creature *crt_ptr )
{
	char		str[1024], temp[1024];
	int         fd, i,splno;
	time_t		t,x;

	fd = ply_ptr->fd;
	t=time(0);
	if(crt_ptr->type == PLAYER && Ply[crt_ptr->fd].io) {
		/* prevent builders from getting status on players */
		if ( ply_ptr->class < CARETAKER ) {
			output_nl(fd, "You must be a CT or higher to perform this function.");
			return(0);
		}
		if (( ply_ptr->class == CARETAKER ) &&
		    ( crt_ptr->class > CARETAKER)) {
			output_nl(fd, " You cannot *status that player.");
			return(0);
		}

		sprintf(g_buffer, "\n%s the %s:\n",
			crt_ptr->name, title_ply(crt_ptr));
		output(fd, g_buffer);
		sprintf(g_buffer,"Addr: %s@%s    ", Ply[crt_ptr->fd].io->userid,
			Ply[crt_ptr->fd].io->address);
		output(fd, g_buffer);
		sprintf(g_buffer, "Idle: %02ld:%02ld\n",
			(t-Ply[crt_ptr->fd].io->ltime)/60L,
			(t-Ply[crt_ptr->fd].io->ltime)%60L);
		output(fd, g_buffer);
		sprintf(g_buffer, "Room: %-5hd -- %-35.35s\n",
			Ply[crt_ptr->fd].ply->rom_num,
			Ply[crt_ptr->fd].ply->parent_rom->name);
		output(fd, g_buffer);

		if(Ply[crt_ptr->fd].ply->class < DM || ply_ptr->class==DM) {
			sprintf(g_buffer, "Cmd : %s\n\n",
				Ply[crt_ptr->fd].extr->lastcommand);
			output(fd, g_buffer);
		}
        else
            output(fd, "Cmd : l\n\n");

	}
	else {
		sprintf(g_buffer, "Name: %s\n", crt_ptr->name);
		output(fd, g_buffer);
		sprintf(g_buffer, "Desc: %s\n", crt_ptr->description);
		output(fd, g_buffer);
		sprintf(g_buffer, "Talk: %s\n", crt_ptr->talk);
		output(fd, g_buffer);
		sprintf(g_buffer, "Keys: %s %20s%20s\n\n",
			crt_ptr->key[0],crt_ptr->key[1], crt_ptr->key[2]);
		output(fd, g_buffer);
		sprintf(g_buffer, "Index: %d\n", atoi(crt_ptr->password));
		output(fd, g_buffer);
	}

	sprintf(g_buffer, "Level: %-20d       Race: %s\n",
		crt_ptr->level, get_race_string(crt_ptr->race));
	output(fd, g_buffer);

	sprintf(g_buffer, "Class: %-20s  Alignment: %s %d\n",
		get_class_string(crt_ptr->class),
		F_ISSET(crt_ptr, PCHAOS)?"Chaotic":"Lawful",crt_ptr->alignment);
	output(fd, g_buffer);

        if(crt_ptr->type == PLAYER) {
		if(F_ISSET(crt_ptr, PPLDGK)) {
          		sprintf(g_buffer, "Guild: %s",
				cur_guilds[check_guild(crt_ptr)].name);
          		output(fd, g_buffer );
		}
		else
          		output(fd, "Guild: None");
	}
	else {
		if(F_ISSET(crt_ptr, MGUILD)) {
          		sprintf(g_buffer, "Guild: ");
	  		output(fd, g_buffer);
          		sprintf(g_buffer,
				"%s", cur_guilds[monstercheck_guild(crt_ptr)].name); 
          		output(fd, g_buffer );
		}
		else
          		output(fd, "Guild: None");
	}

	if(crt_ptr->type == PLAYER) {
        	if(F_ISSET(crt_ptr, PGILDA))
        		strcpy(temp, " (1,");
          	else
             		strcpy(temp, " (0,");
          
		if(F_ISSET(crt_ptr, PGILDB))
             		strcat(temp, "1,");
          	else
             		strcat(temp, "0,");
          
		if(F_ISSET(crt_ptr, PGILDC))
             		strcat(temp, "1)");
          	else
             		strcat(temp, "0)");
        }

	else if(crt_ptr->type == MONSTER) {
		if(F_ISSET(crt_ptr, MGILDA))
			strcpy(temp, " (1,");
		else
			strcpy(temp, " (0,");
		
		if(F_ISSET(crt_ptr, MGILDB))
			strcat(temp, "1,");
		else
			strcat(temp, "0,");

		if(F_ISSET(crt_ptr, MGILDC))
			strcat(temp, "1)");
		else
			strcat(temp, "0)");
	}

	output(fd, temp);

#define INTERVAL crt_ptr->lasttime[LT_HOURS].interval

	if(crt_ptr->type == PLAYER) {

        output(fd, "       Age: ");
        if(INTERVAL > 86400L)
        {
                sprintf(g_buffer, "[%3d] ", (int)(INTERVAL/86400L));
                output(fd, g_buffer);
        }
        else
                output(fd, "[  0] ");

        output(fd, "day/s ");
        if(INTERVAL > 3600L)
        {
                sprintf(g_buffer, "[%2d] ", (int)((INTERVAL % 86400L)/3600L));
                output(fd, g_buffer);
        }
        else
                output(fd, "[ 0] ");
        output(fd, "hour/s ");

        sprintf(g_buffer, "[%2d] ", (int)((INTERVAL % 3600L)/60L));
        output(fd, g_buffer);
        output(fd, "minute/s");

	}	

	sprintf(g_buffer, "\nExp: %d", (int)crt_ptr->experience);
	output(fd, g_buffer);
	sprintf(g_buffer, "   Gold: %d\n", (int)crt_ptr->gold);
	output(fd, g_buffer);

	if ( crt_ptr->type == PLAYER ) {
		sprintf(g_buffer, "Gold in bank: %ld\n",
			crt_ptr->bank_balance);
		output(fd, g_buffer);
	}

	sprintf(g_buffer, "HP: %d/%d", crt_ptr->hpcur, crt_ptr->hpmax);
	output(fd, g_buffer);

	sprintf(g_buffer, "   MP: %d/%d\n", crt_ptr->mpcur, crt_ptr->mpmax);
	output(fd, g_buffer);
	
/*	sprintf(g_buffer, "Clan: %d\n", crt_ptr->clan);
	output(fd, g_buffer);
*/
	
	sprintf(g_buffer, "AC: %d", crt_ptr->armor);
	output(fd, g_buffer);

	sprintf(g_buffer, "   THAC0: %d\n", crt_ptr->thaco);
	output(fd, g_buffer);
	if(EATNDRINK)
		if(crt_ptr->type == PLAYER) {
			sprintf(g_buffer, "Food: %2d  Drink: %2d\n",
				crt_ptr->talk[6], crt_ptr->talk[5]);
			output(fd, g_buffer);
		}

	if (crt_ptr->type == PLAYER && crt_ptr->class == MONK) {
		sprintf(g_buffer, "Hit: %d-%d+%d\n",
			2 + ((crt_ptr->level - 1)/3),crt_ptr->level+3,
			bonus[(int)crt_ptr->strength] +
			(check_guild(ply_ptr)==6?1:0));
		output(fd, g_buffer);
	}
	else {
		sprintf(g_buffer, "Hit: %dd%d+%d\n", crt_ptr->ndice,
		crt_ptr->sdice, crt_ptr->pdice);
		output(fd, g_buffer);
	}

	sprintf(g_buffer, "Str[%2d]  Dex[%2d]  Con[%2d]  Int[%2d]  Pty[%2d]", 
		crt_ptr->strength, crt_ptr->dexterity, crt_ptr->constitution, 
		crt_ptr->intelligence, crt_ptr->piety);
	output(fd, g_buffer );
	if(crt_ptr->type == PLAYER) {
		sprintf(g_buffer, "  Lck[%2d]", Ply[crt_ptr->fd].extr->luck);
		output(fd, g_buffer );
	}
	output(fd, "\n");

	strcpy(str, "Flags set: ");
	if(crt_ptr->type == PLAYER) {
		sprintf(g_buffer, 
			"Sharp: %ld Thrust: %ld Blunt: %ld Pole: %ld Missile: %ld Hand: %ld\n",
			crt_ptr->proficiency[0], crt_ptr->proficiency[1],
			crt_ptr->proficiency[2], crt_ptr->proficiency[3],
			crt_ptr->proficiency[4], crt_ptr->proficiency[5]);
		output(fd, g_buffer );
		sprintf(g_buffer,
			"Earth: %ld   Wind: %ld   Fire: %ld  Water: %ld\n",
			crt_ptr->realm[0], crt_ptr->realm[1],
			crt_ptr->realm[2], crt_ptr->realm[3]);
		output(fd, g_buffer );

		sprintf(g_buffer,
			"Dtect: %ld   Prot: %ld   Clrc: %ld  Srcry: %ld\n",
			crt_ptr->realm[4], crt_ptr->realm[5],
			crt_ptr->realm[6], crt_ptr->realm[7]);
		output(fd, g_buffer );

		if(F_ISSET(crt_ptr, PBLESS)) strcat(str, "Bless, ");
		if(F_ISSET(crt_ptr, PHIDDN)) strcat(str, "Hidden, ");
		if(F_ISSET(crt_ptr, PINVIS)) strcat(str, "Invis, ");
		if(F_ISSET(crt_ptr, PNOBRD)) strcat(str, "NoBroad, ");
		if(F_ISSET(crt_ptr, PNOLDS)) strcat(str, "NoLong, ");
		if(F_ISSET(crt_ptr, PNOSDS)) strcat(str, "NoShort, ");
		if(F_ISSET(crt_ptr, PNORNM)) strcat(str, "NoName, ");
		if(F_ISSET(crt_ptr, PNOEXT)) strcat(str, "NoExits, ");
		if(F_ISSET(crt_ptr, PNOAAT)) strcat(str, "NoAutoAttk, ");
		if(F_ISSET(crt_ptr, PNOEXT)) strcat(str, "NoWaitMsg, ");
		if(F_ISSET(crt_ptr, PPROTE)) strcat(str, "Protect, ");
		if(F_ISSET(crt_ptr, PDMINV)) strcat(str, "DMInvis, ");
		if(F_ISSET(crt_ptr, PNOCMP)) strcat(str, "Noncompact, ");
		if(F_ISSET(crt_ptr, PMALES)) strcat(str, "Male, ");
		if(F_ISSET(crt_ptr, PWIMPY)) {
			sprintf(temp, "Wimpy%d, ", crt_ptr->WIMPYVALUE);
			strcat(str, temp);
		}
		if(F_ISSET(crt_ptr, PEAVES)) strcat(str, "Eaves, ");
		if(F_ISSET(crt_ptr, PBLIND)) strcat(str, "Blind, ");
		if(F_ISSET(crt_ptr, PCHARM)) strcat(str, "Charmed, ");
		if(F_ISSET(crt_ptr, PLECHO)) strcat(str, "Echo, ");
		if(F_ISSET(crt_ptr, PSECOK)) strcat(str, "Secure, ");
		if(F_ISSET(crt_ptr, PALIAS)) strcat(str, "Alias, ");
		if(F_ISSET(crt_ptr, PAUTHD)) strcat(str, "Authd, ");
		if(F_ISSET(crt_ptr, PNLOGN)) strcat(str, "No-login, ");
		if(F_ISSET(crt_ptr, PPOISN)) strcat(str, "Poisoned, ");
		if(F_ISSET(crt_ptr, PDISEA)) strcat(str, "Diseased, ");
		if(F_ISSET(crt_ptr, PLIGHT)) strcat(str, "Light, ");
		if(F_ISSET(crt_ptr, PPROMP)) strcat(str, "Prompt, ");
		if(F_ISSET(crt_ptr, PHASTE)) strcat(str, "Haste, ");
		if(F_ISSET(crt_ptr, PDMAGI)) strcat(str, "D-magic, ");
		if(F_ISSET(crt_ptr, PDINVI)) strcat(str, "D-invis, ");
		if(F_ISSET(crt_ptr, PPRAYD)) strcat(str, "Pray, ");
		if(F_ISSET(crt_ptr, PPREPA)) strcat(str, "Prepared, ");
		if(F_ISSET(crt_ptr, PLEVIT)) strcat(str, "Levitate, ");
		if(F_ISSET(crt_ptr, PANSIC)) strcat(str, "Ansi, ");
		if(F_ISSET(crt_ptr, PRFIRE)) strcat(str, "R-fire, ");
		if(F_ISSET(crt_ptr, PFLYSP)) strcat(str, "Fly, ");
		if(F_ISSET(crt_ptr, PRMAGI)) strcat(str, "R-magic, ");
		if(F_ISSET(crt_ptr, PKNOWA)) strcat(str, "Know-a, ");
		if(F_ISSET(crt_ptr, PNOSUM)) strcat(str, "Nosummon, ");
		if(F_ISSET(crt_ptr, PIGNOR)) strcat(str, "Ignore-a, ");
		if(F_ISSET(crt_ptr, PRCOLD)) strcat(str, "R-cold, ");
		if(F_ISSET(crt_ptr, PBRWAT)) strcat(str, "Breath-wtr, ");
		if(F_ISSET(crt_ptr, PSSHLD)) strcat(str, "Earth-shld, ");
		if(F_ISSET(crt_ptr, PSILNC)) strcat(str, "Mute, ");
		if(F_ISSET(crt_ptr, PNSUSN)) strcat(str, "Hungry|Thirsty, ");
		if(F_ISSET(crt_ptr, PIGCLA)) strcat(str, "ClassIgnore, ");
		if(F_ISSET(crt_ptr, PIGGLD)) strcat(str, "GuildIgnore, ");
		if(F_ISSET(crt_ptr, PDMOWN)) strcat(str, "Owned, ");
		if(F_ISSET(crt_ptr, PAFK))   strcat(str, "AFK, ");
		if(F_ISSET(crt_ptr, PBESRK)) strcat(str, "Berserk, ");
		if(F_ISSET(crt_ptr, PBARKS)) strcat(str, "Barkskin, ");
		if(F_ISSET(crt_ptr, PSOUND)) strcat(str, "MSP on, ");
		if(F_ISSET(crt_ptr, PPLDGK)) {
			sprintf(temp, "%s ",
				cur_guilds[check_guild(crt_ptr)].name);
			strcat(str, temp);
			if( F_ISSET(crt_ptr, PGILDA))
				strcat(str, "Guild(1,");
			else 
				strcat(str, "Guild(0,");
			if( F_ISSET(crt_ptr, PGILDB))
				strcat(str, "1,");
			else 
				strcat(str, "0,");
			if( F_ISSET(crt_ptr, PGILDC))
				strcat(str, "1), ");
			else 
				strcat(str, "0), ");
		}

	}
	else {
		sprintf(g_buffer,
                       "Earth: %ld    Wind: %ld   Fire: %ld  Water: %ld\n",
                       crt_ptr->realm[0], crt_ptr->realm[1],
                       crt_ptr->realm[2], crt_ptr->realm[3]);
		output(fd, g_buffer );

		if(F_ISSET(crt_ptr, MPERMT)) strcat(str, "Perm, ");
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
		if(F_ISSET(crt_ptr, MHASSC)) strcat(str, "Scavenged, ");
		if(F_ISSET(crt_ptr, MBRETH)) {
			if(!F_ISSET(crt_ptr,MBRWP1) &&
			   !F_ISSET(crt_ptr,MBRWP2))
				strcat(str, "BR-fire, ");
			else if(F_ISSET(crt_ptr,MBRWP1) &&
			       !F_ISSET(crt_ptr,MBRWP2))
				strcat(str, "BR-acid, ");
			else if(!F_ISSET(crt_ptr,MBRWP1) &&
			 	F_ISSET(crt_ptr,MBRWP2))
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
                        sprintf(temp, "%s ",
				cur_guilds[monstercheck_guild(crt_ptr)].name);
                        strcat(str, temp);

	             if( F_ISSET(crt_ptr, MGILDA))
        	        strcat(str, "(1,");
               		else
                	strcat(str, "(0,");
             	     if( F_ISSET(crt_ptr, MGILDB))
                	strcat(str, "1,");
               		else
                	strcat(str, "0,");
             	     if( F_ISSET(crt_ptr, MGILDC))
                	strcat(str, "1), ");
               		else
                	strcat(str, "0), ");
		}
		if(F_ISSET(crt_ptr, MPLDGK) ) strcat(str, "Pledge, ");
		if(F_ISSET(crt_ptr, MRSCND) ) strcat(str, "Rescind, ");
		if(F_ISSET(crt_ptr, MSTOLE) ) strcat(str, "Stolen from, ");	
		if(F_ISSET(crt_ptr, MCONJU) ) strcat(str, "Conjured, ");
		if(F_ISSET(crt_ptr, MTEAMM) ) strcat(str, "Team member, ");
		if(F_ISSET(crt_ptr, MTEAML) ) strcat(str, "Team leader, ");
	}

	if(strlen(str) > 11) {
		str[strlen(str)-2] = '.';
		str[strlen(str)-1] = 0;
	}
	else
		strcat(str, "None.");

	output_nl(fd, str);

        if(crt_ptr->type == PLAYER || 
		(crt_ptr->type == MONSTER && F_ISSET(crt_ptr, MMAGIC)) ) {
		strcpy(str, "Magic set: ");
			for (splno = 0; splno < get_spell_list_size()-1; splno++) {
				if (S_ISSET(crt_ptr, splno)) {
					sprintf (g_buffer, "%s(%d), ",
						get_spell_name(splno),splno+1);
					strcat (str, g_buffer);
				}
			}
	

		if(strlen(str) > 11) {
			str[strlen(str)-2] = '.';
			str[strlen(str)-1] = 0;
		}
		else
			strcat(str, "None.");

		output_nl(fd, str);
	}

	if ( crt_ptr->type==MONSTER && F_ISSET(crt_ptr, MCONJU) ) {
                        x = (crt_ptr->lasttime[LT_CONJU].ltime+crt_ptr->lasttime[LT_CONJU].interval)-t;
                        sprintf(g_buffer, "\nLife remaining: %02ld:%02ld\n", (x)/60L, (x)%60L );
                        output(fd, g_buffer);
        }

	if ( crt_ptr->type==MONSTER ) {
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
				output(fd, g_buffer);
			}
			output(fd, "\n");
		}
	}

/* This creates too much load on most machines */
/* 

	int x, n; 
	if(F_ISSET(crt_ptr, MPERMT) && crt_ptr->type == MONSTER) {
                        n=find_crt_num(crt_ptr);
                        if(!n)
                                output(fd, "Note: Monster is unique.\n");
			else
                        	for(x=0;x<10;x++) {
               				if(ply_ptr->parent_rom->perm_mon[x].misc==n)
						if(ply_ptr->parent_rom->perm_mon[x].interval) {
							sprintf(g_buffer,"Note: Perm interval= %d.\n", ply_ptr->parent_rom->perm_mon[x].interval);
							output(fd, g_buffer);
						}
						else
							output(fd, "Note: Perm interval is invalid.\n");			
				}					
							
        }
*/
return(0);
}

/************************************************************************/
/*				stat_obj				*/
/************************************************************************/

/*  Display information on object given to player given.		*/

int stat_obj(creature *ply_ptr, object *obj_ptr )
{
	char	str[1024], temp[1024];
	int	fd,i;

	fd = ply_ptr->fd;

	sprintf(g_buffer, "Name: %s\n", obj_ptr->name);
	output(fd, g_buffer);
	sprintf(g_buffer, "Desc: %s\n", obj_ptr->description);
	output(fd, g_buffer);
	sprintf(g_buffer, "Use:  %s\n", obj_ptr->use_output);
	output(fd, g_buffer);
	sprintf(g_buffer, "Keys: %s %20s %20s\n\n",obj_ptr->key[0],
		obj_ptr->key[1], obj_ptr->key[2]);
	output(fd, g_buffer);
	sprintf(g_buffer, "Hit: %dd%d + %d", obj_ptr->ndice, obj_ptr->sdice,
		obj_ptr->pdice);
	output(fd, g_buffer);

	if(obj_ptr->adjustment) {
		sprintf(g_buffer, " (+%d)\n", obj_ptr->adjustment);
		output(fd, g_buffer);
	}
	else
		output(fd, "\n");

	sprintf(g_buffer, "Shots: %d/%d\n", obj_ptr->shotscur,
		obj_ptr->shotsmax);
	output(fd, g_buffer);
        if(obj_ptr->wearflag == WIELD) output(fd, "Wear: Wielded    ");
        if(obj_ptr->wearflag == HELD)  output(fd, "Wear: Held       ");   

	output(fd, "Type: ");
	if(obj_ptr->type <= MISSILE) {
		switch(obj_ptr->type) {
		case SHARP: output(fd, "sharp"); break;
		case THRUST: output(fd, "thrusting"); break;
		case BLUNT: output(fd, "blunt"); break;
		case POLE: output(fd, "pole"); break;
		case MISSILE: output(fd, "missile"); break;
		}
		output(fd, " weapon.\n");
	}
	else {
		sprintf(g_buffer, "%d\n", obj_ptr->type);
		output(fd, g_buffer);
	}

	sprintf(g_buffer, "AC: %2.2d", obj_ptr->armor);
	output(fd, g_buffer);
	sprintf(g_buffer, "   Value: %5.5d", (int)obj_ptr->value);
	output(fd, g_buffer);
	sprintf(g_buffer, "   Weight: %2.2d", obj_ptr->weight);
	output(fd, g_buffer);
	sprintf(g_buffer, "   Magic: %2.2d", obj_ptr->magicpower);
	output(fd, g_buffer);

	if(F_ISSET(obj_ptr,OADDSA)) {
	   sprintf(g_buffer, "\nStr: %d  Dex: %d  Int: %d  Pie: %d  Con: %d\n",
		obj_ptr->strength,obj_ptr->dexterity, obj_ptr->intelligence,
		obj_ptr->piety, obj_ptr->constitution);
	   output(fd, g_buffer);
	}

	if(obj_ptr->questnum) {
		sprintf(g_buffer, "   Quest: %d\n", obj_ptr->questnum);
		output(fd, g_buffer );
	}
	else
		output(fd, "\n");
	if(obj_ptr->special) {
		output(fd, "Special: ");
		switch(obj_ptr->special) {
		  case SP_MAPSC: output(fd, "Map or Scroll.\n");break;
		  case SP_COMBO: output(fd, "Combo Lock.\n");break;
		  case SP_HERB_HEAL: output(fd, "Healing herb.\n");break;
		  case SP_HERB_HARM: output(fd, "Harming herb.\n");break;
		  case SP_HERB_POISON: output(fd, "Poison herbs.\n");break;
		  case SP_HERB_DISEASE: output(fd, "Disease herbs.\n");break;
		  case SP_HERB_CURE_POISON: output(fd, "Cure poison herb.\n");
			break;
		  case SP_HERB_CURE_DISEASE: output(fd, "Cure disease herb.\n");
			break;
	 	  default: output(fd, "Unknown.\n");
		}
	}
	strcpy(str, "Flags set: ");
	if(F_ISSET(obj_ptr, OPERMT)) strcat(str, "Pperm, ");
	if(F_ISSET(obj_ptr, OHIDDN)) strcat(str, "Hidden, ");
	if(F_ISSET(obj_ptr, OINVIS)) strcat(str, "Invis, ");
	if(F_ISSET(obj_ptr, OCONTN)) strcat(str, "Cont, ");
	if(F_ISSET(obj_ptr, OWTLES)) strcat(str, "Wtless, ");
	if(F_ISSET(obj_ptr, OTEMPP)) strcat(str, "Tperm, ");
	if(F_ISSET(obj_ptr, OPERM2)) strcat(str, "Iperm, ");
	if(F_ISSET(obj_ptr, ONOMAG)) strcat(str, "Nomage, ");
	if(F_ISSET(obj_ptr, OLIGHT)) strcat(str, "Light, ");
	if(F_ISSET(obj_ptr, OGOODO)) strcat(str, "Good, ");
	if(F_ISSET(obj_ptr, OEVILO)) strcat(str, "Evil, ");
	if(F_ISSET(obj_ptr, OENCHA)) strcat(str, "Ench, ");
	if(F_ISSET(obj_ptr, ONOFIX)) strcat(str, "Nofix, ");
	if(F_ISSET(obj_ptr, OCLIMB)) strcat(str, "Climbing, ");
	if(F_ISSET(obj_ptr, ONOTAK)) strcat(str, "Notake, ");
	if(F_ISSET(obj_ptr, OSCENE)) strcat(str, "Scenery, ");
	if(F_ISSET(obj_ptr, OSIZE1) || F_ISSET(obj_ptr, OSIZE2))
		strcat(str, "Sized, ");
	if(F_ISSET(obj_ptr, ORENCH)) strcat(str, "RandEnch, ");
	if(F_ISSET(obj_ptr, OCURSE)) strcat(str, "Cursed, ");
	if(F_ISSET(obj_ptr, OLUCKY)) strcat(str, "Lucky, ");
	if(F_ISSET(obj_ptr, OWEARS)) strcat(str, "Worn, ");
	if(F_ISSET(obj_ptr, OUSEFL)) strcat(str, "Use-floor, ");
	if(F_ISSET(obj_ptr, OCNDES)) strcat(str, "Devours, ");
	if(F_ISSET(obj_ptr, ONOMAL)) strcat(str, "Nomale, ");
	if(F_ISSET(obj_ptr, ONOFEM)) strcat(str, "Nofemale, ");
	if(F_ISSET(obj_ptr, ONSHAT)) strcat(str, "Shatterproof, ");
	if(F_ISSET(obj_ptr, OALCRT)) strcat(str, "Always crit, ");
	if(F_ISSET(obj_ptr, ODDICE)) strcat(str, "NdS damage, ");
	if(F_ISSET(obj_ptr, OHBREW)) strcat(str, "Brew herb, ");
	if(F_ISSET(obj_ptr, OHPAST)) strcat(str, "Paste herb, ");
	if(F_ISSET(obj_ptr, OHNGST)) strcat(str, "Injest, ");
	if(F_ISSET(obj_ptr, OHAPLY)) strcat(str, "Cream herb, ");
	if(F_ISSET(obj_ptr, ODRUDT)) strcat(str, "Druid Item, ");
	if(F_ISSET(obj_ptr, OALCHT)) strcat(str, "Alchem Item, ");
	if(F_ISSET(obj_ptr, OTMPEN)) strcat(str, "Temp enchant, ");
	if(F_ISSET(obj_ptr, OIREGP)) strcat(str, "Irrplural, ");
	if(F_ISSET(obj_ptr, OPLDGK)) strcat(str, "Pledged: ");
	if(F_ISSET(obj_ptr, OPLDGK)) {
                        sprintf(temp, "%s ",
				cur_guilds[objectcheck_guild(obj_ptr)].name);
                        strcat(str, temp);
                        if( F_ISSET(obj_ptr, OGILDA))
                                strcat(str, "Guild(1,");
                        else
                                strcat(str, "Guild(0,");
                        if( F_ISSET(obj_ptr, OGILDB))
                                strcat(str, "1,");
                        else
                                strcat(str, "0,");
                        if( F_ISSET(obj_ptr, OGILDC))
                                strcat(str, "1), ");
                        else
                                strcat(str, "0), ");
        }

	if(F_ISSET(obj_ptr, OBKSTA)) strcat(str, "BckStb, ");
	if(F_ISSET(obj_ptr, OSTOLE)) strcat(str, "GuildStolen, ");
	if(F_ISSET(obj_ptr, OTHEFT)) strcat(str, "PlyTheft, ");
	if(F_ISSET(obj_ptr, OCLSEL)){
		strcat(str, "Cls-Sel: ");
		if (F_ISSET(obj_ptr, OASSNO)) strcat(str, "Assi, ");
		if (F_ISSET(obj_ptr, OBARBO)) strcat(str, "Barb, ");
		if (F_ISSET(obj_ptr, OBARDO)) strcat(str, "Bard, ");
		if (F_ISSET(obj_ptr, OCLERO)) strcat(str, "Cler, ");
		if (F_ISSET(obj_ptr, OFIGHO)) strcat(str, "Figh, ");
		if (F_ISSET(obj_ptr, OMAGEO)) strcat(str, "Mage, ");
		if (F_ISSET(obj_ptr, OPALAO)) strcat(str, "Paly, ");
		if (F_ISSET(obj_ptr, ORNGRO)) strcat(str, "Rngr, ");
		if (F_ISSET(obj_ptr, OTHIEO)) strcat(str, "Thie, ");
		if (F_ISSET(obj_ptr, ODRUDO)) strcat(str, "Drud, ");
		if (F_ISSET(obj_ptr, OALCHO)) strcat(str, "Alch, ");
	}
	if (F_ISSET(obj_ptr, OMONKO)) strcat(str, "Monk, ");
	if (F_ISSET(obj_ptr, OADDSA)) strcat(str, "SetsAttribute, ");
	if (F_ISSET(obj_ptr, ONOSEL)) strcat(str, "UnSellable, ");
	

	if(strlen(str) > 11) {
		str[strlen(str)-2] = '.';
		str[strlen(str)-1] = 0;
	}
	else
		strcat(str, "None.");

	output_nl(fd, str);


	if(F_ISSET(obj_ptr, OADDSA)) {
	  sprintf(str, "Sets Pflags: ");
	  for(i=0;i<128;i++) {
		if(FS_ISSET(obj_ptr,i)) {
			sprintf(g_buffer,"%d, ",i+1);
			strcat(str, g_buffer);
		}
	  }
	
	if(strlen(str) > 11) {
		str[strlen(str)-2] = '.';
		str[strlen(str)-1] = 0;
	}
	else
		strcat(str, "None.");

	output_nl(fd, str);
	}	/* endif F_ISSET(obj_ptr, OADDSA) */

	if(F_ISSET(obj_ptr, OTMPEN)) {
		sprintf(g_buffer, "Enchant timeout: %d.\n",
			((int)obj_ptr->magicpower+(int)obj_ptr->magicrealm)*40);
		output(fd, g_buffer);
	}
	return(0);
}

/**********************************************************************/
/*				dm_add_rom			      */
/**********************************************************************/

/* This function allows a DM to add a new, empty room to the current  */
/* database of rooms.						      */

int dm_add_rom(creature *ply_ptr, cmd *cmnd )
{
	room	*new_rom;
	char	file[80];
	int	fd, num;


	fd = ply_ptr->fd;

	if(ply_ptr->class < DM)
		return(PROMPT);

	if(!strcmp(cmnd->str[1], "c") && (cmnd->num > 1)) {
                dm_add_crt(ply_ptr, cmnd);
                return(0);
        }
        if(!strcmp(cmnd->str[1], "o") && (cmnd->num > 1)) {
                dm_add_obj(ply_ptr, cmnd);
                return(0);
        }


	if(cmnd->val[1] < 2 || cmnd->num < 2) {
		output(fd, "Index error: please specify room number.\n");
		return(0);
	}

	num = (int)cmnd->val[1];
	get_room_filename( num, file );

	if ( file_exists(file) ) {
		output(fd, "Room already exists.\n");
		return(0);
	}

	new_rom = (room *)malloc(sizeof(room));
	if(!new_rom)
		merror("dm_add_room", FATAL);
	zero(new_rom, sizeof(room));

	new_rom->rom_num = (short) num;
	sprintf(new_rom->name, "Room #%d", num);

	F_SET(new_rom, RNOTEL); /* set room to no tport MJK */
	new_rom->death_rom = 0;

	if ( save_rom_to_file(num, new_rom) != 0 ) {
		output(fd, "Error: Unable to save room to file.\n");
		return(0);
	}
	
	free(new_rom);

	sprintf(g_buffer, "Room #%d created.\n", num);
	output(fd, g_buffer);
	return(0);

}

/**********************************************************************/
/*				dm_spy			      */
/**********************************************************************/
int dm_spy(creature	*ply_ptr, cmd *cmnd )
{

	int 		fd, i;
	creature	*crt_ptr;

	if(HEAVEN) {
		if(ply_ptr->class < DM)
			return(PROMPT);
	}
	else {
		if(ply_ptr->class < CARETAKER)
			return(PROMPT);
	}

	fd = ply_ptr->fd;

	if(cmnd->num < 2 && !F_ISSET(ply_ptr, PSPYON)) {
		output(fd, "Spy on whom?\n");
		return(0);
	}
	
	if(F_ISSET(ply_ptr, PSPYON)) {
		for(i=0; i<Tablesize; i++)
			if(get_spy(i) == fd) {
				set_spy(i, -1);

				if(!HEAVEN) {
					crt_ptr = Ply[i].ply;
					if ( crt_ptr ) {
						if ( ply_ptr->class <= crt_ptr->class ) {
							ANSI(crt_ptr->fd,AFC_RED);
							sprintf(g_buffer,"%s has stopped observing you.\n", ply_ptr->name);
							output(crt_ptr->fd, g_buffer);
							ANSI(crt_ptr->fd,AFC_WHITE);
						}
					}
				}
			}
		F_CLR(ply_ptr, PSPYON);
		output(fd, "Spy mode off.\n");
		return(0);
	}

	cmnd->str[1][0] = up(cmnd->str[1][0]);
	crt_ptr = find_who(cmnd->str[1]);
	if(!crt_ptr) {
		output(fd, "Spy on whom?  Use full names.\n");
		return(0);
	}

	/* CT's can't spy on DM's */
	if(ply_ptr->class < crt_ptr->class) {
		output(fd, "Spy on whom?  Use full names.\n");
		return(0);
	}

	if(get_spy( crt_ptr->fd ) > -1) {
		output(fd, "That person is being spied on already.\n");
		return(0);
	}

	if(!HEAVEN) {
		if ( ply_ptr->class  == crt_ptr->class ) {
			ANSI(crt_ptr->fd, AFC_RED);
			sprintf(g_buffer,"%s is observing you.\n",
				ply_ptr->name);
			output(crt_ptr->fd,g_buffer);
			ANSI(crt_ptr->fd, AFC_WHITE);
		}
	}

	set_spy(crt_ptr->fd, ply_ptr->fd);
	F_SET(ply_ptr, PSPYON);

	if(!HEAVEN)
		F_SET(ply_ptr, PDMINV);
	
	output(fd, "Spy on.  Type *spy to turn it off.\n");
	return(0);
}

/************************************************************************/
/*                              dm_mstat                                */
/************************************************************************/
/*									*/
/*  This function will allow a DM to display information on monster by  */
/*  index number							*/

int dm_mstat(creature *ply_ptr, cmd *cmnd )
{
	char		str[1024], temp[1024];
	creature        *crt_ptr;
	int		fd, i;
	
	if(ply_ptr->class < BUILDER )
		return(PROMPT);

	fd = ply_ptr->fd;

/*	cmnd->num only counts strings :(
	if(cmnd->num < 2) {
		output(fd, "Use: *mstat <monster number>\n");
		return(0);
	}
*/

	if(load_crt(cmnd->val[0], &crt_ptr) < 0) {
		sprintf(g_buffer, "Error (%d)\n", (int)cmnd->val[0]);
		output(fd, g_buffer);
		return(0);
	}

	add_active(crt_ptr);

	sprintf(g_buffer, "Name: %s\n", crt_ptr->name);
	output(fd, g_buffer);
	sprintf(g_buffer, "Desc: %s\n", crt_ptr->description);
	output(fd, g_buffer);
	sprintf(g_buffer, "Talk: %s\n", crt_ptr->talk);
	output(fd, g_buffer);
	sprintf(g_buffer, "Keys: %s %20s%20s\n\n",
		crt_ptr->key[0],crt_ptr->key[1], crt_ptr->key[2]);
	output(fd, g_buffer);
	sprintf(g_buffer, "Index: %d\n", atoi(crt_ptr->password));
	output(fd, g_buffer);

	sprintf(g_buffer, "Level: %-20d       Race: %s\n",
	crt_ptr->level, get_race_string(crt_ptr->race));
	output(fd, g_buffer);
	sprintf(g_buffer, "Class: %-20s  Alignment: %s %d\n\n",
	get_class_string(crt_ptr->class),
	F_ISSET(crt_ptr, PCHAOS) ? "Chaotic":"Lawful", crt_ptr->alignment);
	output(fd, g_buffer);

	sprintf(g_buffer, "Exp: %d", (int)crt_ptr->experience);
	output(fd, g_buffer);
	sprintf(g_buffer, "   Gold: %d\n", (int)crt_ptr->gold);
	output(fd, g_buffer);

	if ( crt_ptr->type == PLAYER ) {
		sprintf(g_buffer, "Gold in bank: %ld\n",
			crt_ptr->bank_balance);
		output(fd, g_buffer);
	}

	sprintf(g_buffer, "HP: %d/%d", crt_ptr->hpcur, crt_ptr->hpmax);
	output(fd, g_buffer);

	sprintf(g_buffer, "   MP: %d/%d\n", crt_ptr->mpcur, crt_ptr->mpmax);
	output(fd, g_buffer);
/*
	sprintf(g_buffer, "Clan: %d\n", crt_ptr->clan);
	output(fd, g_buffer);
*/
	sprintf(g_buffer, "AC: %d", crt_ptr->armor);
	output(fd, g_buffer);

	sprintf(g_buffer, "   THAC0: %d\n", crt_ptr->thaco);
	output(fd, g_buffer);

	sprintf(g_buffer, "Hit: %dd%d+%d\n",
		crt_ptr->ndice, crt_ptr->sdice, crt_ptr->pdice);
	output(fd, g_buffer);

	sprintf(g_buffer, "Str[%2d]  Dex[%2d]  Con[%2d]  Int[%2d]  Pty[%2d]", 
		crt_ptr->strength, crt_ptr->dexterity, crt_ptr->constitution, 
		crt_ptr->intelligence, crt_ptr->piety);
	output(fd, g_buffer );
	output(fd, "\n");
	strcpy(str, "Flags set: ");
	if(F_ISSET(crt_ptr, MPERMT)) strcat(str, "Perm, ");
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
		sprintf(temp, "%s, ",
			cur_guilds[monstercheck_guild(crt_ptr)].name);
		strcat(str, temp);
	}
	if(F_ISSET(crt_ptr, MPLDGK) ) strcat(str, "Pldg, ");
	if(F_ISSET(crt_ptr, MRSCND) ) strcat(str, "Rsnd, ");

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

/************************************************************************/
/*				dm_ostat				*/
/************************************************************************/
/*                                                                      */
/*  This function will allow a DM to display information on items by    */
/*  index number                                                        */

int dm_ostat(creature *ply_ptr, cmd *cmnd )
{
        char    str[1024];
	object  *obj_ptr;
        int     fd;

	if (ply_ptr->class < BUILDER )
		return(PROMPT);

        fd = ply_ptr->fd;

/*  cmnd->num only counts strings :(
	if(cmnd->num < 2) {
		output(fd, "Use: *ostat <object number>\n");
		return(0);
	}
*/

        if(load_obj(cmnd->val[0], &obj_ptr) < 0) {
                sprintf(g_buffer, "Error (%d)\n", (int)cmnd->val[0]);
                output(fd, g_buffer);
                return(0);
        }

        sprintf(g_buffer, "Index: %d\n", (int)cmnd->val[0]);
        output(fd, g_buffer);
        sprintf(g_buffer, "Name: %s\n", obj_ptr->name);
        output(fd, g_buffer);
        sprintf(g_buffer, "Desc: %s\n", obj_ptr->description);
        output(fd, g_buffer);
        sprintf(g_buffer, "Use:  %s\n", obj_ptr->use_output);
        output(fd, g_buffer);
        sprintf(g_buffer, "Keys: %s %20s %20s\n\n",
		obj_ptr->key[0],obj_ptr->key[1], obj_ptr->key[2]);
        output(fd, g_buffer);
        sprintf(g_buffer, "Hit: %dd%d + %d", obj_ptr->ndice, obj_ptr->sdice,
                obj_ptr->pdice);
        output(fd, g_buffer);

        if(obj_ptr->adjustment) {
                sprintf(g_buffer, " (+%d)\n", obj_ptr->adjustment);
                output(fd, g_buffer);
        }
        else
                output(fd, "\n");

        sprintf(g_buffer, "Shots: %d/%d\n",
		obj_ptr->shotscur, obj_ptr->shotsmax);
        output(fd, g_buffer);
        if(obj_ptr->wearflag == WIELD) output(fd, "Wear: Wielded    ");
        if(obj_ptr->wearflag == HELD)  output(fd, "Wear: Held       ");   

        output(fd, "Type: ");
        if(obj_ptr->type <= MISSILE) {
                switch(obj_ptr->type) {
                case SHARP: output(fd, "sharp"); break;
                case THRUST: output(fd, "thrusting"); break;
                case BLUNT: output(fd, "blunt"); break;
                case POLE: output(fd, "pole"); break;
                case MISSILE: output(fd, "missile"); break;
                }
                output(fd, " weapon.\n");
        }
        else
        {
                sprintf(g_buffer, "%d\n", obj_ptr->type);
                output(fd, g_buffer);
        }

        sprintf(g_buffer, "AC: %2.2d", obj_ptr->armor);
        output(fd, g_buffer);
        sprintf(g_buffer, "   Value: %5.5d", (int)obj_ptr->value);
        output(fd, g_buffer);
        sprintf(g_buffer, "   Weight: %2.2d", obj_ptr->weight);
        output(fd, g_buffer);
        sprintf(g_buffer, "   Magic: %2.2d", obj_ptr->magicpower);
        output(fd, g_buffer);

        if(obj_ptr->questnum) {
                sprintf(g_buffer, "   Quest: %d\n", obj_ptr->questnum);
                output(fd, g_buffer );
        }
        else
                output(fd, "\n");
        if(obj_ptr->special) {
                output(fd, "Special: ");
                switch(obj_ptr->special) {
                  case SP_MAPSC: output(fd, "Map or Scroll.\n");break;
                  case SP_COMBO: output(fd, "Combo Lock.\n");break;
                  case SP_HERB_HEAL: output(fd, "Healing herb.\n");break;
                  case SP_HERB_HARM: output(fd, "Harming herb.\n");break;
                  case SP_HERB_POISON: output(fd, "Poison herbs.\n");break;
                  case SP_HERB_DISEASE: output(fd, "Disease herbs.\n");break;
                  case SP_HERB_CURE_POISON: output(fd, "Cure poison herb.\n");
			break;
                  case SP_HERB_CURE_DISEASE: output(fd, "Cure disease herb.\n");
			break;
                  default: output(fd, "Unknown.\n");
                }
        }
        strcpy(str, "Flags set: ");
        if(F_ISSET(obj_ptr, OPERMT)) strcat(str, "Pperm, ");
        if(F_ISSET(obj_ptr, OHIDDN)) strcat(str, "Hidden, ");
        if(F_ISSET(obj_ptr, OINVIS)) strcat(str, "Invis, ");
        if(F_ISSET(obj_ptr, OCONTN)) strcat(str, "Cont, ");
        if(F_ISSET(obj_ptr, OWTLES)) strcat(str, "Wtless, ");
        if(F_ISSET(obj_ptr, OTEMPP)) strcat(str, "Tperm, ");
        if(F_ISSET(obj_ptr, OPERM2)) strcat(str, "Iperm, ");
        if(F_ISSET(obj_ptr, ONOMAG)) strcat(str, "Nomage, ");
        if(F_ISSET(obj_ptr, OLIGHT)) strcat(str, "Light, ");
        if(F_ISSET(obj_ptr, OGOODO)) strcat(str, "Good, ");
        if(F_ISSET(obj_ptr, OEVILO)) strcat(str, "Evil, ");
        if(F_ISSET(obj_ptr, OENCHA)) strcat(str, "Ench, ");
        if(F_ISSET(obj_ptr, ONOFIX)) strcat(str, "Nofix, ");
        if(F_ISSET(obj_ptr, OCLIMB)) strcat(str, "Climbing, ");
        if(F_ISSET(obj_ptr, ONOTAK)) strcat(str, "Notake, ");
        if(F_ISSET(obj_ptr, OSCENE)) strcat(str, "Scenery, ");
        if(F_ISSET(obj_ptr, OSIZE1) || F_ISSET(obj_ptr, OSIZE2))
                strcat(str, "Sized, ");
        if(F_ISSET(obj_ptr, ORENCH)) strcat(str, "RandEnch, ");
        if(F_ISSET(obj_ptr, OCURSE)) strcat(str, "Cursed, ");
        if(F_ISSET(obj_ptr, OLUCKY)) strcat(str, "Lucky, ");
        if(F_ISSET(obj_ptr, OWEARS)) strcat(str, "Worn, ");
        if(F_ISSET(obj_ptr, OUSEFL)) strcat(str, "Use-floor, ");
        if(F_ISSET(obj_ptr, OCNDES)) strcat(str, "Devours, ");
        if(F_ISSET(obj_ptr, ONOMAL)) strcat(str, "Nomale, ");
        if(F_ISSET(obj_ptr, ONOFEM)) strcat(str, "Nofemale, ");
        if(F_ISSET(obj_ptr, ONSHAT)) strcat(str, "Shatterproof, ");
        if(F_ISSET(obj_ptr, OALCRT)) strcat(str, "Always crit, ");
        if(F_ISSET(obj_ptr, ODDICE)) strcat(str, "NdS damage, ");
        if(F_ISSET(obj_ptr, OHBREW)) strcat(str, "Brew herb, ");
        if(F_ISSET(obj_ptr, OHPAST)) strcat(str, "Paste herb, ");
        if(F_ISSET(obj_ptr, OHNGST)) strcat(str, "Injest, ");
        if(F_ISSET(obj_ptr, OHAPLY)) strcat(str, "Cream herb, ");
        if(F_ISSET(obj_ptr, ODRUDT)) strcat(str, "Druid Item, ");
        if(F_ISSET(obj_ptr, OALCHT)) strcat(str, "Alchem Item, ");
        if(F_ISSET(obj_ptr, OTMPEN)) strcat(str, "Temp enchant, ");
        if(F_ISSET(obj_ptr, OBKSTA)) strcat(str, "BckStb ");
        if(F_ISSET(obj_ptr, OIREGP)) strcat(str, "Irrplural. ");

	if(F_ISSET(obj_ptr, OPLDGK)) {
		if( F_ISSET(obj_ptr, OGILDA))
			strcat(str, "Guild(1,");
		else
			strcat(str, "Guild(0,");
		if( F_ISSET(obj_ptr, OGILDB))
			strcat(str, "1,");
		else
			strcat(str, "0,");
		if( F_ISSET(obj_ptr, OGILDC))
			strcat(str, "1), ");
		else
			strcat(str, "0), ");
        }
        if(F_ISSET(obj_ptr, OCLSEL)){
                strcat(str, "Cls-Sel: ");
                if (F_ISSET(obj_ptr, OASSNO)) strcat(str, "Assi, ");
                if (F_ISSET(obj_ptr, OBARBO)) strcat(str, "Barb, ");
                if (F_ISSET(obj_ptr, OBARDO)) strcat(str, "Bard, ");
                if (F_ISSET(obj_ptr, OCLERO)) strcat(str, "Cler, ");
                if (F_ISSET(obj_ptr, OFIGHO)) strcat(str, "Figh, ");
                if (F_ISSET(obj_ptr, OMAGEO)) strcat(str, "Mage, ");
                if (F_ISSET(obj_ptr, OPALAO)) strcat(str, "Paly, ");
                if (F_ISSET(obj_ptr, ORNGRO)) strcat(str, "Rngr, ");
                if (F_ISSET(obj_ptr, OTHIEO)) strcat(str, "Thie, ");
                if (F_ISSET(obj_ptr, ODRUDO)) strcat(str, "Drud, ");
                if (F_ISSET(obj_ptr, OALCHO)) strcat(str, "Alch, ");
        }
        if (F_ISSET(obj_ptr, OMONKO)) strcat(str, "Monk, ");                    

        if(strlen(str) > 11) {
                str[strlen(str)-2] = '.';
                str[strlen(str)-1] = 0;
        }
        else
                strcat(str, "None.");

        output_nl(fd, str);
        if(F_ISSET(obj_ptr, OTMPEN)) {
                sprintf(g_buffer, "Enchant timeout: %d.\n",
			((int)obj_ptr->magicpower+(int)obj_ptr->magicrealm)*40);
			output(fd, g_buffer);
        }
        return(0);
}
