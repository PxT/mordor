/*
 * DM7.C:
 *
 *  DM functions
 *
 *  Copyright (C) 1995, 1997 Brooke Paul
 *
 */

#include "mstruct.h"
#include "mextern.h"
#ifdef DMALLOC
  #include "/usr/local/include/dmalloc.h"
#endif

/**************************************************************************
 *  dm_save_crt allows a dm to save modified creatures.  
 */

int dm_save_crt(ply_ptr, cmnd)
creature 	*ply_ptr;
cmd		*cmnd;
{
	creature  *crt_ptr;
	ttag      *tp,*tempt;
        char	  file[80];
	int	  fd, ff, n, index, i=0,x,y, bad=0;
	otag	  *op, *otemp;
	ctag	  *fol;
	fd = ply_ptr->fd;

	if(ply_ptr->class < DM)
		return(PROMPT);

	if(cmnd->num < 3) {
		print(fd, "Syntax: *save c [name] [number]\n");
		return(0);
	}
        
       crt_ptr = find_crt(ply_ptr,ply_ptr->parent_rom->first_mon, cmnd->str[2], 1);    
 
       if (!crt_ptr)
       {
         print(fd,"Creature not found.\n");
         return(PROMPT);
       }

       if(cmnd->val[2] < 2 || cmnd->val[2] > CMAX) {
		print(fd, "Index error: creature number invalid.\n");
		return(PROMPT);   
       }
	index = cmnd->val[2];
        tp = crt_ptr->first_tlk;
	while(tp) {
		tempt = tp->next_tag;
		if(tp->key) free(tp->key);
		if(tp->response) free(tp->response);
		if(tp->action) free(tp->action);
		if(tp->target) free(tp->target);
	        free(tp);
		tp = tempt;
	}
        crt_ptr->first_tlk = 0;
	op=crt_ptr->first_obj;
	if(!F_ISSET(crt_ptr, MTRADE)) { 
		while(op) {
			x=find_obj_num(op->obj);
			if(!x) {
			print(fd,"Unique object in inventory not saved.\n");
			op=op->next_tag;
			continue;
			}
			crt_ptr->carry[i]=(long)x;
			i++;
			if(i>9){
			print(fd, "Only first 10 objects in inventory saved.\n");
			break;
			}		
		op=op->next_tag;
		}
	 /*

	else 
		while(op) {
			otemp=op->next_tag;
			x=find_obj_num(op->obj);		
			if(!x) {
                        print(fd,"Unique object in inventory.\n");
			print(fd, "Fix inventory and resave.\n");
			print(fd, "Save Aborted.\n");
			bad=1;
			break;
                        }
			y=0;
			if(otemp)
				y=find_obj_num(otemp->obj);
			if(!y) {
                        print(fd, "Fix inventory and resave.\n");
                        print(fd, "Save Aborted.\n");
                        bad=1;
                        break;
                        }
			crt_ptr->carry[i]=(long)x;
			crt_ptr->carry[i+5]=(long)y;
			i +=2;  
                        if(i>9){
                        print(fd, "Too many items in inventory.\n");
			print(fd, "Fix inventory and resave.\n");
                        print(fd, "Save Aborted.\n");
                        bad=1;
			break;
                        }
		
		op=otemp->next_tag;
		}
		if(bad)
			return(PROMPT); */

		/* clear remaining carry slots */
	        while(i<9) {
                crt_ptr->carry[i]=0;
                i++;
        	}

	}
	/* flush inventory -- dont exactly know why this is necessary... */
	op=crt_ptr->first_obj;
	crt_ptr->first_obj=0;
	while(op) {
		otemp=op->next_tag;
		free_obj(op->obj);
		free(op);
		op=otemp;
	}

	/* clean up possesed before save */
	if(F_ISSET (crt_ptr, MDMFOL)) { /* clear relevant follow lists */
                        F_CLR(crt_ptr->following, PALIAS);
			F_CLR(crt_ptr, MDMFOL);
                        Ply[crt_ptr->following->fd].extr->alias_crt = 0;
                        print (crt_ptr->following->fd, "%1M's soul was saved.\n", crt_ptr);
                        fol = crt_ptr->following->first_fol;
                        if(fol->crt == crt_ptr) {
                                crt_ptr->following->first_fol = fol->next_tag;
                                free(fol);
                        }
	
	 }
	/* Update index */
	sprintf(crt_ptr->password, "%d", index);

	sprintf(file, "%s/m%02d", MONPATH, index/MFILESIZE);
        ff = open(file, O_RDONLY, 0);
        if(ff >= 0)
        {
             print(fd, "Creature #%d might already exist.\n", index);
        }
        close(ff);
       
       sprintf(file, "%s/m%02d", MONPATH, index/MFILESIZE);
       ff = open(file, O_WRONLY | O_CREAT, ACC);
       if(ff == -1) {
		close(ff);
		print(fd, "File open error.\n");
		return(0);
	}
	n = lseek(ff, (long)((index%MFILESIZE)*sizeof(creature)), 0);
     
        n = write(ff, crt_ptr, sizeof(creature));
	if(n < sizeof(creature))
		merror("write_crt", FATAL);
   
	close(ff);
	print(fd, "Creature #%d updated.\n", index);
	return(0);

}

