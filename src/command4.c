/*
 * COMMAND4.C:
 *
 *	Additional user routines
 *
 *	Copyright (C) 1991, 1992, 1993, 1997 Brooke Paul & Brett Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"

#ifndef WIN32
        #include <sys/time.h>
#else
        #include <time.h>
#endif

#include <string.h>
#ifdef DMALLOC
  #include "/usr/local/include/dmalloc.h"
#endif

/**********************************************************************/
/*				health				      */
/**********************************************************************/

/* This function shows a player his current hit points, magic points,  */
/* experience, gold and level.					       */

int health(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	int	fd;

	fd = ply_ptr->fd;
	
	if(F_ISSET(ply_ptr, PBLIND)){
		ANSI(fd, RED);
		ANSI(fd, BOLD);
		ANSI(fd, BLINK);
		print(fd, "You're obviously blind!\n");
		ANSI(fd, NORMAL);
		ANSI(fd, WHITE);
		return(0);
	}
	ANSI(fd, WHITE);
	print(fd, "%s the %s (level %d)", ply_ptr->name, title_ply(ply_ptr), ply_ptr->level);
	if(F_ISSET(ply_ptr, PHIDDN)) {
		ANSI(fd, CYAN);
		print(fd, " *Hidden*");
		ANSI(fd, WHITE);
	}
	if(F_ISSET(ply_ptr, PNSUSN)) {
		ANSI(fd, YELLOW);
		print(fd, " *Thirsty|Hungry*");
		ANSI(fd, WHITE);
	}
	if(F_ISSET(ply_ptr, PPOISN)) {
		ANSI(fd, BLINK);
		ANSI(fd, GREEN);
		print(fd, " *Poisoned*");
		ANSI(fd, NORMAL);
	}
	if(F_ISSET(ply_ptr, PCHARM)) {
		ANSI(fd, BOLD);
		ANSI(fd, CYAN);
		print(fd, " *Charmed*");
		ANSI(fd, NORMAL);
	}
	if(F_ISSET(ply_ptr, PSILNC)) {
		ANSI(fd, BLINK);
		ANSI(fd, MAGENTA);
		print(fd, " *Mute*");
		ANSI(fd, NORMAL);
	}
	if(F_ISSET(ply_ptr, PDISEA)) {
		ANSI(fd, BLINK);
		ANSI(fd, RED);	
		print(fd," *Diseased*");
		ANSI(fd, NORMAL);
	}
	ANSI(fd, GREEN);
	print(fd, "\n %3d/%3d Hit Points    %3d/%3d Magic Points",
		ply_ptr->hpcur, ply_ptr->hpmax, ply_ptr->mpcur, ply_ptr->mpmax);
	ANSI(fd, RED);
	print(fd, "    AC: %d\n", ply_ptr->armor/10);
	ANSI(fd, YELLOW);
	print(fd, " %7ld Experience    %7ld Gold Pieces\n", ply_ptr->experience, ply_ptr->gold);
	ANSI(fd, NORMAL);
	ANSI(fd, WHITE);
	return(0);
}

/**********************************************************************/
/*				help				      */
/**********************************************************************/

/* This function allows a player to get help in general, or help for a */
/* specific command.  If help is typed by itself, a list of commands   */
/* is produced.  Otherwise, help is supplied for the command specified */

