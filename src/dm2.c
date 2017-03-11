/*
 * DM2.C:
 *
 *	DM functions
 *
 *	Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"

/************************************************************************/
/*				dm_stat					*/
/************************************************************************/

/*  This function will allow a DM to display information on an object	*/
/*  creature, player, or room.						*/

int dm_stat(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	room		*rom_ptr;
	object		*obj_ptr;
	creature	*crt_ptr;
	creature	*ply_ptr2;
	char		str[2048];
	int		fd, n, i, j;

	if(ply_ptr->class < CARETAKER)
		return(PROMPT);

	fd = ply_ptr->fd;

	/* Give stats on room DM is in or specified room # */
	if(cmnd->num < 2) {
		if(cmnd->val[0] >= RMAX) return(0);
		if(cmnd->val[0] == 1)
			rom_ptr = ply_ptr->parent_rom;
		else {
			if(load_rom(cmnd->val[0], &rom_ptr) < 0) {
				print(ply_ptr->fd, "Error (%d)\n", cmnd->val[0]);
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
		ply_ptr2 = find_crt(ply_ptr, ply_ptr->parent_rom->first_mon,
				    cmnd->str[2], cmnd->val[2]);
		cmnd->str[2][0] = up(cmnd->str[2][0]);
		if(!ply_ptr2)
			ply_ptr2 = find_crt(ply_ptr,
					    ply_ptr->parent_rom->first_ply,
					    cmnd->str[2], cmnd->val[2]);
		if(!ply_ptr2)
			ply_ptr2 = find_who(cmnd->str[2]);
		if(!ply_ptr2 || (ply_ptr->class<DM && 
		   F_ISSET(ply_ptr2, PDMINV)))
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
	crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[1],
			   cmnd->val[1]);
	cmnd->str[1][0] = up(cmnd->str[1][0]);
	if (!crt_ptr)
		crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply, cmnd->str[1],
				   cmnd->val[1]);
	if(!crt_ptr)
		crt_ptr = find_who(cmnd->str[1]);
	if(crt_ptr && !(ply_ptr->class<CARETAKER && F_ISSET(crt_ptr, PDMINV))) {
		stat_crt(ply_ptr, crt_ptr);
		return(0);
	}

	else
		print(fd, "Unable to locate.\n");

	return(0);

}

/************************************************************************/
/*				stat_rom				*/
/************************************************************************/

/*  Display information on room given to player given.			*/

int stat_rom(ply_ptr, rom_ptr)
creature	*ply_ptr;
room		*rom_ptr;
{
	int		i, fd;
	char		str[1024];
	xtag		*next_xtag;
	exit_		*ext;

	fd = ply_ptr->fd;

	print(fd, "Room #: %d\n", rom_ptr->rom_num);
	print(fd, "Name: %s\n", rom_ptr->name);

	print(fd, "Traffic: %d%%\n", rom_ptr->traffic);
	print(fd, "Random monsters:");
	for(i=0; i<10; i++)
		print(fd, " %3hd", rom_ptr->random[i]);
	print(fd, "\n");

	if (rom_ptr->lolevel || rom_ptr->hilevel){
		print(fd, "Level Boundary: ");
		if (rom_ptr->lolevel)
			print(fd,"%d+ level  ",rom_ptr->lolevel);
		if (rom_ptr->hilevel)
			print(fd,"%d- level  ",rom_ptr->hilevel);
		print(fd,"\n");
	}

	if (rom_ptr->trap) {
		print(fd, "Trap type: ");	
		switch(rom_ptr->trap){
		   case TRAP_PIT:
			print(fd,"Pit Trap (exit rm %d)\n",
				rom_ptr->trapexit);
		    break;
		   case TRAP_DART:
			print(fd,"Poison Dart Trap\n");
		    break;
		   case TRAP_BLOCK:
			print(fd,"Falling Block Trap\n");
		    break;
		   case TRAP_MPDAM:
			print(fd,"MP Damage Trap\n");
		    break;
		   case TRAP_RMSPL:
			print(fd,"Negate Spell Trap\n");
		    break;
		   case TRAP_NAKED:
			print(fd,"Naked Trap\n");
		    break;
		   case TRAP_ALARM:
			print(fd," Alarm Trap (guard rm %d)\n",
				rom_ptr->trapexit);
		    break;
		   default:
			print(fd,"inalid trap #\n");
		   break;
		}
	}
	strcpy(str, "Flags set: ");
	if(F_ISSET(rom_ptr, RSHOPP)) strcat(str, "Shoppe, ");
	if(F_ISSET(rom_ptr, RDUMPR)) strcat(str, "DumpRoom, ");
	if(F_ISSET(rom_ptr, RPAWNS)) strcat(str, "PawnShop, ");
	if(F_ISSET(rom_ptr, RTRAIN)) strcat(str, "train, ");
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
	if(F_ISSET(rom_ptr, RPLDGK)) strcat(str, "Pledge, ");
	if(F_ISSET(rom_ptr, RRSCND)) strcat(str, "Rescind, ");
	if(F_ISSET(rom_ptr, RNOPOT)) strcat(str, "No Potion, ");
	if(F_ISSET(rom_ptr, RPMEXT)) strcat(str, "Pmagic, ");
	if(F_ISSET(rom_ptr, RNOLOG)) strcat(str, "NoLog, ");
	if(F_ISSET(rom_ptr, RELECT)) strcat(str, "Elect, ");
	if(strlen(str) > 13) {
		str[strlen(str)-2] = '.';
		str[strlen(str)-1] = 0;
	}
	else
		strcat(str, "None.");

	print(fd, "%s\n", str);

	print(fd, "Exits:\n");
	next_xtag = rom_ptr->first_ext;
	while(next_xtag) {
		ext = next_xtag->ext;
		print(fd, "  %s: %d", ext->name, ext->room);

		*str = 0;
		if(F_ISSET(ext, XSECRT)) strcat(str, "Secret, ");
		if(F_ISSET(ext, XINVIS)) strcat(str, "Invisible, ");
		if(F_ISSET(ext, XLOCKD)) strcat(str, "Locked, ");
		if(F_ISSET(ext, XCLOSD)) strcat(str, "Closed, ");
		if(F_ISSET(ext, XLOCKS)) strcat(str, "Lockable, ");
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
		if(F_ISSET(ext, XNOSEE)) strcat(str, "No-See, ");
		if(F_ISSET(ext, XPGUAR)) strcat(str, "P-Guard, ");
		if(F_ISSET(ext, XPLDGK))
			if(F_ISSET(ext, XKNGDM)) strcat(str, "Organization 1, ");
			else strcat(str, "Organization 0, ");

		if(*str) {
			str[strlen(str)-2] = '.';
			str[strlen(str)-1] = 0;
			print(fd, ", Flags: %s\n", str);
		}
		else
			print(fd, ".\n");
		
		next_xtag = next_xtag->next_tag;
	}
}

/************************************************************************/
/*				stat_crt				*/
/************************************************************************/

/*  Display information on creature given to player given.		*/

int stat_crt(ply_ptr, crt_ptr)
creature	*ply_ptr;
creature	*crt_ptr;
{
	char		str[1024], temp[20];
	int		i, fd;

	fd = ply_ptr->fd;

	if(crt_ptr->type == PLAYER && Ply[crt_ptr->fd].io) {
		print(fd, "\n%s the %s:\n", crt_ptr->name, title_ply(crt_ptr));
		print(fd, "Addr: %s@%s\n\n", Ply[crt_ptr->fd].io->userid,
			Ply[crt_ptr->fd].io->address);
	}
	else {
		print(fd, "Name: %s\n", crt_ptr->name);
		print(fd, "Desc: %s\n", crt_ptr->description);
		print(fd, "Talk: %s\n", crt_ptr->talk);
	        print(fd, "Keys: %s %+20s%+20s\n\n",crt_ptr->key[0],crt_ptr->key[1], crt_ptr->key[2]);

	}

	print(fd, "Level: %-20d       Race: %s\n",
		crt_ptr->level, race_str[crt_ptr->race]);
	print(fd, "Class: %-20s  Alignment: %s %d\n\n",
		class_str[crt_ptr->class],
		F_ISSET(crt_ptr, PCHAOS) ? "Chaotic":"Lawful", crt_ptr->alignment);

	print(fd, "Exp: %d", crt_ptr->experience);
	print(fd, "   Gold: %d\n", crt_ptr->gold);

	print(fd, "HP: %d/%d", crt_ptr->hpcur, crt_ptr->hpmax);
	print(fd, "   MP: %d/%d\n", crt_ptr->mpcur, crt_ptr->mpmax);

	print(fd, "AC: %d", crt_ptr->armor);
	print(fd, "   THAC0: %d\n", crt_ptr->thaco);

	print(fd, "Hit: %dd%d+%d\n", crt_ptr->ndice, crt_ptr->sdice,
		crt_ptr->pdice);

	print(fd, "Str[%2d]  Dex[%2d]  Con[%2d]  Int[%2d]  Pty[%2d]\n",
	      crt_ptr->strength, crt_ptr->dexterity, crt_ptr->constitution,
	      crt_ptr->intelligence, crt_ptr->piety);

	strcpy(str, "Flags set: ");
	if(crt_ptr->type == PLAYER) {
		print(fd, 
	"Sharp: %ld  Thrust: %ld  Blunt: %ld   Pole: %ld  Missile: %ld\n",
			crt_ptr->proficiency[0], crt_ptr->proficiency[1],
			crt_ptr->proficiency[2], crt_ptr->proficiency[3],
			crt_ptr->proficiency[4]);
		print(fd,
			"Earth: %ld    Wind: %ld   Fire: %ld  Water: %ld\n",
			crt_ptr->realm[0], crt_ptr->realm[1],
			crt_ptr->realm[2], crt_ptr->realm[3]);
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
		if(F_ISSET(crt_ptr, PBRWAT)) strcat(str, "Breath-wtr,, ");
		if(F_ISSET(crt_ptr, PSSHLD)) strcat(str, "Earth-shld, ");
		if(F_ISSET(crt_ptr, PSILNC)) strcat(str, "Mute, ");
		if(F_ISSET(crt_ptr, PFEARS)) strcat(str, "Fear, ");
		if(F_ISSET(crt_ptr, PPLDGK))
			if(F_ISSET(crt_ptr, PKNGDM)) strcat(str, "Organization 1, ");
			else strcat(str, "Organization 0, ");
	}
	else {
		if(F_ISSET(crt_ptr, MPERMT)) strcat(str, "Perm, ");
		if(F_ISSET(crt_ptr, MINVIS)) strcat(str, "Invis, ");
		if(F_ISSET(crt_ptr, MAGGRE)) strcat(str, "Aggr, ");
		if(F_ISSET(crt_ptr, MGAGGR)) strcat(str, "Good-Aggr, ");
		if(F_ISSET(crt_ptr, MEAGGR)) strcat(str, "Evil-Aggr, ");
		if(F_ISSET(crt_ptr, MGUARD)) strcat(str, "Guard, ");
		if(F_ISSET(crt_ptr, MBLOCK)) strcat(str, "Block, ");
		if(F_ISSET(crt_ptr, MFOLLO)) strcat(str, "Follow, ");
		if(F_ISSET(crt_ptr, MFLEER)) strcat(str, "Flee, ");
		if(F_ISSET(crt_ptr, MSCAVE)) strcat(str, "Scav, ");
		if(F_ISSET(crt_ptr, MMALES)) strcat(str, "Male, ");
		if(F_ISSET(crt_ptr, MPOISS)) strcat(str, "Poison, ");
		if(F_ISSET(crt_ptr, MUNDED)) strcat(str, "Undead, ");
		if(F_ISSET(crt_ptr, MUNSTL)) strcat(str, "No-steal, ");
		if(F_ISSET(crt_ptr, MPOISN)) strcat(str, "Poisoned, ");
		if(F_ISSET(crt_ptr, MMAGIC)) strcat(str, "Magic, ");
		if(F_ISSET(crt_ptr, MHASSC)) strcat(str, "Scavenged, ");
		if(F_ISSET(crt_ptr, MBRETH))
			if(!F_ISSET(crt_ptr,MBRWP1) && !F_ISSET(crt_ptr,MBRWP2))
				strcat(str, "BR-fire, ");
			else if(F_ISSET(crt_ptr,MBRWP1) && !F_ISSET(crt_ptr,MBRWP2))
				strcat(str, "BR-acid, ");
			else if(!F_ISSET(crt_ptr,MBRWP1) && F_ISSET(crt_ptr,MBRWP2))
				strcat(str, "BR-frost, ");
			else
				strcat(str, "BR-gas, ");
		if(F_ISSET(crt_ptr, MMGONL)) strcat(str, "Magic-only, ");
		if(F_ISSET(crt_ptr, MBLNDR)) strcat(str, "Blinder, ");
		if(F_ISSET(crt_ptr, MBLIND)) strcat(str, "Blind, ");
		if(F_ISSET(crt_ptr, MCHARM)) strcat(str, "Charmed, ");
		if(F_ISSET(crt_ptr, MTESTM)) strcat(str, "Test, ");
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
		if(F_ISSET(crt_ptr, MFEARS)) strcat(str, "Fear, ");
		if(F_ISSET(crt_ptr, MPGUAR)) strcat(str, "P-Guard, ");
		if(F_ISSET(crt_ptr, MDEATH)) strcat(str, "Death scene, ");
		if(F_ISSET(crt_ptr, MDMFOL)) strcat(str, "DM Follow, ");
		if(F_ISSET(crt_ptr, MPLDGK) ) 
			if(F_ISSET(crt_ptr, MKNGDM)) strcat(str, "Pledge 1, ");
			else strcat(str, "Pledge 0, ");
		if(F_ISSET(crt_ptr, MRSCND) ) 
			if(F_ISSET(crt_ptr, MKNGDM)) strcat(str, "Rescind 1, ");
			else strcat(str, "Rescind 0, ");
	}

	if(strlen(str) > 11) {
		str[strlen(str)-2] = '.';
		str[strlen(str)-1] = 0;
	}
	else
		strcat(str, "None.");
	print(fd, "%s\n", str);

}

/************************************************************************/
/*				stat_obj				*/
/************************************************************************/

/*  Display information on object given to player given.		*/

int stat_obj(ply_ptr, obj_ptr)
creature	*ply_ptr;
object		*obj_ptr;
{
	char	str[1024];
	int	fd;

	fd = ply_ptr->fd;

	print(fd, "Name: %s\n", obj_ptr->name);
	print(fd, "Desc: %s\n", obj_ptr->description);
	print(fd, "Use:  %s\n", obj_ptr->use_output);
	print(fd, "Keys: %s %+20s %+20s\n\n",obj_ptr->key[0],obj_ptr->key[1], obj_ptr->key[2]);
	print(fd, "Hit: %dd%d + %d", obj_ptr->ndice, obj_ptr->sdice,
		obj_ptr->pdice);
	if(obj_ptr->adjustment)
		print(fd, " (+%d)\n", obj_ptr->adjustment);
	else
		print(fd, "\n");

	print(fd, "Shots: %d/%d\n", obj_ptr->shotscur, obj_ptr->shotsmax);

	print(fd, "Type: ");
	if(obj_ptr->type <= MISSILE) {
		switch(obj_ptr->type) {
		case SHARP: print(fd, "sharp"); break;
		case THRUST: print(fd, "thrusting"); break;
		case BLUNT: print(fd, "blunt"); break;
		case POLE: print(fd, "pole"); break;
		case MISSILE: print(fd, "missile"); break;
		}
		print(fd, " weapon.\n");
	}
	else
		print(fd, "%d\n", obj_ptr->type);

	print(fd, "AC: %2.2d", obj_ptr->armor);
	print(fd, "   Value: %5.5d", obj_ptr->value);
	print(fd, "   Weight: %2.2d", obj_ptr->weight);
	if(obj_ptr->questnum)
		print(fd, "   Quest: %d\n", obj_ptr->questnum);
	else
		print(fd, "\n");

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
	if(F_ISSET(obj_ptr, OWEARS)) strcat(str, "Worn, ");
	if(F_ISSET(obj_ptr, OUSEFL)) strcat(str, "Use-floor, ");
	if(F_ISSET(obj_ptr, OCNDES)) strcat(str, "Devours, ");
	if(F_ISSET(obj_ptr, ONOMAL)) strcat(str, "Nomale, ");
	if(F_ISSET(obj_ptr, ONOFEM)) strcat(str, "Nofemale, ");
	if(F_ISSET(obj_ptr, ONSHAT)) strcat(str, "Shatterproof, ");
	if(F_ISSET(obj_ptr, OALCRT)) strcat(str, "Always crit, ");
	if(F_ISSET(obj_ptr, ODDICE)) strcat(str, "NdS damage, ");
	if(F_ISSET(obj_ptr, OPLDGK))
		if(F_ISSET(obj_ptr, OKNGDM)) strcat(str, "Organization 1, ");
		else  strcat(str, "Organization 0, ");
	if(F_ISSET(obj_ptr, OCLSEL)){
		strcat(str, "Cls-Sel: ");
		if (F_ISSET(obj_ptr, OASSNO)) strcat(str, "A, ");
		if (F_ISSET(obj_ptr, OBARBO)) strcat(str, "B, ");
		if (F_ISSET(obj_ptr, OCLERO)) strcat(str, "C, ");
		if (F_ISSET(obj_ptr, OFIGHO)) strcat(str, "F, ");
		if (F_ISSET(obj_ptr, OMAGEO)) strcat(str, "M, ");
		if (F_ISSET(obj_ptr, OPALAO)) strcat(str, "P, ");
		if (F_ISSET(obj_ptr, ORNGRO)) strcat(str, "R, ");
		if (F_ISSET(obj_ptr, OTHIEO)) strcat(str, "T, ");
	}


	if(strlen(str) > 11) {
		str[strlen(str)-2] = '.';
		str[strlen(str)-1] = 0;
	}
	else
		strcat(str, "None.");
	print(fd, "%s\n", str);

}

/**********************************************************************/
/*				dm_add_rom			      */
/**********************************************************************/

/* This function allows a DM to add a new, empty room to the current  */
/* database of rooms.						      */

int dm_add_rom(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	room	*new_rom;
	char	file[80];
	int	fd, ff;

	fd = ply_ptr->fd;

	if(ply_ptr->class < DM)
		return(PROMPT);

	if(cmnd->val[1] < 2) {
		print(fd, "Add what?\n");
		return(0);
	}

	sprintf(file, "%s/r%05d", ROOMPATH, cmnd->val[1]);
	ff = open(file, O_RDONLY, 0);
	if(ff >= 0) {
		close(ff);
		print(fd, "Room already exists.\n");
		return(0);
	}

	new_rom = (room *)malloc(sizeof(room));
	if(!new_rom)
		merror("dm_add_room", FATAL);
	new_rom->rom_num = cmnd->val[1];
	sprintf(new_rom->name, "Room #%d", cmnd->val[1]);

	zero(new_rom, sizeof(room));

	ff = open(file, O_RDWR | O_CREAT, ACC);
	if(ff < 0) {
		print(fd, "Error: Unable to open file.\n");
		return(0);
	}

	if(write_rom(ff, new_rom, 0) < 0) {
		print(fd, "Write failed.\n");
		return(0);
	}

	close(ff);
	free(new_rom);
	print(fd, "Room #%d created.\n", cmnd->val[1]);
	return(0);

}

int dm_spy(ply_ptr, cmnd)
creature 	*ply_ptr;
cmd		*cmnd;
{
	int 		fd, i;
	creature	*crt_ptr;

	if(ply_ptr->class < CARETAKER)
		return(PROMPT);

	fd = ply_ptr->fd;

	if(cmnd->num < 2 && !F_ISSET(ply_ptr, PSPYON)) {
		print(fd, "Spy on whom?\n");
		return(0);
	}
	
	if(F_ISSET(ply_ptr, PSPYON)) {
		for(i=0; i<Tablesize; i++)
			if(Spy[i] == fd) Spy[i] = -1;
		F_CLR(ply_ptr, PSPYON);
		print(fd, "Spy mode off.\n");
		return;
	}

	cmnd->str[1][0] = up(cmnd->str[1][0]);
	crt_ptr = find_who(cmnd->str[1]);
	if(!crt_ptr) {
		print(fd, "Spy on whom?  Use full names.\n");
		return(0);
	}

	if(Spy[crt_ptr->fd] > -1) {
		print(fd, "That person is being spied on already.\n");
		return(0);
	}

    	if(crt_ptr->class >= CARETAKER) {
		if(!(!strcmp(ply_ptr->name, DMNAME2) || !strcmp(ply_ptr->name, DMNAME6))){
		ANSI(crt_ptr->fd,RED);
		print(crt_ptr->fd,"%s is observing you.\n",ply_ptr->name);
		ANSI(crt_ptr->fd,WHITE);
		output_buf();
		}
	}
	Spy[crt_ptr->fd] = ply_ptr->fd;
	F_SET(ply_ptr, PSPYON);
	F_SET(ply_ptr, PDMINV);
	print(fd, "Spy on.  Type *spy to turn it off.\n");
	return(0);
}

