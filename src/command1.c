/*
 * COMMAND1.C:
 *
 *	Command handling/parsing routines.
 *
 *	Copyright (C) 1991, 1992, 1993, 1997 Brooke Paul 
 * $Id: command1.c,v 6.25 2001/07/22 19:03:06 develop Exp $
 *
 * $Log: command1.c,v $
 * Revision 6.25  2001/07/22 19:03:06  develop
 * first run at alllowing thieves to steal gold from other players
 *
 * Revision 6.24  2001/07/17 19:25:11  develop
 * *** empty log message ***
 *
 * Revision 6.24  2001/07/14 21:26:44  develop
 * *** empty log message ***
 *
 * Revision 6.23  2001/06/10 12:32:04  develop
 * added disclaimer before law/chaos choice at character creation
 *
 * Revision 6.22  2001/04/30 19:55:02  develop
 * *** empty log message ***
 *
 * Revision 6.21  2001/04/26 01:20:51  develop
 * changed output for NOCREATE
 *
 * Revision 6.20  2001/04/25 01:22:26  develop
 * added output to elog_broad for character create attempt when
 * NOCREATE is set.
 *
 * Revision 6.19  2001/04/23 04:24:20  develop
 * added email response to NOCREATE output
 *
 * Revision 6.18  2001/04/23 04:06:47  develop
 * added NOCREATE
 *
 * Revision 6.17  2001/04/23 03:52:29  develop
 * added NOCREATE flag to toggle character creation
 *
 * Revision 6.16  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 *
 */

#include "../include/mordb.h"
#include "mextern.h"

#include <ctype.h>
#include <string.h>

/**********************************************************************/
/*				login				      */
/**********************************************************************/

/* This function is the first function that gets input from a player when */
/* he logs in.  It asks for the player's name and password, and performs  */
/* the according function calls.					  */

