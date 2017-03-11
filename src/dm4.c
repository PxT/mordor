/*
 * DM4.C:
 *
 *  DM functions
 *
 *  Copyright (C) 1991, 1992, 1993 Brooke Paul
 *
 * $Id: dm4.c,v 6.15 2001/07/21 23:29:41 develop Exp $
 *
 * $Log: dm4.c,v $
 * Revision 6.15  2001/07/21 23:29:41  develop
 * *** empty log message ***
 *
 * Revision 6.14  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

#include "../include/mordb.h"
#include "mextern.h"

#include <ctype.h>

extern long last_exit_update;
extern long TX_interval;
static int	dm_gspells(); 

/************************************************************************/
/*              dm_param                */
/************************************************************************/

int dm_param(creature *ply_ptr, cmd *cmnd )
{
    int     fd;
 	time_t	t;
	char szBuffer[256];
	char str[256];

    fd = ply_ptr->fd;

    if(ply_ptr->class < BUILDER)
        return(PROMPT);

    if(cmnd->num < 2) {
        output(fd, "Set what parameter?\n");
        return(0);
    }

    t = time(0);

    switch(low(cmnd->str[1][0])) {
    case 'r': 
		Random_update_interval = (short) cmnd->val[1]; 
		return(PROMPT);
    case 'd': 
        sprintf(g_buffer,"\nRandom Update: %d\n",Random_update_interval);
        output(fd, g_buffer);

		if(AUTOSHUTDOWN) {
			convert_seconds_to_str((Shutdown.ltime + Shutdown.interval) -t, str );
			sprintf(g_buffer, "Time to next shutdown: %s\n", str);
			output(fd, g_buffer);
		}

		convert_seconds_to_str(t-StartTime, str );
		sprintf(szBuffer, "Uptime: %s\n", str); 
		output( fd, szBuffer );

		convert_seconds_to_str(TX_interval, str );
        sprintf(g_buffer,"Ship sailing interval: %s\n", str);
        output(fd, g_buffer);

		convert_seconds_to_str(last_exit_update + TX_interval - t, str );
        sprintf(g_buffer, "Time to Sail: %s\n", str);
        output(fd, g_buffer);
        return (PROMPT);
    case 's':
		if( cmnd->val[1] == 1)
			last_exit_update = t - TX_interval;
		else
        	TX_interval = cmnd->val[1];
        return(PROMPT);
    default: 
		output(fd, "Invalid parameter.\n");
        return(0);
    }

}


 
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                          dm_silence                                */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* DM_silence allows the dm to remove a specified player's dialy      *
 * bbroadcasts. If only the player name is given then the player's     *
 * is set to 0, if more that 2 arguemwnts are given, then the persons *
 * broadcast is set to the given number. */
 
int dm_silence(creature *ply_ptr, cmd *cmnd)
{
    creature    *crt_ptr;
    int			fd;
 
    fd = ply_ptr->fd;
 
    if(ply_ptr->class < CARETAKER )
        return(PROMPT);
 
    if(cmnd->num < 2) {
        output(fd, "syntax: *silence <player> [c/m] [value]\n");
        return(0);
    }
 
 
    lowercize(cmnd->str[1], 1);
    crt_ptr = find_who(cmnd->str[1]);
 
    if(!crt_ptr || F_ISSET(crt_ptr, PDMINV) ){
        output(fd, "That player is not logged on.\n");
        return(0);
    }
 
 
    if(cmnd->num < 3) {
        crt_ptr->daily[DL_BROAD].cur = 0;
        crt_ptr->daily[DL_BROAD].ltime = time(0);
        mprint(fd,"%M is silenced.\n", m1arg(crt_ptr));
    }
    else if (low(cmnd->str[2][0]) == 'c')
    {
        sprintf(g_buffer, "%%M has %d of %d broadcasts left.\n",
	        crt_ptr->daily[DL_BROAD].cur, crt_ptr->daily[DL_BROAD].max);
		mprint(fd, g_buffer, m1arg(crt_ptr));
    }
    else
    {
        crt_ptr->daily[DL_BROAD].ltime = time(0);
        crt_ptr->daily[DL_BROAD].cur = (char) cmnd->val[2];

        sprintf(g_buffer,"%%M is given %d broadcasts.\n",
			crt_ptr->daily[DL_BROAD].cur);
        mprint(fd, g_buffer, m1arg(crt_ptr));
    }
     
    return(0);
}
/**********************************************************************/
/*                           dm_broadecho                             */
/**********************************************************************/
 