int help(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	char 	file[80];
	int	fd, c=0, match=0, num=0;
	
	fd = ply_ptr->fd;
	strcpy(file, DOCPATH);

	if(cmnd->num < 2) {
		strcat(file, "/helpfile");
		view_file(fd, 1, file);
		return(DOPROMPT);
	}
	if(!strcmp(cmnd->str[1], "spells")) {
		strcat(file, "/spellfile");
		view_file(fd, 1, file);
		return(DOPROMPT);
	}

	if(!strcmp(cmnd->str[1], "policy")) {
		strcat(file, "/policy");
		view_file(fd, 1, file);
		return(DOPROMPT);
	}

	do {
		if(!strcmp(cmnd->str[1], cmdlist[c].cmdstr)) {
			match = 1;
			num = c;
			break;
		}
		else if(!strncmp(cmnd->str[1], cmdlist[c].cmdstr, 
			strlen(cmnd->str[1]))) {
			match++;
			num = c;
		}
		c++;
	} while(cmdlist[c].cmdno);

	if(num > 200 && ply_ptr->class < CARETAKER) {
		print(fd, "Command not found.\n");
		return(0);
	}

	if(match == 1) {
		sprintf(file, "%s/help.%d", DOCPATH, cmdlist[num].cmdno);
		view_file(fd, 1, file);
		return(DOPROMPT);
	}
	else if(match > 1) {
			print(fd, "Command is not unique.\n");
			return(0);
	}

	c = num = 0;
	do {
		if(!strcmp(cmnd->str[1], spllist[c].splstr)) {
			match = 1;
			num = c;
			break;
		}
		else if(!strncmp(cmnd->str[1], spllist[c].splstr, 
			strlen(cmnd->str[1]))) {
			match++;
			num = c;
		}
		c++;
	} while(spllist[c].splno != -1);

	if(match == 0) {
		print(fd, "That command does not exist.\n");
		return(0);
	}
	else if(match > 1) {
		print(fd, "Spell name not unique.\n");
		return(0);
	}

	sprintf(file, "%s/spell.%d", DOCPATH, spllist[num].splno);
	view_file(fd, 1, file);
	return(DOPROMPT);
}

/**********************************************************************/
/*				welcome				      */
/**********************************************************************/

/* Outputs welcome file to user, giving him/her info on how to play   */
/* the game 							      */

int welcome(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	char	file[80];
	int	fd;

	fd = ply_ptr->fd;

	sprintf(file, "%s/welcome", DOCPATH);

	view_file(fd, 1, file);
	return(DOPROMPT);
}

/**********************************************************************/
/*				info				      */
/**********************************************************************/

/* This function displays a player's entire list of information, including */
/* player stats, proficiencies, level and class.			   */

