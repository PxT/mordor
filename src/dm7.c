/*
 * DM7.C:
 *
 *  DM functions
 *
 *  Copyright (C) 1994, 1995	   Brooke Paul
 *
 * $Id: dm7.c,v 6.19 2001/07/29 22:19:52 develop Exp $
 *
 * $Log: dm7.c,v $
 * Revision 6.19  2001/07/29 22:19:52  develop
 * shop_cost
 *
 * Revision 6.18  2001/07/29 22:11:02  develop
 * added shop_cost
 *
 * Revision 6.17  2001/04/29 02:30:26  develop
 * fixed output on *game
 *
 * Revision 6.16  2001/04/23 04:14:21  develop
 * added NOCREATE stuff
 *
 * Revision 6.15  2001/04/23 04:06:47  develop
 * added NOCREATE
 *
 * Revision 6.14  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

#include "../include/mordb.h"
#include "mextern.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


/**************************************************************************
 *  dm_save_crt allows a dm to save modified creatures.  
 */

int dm_save_crt(creature *ply_ptr, cmd *cmnd )
{
	creature  *crt_ptr;
	ttag      *tp,*tempt;
        char	  file[80];
	int	  fd, index, i=0,x;
	otag	  *op, *otemp;
	ctag	  *fol;
	fd = ply_ptr->fd;

	if(ply_ptr->class < DM)
		return(PROMPT);

	if(cmnd->num < 3) {
		output(fd, "Syntax: *save c [name] [number]\n");
		return(0);
	}
        
	crt_ptr = find_crt(ply_ptr,ply_ptr->parent_rom->first_mon, cmnd->str[2], 1);    

	if (!crt_ptr)
	{
	 output(fd,"Creature not found.\n");
	 return(PROMPT);
	}

	if(cmnd->val[2] < 2 || cmnd->val[2] > CMAX) {
		output(fd, "Index error: creature number invalid.\n");
		return(PROMPT);   
       }
	F_CLR(crt_ptr, MPERMT);
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
			output(fd,"Unique object in inventory not saved.\n");
			op=op->next_tag;
			continue;
			}
			crt_ptr->carry[i]= x;
			output(fd, "Inventory save.\n");
			i++;
			if(i>9){
			output(fd, "Only first 10 objects in inventory saved.\n");
			break;
			}		
		op=op->next_tag;
		}
	 /*

    int y;
	else 
		while(op) {
			otemp=op->next_tag;
			x=find_obj_num(op->obj);		
			if(!x) {
                output(fd,"Unique object in inventory.\n");
				output(fd, "Fix inventory and resave.\n");
				output(fd, "Save Aborted.\n");
				bad=1;
				break;
                }
			y=0;
			if(otemp)
				y=find_obj_num(otemp->obj);
			if(!y) {
                output(fd, "Fix inventory and resave.\n");
                output(fd, "Save Aborted.\n");
                bad=1;
                break;
                }
			crt_ptr->carry[i]=(long)x;
			crt_ptr->carry[i+5]=(long)y;
			i +=2;  
                        if(i>9){
                        output(fd, "Too many items in inventory.\n");
			output(fd, "Fix inventory and resave.\n");
                        output(fd, "Save Aborted.\n");
                        bad=1;
			break;
                        }
		
		op=otemp->next_tag;
		}
		if(bad)
			return(PROMPT); */

		/* clear remaining carry slots */
	        /* while(i<9) {
                crt_ptr->carry[i]=0;
                i++;

        	} */

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


	if(crt_ptr->first_enm) {
		output(fd, "clearing enemy list b4 save\n");
		crt_ptr->first_enm = NULL;
	}

	
	/* clean up possesed before save */
	if(F_ISSET (crt_ptr, MDMFOL)) { /* clear relevant follow lists */
	    F_CLR(crt_ptr->following, PALIAS);
		F_CLR(crt_ptr, MDMFOL);
		Ply[crt_ptr->following->fd].extr->alias_crt = 0;
		mprint (crt_ptr->following->fd, "%1M's soul was saved.\n", 
			m1arg(crt_ptr));
		fol = crt_ptr->following->first_fol;
		if(fol->crt == crt_ptr) {
				crt_ptr->following->first_fol = fol->next_tag;
				free(fol);
		}
	}

	/* Update index */
	sprintf(crt_ptr->password, "%d", index);

	sprintf(file, "%s/m%02d", get_monster_path(), index/MFILESIZE);
	if ( file_exists(file) )
	{
	    sprintf(g_buffer, "Creature #%d might already exist.\n", index);
	    output(fd, g_buffer);
    	}
    
	if ( save_crt_to_file(index, crt_ptr) != 0)
	{
		elog_broad("Error saving creature in dm_save_crt()");
		output( fd, "Error: creature was not saved\n" );
	}
	else
	{
		sprintf(g_buffer, "Creature #%d updated.\n", index);
		output(fd, g_buffer);
	}


	/* swap this new creature if its in the queue */
	replace_crt_in_queue(index, crt_ptr );

	return(0);

}

