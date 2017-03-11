/*
 * TALK.C
 *
 *	Handle writing interactive creature talk strgins on-line
 *
 * 	Copyright (C) 1996 Brooke Paul & Paul Telford
 *
 * $Id: talk.c,v 6.13 2001/03/08 16:09:09 develop Exp $
 *
 * $Log: talk.c,v $
 * Revision 6.13  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

#include "../include/mordb.h"
#include "mextern.h"


int dm_talk(creature *ply_ptr, cmd *cmnd )
{

  	int fd;
	
  	fd = ply_ptr->fd;

	if(ply_ptr->class < BUILDER)
	   return(PROMPT);

	if(cmnd->num < 2) {
		output(fd, "Use: *talk [i|k|r|l|s|d]\n");
		return(PROMPT);
	}
	
	switch(cmnd->str[1][0]) {

	case 'i':  			/* initialize talk stuff */
		init_talk(ply_ptr);
		break;
	case 'k':			/* set the key */
		set_keys_talk(ply_ptr, cmnd);
		break;
	case 'r':			/* set the response */
		set_response_talk(ply_ptr, cmnd);
		break;
	case 'l':			/* list what we have so far */
		list_talk(ply_ptr);
		break;
	case 's':			/* save our handiwork */
		save_talk(ply_ptr, cmnd);
		break;
	case 'd':			/* Delete the talk file for a monster */
		delete_talk(ply_ptr, cmnd);
		break;

	default:
		output(fd, "Use: *talk [i|k|r|l|s|d]\n");

	}

	return(0);
}	

int init_talk(creature *ply_ptr)
{

	ttag     *talk;

	talk = (ttag *) malloc(sizeof(ttag));
  	if(!talk)
		merror("dm_add_talk", FATAL);

  	talk->next_tag = 0;
  	talk->key = 0;
  	talk->response = 0;
  	talk->type = 0;
  	talk->action = 0;
  	talk->target = 0;
  	ply_ptr->first_tlk = talk;
  	output(ply_ptr->fd,"Talk ready for input.\n");

  	return(0);
}

int set_keys_talk(creature *ply_ptr, cmd *cmnd)
{

	int fd;
	char *ptr;

	fd = ply_ptr->fd;

	if(!ply_ptr->first_tlk) {
		output(fd,"You must start a talk first.\n");
       		return(0);
     	}
	if(ply_ptr->first_tlk->key) {
      		output(fd,"Keyword replaced.\n");
      		free(ply_ptr->first_tlk->key);
   	} else
      		output(fd,"Keyword set\n");

   ptr = cmnd->fullstr;
   for(;*ptr == ' ';ptr++);
   for(;*ptr != ' ' && *ptr;ptr++);
   for(;*ptr == ' ' && *ptr;ptr++);
   for(;*ptr != ' ' && *ptr;ptr++);
   for(;*ptr == ' ' && *ptr;ptr++);
   if(!*ptr) {
       output(fd,"*talk k keyword [ACTION <action> <action args>]\n");
       return 0;
   }
   ply_ptr->first_tlk->key = (char *)malloc(strlen(ptr)+1);

   if(!ply_ptr->first_tlk->key)
     	merror("dm_add_talk_key",FATAL);

   strcpy(ply_ptr->first_tlk->key,ptr);
   return(0);

}

int set_response_talk(creature *ply_ptr, cmd *cmnd)
{

	int fd,append;
	char *ptr, buf[1024];

	fd = ply_ptr->fd;

  	if(!ply_ptr->first_tlk) {
       		output(fd,"You must start a talk first.\n");
       		return(0);
     	}

  	ptr = cmnd->fullstr;
  	for(;*ptr == ' ';ptr++);
   	for(;*ptr != ' ' && *ptr;ptr++);
   	for(;*ptr == ' ' && *ptr;ptr++);

  	for(;;ptr++) {
       		if(!*ptr) {
           		output(fd,"*talk r [-|+ ]<line>\n");
           		return(0);
          	}
       		if(*ptr == '+') {
          		append = 1;
          		break;
         	}
       		if(*ptr == '-') {
          		append = 0;
          		break;
         	}
	}

	if(!*(ptr+1))
     		return 0;

   	if(append && ply_ptr->first_tlk->response) {
       		strcpy(buf,ply_ptr->first_tlk->response);
       		free(ply_ptr->first_tlk->response);
       		strcat(buf,++ptr);
     	}
   	else {
        	free(ply_ptr->first_tlk->response);
        	strcpy(buf,++ptr);
     	}

   	ply_ptr->first_tlk->response = (char *) malloc(strlen(buf)+1);
   	if(!ply_ptr->first_tlk->response)
     		merror("dm_add_talk_resp",FATAL);

	strcpy(ply_ptr->first_tlk->response,buf);
        output(fd, "Response added.\n");
   	return(0);
}

