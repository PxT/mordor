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
 *		int transform(ply_ptr,cmnd)
 *		int transmute(ply_ptr,cmnd)
 *		int apply_herb(ply_ptr,cmnd)
 *		int ingest_herb(ply_ptr,cmnd)
 *		int eat_herb(ply_ptr,cmnd)
 *		int paste_herb(ply_ptr,cmnd)
 *		int use_herb(ply_ptr,cmnd)
 *              int eat(ply_ptr,cmnd)
 *              int describe_me(ply_ptr,cmnd)
 *		int scout(ply_ptr,cmnd)
 *
 *	(c) 1996  Brooke Paul, Eric Krichbaum & Paul Telford
 * $Id: command13.c,v 6.16 2001/07/17 19:25:11 develop Exp $
 *
 * $Log: command13.c,v $
 * Revision 6.16  2001/07/17 19:25:11  develop
 * *** empty log message ***
 *
 * Revision 6.15  2001/05/17 02:32:35  develop
 * fixed extra \n in scout output
 *
 * Revision 6.14  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 *
 */

#include "../include/mordb.h"
#include "mextern.h"
#include <sys/types.h>
#include <sys/stat.h>

#include <ctype.h>


/******************************************************************/
/*				channelemote				  */
/******************************************************************/

/* This command allows a player to echo a message unaccompanied by */
/* any message format, except for the players name at the beginning */

int channelemote( creature *ply_ptr, cmd *cmnd )
{
	room		*rom_ptr;
	int		fd;
	char	str[2048];

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	cmnd->fullstr[255] = 0;
	clean_str( cmnd->fullstr, 1);

	if(strlen(cmnd->fullstr) < 1) {
		output(fd, "Emote what?\n");
		return(0);
	}

	if(F_ISSET(ply_ptr, PSILNC)){
		output_wc(fd, "You are unable to do that right now.\n", SILENCECOLOR);
		return(0);
	}

	/* spam check */
	if ( check_for_spam( ply_ptr ) )
	{
		return(0);
	}

	if(!dec_daily(&ply_ptr->daily[DL_BROAE])) {
        output(fd,"You've used up all your broadcasts today.\n");                        
		return(0);
                }

	F_CLR(ply_ptr, PHIDDN);
	if(F_ISSET(ply_ptr, PLECHO)){
		sprintf(g_buffer, "You emote: %s\n", cmnd->fullstr);
		output_wc(fd, g_buffer, ECHOCOLOR);
	}
	else
		output(fd, "Ok.\n");

	strcpy(str, "### %M ");
	strcat(str, cmnd->fullstr);
	broadcast_c(str, ply_ptr);

	return(0);
}
/******************************************************************/
/*				classemote										  */
/******************************************************************/

/* This command allows a player to echo a message unaccompanied by */
/* any message format, except for the players name at the beginning */

int classemote(creature	*ply_ptr, cmd *cmnd )
{
	room	*rom_ptr;
	int		fd;
	
	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

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

	/* spam check */
	if ( check_for_spam( ply_ptr ) )
	{
		return(0);
	}

	F_CLR(ply_ptr, PHIDDN);
	if(F_ISSET(ply_ptr, PLECHO)){
		sprintf(g_buffer, "You emote: %s\n", cmnd->fullstr);
		output_wc(fd, g_buffer, ECHOCOLOR);
	}
	else {
		output(fd, "Ok.\n");
	}

	sprintf(g_buffer, "%s %s.", ply_ptr->name, cmnd->fullstr);
	broadcast_class(ply_ptr, g_buffer);

	if (ply_ptr->class == DM)
		return(0);

	sprintf(g_buffer, "--- (%s): %s %s.", get_class_string(ply_ptr->class), ply_ptr->name, 
		cmnd->fullstr);
	broadcast_eaves(g_buffer);
	return(0);
}

/**********************************************************************/
/*				class_send				      */
/**********************************************************************/

/* This function allows classes to send messages that only they can see. */
/* It is similar to a broadcast, but there are no limits.	     */