void login(int 	fd, int	param, char	*str )
{
	int		i,art,o; /*, match=0; */
	creature	*ply_ptr;
	char		tempstr[20];

	switch(param) {
	case 0:
		if(strcmp(Ply[fd].extr->tempstr[0], str)) {
			disconnect(fd);
			return;
		}
		output(fd, "\nPlease enter name: ");
		RETURN(fd, login, 1);
	case 1:
		Ply[fd].extr->loginattempts +=1;
		if(Ply[fd].extr->loginattempts>3){
                	disconnect(fd);
			return;
		}

		if(!isalpha((int)str[0])) {
			output(fd, "Please enter name: ");
			RETURN(fd, login, 1);
		}

		if(PARANOID) {
			/* Check for ident */
			if((!strcmp(Ply[fd].io->userid, "no_port") || !strcmp(Ply[fd].io->userid, "unknown")) && (strcmp(Ply[fd].io->address, "mordor.nazgul.com") || strcmp(Ply[fd].io->address, "128.200.21.101"))) {
				output(fd, "\n\nI am unable to get authorization from your server.\n");
				output(fd, "Please try connecting from another host, or contact\n");
				output(fd, "your system administrator to request RFC 931 auth/identd.\n\n");
				output(fd, "If you have connected from this account in the past,\n");
				output(fd, "try logging in more slowly.\n\n");
				output_ply_buf(fd);
				disconnect(fd);
				return;
			}
		}

		/* Check for double log */
/*
		for(i=0; i<Tablesize; i++) {
			if(!Ply[i].ply) 
				continue;
			if(Ply[i].ply->fd < 0) 
				continue;
			if(!strcmp(Ply[i].io->userid, "no_port") || !strcmp(Ply[i].io->userid, "unknown")) 
				continue;
			if(!strcmp(Ply[i].io->userid, Ply[fd].io->userid)) {
				match += 1;
				if(match > 0){
					sprintf(g_buffer, "\n\n%s\n", account_exists);
					output(fd, g_buffer );
					output(fd, "Please only play one character at a time.\n\n");
					output_ply_buf(fd);
					disconnect(fd);
					return;
				}
			}
		}
*/


		if(strlen(str) < 3) {
			output(fd, "Name must be at least 3 characters.\n\n");
			output(fd, "Please enter name: ");
			RETURN(fd, login, 1);
		}
		if(strlen(str) >= 20) {
			output(fd, "Name must be less than 20 characters.\n\n");
			output(fd, "Please enter name: ");
			RETURN(fd, login, 1);
		}

		for(i=0; i< (int)strlen(str); i++)
			if(!isalpha((int)str[i])) {
				output(fd, "Name must be alphabetic.\n\n");
				output(fd, "Please enter name: ");
				RETURN(fd, login, 1);
			}

		/* check to make sure the name isn't a reserved article */
		lowercize(str, 0);
                o = art = 0;
                while(article[o][0] != '@') {
                        if(!strcmp(article[o++], str)) {
                                art = 1;
                                break;
                        }
                }
                if(art) {
                        output(fd, "That name is not allowed.\n");
                        output(fd, "\nPlease enter name: ");
                        RETURN(fd, login, 1);
                }

		/* check to see the name is allowed */
		lowercize(str, 1);
		str[25]=0;

		if ( contains_bad_word( str ) )
		{
	        output(fd, "That name is not allowed.\n");
			output(fd, "\nPlease enter name: ");
            		RETURN(fd, login, 1);
        	}

		if(load_ply(str, &ply_ptr) < 0) {
			strcpy(Ply[fd].extr->tempstr[0], str);
			sprintf(g_buffer, "\n%s? Did I get that right? ", str);
			output(fd, g_buffer);
			RETURN(fd, login, 2);
		}

		else {
			ply_ptr->fd = -1;
			Ply[fd].ply = ply_ptr;

			if(CHECKDOUBLE) {
				if(checkdouble(ply_ptr->name)) {
					scwrite(fd, "No simultaneous playing.\n\r", 26);
					disconnect(fd);
					return;
				}
			} /* CHECKDOUBLE */

			sprintf(g_buffer, "Please enter password: %c%c%c", 255, 251, 1); 
			output(fd, g_buffer); 
			RETURN(fd, login, 3);
		}

	case 2:
		if(str[0] != 'y' && str[0] != 'Y') {
			Ply[fd].extr->tempstr[0][0] = 0;
			output(fd, "Please enter name: ");
			RETURN(fd, login, 1);
		}
		else {
			if(NOCREATE) {
				output(fd, "\nThat name is not known, and you may not create a new character at this time.\nPlease hit return to try again.\n");
				sprintf(g_buffer, "\n\rIf you would like to create a character, please email %s.\n\r", questions_to_email);
				output(fd, g_buffer);
				sprintf(g_buffer, "Character create denied from: %s", Ply[fd].io->address);
                		slog(g_buffer);
				RETURN(fd, login, 1);
			} 
			else {
				output(fd, "\nHit return: ");
				RETURN(fd, create_ply, 1);
			}
		}

	case 3:
		if(strcmp(str, Ply[fd].ply->password)) {
	     	scwrite(fd, "\255\252\1\n\rIncorrect.\n\r", 17);
			disconnect(fd);
			return;
		}
		else {
			sprintf(g_buffer, "%c%c%c\n\r", 255, 252, 1);
			output(fd, g_buffer );
			strcpy(tempstr, Ply[fd].ply->name);

			/* check for double login */
			check_double_login( fd );
					
			free_crt(Ply[fd].ply);

/* It used to cause a crash if a player suicided at the same time */
/* as creating a new charater, this fixes it.			  */

			if(load_ply(tempstr, &Ply[fd].ply) < 0)
			{
				scwrite(fd, "Player no longer exists!\n\r", 25);
			
				sprintf(g_buffer,"%s (%s) suicided.", 
					Ply[fd].ply->name, Ply[fd].io->address);

				logn("sui_crash",g_buffer );
				disconnect(fd);
				return;
			}
			Ply[fd].ply->fd = fd;
			init_ply(Ply[fd].ply);
			RETURN(fd, command, 1);
		}
	}
}

/**********************************************************************/
/*				create_ply			      */
/**********************************************************************/

/* This function allows a new player to create his or her character. */

