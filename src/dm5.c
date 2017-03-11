/*
 * DM5.C:
 *
 *  DM functions
 *
 *  Copyright (C) 1991, 1992, 1993 Brooke Paul
 *
 * $Id: dm5.c,v 6.16 2001/07/05 14:22:23 develop Exp $
 *
 * $Log: dm5.c,v $
 * Revision 6.16  2001/07/05 14:22:23  develop
 * added immortals only access to dm_memory
 * OLD bug
 *
 * Revision 6.15  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

#include "../include/mordb.h"
#include "mextern.h"

#include <ctype.h>


/*******************************************************************/
/*                             dm_replace                          */
/*******************************************************************/
/* The dm_replace command allows a caretaker+ to search and replace *
 * selected word in a rooms description.  If the search word does   *
 * exists, an error is returned.  NOTE: the command format is       *
 * derived by the txt_parse function. The search pattern may only   *
 * one word, while the repalcement pattern may consist of any       *
 * number of characters, spaces, symbols. The line which the        *
 * repalcement occurs, wil not be reformated. */

int dm_replace(creature *ply_ptr, cmd *cmnd )
{
    room    *rom_ptr;
    int     fd, i;
    char    *desc, *pattern, *replace, *tmp;
    char    domain =0;
    int     len1, len2, len3, len4;
    int     value;

    rom_ptr = ply_ptr->parent_rom;
    fd = ply_ptr->fd;

    if(ply_ptr->class < BUILDER)
        return(PROMPT);

    txt_parse(cmnd->fullstr,&pattern,&value,&replace);


    if (!pattern || !replace) {
        output(fd,"syntax:*replace <pattern> <replacement>\n");
        return(PROMPT);
    }
    domain = 0;
    desc =  rom_ptr->short_desc;
    

    len1 = (desc) ? strlen(desc) : 0;
    len2 = (pattern) ? strlen(pattern) : 0;
    len3 = (replace) ? strlen(replace) : 0;
    len4 = len1 + len3 - len2;

    i=desc_search(desc, pattern,&value); 

    if (i== -1 && value){
        domain = 1;
        desc =  rom_ptr->long_desc;
        i=desc_search(desc, pattern,&value); 
        len1 = (desc) ? strlen(desc) : 0;
        len4 = len1 + len3 - len2;
    }

    if(i<0){
        output(fd, "Pattern not found.\n");
        return(PROMPT);
    }

    tmp = (char *)malloc(len4+1);
    if(!tmp)
           merror("dm_replace", FATAL);     

    memcpy(tmp,desc,i);
    memcpy(&tmp[i],replace,len3);
    memcpy(&tmp[i+len3],&desc[i+len2],len1-(i+len2));
    tmp[len4] = 0; 
    

    free(desc);

    if (domain == 0)
        rom_ptr->short_desc = tmp;
    else
        rom_ptr->long_desc = tmp; 
    
    return(PROMPT);
}
/*******************************************************************/
/*                             dm_delete                           */
/*******************************************************************/
/* The dm_pattern command allows a caretaker+ to search and pattern *
 * selected word in a rooms description.  If the search word does   *
 * exists, an error is returned.  NOTE: the command format is       *
 * derived by the txt_parse function. The search flag may only   *
 * one word, while the repalcement flag may consist of any       *
 * number of characters, spaces, symbols. The line which the        *
 * repalcement occurs, wil not be reformated. */