int class_send( creature *ply_ptr, cmd *cmnd )
{
	int	fd, found = 0;
	unsigned int i;

	fd = ply_ptr->fd;

	for(i=0; i<strlen(cmnd->fullstr); i++) {
		if(cmnd->fullstr[i] == ' ' && cmnd->fullstr[i+1] != ' ') {
			found++;
			break;
		}
	}

	if(found < 1 || strlen(&cmnd->fullstr[i+1]) < 1) {
		output(fd, "Send what?\n");
		return(0);
	}

	if(F_ISSET(ply_ptr, PSILNC)){
		output_wc(fd, "You are unable to do that right now.\n", SILENCECOLOR);
		return(0);
	}

	/* spam check */
	if ( check_for_spam( ply_ptr ) )
	{
		return(0);
	}

	output(fd, "Ok.\n");

	sprintf(g_buffer, "%s sent, \"%s\".", ply_ptr->name, &cmnd->fullstr[i+1]);
	broadcast_class(ply_ptr, g_buffer);

	if (ply_ptr->class == DM)
		return(0);

	sprintf(g_buffer, "--- %s class sent, \"%s\".", ply_ptr->name, &cmnd->fullstr[i+1]);
	broadcast_eaves(g_buffer);

	return(0);

}


/**********************************************************************/
/*				transform			      */
/**********************************************************************/
int transform( creature *ply_ptr, cmd *cmnd )
{

  object *obj_ptr;
  time_t   t;


  if(ply_ptr->class != DRUID && ply_ptr->class < BUILDER) {
    	        output(ply_ptr->fd,"You don't know how to transform anything.\n");
		return(0);
  }   

  if(cmnd->num < 2)  {
      output(ply_ptr->fd,"Transform what?\n");
      return(0);
  }
   
  t = time(0);

  if(LT(ply_ptr,LT_ATTCK) > t) {
     output(ply_ptr->fd,"You cannot transform while fighting!\n");
     return(0);
  }

  if(ply_ptr->level < 4 && ply_ptr->class < BUILDER) {      
      output(ply_ptr->fd, "You are not experienced enough to do that yet.\n");          
      return(0);    
  }      

  if(!dec_daily(&ply_ptr->daily[DL_CHARM]) && ply_ptr->class < BUILDER) { 
      output(ply_ptr->fd, "You have transformed enough for one day.\n");               
      return(0);          
  }      

  if(F_ISSET(ply_ptr, PBLIND)) {
      output_wc(ply_ptr->fd, "How do you do that?  You're blind.\n", BLINDCOLOR);  
      return(0);          
  }      

  obj_ptr = find_obj(ply_ptr,ply_ptr->first_obj,cmnd->str[1],cmnd->val[1]);
  
  if(!obj_ptr)
  {
        sprintf(g_buffer,"You don't seem to have that.\n");
        output(ply_ptr->fd, g_buffer);
        return 0;
  }

   if(ply_ptr->ready[WIELD-1]) {
	output(ply_ptr->fd, "You're hands are too full to do that.\n");
	return(0);
   }

   if((F_ISSET(obj_ptr, ONOFIX)) || (!F_ISSET(obj_ptr, ODRUDT))) {
	output(ply_ptr->fd, "That cannot be transformed.\n");
	return(0);
   }   

   if(F_ISSET(obj_ptr,ODRUDT)) {	
	F_SET(obj_ptr, ONOFIX);
	F_SET(obj_ptr, OENCHA);
	obj_ptr->shotscur = obj_ptr->shotsmax;
        mprint(ply_ptr->fd,"You transform %i into something useful.\n",
		 m1arg(obj_ptr));
         broadcast_rom(ply_ptr->fd,ply_ptr->rom_num,
		   "%M transforms %i.", 
		   m2args(ply_ptr, obj_ptr));
   }
   return(PROMPT);
}
/**********************************************************************/
/*				transmute			      */
/**********************************************************************/
int transmute( creature *ply_ptr, cmd *cmnd )
{

  object *obj_ptr;
  time_t   t;


  if(ply_ptr->class != ALCHEMIST && ply_ptr->class < BUILDER) {
    	        output(ply_ptr->fd,"You don't know how to transmute anything.\n");
		return(0);
  }   

  if(cmnd->num < 2)  {
      output(ply_ptr->fd,"Transmute what?\n");
      return(0);
  }
   
  t = time(0);

  if(LT(ply_ptr,LT_ATTCK) > t) {
     output(ply_ptr->fd,"You cannot transmute while fighting!\n");
     return(0);
  }

  if(ply_ptr->level < 4 && ply_ptr->class < BUILDER) {
      output(ply_ptr->fd, "You are not experienced enough to do that yet.\n");
      return(0);
  }

  if(!dec_daily(&ply_ptr->daily[DL_CHARM]) && ply_ptr->class < BUILDER) {
      output(ply_ptr->fd, "You have transmuted enough for one day.\n");
      return(0);
  }

  if(F_ISSET(ply_ptr, PBLIND)) {
      output_wc(ply_ptr->fd, "How do you do that?  You're blind.\n", BLINDCOLOR);
      return(0);
  }

  obj_ptr = find_obj(ply_ptr,ply_ptr->first_obj,cmnd->str[1],cmnd->val[1]);

  if(!obj_ptr)
  {
	sprintf(g_buffer,"You don't seem to have that.\n");
	output(ply_ptr->fd, g_buffer);
	return 0;
  }

   if(ply_ptr->ready[WIELD-1]) {
	output(ply_ptr->fd, "You're hands are too full to do that.\n");
	return(0);
   }

   if((F_ISSET(obj_ptr, ONOFIX)) || (!F_ISSET(obj_ptr, OALCHT))) {
	output(ply_ptr->fd, "That cannot be transmuted.\n");
	return(0);
   }   

   if(F_ISSET(obj_ptr,OALCHT)) {	
	F_SET(obj_ptr, ONOFIX);
	F_SET(obj_ptr, OENCHA);
	obj_ptr->shotscur = obj_ptr->shotsmax;
        mprint(ply_ptr->fd,"You transmute %i into something useful.\n",
		 m1arg(obj_ptr));
         broadcast_rom(ply_ptr->fd,ply_ptr->rom_num,
		   "%M transmutes %i.", 
		   m2args(ply_ptr, obj_ptr));
   }
   return(PROMPT);
}