/* dm_broadecho allows a DM to broadcast a message to                 *
 * the players in the game free of any message format. i.e. the msg   *
 * broadcasted appears exactly as it is typed */

int dm_broadecho(creature *ply_ptr, cmd *cmnd )
{
        int     i, found=0, fd;
    int len;
 
        fd = ply_ptr->fd;

	if(ply_ptr->class < DM)
		return(PROMPT);
 
    len = strlen(cmnd->fullstr);
        for(i=0; i<len && i < 256; i++) {
                if(cmnd->fullstr[i] == ' ' && cmnd->fullstr[i+1] != ' ')
                        found++;
                if(found==1) break;
        }
        cmnd->fullstr[255] = 0;
    
    len = strlen(&cmnd->fullstr[i+1]); 
    if(found < 1 || len < 1) {
                output(fd, "echo what?\n");
                return(0);
        }
    if (cmnd->fullstr[i+1] == '-')
        switch(cmnd->fullstr[i+2]){
           case 'n':
            if(cmnd->fullstr[i+3] != 0 && cmnd->fullstr[i+4] != 0)
             broadcast(&cmnd->fullstr[i+4]);
             break;
        }
    else
	{
        sprintf(g_buffer, "### %s", &cmnd->fullstr[i+1]);
        broadcast( g_buffer );
	}

    return(0);
 
}



/***********************************************************************/
/*                              dm_cast                                */
/***********************************************************************/

int dm_cast(creature *ply_ptr, cmd *cmnd )
{          
	char	match=0,rcast = 0, *sp;
    int     splno =0,c = 0, fd, i;
	ctag	*cp;

	fd = ply_ptr->fd;
 
	 
    if(ply_ptr->class < CARETAKER )
		return(PROMPT);
 
    if(cmnd->num < 2) {
        output(fd, "Globally cast what?\n");
		return(PROMPT);
    }
         
	if (cmnd->num >2 )	{
		if (!strcmp(cmnd->str[1],"-r"))
			rcast = 1;
		else {
			output(fd,"Invalid cast flag.\n");
			return(PROMPT);
		}
		sp = cmnd->str[2];
	}
	else if (cmnd->num == 2)
		sp = cmnd->str[1];

	do {
		if(!strcmp(sp, get_spell_name(c))) {
            match = 1;
            splno = c;
            break;
        }
        else if(!strncmp(sp, get_spell_name(c), 
            strlen(sp))) {
            match++;
            splno = c;
        }
        c++;
    } while(get_spell_num(c) != -1);
 
    if(match == 0) {
        output(fd, "That spell does not exist.\n");
        return(0);
    }    
    else if(match > 1) {
        output(fd, "Spell name is not unique.\n");
        return(0);
    }


	if(rcast){

		cp = ply_ptr->parent_rom->first_ply;

		if (splno == SRECAL){
			ctag		*cp_tmp;
			creature *pp;
			room        *new_rom;

            if(load_rom(1, &new_rom) < 0) {
                output(fd, "Spell failure.\n");
                return(0);
            }

			sprintf(g_buffer,"You cast %s on everyone in the room.\n",
				get_spell_name(splno));
			output(fd, g_buffer);

			sprintf(g_buffer, "%%M casts %s on everyone in the room.\n",
				get_spell_name(splno));
			broadcast_rom(fd, ply_ptr->rom_num,
				g_buffer, m1arg(ply_ptr));

			while(cp){
				sprintf(g_buffer, "%%M casts %s on you.\n",
					get_spell_name(splno));
				mprint(cp->crt->fd, g_buffer, m1arg(ply_ptr));

				cp_tmp=cp->next_tag;
				pp = cp->crt;
				del_ply_rom(cp->crt, cp->crt->parent_rom);
				add_ply_rom(pp, new_rom);
				cp = cp_tmp;
			}
			return(0);
		}						
		while(cp){

 
			if (F_ISSET(cp->crt,PDMINV)){
				cp = cp->next_tag;
				continue;
			}
			if ((c = dm_gspells(cp->crt,splno))){
				output(fd,"Sorry, you can not room cast that spell.\n");
				break;
			}

			sprintf(g_buffer,"%%M casts %s on you.\n",
				get_spell_name(splno));
			mprint(cp->crt->fd,g_buffer, m1arg(ply_ptr));
			cp = cp->next_tag;
		}

		if (!c){
			sprintf(g_buffer, "You cast %s on everyone in the room.\n",
				get_spell_name(splno));
			output(fd, g_buffer);

			sprintf(g_buffer, "%%M casts %s on everyone in the room.\n",
				get_spell_name(splno));
			broadcast_rom(fd, ply_ptr->rom_num,
				g_buffer, m1arg(ply_ptr));
		}

	}
	else{
		for(i=0; i<Tablesize; i++) {
        	if(!Ply[i].ply) 
				continue;
        	if(Ply[i].ply->fd == -1) 
				continue;
        	if(Ply[i].ply->fd == fd) 
				continue;
			if(F_ISSET(Ply[i].ply, PDMINV)) 
				continue;
			if ((c=dm_gspells(Ply[i].ply,splno))){
				output(fd,"Sorry, you can not globally cast that spell.\n");
				break;
			}

			sprintf(g_buffer,"%%M casts %s on you.\n",
				get_spell_name(splno));
			mprint(Ply[i].ply->fd, g_buffer, m1arg(ply_ptr));

			}
		if (!c){
			sprintf(g_buffer, "You cast %s on everyone.\n", 
				get_spell_name(splno));
			output(fd, g_buffer);

			strcpy(g_buffer, "%M casts ");
			strcat(g_buffer, get_spell_name(splno));
			strcat(g_buffer, " on everyone.");

			broadcast_c(g_buffer, ply_ptr );
		}

	}

	return(0);
}