void create_ply(int	fd, int	param, char	*str )
{
	int 	i, k, l, m, n, obj, sum, num[5];
	object	*obj_ptr;

	switch(param) {
	case 1:
/*		output(fd,"\n\n"); */
		Ply[fd].ply = (creature *)malloc(sizeof(creature));
		if(!Ply[fd].ply)
			merror("create_ply", FATAL);
		zero(Ply[fd].ply, sizeof(creature));
		Ply[fd].ply->fd = -1;
		Ply[fd].ply->rom_num = start_room_num;


	if((!strcmp(Ply[fd].extr->tempstr[0],dmname[0])) ||
	   (!strcmp(Ply[fd].extr->tempstr[0],dmname[1])) ||
	   (!strcmp(Ply[fd].extr->tempstr[0],dmname[2])) ||
	   (!strcmp(Ply[fd].extr->tempstr[0],dmname[3])) ||
	   (!strcmp(Ply[fd].extr->tempstr[0],dmname[4])) ||
	   (!strcmp(Ply[fd].extr->tempstr[0],dmname[5])) ||
	   (!strcmp(Ply[fd].extr->tempstr[0],dmname[6]))) {
		output(fd, "\nA password is required to create that character.\n");
	 	output(fd, "Please enter password: ");
		output_ply_buf(fd);
		RETURN(fd, create_ply, 2);
	}
	else goto no_pass;
	case 2:
			if(strcmp(dm_pass, str)) {
			   disconnect(fd);
			   return;
			}
	case 3:
	no_pass:
		if(ANSILINE) {
			clrscr(fd);
		}
		
		ask_for(fd, "[M] Male or [F] Female: ");
		RETURN(fd, create_ply, 4);
	case 4:
		if(low(str[0]) != 'm' && low(str[0]) != 'f') {
			ask_for(fd, "[M] Male or [F] Female: ");
			RETURN(fd, create_ply, 4);
		}
		if(low(str[0]) == 'm')
			F_SET(Ply[fd].ply, PMALES);
		output(fd, "\nAvailable classes:");
		output(fd, "\n		    /----- AVAILABLE CLASSES ------\\");
		output(fd, "\n		    |                              |");
		output(fd, "\n		    |  [A] Assassin  [B] Barbarian |");  
		output(fd, "\n		    |  [C] Cleric    [D] Fighter   |");
		output(fd, "\n		    |  [E] Bard      [F] Mage      |"); 
		output(fd, "\n		    |  [G] Paladin   [H] Ranger    |");
		output(fd, "\n		    |  [I] Thief     [J] Monk      |");
		output(fd, "\n		    |  [K] Druid     [L] Alchemist |");
		output(fd, "\n		    \\------------------------------/");
		ask_for(fd, "Choose one: ");
		RETURN(fd, create_ply, 5);
	case 5:
		switch(low(str[0])) {
			case 'a': Ply[fd].ply->class = ASSASSIN; break;
			case 'b': Ply[fd].ply->class = BARBARIAN; break;
			case 'c': Ply[fd].ply->class = CLERIC; break;
			case 'd': Ply[fd].ply->class = FIGHTER; break;
			case 'e': Ply[fd].ply->class = BARD; break;
			case 'f': Ply[fd].ply->class = MAGE; break;
			case 'g': Ply[fd].ply->class = PALADIN; break;
			case 'h': Ply[fd].ply->class = RANGER; break;
			case 'i': Ply[fd].ply->class = THIEF; break;
			case 'j': Ply[fd].ply->class = MONK; break;
			case 'k': Ply[fd].ply->class = DRUID; break;
			case 'l': Ply[fd].ply->class = ALCHEMIST; break;
			default: {
				ask_for(fd, "Choose one: ");
				RETURN(fd, create_ply, 5);
			}
		}
		if(!Ply[fd].ply->class) {
			sprintf(g_buffer, "Invalid selection: %s", str);
			output(fd, g_buffer);
			ask_for(fd, "Choose one: ");
			RETURN(fd, create_ply, 5);
		}

		output(fd, "\n		 ----- CHARACTER STATS -----");
		output(fd, "\nYou have 54 points to distribute among your 5 stats. Please enter your 5");
		output(fd, "\nnumbers in the following order: Strength, Dexterity, Constitution,");
		output(fd, "\nIntelligence, Piety.  No stat may be smaller than 3 or larger than 18.i");
		output(fd, "\nUse the following format: ## ## ## ## ##\n\n");
		
		ask_for(fd, ": ");
		RETURN(fd, create_ply, 6);
	case 6:
		n = strlen(str); l = 0; k = 0;
		for(i=0; i<=n; i++) {
			if(str[i]==' ' || str[i]==0) {
				str[i] = 0;
				num[k++] = atoi(&str[l]);
				l = i+1;
			}
			if(k>4) break;
		}
		if(k<5) {
			Ply[fd].extr->loginattempts+=1;
			if(Ply[fd].extr->loginattempts>5){
                		disconnect(fd);
                       	return;
               		}

			output(fd, "Please enter all 5 numbers.\n");
			ask_for(fd, ": ");
			RETURN(fd, create_ply, 6);
		}
		sum = 0;
		for(i=0; i<5; i++) {
			if(num[i] < 3 || num[i] > 18) {
				output(fd, "No stats < 3 or > 18 please.\n");
				output(fd, ": ");
				RETURN(fd, create_ply, 6);
			}
			sum += num[i];
		}
		if(sum > 54) {
			output(fd, "Stat total may not exceed 54.\n");
			output(fd, ": ");
			RETURN(fd, create_ply, 6);
		}
		Ply[fd].ply->strength = num[0];
		Ply[fd].ply->dexterity = num[1];
		Ply[fd].ply->constitution = num[2];
		Ply[fd].ply->intelligence = num[3];
		Ply[fd].ply->piety = num[4];
		output(fd, "\n		      ----- WEAPON PROFICIENCIES -----");
		output(fd, "\nChoose a weapons proficiency:");
		output(fd, "\n[A] Sharp  [B] Thrusting  [C] Blunt");
		output(fd, "\n[D] Pole   [E] Missile    [F] Bare Hand\n\n");
		ask_for(fd, ": ");
		RETURN(fd, create_ply, 7);
	case 7:
		switch(low(str[0])) {
			case 'a': Ply[fd].ply->proficiency[0]=1024; break;
			case 'b': Ply[fd].ply->proficiency[1]=1024; break;
			case 'c': Ply[fd].ply->proficiency[2]=1024; break;
			case 'd': Ply[fd].ply->proficiency[3]=1024; break;
			case 'e': Ply[fd].ply->proficiency[4]=1024; break;
			case 'f': Ply[fd].ply->proficiency[5]=1024; break;
			default: output(fd, "        <-- Try again.");
				ask_for(fd, ": ");
				RETURN(fd, create_ply, 7);
		}
		output(fd, "\n                    ----- MAGIC PROFICIENCIES -----");
		output(fd, "\nChoose a magic proficiency:");
		output(fd, "\n[A] Earth    [B] Wind     [C] Fire     [D] Water");
		output(fd, "\n");
		ask_for(fd, ": ");
		RETURN(fd, create_ply, 8);
	case 8:
		switch(low(str[0])) {
			case 'a': Ply[fd].ply->realm[0]=2048; break;
			case 'b': Ply[fd].ply->realm[1]=2048; break;
			case 'c': Ply[fd].ply->realm[2]=2048; break;
			case 'd': Ply[fd].ply->realm[3]=2048; break;
			default: output(fd, "        <-- Try again.");
				ask_for(fd, ": ");
				RETURN(fd, create_ply, 8);
		}
		output(fd, "\n			  ----- ALIGNMENT -----");
                output(fd, "\nLawful players cannot attack or steal from other players, nor can they\nbe attacked or stolen from.\n");
                output(fd, "\nChaotic players may attack or steal from other chaotic players, and they can\nbe attacked or stolen from.\n");
		output(fd, "\nIf you choose chaos you may be attacked repeatedly, and harassed continuously.\n");
		output(fd, "\nIt is STRONGLY suggested that you choose to be lawful if this is your first time creating a character.\n");
		output(fd, "\nAlignment also dictates which guild you may join.\n\n");
		ask_for(fd, "Choose an alignment, [C] Chaotic or [L] Lawful: "); 
		RETURN(fd, create_ply, 9);
	case 9:
		if(low(str[0]) == 'c')
			F_SET(Ply[fd].ply, PCHAOS);
		else if(low(str[0]) == 'l')
			F_CLR(Ply[fd].ply, PCHAOS);
		else {
			ask_for(fd, "[C] Chaotic or [L] Lawful: ");
			RETURN(fd, create_ply, 9);
		}
		output(fd, "\nAvailable races:");
		output(fd, "\n      /----------------- ELF RACES ----------------\\");
		output(fd, "\n      |  [A] Elf    [B] Dark-Elf    [C] Half-Elf   |");
		output(fd, "\n      |----------------- ORC RACES ----------------|");
		output(fd, "\n      |  [D] Orc    [E] Half-Orc    [F] Goblin     |");
		output(fd, "\n      |----------------- BIG RACES ----------------|");
		output(fd, "\n      |  [G] Troll  [H] Ogre        [I] Half-Giant |");
		output(fd, "\n      |-------------- LITTLE RACES ----------------|");
		output(fd, "\n      |  [J] Dwarf  [K] Hobbit      [L] Gnome      |");
		output(fd, "\n      |------------ HUMANOID RACES ----------------|");
		output(fd, "\n      |  [M] Human                                 |");
		output(fd, "\n      \\-------------------------------------------/\n\n");
		ask_for(fd, "Choose one: ");
		RETURN(fd, create_ply, 10);
	case 10:
		switch(low(str[0])) {
 		case 'a': Ply[fd].ply->race = ELF; break;
 		case 'b': Ply[fd].ply->race = DARKELF; break;
		case 'c': Ply[fd].ply->race = HALFELF; break;
		case 'd': Ply[fd].ply->race = ORC; break;
		case 'e': Ply[fd].ply->race = HALFORC; break;
		case 'f': Ply[fd].ply->race = GOBLIN; break;
		case 'g': Ply[fd].ply->race = TROLL; break;
		case 'h': Ply[fd].ply->race = OGRE; break;
		case 'i': Ply[fd].ply->race = HALFGIANT; break;
		case 'j': Ply[fd].ply->race = DWARF; break;
		case 'k': Ply[fd].ply->race = HOBBIT; break;
		case 'l': Ply[fd].ply->race = GNOME; break;
		case 'm': Ply[fd].ply->race = HUMAN; break;
		}
		if(!Ply[fd].ply->race) {
			ask_for(fd, "Choose one: ");
			RETURN(fd, create_ply, 10);
		}

		switch(Ply[fd].ply->race) {
		case DARKELF:
			Ply[fd].ply->intelligence+=1;
			Ply[fd].ply->piety-=2;
			Ply[fd].ply->dexterity+=1;
			break;
		case DWARF: 
			Ply[fd].ply->strength++; 
			Ply[fd].ply->piety-=2; 
			Ply[fd].ply->constitution++;
			break;
		case ELF: 
			Ply[fd].ply->intelligence+=2;
			Ply[fd].ply->constitution--;
			Ply[fd].ply->strength--; 
			break;
		case GOBLIN:
			Ply[fd].ply->intelligence--;
			Ply[fd].ply->dexterity++;
			break;
		case GNOME:
			Ply[fd].ply->piety+=2;
			Ply[fd].ply->strength--;
			Ply[fd].ply->constitution--;
			break;
		case HALFELF: 
			Ply[fd].ply->intelligence++; 
			Ply[fd].ply->strength--; 
			break;
		case HALFORC:
			Ply[fd].ply->strength++; 
			Ply[fd].ply->piety--;
			break;			
		case HOBBIT: 
			Ply[fd].ply->dexterity+=2; 
			Ply[fd].ply->strength--; 
			Ply[fd].ply->piety--;
			break;
		case HUMAN: 
			Ply[fd].ply->constitution++; 
			break;
		case OGRE:
			Ply[fd].ply->strength++;
			Ply[fd].ply->intelligence--;
			break;
		case ORC: 
			Ply[fd].ply->strength++; 
			Ply[fd].ply->dexterity++; 
			Ply[fd].ply->intelligence--;
			Ply[fd].ply->piety--;
			break;
		case TROLL: 
			Ply[fd].ply->piety--; 
			Ply[fd].ply->intelligence--;
			Ply[fd].ply->constitution+=2;
			break;
		case HALFGIANT: 
			Ply[fd].ply->strength+=2; 
			Ply[fd].ply->intelligence--; 
			Ply[fd].ply->piety--; 
			break;
		}

		sprintf(g_buffer, "%c%c%cChoose a password (up to 14 chars): ", 255,251,1);
		output(fd, g_buffer);
		RETURN(fd, create_ply, 11);
	case 11:
		/* check for double login */
		if ( find_who(Ply[fd].extr->tempstr[0] ) )
		{
			char *str_temp = "That character is already playing.\n\r";
			scwrite(fd, str_temp, strlen(str_temp));
			disconnect(fd);
			return;
		}

		if(strlen(str) > 14) {
			output(fd, "Too long.\nChoose a password: ");
			RETURN(fd, create_ply, 11);
		}
		if(strlen(str) < 3) {
			output(fd, "Too short.\nChoose a password: ");
			RETURN(fd, create_ply, 11);
		}

		/* un-invis the input */
		sprintf(g_buffer, "%c%c%c\n", 255,252,1);
		output(fd, g_buffer);

		strncpy(Ply[fd].ply->password, str, 14);
		strcpy(Ply[fd].ply->name, Ply[fd].extr->tempstr[0]);
		up_level(Ply[fd].ply);
		Ply[fd].ply->fd = fd;

		init_ply(Ply[fd].ply);
		save_ply(Ply[fd].ply);
		output(fd, "\n");
		F_SET(Ply[fd].ply, PNOAAT);

                if(Ply[fd].ply->class > BUILDER) {
                        for(i=0;i<get_spell_list_size();i++)
                               S_SET(Ply[fd].ply,i);
                }

		/* Set-up newbies w/ some basic stuff */
		Ply[fd].ply->gold = 100;
		for(obj = 0; obj < 4; obj++) {
		m=load_obj(init_eq[obj], &obj_ptr);
		if(m > -1) 
		    add_obj_crt(obj_ptr, Ply[fd].ply);
		}
		
		for(obj=0;obj<5;obj++) {
			if(Ply[fd].ply->proficiency[obj] == 1024) {
				if(load_obj(init_eq[obj+4], &obj_ptr) > -1)
					add_obj_crt(obj_ptr, Ply[fd].ply);
			}
		}

		if(Ply[fd].ply->realm[0] == 2048) 
			S_SET(Ply[fd].ply, SRUMBL);
		if(Ply[fd].ply->realm[1] == 2048) 
			S_SET(Ply[fd].ply, SHURTS);
		if(Ply[fd].ply->realm[2] == 2048) 
			S_SET(Ply[fd].ply, SBURNS);
		if(Ply[fd].ply->realm[3] == 2048) 
			S_SET(Ply[fd].ply, SBLIST);

		if(MSP) output(fd, "This world supports the Mud Sound Protocol.  Type 'set sound' to enable.\n");
		output(fd, "Type 'welcome' at prompt to get more info on the game and help you get started.\n\n");

		RETURN(fd, command, 1);
	}
}