/**********************************************************************/
/*				dm_add_crt			      */
/**********************************************************************/

/* This function creates a generic creature for a DM to work on.       */
/*              						      */

int dm_add_crt(creature	*ply_ptr, cmd *cmnd )
{
	creature  *new_crt;
	int			fd, n;

	fd = ply_ptr->fd;

	if(ply_ptr->class < DM)
		return(PROMPT);

    new_crt = (creature *)malloc(sizeof(creature));
	if(!new_crt) {
		output(fd, "Cannot allocate memory for creature.\n");
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
	output(fd, "Creature created.\n");
	return(0);

}
/*****************************************************************************
 *  dm_add_obj and dm_save_obj 
 *  Jan 11 1996
 */

int dm_add_obj( creature *ply_ptr, cmd *cmnd )
{
        object    *new_obj;
	int		fd;
	int i;

	fd = ply_ptr->fd;

	if(ply_ptr->class < DM)
		return(PROMPT);

	if(cmnd->val[1] < 1) {
		output(fd, "Add what?\n");
		return(0);
	}

        new_obj = (object *)malloc(sizeof(object));
	if(!new_obj) {
		merror("dm_add_obj", NONFATAL);
	        output(fd, "Cannot allocate object.\n");
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
	
	for(i=0;i<128;i++)
		FS_CLR(new_obj,	i);

	add_obj_crt(new_obj, ply_ptr);
	output(fd, "Generic object added to your inventory.\n");
	return(0);

}

/**********************************************************************/
/*				dm_save_obj			      */
/**********************************************************************/

int dm_save_obj(creature *ply_ptr, cmd *cmnd )
{
	object    *obj_ptr,*po;
        otag      *fo;
        room      *rom;
        creature  *pc;
        char	  file[80];
	int			fd, index;
        
	fd = ply_ptr->fd;

	if(ply_ptr->class < DM)
		return(PROMPT);

	if(cmnd->num < 3) {
		output(fd, "Syntax: *save o [object] [index]\n");
		return(0);
	}
     
    obj_ptr = find_obj(ply_ptr,ply_ptr->parent_rom->first_obj, cmnd->str[2], 1);    

    if (!obj_ptr)
		obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj, cmnd->str[2],1);
  
	if (!obj_ptr){
	        output(fd,"Object not found.\n");
         	return(PROMPT);
        }
       
	if(cmnd->val[2] < 1 || cmnd->val[2] > OMAX) {
	  output(fd,"Index error: object number invalid.\n");
          return(PROMPT);  
        }       

	index=cmnd->val[2];

	sprintf(file, "%s/o%02d", get_object_path(), index/OFILESIZE);
	if ( file_exists( file ) )
	{
		output(fd, "Object might already exist.\n");
	}

	/* save some pointers */
	rom = obj_ptr->parent_rom;
	po  = obj_ptr->parent_obj;
	fo  = obj_ptr->first_obj;
	pc  = obj_ptr->parent_crt;

	/* now clear them for the save */
	obj_ptr->parent_rom = 0;
	obj_ptr->parent_obj = 0;
	obj_ptr->first_obj = 0;
	obj_ptr->parent_crt = 0;    


	if ( save_obj_to_file( index, obj_ptr ) != 0 )
	{
		elog_broad( "Error saving object in dm_save_obj()");
		output(fd, "Error: object was not saved.\n");
	}
	else
	{
		sprintf(g_buffer, "Object #%d updated.\n", index);
		output(fd, g_buffer);
	}


	/* swap this new object if its in the queue */
	replace_obj_in_queue(index, obj_ptr );


	/* and restore the pointers */
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
/**********************************************************************/
/*				dm_add_talk			      */
/**********************************************************************/

int dm_add_talk(creature *ply_ptr, cmd *cmnd )
{
  ttag     *talk;
  int      fd;
   
  fd = ply_ptr->fd;
   
  if(ply_ptr->first_tlk)
     {
       output(fd,"You have a talk defined already.\n");
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
  output(fd,"Talk ready for input.\n");
   
  return(PROMPT);
}


/**********************************************************************/
/*				dm_add_talk_key			      */
/**********************************************************************/

int dm_add_talk_key(creature *ply_ptr, cmd *cmnd )
{
  int      fd;
  char     *ptr;
   
  fd = ply_ptr->fd;
   
  if(!ply_ptr->first_tlk)
     {
       output(fd,"You must start a talk first.\n");
       return(PROMPT);
     }
   
  if(ply_ptr->first_tlk->key)
   {  
      output(fd,"Keyword replaced.\n");
      free(ply_ptr->first_tlk->key);
   }
   else
      output(fd,"Keyword set\n");

   ptr = cmnd->fullstr;
   for(;*ptr == ' ';ptr++);
   for(;*ptr != ' ' && *ptr;ptr++);
   for(;*ptr == ' ' && *ptr;ptr++);
   if(!*ptr)
     {
       output(fd,"*tkey <keyword [action] [action args]>");
       return 0;
     }
   ply_ptr->first_tlk->key = (char *)malloc(strlen(ptr)+1);
   if(!ply_ptr->first_tlk->key)
     merror("dm_add_talk_key",FATAL);
   strcpy(ply_ptr->first_tlk->key,ptr);
   return(PROMPT);
}


/**********************************************************************/
/*				dm_add_talk_resp			      */
/**********************************************************************/
int dm_add_talk_resp(creature *ply_ptr, cmd *cmnd )
{
  int      fd,append;
  char     *ptr;
  char     buf[1024];
   
  fd = ply_ptr->fd;
   
  if(!ply_ptr->first_tlk)
     {
       output(fd,"You must start a talk first.\n");
       return(PROMPT);
     }
  
  ptr = cmnd->fullstr;
  for(;*ptr == ' ';ptr++);
  for(;;ptr++)
     {
       if(!*ptr)
	  {
           output(fd,"*tres +/- <line>\n");
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


/**********************************************************************/
/*				dm_add_talk_show			      */
/**********************************************************************/
int dm_add_talk_show(creature *ply_ptr, cmd *cmnd )
{
	int		fd;
   
  fd = ply_ptr->fd;
   
	if(!ply_ptr->first_tlk) {
       output(fd,"You must start a talk first.\n");
       return(PROMPT);
     }

	if(ply_ptr->first_tlk->key) {
		output(fd,"Keyword: ");
        output_nl(fd,ply_ptr->first_tlk->key);
     }
	if(ply_ptr->first_tlk->response) {
		output(fd,"Response:\n");
        output_nl(fd,ply_ptr->first_tlk->response);
     }
	return(0);
}

/**********************************************************************/
/*				dm_add_talk_save			      */
/**********************************************************************/
int dm_add_talk_save(creature *ply_ptr, cmd *cmnd )
{
  int      fd,i;
  char     file[256];
  char     crt_name[80];
  FILE     *fp;
  creature *crt_ptr;
   
   
  fd = ply_ptr->fd;
   
  if(!ply_ptr->first_tlk)
     {
       output(fd,"You must start a talk first.\n");
       return(PROMPT);
     }
  if(!ply_ptr->first_tlk->key)
     {
       output(fd,"You must define a keyword.\n");
       return(PROMPT);
     }
  if(!ply_ptr->first_tlk->response)
     {
       output(fd,"You must supply a response.\n");
       return(PROMPT);
     }
  cmnd->val[1]= 1;
  crt_ptr = find_crt(ply_ptr,ply_ptr->parent_rom->first_mon, cmnd->str[1],
                           cmnd->val[1]);    
 
  if (!crt_ptr)
   {
     output(fd,"Creature not found in the room.\n");
     return(PROMPT);
   }  

   strcpy(crt_name,crt_ptr->name);
   for(i=0;crt_name[i];i++)
     if(crt_name[i] == ' ')
        crt_name[i] = '_';
  sprintf(file,"%s/talk/%s-%d", get_monster_path(), crt_name,crt_ptr->level);
  fp = fopen(file,"a");
  fprintf(fp,"%s\n",ply_ptr->first_tlk->key);
  fprintf(fp,"%s\n",ply_ptr->first_tlk->response);
  fclose(fp);
  output(fd,"Talk saved.\n");
  return 0;
   
}

/***************************************************************************
*	This function finds an object or creature number from the
*	database.
*/

int dm_find_db(creature	*ply_ptr, cmd *cmnd )
{
	int fd, n;
	object		*obj_ptr;
	creature	*crt_ptr;

	if(ply_ptr->class < DM)
		return(PROMPT);

	fd=ply_ptr->fd;

	if(cmnd->num < 3) {
		output(fd, "Syntax: *find o|c name [#]\n");
		return(0);
	}

	switch(low(cmnd->str[1][0])) {
		case 'o':
			obj_ptr=find_obj(ply_ptr, ply_ptr->first_obj, cmnd->str[2], cmnd->val[2]);	
			if(!obj_ptr)
				obj_ptr=find_obj(ply_ptr, ply_ptr->parent_rom->first_obj, cmnd->str[2], cmnd->val[2]);
			if(!obj_ptr) {
				output(fd, "Cannot locate object.\n");
				return(0);
			}
			n=find_obj_num(obj_ptr);
			if(n)
			{
				sprintf(g_buffer, "Object is #%d.\n", n);
				output(fd, g_buffer);
			}
			else
				output(fd, "Object is unique.\n");

			break;	
		case 'm':
		case 'c':
			crt_ptr=find_crt(ply_ptr, ply_ptr->parent_rom->first_mon, cmnd->str[2], cmnd->val[2]);
			if(!crt_ptr) {
				output(fd, "Cannot locate creature.\n");
				return(0);
			}
			n=find_crt_num(crt_ptr);
			if(n)
			{
	            sprintf(g_buffer, "Creature is #%d.\n", n);
				output(fd, g_buffer);
			}
            else
                output(fd, "Creature is unique.\n");
        
                        break;

		default:
			output(fd, "Syntax: *find o|c name [#]\n");
			return(0);
	}

	return(0);
}

/*************************************************************************
*  This function allows the deletion of the database.
*
*/
int dm_delete_db(creature *ply_ptr, cmd *cmnd )
{
	int 		fd, ff, index,n;
	creature	*crt_ptr;
	object		*obj_ptr;
	char      	file[80];

	if(ply_ptr->class < DM)
		return(PROMPT);

	fd=ply_ptr->fd;

	if(cmnd->num < 2) 
	{
		output(fd, "Syntax: *clear [o|c] [index]\n");
		return(0);
	}

	if(!strcmp(cmnd->str[1], "c") || !strcmp(cmnd->str[1], "m")) 
	{
		if(cmnd->val[1] < 2 || cmnd->val[1] > CMAX) 
		{
                	output(fd, "Index error.\n");
                	return(0);
		}
	
		index = cmnd->val[1];
		
		sprintf(file, "%s/m%02d", get_monster_path(), index/MFILESIZE);
       	ff = open(file, O_WRONLY | O_BINARY, ACC);
       	if(ff == -1) 
		{
                close(ff);
                output(fd, "File open error.\n");
                return(0);
        }
        	
		crt_ptr=(creature *)malloc(sizeof(creature));
		zero(crt_ptr, sizeof(creature));
		n = lseek(ff, (long)((index%MFILESIZE)*sizeof(creature)), 0);
       	n = write(ff, crt_ptr, sizeof(creature));
       	if(n < sizeof(creature))
           	merror("dm_delete_db", FATAL);
 
       	close(ff);
		sprintf(g_buffer,"Creature #%d deleted.\n", index);
		output(fd, g_buffer);
		return(0);
	}
	else 
	{
		if(strcmp(cmnd->str[1], "o")) 
		{	
            output(fd, "Syntax: *clear [o|c] [index].\n");
            return(0);
		}
		if(cmnd->val[1] < 2 || cmnd->val[1] > OMAX) {
                output(fd, "Index error.\n");
                return(0);
        }
	
		index = cmnd->val[1];
                
        sprintf(file, "%s/o%02d", get_object_path(), index/OFILESIZE);
        ff = open(file, O_WRONLY | O_BINARY);
        if(ff == -1) {
			close(ff);
            output(fd, "File open error.\n");
            return(0);
        }

		obj_ptr=(object *)malloc(sizeof(object));
        zero(obj_ptr, sizeof(object));
        n = lseek(ff, (long)((index%OFILESIZE)*sizeof(object)), 0);
        n = write(ff, obj_ptr, sizeof(object));
        if(n < sizeof(object))
			merror("dm_delete_db", FATAL);
 
        close(ff);
        sprintf(g_buffer, "Object #%d deleted.\n", index);
        output(fd, g_buffer);
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

	if(ply_ptr->class < CARETAKER )
		return(PROMPT);

	if(cmnd->num == 2 && !strcmp(cmnd->str[1], "r")) {
		i=readcf();
		output(ply_ptr->fd, "mordor.cf Reloaded.\n");
		return(0);
	}
	
	fd=ply_ptr->fd;

	ANSI(fd, AFC_BLUE);
	output(fd,"\n\nGame Variable Status\n");
	ANSI(fd, AM_BOLD);
	ANSI(fd, AFC_CYAN);
	output(fd,"Game Title: ");
	ANSI(fd, AFC_WHITE);
	output_nl(fd, title);
	ANSI(fd, AFC_CYAN);
	output(fd,"Game Port : ");
	ANSI(fd, AFC_WHITE);
	sprintf(g_buffer,"%u\n\n", (int)PORTNUM);
	output(fd, g_buffer);
	ANSI(fd, AFC_CYAN);
	output(fd,"DMs here are: ");
	ANSI(fd, AFC_WHITE);
	sprintf(g_buffer,"%s, %s, %s, %s\n", dmname[0], dmname[1], dmname[2], dmname[3]);
	output(fd, g_buffer);
	sprintf(g_buffer,"            : %s, %s, %s\n", dmname[4], dmname[5], dmname[6]);
	output(fd, g_buffer);
	ANSI(fd, AFC_CYAN);
	output(fd,"Questions email address    : ");
	ANSI(fd, AFC_WHITE);
	output_nl(fd, questions_to_email);
	ANSI(fd, AFC_CYAN);
	output(fd,"Authorization email address: ");
	ANSI(fd, AFC_WHITE);
	output_nl(fd, auth_questions_email);
	ANSI(fd, AFC_CYAN);
	output(fd,"Registrations email address: ");
	ANSI(fd, AFC_WHITE);
	output_nl(fd, register_questions_email);
	ANSI(fd, AFC_CYAN);
	output(fd,"Account exists message: ");
	ANSI(fd, AFC_WHITE);
	output_nl(fd, account_exists);
	ANSI(fd, AFC_CYAN);
	output(fd, "\nInitial Equipment item #'s: ");
	sprintf(g_buffer, "%d %d %d %d : %d %d %d %d %d", init_eq[0], init_eq[1], 
			init_eq[2], init_eq[3], init_eq[4], init_eq[5], init_eq[6],
			init_eq[7], init_eq[8]);
	ANSI(fd, AFC_WHITE);
	output_nl(fd, g_buffer);
	ANSI(fd, AFC_CYAN);
	output(fd, "Trophy Item Number:         ");
	sprintf(g_buffer, "%d", trophy_num);
	ANSI(fd, AFC_WHITE);
	output_nl(fd, g_buffer);

	ANSI(fd, AFC_CYAN);
        output(fd, "Shop Creation cost:         ");
        sprintf(g_buffer, "%ld", shop_cost);
        ANSI(fd, AFC_WHITE);
        output_nl(fd, g_buffer);

	
	ANSI(fd, AM_NORMAL);
	ANSI(fd, AFC_BLUE);
	output(fd, "\nOn/Off Settings (1=On, 0=Off)\n");
	ANSI(fd, AM_BOLD);
	ANSI(fd, AFC_CYAN);
	output(fd, "Ansi Bottom Line = ");
	ANSI(fd, AFC_WHITE);
	sprintf(g_buffer, "%d",ANSILINE);
	output(fd, g_buffer);
	ANSI(fd, AFC_CYAN);
	output(fd, ",  AutoShutDown = ");
	ANSI(fd, AFC_WHITE);
	sprintf(g_buffer, "%d",AUTOSHUTDOWN);
	output(fd, g_buffer);
	ANSI(fd, AFC_CYAN);
	output(fd, ",  Checkdouble  = ");
	ANSI(fd, AFC_WHITE);
	sprintf(g_buffer, "%d\n",CHECKDOUBLE);
	output(fd, g_buffer);
	ANSI(fd, AFC_CYAN);
	output(fd, "Crashtrap        = ");
	ANSI(fd, AFC_WHITE);
	sprintf(g_buffer, "%d",CRASHTRAP);
	output(fd, g_buffer);
	ANSI(fd, AFC_CYAN);
	output(fd, ",  EatnDrink    = ");
	ANSI(fd, AFC_WHITE);
	sprintf(g_buffer, "%d",EATNDRINK);
	output(fd, g_buffer);
	ANSI(fd, AFC_CYAN);
	output(fd, ",  GetHostByName= ");
	ANSI(fd, AFC_WHITE);
	sprintf(g_buffer, "%d\n",GETHOSTBYNAME);
	output(fd, g_buffer);
	ANSI(fd, AFC_CYAN);
	output(fd, "Hashrooms        = ");
	ANSI(fd, AFC_WHITE);
	sprintf(g_buffer, "%d",get_hash_rooms());
	output(fd, g_buffer);
	ANSI(fd, AFC_CYAN);
	output(fd, ",  Heaven       = ");
	ANSI(fd, AFC_WHITE);
	sprintf(g_buffer, "%d",HEAVEN);
	output(fd, g_buffer);
	ANSI(fd, AFC_CYAN);
	output(fd, ",  Isengard     = ");
	ANSI(fd, AFC_WHITE);
	sprintf(g_buffer, "%d\n",ISENGARD);
	output(fd, g_buffer);
	ANSI(fd, AFC_CYAN);
	output(fd, "LastCommand      = ");
	ANSI(fd, AFC_WHITE);
	sprintf(g_buffer, "%d",LASTCOMMAND);
	output(fd, g_buffer);
	ANSI(fd, AFC_CYAN);
	output(fd, ",  GuildExp     = ");
	ANSI(fd, AFC_WHITE);
	sprintf(g_buffer, "%d",GUILDEXP);
	output(fd, g_buffer);
	ANSI(fd, AFC_CYAN);
	output(fd, ",  Paranoid     = ");
	ANSI(fd, AFC_WHITE);
	sprintf(g_buffer, "%d\n",PARANOID);
	output(fd, g_buffer);
	ANSI(fd, AFC_CYAN);
	output(fd, "Record_All       = ");
	ANSI(fd, AFC_WHITE);
	sprintf(g_buffer, "%d",RECORD_ALL);
	output(fd, g_buffer);
	ANSI(fd, AFC_CYAN);
	output(fd, ",  RFC1413      = ");
	ANSI(fd, AFC_WHITE);
	sprintf(g_buffer, "%d",RFC1413);
	output(fd, g_buffer);
	ANSI(fd, AFC_CYAN);
	output(fd, ",  Secure       = ");
	ANSI(fd, AFC_WHITE);
	sprintf(g_buffer, "%d\n",SECURE);
	output(fd, g_buffer);
	ANSI(fd, AFC_CYAN);
	output(fd, "Sched            = ");
	ANSI(fd, AFC_WHITE);
	sprintf(g_buffer, "%d",SCHED);
	output(fd, g_buffer);
	ANSI(fd, AFC_CYAN);
	output(fd, ",  Suicide      = ");
	ANSI(fd, AFC_WHITE);
	sprintf(g_buffer, "%d",SUICIDE);
	output(fd, g_buffer);
	ANSI(fd, AFC_CYAN);
	output(fd, ",  Save_on_drop = ");
	ANSI(fd, AFC_WHITE);
	sprintf(g_buffer, "%d\n", SAVEONDROP);
	output(fd, g_buffer);
	ANSI(fd, AFC_CYAN);
	output(fd, "NoBully's        = ");
	ANSI(fd, AFC_WHITE);
	sprintf(g_buffer, "%d", NOBULLYS);	
	output(fd, g_buffer);
	ANSI(fd, AFC_CYAN);
	output(fd, ",  MudSound     = ");
	ANSI(fd, AFC_WHITE);
	sprintf(g_buffer, "%d",MSP);
	output(fd, g_buffer);
	ANSI(fd, AFC_CYAN);
        output(fd, ",  CharCreateOff= ");
        ANSI(fd, AFC_WHITE);
        sprintf(g_buffer, "%d",NOCREATE);
        output(fd, g_buffer);
	ANSI(fd, AM_NORMAL);
	ANSI(fd, AFC_WHITE);

	F_SET(ply_ptr, PREADI);
	ask_for(fd, "[Hit Return, Q to quit]: ");
	output_ply_buf(fd);
    Ply[fd].io->intrpt &= ~1; 
	Ply[fd].io->fn = gamestat2;
	Ply[fd].io->fnparam = 1;
	return(DOPROMPT);

}

/**********************************************************************/
/*				gamestat2			      */
/**********************************************************************/

void gamestat2(int fd, int param,char *instr)
{

	if(instr[0]) {
		output(fd, "Aborted.\n");
		F_CLR(Ply[fd].ply, PREADI);
		RETURN(fd, command, 1);
	}

	ANSI(fd, AFC_BLUE);
	output(fd,"\nSailing Messages\n");
	ANSI(fd, AM_BOLD); 
	ANSI(fd, AFC_CYAN);
	output(fd, "TXmesg1: "); 
	ANSI(fd, AFC_WHITE);
	output_nl(fd, tx_mesg1); 
	ANSI(fd, AFC_CYAN);
	output(fd, "TXmesg2: "); 
	ANSI(fd, AFC_WHITE);
	output_nl(fd, tx_mesg2);

	ANSI(fd, AFC_BLUE);
	output(fd,"Weather Strings\n");
	ANSI(fd, AM_BOLD);
	ANSI(fd, AFC_CYAN);
	output(fd,"Sunrise: ");
	ANSI(fd, AFC_WHITE);
	output_nl(fd, sunrise);
	ANSI(fd, AFC_CYAN);
	output(fd,"Sunset: ");
	ANSI(fd, AFC_WHITE);
	output_nl(fd, sunset);
	ANSI(fd, AFC_CYAN);
	output(fd,"Earth_Trembles: ");
	ANSI(fd, AFC_WHITE);
	output_nl(fd, earth_trembles);
	ANSI(fd, AFC_CYAN);
	output(fd,"Heavy_fog: ");
	ANSI(fd, AFC_WHITE);
	output_nl(fd, heavy_fog);
	ANSI(fd, AFC_CYAN);
	output(fd,"Beautiful_day: ");
	ANSI(fd, AFC_WHITE);
	output_nl(fd, beautiful_day);
	ANSI(fd, AFC_CYAN);
	output(fd,"Bright_sun: ");
	ANSI(fd, AFC_WHITE);
	output_nl(fd, bright_sun);
	ANSI(fd, AFC_CYAN);
	output(fd,"Glaring Sun: ");
	ANSI(fd, AFC_WHITE);
	output_nl(fd, glaring_sun);
	ANSI(fd, AFC_CYAN);
	output(fd,"Heat: ");
	ANSI(fd, AFC_WHITE);
	output_nl(fd, heat);
	ANSI(fd, AFC_CYAN);
	output(fd,"Still: ");
	ANSI(fd, AFC_WHITE);
	output_nl(fd, still);
	ANSI(fd, AFC_CYAN);
	output(fd,"Light_breeze: ");
	ANSI(fd, AFC_WHITE);
	output_nl(fd, light_breeze);	
	ANSI(fd, AFC_CYAN);
	output(fd,"Strong_wind: ");
	ANSI(fd, AFC_WHITE);
	output_nl(fd, strong_wind);
	ANSI(fd, AFC_CYAN);
	output(fd,"Wind_gusts: ");
	ANSI(fd, AFC_WHITE);
	output_nl(fd, wind_gusts);
	ANSI(fd, AFC_CYAN);
	output(fd,"Gale_force: ");
	ANSI(fd, AFC_WHITE);
	output_nl(fd, gale_force);
	ANSI(fd, AFC_CYAN);
	output(fd,"Clear Skies: ");
	ANSI(fd, AFC_WHITE);
	output_nl(fd, clear_skies);
	ANSI(fd, AFC_CYAN);
	output(fd,"Light_clouds: ");
	ANSI(fd, AFC_WHITE);
	output_nl(fd, light_clouds);
	ANSI(fd, AFC_CYAN);
	output(fd,"Thunderheads: ");
	ANSI(fd, AFC_WHITE);
	output_nl(fd, thunderheads);
	ANSI(fd, AFC_CYAN);
	output(fd,"Light_rain : ");
	ANSI(fd, AFC_WHITE);
	output_nl(fd, light_rain);
	ANSI(fd, AFC_CYAN);
	output(fd,"Heavy_rain: ");
	ANSI(fd, AFC_WHITE);
	output_nl(fd, heavy_rain);
	ANSI(fd, AFC_CYAN);
	output(fd,"Sheets_rain: ");
	ANSI(fd, AFC_WHITE);
	output_nl(fd, sheets_rain);
	ANSI(fd, AFC_CYAN);
	output(fd,"Torrent_rain: ");
	ANSI(fd, AFC_WHITE);
	output_nl(fd, torrent_rain);
	ANSI(fd, AFC_CYAN);
	output(fd,"No_moon: ");
	ANSI(fd, AFC_WHITE);
	output_nl(fd, no_moon);
	ANSI(fd, AFC_CYAN);
	output(fd,"Sliver_moon: ");
	ANSI(fd, AFC_WHITE);
	output_nl(fd, sliver_moon);
	ANSI(fd, AFC_CYAN);
	output(fd,"Half_moon: ");
	ANSI(fd, AFC_WHITE);
	output_nl(fd, half_moon);
	ANSI(fd, AFC_CYAN);
	output(fd,"Waxing_moon: ");
	ANSI(fd, AFC_WHITE);
	output_nl(fd, waxing_moon);
	ANSI(fd, AFC_CYAN);
	output(fd,"Full_moon: ");
	ANSI(fd, AFC_WHITE);
	output_nl(fd, full_moon);
	ANSI(fd, AM_NORMAL);
	ANSI(fd, AFC_WHITE);

	F_CLR(Ply[fd].ply, PREADI);
	RETURN(fd, command, 1);

}
/************************************************************************/
/*				dm_advance				*/
/************************************************************************/
int dm_advance(creature	*ply_ptr, cmd *cmnd )
{
   	int 		fd, lev, i;
	creature 	*crt_ptr=0;
	 
    	fd = ply_ptr->fd;
 
    	if (ply_ptr->class < DM)
        	return(PROMPT);

    	if (cmnd->num < 2) {
    		output(fd, "Advance who at what level?\n");
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
                output(fd, "Advance who?\n");
                return(0);
	}
	if(cmnd->val[1] <= 0 || cmnd->val[1] > MAXALVL) {
		output(fd, "Only levels between 1 and 25!\n");
		return(0);
	}
	lev = cmnd->val[1] - crt_ptr->level;
	if(lev == 0) {
		sprintf(g_buffer, "But %%M is already level %d!\n", crt_ptr->level);
		mprint(fd, g_buffer, m1arg(crt_ptr));
		return(0);
	}
	if(lev > 0) {
		for(i=0; i<lev; i++) 
			up_level(crt_ptr);

		sprintf(g_buffer, "%%M has been raised to level %d.\n", crt_ptr->level);
		mprint(fd, g_buffer, m1arg(crt_ptr));
	}
	if(lev < 0) {
		for(i=0; i>lev; i--) 
			down_level(crt_ptr);

		sprintf(g_buffer, "%%M has been lowered to level %d.\n", crt_ptr->level);
		mprint(fd, g_buffer, m1arg(crt_ptr));
	}

	return(0);
}



/************************************************************************/
/*				dm_dmalloc_stats				*/
/*  calls the dmalloc_log_stats if dmalloc was compiled in */
/************************************************************************/
int dm_dmalloc_stats(creature *ply_ptr, cmd *cmnd )
{
   	int 		fd;

	ASSERTLOG( ply_ptr );
	 
   	fd = ply_ptr->fd;
 
   	if (ply_ptr->class < DM)
       	return(PROMPT);


#ifdef DMALLOC
	dmalloc_log_stats();
	output( fd, "DMALLOC log stats were dumped.\n");
#else
	output( fd, "DMALLOC was not compiled in.\n");
#endif /* DMALLOC */

	return(0);
}


/************************************************************************/
/*		dm_plist						*/
