/*
 * COMMAND13.C:
 * 
 *   Additional user routines:
 *		int channelemote(ply_ptr, cmnd)
 *				emote to entire channel (i.e. broadcast emote)
 *		int classemote(ply_ptr, cmnd)
 *				emote to same class (i.e. broadcast emote to class)
 *		int class_send(ply_ptr, cmnd)
 *				send broadcast message to same class
 *
 *		int prep_herb(ply_ptr,cmnd)
 *		int apply_herb(ply_ptr,cmnd)
 *		int ingest_herb(ply_ptr,cmnd)
 *		int eat_herb(ply_ptr,cmnd)
 *		int paste_herb(ply_ptr,cmnd)
 *		int use_herb(ply_ptr,cmnd)
 *              int eat(ply_ptr,cmnd)
 *              int describe_me(ply_ptr,cmnd)
 *
 *	(c) 1996-1997  Brooke Paul
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
/*				channelemote				  */
/******************************************************************/

/* This command allows a player to echo a message unaccompanied by */
/* any message format, except for the players name at the beginning */

int channelemote(ply_ptr, cmnd)
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
	if(!dec_daily(&ply_ptr->daily[DL_BROAE])) {
                print(fd,"You've used up all your broadcasts today.\n");                        
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

	ANSI(fd, YELLOW);
	broadcast("### %M %s.", ply_ptr, &cmnd->fullstr[index]);
	ANSI(fd, WHITE);

	return(0);
}
/******************************************************************/
/*				classemote										  */
/******************************************************************/

/* This command allows a player to echo a message unaccompanied by */
/* any message format, except for the players name at the beginning */

int classemote(ply_ptr, cmnd)
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
		broadcast_class(ply_ptr, "### %s %s.", ply_ptr->name, &cmnd->fullstr[index]);
		broadcast_eaves("--- (%s): %s %s.", class_str[ply_ptr->class], ply_ptr->name, &cmnd->fullstr[index]);
	return(0);
}

/**********************************************************************/
/*				class_send				      */
/**********************************************************************/

/* This function allows classes to send messages that only they can see. */
/* It is similar to a broadcast, but there are no limits.	     */

int class_send(ply_ptr, cmnd)
creature	*ply_ptr;
cmd			*cmnd;
{
	int	i, fd, found = 0;

	fd = ply_ptr->fd;

	for(i=0; i<strlen(cmnd->fullstr); i++) {
		if(cmnd->fullstr[i] == ' ' && cmnd->fullstr[i+1] != ' ') {
			found++;
			break;
		}
	}

	if(found < 1 || strlen(&cmnd->fullstr[i+1]) < 1) {
		print(fd, "Send what?\n");
		return(0);
	}

	print(fd, "Ok.\n");
	broadcast_class(ply_ptr, "### %s sent, \"%s\".", ply_ptr->name, &cmnd->fullstr[i+1]);
	broadcast_eaves("--- %s class sent, \"%s\".", ply_ptr->name, &cmnd->fullstr[i+1]);
	return(0);

}


