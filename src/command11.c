/*
 * COMMAND11.C:
 * 
 *   Copyright 1994, 1995 Brooke Paul
 *   Additional user routines
 * $Id: command11.c,v 6.14 2001/03/26 23:40:29 develop Exp $
 *
 * $Log: command11.c,v $
 * Revision 6.14  2001/03/26 23:40:29  develop
 * added spam check for emote command
 *
 * Revision 6.13  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 *
 */

#include "../include/mordb.h"
#include "mextern.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>

/******************************************************************/
/*				emote				  */
/******************************************************************/

/* This command allows a player to echo a message unaccompanied by */
/* any message format, except for the players name at the beginning */

int emote( creature *ply_ptr, cmd *cmnd )
{
	room		*rom_ptr;
	int		fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	/* check for spam emotes */
	if ( check_for_spam( ply_ptr ) )
        {
                return(0);
        }

	cmnd->fullstr[255] = 0;
	clean_str(cmnd->fullstr, 1);

	if(strlen(cmnd->fullstr) < 1) {
		output(fd, "Emote what?\n");
		return(0);
	}
	if(F_ISSET(ply_ptr, PSILNC)){
		output_wc(fd, "You are unable to do that right now.\n", SILENCECOLOR);
		return(0);
	}

	F_CLR(ply_ptr, PHIDDN);
	if(F_ISSET(ply_ptr, PLECHO)){
		sprintf(g_buffer, "You emote: %s\n", cmnd->fullstr);
		output_wc(fd, g_buffer, ECHOCOLOR);
	}
	else
		output(fd, "Ok.\n");

	sprintf(g_buffer, "%%M %s.", cmnd->fullstr);
	broadcast_rom(fd, rom_ptr->rom_num, g_buffer, 
			m1arg(ply_ptr));

	return(0);
}
/*==============================================================*/
/*                          passwd                              */
/*==============================================================*/
/* The passwd function callls the necessary function to allow *
 * a player to change their password. */

