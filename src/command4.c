/*
 * COMMAND4.C:
 *
 *	Additional user routines
 *
 *	Copyright (C) 1991, 1992, 1993, 1997 Brooke Paul 
 *
 * $Id: command4.c,v 6.20 2001/07/22 20:05:52 develop Exp $
 *
 * $Log: command4.c,v $
 * Revision 6.20  2001/07/22 20:05:52  develop
 * gold theft changes
 *
 * Revision 6.19  2001/06/18 19:58:00  develop
 * "lose" fix to handle conjures
 *
 * Revision 6.18  2001/06/06 19:36:46  develop
 * conjure added
 *
 * Revision 6.17  2001/04/30 19:55:02  develop
 * re-fixed some help problems
 *
 * Revision 6.16  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

#include "../include/mordb.h"
#include "mextern.h"

#include <string.h>


/**********************************************************************/
/*				health				      */
/**********************************************************************/

/* This function shows a player his current hit points, magic points,  */
/* experience, gold and level.					       */

int health( creature *ply_ptr, cmd *cmnd )
{
	int	fd;

	fd = ply_ptr->fd;
	
	if(F_ISSET(ply_ptr, PBLIND)){
		ANSI(fd, BLINDCOLOR);
		ANSI(fd, AM_BOLD);
		ANSI(fd, AM_BLINK);
		output(fd, "You're obviously blind!\n");
		ANSI(fd, AM_NORMAL);
		ANSI(fd, MAINTEXTCOLOR);
		return(0);
	}
	ANSI(fd, AFC_WHITE);
	sprintf(g_buffer, "%s the %s (level %d)", ply_ptr->name, title_ply(ply_ptr), ply_ptr->level);
	output(fd, g_buffer);
	if(F_ISSET(ply_ptr, PHIDDN)) {
		output_wc(fd, " *Hidden*", AFC_CYAN);
	}
	if(F_ISSET(ply_ptr, PNSUSN)) {
		output_wc(fd, " *Thirsty|Hungry*", AFC_YELLOW);
	}
	if(F_ISSET(ply_ptr, PPOISN)) {
		ANSI(fd, AM_BLINK);
		ANSI(fd, AFC_GREEN);
		output(fd, " *Poisoned*");
		ANSI(fd, AM_NORMAL);
	}
	if(F_ISSET(ply_ptr, PCHARM)) {
		ANSI(fd, AM_BOLD);
		ANSI(fd, AFC_CYAN);
		output(fd, " *Charmed*");
		ANSI(fd, AM_NORMAL);
	}
	if(F_ISSET(ply_ptr, PSILNC)) {
		ANSI(fd, AM_BLINK);
		ANSI(fd, AFC_MAGENTA);
		output(fd, " *Mute*");
		ANSI(fd, AM_NORMAL);
	}
	if(F_ISSET(ply_ptr, PDISEA)) {
		ANSI(fd, AM_BLINK);
		ANSI(fd, AFC_RED);	
		output(fd," *Diseased*");
		ANSI(fd, AM_NORMAL);
	}
	ANSI(fd, AFC_GREEN);
	sprintf(g_buffer, "\n %3d/%3d Hit Points    %3d/%3d Magic Points",
		ply_ptr->hpcur, ply_ptr->hpmax, ply_ptr->mpcur, ply_ptr->mpmax);
	output(fd, g_buffer);

	ANSI(fd, AFC_RED);
	sprintf(g_buffer, "    AC: %d\n", ply_ptr->armor/10);
	output(fd, g_buffer );

	ANSI(fd, AFC_YELLOW);
	sprintf(g_buffer, " %7ld Experience    %7ld Gold Pieces\n", ply_ptr->experience, ply_ptr->gold);
	output(fd, g_buffer);
	ANSI(fd, AM_NORMAL);
	ANSI(fd, AFC_WHITE);


	/* show spells under also */
	show_spells( fd, ply_ptr );

	return(0);
}

/**********************************************************************/
/*				help				      */
/**********************************************************************/

/* This function allows a player to get help in general, or help for a */
/* specific command.  If help is typed by itself, a list of commands   */
/* is produced.  Otherwise, help is supplied for the command specified */