int prep_herb(ply_ptr,cmnd)
creature *ply_ptr;
cmd      *cmnd;
{
  object *obj_ptr;
  long   t;

  if(ply_ptr->class != DRUID && ply_ptr->class < CARETAKER) {
		prepare(ply_ptr, cmnd);
		return(0);
  }   
  if(cmnd->num < 2)
     {
      print(ply_ptr->fd,"Prepare what herb?\n");
      return(0);
     }
   
  t = time(0);
  if(ply_ptr->lasttime[LT_PREPN].interval+ply_ptr->lasttime[LT_PREPN].ltime > t)
   {
     print(ply_ptr->fd,"You cannot prepare any more herbs right now.\n");
     return(0);
   }
   if(LT(ply_ptr,LT_ATTCK) > t)
   {
     print(ply_ptr->fd,"You cannot prepare herbs while fighting!\n");
   }

   obj_ptr = ply_ptr->ready[HELD-1];
   if(!obj_ptr)
   {
     print(ply_ptr->fd,"You aren't holding an herb.\n");
     return(0);
   }
   if(obj_ptr->type != HERB)
   {
     print(ply_ptr->fd,"%s is not an herb.\n",obj_ptr);
     return(0);
   }
   if(ply_ptr->ready[WIELD-1]) {
	print(ply_ptr->fd, "You're hands are too full to do that.\n");
	return(0);
   }
   if(obj_ptr->shotscur < 1) {
	print(ply_ptr->fd, "That herb is used up.\n");
	return(0);
   }   
   F_SET(obj_ptr, ONPREP);
   F_SET(ply_ptr, PPREPN);
   ply_ptr->lasttime[LT_PREPN].ltime = t; 

   if(F_ISSET(obj_ptr,OHBREW)) 
   {	
     print(ply_ptr->fd,"You begin to brew the %s for drinking.\n",obj_ptr);
     broadcast_rom(ply_ptr->fd,ply_ptr->rom_num,
		   "%M begins to brew a %s for drinking.",ply_ptr,obj_ptr->name);
     ply_ptr->lasttime[LT_PREPN].interval=60L;

   }
   if(F_ISSET(obj_ptr,OHNGST))
   {	
     print(ply_ptr->fd,"You begin to prepare a %s for eating.\n",obj_ptr->name);
     broadcast_rom(ply_ptr->fd,ply_ptr->rom_num,
		   "%M begins to prepare a %s for eating.",ply_ptr,obj_ptr->name);
     ply_ptr->lasttime[LT_PREPN].interval=10L;
   }
   if(F_ISSET(obj_ptr,OHAPLY))
   {	
     print(ply_ptr->fd,"You begin to soften a %s into a cream.\n",obj_ptr->name);
     broadcast_rom(ply_ptr->fd,ply_ptr->rom_num,
		   "%M begins to soften a %s into a cream.",ply_ptr,obj_ptr->name);
     ply_ptr->lasttime[LT_PREPN].interval=10L;
   }
   if(F_ISSET(obj_ptr,OHPAST))
   {	
     print(ply_ptr->fd,"You begin to mix a %s into a paste.\n",obj_ptr->name);
     broadcast_rom(ply_ptr->fd,ply_ptr->rom_num,
		   "%M begins to mix a %s into a paste.",ply_ptr,obj_ptr->name);
     ply_ptr->lasttime[LT_PREPN].interval=15L;
   }
   return(PROMPT);
}

int apply_herb(ply_ptr,cmnd)
creature *ply_ptr;
cmd      *cmnd;
{
  object *obj_ptr;
  long   t;
   
	if(cmnd->num < 2) {
		print(ply_ptr->fd, "Apply what?\n");
		return(0);
	}

  t = time(0);
  if(LT(ply_ptr,LT_PREPN) > t)
   {
     print(ply_ptr->fd,"You are still preparing the herb.\n");
     return(0);
   }
  if(LT(ply_ptr,LT_ATTCK) > t)
   {
     print(ply_ptr->fd,"You can not apply herbs in combat!\n");
     return(0);
   }

   obj_ptr = find_obj(ply_ptr,ply_ptr->first_obj,cmnd->str[1],cmnd->val[1]);
   if(!obj_ptr)
   {
     print(ply_ptr->fd,"You don't seem to have the %s.\n",cmnd->str[1]);
     return(0);
   }
   if(obj_ptr->type != HERB)
   {
     print(ply_ptr->fd,"That is not an herb.\n");
     return(0);
   }
   if(!F_ISSET(obj_ptr, ODPREP)) {
	print(ply_ptr->fd, "The %s has not been prepared properly.\n", obj_ptr->name);
	return(0);
   }
   if(!F_ISSET(obj_ptr,OHAPLY))
   {
     print(ply_ptr->fd,"You cannot apply %s.\n",obj_ptr->name);
     return(0);
   }
   print(ply_ptr->fd,"You apply the herb to your skin.\n");
   broadcast_rom(ply_ptr->fd, ply_ptr->rom_num, "%M applies some %s.", 
						ply_ptr, obj_ptr->name);
   use_herb(ply_ptr,obj_ptr,obj_ptr->special);
   return(PROMPT);
}

int ingest_herb(ply_ptr,cmnd)
creature *ply_ptr;
cmd      *cmnd;
{
  object *obj_ptr;
  long   t;
   
	if(cmnd->num < 2) {
                print(ply_ptr->fd, "Ingest what?\n");
                return(0);
        }

  t = time(0);
  if(LT(ply_ptr,LT_PREPN) > t)
   {
     print(ply_ptr->fd,"You are still preparing the herb.\n");
     return(0);
   }
 

   obj_ptr = find_obj(ply_ptr,ply_ptr->first_obj,cmnd->str[1],cmnd->val[1]);
   if(!obj_ptr)
   {
     print(ply_ptr->fd,"You don't have that.\n");
     return(0);
   }
   if(obj_ptr->type != HERB)
   {
     print(ply_ptr->fd,"That is not an herb.\n");
     return(0);
   }
   if(!F_ISSET(obj_ptr, ODPREP)) {
        print(ply_ptr->fd, "The %s has not been prepared properly.\n", obj_ptr->name);
        return(0);
   }
   if(!F_ISSET(obj_ptr,OHBREW))
   {
     print(ply_ptr->fd,"You cannot drink a %s.\n",obj_ptr->name);
     return(0);
   }
   print(ply_ptr->fd,"You drink the herb's broth.\n");
   broadcast_rom(ply_ptr->fd, ply_ptr->rom_num, "%M drinks some %s.",
                                                ply_ptr, obj_ptr->name);
   use_herb(ply_ptr,obj_ptr,obj_ptr->special);
   return(PROMPT);
}