int info(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	char	alstr[8], tmp[80];
	int 	fd, cnt;
	long	expneeded, lv;

	fd = ply_ptr->fd;

	update_ply(ply_ptr);

	if(ply_ptr->level < MAXALVL)
		expneeded = needed_exp[ply_ptr->level -1];
	else
		expneeded = (long)((needed_exp[MAXALVL-1]*ply_ptr->level));   

	if(ply_ptr->alignment < -100)
		strcpy(alstr, "Evil");
	else if(ply_ptr->alignment < 101)
		strcpy(alstr, "Neutral");
	else
		strcpy(alstr, "Good");

	for(lv=0,cnt=0; lv<MAXWEAR; lv++)
		if(ply_ptr->ready[lv]) cnt++;
	cnt += count_inv(ply_ptr, 0);

        print(fd, "\n\n->");
	print(fd, "You are ");
	print(fd, "%s the %s\n", ply_ptr->name, title_ply(ply_ptr));
        print(fd, "->");
	print(fd, "Description: ");
	print(fd, "%s\n", ply_ptr->description);
	print(fd, "\nLevel: ");
	print(fd, "%-20d          ", ply_ptr->level);
	print(fd, "Race: ");
	print(fd, "%-20s         ", race_str[ply_ptr->race]);
	print(fd, "\nClass: ");
	print(fd, "%-20s  ", class_str[ply_ptr->class]);
	print(fd, "Alignment: ");
	print(fd, "%-8s %-8s               ", F_ISSET(ply_ptr, PCHAOS) ? "Chaotic" : "Lawful", alstr);
        print(fd, "\n");

#define INTERVAL ply_ptr->lasttime[LT_HOURS].interval

	print(fd, "Time of play: ");
	if(INTERVAL > 86400L)
		print(fd, "[%3d] ", INTERVAL/86400L);
	else
		print(fd, "[  0] ");
	print(fd, "day/s ");
	if(INTERVAL > 3600L)
		print(fd, "[%2d] ", (INTERVAL % 86400L)/3600L);
	else
		print(fd, "[ 0] ");
	print(fd, "hour/s ");
	print(fd, "[%2d] ", (INTERVAL % 3600L)/60L);
	print(fd, "minute/s                     ");

	print(fd, "\n\nSTR: ");
	print(fd, "%-2d        ", ply_ptr->strength);
	print(fd, "DEX: ");
	print(fd, "%-2d        ", ply_ptr->dexterity);
	print(fd, "CON: ");
	print(fd, "%-2d        ", ply_ptr->constitution);
	print(fd, "INT: ");
	print(fd, "%-2d        ", ply_ptr->intelligence);
	print(fd, "PTY: ");
	print(fd, "%-2d     ", ply_ptr->piety);
	print(fd, "\n\n  Hit Points[Hp]: ");
	print(fd, "%5d/%-5d     ", ply_ptr->hpcur, ply_ptr->hpmax);
	print(fd, "Exp: ");
	print(fd, "%9lu /%-9lu ", ply_ptr->experience, MAX(0, expneeded-ply_ptr->experience));
	print(fd, "next level  ");
	print(fd, "\nMagic Points[Mp]: ");
	print(fd, "%5d/%-5d                    ", ply_ptr->mpcur, ply_ptr->mpmax);
	print(fd, "Gold: ");
	print(fd, "%-9lu       ", ply_ptr->gold);
	print(fd, "\nArmour class[AC]:     ");
	print(fd, "%-4d          ", ply_ptr->armor/10);
	print(fd, "Inventory Weight: ");
	print(fd, "%-4d ", weight_ply(ply_ptr));
	print(fd, "Lbs/");
	print(fd, "%-3d ", cnt);
	print(fd, "Obj");
        print(fd, "\n");
	print(fd, "\nProficiencies:                                                          ");
	print(fd, "\nSharp:  ");
	print(fd, "%2d%%   ", profic(ply_ptr, SHARP));
	print(fd, "Thrust: ");
	print(fd, "%2d%%   ", profic(ply_ptr, THRUST));
	print(fd, "Blunt:  ");
	print(fd, "%2d%%   ", profic(ply_ptr, BLUNT));
	print(fd, "Pole:   ");
	print(fd, "%2d%%   ", profic(ply_ptr, POLE));
	print(fd, "Missile: ");
	print(fd, "%2d%%    ", profic(ply_ptr, MISSILE));
	print(fd, "\n\nRealms:                                                                 ");
	print(fd, "\nEarth:  ");
	print(fd, "%3d%%   ", mprofic(ply_ptr, EARTH));
	print(fd, "Wind:   ");
	print(fd, "%3d%%   ", mprofic(ply_ptr, WIND));
	print(fd, "Fire:   ");
	print(fd, "%3d%%   ", mprofic(ply_ptr, FIRE));
	print(fd, "Water:  ");
	print(fd, "%3d%%               ", mprofic(ply_ptr, WATER));
        print(fd, "\n");
	
	F_SET(ply_ptr, PREADI);
	if(ANSILINE)
		ask_for(fd, "[Hit Return, Q to quit]: ");
	else
		print(fd, "[Hit Return, Q to Quit]: ");
	output_buf();
	Ply[fd].io->intrpt &= ~1;
	Ply[fd].io->fn = info_2;
	Ply[fd].io->fnparam = 1;
	return(DOPROMPT);
}

/************************************************************************/
/*				info_2					*/
/************************************************************************/

/* This function is the second half of info which outputs spells	*/

void info_2(fd, param, instr)
int 	fd, param;
char 	*instr;
{
	char		str[2048];
	char		spl[128][20];
	int		i, j;
	creature	*ply_ptr;

	ply_ptr = Ply[fd].ply;

	if(instr[0]) {
		print(fd, "Aborted.\n");
		F_CLR(ply_ptr, PREADI);
		RETURN(fd, command, 1);
	}

	strcpy(str, "\nSpells known: ");
	for(i=0,j=0; i< spllist_size; i++)
		if(S_ISSET(ply_ptr, i))
			strcpy(spl[j++], spllist[i].splstr);

	if(!j)
		strcat(str, "None.");
	else {
		qsort((void *)spl, j, 20, strcmp);
		for(i=0; i<j; i++) {
			strcat(str, spl[i]);
			strcat(str, ", ");
		}
		str[strlen(str)-2] = '.';
		str[strlen(str)-1] = 0;
	}
	print(fd, "%s\n", str);

	strcpy(str, "Spells under: ");
	if(F_ISSET(ply_ptr, PBLESS)) strcat(str, "bless, ");
	if(F_ISSET(ply_ptr, PLIGHT)) strcat(str, "light, ");
	if(F_ISSET(ply_ptr, PPROTE)) strcat(str, "protection, ");
	if(F_ISSET(ply_ptr, PINVIS)) strcat(str, "invisibility, ");
	if(F_ISSET(ply_ptr, PDINVI)) strcat(str, "detect-invisible, ");
	if(F_ISSET(ply_ptr, PDMAGI)) strcat(str, "detect-magic, ");
	if(F_ISSET(ply_ptr, PLEVIT)) strcat(str, "levitation, ");
	if(F_ISSET(ply_ptr, PRFIRE)) strcat(str, "resist-fire, ");
	if(F_ISSET(ply_ptr, PFLYSP)) strcat(str, "fly, ");
	if(F_ISSET(ply_ptr, PRMAGI)) strcat(str, "resist-magic, ");
	if(F_ISSET(ply_ptr, PKNOWA)) strcat(str, "know-aura, ");
	if(F_ISSET(ply_ptr, PRCOLD)) strcat(str, "resist-cold, ");
	if(F_ISSET(ply_ptr, PBRWAT)) strcat(str, "breathe-water, ");
	if(F_ISSET(ply_ptr, PSSHLD)) strcat(str, "earth-shield, ");
	if(strlen(str) == 14)
		strcat(str, "None.");
	else {
		str[strlen(str)-2] = '.';
		str[strlen(str)-1] = 0;
	}
	print(fd, "%s\n\n", str);

	F_CLR(Ply[fd].ply, PREADI);
	RETURN(fd, command, 1);
}

