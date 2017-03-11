/*
 * COMMAND11.C:
 * 
 *
 *   Additional user routines
 *
 */

#include "mstruct.h"
#include "mextern.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#ifdef DMALLOC
  #include "/usr/local/include/dmalloc.h"
#endif

/******************************************************************/
/*				emote				  */
/******************************************************************/

/* This command allows a player to echo a message unaccompanied by */
/* any message format, except for the players name at the beginning */

int emote(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	room		*rom_ptr;
	int		index = -1, j, i, fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	for(i=0; i<strlen(cmnd->fullstr) && i < 256; i++) {
		if(cmnd->fullstr[i] == ' ') {
			index = i + 1;
			break;
		}
	}
	cmnd->fullstr[255] = 0;
	/* Check for modem escape code */
        for(j=0; j<strlen(cmnd->fullstr) && j < 256; j++) {
                if(cmnd->fullstr[j] == '+' && cmnd->fullstr[j+1] == '+'){
                        index = -1;
                        break;
        	}
	}

	if(index == -1 || strlen(&cmnd->fullstr[index]) < 1) {
		print(fd, "Emote what?\n");
		return(0);
	}
	if(F_ISSET(ply_ptr, PSILNC)){
		print(fd, "You are unable to do that right now.\n");
		return(0);
	}
	F_CLR(ply_ptr, PHIDDN);
	if(F_ISSET(ply_ptr, PLECHO)){
		ANSI(fd, CYAN);
		print(fd, "You emote: %s\n", &cmnd->fullstr[index]);
		ANSI(fd, NORMAL);
	}
	else
		print(fd, "Ok.\n");

	broadcast_rom(fd, rom_ptr->rom_num, "%M %s.", 
			ply_ptr, &cmnd->fullstr[index]);

	return(0);
}
/*==============================================================*/
/*                          passwd                              */
/*==============================================================*/
/* The passwd function callls the necessary function to allow *
 * a player to change their password. */