/**********************************************************************/
/*				command			 	      */
/**********************************************************************/

/* This function handles the main prompt commands, and calls the   	*/
/* appropriate function, depending on what service is requested by the 	*/
/* player.								*/

void command(int fd, int param, char *str )
{
	cmd	cmnd;
	int	n;
	unsigned char ch;

	/* zero out the command first */
	memset(&cmnd, 0, sizeof(cmnd));

	/*
	this logn command will print out all the commands entered by players.
	It should be used in extreme cases when trying to isolate a players
	input which may be causing a crash.
	*/
	if(RECORD_ALL)
	{
		sprintf(g_buffer, "\n%s-%d (%d): %s",Ply[fd].ply->name,fd,Ply[fd].ply->rom_num,str);  
		logn("all_cmd", g_buffer);  
	}

	switch(param) {
	case 1:

		if(F_ISSET(Ply[fd].ply, PHEXLN)) {
			for(n=0; n < (int) strlen(str);n++) {
				ch = str[n];
				sprintf(g_buffer, "%02X", ch);
				output(fd, g_buffer);
			}
			output(fd, "\n");
		}

		if(!strcmp(str, "!"))
			strncpy(str, Ply[fd].extr->lastcommand, 79);

		if(str[0]) {
			for(n=0; str[n] && str[n] == ' '; n++) ;
			strncpy(Ply[fd].extr->lastcommand, &str[n], 79);
		}

		strncpy(cmnd.fullstr, str, 255);
		lowercize(str, 0);
		parse(str, &cmnd); 
		
		n = 0;

		if(cmnd.num)
			n = process_cmd(fd, &cmnd);
		else
			n = PROMPT;

		if(n == DISCONNECT) {
			scwrite(fd, "Goodbye!\n\r\n\r", 11);
			disconnect(fd);
			return;
		}
		else if(n == PROMPT) {
			ply_prompt(Ply[fd].ply );
		}

		if(n != DOPROMPT) {
			RETURN(fd, command, 1);
		}
		else
			return;
	}
}