int help( creature *ply_ptr, cmd *cmnd )
{
	char 	file[80];
	int	fd, c=0, match=0, num=0,cmdno=0;
#ifndef WIN32
	int 	a = '/';
#else
	int     a = '\\';
#endif
	
	fd = ply_ptr->fd;
	strcpy(file, get_doc_path());

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
	if(!strcmp(cmnd->str[1], "news")) {
                sprintf(file, "%s/news", get_log_path());
                view_file(fd, 1, file);
		return(DOPROMPT);
	}
	

	/* the strrchr call here strips out everything up to the final
	 * '/' so that players cant read files outside the help dir. */

	if(strrchr(cmnd->str[1], a) == '\0')
		sprintf(file, "%s/%s", get_doc_path(), cmnd->str[1]);
	else
		sprintf(file, "%s/%s", get_doc_path(), strrchr(cmnd->str[1], a));

	if( (strstr(file, ".imm") != '\0') && (ply_ptr->class < CARETAKER) ) {
		output(fd, "Sorry, no help on that topic.\n");
		return(DOPROMPT);
	}

	if(file_exists(file)) {
		view_file(fd, 1, file);
		return(DOPROMPT);
	}
	else {  /* start looking for spell/command names */

	do {
		if(!strcmp(cmnd->str[1], cmdlist[c].cmdstr)) {
			match = 1;
			num = c;
			cmdno = cmdlist[c].cmdno;
			break;
		}
		else if(!strncmp(cmnd->str[1], cmdlist[c].cmdstr, 
			strlen(cmnd->str[1]))) {
			match++;
			cmdno = cmdlist[c].cmdno;
			num = c;
		}
		c++;
	} while(cmdlist[c].cmdno);

	/* reserve help 101-399 for immortal commands */
	if( (cmdno > 100 && cmdno < 400) && ply_ptr->class < BUILDER) {
		output(fd, "Command not found.\n");
		return(0);
	}

	if(match == 1) {
		sprintf(file, "%s/help.%d", get_doc_path(), cmdlist[num].cmdno);
		view_file(fd, 1, file);
		return(DOPROMPT);
	}
	else if(match > 1) {
			output(fd, "Command is not unique.\n");
			return(0);
	}

	c = num = 0;
	do {
		if(!strcmp(cmnd->str[1], get_spell_name(c))) {
			match = 1;
			num = c;
			break;
		}
		else if(!strncmp(cmnd->str[1], get_spell_name(c), 
			strlen(cmnd->str[1]))) {
			match++;
			num = c;
		}
		c++;
	} while( get_spell_num(c) != -1);

	if(match > 1) {
		output(fd, "Spell name not unique.\n");
		return(0);
	}

	else if(match == 1) {
		sprintf(file, "%s/spell.%d", get_doc_path(), get_spell_num(num));
		view_file(fd, 1, file);
		return(DOPROMPT);
	}

	else {
		output(fd, "Sorry, no help on that topic.\n");
		return(DOPROMPT);
	}
			
	}
}

/**********************************************************************/
/*				welcome				      */
/**********************************************************************/

/* Outputs welcome file to user, giving him/her info on how to play   */
/* the game 							      */

int welcome( creature *ply_ptr, cmd *cmnd )
{
	char	file[80];
	int	fd;

	fd = ply_ptr->fd;

	sprintf(file, "%s/welcome", get_doc_path());

	view_file(fd, 1, file);
	return(DOPROMPT);
}

/**********************************************************************/
/*				info				      */
/**********************************************************************/

/* This function displays a player's entire list of information, including */
/* player stats, proficiencies, level and class.			   */