/**********************************************************************/
/*				psend				      */
/**********************************************************************/

/* This function allows a player to send a message to another player.  If */
/* the other player is logged in, the message is sent successfully.       */

int psend(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	creature	*crt_ptr = 0;
	etag		*ign;
	int			spaces=0, i, j, fd, len;

	fd = ply_ptr->fd;

	if(cmnd->num < 2) {
		print(fd, "Send to whom?\n");
		return 0;
	}

	if(!F_ISSET(ply_ptr, PSECOK)) {
		print(fd, "You may not do that yet.\n");
		return(0);
	}

	cmnd->str[1][0] = up(cmnd->str[1][0]);
	for(i=0; i<Tablesize; i++) {
		if(!Ply[i].ply) continue;
		if(Ply[i].ply->fd == -1) continue;
		if(F_ISSET(Ply[i].ply, PDMINV) && ply_ptr->class < CARETAKER) 
			continue;
		if(!strncmp(Ply[i].ply->name, cmnd->str[1], 
		   strlen(cmnd->str[1])))
			crt_ptr = Ply[i].ply;
		if(!strcmp(Ply[i].ply->name, cmnd->str[1]))
			break;
	}
	if(!crt_ptr) {
		print(fd, "Send to whom?\n");
		return(0);
	}
	if(ply_ptr->class < CARETAKER && (F_ISSET(crt_ptr, PINVIS) && !F_ISSET(ply_ptr, PDINVI))) {
		print(fd, "Send to whom?\n");
		return(0);
	}
 	if(F_ISSET(crt_ptr, PIGNOR) && (ply_ptr->class < CARETAKER)) {
		print(fd, "%s is ignoring everyone.\n", crt_ptr->name);
		return(0);
	}

	ign = Ply[crt_ptr->fd].extr->first_ignore;
	while(ign) {
		if(!strcmp(ign->enemy, ply_ptr->name)) {
			print(fd, "%s is ignoring you.\n", crt_ptr->name);
			return(0);
		}
		ign = ign->next_tag;
	}
	
	len = strlen(cmnd->fullstr);
	for(i=0; i< len && i<256; i++) {
		if(cmnd->fullstr[i] == ' ' && cmnd->fullstr[i+1] != ' ')
			spaces++;
		if(spaces==2) break;
	}
	cmnd->fullstr[255] = 0;
	/* Check for modem escape code */
        for(j=0; j<len && j < 256; j++) {
                if(cmnd->fullstr[j] == '+' && cmnd->fullstr[j+1] == '+') {
                        spaces=0;
                        break;
                }
        }

	if(spaces < 2 || strlen(&cmnd->fullstr[i+1]) < 1) {
		print(fd, "Send what?\n");
		return(0);
	}
	if(F_ISSET(ply_ptr, PSILNC)) {
		print(fd, "You can't speak.\n");
		return(0);
	}
	if(F_ISSET(ply_ptr, PLECHO)){
		ANSI(fd, CYAN);
		print(fd, "You sent: \"%s\" to %M.\n", &cmnd->fullstr[i+1], crt_ptr);
		ANSI(fd, NORMAL)
	}
	else
		print(fd, "Message sent to %s.\n", crt_ptr->name);
	
	print(crt_ptr->fd, "### %M just flashed, \"%s\".\n", ply_ptr,
	      &cmnd->fullstr[i+1]);
	
	if(F_ISSET(ply_ptr, PDMINV) && crt_ptr->class < CARETAKER) {
		print(fd, "They will be unable to reply.\n");
		if(F_ISSET(ply_ptr, PALIAS)) 
			print(fd, "Sent from: %s.\n", Ply[fd].extr->alias_crt);
	}

	if(ply_ptr->class > CARETAKER || crt_ptr->class > CARETAKER)
		return(0);

	broadcast_eaves("--- %s sent to %s, \"%s\".", ply_ptr->name,
		crt_ptr->name, &cmnd->fullstr[i+1]);

	return(0);

}