/**********************************************************************/
/*				parse				      */
/**********************************************************************/

/* This function takes the string in the first parameter and breaks it */
/* up into its component words, stripping out useless words.  The      */
/* resulting words are stored in a command structure pointed to by the */
/* second argument. 						       */

void parse(char	*str, cmd *cmnd )
{
	int	i, j, l, n, o, art;
	char	token[MAX_TOKEN_SIZE];
	int		isquote;

	l = n = 0;
	j = strlen(str);
	isquote = 0;

	for(i=0; i<=j; i++) {

		/* look for first non space or comment  */
		if (str[i] == ' '  || str[i] == '#' )
			continue;

		/* ok we at first non space */
		if ( str[i] == '\"' )
		{
			isquote = 1;
			/* skip quote char */
			i++;
		}
		
		/* save this position as the begining of a token */
		l = i;

		/* now find the end of the token */
		if( isquote )
		{
			while ( str[i] != '\0' && str[i] != '\"' )
			{
				i++;
			}

			/* terminate the token */
			if ( str[i] == '\"' )
			{
				str[i] = '\0';
			}
		}
		else
		{
			while ( str[i] != '\0' && str[i] != ' ' && str[i] != '#')
			{
				i++;
			}

			/* terminate the token */
			str[i] = '\0';
		}

		/* dont overflow the buffers */
		strncpy(token, &str[l], MAX_TOKEN_SIZE); 
		token[MAX_TOKEN_SIZE - 1] = 0;

		/* whas there any thing here? */
		if(!strlen(token)) 
		{
			isquote = 0;
			continue;
		}

		if ( isquote )
		{
			strncpy(cmnd->str[n], token, MAX_TOKEN_SIZE);
			cmnd->str[n][MAX_TOKEN_SIZE - 1] = '\0';
			cmnd->val[n] = 1L;
			isquote = 0;
			n++;
		}
		else
		{
			/* Ignore article/useless words */
			o = art = 0;
			while(article[o][0] != '@') {
				if(!strcmp(article[o++], token)) {
					art = 1;
					break;
				}
			}
			if(art) 
				continue;

			/* Copy into command structure */
			if(n == 0) {
				strncpy(cmnd->str[n], token, MAX_TOKEN_SIZE);
				cmnd->str[n][MAX_TOKEN_SIZE - 1] = '\0';
				/* set the value to 1 in case there is non following */
				cmnd->val[n] = 1L;
				n++;
			}
			else if(isdigit((int)token[0]) || (token[0] == '-' &&
				isdigit((int)token[1]))) {
				/* this is a value for the previous command */
				cmnd->val[MAX(0, n - 1)] = atol(token);
			}
			else {
				strncpy(cmnd->str[n], token, MAX_TOKEN_SIZE);
				cmnd->str[n][MAX_TOKEN_SIZE - 1] = '\0';
				/* set the value to 1 in case there is non following */
				cmnd->val[n] = 1L;
				n++;
			}
		}

		if(n >= COMMANDMAX) {
			break;
		}
	}

	/* set the number of tokens in the command struct */
	cmnd->num = n;

	return;
}