int info( creature *ply_ptr, cmd *cmnd )
{
	char	alstr[8];
	int 	fd, cnt;
	long	expneeded, lv;

	fd = ply_ptr->fd;
/*
	update_ply(ply_ptr); */

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
		if(ply_ptr->ready[lv]) 
			cnt++;
	cnt += count_inv(ply_ptr, 0);

    output(fd, "\n\n->");
	output(fd, "You are ");
	sprintf(g_buffer, "%s the %s\n", ply_ptr->name, title_ply(ply_ptr));
	output(fd, g_buffer);
    output(fd, "->");
	output(fd, "Description: ");
	sprintf(g_buffer, "%s\n", ply_ptr->description);
	output(fd, g_buffer);
	output(fd, "\nLevel: ");
	sprintf(g_buffer, "%-20d          ", ply_ptr->level);
	output(fd, g_buffer );
	output(fd, "Race: ");
	sprintf(g_buffer, "%-20s         ", get_race_string(ply_ptr->race));
	output(fd, g_buffer);
	output(fd, "\nClass: ");
	sprintf(g_buffer, "%-20s  ", get_class_string(ply_ptr->class));
	output(fd, g_buffer);
	output(fd, "Alignment: ");
	sprintf(g_buffer, "%-8s %-8s               ", F_ISSET(ply_ptr, PCHAOS) ? "Chaotic" : "Lawful", alstr);
	output_nl(fd, g_buffer);
	output(fd, "Guild: ");
        if(F_ISSET(ply_ptr, PPLDGK))
	{
           sprintf(g_buffer, "%s", cur_guilds[check_guild(ply_ptr)].name);
           output(fd, g_buffer );
	}
	else
		output(fd, "None");


#define INTERVAL ply_ptr->lasttime[LT_HOURS].interval

	output(fd, "\nTime of play: ");
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
	output(fd, "minute/s                     ");

	sprintf(g_buffer, "\n\nSTR: %-2d        ", ply_ptr->strength);
	output(fd, g_buffer);
	sprintf(g_buffer, "DEX: %-2d        ", ply_ptr->dexterity);
	output(fd, g_buffer);
	sprintf(g_buffer, "CON: %-2d        ", ply_ptr->constitution);
	output(fd, g_buffer);
	sprintf(g_buffer, "INT: %-2d        ", ply_ptr->intelligence);
	output(fd, g_buffer);
	sprintf(g_buffer, "PTY: %-2d     ", ply_ptr->piety);
	output(fd, g_buffer);

	sprintf(g_buffer, "\n\n  Hit Points[Hp]: %5d/%-5d     ", ply_ptr->hpcur, ply_ptr->hpmax);
	output(fd, g_buffer);
	sprintf(g_buffer, "Exp: %9lu /%-9lu next level  ", ply_ptr->experience, MAX(0, expneeded-ply_ptr->experience));
	output(fd, g_buffer);
	sprintf(g_buffer, "\nMagic Points[Mp]: %5d/%-5d                    ", ply_ptr->mpcur, ply_ptr->mpmax);
	output(fd, g_buffer);
	sprintf(g_buffer, "Gold: %-9lu       ", ply_ptr->gold);
	output(fd, g_buffer);
	sprintf(g_buffer, "\nArmour class[AC]:     %-4d          ", ply_ptr->armor/10);
	output(fd, g_buffer);
	sprintf(g_buffer, "Inventory Weight: %-4d Lbs/%-3d Objs\n", weight_ply(ply_ptr), cnt);
	output(fd, g_buffer);

	output(fd, "\nProficiencies:                                                          ");
	sprintf(g_buffer, "\nSharp:  %2d%% ", profic(ply_ptr, SHARP));
	output(fd, g_buffer);
	sprintf(g_buffer, "Thrust: %2d%% ", profic(ply_ptr, THRUST));
	output(fd, g_buffer);
	sprintf(g_buffer, "Blunt:  %2d%% ", profic(ply_ptr, BLUNT));
	output(fd, g_buffer);
	sprintf(g_buffer, "Pole:  %2d%% ", profic(ply_ptr, POLE));
	output(fd, g_buffer);
	sprintf(g_buffer, "Missile: %2d%% ", profic(ply_ptr, MISSILE));
	output(fd, g_buffer);
	sprintf(g_buffer, "Bare Hand: %2d%%", profic(ply_ptr, HAND));
	output(fd, g_buffer);

	output(fd, "\n\nRealms:                                                                 ");
	sprintf(g_buffer, "\nEarth:  %3d%%   ", mprofic(ply_ptr, EARTH));
	output(fd, g_buffer);
	sprintf(g_buffer, "Wind:   %3d%%   ", mprofic(ply_ptr, WIND));
	output(fd, g_buffer);
	sprintf(g_buffer, "Fire:   %3d%%   ", mprofic(ply_ptr, FIRE));
	output(fd, g_buffer);
	sprintf(g_buffer, "Water:  %3d%%               \n", mprofic(ply_ptr, WATER));
	output(fd, g_buffer);
	
	F_SET(ply_ptr, PREADI);
	ask_for(fd, "[Hit Return, Q to quit]: ");
	output_ply_buf(fd);
    Ply[fd].io->intrpt &= ~1; 
	Ply[fd].io->fn = info_2;
	Ply[fd].io->fnparam = 1;
	return(DOPROMPT);
}

