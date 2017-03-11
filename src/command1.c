/*
 * COMMAND1.C:
 *
 *	Command handling/parsing routines.
 *
 *	Copyright (C) 1991, 1992, 1993, 1997 Brooke Paul & Brett Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"
#include <ctype.h>
#include <string.h>
#ifdef DMALLOC
  #include "/usr/local/include/dmalloc.h"
#endif
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
	int		i, match=0;
	extern int	Numplayers;
	char		tempstr[20], str2[50], path[80], forbid[20];
	long 		t;
	creature	*ply_ptr;
	FILE		*fp;


	switch(param) {
	case 0:
		if(strcmp(Ply[fd].extr->tempstr[0], str)) {
			disconnect(fd);
			return;
		}
		print(fd, "\nPlease enter name: ");
		RETURN(fd, login, 1);
	case 1:
		/* Doneval: Here I check the name of the player. If it's the same
		of one of the mail's boards, bad. 
		Brooke, check the already made characters! *
		for(i=0;i<=BOARDS;i++) {
			if(!strcmp(boards[i], str)) {
				print(fd, "\nThis is the name of a mail board.\n");
				ask_for(fd, "Please enter name: ");
			}
		}
		*/

		if(!isalpha(str[0])) {
			print(fd, "Please enter name: ");
			RETURN(fd, login, 1);
		}