/**********************************************************************/
/*				dm_add_crt			      */
/**********************************************************************/

/* This function creates a generic creature for a DM to work on.       */
/*              						      */

int dm_add_crt(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	creature  *new_crt;
	int			fd, n;

	fd = ply_ptr->fd;

	if(ply_ptr->class < DM)
		return(PROMPT);

        new_crt = (creature *)malloc(sizeof(creature));
	if(!new_crt) {
		print(fd, "Cannot allocate memory for creature.\n");
		merror("dm_add_crt", NONFATAL);
		return(PROMPT);
	}	       
	zero(new_crt, sizeof(creature));
        
       	strcpy(new_crt->name, "clay form");
       	strcpy(new_crt->description,"An unformed mass of clay is here.");
        strcpy(new_crt->key[0],"form");
        new_crt->level = 1;
        new_crt->type = MONSTER;
        new_crt->strength     = 10;
        new_crt->dexterity    = 10;
        new_crt->constitution = 10;
        new_crt->intelligence = 10;
        new_crt->piety        = 10;
        new_crt->hpmax = new_crt->hpcur = 12;
        new_crt->armor = 100;
        new_crt->thaco = 19;
        new_crt->experience = 10;
        new_crt->ndice = 1;
        new_crt->sdice = 4;
        new_crt->pdice = 1;
        new_crt->first_obj = 0;
	new_crt->first_fol = 0;
	new_crt->first_enm = 0;
	new_crt->first_tlk = 0;
	new_crt->parent_rom = 0;
	new_crt->following = 0;
	for(n=0; n<20; n++)
		new_crt->ready[n] = 0; 

   	add_crt_rom(new_crt, ply_ptr->parent_rom, 1);
	print(fd, "Creature created.\n");
	return(0);

}
/*****************************************************************************
 *  dm_add_obj and dm_save_obj 
 *  Jan 11 1996
 */

