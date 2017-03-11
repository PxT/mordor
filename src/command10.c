/*
 * COMMAND10.C:
 *
 *  Additional user routines.
 *  Copyright 1994, 1997 Brooke Paul
 *
 * $Id: command10.c,v 6.16 2001/07/17 19:25:11 develop Exp $
 *
 * $Log: command10.c,v $
 * Revision 6.16  2001/07/17 19:25:11  develop
 * *** empty log message ***
 *
 * Revision 6.15  2001/07/03 22:47:47  develop
 * conjure/sneak fix
 *
 * Revision 6.14  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

#include "../include/mordb.h"
#include "mextern.h"


/**********************************************************************/
/*                           sneak                                   */
/**********************************************************************/


int sneak( creature *ply_ptr, cmd *cmnd)
{
    room        *rom_ptr, *old_rom_ptr;
    ctag        *cp;
    exit_       *ext_ptr;
    time_t        i, t;
    int     fd, old_rom_num, fall, dmg, n, x=0;
    int         chance, notclass=0;

    rom_ptr = ply_ptr->parent_rom;
    fd = ply_ptr->fd;

    if(ply_ptr->class != RANGER &&ply_ptr->class != ASSASSIN && ply_ptr->class != THIEF && ply_ptr->class != MONK && ply_ptr->class < BUILDER) 
                notclass=1;

    if(!F_ISSET(ply_ptr, PHIDDN)){
                output(fd, "You need to hide first.\n");
                return(0);
        }                  

    if(cmnd->num < 2) {
        output(fd, "Sneak where?\n");
        return(0);
    }

    ext_ptr = find_ext(ply_ptr, rom_ptr->first_ext, 
               cmnd->str[1], cmnd->val[1]);

    if(!ext_ptr) {
        output(fd, "I don't see that exit.\n");
        return(0);
    }

    if(F_ISSET(ext_ptr, XLOCKD)) {
        output(fd, "It's locked.\n");
        return(0);
    }
    else if(F_ISSET(ext_ptr, XCLOSD)) {
        output(fd, "You have to open it first.\n");
        return(0);
    }

    if(F_ISSET(ext_ptr, XFLYSP) && !F_ISSET(ply_ptr, PFLYSP)) {
        output(fd, "You must fly to get there.\n");
        return(0);
    }
	if(F_ISSET(ext_ptr, XPLSEL)) {
		if(!F_ISSET(ext_ptr, XPLSEL + ply_ptr->class) && ply_ptr->class < BUILDER) {
			output(fd, "Your class prohibits you from entering there.\n");
			return(0);
		}
	}

    if(F_ISSET(ext_ptr, XNAKED) && weight_ply(ply_ptr)) {
        output(fd, "You cannot bring anything through that exit.\n");
        return(0);
    }


    if(F_ISSET(ext_ptr, XPLDGK)) {
     if (!F_ISSET(ply_ptr, PPLDGK)){
            output(fd, "You do not have the proper authorization to go there.\n");
            return(0);
     }
     else if (exitcheck_guild(ext_ptr) !=  check_guild(ply_ptr)){
            output(fd, "That guild area is restricted.\n");
            return(0);
         }          
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
 
    if(F_ISSET(ext_ptr,XPGUAR))
	{
        cp = rom_ptr->first_mon;
        while(cp) 
		{
            if(F_ISSET(cp->crt, MPGUAR)) 
			{ 
            	if(!F_ISSET(ply_ptr, PINVIS) && ply_ptr->class < BUILDER)
				{
					mprint(fd, "%M blocks your exit.\n", m1arg(cp->crt));
					return(0);
				}
     			if(F_ISSET(cp->crt, MDINVI) && ply_ptr->class < BUILDER) {
       				mprint(fd, "%M blocks your exit.\n", m1arg(cp->crt));
					return(0);
				}
			}
            cp = cp->next_tag;
        }
    }         
 
    if(F_ISSET(ext_ptr, XFEMAL) && F_ISSET(ply_ptr, PMALES)){
        output(fd, "Sorry, only females are allowed to go there.\n");
        return(0); 
    }
    if(F_ISSET(ext_ptr, XMALES) && !F_ISSET(ply_ptr, PMALES)){
        output(fd, "Sorry, only males are allowed to go there.\n");
        return(0); 
    }           

    if(F_ISSET(ext_ptr, XUNIQP)) {
            if ( ply_ptr->class >= CARETAKER ) {
                        output(fd, "Unique player exit passed.\n");
            }
            else {
                    x = check_ply_ext(ply_ptr->name, ext_ptr);
                    if(!x) {
                            output(fd, "You may not go there.\n");
                            return(0);
                    }
            }
    }

    if((F_ISSET(ext_ptr, XCLIMB) || F_ISSET(ext_ptr, XREPEL)) &&
       !F_ISSET(ply_ptr, PLEVIT)) {
        fall = (F_ISSET(ext_ptr, XDCLIM) ? 50:0) + 50 - 
               fall_ply(ply_ptr);

        if(mrand(1,100) < fall) {
            dmg = mrand(5, 15+fall/10);
            if(ply_ptr->hpcur <= dmg){
                   output(fd, "You fell to your death.\n");
                   ply_ptr->hpcur=0;
                   broadcast_rom(fd, ply_ptr->rom_num, "%M died from the fall.\n", m1arg(ply_ptr));
                   die(ply_ptr, ply_ptr);
                   return(0);
            }

            ply_ptr->hpcur -= dmg;
            sprintf(g_buffer, 
				"You fell and hurt yourself for %d damage.\n",
                dmg);
            output(fd, g_buffer );
            broadcast_rom(fd, ply_ptr->rom_num, "%M fell down.", 
                      m1arg(ply_ptr));

		if(ply_ptr->hpcur < 1) {
            output(fd, "You died.\n");
            die(ply_ptr, ply_ptr);
		}

            if(F_ISSET(ext_ptr, XCLIMB))
                return(0);
        }
    }

    i = LT(ply_ptr, LT_ATTCK);
    t = time(0);
    if(t < i) {
        please_wait(fd, i-t);
        return(0);
    }
        chance = MIN(85, 5 + 6*ply_ptr->level + 3*bonus[(int) ply_ptr->dexterity]);    
	if(F_ISSET(ply_ptr, PBLIND))
		chance = MIN(20, chance);

	if(ply_ptr->class == RANGER)
		chance +=5;

    if(mrand(1,100) > chance || notclass) 
    {
        output(fd,"You failed to sneak.\n"); 
        F_CLR(ply_ptr, PHIDDN);

        cp = rom_ptr->first_mon;
        while(cp) {
            if(F_ISSET(cp->crt, MBLOCK) && 
            is_enm_crt(ply_ptr->name, cp->crt) &&
            !F_ISSET(ply_ptr, PINVIS) && ply_ptr->class < BUILDER) {
                mprint(fd, "%M blocks your exit.\n", m1arg(cp->crt));
                return(0);
            }
            cp = cp->next_tag;
        }
    }

    if(!F_ISSET(rom_ptr, RPTRAK))
        strcpy(rom_ptr->track, ext_ptr->name);

    old_rom_num = rom_ptr->rom_num;
    old_rom_ptr = rom_ptr;

    if ( load_rom(ext_ptr->room, &rom_ptr) < 0 )
	{
        output(fd, "Off map in that direction.\n");
        return(0);
    }

    n = count_vis_ply(rom_ptr);

	/* bypass all room restrictions for immortals */
	if ( ply_ptr->class < BUILDER )
	{

		if(rom_ptr->lolevel > ply_ptr->level && ply_ptr->class < BUILDER) {
			sprintf(g_buffer, "You must be at least level %d to go that way.\n",
				  rom_ptr->lolevel);
			output(fd, g_buffer);
			return(0);
		}
		else if(ply_ptr->level > rom_ptr->hilevel && rom_ptr->hilevel &&
			ply_ptr->class < BUILDER) {
			sprintf(g_buffer, "Only players under level %d may go that way.\n",
				  rom_ptr->hilevel+1);
			output(fd, g_buffer);
			return(0);
		}
		else if((F_ISSET(rom_ptr, RONEPL) && n > 0) ||
			(F_ISSET(rom_ptr, RTWOPL) && n > 1) ||
			(F_ISSET(rom_ptr, RTHREE) && n > 2)) {
			output(fd, "That room is full.\n");
			return(0);
		}
	}

    if(!F_ISSET(ply_ptr, PHIDDN)) {
        if(strcmp(ext_ptr->name, "up") && strcmp(ext_ptr->name, "down") 
           && strcmp(ext_ptr->name, "out") && !F_ISSET(ply_ptr, PDMINV)) {
                sprintf(g_buffer, "%%M went to the %s.", ext_ptr->name);
                broadcast_rom(fd, old_rom_ptr->rom_num, g_buffer, 
                              m1arg(ply_ptr));
        }
        else if(!F_ISSET(ply_ptr, PDMINV)) {
                sprintf(g_buffer, "%%M went %s.", ext_ptr->name);
                broadcast_rom(fd, old_rom_ptr->rom_num, g_buffer, 
                              m1arg(ply_ptr));
        }
    }
                                

   cp = ply_ptr->first_fol;
   while(cp) {
		if(cp->crt->type == MONSTER && F_ISSET(cp->crt, MCONJU)) {
			if(F_ISSET(rom_ptr, RNOLEA)) {
				output(fd, "Your conjure dissipates.\n");
				del_conjured(cp->crt);
				del_crt_rom(cp->crt, ply_ptr->parent_rom);
			}
			else {
				del_crt_rom(cp->crt, ply_ptr->parent_rom);
				add_crt_rom(cp->crt, rom_ptr,0);
			}
		}
		cp = cp->next_tag;
    }		
	
    del_ply_rom(ply_ptr, ply_ptr->parent_rom);
    add_ply_rom(ply_ptr, rom_ptr);


    check_traps(ply_ptr, rom_ptr);
    return(0);
}
/**********************************************************************/
/*                          Group_talk                                 */
/**********************************************************************/
/* Group talk allows a player to broadcast a message to everyone in   *
 * their group. gtalk first checks to see if the player is following  *
 * another player; in that case, the group talk message will be sent  *
 * every member in the leader's group.  If the player isn't following *
 * then it is assumed the player is a leader and the message will be    *
 * sent to all the followers in the group. */
 
int gtalk(creature *ply_ptr, cmd *cmnd )
{
    creature *leader;
    ctag     *cp;
    int      fd;
    int	     found;

    fd = ply_ptr->fd;

	if(ply_ptr->following)
	{
		leader = ply_ptr->following;
		cp = ply_ptr->following->first_fol;
	}
		
	else
	{
		leader = ply_ptr;
		cp = ply_ptr->first_fol;
	}

	if(!cp) 
	{
        	output(fd, "You are not in a group.\n");
        	return(0);
	}

    	cmnd->fullstr[255] = 0;
	clean_str( cmnd->fullstr, 1 );

	if( strlen(cmnd->fullstr) < 1) 
	{
        	output(fd, "Group mention what?\n");
        	return(0);
	} 


	if(F_ISSET(ply_ptr, PSILNC)) 
	{
        	output_wc(fd, "You can't seem to form the words.\n", SILENCECOLOR);
       		return(0);
	} 

	/* spam check */
	if ( check_for_spam( ply_ptr ) )
	{
		return(0);
	}


while(cp) 
{
        
	if((F_ISSET(cp->crt, PIGNOR) && (ply_ptr->class < BUILDER)) && !F_ISSET(cp->crt,PDMINV)) 
	{
		 sprintf(g_buffer, "%s is ignoring everyone.\n", cp->crt->name);
		output(fd, g_buffer);
	}
        
	else
	{
        	sprintf(g_buffer, "### %%M group mentioned, \"%s\".\n", cmnd->fullstr);
        	mprint(cp->crt->fd, g_buffer, m1arg(ply_ptr));
	}
        
	if (!F_ISSET(cp->crt,PDMINV))
        	found = 1;
        
	cp = cp->next_tag;
}
    
	if((F_ISSET(leader, PIGNOR) && (ply_ptr->class < BUILDER)) && !F_ISSET(leader,PDMINV ))  
	{
        	sprintf(g_buffer, "%s is ignoring everyone.\n", leader->name);
        	output(fd, g_buffer);
	}
    
	else
	{
        	sprintf(g_buffer, "### %%M group mentioned, \"%s\".\n", cmnd->fullstr);     
        	mprint(leader->fd, g_buffer, m1arg(ply_ptr));     
	}

	if (found)
	{

		if (ply_ptr->class == DM)
			return (0);

		sprintf(g_buffer, "--- %s group talked: \"%s\".", ply_ptr->name, cmnd->fullstr);
		broadcast_eaves(g_buffer);
	}

    return(0);

}

     
/************************************************************************/
/*                              lower_prof                              */
/************************************************************************/
/* The lower_prof function adjusts a player's magical realm and weapon *
 * proficiency after a player loses (exp) amount of experience         */

void lower_prof( creature *ply_ptr, long exp )
{
    long    profloss, total;
    int     n, below=0;

        for(n=0;n<6;n++) {
		if(ply_ptr->proficiency[n]>-1)
			continue;
		ply_ptr->proficiency[n]=0;
	}      
        for(n=0,total=0L; n<6; n++)
            total += ply_ptr->proficiency[n];
        for(n=0; n<4; n++)
            total += ply_ptr->realm[n];
 
        profloss = MIN(exp,total);
        while(profloss > 10L && below < 10) {
            below = 0;
            for(n=0; n<10; n++) {
                if(n < 6) {
                    ply_ptr->proficiency[n] -=
                       profloss/(10-n);
                    profloss -= profloss/(10-n);
                    if(ply_ptr->proficiency[n] < 0L) {
                        below++;
                        profloss -=
                           ply_ptr->proficiency[n];
                        ply_ptr->proficiency[n] = 0L;
                    }
                }
                else {
                    ply_ptr->realm[n-6] -= profloss/(10-n);
                    profloss -= profloss/(10-n);
                    if(ply_ptr->realm[n-6] < 0L) {
                        below++;
                        profloss -=
                            ply_ptr->realm[n-6];
                        ply_ptr->realm[n-6] = 0L;
                    }
                }
            }
        }
        for(n=1,total=0; n<6; n++)
            if(ply_ptr->proficiency[n] >
                ply_ptr->proficiency[total]) total = n;
        if(ply_ptr->proficiency[total] < 1024L)
            ply_ptr->proficiency[total] = 1024L;
    return;
}
/************************************************************************/
/*                              add_prof                                */
/************************************************************************/
/* The add_prof function adjusts a player's magical realm and weapon     *
 * proficiency after the player gains an unassigned amount of experience *
 * such as from a quest item or pkilling.  The exp gain is divided       *
 * between the player's 6 weapon proficiency and 4 offnsive magic realms */

void add_prof( creature *ply_ptr, long exp )
{
    int     n;
          
            if (exp) 
                exp  = exp/10L;
	    else return;
	      
            for(n=0; n<10; n++) 
                if(n < 6)
                    ply_ptr->proficiency[n] += exp;
                else
                    ply_ptr->realm[n-6]  += exp;

        return;
}
               
/*====================================================================*/
/*              lose_all				              */
/*====================================================================*/
/* lose_all causes the given player to lose all his or her possessions *
 * including any equipted items.                                       */

void lose_all( creature *ply_ptr )
{
    object  *obj_ptr;
    room    *rom_ptr;
    otag    *op;
    int fd, i;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

/* remove all equipted items */
    for(i=0; i<MAXWEAR; i++) {
        if(ply_ptr->ready[i] && !F_ISSET(ply_ptr->ready[i], OCURSE)) {
            F_CLR(ply_ptr->ready[i], OWEARS);
	    dequip(ply_ptr, ply_ptr->ready[i]);
            add_obj_crt(ply_ptr->ready[i], ply_ptr);
            ply_ptr->ready[i] = 0;
        }
    }


    compute_ac(ply_ptr);
    compute_thaco(ply_ptr);

/* remove and delete all possessions */
    op = ply_ptr->first_obj;
    while(op) {
            obj_ptr = op->obj;
            op = op->next_tag;
            del_obj_crt(obj_ptr, ply_ptr);
            free_obj(obj_ptr);
    }

}

/**********************************************************************/
/*                      dissolve_item                                 */
/**********************************************************************/
/* dissolve_item will randomly select one equipted (including held or *
 * wield) items on the given player and then delete it. The player    *
 * receives a message that the item was destroyed as well as who is   *
 * responsible for the deed.										  */
void dissolve_item (creature *ply_ptr, creature *crt_ptr)
{
    int     n;
    char    checklist[MAXWEAR];
    int     numwear=0, i;
 
        for(i=0; i<MAXWEAR; i++) {
                checklist[i] = 0;
/*                if(i==WIELD-1 || i==HELD-1) continue; */
                if(ply_ptr->ready[i])
                        checklist[numwear++] = i+1;
        }
 
        if(!numwear) 
            n = 0;
        else {
            i = mrand(0, numwear-1);
            n = (int) checklist[i];
        }
            
        if(n) {
            sprintf(g_buffer, "%%M destroys %%m's %s.",
                ply_ptr->ready[n-1]->name);
            broadcast_rom(ply_ptr->fd,ply_ptr->rom_num,
				g_buffer, m2args(crt_ptr, ply_ptr));

			sprintf(g_buffer, "%%M destroys your %s.\n", 
				ply_ptr->ready[n-1]->name); 
			mprint(ply_ptr->fd, g_buffer, m1arg(crt_ptr));
			dequip(ply_ptr,ply_ptr->ready[n-1]);
			free_obj(ply_ptr->ready[n-1]);
			ply_ptr->ready[n-1] = 0;
			compute_ac(ply_ptr);
        }
	return;
}            

/**********************************************************************/
/*                              purchase                              */
/**********************************************************************/
/* purchase allows a playe rto buy an item from a monster.  The      *
 * purchase item flag must be set, and the monster must have an    *
 * object to sell.  The object for sale is determined by the first   *
 * object listed in carried items. */

int purchase(creature *ply_ptr, cmd *cmnd )
{
        creature        *crt_ptr;
        room            *rom_ptr;
		object		*obj_ptr;
		int			maxitem = 0,obj_num[10];
		long		amt;
		int 		fd, i, j, found = 0, match = 0;
 
        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
 
        if(cmnd->num < 2) {
                output(fd, "Purchase what?\n");
                return(0);
        }

        if(cmnd->num < 3 ) {
                output(fd, "Syntax: purchase <item> <monster>\n");
                return(0);
        }
 
        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[2],
                           cmnd->val[2]);
 
        if(!crt_ptr) {
                output(fd, "That is not here.\n");
                return(0);
        }
	
	if (!F_ISSET(crt_ptr,MPURIT)){
		mprint(fd, "You can not purchase objects from %m.\n",
			m1arg(crt_ptr));
		return(0);
	}

	for (i=0;i <  10; i++)
		if (crt_ptr->carry[i] > 0){
		    found = 0;
		    for(j=0; j< maxitem; j++)
			if (crt_ptr->carry[i] == obj_num[j])
				found = 1;
		    if (!found){
			maxitem++;
			obj_num[i] = crt_ptr->carry[i];
		    }
		}

	if (!maxitem){
		mprint(fd, "%M has nothing to sell.\n", m1arg(crt_ptr));
		return(0);
	}
	
	found = 0;
	for(i=0;i< maxitem; i++){
		if (!(load_obj(crt_ptr->carry[i], &obj_ptr) < 0))
		if(EQUAL(obj_ptr, cmnd->str[1]) && (F_ISSET(ply_ptr, PDINVI) ?
                   1:!F_ISSET(obj_ptr, OINVIS))) { 
                        match++;
                        if(match == cmnd->val[1]) {
                                found = 1;
                                break;
                        }
                }
	}
 
	if(!found){
		mprint(fd, "%M says, \"Sorry, I don't have any of those to sell.\"\n", m1arg(crt_ptr));
        return(0);
	}

 
	amt =  MAX(10,obj_ptr->value*1);
        if(ply_ptr->gold < amt){
             sprintf(g_buffer, "%%M says \"The price is $%ld, and not a gold piece less!\"\n", amt);
             mprint(fd, g_buffer, m1arg(crt_ptr) );
        }
        else {
			sprintf(g_buffer, "You pay %%m %ld gold pieces.\n", amt);
			mprint(fd, g_buffer, m1arg(crt_ptr));

		    sprintf(g_buffer, "%%M says, \"Thank you very much.  Here is your %s.\"\n", obj_ptr->name);
		    mprint(fd, g_buffer, m1arg(crt_ptr));

    		sprintf(g_buffer, "%%M pays %%m $%ld for %%i.\n", amt);          
    		broadcast_rom(fd, ply_ptr->rom_num, g_buffer, 
				m3args(ply_ptr, crt_ptr, obj_ptr));          
            ply_ptr->gold -= amt;
			add_obj_crt(obj_ptr, ply_ptr);  	
        }
 
        return(0);
 
}                 