int dm_delete(creature *ply_ptr, cmd *cmnd )
{
    room    *rom_ptr;
    int     fd, i;
    char    *desc, *flag, *pattern, *tmp;
    char    domain =0, dcase = 0;
    int     len1, len2;
    int     value;

    rom_ptr = ply_ptr->parent_rom;
    fd = ply_ptr->fd;

    if(ply_ptr->class < BUILDER)
        return(PROMPT);

    txt_parse(cmnd->fullstr,&flag,&value,&pattern);

    if (!flag) {
        output(fd,"syntax:*delete [-PESLA] <delete_word>\n");
        return(0);
    }

    if (flag[0] == '-'){
        if (strlen(flag) > 1)
        switch(flag[1]) {
        case 'S':
			if( rom_ptr->short_desc != NULL )
			{
				free(rom_ptr->short_desc);
			}
			rom_ptr->short_desc = NULL;
			return (0);
            break;
        case 'L':
			if ( rom_ptr->long_desc != NULL )
			{
				free(rom_ptr->long_desc);
			}
			rom_ptr->long_desc = NULL;
			return (0);
            break;
       case 'A':
		   if ( rom_ptr->short_desc != NULL )
		   {
				free(rom_ptr->short_desc);
		   }
		   if ( rom_ptr->long_desc != NULL)
		   {
			   free(rom_ptr->long_desc);
		   }
			rom_ptr->short_desc = NULL;
			rom_ptr->long_desc = NULL;
        return (0);
            break;
       case 'E':
        dcase = 1;
        break;
       case 'P': case 'D':
        dcase = 0;
		if (!pattern){
            output(fd,"syntax:*delete [-PESLA] <delete_word>\n");
            return(0);
		}
        break;
       default:
            output(fd,"syntax:*delete [-PESLA] <delete_word>\n");
            return(0);
        break;
    }
    else {
           output(fd,"syntax:*delete [-PESLA] <delete_word>\n");
           return(0);
    }
    }
    else {
        tmp = flag;
        flag = pattern;
        pattern = tmp;
    }


/* search for word / phrase */
        domain = 0;
        desc =  rom_ptr->short_desc;

        len1 = (desc) ? strlen(desc) : 0;
        len2 = (pattern) ? strlen(pattern) : 0;

        i=desc_search(desc, pattern,&value); 

        if (i== -1 && value){
            domain = 1;
            desc =  rom_ptr->long_desc;
            i=desc_search(desc, pattern,&value); 
            len1 = (desc) ? strlen(desc) : 0;
        }

        if(i<0){
            output(fd, "Pattern not found.\n");
            return(0);
        }

/*delete word / phrase */   
    if (dcase == 1){
        tmp = (char *)malloc(i+1);
        if(!tmp)
            merror("dm_delete", FATAL);     

        memcpy(tmp,desc,i);
        tmp[i] = 0; 
    }
    else{
        tmp = (char *)malloc(len1-len2+1);
        if(!tmp)
            merror("dm_delete", FATAL);     

        memcpy(tmp,desc,i);
        memcpy(&tmp[i],&desc[i+len2],len1-(i+len2));
        tmp[len1-len2] = 0; 
    }

        free(desc);

        if (domain == 0)
            rom_ptr->short_desc = tmp;
        else
            rom_ptr->long_desc = tmp; 
       
    return(0);
}

/**************************************************************/
/*        desc_search                  */
/**************************************************************/
 
/* The desc_search function searches the given string (desc)   *
 * for the given pattern (pattern) and returns the location of *
 * the 'val' occurace of the pattern.  Note: the value of      *
 * 'val' will change to reflect to needed number of matchs.    *
 * This is done to allow to search several realted, but not    *
 * conencted strings for the pattern as if searching 1 string  */
 
int desc_search(char *desc, char *pattern, int *val )
{
    int		i, len, len2;
    char        found= 0;
 
    if(!desc || !pattern)
        return (-1);
    len = strlen(pattern); 
    len2 = strlen(desc);
    i = 0;
 
    while(i+len <= len2){
    if(memcmp(pattern,&desc[i],len) == 0){
           (*val)--;
           if((*val)==0) {
                found = 1;
                break;
            }
          }
        i++;
    }
    if(found)
        return(i);
    else
        return(-1);
}
       
/*==================================================================*/
/*                          txt_parse                               */
/*==================================================================*/
/* txt parse is design to parse a string (str), and return the 2nd  *
 * in the string as the pararmetr pattern, if the 2nd word of the   *
 * is followed by a number, then the number value is assignmed to   *
 * val. The remained of of the string is teh assigned to repalce    *
 * parameter. Note: white spaces inbetween the paraments are not    *
 * counted.  If txt_parse is unable to assign a value to  either of *
 * the char parameters, (due to lack of words), the parameters will *
 * be assigned the null value.*/