/**********************************************************************/
/*				broadsend			      */
/**********************************************************************/

/* This function is used by players to broadcast a message to every   */
/* player in the game.  Broadcasts by players are of course limited,  */
/* so the number used that day is checked before the broadcast is     */
/* allowed.							      */

int broadsend(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	int	i, j, found=0, fd;
	int 	len;

	fd = ply_ptr->fd;
	len = strlen(cmnd->fullstr);
	for(i=0; i<len && i < 256; i++) {
		if(cmnd->fullstr[i] == ' ' && cmnd->fullstr[i+1] != ' ')
			found++;
		if(found==1) break;
	}
	cmnd->fullstr[255] = 0;

	/* Check for modem escape code */
        for(j=0; j<len && j < 256; j++) {
                if(cmnd->fullstr[j] == '+' && cmnd->fullstr[j+1] == '+') {
                        found=0;
                        break;
                }
        }

	if(found < 1 || strlen(&cmnd->fullstr[i+1]) < 1) {
		print(fd, "Send what?\n");
		return(0);
	}

	if(RFC1413) {
		if((!strcmp(Ply[fd].io->userid, "no_port") || !strcmp(Ply[fd].io->userid, "unknown")) && !F_ISSET(ply_ptr, PAUTHD)){
			print(fd, "You are not authorized to broadcast.\n");
			return(0);
		}
	} /* RFC1413 */

	if(!F_ISSET(ply_ptr, PSECOK)) {
		print(fd, "You may not do that yet.\n");
		return(0);
	}

	if(!HEAVEN)
		if(!dec_daily(&ply_ptr->daily[DL_BROAD])) {
			print(fd,"You've used up all your broadcasts today.\n");
			return(0);
		}

	if(F_ISSET(ply_ptr, PSILNC)) {
		print(fd, "Your voice is too weak to do that.\n");
		return(0);
	}
	print(fd, "Message broadcast.\n");
	broadcast("### %M broadcasted, \"%s\".", ply_ptr, &cmnd->fullstr[i+1]);

	return(0);

}

/**********************************************************************/
/*				follow				      */
/**********************************************************************/

/* This command allows a player (or a monster) to follow another player. */
/* Follow loops are not allowed; i.e. you cannot follow someone who is   */
/* following you.  Also, you cannot follow yourself.			 */

