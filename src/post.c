/*
 * POST.C:
 *
 *	These routines are for the game post office.
 *
 *	Copyright (C) 1991, 1992, 1993 Brooke Paul
 *
 * $Id: post.c,v 6.14 2001/03/08 16:09:09 develop Exp $
 *
 * $Log: post.c,v $
 * Revision 6.14  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */
#include "../include/mordb.h"
#include "mextern.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



/************************************************************************/
/*				postsend				*/
/************************************************************************/

/* This function allows a player to send a letter to another player if	*/
/* he/she is in a post office.						*/

int postsend( creature *ply_ptr, cmd *cmnd )
{
	room	*rom_ptr;
	char	file[80];
	int	fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(!F_ISSET(rom_ptr, RPOSTO) && ply_ptr->class < CARETAKER) {
		output(fd, "This is not a post office.\n");
		return(0);
	}

	if(cmnd->num < 2) {
		output(fd, "Mail whom?\n");
		return(0);
	}

	cmnd->str[1][0] = up(cmnd->str[1][0]);
	sprintf(file, "%s/%s", get_player_path(), cmnd->str[1]);
	if ( !file_exists( file ) )
	{
		output(fd, "That player does not exist.\n");
		return(0);
	}

	output(fd, "Enter your message now. Use an empty line to finish.\nEach line should be NO LONGER THAN 80 CHARACTERS.\n-: ");

	sprintf(Ply[fd].extr->tempstr[0], "%s/%s", get_post_path(), cmnd->str[1]);
	sprintf(Ply[fd].extr->tempstr[1], "%s/%s.send", get_post_path(), ply_ptr->name);

	F_SET(Ply[fd].ply, PREADI);
	output_ply_buf(fd);
	Ply[fd].io->intrpt &= ~1; 
	Ply[fd].io->fn = postedit;
	Ply[fd].io->fnparam = 1;
	return(DOPROMPT);

}

/************************************************************************/
/*				postedit				*/
/************************************************************************/
 
/* This function is called when a player is editing a message to send	*/
/* to another player.  							*/

void postedit(int fd, int param, char *str )
{
	char	outstr[85];
	FILE *ff, *ff2;

	if(!str[0]) {

		if((ff = fopen(Ply[fd].extr->tempstr[1], "r")) == NULL) {
			output(fd, "Sorry, there was an error processing your mail. Aborted.");
			F_CLR(Ply[fd].ply, PREADI);
			RETURN(fd, command,1);
		}
		if((ff2 = fopen(Ply[fd].extr->tempstr[0], "a")) == NULL) {
			output(fd, "Sorry, there was an error processing your mail. Aborted.");
			sprintf(g_buffer, "Mail problem(2) with %s sending to %s\n",
				Ply[fd].ply->name, Ply[fd].extr->tempstr[0]);
			elog(g_buffer);
			F_CLR(Ply[fd].ply, PREADI);
			RETURN(fd, command,1);
		}
		while(fgets(g_buffer,80,ff)) {
			fprintf(ff2, "%s", g_buffer);
		}
		
		fclose(ff); fclose(ff2);
		unlink(Ply[fd].extr->tempstr[1]);
		
		F_CLR(Ply[fd].ply, PREADI);
		output(fd, "Mail sent.\n");
		RETURN(fd, command, 1);
	}


	if((ff = fopen(Ply[fd].extr->tempstr[1], "a")) == NULL)
		merror("postedit", FATAL);

	if(param == 1) {
		sprintf(outstr, "\n---\nMail from %s (%s):\n\n",
		    Ply[fd].ply->name, get_time_str());
		fprintf(ff, "%s", outstr);
	}

	strncpy(outstr, str, 79);
	outstr[79] = 0;
	strcat(outstr, "\n");

	fprintf(ff, "%s", outstr);

	fclose(ff);

	output(fd, "-: ");
	output_ply_buf(fd);
	Ply[fd].io->intrpt &= ~1; 

	RETURN(fd, postedit, 2);
}

/************************************************************************/
/*				postread				*/
/************************************************************************/

/* This function allows a player to read his or her mail.		*/

int postread(creature *ply_ptr, cmd	*cmnd )
{
	room	*rom_ptr;
	char 	file[80];
	int	fd;
	
	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(!F_ISSET(rom_ptr, RPOSTO) && ply_ptr->class < CARETAKER) {
		output(fd, "This is not a post office.\n");
		return(0);
	}

	strcpy(file, get_post_path());

	strcat(file, "/");
	strcat(file, ply_ptr->name);
	view_file(fd, 1, file);
	return(DOPROMPT);

}

/************************************************************************/
/*				postdelete				*/
/************************************************************************/

/* This function allows a player to delete his or her mail.		*/