void txt_parse(char *str, char **pattern, int *val, char **replace )
{
        int     i,j, index, len;
        char    *tmp;

/* remove command */
        len = strlen(str); 
        index = len;
        for(i=0; i < len ; i++)
            if (str[i] == ' ' ){
                    index = i;
                    break;
            }
 
    while (str[index] == ' ')
        index++;


/*extract search pattern */                
    len = strlen(&str[index]);
    j =  len;
    if(!len)
        *pattern = 0;
    else{
        for(i=0; i < len; i++)
            if (str[i + index] == ' '){
                j = i;
                break;
        }

        tmp = (char *)malloc(j+1);
    if(!tmp)
           merror("txt_parse", FATAL);  

        memcpy(tmp,&str[index],j);
        tmp[j] = 0;
        *pattern = tmp;
        index += j;
    } 

    while (str[index] == ' ')
        index++;


/* check number place of pattern */
    if(isdigit((int)str[index])){
        *val = atoi(&str[index]);
        if(*val < 1) *val = 1;
        while(isdigit((int)str[++index]));
        while (str[index] == ' ')
            index++;
        }
    else
        *val = 1;


/* remaining string = repalcement */ 
    len = strlen(&str[index]);
    if (!len)
        *replace = 0;
    else{
        tmp = (char *)malloc(len+1);
    if(!tmp)
           merror("txt_parse", FATAL);  
        memcpy(tmp,&str[index],len);
        tmp[len] = 0;
        *replace = tmp;
    }
    return;
}       

/*==================================================================*/
/*                          dm_nameroom                             */
/*==================================================================*/
int dm_nameroom(creature *ply_ptr, cmd *cmnd )
{
    int     len,i=0;
    room    *rom_ptr;

    rom_ptr = ply_ptr->parent_rom;

    if(ply_ptr->class < BUILDER)
        return(PROMPT);

 
    len = strlen(cmnd->fullstr);

    while ( i < len){
        if (cmnd->fullstr[i] == ' ' && cmnd->fullstr[i+1] != ' ') 
          break;
        i++;
    }
    
    len = strlen(&cmnd->fullstr[i+1]); 
    if (!len) {
                output(ply_ptr->fd, "Rename room to what?\n");
                return(0);
    }

    if (len>79) {
                output(ply_ptr->fd, "Room name too long.\n");
                return(0);
    }
        

    memcpy(rom_ptr->name,&cmnd->fullstr[i+1],len);
    rom_ptr->name[len] = 0;
    return(0);

}

/*==================================================================*/

/*==================================================================*/
/* The dm_append command allows a DM to append a text line onto   *
 * a rooms's description.  The default is a new line appended to  *
 * the end of the long text description. The user can select to   *
 * to append to the short descroption (-s) or the end of the      *
 * with no new line (-n) or both (-ns) */

int dm_append(creature *ply_ptr, cmd *cmnd )
{
    int    len, len2, i =0;
    int    fd;
    room   *rom_ptr;
    char   *buf, *desc, nnl = 0, ds = 0;

    rom_ptr = ply_ptr->parent_rom;
    fd = ply_ptr->fd;

    if(ply_ptr->class < BUILDER)
        return(PROMPT);

    len = strlen(cmnd->fullstr);
    while (i < len){
        if (cmnd->fullstr[i] == ' ' && cmnd->fullstr[i+1] != ' ')
            break;
        i++;
    }
    i++;

   if (i >= len){
        output(fd,"syntax: *append [-sn] <text>\n");
	return(0);
    }

    if (cmnd->fullstr[i]  == '-'){
        if (strlen(&cmnd->fullstr[i]) < 4){
            output(fd,"syntax: *append [-sn] <text>\n");
            return(PROMPT);

        }
        i++;

        if (cmnd->fullstr[i] == 's'){
            ds =1; 
            if (cmnd->fullstr[i+1] == 'n')
                nnl = 1;
        }
        else if (cmnd->fullstr[i] == 'n'){
            nnl = 1;
            if (cmnd->fullstr[i+1] == 's')
               ds =1; 
        }

       while(i < len){
           if (nnl && cmnd->fullstr[i] == ' ')
                break;
           if (cmnd->fullstr[i] == ' ' && cmnd->fullstr[i+1] != ' ')
               break;
            i++;
       }
       i++;
       if (i >= len){
           output(fd,"syntax: *append [-sn] <text>\n");
		   return(PROMPT);
       }
    }

    if (ds){
        desc = rom_ptr->short_desc;
        if (!rom_ptr->short_desc)
            nnl = 1;
    }
    else{
        desc = rom_ptr->long_desc;
        if (!rom_ptr->long_desc)
            nnl = 1;
    }


    len = (&cmnd->fullstr[i]) ? strlen(&cmnd->fullstr[i]) : 0;
    len2 =  (desc) ?  strlen(desc) : 0;

    if (nnl) {
        buf  = (char *)malloc(len2 + len+1);
        memcpy(buf,desc,len2);
        memcpy(&buf[len2],&cmnd->fullstr[i],len);
        buf[len+len2] = 0;
    }
    else {
        buf  = (char *)malloc(len2 + len+2);
        memcpy(buf,desc,len2);
        buf[len2] = '\n';
        memcpy(&buf[len2+1],&cmnd->fullstr[i],len);
        buf[len+len2+1] = 0;
    }
    if (desc)
        free(desc);
    if (ds)
        rom_ptr->short_desc = buf;
    else
        rom_ptr->long_desc = buf;
    return(0);

}
/*==================================================================*/