int list_talk(creature *ply_ptr)
{
	int fd;

	fd = ply_ptr->fd;

	if(!ply_ptr->first_tlk) {
       		output(fd,"You must start a talk first.\n");
       		return(0);
     	}

        if(ply_ptr->first_tlk->key) {
       		output(fd,"Keyword: ");
        	output_nl(fd,ply_ptr->first_tlk->key);
	}
        else 
        	output(fd, "Keyword: (not set)\n");
        

        if(ply_ptr->first_tlk->response) {
        	output(fd,"Response: ");
        	output_nl(fd, ply_ptr->first_tlk->response);
     	}
        else 
        	output(fd, "Response: (not set)\n");
        

        return(0);
}

int save_talk(creature *ply_ptr, cmd *cmnd)
{

	int fd,i;
	creature *crt_ptr;
	char file[256], crt_name[80];
	FILE *fp;

	fd = ply_ptr->fd;

	if(!ply_ptr->first_tlk) {
       		output(fd,"You must start a talk first.\n");
       		return(0);
     	}
  	if(!ply_ptr->first_tlk->key) {
       		output(fd,"You must define a keyword.\n");
       		return(0);
     	}
  	if(!ply_ptr->first_tlk->response) {
       		output(fd,"You must supply a response.\n");
       		return(0);
     	}
  	if(cmnd->num < 3) {
        	output(fd, "Use: *talk s <monster>\n");
        	return(0);
        }

  	crt_ptr = find_crt(ply_ptr,ply_ptr->parent_rom->first_mon, cmnd->str[2],1);
  	if (!crt_ptr) {
     		output(fd,"Creature not found in the room.\n");
     		return(0);
   	}

        F_SET(crt_ptr, MTALKS);         /* make sure the crt knows to respond -PDT */
  	strcpy(crt_name,crt_ptr->name);
   	for(i=0;crt_name[i];i++) {
     		if(crt_name[i] == ' ')
        		crt_name[i] = '_';
	}

	sprintf(file,"%s/talk/%s-%d", get_monster_path(), crt_name,crt_ptr->level);
  	fp = fopen(file,"a");
        if(!fp) {
                output(fd, "Could not open talk file. (check permissions).\n");
                return(0);
        }

  	fprintf(fp,"%s\n",ply_ptr->first_tlk->key);
  	fprintf(fp,"%s\n",ply_ptr->first_tlk->response);
  	fclose(fp);
  	output(fd,"Talk saved.\n");
  	flush_crt();                  	/* flush memory of old talks and.... */
  	load_crt_tlk(crt_ptr);        	/* reload so we can see test immediately */
  	return 0;                     	/* that should work, but you might */
					/* need to *reload anyway.  -PDT        */

}

int delete_talk(creature *ply_ptr, cmd *cmnd)
{

	int fd,i;
	creature *crt_ptr;
	char crt_name[80],file[256];

	fd = ply_ptr->fd;

	if(cmnd->num < 3) {
        	output(fd, "Use: *talk d <monster>\n");
        	return(0);
        }

  	crt_ptr = find_crt(ply_ptr,ply_ptr->parent_rom->first_mon, cmnd->str[2],1);
  	if (!crt_ptr) {
     		output(fd,"Creature not found in the room.\n");
     		return(0);
   	}

	strcpy(crt_name,crt_ptr->name);
   	for(i=0;crt_name[i];i++) {
     		if(crt_name[i] == ' ')
        		crt_name[i] = '_';
	}

   	sprintf(file,"%s/talk/%s-%d",get_monster_path(),crt_name,crt_ptr->level);
   	unlink(file);
   	output(fd, "File deleted.\n");
   	return(0);
}