int postdelete( creature *ply_ptr, cmd *cmnd )
{
	room	*rom_ptr;
	char	file[80];
	int	fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(!F_ISSET(rom_ptr, RPOSTO) && ply_ptr->class < CARETAKER) {
		output(fd, "This is not a post office.\n");
		return(0);
	}

	sprintf(file, "%s/%s", get_post_path(), ply_ptr->name);

	output(fd, "Mail deleted.\n");

	unlink(file);

	return(0);
}
/************************************************************************/
/*                              notepad                                 */
/************************************************************************/
int notepad( creature *ply_ptr, cmd *cmnd )
{
    room    *rom_ptr;
    char    file[80];
    int		fd;
 
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;
 

    if (ply_ptr->class < BUILDER)
        return(PROMPT);
 
    sprintf(file, "%s/%s_pad", get_post_path(), ply_ptr->name);
 
    if (cmnd->num == 2) {
        if( low(cmnd->str[1][0]) == 'a'){
            strcpy(Ply[fd].extr->tempstr[0],file);
            output(fd, "DM notepad:\n->");
            F_SET(Ply[fd].ply, PREADI);
            output_ply_buf(fd);
	        Ply[fd].io->intrpt &= ~1; 
            Ply[fd].io->fn = noteedit;
            Ply[fd].io->fnparam = 1;
            return(DOPROMPT);
        }
        else if (low(cmnd->str[1][0]) == 'd'){
            unlink(file);
            output(fd,"Clearing your notepad.\n");
            return(PROMPT);
        }
        else{
            output(fd,"invalid option.\n");
            return(PROMPT);
        }
    }
    else{
        view_file(fd, 1, file);
        return(DOPROMPT);
    }
}
 
/************************************************************************/
/*                              noteedit                                */
/************************************************************************/
 
void noteedit(int fd, int param, char *str )
{
    char    outstr[85], tmpstr[40];
    int ff;
 
    if(str[0] == '.') {
        F_CLR(Ply[fd].ply, PREADI);
        output(fd, "Message appended.\n");
        RETURN(fd, command, 1); 
    }
 
    ff = open(Ply[fd].extr->tempstr[0], O_RDONLY | O_BINARY);
    if(ff < 0){
        ff = open(Ply[fd].extr->tempstr[0], O_CREAT | O_RDWR | O_BINARY, ACC);
	sprintf(tmpstr, "\n\n");
        write(ff, tmpstr,strlen(tmpstr));
    }
    close(ff);
 
    ff = open(Ply[fd].extr->tempstr[0], O_CREAT | O_APPEND | O_RDWR | O_BINARY, ACC);
    if(ff < 0)
        merror("noteedit", FATAL);
 
    strncpy(outstr, str, 79);
    outstr[79] = 0;
    strcat(outstr, "\n");
 
    write(ff, outstr, strlen(outstr));
 
    close(ff);
 
    output(fd, "->");
    output_ply_buf(fd);
    Ply[fd].io->intrpt &= ~1; 

    RETURN(fd, noteedit, 2);
}

/************************************************************************/
/*                              welcome_edit                             */
/************************************************************************/
int welcome_edit(creature *ply_ptr, cmd *cmnd)
{
    room    *rom_ptr;
    char    file[80];
    int         fd;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;


    if (ply_ptr->class < DM)
        return(PROMPT);

    if (cmnd->num < 2) {
		output(fd, "Usage: *welcome <dm|ct|builder|player> [a|d]\n");
		return(PROMPT);
    }


    switch(low(cmnd->str[1][0])) {
	case 'd':
		sprintf(file, "%s/welcome.dm", get_log_path() );
		break;
	case 'c':
		sprintf(file, "%s/welcome.ct", get_log_path() );
		break;
	case 'b':
		sprintf(file, "%s/welcome.build", get_log_path() );
		break;
	case 'p':
		sprintf(file, "%s/welcome", get_log_path() );
		break;
	default:
		output(fd, "Usage: *welcome <dm|ct|builder|player> [a|d]\n");
		return(PROMPT);
		break;
    }	


    if(cmnd->num == 3) {	
    	if (low(cmnd->str[2][0]) == 'd'){
            unlink(file);
            output(fd,"Clearing the welcome file.\n");
            return(PROMPT);
    	}
    	else if( low(cmnd->str[2][0]) == 'a'){
            strcpy(Ply[fd].extr->tempstr[0],file);
            output(fd, "\n->");
            F_SET(Ply[fd].ply, PREADI);
            output_ply_buf(fd);
                Ply[fd].io->intrpt &= ~1;
            Ply[fd].io->fn = noteedit;
            Ply[fd].io->fnparam = 1;
            return(DOPROMPT);
    	}
    	else {
            output(fd,"invalid option.\n");
            return(PROMPT);
        }
    
    }
    else {
	view_file(fd, 1, file);
        return(PROMPT); 
    }
}