int dm_add_obj(ply_ptr,cmnd)
creature *ply_ptr;
cmd      *cmnd;
{
        object    *new_obj;
	int		fd;

	fd = ply_ptr->fd;

	if(ply_ptr->class < DM)
		return(PROMPT);

	if(cmnd->val[1] < 1) {
		print(fd, "Add what?\n");
		return(0);
	}

        new_obj = (object *)malloc(sizeof(object));
	if(!new_obj) {
		merror("dm_add_obj", NONFATAL);
	        print(fd, "Cannot allocate object.\n");
		return(0);
	}
	       
	zero(new_obj, sizeof(object));
        strcpy(new_obj->name,"small clay ball");
        strcpy(new_obj->description,"You see a plain grey ball.  Perhaps you can make something out of it.");
        strcpy(new_obj->key[0],"ball");
	strcpy(new_obj->key[1],"clay");
        strcpy(new_obj->use_output,"You squeeze a clay ball in your hand.");
	new_obj->wearflag = HELD;
	new_obj->type = MISC;
        new_obj->first_obj = 0;
        new_obj->parent_obj = 0;
        new_obj->parent_rom = 0;
        new_obj->parent_crt = 0;
	
	add_obj_crt(new_obj, ply_ptr);
	print(fd, "Generic object added to your inventory.\n");
	return(0);

}


int dm_save_obj(ply_ptr, cmnd)
creature 	*ply_ptr;
cmd		*cmnd;
{
	object    *obj_ptr,*po;
        otag      *fo;
        room      *rom;
        creature  *pc;
        char	  file[80];
	int			fd, ff, n, index;
        
	fd = ply_ptr->fd;

	if(ply_ptr->class < DM)
		return(PROMPT);

	if(cmnd->num < 3) {
		print(fd, "Syntax: *save o [object] [index]\n");
		return(0);
	}
     
        obj_ptr = find_obj(ply_ptr,ply_ptr->parent_rom->first_obj, cmnd->str[2], 1);    
 
        if (!obj_ptr)
		obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj, cmnd->str[2],1);
  
	if (!obj_ptr){
	        print(fd,"Object not found.\n");
         	return(PROMPT);
        }
       
	if(cmnd->val[2] < 2 || cmnd->val[2] > OMAX) {
	  print(fd,"Index error: object number invalid.\n");
          return(PROMPT);  
        }       

       index=cmnd->val[2];
       sprintf(file, "%s/o%02d", OBJPATH, index/OFILESIZE);
       ff = open(file, O_WRONLY, index);
       if(ff >= 0)
             print(fd, "Object might already exist.\n");
       close(ff);     

       sprintf(file, "%s/o%02d", OBJPATH, index/OFILESIZE);
       ff = open(file, O_WRONLY | O_CREAT, ACC);
       if(ff == -1) {
		close(ff);
		print(fd, "File open error.\n");
		return(0);
	}
	n = lseek(ff, (long)((index%OFILESIZE)*sizeof(object)), 0);
     
       rom = obj_ptr->parent_rom;
       po  = obj_ptr->parent_obj;
       fo  = obj_ptr->first_obj;
       pc  = obj_ptr->parent_crt;
       obj_ptr->parent_rom = 0;
       obj_ptr->parent_obj = 0;
       obj_ptr->first_obj = 0;
       obj_ptr->parent_crt = 0;    
   

        n = write(ff, obj_ptr, sizeof(object));
	if(n < sizeof(object))
		merror("write_obj", FATAL);

	close(ff);
	print(fd, "Object #%d updated.\n", index);
       
       obj_ptr->parent_rom = rom;
       obj_ptr->parent_obj = po;
       obj_ptr->first_obj = fo;
       obj_ptr->parent_crt = pc;    

   return(0);  
}



/*****************************************************************************
 * Talk editing functions 
 * 
 */

int dm_add_talk(ply_ptr, cmnd)
creature *ply_ptr;
cmd      *cmnd;
{
  ttag     *talk;
  int      fd;
   
  fd = ply_ptr->fd;
   
  if(ply_ptr->first_tlk)
     {
       print(fd,"You have a talk defined already.\n");
       return(PROMPT);
     }

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
}