/**********************************************************************/
/*                              selection                              */
/**********************************************************************/
/* The selection command  lists all the items  a monster is selling.  *
 * The monster needs the MPURIT flag set to denote it can sell. */

int selection(creature *ply_ptr, cmd *cmnd)
{
        creature        *crt_ptr;
        room            *rom_ptr;
	object		*obj_ptr[10];
	int		obj_list[10];
	int 		fd, i, j, found = 0, maxitem = 0;
 
        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
 
        if(cmnd->num < 2) {
                output(fd, "Ask who for a selection?\n");
                return(0);
        }
 
 
        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[1],
                           cmnd->val[1]);
 
        if(!crt_ptr) {
                output(fd, "That is not here.\n");
                return(0);
        }

	if(!F_ISSET(crt_ptr,MPURIT)){
		mprint(fd, "%M is not selling anything.\n", m1arg(crt_ptr));
		return(0);
	}
	
	for (i=0;i <  10; i++)
		if (crt_ptr->carry[i] > 0){
		    found = 0;
		    for(j=0; j< maxitem; j++)
			if (crt_ptr->carry[i] == obj_list[j])
				found = 1;
		    
		    if (!found){
			maxitem++;
			obj_list[i] = crt_ptr->carry[i];
		    }
		}

	if(!maxitem){
		mprint(fd, "%M has nothing to sell.\n", m1arg(crt_ptr));
		return(0);
	}
	mprint(fd, "%M is currently selling:\n", m1arg(crt_ptr));
	for (i=0;i<maxitem;i++)
		if ((load_obj(crt_ptr->carry[i], &(obj_ptr[i])) < 0)  ||
			(crt_ptr->carry[i] == 0))
		{
			sprintf(g_buffer,"%d) Out of stock.\n",i+1);
			output(fd, g_buffer);
		}
		else 
		{
			sprintf(g_buffer,"%d) %-22s    %ldgp\n",i+1, 
				(obj_ptr[i])->name, MAX(10,((obj_ptr[i])->value)));
			output(fd, g_buffer);
		}

	output(fd,"\n"); 
    return(0);
 
}                 

 
 