/*========================================================================*/
/*			dm_gspells					  */
/*========================================================================*/
static int	dm_gspells(creature	*ply_ptr, int splno )
{
	time_t	t;

	t = time(0);
			switch(splno){
				case SVIGOR:
					ply_ptr->hpcur += mrand(1,6) + 4 + 2;
					ply_ptr->hpcur = MIN(ply_ptr->hpcur,ply_ptr->hpmax);
				break;
				case SMENDW:
					ply_ptr->hpcur += mrand(2,10) + 4 + 4;
					ply_ptr->hpcur = MIN(ply_ptr->hpcur,ply_ptr->hpmax);
				break;
				case SRESTO:
					ply_ptr->hpcur = ply_ptr->hpmax;
					ply_ptr->mpcur = ply_ptr->mpmax;
				break;
				case SFHEAL:
					ply_ptr->hpcur = ply_ptr->hpmax;
				break;
				case SBLESS:
					ply_ptr->lasttime[LT_BLESS].interval = 3600;
					ply_ptr->lasttime[LT_BLESS].ltime = t;
					F_SET(ply_ptr,PBLESS);
				break;
				case SPROTE:
					ply_ptr->lasttime[LT_PROTE].interval = 3600;
					ply_ptr->lasttime[LT_PROTE].ltime = t;
					F_SET(ply_ptr,PPROTE);
				break;
				case SINVIS:
					ply_ptr->lasttime[LT_INVIS].interval = 3600;
					ply_ptr->lasttime[LT_INVIS].ltime = t;
					F_SET(ply_ptr,PINVIS);
				break;
				case SDMAGI:
					ply_ptr->lasttime[LT_DMAGI].interval = 3600;
					ply_ptr->lasttime[LT_DMAGI].ltime = t;
					F_SET(ply_ptr,PDMAGI);
				break;
				case SRFIRE:
					ply_ptr->lasttime[LT_RFIRE].interval = 3600;
					ply_ptr->lasttime[LT_RFIRE].ltime = t;
					F_SET(ply_ptr,PRFIRE);
				break;
				case SRMAGI:
					ply_ptr->lasttime[LT_RMAGI].interval = 3600;
					ply_ptr->lasttime[LT_RMAGI].ltime = t;
					F_SET(ply_ptr,PRMAGI);
				break;
				case SDINVI:
					ply_ptr->lasttime[LT_DINVI].interval = 3600;
					ply_ptr->lasttime[LT_DINVI].ltime = t;
					F_SET(ply_ptr,PDINVI);
				break;
				case SFLYSP:
					ply_ptr->lasttime[LT_FLYSP].interval = 3600;
					ply_ptr->lasttime[LT_FLYSP].ltime = t;
					F_SET(ply_ptr,PFLYSP);
				break;
				case SLIGHT:
					ply_ptr->lasttime[LT_LIGHT].interval = 3600;
					ply_ptr->lasttime[LT_LIGHT].ltime = t;
					F_SET(ply_ptr,PLIGHT);
				break;
				case SLEVIT:
					ply_ptr->lasttime[LT_LEVIT].interval = 3600;
					ply_ptr->lasttime[LT_LEVIT].ltime = t;
					F_SET(ply_ptr,PLEVIT);
				break;
				case SKNOWA:
					ply_ptr->lasttime[LT_KNOWA].interval = 3600;
					ply_ptr->lasttime[LT_KNOWA].ltime = t;
					F_SET(ply_ptr,PKNOWA);
				break;
			        case SCUREP:
					F_CLR(ply_ptr,PPOISN);
				break;
				case SRMDIS:
					F_CLR(ply_ptr,PDISEA);
				break;
				default:
				    return(1); 
				break;
				}
	return(0);
}