/**********************************************************************/
/*				apply_herb				      */
/**********************************************************************/
int apply_herb( creature *ply_ptr, cmd *cmnd )
{
	object *obj_ptr;
	time_t   t;
   
	if(cmnd->num < 2) {
		output(ply_ptr->fd, "Apply what?\n");
		return(0);
	}

	t = time(0);

	if(LT(ply_ptr,LT_ATTCK) > t)
	{
		output(ply_ptr->fd,"You can not apply herbs in combat!\n");
		return(0);
	}

	obj_ptr = find_obj(ply_ptr,ply_ptr->first_obj,cmnd->str[1],cmnd->val[1]);
	if(!obj_ptr)
	{
		sprintf(g_buffer, "You don't seem to have the %s.\n",cmnd->str[1]);
		output(ply_ptr->fd,g_buffer);
		return(0);
	}
	if(obj_ptr->type != HERB)
	{
		output(ply_ptr->fd,"That is not an herb.\n");
		return(0);
	}
	if(!F_ISSET(obj_ptr,OHAPLY))
	{
		sprintf(g_buffer, "You cannot apply %s.\n",obj_ptr->name);
		output(ply_ptr->fd, g_buffer);
		return(0);
	}
	output(ply_ptr->fd,"You apply the herb to your skin.\n");
	broadcast_rom(ply_ptr->fd, ply_ptr->rom_num, "%M applies %i.", 
						m2args(ply_ptr, obj_ptr));
	use_herb(ply_ptr,obj_ptr,obj_ptr->special);
	return(PROMPT);
}



/**********************************************************************/
/*				ingest_herb				      */
/**********************************************************************/
int ingest_herb( creature *ply_ptr, cmd *cmnd )
{
	object *obj_ptr;
	time_t   t;
   
	if(cmnd->num < 2) {
		output(ply_ptr->fd, "Ingest what?\n");
		return(0);
	}

	t = time(0);

	obj_ptr = find_obj(ply_ptr,ply_ptr->first_obj,cmnd->str[1],cmnd->val[1]);
	if(!obj_ptr)
	{
		output(ply_ptr->fd,"You don't have that.\n");
		return(0);
	}
	if(obj_ptr->type != HERB)
	{
		output(ply_ptr->fd,"That is not an herb.\n");
		return(0);
	}
	if(!F_ISSET(obj_ptr,OHBREW))
	{
		sprintf(g_buffer,"You cannot drink a %s.\n",obj_ptr->name);
		output(ply_ptr->fd, g_buffer);
		return(0);
	}
	output(ply_ptr->fd,"You drink the herb's broth.\n");
	broadcast_rom(ply_ptr->fd, ply_ptr->rom_num, "%M drinks %i.",
		m2args(ply_ptr, obj_ptr));
	use_herb(ply_ptr,obj_ptr,obj_ptr->special);
	return(PROMPT);
}