/**********************************************************************/
/*				process_cmd			      */
/**********************************************************************/

/* This function takes the command structure of the person at the socket */
/* in the first parameter and interprets the person's command.		 */

int process_cmd(int	fd, cmd	*cmnd )
{
	int	match=0, cmdno=0, c=0;

	do {
		if(!strcmp(cmnd->str[0], cmdlist[c].cmdstr)) {
			match = 1;
			cmdno = c;
			break;
		}
		else if(!strncmp(cmnd->str[0], cmdlist[c].cmdstr, 
			strlen(cmnd->str[0]))) {
			match++;
			cmdno = c;
		}
		c++;
	} while(cmdlist[c].cmdno);

	if(match == 0) {
		sprintf(g_buffer, "The command \"%s\" does not exist.\n", cmnd->str[0]);
		output(fd, g_buffer);
		RETURNV(fd, command, 1, PROMPT);
	}

	else if(match > 1) {
		output(fd, "Command is not unique.\n");
		RETURNV(fd, command, 1, PROMPT);
	}

	if(cmdlist[cmdno].cmdno < 0)
		return(special_cmd(Ply[fd].ply, 0-cmdlist[cmdno].cmdno, cmnd));
	
	return((*cmdlist[cmdno].cmdfn)(Ply[fd].ply, cmnd));
}