if(PARANOID) {
	/* Check for ident */
	if((!strcmp(Ply[fd].io->userid, "no_port") || !strcmp(Ply[fd].io->userid, "unknown")) && (strcmp(Ply[fd].io->address, "moria.bio.uci.edu") || strcmp(Ply[fd].io->address, "128.200.21.101"))) {
		print(fd, "\n\nI am unable to get authorization from your server.\n");
		print(fd, "Please try connecting from another host, or contact\n");
		print(fd, "your system administrator to request RFC 931 auth/identd.\n\n");
		print(fd, "If you have connected from this account in the past,\n");
		print(fd, "try logging in more slowly.\n\n");
		output_buf();
		disconnect(fd);
		return;
	}
}

	/* Check for double log */
	for(i=0; i<Tablesize; i++) {
		if(!Ply[i].ply) continue;
		if(Ply[i].ply->fd < 0) continue;
		if(strcmp(Ply[i].io->userid, "no_port") || strcmp(Ply[i].io->userid, "unknown")) continue;
		if(!strcmp(Ply[i].io->userid, Ply[fd].io->userid)) {
			match += 1;
			if(match > 0){
				print(fd, "\n\n%s\n", account_exists);
				print(fd, "Please only play one character at a time.\n\n");
				output_buf();
				disconnect(fd);
				return;
			}
		}
	}


	if(strlen(str) < 3) {
		print(fd, "Name must be at least 3 characters.\n\n");
		print(fd, "Please enter name: ");
		RETURN(fd, login, 1);
	}
	if(strlen(str) >= 20) {
		print(fd, "Name must be less than 20 characters.\n\n");
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

/* check to see the name is allowed */
/*
		sprintf(path, "%s/forbidden_name", PLAYERPATH);
        fp=fopen(path, "r");
        if(!fp)
                merror("ERROR - forbidden name", NONFATAL);
		else {
			while(!feof(fp)) {
                fscanf(fp, "%s", forbid);
                if(!strcmp(forbid, str)) {
                    print(fd, "That name is not allowed.\n");
					print(fd, "\nPlease enter name: ");
                    RETURN(fd, login, 1);
                }
			}
        }
*/
		if(load_ply(str, &ply_ptr) < 0) {
			strcpy(Ply[fd].extr->tempstr[0], str);
			print(fd, "\n%s? Did I get that right? ", str);
			RETURN(fd, login, 2);
		}

		else {
			ply_ptr->fd = -1;
			Ply[fd].ply = ply_ptr;

			if(CHECKDOUBLE) {
				if(checkdouble(ply_ptr->name)) {
			#ifdef WIN32
					scwrite(fd, "No simultaneous playing.\n\r", 26);
			#else 
					write(fd, "No simultaneous playing.\n\r", 26);
			#endif /* WIN32 */
					disconnect(fd);
					return;
				}
			} /* CHECKDOUBLE */

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
			#ifdef WIN32
			     	scwrite(fd, "\255\252\1\n\rIncorrect.\n\r", 17);
			#else
				write(fd, "\255\252\1\n\rIncorrect.\n\r", 17);
			#endif /* WIN32 */

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

/* It used to cause a crash if a player suicided at the same time */
/* as creating a new charater, this fixes it.			  */

			if(load_ply(tempstr, &Ply[fd].ply) < 0)
			{
				#ifdef WIN32
					scwrite(fd, "Player no longer exists!\n\r", 25);
				#else 	
					write(fd, "Player no longer exists!\n\r", 25);
				#endif /* WIN32 */        
			
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
	int 	i, k, l, n, sum, num[5], dm_need_pass=0;
	char	ph, ph_str[10];

	switch(param) {
	case 1:
/*		print(fd,"\n\n"); */
		Ply[fd].ply = (creature *)malloc(sizeof(creature));
		if(!Ply[fd].ply)
			merror("create_ply", FATAL);
		zero(Ply[fd].ply, sizeof(creature));
		Ply[fd].ply->fd = -1;
		Ply[fd].ply->rom_num = 1;


	if((!strcmp(Ply[fd].extr->tempstr[0],dmname[0])) ||
	   (!strcmp(Ply[fd].extr->tempstr[0],dmname[1])) ||
	   (!strcmp(Ply[fd].extr->tempstr[0],dmname[2])) ||
	   (!strcmp(Ply[fd].extr->tempstr[0],dmname[3])) ||
	   (!strcmp(Ply[fd].extr->tempstr[0],dmname[4])) ||
	   (!strcmp(Ply[fd].extr->tempstr[0],dmname[5])) ||
	   (!strcmp(Ply[fd].extr->tempstr[0],dmname[6]))) {
		print(fd, "\nA password is required to create that character.\n");
	 	print(fd, "Please enter password: ");
		output_buf();
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
			ask_for(fd, "[M] Male or [F] Female: ");
		}
		else
			print(fd, "[M] Male or [F] Female: ");
		RETURN(fd, create_ply, 4);
	case 4:
		if(low(str[0]) != 'm' && low(str[0]) != 'f') {
			if(ANSILINE)
				ask_for(fd, "[M] Male or [F] Female: ");
			else
				print(fd, "[M] Male or [F] Female: ");
			RETURN(fd, create_ply, 4);
		}
		if(low(str[0]) == 'm')
			F_SET(Ply[fd].ply, PMALES);
		print(fd, "\nAvailable classes:");
		print(fd, "\n		    /----- AVAILABLE CLASSES ------\\");
		print(fd, "\n		    |                              |");
		print(fd, "\n		    |  [A] Assassin  [B] Barbarian |");  
		print(fd, "\n		    |  [C] Cleric    [D] Fighter   |");
		print(fd, "\n		    |  [E] Bard      [F] Mage      |"); 
		print(fd, "\n		    |  [G] Paladin   [H] Ranger    |");
		print(fd, "\n		    |  [I] Thief     [J] Monk      |");
		print(fd, "\n		    |  [K] Druid     [L] Alchemist |");
		print(fd, "\n		    \\------------------------------/");
		if(ANSILINE)
			ask_for(fd, "Choose one: ");
		else
			print(fd, "\nChoose one: ");
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
				if(ANSILINE)
					ask_for(fd, "Choose one: ");
				else
					print(fd, "\nChoose one: ");
				RETURN(fd, create_ply, 5);
			}
		}
		if(!Ply[fd].ply->class) {
			print(fd, "Invalid selection: %s", str);
			if(ANSILINE)
				ask_for(fd, "Choose one: ");
			else
				print(fd, "\nChoose one: ");
			RETURN(fd, create_ply, 5);
		}

		print(fd, "\n		 ----- CHARACTER STATS -----");
		print(fd, "\nYou have 54 points to distribute among your 5 stats. Please enter your 5");
		print(fd, "\nnumbers in the following order: Strength, Dexterity, Constitution,");
		print(fd, "\nIntelligence, Piety.  No stat may be smaller than 3 or larger than 18.i");
		print(fd, "\nUse the following format: ## ## ## ## ##\n\n");
		
		if(ANSILINE)
			ask_for(fd, ": ");
		else
			print(fd, ": ");
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
			print(fd, "Please enter all 5 numbers.\n");
			if(ANSILINE)
				ask_for(fd, ": ");
			else
				print(fd, "\n: ");
			RETURN(fd, create_ply, 6);
		}
		sum = 0;
		for(i=0; i<5; i++) {
			if(num[i] < 3 || num[i] > 18) {
				print(fd, "No stats < 3 or > 18 please.\n");
				print(fd, ": ");
				RETURN(fd, create_ply, 6);
			}
			sum += num[i];
		}
		if(sum > 54) {
			print(fd, "Stat total may not exceed 54.\n");
			print(fd, ": ");
			RETURN(fd, create_ply, 6);
		}
		Ply[fd].ply->strength = num[0];
		Ply[fd].ply->dexterity = num[1];
		Ply[fd].ply->constitution = num[2];
		Ply[fd].ply->intelligence = num[3];
		Ply[fd].ply->piety = num[4];
		print(fd, "\n		      ----- WEAPON PROFICIENCIES -----");
		print(fd, "\nChoose a weapons proficiency:");
		print(fd, "\n[A] Sharp  [B] Thrusting  [C] Blunt");
		print(fd, "\n[D] Pole   [E] Missile\n\n");
		if(ANSILINE)
			ask_for(fd, ": ");
		else
			print(fd, ": ");
		RETURN(fd, create_ply, 7);
	case 7:
		switch(low(str[0])) {
			case 'a': Ply[fd].ply->proficiency[0]=1024; break;
			case 'b': Ply[fd].ply->proficiency[1]=1024; break;
			case 'c': Ply[fd].ply->proficiency[2]=1024; break;
			case 'd': Ply[fd].ply->proficiency[3]=1024; break;
			case 'e': Ply[fd].ply->proficiency[4]=1024; break;
			default: print(fd, "        <-- Try again.");
				if(ANSILINE)
					ask_for(fd, ": ");
				else
					print(fd, "\n: ");
				RETURN(fd, create_ply, 7);
		}
		print(fd, "\n			  ----- ALIGNMENT -----");
		print(fd, "\nLawful players cannot attack or steal from other players, nor can they\nbe attacked or stolen from by other players.");
		print(fd, "\nChaotic players may attack or steal from other chaotic players, and they can\nbe attacked or stolen from by other chaotic players.\n\n");
		if(ANSILINE)
			ask_for(fd, "Choose an alignment, [C] Chaotic or [L] Lawful: "); 
		else
			print(fd, "\nChoose an alignment, [C] Chaotic or [L] Lawful: ");
		RETURN(fd, create_ply, 8);
	case 8:
		if(low(str[0]) == 'c')
			F_SET(Ply[fd].ply, PCHAOS);
		else if(low(str[0]) == 'l')
			F_CLR(Ply[fd].ply, PCHAOS);
		else {
			if(ANSILINE)
				ask_for(fd, "[C] Chaotic or [L] Lawful: ");
			else
				print(fd, "\n[C] Chaotic or [L] Lawful: ");
			RETURN(fd, create_ply, 8);
		}
		print(fd, "\nAvailable races:");
		print(fd, "\n      /----------------- ELF RACES ----------------\\");
		print(fd, "\n      |  [A] Elf    [B] Dark-Elf    [C] Half-Elf   |");
		print(fd, "\n      |----------------- ORC RACES ----------------|");
		print(fd, "\n      |  [D] Orc    [E] Half-Orc    [F] Goblin     |");
		print(fd, "\n      |----------------- BIG RACES ----------------|");
		print(fd, "\n      |  [G] Troll  [H] Ogre        [I] Half-Giant |");
		print(fd, "\n      |-------------- LITTLE RACES ----------------|");
		print(fd, "\n      |  [J] Dwarf  [K] Hobbit      [L] Gnome      |");
		print(fd, "\n      |------------ HUMANOID RACES ----------------|");
		print(fd, "\n      |  [M] Human                                 |");
		print(fd, "\n      \\-------------------------------------------/\n\n");
		if(ANSILINE)
			ask_for(fd, "Choose one: ");
		else
			print(fd, "\nChoose one: ");
		RETURN(fd, create_ply, 9);
	case 9:
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
			if(ANSILINE)
				ask_for(fd, "Choose one: ");
			else
				print(fd, "\nChoose one: ");
			RETURN(fd, create_ply, 9);
		}

		switch(Ply[fd].ply->race) {
		case DARKELF:
			Ply[fd].ply->intelligence+=1;
			Ply[fd].ply->constitution+=1;
			Ply[fd].ply->piety-=2;
			break;
		case DWARF: 
			Ply[fd].ply->strength++; 
			Ply[fd].ply->piety--; 
			break;
		case ELF: 
			Ply[fd].ply->intelligence+=2;
			Ply[fd].ply->constitution--;
			Ply[fd].ply->strength--; 
			break;
		case GOBLIN:
			Ply[fd].ply->intelligence-=2;
			Ply[fd].ply->constitution++;
			Ply[fd].ply->strength++;
			break;
		case GNOME:
			Ply[fd].ply->piety++;
			Ply[fd].ply->strength--;
			break;
		case HALFELF: 
			Ply[fd].ply->intelligence++; 
			Ply[fd].ply->constitution--; 
			break;
		case HALFORC:
			Ply[fd].ply->constitution--;
			break;			
		case HOBBIT: 
			Ply[fd].ply->dexterity++; 
			Ply[fd].ply->strength--; 
			break;
		case HUMAN: 
			Ply[fd].ply->constitution++; 
			break;
		case OGRE:
			Ply[fd].ply->strength++;
			Ply[fd].ply->piety--;
			break;
		case ORC: 
			Ply[fd].ply->strength++; 
			Ply[fd].ply->constitution++;
			Ply[fd].ply->dexterity--; 
			Ply[fd].ply->intelligence--;
			break;
		case TROLL: 
			Ply[fd].ply->strength++; 
			Ply[fd].ply->intelligence--;
			break;
		case HALFGIANT: 
			Ply[fd].ply->strength+=2; 
			Ply[fd].ply->intelligence--; 
			Ply[fd].ply->piety--; 
			break;
		}

		print(fd, "\nChoose a password (up to 14 chars): ");
		RETURN(fd, create_ply, 10);
	case 10:
		if(strlen(str) > 14) {
			print(fd, "Too long.\nChoose a password: ");
			RETURN(fd, create_ply, 10);
		}
		if(strlen(str) < 3) {
			print(fd, "Too short.\nChoose a password: ");
			RETURN(fd, create_ply, 10);
		}
		strncpy(Ply[fd].ply->password, str, 14);
		strcpy(Ply[fd].ply->name, Ply[fd].extr->tempstr[0]);
		up_level(Ply[fd].ply);
		Ply[fd].ply->fd = fd;
		init_ply(Ply[fd].ply);
		save_ply(Ply[fd].ply->name, Ply[fd].ply);
		print(fd, "\n");
		F_SET(Ply[fd].ply, PNOAAT);
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

	/*
	this logn command will print out all the commands entered by players.
	It should be used in extreme cases when trying to isolate a players
	input which may be causing a crash.
	*/
	if(RECORD_ALL)
		logn("all_cmd","\n%s-%d (%d): %s\n",Ply[fd].ply->name,fd,Ply[fd].ply->rom_num,str);  

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
			#ifdef WIN32
				scwrite(fd, "Goodbye!\n\r\n\r", 11);
			#else
				write(fd, "Goodbye!\n\r\n\r", 11);
			#endif

			disconnect(fd);
			return;
		}
		else if(n == PROMPT) {
				ANSI(fd, MAGENTA);
				if(F_ISSET(Ply[fd].ply, PPROMP))
					sprintf(str, "(%d H %d M): ",
						Ply[fd].ply->hpcur, Ply[fd].ply->mpcur);
				else
					strcpy(str, ": ");
				#ifdef WIN32
					scwrite(fd, str, strlen(str));
				#else 
					write(fd, str, strlen(str));
				#endif /* WIN32 */
				ANSI(fd, WHITE);
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
		print(fd, "The command \"%s\" does not exist.\n", cmnd->str[0]);
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