/**********************************************************************/
/*				eat_herb				      */
/**********************************************************************/
int eat_herb( creature *ply_ptr, cmd *cmnd )
{
	object *obj_ptr;
	time_t   t;
	
	if(cmnd->num < 2) {
		output(ply_ptr->fd, "Eat what?\n");
		return(0);
        }

  t = time(0);

   obj_ptr = find_obj(ply_ptr,ply_ptr->first_obj,cmnd->str[1],cmnd->val[1]);
   if(!obj_ptr)
   {
     output(ply_ptr->fd,"You don't have that.\n");
     return(0);
   }
   if(obj_ptr->type != HERB)
   {
		output(ply_ptr->fd,"That is not an herb.\n");
		return(0);
   }

   if(!F_ISSET(obj_ptr,OHNGST))
   {
		sprintf(g_buffer, "You cannot eat a %s.\n",obj_ptr->name);
		output(ply_ptr->fd, g_buffer);
		return(0);
   }
   output(ply_ptr->fd,"You eat the herb.\n");
   broadcast_rom(ply_ptr->fd, ply_ptr->rom_num, "%M eats %i.",
	   m2args(ply_ptr, obj_ptr));
   use_herb(ply_ptr,obj_ptr,obj_ptr->special);
   return(PROMPT);
}



/**********************************************************************/
/*				paste_herb				      */
/**********************************************************************/
int paste_herb( creature *ply_ptr, cmd *cmnd )
{
	object *obj_ptr;
	time_t   t;

	if(cmnd->num < 2) {
		output(ply_ptr->fd, "Paste what?\n");
		return(0);
	}

	t = time(0);
	if(LT(ply_ptr,LT_ATTCK) > t)
	{
		output(ply_ptr->fd,"You cannot apply herbs in combat!\n");
		return(0);
	}

	obj_ptr = find_obj(ply_ptr,ply_ptr->first_obj,cmnd->str[1],cmnd->val[1]);
	if(!obj_ptr)
	{
		output(ply_ptr->fd,"You don't have that.\n");
		return(0);
	}
	if(obj_ptr->type != HERB)
	{
		output(ply_ptr->fd,"That is not an herb.\n");
		return(0);
	}

	if(!F_ISSET(obj_ptr,OHPAST))
	{
		sprintf(g_buffer, "You cannot apply a %s.\n",obj_ptr->name);
		output(ply_ptr->fd, g_buffer);
		return(0);
	}
	output(ply_ptr->fd,"You apply the herb.\n");
	broadcast_rom(ply_ptr->fd, ply_ptr->rom_num, "%M applies %i.",
		m2args(ply_ptr, obj_ptr));
	use_herb(ply_ptr,obj_ptr,obj_ptr->special);
	return(PROMPT);
}