/************************************************************************/
/*				info_2					*/
/************************************************************************/

/* This function is the second half of info which outputs spells	*/

void info_2(int	fd, int param, char *instr )
{
	char		str[2048];
	char		spl[128][20];
	int		i, j;
	creature	*ply_ptr;

	ply_ptr = Ply[fd].ply;
	
	if(instr[0]) {
		output(fd, "Aborted.\n");
		F_CLR(ply_ptr, PREADI);
		RETURN(fd, command, 1);
	}

	strcpy(str, "\nSpells known: ");
	for(i=0,j=0; i< get_spell_list_size(); i++)
		if(S_ISSET(ply_ptr, i))
			strcpy(spl[j++], get_spell_name(i));
	

	if(!j)
		strcat(str, "None.");
	else {
		qsort((void *)spl, j, 20, (PFNCOMPARE)strcmp);
		for(i=0; i<j; i++) {
			strcat(str, spl[i]);
			strcat(str, ", ");
		}
		str[strlen(str)-2] = '.';
		str[strlen(str)-1] = 0;
	}

	output_nl(fd, str);

	show_spells( fd, ply_ptr );

	F_CLR(Ply[fd].ply, PREADI);
	RETURN(fd, command, 1);
}



/**********************************************************************/
/*				show_spells()				      */
/**********************************************************************/
void show_spells( int fd, creature *ply_ptr )
{
	char		str[512];
	int			count = 0;

	strcpy(str, "Spells under: ");

	if(F_ISSET(ply_ptr, PBLESS)) 
	{
		strcat(str, "bless, ");
		count++;
	}
	if(F_ISSET(ply_ptr, PLIGHT)) 
	{
		strcat(str, "light, ");
		count++;
	}
	if(F_ISSET(ply_ptr, PPROTE)) 
	{
		strcat(str, "protection, ");
		count++;
	}
	if(F_ISSET(ply_ptr, PINVIS)) 
	{
		strcat(str, "invisibility, ");
		count++;
	}
	if(F_ISSET(ply_ptr, PDINVI)) 
	{
		strcat(str, "detect-invisible, ");
		count++;
	}
	if(F_ISSET(ply_ptr, PDMAGI)) 
	{
		strcat(str, "detect-magic, ");
		count++;
	}
	if(F_ISSET(ply_ptr, PLEVIT)) 
	{
		strcat(str, "levitation, ");
		count++;
	}
	if(F_ISSET(ply_ptr, PRFIRE)) 
	{
		strcat(str, "resist-fire, ");
		count++;
	}
	if(F_ISSET(ply_ptr, PFLYSP)) 
	{
		strcat(str, "fly, ");
		count++;
	}
	if(F_ISSET(ply_ptr, PRMAGI)) 
	{
		strcat(str, "resist-magic, ");
		count++;
	}
	if(F_ISSET(ply_ptr, PKNOWA)) 
	{
		strcat(str, "know-aura, ");
		count++;
	}
	if(F_ISSET(ply_ptr, PRCOLD)) 
	{
		strcat(str, "resist-cold, ");
		count++;
	}
	if(F_ISSET(ply_ptr, PBRWAT)) 
	{
		strcat(str, "breathe-water, ");
		count++;
	}
	if(F_ISSET(ply_ptr, PSSHLD)) 
	{
		strcat(str, "resist-earth, ");
		count++;
	}

	if( count == 0)
		strcat(str, "None.");
	else {
		str[strlen(str)-2] = '.';
		str[strlen(str)-1] = 0;
	}

	strcat(str, "\n\n");
	output( fd, str );


	return;
}



/**********************************************************************/
/*				psend				      */
/**********************************************************************/

/* This function allows a player to send a message to another player.  If */
/* the other player is logged in, the message is sent successfully.       */