int checkdouble( char *name )
{
	char	path[128], tempname[80];
	FILE 	*fp;
	int	rtn=0;

	sprintf(path, "%s/simul/%s", get_player_path(), name);
	fp = fopen(path, "r");
	if(!fp)
		return(0);

	while(!feof(fp)) {
		fgets(tempname, 80, fp);
		tempname[strlen(tempname)-1] = 0;
		if(!strcmp(tempname, name))
			continue;
		if(find_who(tempname)) {
			rtn = 1;
			break;
		}
	}

	fclose(fp);
	return(rtn);
}


/**********************************************************************/
/*				check_double_login			      */
/**********************************************************************/

/* This function takes the socket if a player about to connect and */
/* disconnects all other players with the same name to prevent */
/* double logins with the same char */
void check_double_login(int fd)
{
	int	i;

	/* check for multiple logins with same char */
	for(i=0; i<Tablesize; i++)
	{
		if(Ply[i].ply && i != fd)
		{
			if(!strcmp(Ply[i].ply->name, Ply[fd].ply->name))
			{
				scwrite(fd, "No simultaneous playing.\n\r", 26);
				disconnect(i);
			}
		}
	}

	return;
}



/**********************************************************************/
/*				contains_bad_word		      */
/**********************************************************************/
/* This function takes a string and checks against the bad word list  */
/* to see if it contains any bad words.				      */
/* Returns 1 (TRUE) if the string contains a bad word, 0 otherwise.   */
int contains_bad_word(char *str)
{
	char		path[256];
	char		bad_word[80];
	char		*temp_str;
	FILE		*fp;

	/* default is OK if no file or bad word not found */
	int			nReturn = 0;

	sprintf(path, "%s/badwords.txt", get_log_path());
	fp=fopen(path, "r");
	if(fp)
	{
		/* make a copy of the string and lowercase it */
		temp_str = malloc( strlen(str) + 1);
		if ( temp_str)
		{
			/* copy it in th temp buffer */
			strcpy(temp_str, str );

			/* and make it lowercase */
			lowercize(temp_str, 0);

			while(!feof(fp)) 
			{
				fscanf(fp, "%s", bad_word);

				/* make this lower case too */
				lowercize( bad_word, 0 );

				/* check for the word anywhere in the string */
				if(strstr(temp_str, bad_word) != NULL) 
				{
					/* found one */
					nReturn = 1;
					break;
				}
			}

			/* now dont forget to clean up */
			free(temp_str);
		}
		else
		{
			/* TODO - error message here about out of memory */
		}

		fclose( fp );
	}

	return( nReturn );
}