int passwd (creature *ply_ptr, cmd *cmnd )   
{
	int	fd;
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

void chpasswd(int fd, int param, char *str )
{
	creature	*ply_ptr;


	ply_ptr = Ply[fd].ply;
	output_ply_buf(fd);
	switch (param) {
	case 0:
			output(fd,"Current password: ");
	 		output_ply_buf(fd);
            Ply[fd].io->intrpt &= ~1; 
			RETURN(fd,chpasswd,1);
	break;
	case 1:
		if (!strcmp(ply_ptr->password,str)){
			sprintf(g_buffer, "%c%c%c\n\r", 255, 252, 1);
			output(fd, g_buffer);
			output(fd,"New password: ");
	 		output_ply_buf(fd);
            Ply[fd].io->intrpt &= ~1; 
			RETURN(fd,chpasswd,2);
		}
		else {
			sprintf(g_buffer, "%c%c%c\n\r", 255, 252, 1);
			output(fd, g_buffer);
			output(fd,"Incorrect password.\n");
			output(fd,"Aborting.\n");
            F_CLR(Ply[fd].ply, PREADI);
			RETURN(fd, command, 1);
		}
	case 2:
			if (strlen(str) <3){
				sprintf(g_buffer, "%c%c%c\n\r", 255, 252, 1);
				output(fd, g_buffer);
				output(fd,"Password too short.\n");
                output(fd,"Aborting.\n");
                F_CLR(Ply[fd].ply, PREADI);
				RETURN(fd, command, 1);
			}
			else if (strlen(str) > 14){
				sprintf(g_buffer, "%c%c%c\n\r", 255, 252, 1);
				output(fd, g_buffer);
				output(fd,"Password too long.\n");
				output(fd,"Aborting.\n");
            	F_CLR(Ply[fd].ply, PREADI);
				RETURN(fd, command, 1);
			}
			else{
				strcpy(Ply[fd].extr->tempstr[1], str);
				sprintf(g_buffer, "%c%c%c\n\r", 255, 252, 1);
				output(fd, g_buffer);
				output(fd,"Re-enter password: ");
	 			output_ply_buf(fd);
	            Ply[fd].io->intrpt &= ~1; 
				RETURN(fd,chpasswd,3);
			}
	break;
	case 3:
		if(!strcmp(Ply[fd].extr->tempstr[1],str)){
			strcpy(ply_ptr->password,str);
			sprintf(g_buffer, "%c%c%c\n\r", 255, 252, 1);
			output(fd, g_buffer);
			output(fd,"Password changed.\n");
           	F_CLR(Ply[fd].ply, PREADI);
			save_ply(ply_ptr);
			RETURN(fd, command, 1);
		}
		else{
			sprintf(g_buffer, "%c%c%c\n\r", 255, 252, 1);
			output(fd, g_buffer);
			output(fd,"Different passwords given.\n");
			output(fd,"Aborting.\n");
           	F_CLR(Ply[fd].ply, PREADI);
            RETURN(fd, command, 1);
		}
	break;

	}

}


/*==============================================================*/
int vote (creature *ply_ptr, cmd *cmnd )   
{
	int		fd;
	int 	i,n, number;
	int		match = 0;
	char	str[80], tmp[256];
	FILE	*fp;
	
    fd = ply_ptr->fd;
	str[0] = 0; 
	if ((18 + ply_ptr->lasttime[LT_HOURS].interval/86400L) < 21) {
		output(fd, "You're too young to vote in this election.\n");
		return(0);
	}
	if (!F_ISSET(ply_ptr->parent_rom,RELECT)){
		output(fd,"This is not an electon booth.\n");
		return(0);
	}

	sprintf(Ply[fd].extr->tempstr[0],"%s/ISSUE", get_post_path());
	fp = fopen(Ply[fd].extr->tempstr[0],"r");
	if(!fp){
		output(fd,"There are currently no issues for you to vote on.\n");
		return(0);
	}
	
	fgets(tmp, 256,fp);
	if (feof(fp)) {
		output(fd,"Currently there are no issues for you to vote on.\n");
		return(0);
	}
	fclose(fp);

	n =  sscanf(tmp,"%d %s",&number,str);

	lowercize(str,1);
	if (n <1){
		output(fd,"There are no issues for you to vote on at this time.\n");
		return(0);
	} else if (n == 2 && str){
		if(!strcmp(str,"Prince0") && F_ISSET(ply_ptr,PPLDGK)
			/*  && !F_ISSET(ply_ptr,PKNGDM) */   )
			match = 1;
		else if(!strcmp(str,"Prince1") && F_ISSET(ply_ptr,PPLDGK)
			/*  && F_ISSET(ply_ptr,PKNGDM)  */   )
			match = 1;
		else
			for (i=1;i<BUILDER; i++)
				if(!strcmp(str, get_class_string(i))){
					if(i == ply_ptr->class)
						match = 1;
					break;
				}

		if(!match){
			output(fd,"Sorry, you may not vote in this election.\n");
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
void vote_cmnd(int fd, int param, char *str )
{
    creature	*ply_ptr;
		int		i, n =0;
		char	tmp[1024], c;
		FILE	*fp;
	
        ply_ptr = Ply[fd].ply;

        output_ply_buf(fd);
        switch (param) {
        case 0:
			sprintf(tmp,"%s/vote/%s_v",get_player_path(), ply_ptr->name);
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
				output(fd,"\n");
				output(fd, tmp);
				output(fd,"How do you vote: ");
				output_ply_buf(fd);
	            Ply[fd].io->intrpt &= ~1; 

				Ply[fd].extr->tempstr[1][1] = 1;
				RETURN(fd,vote_cmnd,2);
			}
			else {
				fclose(fp);
				output(fd,"You have already voted.\n");
				output(fd,"Do you wish to change your vote (y/n): ");
				output_ply_buf(fd);
	            Ply[fd].io->intrpt &= ~1; 
				RETURN(fd,vote_cmnd,1);
			}
        break;
        case 1:
			if (str[0] == 'y' || str[0] == 'Y'){
				sprintf(tmp,"%s/vote/%s_v",get_player_path(),ply_ptr->name);
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
				output(fd,"\n");
				output(fd, tmp);
				output(fd,"How do you vote: ");
				output_ply_buf(fd);
	            Ply[fd].io->intrpt &= ~1; 

				Ply[fd].extr->tempstr[1][1] = 1;
				RETURN(fd,vote_cmnd,2);
			}
            output(fd,"Aborting.\n");
            F_CLR(Ply[fd].ply, PREADI);
            RETURN(fd, command, 1);

		break;
		case 2:
			c = low(str[0]);
			if (c != 'a' && c != 'b' && c != 'c' && c != 'd' && c != 'e' && c != 'f' && c != 'g'){
				output(fd,"Invalid selection. Aborting\n");
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
				output(fd, "\n");
				output(fd, tmp);
				output(fd,"How do you vote: ");
				output_ply_buf(fd);
	            Ply[fd].io->intrpt &= ~1; 

				Ply[fd].extr->tempstr[1][1] = n;
				fgets(tmp,1024,fp);
				if (feof(fp))
					Ply[fd].extr->tempstr[1][0] = Ply[fd].extr->tempstr[1][1];
				fclose(fp);
	
				RETURN(fd,vote_cmnd,2);
			}
		break;

		case 3:
			n = Ply[fd].extr->tempstr[1][0] ;
			sprintf(tmp,"%s/vote/%s_v", get_player_path(), ply_ptr->name);
			Ply[fd].extr->tempstr[2][n] = 0;
			i = open(tmp, O_CREAT | O_RDWR | O_BINARY, ACC);
			if(i < 0)
                merror("vote_cmnd", FATAL);
                                          
			write(i,Ply[fd].extr->tempstr[2],n);
			close(i);
			output(fd,"voted.\n");
			F_CLR(Ply[fd].ply, PREADI);
            RETURN(fd, command, 1);
		break;
	}
}

/*************************************************************************/
/*                              pfinger                                  */
/*************************************************************************/
int pfinger(creature *ply_ptr, cmd *cmnd )
{
    struct stat f_stat;
    creature    *player;
	char		tmp[80];
	int			fd;

	fd = ply_ptr->fd; 
    if(cmnd->num < 2) {
        output(fd, "Finger who?\n");
        return(0);
    }


	cmnd->str[1][0] = up(cmnd->str[1][0]);
	player = find_who(cmnd->str[1]);


	if (!player){

		sprintf(tmp,"%s/%s",get_player_path(), cmnd->str[1]);
	    if (stat(tmp,&f_stat))
		{
       		output(fd,"Player does not exist.\n");
       		return (0);
    	}

    	if(load_ply(cmnd->str[1], &player) < 0){
       		output(fd,"Player does not exist.\n");
       		return (0);
    	}             

		if ( player->class >= IMMORTAL && ply_ptr->class < player->class )
		{
			output(fd,"You are currently unable to finger that player.\n");
	    	free_crt(player);
			return (0);
		}

		sprintf(g_buffer,"%s %25s %15s\n",player->name, 
    		 get_race_string(player->race), title_ply(player));
		output(fd, g_buffer );

		if ( ply_ptr->class == DM )
		{
			sprintf(g_buffer,"Logged out from room %d\n", player->rom_num);
			output(fd, g_buffer );
		}

	   	sprintf(g_buffer, "last login: %s",ctime(&f_stat.st_ctime));
		output(fd, g_buffer );
    	free_crt(player);
	} 
	else
	{
		if ( F_ISSET(player, PDMINV) && ply_ptr->class < player->class )
		{
			output(fd,"You are currently unable to finger that player.\n");
			return (0);
		}

	    sprintf(g_buffer,"%s %25s %15s\n",player->name, 
   	    	 get_race_string(player->race), title_ply(player));
	    output(fd, g_buffer);
    	output(fd,"Currently logged on.\n");
	}

    sprintf(tmp,"%s/%s", get_post_path(), cmnd->str[1]);
    if (stat(tmp,&f_stat)){
       output(fd,"No mail.\n");
       return (0);
    }

	if (f_stat.st_atime > f_stat.st_ctime)
	{
		sprintf(g_buffer,"No unread mail since: %s",ctime(&f_stat.st_atime));
		output(fd, g_buffer);
	}
 	else
	{
		sprintf(g_buffer,"New mail since: %s",ctime(&f_stat.st_ctime));
		output(fd, g_buffer );
	}

	return(0);

}