int psend( creature	*ply_ptr, cmd *cmnd )
{
	creature	*crt_ptr = 0;
	etag		*ign;
	int			i, fd;

	fd = ply_ptr->fd;

	if(cmnd->num < 2) {
		output(fd, "Send to whom?\n");
		return 0;
	}


	if(!F_ISSET(ply_ptr, PSECOK) && ply_ptr->class < IMMORTAL) {
		output(fd, "You may not do that yet.\n");
		return(0);
	}

	if(F_ISSET(ply_ptr, PSILNC)) {
		output_wc(fd, "You can't speak.\n", SILENCECOLOR);
		return(0);
	}

	cmnd->str[1][0] = up(cmnd->str[1][0]);
	for(i=0; i<Tablesize; i++) {
		if(!Ply[i].ply) 
			continue;
		if(Ply[i].ply->fd == -1) 
			continue;
		if(F_ISSET(Ply[i].ply, PDMINV) && ply_ptr->class < CARETAKER) 
			continue;
		if(!strncmp(Ply[i].ply->name, cmnd->str[1], 
		   strlen(cmnd->str[1])))
			crt_ptr = Ply[i].ply;
		if(!strcmp(Ply[i].ply->name, cmnd->str[1]))
			break;
	}
	if(!crt_ptr) {
		output(fd, "Send to whom?\n");
		return(0);
	}
	if( (ply_ptr->class < BUILDER) && (F_ISSET(crt_ptr, PINVIS)) && (!F_ISSET(ply_ptr, PDINVI))) {
		output(fd, "Send to whom?\n");
		return(0);
	}
 	if(F_ISSET(crt_ptr, PIGNOR) && (ply_ptr->class < BUILDER)) {
		sprintf(g_buffer, "%s is ignoring everyone.\n", crt_ptr->name);
		output(fd, g_buffer);
		return(0);
	}

	ign = Ply[crt_ptr->fd].extr->first_ignore;
	while(ign) {
		if(!strcmp(ign->enemy, ply_ptr->name)) 
		{
			sprintf(g_buffer, "%s is ignoring you.\n", crt_ptr->name);
			output(fd, g_buffer);
			return(0);
		}
		ign = ign->next_tag;
	}
	
	cmnd->fullstr[255] = 0;

	/* this has 2 words we need to strip from the beginning */
	clean_str( cmnd->fullstr, 2);

	if(strlen(cmnd->fullstr) < 1) {
		output(fd, "Send what?\n");
		return(0);
	}


	/* spam check */
	if ( check_for_spam( ply_ptr ) )
	{
		return(0);
	}


	if(F_ISSET(crt_ptr, PAFK))
	{
		sprintf(g_buffer, "%s is AFK right now and might not respond to your telepath.\n",
			crt_ptr->name);
		output(fd, g_buffer);
	}

	if(F_ISSET(ply_ptr, PLECHO)){
		ANSI(fd, ECHOCOLOR);
		sprintf(g_buffer, "You sent: \"%s\" to %%M.\n", cmnd->fullstr);
		mprint(fd, g_buffer, m1arg(crt_ptr));
		ANSI(fd, MAINTEXTCOLOR);
	}
	else
	{
		sprintf(g_buffer, "Message sent to %s.\n", crt_ptr->name);
		output(fd, g_buffer);
	}
	
	sprintf(g_buffer, "### %%M just flashed, \"%s\".\n", 
		cmnd->fullstr);
	mprint(crt_ptr->fd, g_buffer, m1arg(ply_ptr));
	
	if(F_ISSET(ply_ptr, PDMINV) && crt_ptr->class < BUILDER) {
		output(fd, "They will be unable to reply.\n");
		if(F_ISSET(ply_ptr, PALIAS)) 
		{
			sprintf(g_buffer, "Sent from: %s.\n", Ply[fd].extr->alias_crt->name);
			output(fd, g_buffer);
		}
	}

	if(ply_ptr->class > BUILDER || crt_ptr->class > BUILDER)
		return(0);

	if (ply_ptr->class == DM)
		return(0);

	sprintf(g_buffer, "--- %s sent to %s, \"%s\".", ply_ptr->name,
		crt_ptr->name, cmnd->fullstr);
	broadcast_eaves(g_buffer);

	return(0);

}




/**********************************************************************/
/*				broadsend			      */
/**********************************************************************/