/*==================================================================*/
/* The dm_prepend command allows a DM to prepend a text line onto   *
 * a rooms's description.  The default is a new line prepend to  *
 * the end of the long text description. The user can select to   *
 * to prepend to the short descroption (-s) or the end of the      *
 * with no new line (-n) or both (-ns) */

int dm_prepend(creature *ply_ptr, cmd *cmnd )
{
    int    len, len2, i =0;
    int    fd;
    room   *rom_ptr;
    char   *buf, *desc, nnl = 0, ds = 0;

    rom_ptr = ply_ptr->parent_rom;
    fd = ply_ptr->fd;

    if(ply_ptr->class < BUILDER)
        return(PROMPT);

    len = strlen(cmnd->fullstr);
    while (i < len){
        if (cmnd->fullstr[i] == ' ' && cmnd->fullstr[i+1] != ' ')
            break;
        i++;
    }
    i++;

   if (i >= len){
        output(fd,"syntax: *prepend [-sn] <text>\n");
	return(0);
    }

    if (cmnd->fullstr[i]  == '-'){
        if (strlen(&cmnd->fullstr[i]) < 4){
            output(fd,"syntax: *prepend [-sn] <text>\n");
            return(0);
        }
        i++;

        if (cmnd->fullstr[i] == 's'){
            ds =1; 
            if (cmnd->fullstr[i+1] == 'n')
                nnl = 1;
        }
        else if (cmnd->fullstr[i] == 'n'){
            nnl = 1;
            if (cmnd->fullstr[i+1] == 's')
               ds =1; 
        }

       while(i < len){
           if (cmnd->fullstr[i] == ' ' && cmnd->fullstr[i+1] != ' ')
               break;
            i++;
       }
       i++;
       if (i >= len){
           output(fd,"syntax: *prepend [-sn] <text>\n");
           return(0);
       }
    }

    if (ds){
        desc = rom_ptr->short_desc;
        if (!rom_ptr->short_desc)
            nnl = 1;
    }
    else{
        desc = rom_ptr->long_desc;
        if (!rom_ptr->long_desc)
            nnl = 1;
    }


    len = (&cmnd->fullstr[i]) ? strlen(&cmnd->fullstr[i]) : 0;
    len2 =  (desc) ?  strlen(desc) : 0;

    if (nnl) {
        buf  = (char *)malloc(len2 + len+1);
        memcpy(buf,&cmnd->fullstr[i],len);
        memcpy(&buf[len],desc,len2);
        buf[len+len2] = 0;
    }
    else {
        buf  = (char *)malloc(len2 + len+2);
        memcpy(buf,&cmnd->fullstr[i],len);
        buf[len] = '\n';
        memcpy(&buf[len+1],desc,len2);
        buf[len+len2+1] = 0;
    }

    free(desc);
    if (ds)
        rom_ptr->short_desc = buf;
    else
        rom_ptr->long_desc = buf;
    return(0);
}