/**********************************************************************/
/*			trade					      */
/**********************************************************************/
int trade( creature *ply_ptr, cmd *cmnd )
{
        creature    *crt_ptr;
        room        *rom_ptr;
	object		*obj_ptr, *trd_ptr;
        int         obj_list[5][2];
        int         fd, i, j, found = 0, maxitem = 0;
 
        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
 
        if(cmnd->num < 2) {
                output(fd, "Trade what?\n");
                return(0);
        }
 
        if(cmnd->num < 3) {
                output(fd, "Syntax: trade <item> <monster>\n");
                return(0);
        }
 
        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[2],
                           cmnd->val[2]);
 
        if(!crt_ptr) {
                output(fd, "That is not here.\n");
                return(0);
        }
 
        if(!F_ISSET(crt_ptr,MTRADE)){
                mprint(fd, "You can't trade with %m.\n",m1arg(crt_ptr));
                return(0);
        }
 
        obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
                           cmnd->str[1], cmnd->val[1]);
 
        if(!obj_ptr) {
                output(fd, "You don't have that.\n");
                return(0);
        }
                
        
        for (i=0;i <  5; i++)
                if (crt_ptr->carry[i] > 0){
                    found = 0;
                    for(j=0; j< maxitem; j++)
                        if (crt_ptr->carry[i] == obj_list[j][0])
                                found = 1;
                    
                    if (!found){
                        maxitem++;
                        obj_list[i][0] = crt_ptr->carry[i];
                        obj_list[i][1] = crt_ptr->carry[i+5];
                    }
                }
 
        if(!maxitem){
                mprint(fd, "%M has nothing to trade.\n", m1arg(crt_ptr));
                return(0);
        }

	found = 0;
	for(i=0;i < maxitem; i++){
	if (load_obj(obj_list[i][0], &trd_ptr) < 0)
		continue;
		if (!strcmp(obj_ptr->name, trd_ptr->name)){
			found = i+1;
			break;
		}
	} 

	if(!found || ((obj_ptr->shotscur <= obj_ptr->shotsmax/10)&&(obj_ptr->type != MISC)))
		mprint(fd,"%M says, \"I don't want that!\"\n", m1arg(crt_ptr));
	else{
		if (crt_ptr->carry[found+4] == 0){
        		del_obj_crt(obj_ptr, ply_ptr);
        		add_obj_crt(obj_ptr, crt_ptr); 
			mprint(fd, "%M says, \"Thanks!, I really needed %i.\n",
 				m2args(crt_ptr,obj_ptr));
 			output(fd, "Sorry I don't have anything for you.\"\n");
			broadcast_rom(fd, ply_ptr->rom_num,"%M gives %m %i.",
				m3args(ply_ptr,crt_ptr,obj_ptr));
		}
		else if(!(load_obj((obj_list[found-1][1]), &trd_ptr) < 0)){ 

              if(trd_ptr->questnum && Q_ISSET(ply_ptr, trd_ptr->questnum-1)) {
                        output(fd, "You have already fulfilled that quest.\n");
                        return(0);
                }                    
        		del_obj_crt(obj_ptr, ply_ptr);
        		add_obj_crt(obj_ptr, crt_ptr); 
        		add_obj_crt(trd_ptr, ply_ptr); 

			mprint(fd,"%M says, \"Thank you for retrieving %i for me.\n",
				m2args(crt_ptr,obj_ptr));
			mprint(fd,"For your deed I will give you %i.\"\n",
				m1arg(trd_ptr));
			mprint(fd,"%M gives you %i.\n", m2args(crt_ptr,trd_ptr));
			broadcast_rom(fd, ply_ptr->rom_num,"%M gives %m %i.\n",
				m3args(ply_ptr,crt_ptr,obj_ptr));
	/*		broadcast_rom(fd, ply_ptr->rom_num,"%M gives %m %i.\n", */
	/*			ply_ptr,crt_ptr,obj_ptr); */

/* if return object is a quest item */
  			if(trd_ptr->questnum) {
                       	 output(fd, "Quest fulfilled!  Don't drop it.\n");
                       	 output(fd, "You won't be able to pick it up again.\n");  
                         Q_SET(ply_ptr, trd_ptr->questnum-1);
                         ply_ptr->experience += get_quest_exp(trd_ptr->questnum);
                         sprintf(g_buffer, "%ld experience granted.\n",
                                get_quest_exp(trd_ptr->questnum));
                         output(fd, g_buffer);
                         add_prof(ply_ptr, get_quest_exp(trd_ptr->questnum));
                }     
	   }

	}
	return(0);	
}             

/**********************************************************************/
/*			room_obj_count					      */
/**********************************************************************/
int room_obj_count( otag *first_obj )
{

	long num = 0;

	while (first_obj)
	{
		num++;
		first_obj = first_obj->next_tag;
	}
	return (num);
}