/* This function is used by players to broadcast a message to every   */
/* player in the game.  Broadcasts by players are of course limited,  */
/* so the number used that day is checked before the broadcast is     */
/* allowed.							      */

int broadsend(creature *ply_ptr, cmd *cmnd )
{
	int		fd;
	char	str[512];

	fd = ply_ptr->fd;

	cmnd->fullstr[255] = 0;
	clean_str( cmnd->fullstr, 1 );

	if(strlen(cmnd->fullstr) < 1) {
		output(fd, "Send what?\n");
		return(0);
	}

        if(SECURE) {
                if(ply_ptr->level < 2) {
                        output(fd, "You are not authorized to broadcast.\n");
                        return(0);
                 }
        }

	if(RFC1413) {
		if((!strcmp(Ply[fd].io->userid, "no_port") || !strcmp(Ply[fd].io->userid, "unknown")) && !F_ISSET(ply_ptr, PAUTHD)){
			output(fd, "You are not authorized to broadcast.\n");
			return(0);
		}
	} /* RFC1413 */

	if(!F_ISSET(ply_ptr, PSECOK)) {
		output(fd, "You may not do that yet.\n");
		return(0);
	}

	if(!HEAVEN)
		if(!dec_daily(&ply_ptr->daily[DL_BROAD])) {
			output(fd,"You've used up all your broadcasts today.\n");
			return(0);
		}

	if(F_ISSET(ply_ptr, PSILNC)) {
		output_wc(fd, "Your voice is too weak to do that.\n", SILENCECOLOR);
		return(0);
	}
	output(fd, "Message broadcast.\n");

	strcpy(str, "### %M broadcasted, \"");
	strcat(str, cmnd->fullstr);
	strcat(str, "\"." );
	broadcast_c(str, ply_ptr);

	return(0);

}

/**********************************************************************/
/*				follow				      */
/**********************************************************************/

/* This command allows a player (or a monster) to follow another player. */
/* Follow loops are not allowed; i.e. you cannot follow someone who is   */
/* following you.  Also, you cannot follow yourself.			 */

int follow( creature *ply_ptr, cmd *cmnd )
{
	creature	*old_ptr, *new_ptr;
	room		*rom_ptr;
	ctag		*cp, *pp, *prev;
	etag		*ignore;
	int		fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(cmnd->num < 2) {
		output(fd, "Follow who?\n");
		return(0);
	}

	F_CLR(ply_ptr, PHIDDN);

	if(F_ISSET(ply_ptr, PDMOWN)) {
		output(fd, "You may not do that now.\n");
		return(0);
	}
		
	lowercize(cmnd->str[1], 1);
	new_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
			   cmnd->str[1], cmnd->val[1]);

	if(!new_ptr) {
		output(fd, "No one here by that name.\n");
		return(0);
	}

	if(new_ptr == ply_ptr && !ply_ptr->following) {
		output(fd, "You can't follow yourself.\n");
		return(0);
	}

	ignore = Ply[new_ptr->fd].extr->first_ignore;
        while(ignore) {
                if(!strcmp(ignore->enemy, new_ptr->name)) 
                {
                        sprintf(g_buffer, "%s is ignoring you.\n", new_ptr->name);
                        output(fd, g_buffer);
                        return(0);
                }
                ignore = ignore->next_tag;
        }

	if(new_ptr->following == ply_ptr) {
		sprintf(g_buffer, "You can't.  %s's following you.\n",
		      F_ISSET(new_ptr, PMALES) ? "He":"She");
		output(fd, g_buffer);
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
		sprintf(g_buffer, "You stop following %s.\n", old_ptr->name);
		output(fd, g_buffer);
		if(!F_ISSET(ply_ptr, PDMINV))
			mprint(old_ptr->fd, "%M stops following you.\n", 
			      m1arg(ply_ptr));
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

	sprintf(g_buffer, "You start following %s.\n", new_ptr->name);
	output(fd, g_buffer );
	if(!F_ISSET(ply_ptr, PDMINV) || (F_ISSET(ply_ptr, PDMINV) && new_ptr->class >= ply_ptr->class)) {
		mprint(new_ptr->fd, "%M starts following you.\n", m1arg(ply_ptr));
		broadcast_rom2(fd, new_ptr->fd, ply_ptr->rom_num,
			      "%M follows %m.", m2args(ply_ptr, new_ptr));
	}

	return(0);

}

