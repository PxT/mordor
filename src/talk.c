#include "mstruct.h"
#include "mextern.h"
#ifdef DMALLOC
  #include "/usr/local/include/dmalloc.h"
#endif

/* dm_talk()  Function for writing interactive talk strings on-line
 * Copyright (C) 1996  Brooke Paul & Paul Telford
 *
 * Yes, I know it's a monster function, but it should be quite 
 * readable. 
 */

int dm_talk(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{

  ttag     *talk;
  char     *ptr, buf[1024], file[256], crt_name[80];
  FILE     *fp;
  creature *crt_ptr;
  int fd, i, append;
	
  fd = ply_ptr->fd;

	if(ply_ptr->class < CARETAKER)
	   return(PROMPT);

	if(cmnd->num < 2) {
	print(fd, "Use: *talk [i|k|r|l|s|d]\n");
	return(PROMPT);
	}
	
	switch(cmnd->str[1][0]) {

/*********** START UP TALK STUFF   ************/
	case 'i':  		/* initialize talk stuff */

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
  print(fd,"Talk ready for input.\n");

  return(PROMPT);

/**************  SET KEYS  **********************/

	case 'k':	/* set the key */
  if(!ply_ptr->first_tlk)
     {
       print(fd,"You must start a talk first.\n");
       return(PROMPT);
     }
  if(ply_ptr->first_tlk->key)
   {
      print(fd,"Keyword replaced.\n");
      free(ply_ptr->first_tlk->key);
   }
   else
      print(fd,"Keyword set\n");
   ptr = cmnd->fullstr;
   for(;*ptr == ' ';ptr++);
   for(;*ptr != ' ' && *ptr;ptr++);
   for(;*ptr == ' ' && *ptr;ptr++);
   for(;*ptr != ' ' && *ptr;ptr++);
   for(;*ptr == ' ' && *ptr;ptr++);
   if(!*ptr)
     {
       print(fd,"*talk k keyword [ACTION <action> <action args>]\n");
       return 0;
     }
   ply_ptr->first_tlk->key = (char *)malloc(strlen(ptr)+1);
   if(!ply_ptr->first_tlk->key)
     merror("dm_add_talk_key",FATAL);
   strcpy(ply_ptr->first_tlk->key,ptr);
   return(PROMPT);

/*********  SET THE RESPONSE    *********/

	case 'r':	/* set the response */
  if(!ply_ptr->first_tlk)
     {
       print(fd,"You must start a talk first.\n");
       return(PROMPT);
     }

  ptr = cmnd->fullstr;
  for(;*ptr == ' ';ptr++);
   for(;*ptr != ' ' && *ptr;ptr++);
   for(;*ptr == ' ' && *ptr;ptr++);

  for(;;ptr++)
     {
       if(!*ptr)
          {
           print(fd,"*talk r [-|+ ]<line>\n");
           return(PROMPT);
          }
       if(*ptr == '+')
         {
          append = 1;
          break;
         }
       if(*ptr == '-')
         {
          append = 0;
          break;
         }
     }

   if(!*(ptr+1))
     return PROMPT;

   if(append && ply_ptr->first_tlk->response)
     {
       strcpy(buf,ply_ptr->first_tlk->response);
       free(ply_ptr->first_tlk->response);
       strcat(buf,++ptr);
     }
   else
     {
        free(ply_ptr->first_tlk->response);
        strcpy(buf,++ptr);
     }
   ply_ptr->first_tlk->response = (char *) malloc(strlen(buf)+1);
   if(!ply_ptr->first_tlk->response)
     merror("dm_add_talk_resp",FATAL);
   strcpy(ply_ptr->first_tlk->response,buf);
        print(fd, "Response added.\n");
   return(PROMPT);

/************** DISPLAY OUR WORK    *********/
	case 'l':	/* list what we have so far */
        if(!ply_ptr->first_tlk) {
       print(fd,"You must start a talk first.\n");
       return(PROMPT);
     }

        if(ply_ptr->first_tlk->key) {
        print(fd,"Keyword: ");
        print(fd,ply_ptr->first_tlk->key);
        print(fd,"\n");
     }
	else {
	print(fd, "Keyword: (not set)\n");
	}
	
        if(ply_ptr->first_tlk->response) {
        print(fd,"Response: ");
        print(fd,ply_ptr->first_tlk->response);
        print(fd,"\n");
     }
	else {
	print(fd, "Response: (not set)\n");
	}

        return(0);

/************ SAVE!!!   ***********/

	case 's':	/* save our handiwork */
	  if(!ply_ptr->first_tlk)
     {
       print(fd,"You must start a talk first.\n");
       return(PROMPT);
     }
  if(!ply_ptr->first_tlk->key)
     {
       print(fd,"You must define a keyword.\n");
       return(PROMPT);
     }
  if(!ply_ptr->first_tlk->response)
     {
       print(fd,"You must supply a response.\n");
       return(PROMPT);
     }
if(cmnd->num < 3) {
        print(fd, "Use: *talk s <monster>\n");
        return(PROMPT);
        }

  cmnd->val[2]= 1;
  crt_ptr = find_crt(ply_ptr,ply_ptr->parent_rom->first_mon, cmnd->str[2],
                           cmnd->val[2]);
  if (!crt_ptr)
   {
     print(fd,"Creature not found in the room.\n");
     return(PROMPT);
   }

        F_SET(crt_ptr, MTALKS);         /* make sure the crt knows to respond -PDT */
   strcpy(crt_name,crt_ptr->name);
   for(i=0;crt_name[i];i++)
     if(crt_name[i] == ' ')
        crt_name[i] = '_';
  sprintf(file,"%s/talk/%s-%d",MONPATH,crt_name,crt_ptr->level);
  fp = fopen(file,"a");
	if(!fp) {
		print(fd, "Could not open talk file. (check permissions).\n");
		return(PROMPT);
	}
  fprintf(fp,"%s\n",ply_ptr->first_tlk->key);
  fprintf(fp,"%s\n",ply_ptr->first_tlk->response);
  fclose(fp);
  print(fd,"Talk saved.\n");
  flush_crt();			/* flush memory of old talks and.... */
  load_crt_tlk(crt_ptr);        /* reload so we can see test immediately */
  return 0;			/* that should work, but you might */
				/* need to *reload anyway.  -PDT	*/


/****  DELETE A TALK FILE  ****/

case 'd':		/* Delete the talk file for a monster */
if(cmnd->num < 3) {
        print(fd, "Use: *talk d <monster>\n");
        return(PROMPT);
        }

  cmnd->val[2]= 1;
  crt_ptr = find_crt(ply_ptr,ply_ptr->parent_rom->first_mon, cmnd->str[2],
                           cmnd->val[2]);
  if (!crt_ptr)
   {
     print(fd,"Creature not found in the room.\n");
     return(PROMPT);
   }
   strcpy(crt_name,crt_ptr->name);
   for(i=0;crt_name[i];i++)
     if(crt_name[i] == ' ')
        crt_name[i] = '_';

   sprintf(file,"%s/talk/%s-%d",MONPATH,crt_name,crt_ptr->level);
   unlink(file);
   print(fd, "File deleted.\n");
   return(0);

/****  END OF ALL ALLOWED ARGUMENTS  ****/

	default:
	print(fd, "Use: *talk [i|k|r|l|s|d]\n");
}
}	