int passwd (ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;   
{
int	fd;
extern void chpasswd();
	fd = ply_ptr->fd;

		/* do not flash output until player hits return */
        F_SET(Ply[fd].ply, PREADI); 
		chpasswd(fd,0,"");
        return(DOPROMPT);
                         
}
/*==============================================================*/
/*                       chpasswd                               */
/*==============================================================*/
/* The chpasswd command handles  the procedure involved in       *
 * changing a player's password.  A player first must enter the  *
 * correct current password, then the new password, and re enter *
 * the new password to comfirm it. If the player enters the      *
 * wrong password  or an invalid password (too short or long),   *
 * the password will not be changed and the procedure is aborted. */

void chpasswd(fd,param,str)
int	fd;
int	param;
char *str;
{
	creature	*ply_ptr;


	ply_ptr = Ply[fd].ply;
	output_buf();
	switch (param) {
	case 0:
			print(fd,"Current password: ");
	 		output_buf();
            Ply[fd].io->intrpt &= ~1; 
			RETURN(fd,chpasswd,1);
	break;
	case 1:
		if (!strcmp(ply_ptr->password,str)){
			print(fd, "%c%c%c\n\r", 255, 252, 1);
			print(fd,"New password: ");
	 		output_buf();
            Ply[fd].io->intrpt &= ~1; 
			RETURN(fd,chpasswd,2);
		}
		else {
			print(fd, "%c%c%c\n\r", 255, 252, 1);
			print(fd,"Incorrect password.\n");
			print(fd,"Aborting.\n");
            F_CLR(Ply[fd].ply, PREADI);
			RETURN(fd, command, 1);
		}
	case 2:
			if (strlen(str) <3){
			print(fd, "%c%c%c\n\r", 255, 252, 1);
				print(fd,"Password too short.\n");
                print(fd,"Aborting.\n");
                F_CLR(Ply[fd].ply, PREADI);
				RETURN(fd, command, 1);
			}
			else if (strlen(str) > 14){
			print(fd, "%c%c%c\n\r", 255, 252, 1);
				print(fd,"Password too long.\n");
				print(fd,"Aborting.\n");
            	F_CLR(Ply[fd].ply, PREADI);
				RETURN(fd, command, 1);
			}
			else{
				strcpy(Ply[fd].extr->tempstr[1], str);
			print(fd, "%c%c%c\n\r", 255, 252, 1);
				print(fd,"Re-enter password: ");
	 			output_buf();
            	Ply[fd].io->intrpt &= ~1; 
				RETURN(fd,chpasswd,3);
			}
	break;
	case 3:
		if(!strcmp(Ply[fd].extr->tempstr[1],str)){
			strcpy(ply_ptr->password,str);
			print(fd, "%c%c%c\n\r", 255, 252, 1);
			print(fd,"Password changed.\n");
           	F_CLR(Ply[fd].ply, PREADI);
			save_ply(ply_ptr->name,ply_ptr);
			RETURN(fd, command, 1);
		}
		else{
			print(fd, "%c%c%c\n\r", 255, 252, 1);
			print(fd,"Different passwords given.\n");
			print(fd,"Aborting.\n");
           	F_CLR(Ply[fd].ply, PREADI);
            RETURN(fd, command, 1);
		}
	break;

	}

}


/*==============================================================*/
int vote (ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;   
{
	int		fd;
	int 	i,n, number;
	int		match = 0;
	char	str[80], tmp[256];
	FILE	*fp;
extern void vote_cmnd();
	
    fd = ply_ptr->fd;
	str[0] = 0; 
	if ((18 + ply_ptr->lasttime[LT_HOURS].interval/86400L) < 21) {
		print(fd, "You're too young to vote in this election.\n");
		return(0);
	}
	if (!F_ISSET(ply_ptr->parent_rom,RELECT)){
		print(fd,"This is not an electon booth.\n");
		return(0);
	}

	sprintf(Ply[fd].extr->tempstr[0],"%s/ISSUE",POSTPATH);
	fp = fopen(Ply[fd].extr->tempstr[0],"r");
	if(!fp){
		print(fd,"There are currently no issues for you to vote on.\n");
		return(0);
	}
	
	fgets(tmp, 256,fp);
	if (feof(fp)) {
		print(fd,"Currently there are no issues for you to vote on.\n");
		return(0);
	}
	fclose(fp);

	n =  sscanf(tmp,"%d %s",&number,str);

	lowercize(str,1);
	if (n <1){
		print(fd,"There are no issues for you to vote on at this time.\n");
		return(0);
	} else if (n == 2 && str){
		if(!strcmp(str,"Prince0") && F_ISSET(ply_ptr,PPLDGK)
			&& !F_ISSET(ply_ptr,PKNGDM))
			match = 1;
		else if(!strcmp(str,"Prince1") && F_ISSET(ply_ptr,PPLDGK)
			&& F_ISSET(ply_ptr,PKNGDM))
			match = 1;
		else
			for (i=1;i<CARETAKER; i++)
				if(!strcmp(str,class_str[i])){
					if(i == ply_ptr->class)
						match = 1;
					break;
				}

		if(!match){
			print(fd,"Sorry, you may not vote in this election.\n");
			return(0);
		}
	}

	Ply[fd].extr->tempstr[1][0] = MIN(79,number);
	Ply[fd].extr->tempstr[1][1] = 0;
	
	
 	F_SET(Ply[fd].ply, PREADI); 
    vote_cmnd(fd,0,"");
    return(DOPROMPT);
                         
}
/*==============================================================*/
void vote_cmnd(fd,param,str)
int     fd;
int     param;
char *str;
{
    creature	*ply_ptr;
		int		i, n =0;
		char	tmp[1024], c;
		FILE	*fp;
	
        ply_ptr = Ply[fd].ply;

        output_buf();
        switch (param) {
        case 0:
			sprintf(tmp,"%s/vote/%s_v",PLAYERPATH,ply_ptr->name);
			fp = fopen(tmp,"r");
			if (!fp){
				fp = fopen(Ply[fd].extr->tempstr[0],"r");
				if(!fp) {
					F_CLR(Ply[fd].ply, PREADI);
					RETURN(fd, command, 1);
				}

				fgets(tmp,1024,fp);
				fgets(tmp,1024,fp);
				tmp[1023] = 0;
				fclose(fp);
				print(fd,"\n%s",tmp);
				print(fd,"How do you vote: ");
				output_buf();

				Ply[fd].extr->tempstr[1][1] = 1;
            	Ply[fd].io->intrpt &= ~1; 
				RETURN(fd,vote_cmnd,2);
			}
			else {
				fclose(fp);
				print(fd,"You have already voted.\n");
				print(fd,"Do you wish to change your vote (y/n): ");
				output_buf();
            	Ply[fd].io->intrpt &= ~1; 
				RETURN(fd,vote_cmnd,1);
			}
        break;
        case 1:
			if (str[0] == 'y' || str[0] == 'Y'){
				sprintf(tmp,"%s/vote/%s_v",PLAYERPATH,ply_ptr->name);
				unlink(tmp);

				fp = fopen(Ply[fd].extr->tempstr[0],"r");
				if(!fp) {
					F_CLR(Ply[fd].ply, PREADI);
					RETURN(fd, command, 1);
				}

				fgets(tmp,1024,fp);
				fgets(tmp,1024,fp);
				tmp[1023] = 0;
				fclose(fp);
				print(fd,"\n%s",tmp);
				print(fd,"How do you vote: ");
				output_buf();

				Ply[fd].extr->tempstr[1][1] = 1;
            	Ply[fd].io->intrpt &= ~1; 
				RETURN(fd,vote_cmnd,2);
			}
            print(fd,"Aborting.\n");
            F_CLR(Ply[fd].ply, PREADI);
            RETURN(fd, command, 1);

		break;
		case 2:
			c = low(str[0]);
			if (c != 'a' && c != 'b' && c != 'c' && c != 'd' && c != 'e' && c != 'f' && c != 'g'){
				print(fd,"Invalid selection. Aborting\n");
				F_CLR(Ply[fd].ply, PREADI);
				RETURN(fd, command, 1);
			}
			n = MAX(0,Ply[fd].extr->tempstr[1][1] -1);
			Ply[fd].extr->tempstr[2][n] = up(c);
			if (Ply[fd].extr->tempstr[1][1] >= Ply[fd].extr->tempstr[1][0]){
				vote_cmnd(fd,3,"");
				RETURN(fd,command,1);
			}
			else{
				fp = fopen(Ply[fd].extr->tempstr[0],"r");
				if(!fp) {
					F_CLR(Ply[fd].ply, PREADI);
					RETURN(fd, command, 1);
				}

				fgets(tmp,1024,fp);
				if (feof(fp)) {
					F_CLR(Ply[fd].ply, PREADI);
					RETURN(fd, command, 1);
				}
	
				n = 0;
				while(!feof(fp)){
					fgets(tmp,1024,fp);
					tmp[1023] = 0;
					n++;
					if ( n  == Ply[fd].extr->tempstr[1][1]+1)
						break;
				}
				print(fd,"\n%s",tmp);
				print(fd,"How do you vote: ");
				output_buf();

				Ply[fd].extr->tempstr[1][1] = n;
				fgets(tmp,1024,fp);
				if (feof(fp))
					Ply[fd].extr->tempstr[1][0] = Ply[fd].extr->tempstr[1][1];
				fclose(fp);
	
           		Ply[fd].io->intrpt &= ~1; 
				RETURN(fd,vote_cmnd,2);
			}
		break;

		case 3:
			n = Ply[fd].extr->tempstr[1][0] ;
			sprintf(tmp,"%s/vote/%s_v",PLAYERPATH,ply_ptr->name);
			Ply[fd].extr->tempstr[2][n] = 0;
			i = open(tmp, O_CREAT | O_RDWR, ACC);
			if(i < 0)
                merror("vote_cmnd", FATAL);
                                          
			write(i,Ply[fd].extr->tempstr[2],n);
			close(i);
			print(fd,"voted.\n");
			F_CLR(Ply[fd].ply, PREADI);
            RETURN(fd, command, 1);
		break;
	}
}

/*************************************************************************/
/*                              pfinger                                  */
/*************************************************************************/
int pfinger(ply_ptr, cmnd) 
creature	*ply_ptr;
cmd		*cmnd;
{

    struct stat f_stat;
    creature    *player;
	char		tmp[80];
	int			fd;

	fd = ply_ptr->fd; 
    if(cmnd->num < 2) {
        print(fd, "Finger who?\n");
        return(0);
    }
	cmnd->str[1][0] = up(cmnd->str[1][0]);
	player = find_who(cmnd->str[1]);

	if (!player){

    	if(load_ply(cmnd->str[1], &player) < 0){
       		print(fd,"Player does not exist.\n");
       		return (0);
    	}             

	  if (ply_ptr->class < CARETAKER && (player->class == CARETAKER ||
          player->class == DM)) {
			print(fd,"You are currently unable to finger that player.\n");
			return (0);
		}

		sprintf(tmp,"%s/%s",PLAYERPATH,cmnd->str[1]);
	    if (stat(tmp,&f_stat)){
       		print(fd,"Player does not exist.\n");
       		return (0);
    	}
 
	    print(fd,"%s %+25s %+15s\n",player->name, 
   	    	 race_str[player->race], title_ply(player));
    	print(fd,"last login: %s",ctime(&f_stat.st_ctime));
    	free_crt(player);
	} 
	else{
		if (F_ISSET(player, PDMINV) && 
		  (ply_ptr->class < CARETAKER || (ply_ptr->class == CARETAKER &&
          player->class == DM))) {
			print(fd,"You are currently unable to finger that player.\n");
			return (0);
		}

	    print(fd,"%s %+25s %+15s\n",player->name, 
   	    	 race_str[player->race], title_ply(player));
    	print(fd,"Currently logged on.\n");
	}

    sprintf(tmp,"%s/%s",POSTPATH,cmnd->str[1]);
    if (stat(tmp,&f_stat)){
       print(fd,"No mail.\n");
       return (0);
    }

	if (f_stat.st_atime > f_stat.st_ctime)
		print(fd,"No unread mail since: %s",ctime(&f_stat.st_atime));
 	else
		print(fd,"New mail since: %s",ctime(&f_stat.st_ctime));

}