/**********************************************************************/
/*				lose				      */
/**********************************************************************/

/* This function allows a player to lose another player who might be  */
/* following him.  When successful, that player will no longer be     */
/* following.							      */

int lose( creature *ply_ptr, cmd *cmnd )
{
	creature	*crt_ptr;
	ctag		*cp, *prev;
	int		fd;

	fd = ply_ptr->fd;

	if(cmnd->num == 1) 
	{
 
		if (ply_ptr->following == 0){
			output(fd, "You're not following anyone.\n");
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
        mprint(fd,"You stop following %m\n", m1arg(crt_ptr));
        if (!F_ISSET(ply_ptr,PDMINV))
			mprint(crt_ptr->fd,"%M stops following you\n", m1arg(ply_ptr));
        return(0);
    }                   

	F_CLR(ply_ptr, PHIDDN);

	lowercize(cmnd->str[1], 1);
	crt_ptr = find_crt(ply_ptr, ply_ptr->first_fol,
			   cmnd->str[1], cmnd->val[1]);

	if(!crt_ptr) {
		crt_ptr = find_crt_in_rom(ply_ptr,ply_ptr->parent_rom,
			  cmnd->str[1],cmnd->val[1],MON_FIRST);
	}

	if(!crt_ptr) {
		output(fd, "That person is not following you.\n");
		return(0);
	}

	if(crt_ptr->following != ply_ptr) {
		output(fd, "That person is not following you.\n");
		return(0);
	}

	if(crt_ptr->type == MONSTER && F_ISSET(crt_ptr, MCONJU)) {
		broadcast_rom(fd, crt_ptr->parent_rom->rom_num, "%M fades away.\n", m1arg(crt_ptr));
		sprintf(g_buffer, "%s fades away.\n", crt_ptr->name);
		output(fd, g_buffer);
		del_conjured(crt_ptr);
		del_crt_rom(crt_ptr,crt_ptr->parent_rom);
		free_crt(crt_ptr);
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

	sprintf(g_buffer, "You lose %s.\n", F_ISSET(crt_ptr, PMALES) ? "him":"her");
	output(fd, g_buffer);

	if(!F_ISSET(ply_ptr, PDMINV)) {
		mprint(crt_ptr->fd, "%M loses you.\n", m1arg(ply_ptr));
		broadcast_rom2(fd, ply_ptr->fd, crt_ptr->fd, "%M loses %m.", 
			       m2args(ply_ptr, crt_ptr));
	}

	return(0);

}

/**********************************************************************/
/*				group				      */
/**********************************************************************/

/* This function allows you to see who is in a group or party of people */
/* who are following you.						*/

int group( creature	*ply_ptr, cmd *cmnd )
{
	ctag	*cp;
	char	str[2048];
	int	fd, found = 0;

	ASSERTLOG( ply_ptr );

	fd = ply_ptr->fd;

	if ( ply_ptr->first_fol == NULL 
		&& ply_ptr->following == NULL )
	{
		output(fd, "You are not in a group.\n");
		return(0);
	}

	strcpy(str, "People in your group: ");

	if ( ply_ptr->first_fol )
	{
		/* you are being followed */
		cp = ply_ptr->first_fol;
	}
	else
	{
		/* you are following */
		cp = ply_ptr->following->first_fol;

		/* if you are following, list the leader */
		strcat(str, crt_str(ply_ptr, ply_ptr->following, 0, 0));
		strcat(str, ", ");
		found = 1;
	}


	while(cp) {
		/* dont list yourself */
		if ( cp->crt != ply_ptr 
			&& (!F_ISSET(cp->crt, PDMINV) || ply_ptr->class > BUILDER)) {
			strcat(str, crt_str(ply_ptr, cp->crt, 0, 0));
			strcat(str, ", ");
			found = 1;
		}
		cp = cp->next_tag;
	}

	if(!found) {
		output(fd, "No one is following you.\n");
	}
	else
	{
		str[strlen(str)-2] = 0;
		output_nl(fd, str);
	}

	return(0);

}

/**********************************************************************/
/*				track				      */
/**********************************************************************/

/* This function is the routine that allows rangers and druids to search */
/* for tracks 								 */
/* in a room.  If the ranger is successful, he will be told what dir-    */
/* ection the last person who was in the room left.			 */

int track( creature *ply_ptr, cmd *cmnd )
{
	time_t	i, t;
	int	fd, chance;

	fd = ply_ptr->fd;

	if(ply_ptr->class != RANGER && ply_ptr->class != DRUID && ply_ptr->class < BUILDER) {
		output(fd, "Only rangers and druids can track.\n");
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
	ply_ptr->lasttime[LT_TRACK].interval = MAX(0, 5 - bonus[(int)ply_ptr->dexterity]);
	
	if(F_ISSET(ply_ptr, PBLIND)) {
		output_wc(fd, "You're blind...how can you see tracks?\n", BLINDCOLOR);
		return(0);
	}
	chance = 25 + (bonus[(int)ply_ptr->dexterity] + ply_ptr->level)*5;

	if(mrand(1,100) > chance) {
		output(fd, "You fail to find any tracks.\n");
		return(0);
	}

	if(!ply_ptr->parent_rom->track[0]) {
		output(fd, "There are no tracks in this room.\n");
		return(0);
	}

	sprintf(g_buffer, "You find tracks leading to the %s.\n",
	      ply_ptr->parent_rom->track);
	output(fd, g_buffer);
	broadcast_rom(fd, ply_ptr->rom_num, "%M finds tracks.", m1arg(ply_ptr));

	return(0);

}

/**********************************************************************/
/*				peek				      */
/**********************************************************************/

/* This function allows a thief or assassin to peek at the inventory of */
/* another player.  If successful, they will be able to see it and      */
/* another roll is made to see if they get caught.			*/

int peek( creature *ply_ptr, cmd *cmnd )
{
	creature	*crt_ptr;
	room		*rom_ptr;
	char		str[2048];
	time_t		i, t;
	int		fd, n, chance;
	long		amt;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;
	str[0] = 0;

	if(cmnd->num < 2) {
		output(fd, "Peek at who?\n");
		return(0);
	}

	if(ply_ptr->class != THIEF && ply_ptr->class < BUILDER) {
		output(fd, "Only thieves can peek.\n");
		return(0);
	}
	
	if(F_ISSET(ply_ptr, PBLIND)){
		output_wc(fd, "You can't do that...You're blind!\n", BLINDCOLOR);
		return(0);
	}

	crt_ptr = find_crt_in_rom(ply_ptr, rom_ptr, cmnd->str[1], cmnd->val[1],
		MON_FIRST);

	if(!crt_ptr) {
		output(fd, "That person is not here.\n");
		return(0);
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
		output(fd, "You shouldn't do that, someone will think you are a thief.\n");  
		return(0);		    
	} 
		
	chance = (25 + ply_ptr->level*10)-(crt_ptr->level*5);
	if (chance<0) 
		chance=0;
	if (ply_ptr->class >= BUILDER) 
		chance=100;
	if(mrand(1,100) > chance) {
		output(fd, "You failed.\n");
		return(0);
	}

	chance = MIN(90, 15 + ply_ptr->level*5);

	if(mrand(1,100) > chance && ply_ptr->class < BUILDER) {
		sprintf(g_buffer, "%s peeked at your inventory.\n", ply_ptr->name);
		output(crt_ptr->fd, g_buffer);
		broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
			       "%M peeked at %m's inventory.", 
			       m2args(ply_ptr, crt_ptr));
	}

	sprintf(str, "%s is carrying: ", F_ISSET(crt_ptr, PMALES) ? "He":"She");
	n = strlen(str);
	if(list_obj(&str[n], ply_ptr, crt_ptr->first_obj) > 0) {
		output_nl(fd, str);
	}
	else {
		sprintf(g_buffer, "%s isn't holding anything.\n",
		      F_ISSET(crt_ptr, PMALES) ? "He":"She");
		output(fd, g_buffer);
	}

	if(crt_ptr->type == PLAYER) {
		amt = crt_ptr->gold/(mrand(1,4));
		sprintf(g_buffer, "%s has about %ld gold.\n", crt_ptr->name, amt);
		output(fd, g_buffer);
	}

	return(0);

}