/**********************************************************************/
/*				use_herb				      */
/**********************************************************************/
int use_herb( creature *ply_ptr, object *obj_ptr, int herb_type )
{
   int dmg;
   
   switch(herb_type)
     {
      case SP_HERB_HEAL:
		output(ply_ptr->fd,"A warmth flows through your body.\n");
		dmg = mdice(obj_ptr);
		del_obj_crt(obj_ptr, ply_ptr);
		free_obj(obj_ptr);
		ply_ptr->hpcur += dmg;
		if(ply_ptr->hpcur > ply_ptr->hpmax)
		  ply_ptr->hpcur = ply_ptr->hpmax;
		break;
      case SP_HERB_POISON:
		output(ply_ptr->fd,"Your blood begins to burn as the poison spreads!\n");
		F_SET(ply_ptr,PPOISN);
		dmg = mdice(obj_ptr);
		del_obj_crt(obj_ptr, ply_ptr);
		free_obj(obj_ptr);
		ply_ptr->hpcur -= dmg;
		if(ply_ptr->hpcur < 1)
		  die(ply_ptr,ply_ptr);
		break;
      case SP_HERB_HARM:
		output(ply_ptr->fd,"Your body begins to jerk maddly!\n");
		dmg = mdice(obj_ptr);
		del_obj_crt(obj_ptr, ply_ptr);
		free_obj(obj_ptr);
		ply_ptr->hpcur -= dmg;
		if(ply_ptr->hpcur < 1)
		  die(ply_ptr,ply_ptr);
		break;
      case SP_HERB_DISEASE:
		output(ply_ptr->fd,"Oozing pusstules begin to appears on your arms and face.\n");
		F_SET(ply_ptr,PDISEA);
		break;
		  case SP_HERB_CURE_POISON:
		output(ply_ptr->fd,"You feel your blood cool.\n");
		F_CLR(ply_ptr,PPOISN);
		del_obj_crt(obj_ptr, ply_ptr);
		free_obj(obj_ptr);
		break;
      case SP_HERB_CURE_DISEASE:
		output(ply_ptr->fd,"A clean feeling flows through your body.\n");
		F_CLR(ply_ptr,PDISEA);
		del_obj_crt(obj_ptr, ply_ptr);
		free_obj(obj_ptr);
		break;
      default:
        output(ply_ptr->fd,"The herb went bad.\n");
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
int eat( creature *ply_ptr, cmd	*cmnd )
{

	object 	*obj_ptr;
	int	fd;

	fd=ply_ptr->fd;

	if(cmnd->num < 2) {
		output(fd, "Eat what?\n");
		return(0);
	}

	obj_ptr=find_obj(ply_ptr, ply_ptr->first_obj, cmnd->str[1], cmnd->val[1]);
	
	if(!obj_ptr)	{
		output(fd, "You dont have that.\n");
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
                                output(fd, "You can't drink anymore.\n");
                                break;
                        }
			if(obj_ptr->shotscur){
				obj_ptr->shotscur-=1;			
	                        ply_ptr->talk[5]-=obj_ptr->ndice;
        	                if(ply_ptr->talk[5] < 0)
                	                ply_ptr->talk[5]=0;
							broadcast_rom(fd, ply_ptr->rom_num, 
								"%M drinks from a %i.", 
								m2args(ply_ptr, obj_ptr));
                        	output(fd, "Ahh, refreshing!\n");
                        	break;
			}
			else 
			{
				sprintf(g_buffer, "The %s is empty.\n", obj_ptr->name);
				output(fd, g_buffer);
			}
			break;
			
			
		case FOOD:
			if(ply_ptr->talk[6]<1) {
				output(fd, "You cannot eat another bite.\n");
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
				broadcast_rom(fd, ply_ptr->rom_num, 
					"%M munches on a %i.", 
					m2args(ply_ptr, obj_ptr));
				output(fd, "Yummy!\n");
				break;
			}
			else
				del_obj_crt(obj_ptr, ply_ptr);
				free(obj_ptr);
				output(fd, "I don't see that here.\n");
				break;
		default:
			output(fd, "You can't do that.\n");
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
int describe_me( creature *ply_ptr, cmd *cmnd )
{
	int fd, i;

	fd=ply_ptr->fd;
	if(cmnd->num < 2) {
		output(fd, "Syntax: description [text|-d]\n.");
		return(0);
	}
	if(!strcmp(cmnd->str[1], "-d")) {
		output(fd, "Description cleared.\n");
		ply_ptr->description[0]=0;
		return(0);
	}
	i=0;
	while(!isspace((int)cmnd->fullstr[i]))
            i++;
	
	strncpy(ply_ptr->description,&cmnd->fullstr[i+1],78);
        ply_ptr->description[79]=0;
	output(fd, "Description set.\n");
	return(0);
}

int scout(creature *ply_ptr, cmd *cmnd)
{
	int	fd, chance, i, x=0;
	room	*rom_ptr, *scout_rom;
	exit_	*ext_ptr;
	time_t	t;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(cmnd->num < 2) {
		output(fd, "Scout where?\n");
		return(0);
	}
	if(ply_ptr->class < IMMORTAL) {
	   if (ply_ptr->class != RANGER
		&& ply_ptr->class != ASSASSIN && ply_ptr->class != THIEF) {
		output(fd, "Only certain classes can scout.\n");
		return(0);
	   }
	}

     
	F_CLR(ply_ptr, PHIDDN);
        if(F_ISSET(ply_ptr, PINVIS)) {
                F_CLR(ply_ptr, PINVIS);
                output(fd, "Your invisibility fades.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "%M fades into view.",
                              m1arg(ply_ptr));
        }

	t = time(0);
	i = LT(ply_ptr, LT_SCOUT);

	if(ply_ptr->class < IMMORTAL)
	   if(t < i) {
        	please_wait(fd, i-t);
        	return(0);
	   }

	ply_ptr->lasttime[LT_SCOUT].ltime = t;
	ply_ptr->lasttime[LT_SCOUT].interval = MAX(0, 5 - bonus[(int)ply_ptr->dexterity]);

        if(F_ISSET(ply_ptr, PBLIND)) {
                output_wc(fd, "How do you do that?  You're blind.\n", BLINDCOLOR);
		return(0);
        }

        chance = (ply_ptr->class == RANGER) ? 4*ply_ptr->level : 0 +
		 (ply_ptr->class == DRUID) ? 3*ply_ptr->level : 0 +
		 (ply_ptr->class == ASSASSIN) ? 3*ply_ptr->level : 0 +
		 (ply_ptr->class == THIEF) ? 2*ply_ptr->level : 0 +
                 3*ply_ptr->level;
        chance += bonus[(int)ply_ptr->dexterity]*3;
        chance = MIN(chance, 65);

	if(ply_ptr->class >= IMMORTAL)
		chance = 100;

	ext_ptr = find_ext(ply_ptr, rom_ptr->first_ext,
              cmnd->str[1], cmnd->val[1]);
	if(!ext_ptr) {
		output(fd, "I don't see that exit.\n");
        	return(0);
    	}

	if(F_ISSET(ext_ptr, XUNIQP)) {
                if ( ply_ptr->class >= CARETAKER ) {
                        output(fd, "Unique player exit.\n");
                }
                else {
                        x = check_ply_ext(ply_ptr->name, ext_ptr);
                        if(!x) {
                                output(fd, "You can't see in there.\n");
                                return(0);
                        }
                }
        }

	if(F_ISSET(ext_ptr, XLOCKD)) {
		output(fd, "It's locked.\n");
        	return(0);
    	}
    	else if(F_ISSET(ext_ptr, XCLOSD)) {
		output(fd, "You have to open it first.\n");
        	return(0);
    	}
    	if(F_ISSET(ext_ptr, XPLDGK))
     	    if (!F_ISSET(ply_ptr, PPLDGK)){
            	output(fd, "You do not have the proper authorization to scout in that direction.\n");
            	return(0);
            }

    	t = Time%24L;
    	if(F_ISSET(ext_ptr, XNGHTO) && (t>6 && t < 20)) {
		output(fd, "That exit is not open during the day.\n");
        	return(0);
    	}

    	if(F_ISSET(ext_ptr, XDAYON) && (t<6 || t > 20)) {
		output(fd, "That exit is closed for the night.\n");
        	return(0);
    	}
    	if(F_ISSET(ext_ptr, XFEMAL) && F_ISSET(ply_ptr, PMALES)){
		output(fd, "Sorry, only females are allowed to scout in that direction.\n");
        	return(0);
    	}
    	if(F_ISSET(ext_ptr, XMALES) && !F_ISSET(ply_ptr, PMALES)){
        	output(fd, "Sorry, only males are allowed to scout in that direction.\n");
        	return(0);
    	}

    	if ( load_rom(ext_ptr->room, &scout_rom) < 0 ) {
        	output(fd, "Off map in that direction.\n");
        	return(0);
    	}

	if(mrand(1,100) <= chance) {
		display_rom(ply_ptr, scout_rom);
		sprintf(g_buffer, "%%M scouted the %s.", ext_ptr->name);
		broadcast_rom(fd, ply_ptr->rom_num, g_buffer, m1arg(ply_ptr));
	} else {
		output(fd, "You failed to scout in that direction.\n");
		sprintf(g_buffer, "%%M tried to scout the %s.",ext_ptr->name);
		broadcast_rom(fd, ply_ptr->rom_num, g_buffer, m1arg(ply_ptr));
	}		

	return(0);
	
}