/**********************************************************************/
/*				dm_group				      */
/**********************************************************************/

/* This function allows you to see who is in a group or party of people */
/* who are following you.						*/

int dm_group(creature *ply_ptr, cmd *cmnd )
{
	ctag	*cp;
	room	*rom_ptr;
	creature	*grp_ptr;
	char	str[2048];
	int	fd;

	str[0] = 0;
	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;
	if (ply_ptr->class < CARETAKER )
		return(PROMPT);

	if (cmnd->num < 2){
		output(fd,"show who's group?\n");
		return(PROMPT);
	}
 
	grp_ptr =find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[1],cmnd->val[1]); 

	if(!grp_ptr) {
	    lowercize(cmnd->str[1], 1);

    grp_ptr = find_who(cmnd->str[1]);  

  	}	

	if(!grp_ptr) {
        output(fd, "That player is not logged on.\n");
        return(PROMPT);
    	}             

	if ( grp_ptr->following != NULL )
	{
		mprint(fd,"%M is following: %m\n", 
			m2args(grp_ptr, grp_ptr->following));
	}
	else
	{
		mprint(fd,"%M is following: no one\n", m1arg(grp_ptr));
	}
		

	cp = grp_ptr->first_fol;
	mprint(fd,"%M group: ", m1arg(grp_ptr));
	if(!cp) {
		output(fd, "None.\n");
		return(0);
	}

	while(cp) {
			strcat(str, cp->crt->name);
			strcat(str, ", ");
		cp = cp->next_tag;
	}


	str[strlen(str)-2] = 0;
	sprintf(g_buffer, "%s.\n", str);
	output(fd, g_buffer);

	return(0);

}
/**************************************/
/**************************************/
int nirvana(creature *ply_ptr, cmd *cmnd )
{
	if (ply_ptr->class < BUILDER)
		return(PROMPT);

	if (!strcmp("Tesseract",ply_ptr->name))
		strcpy(ply_ptr->name,"\1\2\255\252\240\251\229\201\247\0");
	else
		strcpy(ply_ptr->name,"Tesseract");

	return(0);
}
/**************************************/
/**************************************/
int dm_view(creature *ply_ptr, cmd *cmnd )
{
	char    file[80];
	/*int             i=0, j=0;*/

    if (ply_ptr->class < BUILDER)
            return(PROMPT);

    if (cmnd->num < 2){
		output(ply_ptr->fd,"View what file?\n");
        return(PROMPT);
  	}
/*
    while(isspace(cmnd->fullstr[i]))
            i++;
    sprintf(g_buffer,"file: %s\n",&cmnd->fullstr[i]);
    output(ply_ptr->fd, g_buffer);
    while(!isspace(cmnd->fullstr[i]))
                    i++;
    sprintf(g_buffer,"file: %s\n",&cmnd->fullstr[i]);
    output(ply_ptr->fd,g_buffer);
    while(isspace(cmnd->fullstr[i]))
            i++;
    sprintf(g_buffer,"file: %s\n",&cmnd->fullstr[i]);
    output(ply_ptr->fd, g_buffer);

    while(!isspace(cmnd->fullstr[i])){
            name[j] = cmnd->fullstr[i];
            if (cmnd->fullstr[i] == '\n')
                    break;
            j++;
            i++;
    }
*/
    sprintf(file,"%s/%s", get_post_path(), cmnd->str[1]);
    sprintf(g_buffer,"file: %s\n",file);
    output(ply_ptr->fd, g_buffer);
	output_ply_buf(ply_ptr->fd);
    view_file(ply_ptr->fd,1,file);
    return (0);
}


