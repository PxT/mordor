/*
 * COMMAND1.C:
 *
 *	Command handling/parsing routines.
 *
 *	Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"
#include <ctype.h>
#include <sys/time.h>
#include <string.h>

/**********************************************************************/
/*				login				      */
/**********************************************************************/

/* This function is the first function that gets input from a player when */
/* he logs in.  It asks for the player's name and password, and performs  */
/* the according function calls.					  */

void login(fd, param, str)
int 	fd;
int	param;
char	*str;
{
	int		i;
	extern int	Numplayers;
	char		tempstr[20], str2[50];
	long 		t;
	creature	*ply_ptr;

	switch(param) {
	case 0:
		if(strcmp(Ply[fd].extr->tempstr[0], str)) {
			disconnect(fd);
			return;
		}
		print(fd, "\nPlease enter name: ");
		RETURN(fd, login, 1);
	case 1:
		if(!isalpha(str[0])) {
			print(fd, "Please enter name: ");
			RETURN(fd, login, 1);
		}

		if(strlen(str) < 3) {
			print(fd, "Name must be at least 3 characters.\n\n");
			print(fd, "Please enter name: ");
			RETURN(fd, login, 1);
		}
		if(strlen(str) >= 20) {
			print(fd, "Name must be less than characters.\n\n");
			print(fd, "Please enter name: ");
			RETURN(fd, login, 1);
		}

		for(i=0; i<strlen(str); i++)
			if(!isalpha(str[i])) {
				print(fd, "Name must be alphabetic.\n\n");
				print(fd, "Please enter name: ");
				RETURN(fd, login, 1);
			}

		lowercize(str, 1);
		str[25]=0;
		if(load_ply(str, &ply_ptr) < 0) {
			strcpy(Ply[fd].extr->tempstr[0], str);
			print(fd, "\n%s? Did I get that right? ", str);
			RETURN(fd, login, 2);
		}

		else {
			ply_ptr->fd = -1;
			Ply[fd].ply = ply_ptr;
#ifdef CHECKDOUBLE
			if(checkdouble(ply_ptr->name)) {
				write(fd, "No simultaneous playing.\n\r", 26);
				disconnect(fd);
				return;
			}
#endif
			print(fd, "%c%c%cPlease enter password: ", 255, 251, 1);
			RETURN(fd, login, 3);
		}

	case 2:
		if(str[0] != 'y' && str[0] != 'Y') {
			Ply[fd].extr->tempstr[0][0] = 0;
			print(fd, "Please enter name: ");
			RETURN(fd, login, 1);
		}
		else {
			print(fd, "\nHit return: ");
			RETURN(fd, create_ply, 1);
		}

	case 3:
		if(strcmp(str, Ply[fd].ply->password)) {
			write(fd, "\255\252\1\n\rIncorrect.\n\r", 17);
			disconnect(fd);
			return;
		}
		else {
			print(fd, "%c%c%c\n\r", 255, 252, 1);
			strcpy(tempstr, Ply[fd].ply->name);
			for(i=0; i<Tablesize; i++)
				if(Ply[i].ply && i != fd)
					if(!strcmp(Ply[i].ply->name, 
					   Ply[fd].ply->name))
						disconnect(i);	
			free_crt(Ply[fd].ply);
		if(load_ply(tempstr, &Ply[fd].ply) < 0)
		{
			write(fd, "Player nolonger exits!\n\r", 24);
                        t = time(0);
                        strcpy(str2, (char *)ctime(&t));
                        str2[strlen(str2)-1] = 0;
                        logn("sui_crash","%s: %s (%s) suicided.\n", 
				str2, Ply[fd].ply->name, Ply[fd].io->address);         
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

void create_ply(fd, param, str)
int	fd;
int	param;
char	*str;
{
	int 	i, k, l, n, sum;
	int	num[5];

	switch(param) {
	case 1:
		print(fd,"\n\n");
		Ply[fd].ply = (creature *)malloc(sizeof(creature));
		if(!Ply[fd].ply)
			merror("create_ply", FATAL);
		zero(Ply[fd].ply, sizeof(creature));
		Ply[fd].ply->fd = -1;
		Ply[fd].ply->rom_num = 1;
		print(fd, "Male or Female: ");
		RETURN(fd, create_ply, 2);
	case 2:
		if(low(str[0]) != 'm' && low(str[0]) != 'f') {
			print(fd, "Male or Female: ");
			RETURN(fd, create_ply, 2);
		}
		if(low(str[0]) == 'm')
			F_SET(Ply[fd].ply, PMALES);
		print(fd, "\nAvailable classes:\n");
		print(fd, "Assassin, Barbarian, Cleric, Fighter,\n");
		print(fd, "Mage, Paladin, Ranger, Thief\n");
		print(fd, "Choose one: ");
		RETURN(fd, create_ply, 3);
	case 3:
		switch(low(str[0])) {
			case 'a': Ply[fd].ply->class = ASSASSIN; break;
			case 'b': Ply[fd].ply->class = BARBARIAN; break;
			case 'c': Ply[fd].ply->class = CLERIC; break;
			case 'f': Ply[fd].ply->class = FIGHTER; break;
			case 'm': Ply[fd].ply->class = MAGE; break;
			case 'p': Ply[fd].ply->class = PALADIN; break;
			case 'r': Ply[fd].ply->class = RANGER; break;
			case 't': Ply[fd].ply->class = THIEF; break;
			default: print(fd, "Choose one: "); 
				 RETURN(fd, create_ply, 3);
		}
		print(fd, "\nYou have 54 points to distribute among your 5 stats. Please enter your 5\nnumbers in the following order: Strength, Dexterity, Constitution,\nIntelligence, Piety.  No stat may be smaller than 3 or larger than 18.\nUse the following format: 
## ## ## ## ##\n");
		print(fd, ": ");
		RETURN(fd, create_ply, 4);
	case 4:
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
			print(fd, "Please enter all 5 numbers.\n");
			print(fd, ": ");
			RETURN(fd, create_ply, 4);
		}
		sum = 0;
		for(i=0; i<5; i++) {
			if(num[i] < 3 || num[i] > 18) {
				print(fd, "No stats < 3 or > 18 please.\n");
				print(fd, ": ");
				RETURN(fd, create_ply, 4);
			}
			sum += num[i];
		}
		if(sum > 54) {
			print(fd, "Stat total may not exceed 54.\n");
			print(fd, ": ");
			RETURN(fd, create_ply, 4);
		}
		Ply[fd].ply->strength = num[0];
		Ply[fd].ply->dexterity = num[1];
		Ply[fd].ply->constitution = num[2];
		Ply[fd].ply->intelligence = num[3];
		Ply[fd].ply->piety = num[4];
		print(fd, "\nChoose a weapons proficiency:\n");
		print(fd, "Sharp, Thrusting, Blunt, Pole, Missile.\n");
		print(fd, ": ");
		RETURN(fd, create_ply, 5);
	case 5:
		switch(low(str[0])) {
			case 's': Ply[fd].ply->proficiency[0]=1024; break;
			case 't': Ply[fd].ply->proficiency[1]=1024; break;
			case 'b': Ply[fd].ply->proficiency[2]=1024; break;
			case 'p': Ply[fd].ply->proficiency[3]=1024; break;
			case 'm': Ply[fd].ply->proficiency[4]=1024; break;
			default: print(fd, "Try again.\n: ");
				 RETURN(fd, create_ply, 5);
		}
		print(fd, "\nLawful players cannot attack or steal from other players, nor can they\nbe attacked or stolen from by other players.");
		print(fd, "\nChaotic players may attack or steal from other chaotic players, and they can\nbe attacked or stolen from by other chaotic players.\n");
		print(fd, "\nChoose an alignment, Chaotic or Lawful: ");
		RETURN(fd, create_ply, 6);
	case 6:
		if(low(str[0]) == 'c')
			F_SET(Ply[fd].ply, PCHAOS);
		else if(low(str[0]) == 'l')
			F_CLR(Ply[fd].ply, PCHAOS);
		else {
			print(fd, "Chaotic or Lawful: ");
			RETURN(fd, create_ply, 6);
		}
		print(fd, "\nAvailable races:");
		print(fd, "\nDwarf, Elf, Gnome, Half-elf,");
		print(fd, "\nHalf-giant, Hobbit, Human, Orc");
		print(fd, "\nChoose one: ");
		RETURN(fd, create_ply, 7);
	case 7:
		switch(low(str[0])) {
		case 'd': Ply[fd].ply->race = DWARF; break;
		case 'e': Ply[fd].ply->race = ELF; break;
		case 'g': Ply[fd].ply->race = GNOME; break;
		case 'o': Ply[fd].ply->race = ORC; break;
		case 'h': switch(low(str[1])) {
			case 'a': switch(low(str[5])) {
				case 'e': Ply[fd].ply->race = HALFELF; break;
				case 'g': Ply[fd].ply->race = HALFGIANT; break;
				}
				break;
			case 'o': Ply[fd].ply->race = HOBBIT; break;
			case 'u': Ply[fd].ply->race = HUMAN; break;
			}
			break;
		}
		if(!Ply[fd].ply->race) {
			print(fd, "\nChoose one: ");
			RETURN(fd, create_ply, 7);
		}

		switch(Ply[fd].ply->race) {
		case DWARF: 
			Ply[fd].ply->strength++; 
			Ply[fd].ply->piety--; 
			break;
		case ELF: 
			Ply[fd].ply->intelligence+=2;
			Ply[fd].ply->constitution--;
			Ply[fd].ply->strength--; 
			break;
		case GNOME:
			Ply[fd].ply->piety++;
			Ply[fd].ply->strength--;
			break;
		case HALFELF: 
			Ply[fd].ply->intelligence++; 
			Ply[fd].ply->constitution--; 
			break;
		case HOBBIT: 
			Ply[fd].ply->dexterity++; 
			Ply[fd].ply->strength--; 
			break;
		case HUMAN: 
			Ply[fd].ply->constitution++; 
			break;
		case ORC: 
			Ply[fd].ply->strength++; 
			Ply[fd].ply->constitution++;
			Ply[fd].ply->dexterity--; 
			Ply[fd].ply->intelligence--;
			break;
		case HALFGIANT: 
			Ply[fd].ply->strength+=2; 
			Ply[fd].ply->intelligence--; 
			Ply[fd].ply->piety--; 
			break;
		}

		print(fd, "\nChoose a password (up to 14 chars): ");
		RETURN(fd, create_ply, 8);
	case 8:
		if(strlen(str) > 14) {
			print(fd, "Too long.\nChoose a password: ");
			RETURN(fd, create_ply, 8);
		}
		if(strlen(str) < 3) {
			print(fd, "Too short.\nChoose a password: ");
			RETURN(fd, create_ply, 8);
		}
		strncpy(Ply[fd].ply->password, str, 14);
		strcpy(Ply[fd].ply->name, Ply[fd].extr->tempstr[0]);
		up_level(Ply[fd].ply);
		Ply[fd].ply->fd = fd;
		init_ply(Ply[fd].ply);
		save_ply(Ply[fd].ply->name, Ply[fd].ply);
		print(fd, "\n");

		print(fd, "Type 'welcome' at prompt to get more info on the game\nand help you get started.\n");

		RETURN(fd, command, 1);
	}
}

/**********************************************************************/
/*				command			 	      */
/**********************************************************************/

/* This function handles the main prompt commands, and calls the   	*/
/* appropriate function, depending on what service is requested by the 	*/
/* player.								*/

void command(fd, param, str)
int	fd;
int	param;
char	*str;
{
	cmd	cmnd;
	int	n;
	unsigned char ch;

#ifdef RECORD_ALL
/*
this logn commands wil print out all the commands entered by players.
It should be used in extreme case hen trying to isolate a players
input which causes a crash.
*/

logn("all_cmd","\n%s-%d (%d): %s\n",Ply[fd].ply->name,fd,Ply[fd].ply->rom_num,str);  
#endif RECORD_ALL

	switch(param) {
	case 1:

		if(F_ISSET(Ply[fd].ply, PHEXLN)) {
			for(n=0;n<strlen(str);n++) {
				ch = str[n];
				print(fd, "%02X", ch);
			}
			print(fd, "\n");
		}

		if(!strcmp(str, "!"))
			strncpy(str, Ply[fd].extr->lastcommand, 79);

		if(str[0]) {
			for(n=0; str[n] && str[n] == ' '; n++) ;
			strncpy(Ply[fd].extr->lastcommand, &str[n], 79);
		}

		strncpy(cmnd.fullstr, str, 255);
		lowercize(str, 0);
		parse(str, &cmnd); n = 0;

		if(cmnd.num)
			n = process_cmd(fd, &cmnd);
		else
			n = PROMPT;

		if(n == DISCONNECT) {
			write(fd, "Goodbye!\n\r\n\r", 11);
			disconnect(fd);
			return;
		}
		else if(n == PROMPT) {
			if(F_ISSET(Ply[fd].ply, PPROMP))
				sprintf(str, "(%d H %d M): ", 
					Ply[fd].ply->hpcur, Ply[fd].ply->mpcur);
			else
				strcpy(str, ": ");
			write(fd, str, strlen(str));
			if(Spy[fd] > -1) write(Spy[fd], str, strlen(str));
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

void parse(str, cmnd)
char	*str;
cmd	*cmnd;
{
	int	i, j, l, m, n, o, art;
	char	tempstr[25];

	l = m = n = 0;
	j = strlen(str);

	for(i=0; i<=j; i++) {
		if(str[i] == ' ' || str[i] == '#' || str[i] == 0) {
			str[i] = 0;	/* tokenize */

			/* Strip extra white-space */
			while((str[i+1] == ' ' || str[i] == '#') && i < j+1)
				str[++i] = 0;

			strncpy(tempstr, &str[l], 24); tempstr[24] = 0;
			l = i+1;
			if(!strlen(tempstr)) continue;

			/* Ignore article/useless words */
			o = art = 0;
			while(article[o][0] != '@') {
				if(!strcmp(article[o++], tempstr)) {
					art = 1;
					break;
				}
			}
			if(art) continue;

			/* Copy into command structure */
			if(n == m) {
				strncpy(cmnd->str[n++], tempstr, 20);
				cmnd->val[m] = 1L;
			}
			else if(isdigit(tempstr[0]) || (tempstr[0] == '-' &&
				isdigit(tempstr[1]))) {
				cmnd->val[m++] = atol(tempstr);
			}
			else {
				strncpy(cmnd->str[n++], tempstr, 20);
				cmnd->val[m++] = 1L;
			}

		}
		if(m >= COMMANDMAX) {
			n = 5;
			break;
		}
	}

	if(n > m)
		cmnd->val[m++] = 1L;
	cmnd->num = n;

}

/**********************************************************************/
/*				process_cmd			      */
/**********************************************************************/

/* This function takes the command structure of the person at the socket */
/* in the first parameter and interprets the person's command.		 */

int process_cmd(fd, cmnd)
int	fd;
cmd	*cmnd;
{
	int	match=0, cmdno=0, c=0, n;

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
		print(fd, "The command \"%s\" does not exist.\n",
		      cmnd->str[0]);
		RETURN(fd, command, 1);
	}

	else if(match > 1) {
		print(fd, "Command is not unique.\n");
		RETURN(fd, command, 1);
	}

	if(cmdlist[cmdno].cmdno < 0)
		return(special_cmd(Ply[fd].ply, 0-cmdlist[cmdno].cmdno, cmnd));
	
	return((*cmdlist[cmdno].cmdfn)(Ply[fd].ply, cmnd));

}

#ifdef CHECKDOUBLE

int checkdouble(name)
char *name;
{
	char	path[128], tempname[80];
	FILE 	*fp;
	int	rtn=0;

	sprintf(path, "%s/simul/%s", PLAYERPATH, name);
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

#endif