int follow(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	creature	*old_ptr, *new_ptr;
	room		*rom_ptr;
	ctag		*cp, *pp, *prev;
	int		fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(cmnd->num < 2) {
		print(fd, "Follow who?\n");
		return(0);
	}

	F_CLR(ply_ptr, PHIDDN);
	lowercize(cmnd->str[1], 1);
	new_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
			   cmnd->str[1], cmnd->val[1]);

	if(!new_ptr) {
		print(fd, "No one here by that name.\n");
		return(0);
	}

	if(new_ptr == ply_ptr && !ply_ptr->following) {
		print(fd, "You can't follow yourself.\n");
		return(0);
	}

	if(new_ptr->following == ply_ptr) {
		print(fd, "You can't.  %s's following you.\n",
		      F_ISSET(new_ptr, PMALES) ? "He":"She");
		return(0);
	}

	if(ply_ptr->following) {
		old_ptr = ply_ptr->following;
		cp = old_ptr->first_fol;
		if(cp->crt == ply_ptr) {
			old_ptr->first_fol = cp->next_tag;
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
		print(fd, "You stop following %s.\n", old_ptr->name);
		if(!F_ISSET(ply_ptr, PDMINV))
			print(old_ptr->fd, "%M stops following you.\n", 
			      ply_ptr);
	}

	if(ply_ptr == new_ptr)
		return(0);

	ply_ptr->following = new_ptr;

	pp = (ctag *)malloc(sizeof(ctag));
	if(!pp)
		merror("follow", FATAL);
	pp->crt = ply_ptr;
	pp->next_tag = 0;

	if(!new_ptr->first_fol)
		new_ptr->first_fol = pp;
	else {
		pp->next_tag = new_ptr->first_fol;
		new_ptr->first_fol = pp;
	}

	print(fd, "You start following %s.\n", new_ptr->name);
	if(!F_ISSET(ply_ptr, PDMINV)) {
		print(new_ptr->fd, "%M starts following you.\n", ply_ptr);
		broadcast_rom2(fd, new_ptr->fd, ply_ptr->rom_num,
			      "%M follows %m.", ply_ptr, new_ptr);
	}

	return(0);

}

/**********************************************************************/
/*				lose				      */
/**********************************************************************/

/* This function allows a player to lose another player who might be  */
/* following him.  When successful, that player will no longer be     */
/* following.							      */

int lose(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	creature	*crt_ptr;
	ctag		*cp, *prev;
	int		fd;

	fd = ply_ptr->fd;

if(cmnd->num == 1) {
 
                if (ply_ptr->following == 0){
                        print(fd, "You're not following anyone.\n");
                        return(0);
 
                }
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
                print(fd,"You stop following %m\n",crt_ptr);
                if (!F_ISSET(ply_ptr,PDMINV))
                        print(crt_ptr->fd,"%M stops following you\n",ply_ptr);
                return(0);
        }                   

	F_CLR(ply_ptr, PHIDDN);

	lowercize(cmnd->str[1], 1);
	crt_ptr = find_crt(ply_ptr, ply_ptr->first_fol,
			   cmnd->str[1], cmnd->val[1]);

	if(!crt_ptr) {
		print(fd, "That person is not following you.\n");
		return(0);
	}

	if(crt_ptr->following != ply_ptr) {
		print(fd, "That person is not following you.\n");
		return(0);
	}

	cp = ply_ptr->first_fol;
	if(cp->crt == crt_ptr) {
		ply_ptr->first_fol = cp->next_tag;
		free(cp);
	}
	else while(cp) {
		if(cp->crt == crt_ptr) {
			prev->next_tag = cp->next_tag;
			free(cp);
			break;
		}
		prev = cp;
		cp = cp->next_tag;
	}
	crt_ptr->following = 0;

	print(fd, "You lose %s.\n", F_ISSET(crt_ptr, PMALES) ? "him":"her");
	if(!F_ISSET(ply_ptr, PDMINV)) {
		print(crt_ptr->fd, "%M loses you.\n", ply_ptr);
		broadcast_rom2(fd, crt_ptr->fd, "%M loses %m.", 
			       ply_ptr, crt_ptr);
	}

	return(0);

}

/**********************************************************************/
/*				group				      */
/**********************************************************************/

/* This function allows you to see who is in a group or party of people */
/* who are following you.						*/

int group(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	ctag	*cp;
	char	str[2048];
	int	fd, found = 0;

	fd = ply_ptr->fd;

	cp = ply_ptr->first_fol;
	if(!cp) {
		print(fd, "No one is following you.\n");
		return(0);
	}

	strcpy(str, "People in your group: ");
	while(cp) {
		if(!F_ISSET(cp->crt, PDMINV)) {
			strcat(str, crt_str(cp->crt, 0, 0));
			strcat(str, ", ");
			found = 1;
		}
		cp = cp->next_tag;
	}

	if(!found) {
		print(fd, "No one is following you.\n");
		return(0);
	}

	str[strlen(str)-2] = 0;
	print(fd, "%s.\n", str);

	return(0);

}

/**********************************************************************/
/*				track				      */
/**********************************************************************/

/* This function is the routine that allows rangers and druids to search */
/* for tracks 								 */
/* in a room.  If the ranger is successful, he will be told what dir-    */
/* ection the last person who was in the room left.			 */