/**********************************************************************/
/*				dm_help				      */
/**********************************************************************/
/* This function allows a DM to obtain a list of flags for rooms, exits, */
/* monsters, players and objects.                                        */

int dm_help(creature *ply_ptr, cmd *cmnd )
{
	char 	file[80];
	int	fd;
#ifndef WIN32
	int a = '/';
#else
	int a = '\\';
#endif
	
	fd = ply_ptr->fd;

	if (ply_ptr->class < IMMORTAL ){
		return(0);
	}

	if(cmnd->num < 2) {
		sprintf(file, "%s/dm_helpfile", get_doc_path());
		view_file(fd, 1, file);
		return(DOPROMPT);
	}

	/* the strrchr call here strips out everything up to the final *
	 * '/' so that we can't read files outside the help dir        */
	
	if(strrchr(cmnd->str[1], a) == '\0')
		sprintf(file, "%s/%s.imm", get_doc_path(), cmnd->str[1]);
	else
		sprintf(file, "%s/%s.imm", get_doc_path(), strrchr(cmnd->str[1],a));


	if(file_exists(file)) {
		view_file(fd, 1, file);
		return(DOPROMPT);
	}
	else {
		output(fd,"That dm help file does not exist.\n");
		return(0);
	}
}



/**********************************************************************/
/*				builder_help			      */
/**********************************************************************/
/* This function allows a BUILDER to obtain help */
int builder_help(creature *ply_ptr, cmd *cmnd )
{
	char 	file[80];
	int	fd;
	
	fd = ply_ptr->fd;

	if (ply_ptr->class < BUILDER ){
		return(0);
	}

	strcpy(file, get_doc_path());

	strcat(file, "/builder_helpfile");
	view_file(fd, 1, file);
	return(DOPROMPT);
}



/************************************************************************/
/* dm_memory()								*/
/************************************************************************/
/* This function allows a DM to obtain a basic printout of memory used 	*/
/* in use in the game.  Adapted from code written by Charles Merchant	*/
/* by JPF															   */
int dm_memory( creature *ply_ptr, cmd *cmnd )
{
	char	buf[256];
	MEM_USAGE	mu;
	int	fd;

	fd = ply_ptr->fd;

	if(ply_ptr->class < IMMORTAL)
			return(0);

	get_memory_usage( &mu );

	output(fd,"Memory Status:\n");
	sprintf(buf,"Total Rooms  :   %-5d",mu.rooms.count);
	output(fd,buf);
	sprintf(buf,"  %ld -> Total memory\n",mu.rooms.mem_used);
	output(fd,buf);
	sprintf(buf,"Total Objects:   %-5d",mu.objects.count);
	output(fd,buf);
	sprintf(buf,"  %ld -> Total memory\n",mu.objects.mem_used);
	output(fd,buf);
	sprintf(buf,"Total Creatures: %-5d",mu.creatures.count);
	output(fd,buf);
	sprintf(buf,"  %ld -> Total memory\n",mu.creatures.mem_used);
	output(fd,buf);
	sprintf(buf,"Total Actions:   %-5d",mu.actions.count);
	output(fd,buf);
	sprintf(buf,"  %ld -> Total memory\n",mu.actions.mem_used);
	output(fd,buf);
	sprintf(buf,"Total Bad Talks: %-5d",mu.bad_talks.count);
	output(fd,buf);
	sprintf(buf,"  %ld -> Total memory\n",mu.bad_talks.mem_used);
	output(fd,buf);
	sprintf(buf,"Total Talks:     %-5d", mu.talks.count);
	output(fd,buf);
	sprintf(buf,"  %ld -> Total memory\n",mu.talks.mem_used);
	output(fd,buf);
	sprintf(buf,"Total Memory:    %ld\n\n",
		mu.bad_talks.mem_used + mu.rooms.mem_used + mu.objects.mem_used + 
		mu.creatures.mem_used + mu.actions.mem_used + mu.talks.mem_used);
	output(fd,buf);


	return(PROMPT);
}