int eat_herb(ply_ptr,cmnd)
creature *ply_ptr;
cmd      *cmnd;
{
  object *obj_ptr;
  long   t;
   
	if(cmnd->num < 2) {
                print(ply_ptr->fd, "Eat what?\n");
                return(0);
        }

  t = time(0);
  if(LT(ply_ptr,LT_PREPN) > t)
   {
     print(ply_ptr->fd,"You are still preparing the herb.\n");
     return(0);
   }
  

   obj_ptr = find_obj(ply_ptr,ply_ptr->first_obj,cmnd->str[1],cmnd->val[1]);
   if(!obj_ptr)
   {
     print(ply_ptr->fd,"You don't have that.\n");
     return(0);
   }
   if(obj_ptr->type != HERB)
   {
     print(ply_ptr->fd,"That is not an herb.\n");
     return(0);
   }
   if(!F_ISSET(obj_ptr, ODPREP)) {
        print(ply_ptr->fd, "The %s has not been prepared properly.\n", obj_ptr->name);
        return(0);
   }

   if(!F_ISSET(obj_ptr,OHNGST))
   {
     print(ply_ptr->fd,"You cannot eat a %s.\n",obj_ptr->name);
     return(0);
   }
   print(ply_ptr->fd,"You eat the herb.\n");
   broadcast_rom(ply_ptr->fd, ply_ptr->rom_num, "%M eats some %s.",
                                                ply_ptr, obj_ptr->name);
   use_herb(ply_ptr,obj_ptr,obj_ptr->special);
   return(PROMPT);
}

int paste_herb(ply_ptr,cmnd)
creature *ply_ptr;
cmd      *cmnd;
{
  object *obj_ptr;
  long   t;
   
	if(cmnd->num < 2) {
                print(ply_ptr->fd, "Paste what?\n");
                return(0);
        }

  t = time(0);
  if(LT(ply_ptr,LT_PREPN) > t)
   {
     print(ply_ptr->fd,"You are still preparing the herb.\n");
     return(0);
   }
  if(LT(ply_ptr,LT_ATTCK) > t)
   {
     print(ply_ptr->fd,"You cannot apply herbs in combat!\n");
     return(0);
   }

   obj_ptr = find_obj(ply_ptr,ply_ptr->first_obj,cmnd->str[1],cmnd->val[1]);
   if(!obj_ptr)
   {
     print(ply_ptr->fd,"You don't have that.\n");
     return(0);
   }
   if(obj_ptr->type != HERB)
   {
     print(ply_ptr->fd,"That is not an herb.\n");
     return(0);
   }
   if(!F_ISSET(obj_ptr, ODPREP)) {
        print(ply_ptr->fd, "The %s has not been prepared properly.\n", obj_ptr->name);
        return(0);
   }

   if(!F_ISSET(obj_ptr,OHPAST))
   {
     print(ply_ptr->fd,"You cannot apply a %s.\n",obj_ptr->name);
     return(0);
   }
   print(ply_ptr->fd,"You apply the herb.\n");
   broadcast_rom(ply_ptr->fd, ply_ptr->rom_num, "%M applies some %s.",
                                                ply_ptr, obj_ptr->name);
   use_herb(ply_ptr,obj_ptr,obj_ptr->special);
   return(PROMPT);
}