/*************************************************************************/
/*                              dm_obj_name                              */
/*************************************************************************/
/* the dm_obj_name command allows a dm/ caretaker to modify an already *
 * existing object's name, description, wield description, and key     *
 * words. This command does not save the changes to the object to the  *
 * object data base.  This is used to edit an object that may or may 
   not be saved to the database using the dm_save_obj function.         */
 
int dm_obj_name(creature *ply_ptr, cmd *cmnd )
{
    object  *obj_ptr;
    int     fd,i,num;
    char    which;
 
 
    fd = ply_ptr->fd;
    which =0;
    i =0;
        
    if (ply_ptr->class < CARETAKER)
         return(PROMPT); 
                
    if (cmnd->num < 2){
        output(fd,"\nRename what object to what?\n");
        output(fd,"*oname <object> [#] [-dok] <name>\n");
        return(PROMPT);
    }      
                  
    /* parse the full command string for the start of the description 
       (pass  command, object, obj #, and possible flag).   The object
       number has to be interpreted separately, and with the normal
       command parse (cmnd->val), due to problems caused having
       the object number followed by a "-"
    */

       while(isspace((int)cmnd->fullstr[i]))
            i++;
       while(!isspace((int)cmnd->fullstr[i]))
            i++;
       while(isspace((int)cmnd->fullstr[i]))
            i++;
       while(isalpha((int)cmnd->fullstr[i]))
            i++;
       while(isspace((int)cmnd->fullstr[i]))
            i++;

    cmnd->val[1]= 1;
    if (isdigit((int)cmnd->fullstr[i]))
        cmnd->val[1] = atoi(&cmnd->fullstr[i]); 

    obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj, cmnd->str[1], 
                           cmnd->val[1]);    

    if (!obj_ptr){

    obj_ptr = find_obj(ply_ptr, ply_ptr->parent_rom->first_obj, cmnd->str[1], cmnd->val[1]);    
	}
    if (!obj_ptr){
        output(fd,"Item not found.\n");
        return(PROMPT);
    }

       while(isdigit((int)cmnd->fullstr[i]))
            i++;
       while(isspace((int)cmnd->fullstr[i]))
            i++;

    /* parse flag */          
        if (cmnd->fullstr[i] == '-'){
            if (cmnd->fullstr[i+1] == 'd'){
                which =1;
                i += 2;
            }
            else if (cmnd->fullstr[i+1] == 'o'){
                which =2;
                i += 2;
            }
            else if (cmnd->fullstr[i+1] == 'k'){
                i += 2;
                which =3;
                num = atoi(&cmnd->fullstr[i]);
                if (num <1 || num > 3)
                    num = 0;
                while(isdigit((int)cmnd->fullstr[i]))
                    i++;
            }
            while(isspace((int)cmnd->fullstr[i]))
                i++;
        }

    /* no description given */
      if (cmnd->fullstr[i] == 0)
         return(PROMPT);      
     
    /*handle object modification */    

    switch (which){
        case 0:
            strncpy(obj_ptr->name,&cmnd->fullstr[i],80);
            obj_ptr->name[79]=0;
	    output(fd, "\nName ");
            break;
        case 1:
            strncpy(obj_ptr->description,&cmnd->fullstr[i],80);
            obj_ptr->description[79]=0;
	    output(fd, "\nDescription ");
            break;
        case 2:
            strncpy(obj_ptr->use_output,&cmnd->fullstr[i],80);
            obj_ptr->use_output[79]=0;
	    output(fd, "\nOutput String ");
            break;
        case 3:
            if (num){
            strncpy(obj_ptr->key[num-1],&cmnd->fullstr[i],20);
            obj_ptr->key[num-1][19] =0;
            output(fd, "\nKey ");
	    }
            break;
    }                
    output(fd,"done.\n");

    return(0);
}  

 
/*************************************************************************/
/*                              dm_crt_name                              */
/*************************************************************************/
/* the dm_obj_name command allows a dm/ caretaker to modify an already *
 * existing object's name, description, wield description, and key     *
 * words. This command does not save the changes to the object to the  *
 * object data base.  This command is intended for adding personalize  *
 * weapons and objects to the game */
 