int track(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	long	i, t;
	int	fd, chance;

	fd = ply_ptr->fd;

	if(ply_ptr->class != RANGER && ply_ptr->class != DRUID && ply_ptr->class < CARETAKER) {
		print(fd, "Only rangers and druids can track.\n");
		return(0);
	}

	F_CLR(ply_ptr, PHIDDN);

	t = time(0);
	i = LT(ply_ptr, LT_TRACK);

	if(t < i) {
		please_wait(fd, i-t);
		return(0);
	}

	ply_ptr->lasttime[LT_TRACK].ltime = t;
	ply_ptr->lasttime[LT_TRACK].interval = 5 - bonus[ply_ptr->dexterity];
	
	if(F_ISSET(ply_ptr, PBLIND)) {
		print(fd, "You're blind...how can you see tracks?\n");
		return(0);
	}
	chance = 25 + (bonus[ply_ptr->dexterity] + ply_ptr->level)*5;

	if(mrand(1,100) > chance) {
		print(fd, "You fail to find any tracks.\n");
		return(0);
	}

	if(!ply_ptr->parent_rom->track[0]) {
		print(fd, "There are no tracks in this room.\n");
		return(0);
	}

	print(fd, "You find tracks leading to the %s.\n",
	      ply_ptr->parent_rom->track);
	broadcast_rom(fd, ply_ptr->rom_num, "%M finds tracks.", ply_ptr);

	return(0);

}

/**********************************************************************/
/*				peek				      */
/**********************************************************************/

/* This function allows a thief or assassin to peek at the inventory of */
/* another player.  If successful, they will be able to see it and      */
/* another roll is made to see if they get caught.			*/

int peek(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	creature	*crt_ptr;
	room		*rom_ptr;
	char		str[2048];
	long		i, t;
	int		fd, n, chance;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;
	str[0] = 0;

	if(cmnd->num < 2) {
		print(fd, "Peek at who?\n");
		return(0);
	}

	if(ply_ptr->class != THIEF && ply_ptr->class < CARETAKER) {
		print(fd, "Only thieves can peek.\n");
		return(0);
	}
	
	if(F_ISSET(ply_ptr, PBLIND)){
		ANSI(fd, RED);
		print(fd, "You can't do that...You're blind!\n");
		ANSI(fd, WHITE);
		return(0);
	}

	crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
			   cmnd->str[1], cmnd->val[1]);

	if(!crt_ptr) {
		lowercize(cmnd->str[1], 1);
		crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
				   cmnd->str[1], cmnd->val[1]);

		if(!crt_ptr) {
			print(fd, "That person is not here.\n");
			return(0);
		}
	}

	i = LT(ply_ptr, LT_PEEKS);
	t = time(0);

	if(i > t) {
		please_wait(fd, i-t);
		return(0);
	}

	ply_ptr->lasttime[LT_PEEKS].ltime = t;
	ply_ptr->lasttime[LT_PEEKS].interval = 5;

	if(crt_ptr->type==MONSTER && (F_ISSET(crt_ptr, MUNSTL) || F_ISSET(crt_ptr, MTRADE) || F_ISSET(crt_ptr, MPURIT)) && ply_ptr->class < DM) {        
		print(fd, "You shouldn't do that, someone will think you are a thief.\n");  
		return(0);		    
	} 
		
	chance = (25 + ply_ptr->level*10)-(crt_ptr->level*5);
	if (chance<0) chance=0;
	if (ply_ptr->class >= CARETAKER) chance=100;
	if(mrand(1,100) > chance) {
		print(fd, "You failed.\n");
		return(0);
	}

	chance = MIN(90, 15 + ply_ptr->level*5);

	if(mrand(1,100) > chance && ply_ptr->class < CARETAKER) {
		print(crt_ptr->fd, "%s peeked at your inventory.\n", ply_ptr);
		broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
			       "%M peeked at %m's inventory.", 
			       ply_ptr, crt_ptr);
	}

	sprintf(str, "%s is carrying: ", F_ISSET(crt_ptr, PMALES) ? "He":"She");
	n = strlen(str);
	if(list_obj(&str[n], ply_ptr, crt_ptr->first_obj) > 0)
		print(fd, "%s.\n", str);
	else
		print(fd, "%s isn't holding anything.\n",
		      F_ISSET(crt_ptr, PMALES) ? "He":"She");

	return(0);

}