int dm_add_talk_key(ply_ptr, cmnd)
creature *ply_ptr;
cmd      *cmnd;
{
  int      fd;
  char     *ptr;
   
  fd = ply_ptr->fd;
   
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
   if(!*ptr)
     {
       print(fd,"*tkey <keyword [action] [action args]>");
       return 0;
     }
   ply_ptr->first_tlk->key = (char *)malloc(strlen(ptr)+1);
   if(!ply_ptr->first_tlk->key)
     merror("dm_add_talk_key",FATAL);
   strcpy(ply_ptr->first_tlk->key,ptr);
   return(PROMPT);
}

int dm_add_talk_resp(ply_ptr, cmnd)
creature *ply_ptr;
cmd      *cmnd;
{
  int      fd,append;
  char     *ptr;
  char     buf[1024];
   
  fd = ply_ptr->fd;
   
  if(!ply_ptr->first_tlk)
     {
       print(fd,"You must start a talk first.\n");
       return(PROMPT);
     }
  
  ptr = cmnd->fullstr;
  for(;*ptr == ' ';ptr++);
  for(;;ptr++)
     {
       if(!*ptr)
	  {
           print(fd,"*tres +/- <line>\n");
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
   
   return(PROMPT);
}


int dm_add_talk_show(ply_ptr, cmnd)
creature *ply_ptr;
cmd      *cmnd;
{
	int		fd;
   
  fd = ply_ptr->fd;
   
	if(!ply_ptr->first_tlk) {
       print(fd,"You must start a talk first.\n");
       return(PROMPT);
     }

	if(ply_ptr->first_tlk->key) {
	print(fd,"Keyword: ");
        print(fd,ply_ptr->first_tlk->key);
	print(fd,"\n");
     }
	if(ply_ptr->first_tlk->response) {
	print(fd,"Response:\n");
        print(fd,ply_ptr->first_tlk->response);
	print(fd,"\n");
     }
	return(0);
}


int dm_add_talk_save(ply_ptr, cmnd)
creature *ply_ptr;
cmd      *cmnd;
{
  int      fd,i;
  char     file[256];
  char     crt_name[80];
  FILE     *fp;
  creature *crt_ptr;
   
   
  fd = ply_ptr->fd;
   
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
  cmnd->val[1]= 1;
  crt_ptr = find_crt(ply_ptr,ply_ptr->parent_rom->first_mon, cmnd->str[1],
                           cmnd->val[1]);    
 
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
  fp = fopen(file,"a");
  fprintf(fp,"%s\n",ply_ptr->first_tlk->key);
  fprintf(fp,"%s\n",ply_ptr->first_tlk->response);
  fclose(fp);
  print(fd,"Talk saved.\n");
  return 0;
   
}

/***************************************************************************
*	This function finds an object or creature number from the
*	database.
*/

int dm_find_db(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	int fd, n;
	object		*obj_ptr;
	creature	*crt_ptr;

	if(ply_ptr->class < CARETAKER)
		return(PROMPT);

	fd=ply_ptr->fd;

	if(cmnd->num < 3) {
		print(fd, "Syntax: *find o|c name [#]\n");
		return(0);
	}

	switch(low(cmnd->str[1][0])) {
		case 'o':
			obj_ptr=find_obj(ply_ptr, ply_ptr->first_obj, cmnd->str[2], cmnd->val[2]);	
			if(!obj_ptr)
				obj_ptr=find_obj(ply_ptr, ply_ptr->parent_rom->first_obj, cmnd->str[2], cmnd->val[2]);
			if(!obj_ptr) {
				print(fd, "Cannot locate object.\n");
				return(0);
			}
			n=find_obj_num(obj_ptr);
			if(n)
				print(fd, "Object is #%d.\n", n);
			else
				print(fd, "Object is unique.\n");

			break;	
		case 'm':
		case 'c':
			crt_ptr=find_crt(ply_ptr, ply_ptr->parent_rom->first_mon, cmnd->str[2], cmnd->val[2]);
			if(!crt_ptr) {
				print(fd, "Cannot locate creature.\n");
				return(0);
			}
			n=find_crt_num(crt_ptr);
			if(n)
                                print(fd, "Creature is #%d.\n", n);
                        else
                                print(fd, "Creature is unique.\n");
        
                        break;

		default:
			print(fd, "Syntax: *find o|c name [#]\n");
			return(0);
	}
}

/*************************************************************************
*  This function allows the deletion of the database.
*
*/
int dm_delete_db(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	int 		fd, ff, index,n;
	creature	*crt_ptr;
	object		*obj_ptr;
	char      	file[80];

	if(ply_ptr->class < DM)
		return(PROMPT);

	fd=ply_ptr->fd;

	if(cmnd->num < 2) {
		print(fd, "Syntax: *clear [o|c] [index]\n");
		return(0);
	}

	if(!strcmp(cmnd->str[1], "c") || !strcmp(cmnd->str[1], "m")) {
		if(cmnd->val[1] < 2 || cmnd->val[1] > CMAX) {
                	print(fd, "Index error.\n");
                	return(0);
		}
	
		index = cmnd->val[1];
		
		sprintf(file, "%s/m%02d", MONPATH, index/MFILESIZE);
       		ff = open(file, O_WRONLY | O_BINARY, ACC);
       		if(ff == -1) {
                	close(ff);
                	print(fd, "File open error.\n");
                	return(0);
        	}
        	
		crt_ptr=(creature *)malloc(sizeof(creature));
		zero(crt_ptr, sizeof(creature));
		n = lseek(ff, (long)((index%MFILESIZE)*sizeof(creature)), 0);
        	n = write(ff, crt_ptr, sizeof(creature));
        	if(n < sizeof(creature))
                	merror("dm_delete_db", FATAL);
 
        	close(ff);
		print(fd,"Creature #%d deleted.\n", index);
		return(0);
	}
	else {
		if(strcmp(cmnd->str[1], "o")) {	
                	print(fd, "Syntax: *clear [o|c] [index].\n");
                	return(0);
		        }
		if(cmnd->val[1] < 2 || cmnd->val[1] > OMAX) {
                        print(fd, "Index error.\n");
                        return(0);
                }
	
		index = cmnd->val[1];
                
        sprintf(file, "%s/o%02d", OBJPATH, index/OFILESIZE);
        ff = open(file, O_WRONLY | O_BINARY, ACC);
        if(ff == -1) {
			close(ff);
            print(fd, "File open error.\n");
            return(0);
        }

		obj_ptr=(object *)malloc(sizeof(object));
        zero(obj_ptr, sizeof(object));
        n = lseek(ff, (long)((index%OFILESIZE)*sizeof(object)), 0);
        n = write(ff, obj_ptr, sizeof(object));
        if(n < sizeof(object))
			merror("dm_delete_db", FATAL);
 
        close(ff);
        print(fd, "Object #%d deleted.\n", index);
        return(0);
	}

}

/******************************************************************
*
*	dm_game_status:
*		Show the status of all configurable options in the game.
*
******************************************************************/

int dm_game_status(creature *ply_ptr, cmd *cmnd)
{
	int fd, i;
	char str[80];

	if(ply_ptr->class < CARETAKER) 
		return(PROMPT);

	if(cmnd->num == 2 && !strcmp(cmnd->str[1], "r")) {
		i=readini();
		print(ply_ptr->fd, "Ini Reloaded.\n");
		return(0);
	}
	
	fd=ply_ptr->fd;

	ANSI(fd, BLUE);
	print(fd,"\n\nGame Variable Status\n");
	ANSI(fd, BOLD);
	ANSI(fd, CYAN);
	print(fd,"Game Title: ");
	ANSI(fd, WHITE);
	print(fd, "%s\n", title);
	ANSI(fd, CYAN);
	print(fd,"Game Port : ");
	ANSI(fd, WHITE);
	print(fd,"%d\n\n",PORTNUM);
	ANSI(fd, CYAN);
	print(fd,"DMs here are: ");
	ANSI(fd, WHITE);
	print(fd,"%s, %s, %s, %s\n", dmname[0], dmname[1], dmname[2], dmname[3]);
	print(fd,"            : %s, %s, %s\n", dmname[4], dmname[5], dmname[6]);
	ANSI(fd, CYAN);
	print(fd,"Questions email address    : ");
	ANSI(fd, WHITE);
	print(fd,"%s\n", questions_to_email);
	ANSI(fd, CYAN);
	print(fd,"Authorization email address: ");
	ANSI(fd, WHITE);
	print(fd,"%s\n", auth_questions_email);
	ANSI(fd, CYAN);
	print(fd,"Registrations email address: ");
	ANSI(fd, WHITE);
	print(fd,"%s\n", register_questions_email);
	ANSI(fd, CYAN);
	print(fd,"Account exists message: ");
	ANSI(fd, WHITE);
	print(fd,"%s\n", account_exists);
	
	ANSI(fd, NORMAL);
	ANSI(fd, BLUE);
	print(fd, "\nOn/Off Settings (1=On, 0=Off)\n");
	ANSI(fd, BOLD);
	ANSI(fd, CYAN);
	print(fd, "Ansi Bottom Line = ");
	ANSI(fd, WHITE);
	print(fd, "%d",ANSILINE);
	ANSI(fd, CYAN);
	print(fd, ",  AutoShutDown = ");
	ANSI(fd, WHITE);
	print(fd, "%d",AUTOSHUTDOWN);
	ANSI(fd, CYAN);
	print(fd, ",  Checkdouble  = ");
	ANSI(fd, WHITE);
	print(fd, "%d\n",CHECKDOUBLE);
	ANSI(fd, CYAN);
	print(fd, "Crashtrap        = ");
	ANSI(fd, WHITE);
	print(fd, "%d",CRASHTRAP);
	ANSI(fd, CYAN);
	print(fd, ",  EatnDrink    = ");
	ANSI(fd, WHITE);
	print(fd, "%d",EATNDRINK);
	ANSI(fd, CYAN);
	print(fd, ",  GetHostByName= ");
	ANSI(fd, WHITE);
	print(fd, "%d\n",GETHOSTBYNAME);
	ANSI(fd, CYAN);
	print(fd, "Hashrooms        = ");
	ANSI(fd, WHITE);
	print(fd, "%d",HASHROOMS);
	ANSI(fd, CYAN);
	print(fd, ",  Heaven       = ");
	ANSI(fd, WHITE);
	print(fd, "%d",HEAVEN);
	ANSI(fd, CYAN);
	print(fd, ",  Isengard     = ");
	ANSI(fd, WHITE);
	print(fd, "%d\n",ISENGARD);
	ANSI(fd, CYAN);
	print(fd, "LastCommand      = ");
	ANSI(fd, WHITE);
	print(fd, "%d",LASTCOMMAND);
	ANSI(fd, CYAN);
	print(fd, ",  NiceExp      = ");
	ANSI(fd, WHITE);
	print(fd, "%d",NICEEXP);
	ANSI(fd, CYAN);
	print(fd, ",  Paranoid     = ");
	ANSI(fd, WHITE);
	print(fd, "%d\n",PARANOID);
	ANSI(fd, CYAN);
	print(fd, "Record_All       = ");
	ANSI(fd, WHITE);
	print(fd, "%d",RECORD_ALL);
	ANSI(fd, CYAN);
	print(fd, ",  RFC1413      = ");
	ANSI(fd, WHITE);
	print(fd, "%d",RFC1413);
	ANSI(fd, CYAN);
	print(fd, ",  Secure       = ");
	ANSI(fd, WHITE);
	print(fd, "%d\n",SECURE);
	ANSI(fd, CYAN);
	print(fd, "Sched            = ");
	ANSI(fd, WHITE);
	print(fd, "%d",SCHED);
	ANSI(fd, CYAN);
	print(fd, ",  Suicide      = ");
	ANSI(fd, WHITE);
	print(fd, "%d",SUICIDE);
	ANSI(fd, CYAN);
	print(fd, ",  Save_on_drop = ");
	ANSI(fd, WHITE);
	print(fd, "%d\n",SAVEONDROP);
	ANSI(fd, NORMAL);
	ANSI(fd, WHITE);

	F_SET(ply_ptr, PREADI);
	if(ANSILINE)
		ask_for(fd, "[Hit Return, Q to quit]: ");
	else
		print(fd, "[Hit Return, Q to Quit]: ");
	output_buf();
	Ply[fd].io->intrpt &= ~1;
	Ply[fd].io->fn = gamestat2;
	Ply[fd].io->fnparam = 1;
	return(DOPROMPT);

}

void gamestat2(int fd, int param,char *instr)
{

	if(instr[0]) {
		print(fd, "Aborted.\n");
		F_CLR(Ply[fd].ply, PREADI);
		RETURN(fd, command, 1);
	}

	ANSI(fd, BLUE);
	print(fd,"\nSailing Messages\n");
	ANSI(fd, BOLD); ANSI(fd, CYAN);
	print(fd, "TXmesg1: "); ANSI(fd, WHITE);
	print(fd, "%s\n", tx_mesg1); ANSI(fd, CYAN);
	print(fd, "TXmesg2: "); ANSI(fd, WHITE);
	print(fd, "%s\n", tx_mesg2);

	ANSI(fd, BLUE);
	print(fd,"Weather Strings\n");
	ANSI(fd, BOLD);
	ANSI(fd, CYAN);
	print(fd,"Sunrise: ");
	ANSI(fd, WHITE);
	print(fd, "%s\n", sunrise);
	ANSI(fd, CYAN);
	print(fd,"Sunset: ");
	ANSI(fd, WHITE);
	print(fd, "%s\n", sunset);
	ANSI(fd, CYAN);
	print(fd,"Earth_Trembles: ");
	ANSI(fd, WHITE);
	print(fd, "%s\n", earth_trembles);
	ANSI(fd, CYAN);
	print(fd,"Heavy_fog: ");
	ANSI(fd, WHITE);
	print(fd, "%s\n", heavy_fog);
	ANSI(fd, CYAN);
	print(fd,"Beautiful_day: ");
	ANSI(fd, WHITE);
	print(fd, "%s\n", beautiful_day);
	ANSI(fd, CYAN);
	print(fd,"Bright_sun: ");
	ANSI(fd, WHITE);
	print(fd, "%s\n", bright_sun);
	ANSI(fd, CYAN);
	print(fd,"Glaring Sun: ");
	ANSI(fd, WHITE);
	print(fd, "%s\n", glaring_sun);
	ANSI(fd, CYAN);
	print(fd,"Heat: ");
	ANSI(fd, WHITE);
	print(fd, "%s\n", heat);
	ANSI(fd, CYAN);
	print(fd,"Still: ");
	ANSI(fd, WHITE);
	print(fd, "%s\n", still);
	ANSI(fd, CYAN);
	print(fd,"Light_breeze: ");
	ANSI(fd, WHITE);
	print(fd, "%s\n", light_breeze);	
	ANSI(fd, CYAN);
	print(fd,"Strong_wind: ");
	ANSI(fd, WHITE);
	print(fd, "%s\n", strong_wind);
	ANSI(fd, CYAN);
	print(fd,"Wind_gusts: ");
	ANSI(fd, WHITE);
	print(fd, "%s\n", wind_gusts);
	ANSI(fd, CYAN);
	print(fd,"Gale_force: ");
	ANSI(fd, WHITE);
	print(fd, "%s\n", gale_force);
	ANSI(fd, CYAN);
	print(fd,"Clear Skies: ");
	ANSI(fd, WHITE);
	print(fd, "%s\n", clear_skies);
	ANSI(fd, CYAN);
	print(fd,"Light_clouds: ");
	ANSI(fd, WHITE);
	print(fd, "%s\n", light_clouds);
	ANSI(fd, CYAN);
	print(fd,"Thunderheads: ");
	ANSI(fd, WHITE);
	print(fd, "%s\n", thunderheads);
	ANSI(fd, CYAN);
	print(fd,"Light_rain : ");
	ANSI(fd, WHITE);
	print(fd, "%s\n", light_rain);
	ANSI(fd, CYAN);
	print(fd,"Heavy_rain: ");
	ANSI(fd, WHITE);
	print(fd, "%s\n", heavy_rain);
	ANSI(fd, CYAN);
	print(fd,"Sheets_rain: ");
	ANSI(fd, WHITE);
	print(fd, "%s\n", sheets_rain);
	ANSI(fd, CYAN);
	print(fd,"Torrent_rain: ");
	ANSI(fd, WHITE);
	print(fd, "%s\n", torrent_rain);
	ANSI(fd, CYAN);
	print(fd,"No_moon: ");
	ANSI(fd, WHITE);
	print(fd, "%s\n", no_moon);
	ANSI(fd, CYAN);
	print(fd,"Sliver_moon: ");
	ANSI(fd, WHITE);
	print(fd, "%s\n", sliver_moon);
	ANSI(fd, CYAN);
	print(fd,"Half_moon: ");
	ANSI(fd, WHITE);
	print(fd, "%s\n", half_moon);
	ANSI(fd, CYAN);
	print(fd,"Waxing_moon: ");
	ANSI(fd, WHITE);
	print(fd, "%s\n", waxing_moon);
	ANSI(fd, CYAN);
	print(fd,"Full_moon: ");
	ANSI(fd, WHITE);
	print(fd, "%s\n", full_moon);
	ANSI(fd, NORMAL);
	ANSI(fd, WHITE);

	F_CLR(Ply[fd].ply, PREADI);
	RETURN(fd, command, 1);

}
/************************************************************************/
/*				dm_advance				*/
/************************************************************************/
int dm_advance(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
    	int 		fd, lev, i;
	creature 	*crt_ptr=0;
	 
    	fd = ply_ptr->fd;
 
    	if (ply_ptr->class < DM)
        	return(PROMPT);

    	if (cmnd->num < 2) {
    		print(fd, "Advance who at what level?\n");
    		return(0);
    	}

        cmnd->str[1][0] = up(cmnd->str[1][0]);
        for(i=0; i<Tablesize; i++) {
                if(!Ply[i].ply) continue;
                if(Ply[i].ply->fd == -1) continue;
                if(!strncmp(Ply[i].ply->name, cmnd->str[1], strlen(cmnd->str[1])))
                        crt_ptr = Ply[i].ply;
                if(!strcmp(Ply[i].ply->name, cmnd->str[1]))
                        break;
        }
        if(!crt_ptr) {
                print(fd, "Advance who?\n");
                return(0);
	}
	if(cmnd->val[1] <= 0 || cmnd->val[1] > MAXALVL) {
		print(fd, "Only levels between 1 and 25!\n");
		return(0);
	}
	lev = cmnd->val[1] - crt_ptr->level;
	if(lev == 0) {
		print(fd, "But %M is already level %d!\n", crt_ptr, crt_ptr->level);
		return(0);
	}
	if(lev > 0) {
		for(i=0; i<lev; i++) up_level(crt_ptr);
		print(fd, "%M has been raised to level %d.\n", crt_ptr, crt_ptr->level);
	}
	if(lev < 0) {
		for(i=0; i>lev; i--) down_level(crt_ptr);
		print(fd, "%M has been lowered to level %d.\n", crt_ptr, crt_ptr->level);
	}
}