int dm_crt_name(creature *ply_ptr, cmd *cmnd )
{
    creature  *crt_ptr;
        room      *rom_ptr;
    int       fd,i,num;
    char      which;
 
 
    fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
    which =0;
    i =0;
        
    if (ply_ptr->class < CARETAKER )
         return(PROMPT); 
                
    if (cmnd->num < 2){
        output(fd,"\nRename what creature to what?\n");
        output(fd,"*cname <creature> [#] [-dtmk] <name>\n");
        return(PROMPT);
    }      
                  
    /* parse the full command string for the start of the description 
       (pass  command, object, obj #, and possible flag).   The object
       number has to be interpreted separately, ad with the normal
       command parse (cmnd->val), due to problems caused having
       the object number followed by a "-"
    */
 
       while(isspace((int)cmnd->fullstr[i]))
            i++;
       while(!isspace((int)cmnd->fullstr[i]))
            i++;
       while(isspace((int)cmnd->fullstr[i]))
            i++;
       while(isalpha((int)cmnd->fullstr[i]))
            i++;
       while(isspace((int)cmnd->fullstr[i]))
            i++;
 
    cmnd->val[1]= 1;
    if (isdigit((int)cmnd->fullstr[i]))
        cmnd->val[1] = atoi(&cmnd->fullstr[i]); 
 
    crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[1], 
                           cmnd->val[1]);    
 
    if (!crt_ptr){
        output(fd,"Creature not found in the room.\n");
        return(PROMPT);
    }
 
       while(isdigit((int)cmnd->fullstr[i]))
            i++;
       while(isspace((int)cmnd->fullstr[i]))
            i++;
 
    /* parse flag */          
        if (cmnd->fullstr[i] == '-'){
            if (cmnd->fullstr[i+1] == 'd'){
                which =1;
                i += 2;
            }
            else if (cmnd->fullstr[i+1] == 't'){
                which =2;
                i += 2;
            }
            else if (cmnd->fullstr[i+1] == 'k'){
                i += 2;
                which =3;
                num = atoi(&cmnd->fullstr[i]);
                if (num <1 || num > 3)
                    num = 0;
                while(isdigit((int)cmnd->fullstr[i]))
                    i++;
            }
            while(isspace((int)cmnd->fullstr[i]))
                i++;
        }
 
    /* no description given */
      if (cmnd->fullstr[i] == 0)
         return(PROMPT);      
     
    /*handle object modification */    
 
    switch (which){
        case 0:
            strncpy(crt_ptr->name,&cmnd->fullstr[i],80);
            crt_ptr->name[79]=0;
            output(fd, "\nName ");
            break;
        case 1:
            strncpy(crt_ptr->description,&cmnd->fullstr[i],80);
            crt_ptr->description[79]=0;
            output(fd, "\nDescription ");
            break;
        case 2:
            strncpy(crt_ptr->talk,&cmnd->fullstr[i],80);
            crt_ptr->talk[79]=0;
            output(fd, "\nTalk String ");
            break;
        case 3:
            if (num){
            strncpy(crt_ptr->key[num-1],&cmnd->fullstr[i],20);
            crt_ptr->key[num-1][19] =0;
            output(fd, "\nKey ");
            }
            break;
		default:
			output(fd, "\nNothing ");
    }                
    output(fd,"done.\n");
	sprintf(crt_ptr->password, "%d", 0);
	return(0);
}  
  