int use_herb(ply_ptr,obj_ptr,herb_type)
creature *ply_ptr;
object   *obj_ptr;
int      herb_type;
{
   int dmg;
   
   switch(herb_type)
     {
      case SP_HERB_HEAL:
	print(ply_ptr->fd,"A warmth flows through your body.\n");
	dmg = mdice(obj_ptr);
	del_obj_crt(obj_ptr, ply_ptr);
	free_obj(obj_ptr);
	ply_ptr->hpcur += dmg;
	if(ply_ptr->hpcur > ply_ptr->hpmax)
	  ply_ptr->hpcur = ply_ptr->hpmax;
	break;
      case SP_HERB_POISON:
	print(ply_ptr->fd,"Your blood begins to burn as the poison spreads!\n");
	F_SET(ply_ptr,PPOISN);
	dmg = mdice(obj_ptr);
	del_obj_crt(obj_ptr, ply_ptr);
	free_obj(obj_ptr);
	ply_ptr->hpcur -= dmg;
	if(ply_ptr->hpcur < 1)
	  die(ply_ptr,ply_ptr);
	break;
      case SP_HERB_HARM:
	print(ply_ptr->fd,"Your body begins to jerk maddly!\n");
	dmg = mdice(obj_ptr);
	del_obj_crt(obj_ptr, ply_ptr);
	free_obj(obj_ptr);
	ply_ptr->hpcur -= dmg;
	if(ply_ptr->hpcur < 1)
	  die(ply_ptr,ply_ptr);
	break;
      case SP_HERB_DISEASE:
	print(ply_ptr->fd,"A puss dripping rash appears on your arms and face.\n");
	F_SET(ply_ptr,PDISEA);
	break;
      case SP_HERB_CURE_POISON:
	print(ply_ptr->fd,"You feel your blood cool.\n");
	F_CLR(ply_ptr,PPOISN);
	del_obj_crt(obj_ptr, ply_ptr);
	free_obj(obj_ptr);
	break;
      case SP_HERB_CURE_DISEASE:
	print(ply_ptr->fd,"A clean feeling flows through your body.\n");
	F_CLR(ply_ptr,PDISEA);
	del_obj_crt(obj_ptr, ply_ptr);
	free_obj(obj_ptr);
	break;
      default:
        print(ply_ptr->fd,"The herb went bad.\n");
	del_obj_crt(obj_ptr, ply_ptr);
	free_obj(obj_ptr);
	break;
     }
     return(PROMPT);
   
}



/****************************************************************************
*  This function handes all attempts to eat any type object.
*
*/
int eat(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{

	object 	*obj_ptr;
	int	fd;

	fd=ply_ptr->fd;

	if(cmnd->num < 2) {
		print(fd, "Eat what?\n");
		return(0);
	}

	obj_ptr=find_obj(ply_ptr, ply_ptr->first_obj, cmnd->str[1], cmnd->val[1]);
	
	if(!obj_ptr)	{
		print(fd, "You dont have that.\n");
		return(0);
	}
	switch(obj_ptr->type) {

		case HERB:
			if(F_ISSET(obj_ptr,OHBREW)) 
				ingest_herb(ply_ptr,cmnd);
			if(F_ISSET(obj_ptr,OHNGST)) 
				eat_herb(ply_ptr,cmnd);			
			break;
		case DRINK:
			if(ply_ptr->talk[5]<1) {
                                print(fd, "You can't drink anymore.\n");
                                break;
                        }
			if(obj_ptr->shotscur){
				obj_ptr->shotscur-=1;			
	                        ply_ptr->talk[5]-=obj_ptr->ndice;
        	                if(ply_ptr->talk[5] < 0)
                	                ply_ptr->talk[5]=0;
				broadcast_rom(fd, ply_ptr->rom_num, "%M drinks from a %s.", ply_ptr, obj_ptr);
                        	print(fd, "Ahh, refreshing!\n");
                        	break;
			}
			else 
				print(fd, "The %s is empty.\n", obj_ptr);
				break;
			
			
		case FOOD:
			if(ply_ptr->talk[6]<1) {
				print(fd, "You cannot eat another bite.\n");
				break;
			}
			if(obj_ptr->shotscur){
                                obj_ptr->shotscur-=1;
				ply_ptr->talk[6]-=obj_ptr->ndice;
				if(ply_ptr->talk[6] < 0)
					ply_ptr->talk[6]=0;
				if(!obj_ptr->shotscur) {			
					del_obj_crt(obj_ptr, ply_ptr);
					free(obj_ptr);	
				}
				broadcast_rom(fd, ply_ptr->rom_num, "%M munches on a %s.", ply_ptr, obj_ptr);
				print(fd, "Yummy!\n");
				break;
			}
			else
				del_obj_crt(obj_ptr, ply_ptr);
				free(obj_ptr);
				print(fd, "I don't see that here.\n");
				break;
		default:
			print(fd, "You can't do that.\n");
	}
	if(ply_ptr->talk[5]<4 && ply_ptr->talk[6]<4)
		F_CLR(ply_ptr, PNSUSN);

	return(0);
}
/****************************************************************************
*  This allows a player to set his/her description that is seen when you
* look at them.
*
*/
int describe_me(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	int fd, i;

	fd=ply_ptr->fd;
	if(cmnd->num < 2) {
		print(fd, "Syntax: description [text|-d]\n.");
		return(0);
	}
	if(!strcmp(cmnd->str[1], "-d")) {
		print(fd, "Description cleared.\n");
		ply_ptr->description[0]=0;
		return(0);
	}
	i=0;
	while(!isspace(cmnd->fullstr[i]))
            i++;
	
	strncpy(ply_ptr->description,&cmnd->fullstr[i+1],78);
        ply_ptr->description[79]=0;
	print(fd, "Description set.\n");
	return(0);
}
