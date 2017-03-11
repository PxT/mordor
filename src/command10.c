/*
 * COMMAND10.C:
 *
 *  Additional user routines.
 *	
 *	(C) 1994-1997 Brooke Paul
 *
 */

#include "mstruct.h"
#include "mextern.h"
#ifdef DMALLOC
  #include "/usr/local/include/dmalloc.h"
#endif

/**********************************************************************/
/*                           sneak                                   */
/**********************************************************************/


int sneak(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    room        *rom_ptr, *old_rom_ptr;
    ctag        *cp;
    exit_       *ext_ptr;
    long        i, t;
    int     fd, old_rom_num, fall, dmg, n;
    int         chance, notclass=0;

    rom_ptr = ply_ptr->parent_rom;
    fd = ply_ptr->fd;

    if(ply_ptr->class != RANGER &&ply_ptr->class != ASSASSIN && ply_ptr->class != THIEF && ply_ptr->class != MONK && ply_ptr->class < CARETAKER) 
                notclass=1;

    if(!F_ISSET(ply_ptr, PHIDDN)){
                print(fd, "You need to hide first.\n");
                return(0);
        }                  

    if(cmnd->num < 2) {
        print(fd, "Sneak where?\n");
        return(0);
    }

    ext_ptr = find_ext(ply_ptr, rom_ptr->first_ext, 
               cmnd->str[1], cmnd->val[1]);

    if(!ext_ptr) {
        print(fd, "I don't see that exit.\n");
        return(0);
    }

    if(F_ISSET(ext_ptr, XLOCKD)) {
        print(fd, "It's locked.\n");
        return(0);
    }
    else if(F_ISSET(ext_ptr, XCLOSD)) {
        print(fd, "You have to open it first.\n");
        return(0);
    }

    if(F_ISSET(ext_ptr, XFLYSP) && !F_ISSET(ply_ptr, PFLYSP)) {
        print(fd, "You must fly to get there.\n");
        return(0);
    }
	if(F_ISSET(ext_ptr, XPLSEL)) {
		if(!F_ISSET(ext_ptr, XPLSEL + ply_ptr->class) && ply_ptr->class < CARETAKER) {
			print(fd, "Your class prohibits you from entering there.\n");
			return(0);
		}
	}

    if(F_ISSET(ext_ptr, XNAKED) && weight_ply(ply_ptr)) {
        print(fd, "You cannot bring anything through that exit.\n");
        return(0);
    }


    if(F_ISSET(ext_ptr, XPLDGK)) 
     if (!F_ISSET(ply_ptr, PPLDGK)){
            print(fd, "You do not have the proper authorization to go there.\n");
            return(0);
     }
     else if (BOOL(F_ISSET(ext_ptr,XKNGDM)) !=  BOOL(F_ISSET(ply_ptr, PKNGDM))){
            print(fd, "You belong to the wrong organization to go there.\n");
            return(0);
         }          

    t = Time%24L;
    if(F_ISSET(ext_ptr, XNGHTO) && (t>6 && t < 20)) {
        print(fd, "That exit is not open during the day.\n");
        return(0);
    }          

    if(F_ISSET(ext_ptr, XDAYON) && (t<6 || t > 20)) {
        print(fd, "That exit is closed for the night.\n");
        return(0);
    }          
 
    if(F_ISSET(ext_ptr,XPGUAR)){
        cp = rom_ptr->first_mon;
        while(cp) {
            if(F_ISSET(cp->crt, MPGUAR)) { 
            	if(!F_ISSET(ply_ptr, PINVIS) && ply_ptr->class < CARETAKER){
                print(fd, "%M blocks your exit.\n", cp->crt);
                return(0);
		}
     		if(F_ISSET(cp->crt, MDINVI) && ply_ptr->class < CARETAKER) {
       		print(fd, "%M blocks your exit.\n", cp->crt);
                return(0);
		}
            }
            cp = cp->next_tag;
        }
    }         
 
    if(F_ISSET(ext_ptr, XFEMAL) && F_ISSET(ply_ptr, PMALES)){
        print(fd, "Sorry, only females are allowed to go there.\n");
        return(0); 
    }
    if(F_ISSET(ext_ptr, XMALES) && !F_ISSET(ply_ptr, PMALES)){
        print(fd, "Sorry, only males are allowed to go there.\n");
        return(0); 
    }           

    if((F_ISSET(ext_ptr, XCLIMB) || F_ISSET(ext_ptr, XREPEL)) &&
       !F_ISSET(ply_ptr, PLEVIT)) {
        fall = (F_ISSET(ext_ptr, XDCLIM) ? 50:0) + 50 - 
               fall_ply(ply_ptr);

        if(mrand(1,100) < fall) {
            dmg = mrand(5, 15+fall/10);
            if(ply_ptr->hpcur <= dmg){
                   print(fd, "You fell to your death.\n");
                   ply_ptr->hpcur=0;
                   broadcast_rom(fd, ply_ptr->rom_num, "%M died from the fall.\n");
                   die(ply_ptr, ply_ptr);
                   return(0);
            }

            ply_ptr->hpcur -= dmg;
            print(fd, "You fell and hurt yourself for %d damage.\n",
                  dmg);
            broadcast_rom(fd, ply_ptr->rom_num, "%M fell down.", 
                      ply_ptr);

		if(ply_ptr->hpcur < 1) {
                                print(fd, "You died.\n");
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
        chance = MIN(85, 5 + 6*ply_ptr->level + 
                3*bonus[ply_ptr->dexterity]);    
	if(F_ISSET(ply_ptr, PBLIND))
		chance = MIN(20, chance);

	if(ply_ptr->class == RANGER)
		chance +=5;

    if(mrand(1,100) > chance || notclass) 
    {
        print(fd,"You failed to sneak.\n"); 
        F_CLR(ply_ptr, PHIDDN);

        cp = rom_ptr->first_mon;
        while(cp) {
            if(F_ISSET(cp->crt, MBLOCK) && 
            is_enm_crt(ply_ptr->name, cp->crt) &&
            !F_ISSET(ply_ptr, PINVIS) && ply_ptr->class < CARETAKER) {
                print(fd, "%M blocks your exit.\n", cp->crt);
                return(0);
            }
            cp = cp->next_tag;
        }
    }

    if(!F_ISSET(rom_ptr, RPTRAK))
        strcpy(rom_ptr->track, ext_ptr->name);

    old_rom_num = rom_ptr->rom_num;
    old_rom_ptr = rom_ptr;

    load_rom(ext_ptr->room, &rom_ptr);
    if(rom_ptr == old_rom_ptr) {
        print(fd, "Off map in that direction.\n");
        return(0);
    }

    n = count_vis_ply(rom_ptr);

    if(rom_ptr->lolevel > ply_ptr->level && ply_ptr->class < CARETAKER) {
        print(fd, "You must be at least level %d to go that way.\n",
              rom_ptr->lolevel);
        return(0);
    }
    else if(ply_ptr->level > rom_ptr->hilevel && rom_ptr->hilevel &&
        ply_ptr->class < CARETAKER) {
        print(fd, "Only players under level %d may go that way.\n",
              rom_ptr->hilevel+1);
        return(0);
    }
    else if((F_ISSET(rom_ptr, RONEPL) && n > 0) ||
        (F_ISSET(rom_ptr, RTWOPL) && n > 1) ||
        (F_ISSET(rom_ptr, RTHREE) && n > 2)) {
        print(fd, "That room is full.\n");
        return(0);
    }

    if(!F_ISSET(ply_ptr, PHIDDN))
        if(strcmp(ext_ptr->name, "up") && strcmp(ext_ptr->name, "down") 
           && strcmp(ext_ptr->name, "out") && !F_ISSET(ply_ptr, PDMINV)) {
                broadcast_rom(fd, old_rom_ptr->rom_num, "%M went to the %s.", 
                              ply_ptr, ext_ptr->name);
        }
        else if(!F_ISSET(ply_ptr, PDMINV)) {
                broadcast_rom(fd, old_rom_ptr->rom_num, "%M went %s.", 
                              ply_ptr, ext_ptr->name);
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
 
int gtalk(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    creature *leader;
    ctag    *cp;
    int     found=0, i, j, fd;
    int     len;

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

    if(!cp) {
        print(fd, "You are not in a group.\n");
        return(0);
    }
    len = strlen(cmnd->fullstr);
    for(i=0; i<  len && i < 256; i++) {
        if(cmnd->fullstr[i] == ' ' && cmnd->fullstr[i+1] != ' ')
            found++;
        if(found==1) break;
    }
    cmnd->fullstr[255] = 0;
 	/* Check for modem escape code */
        for(j=0; j<len && j < 256; j++) {
                if(cmnd->fullstr[j] == '+' && cmnd->fullstr[j+1] == '+') {
                        found=0;
                        break;
                }
        }

    if(found < 1 || strlen(&cmnd->fullstr[i+1]) < 1) {
        print(fd, "Group mention what?\n");
        return(0);
    } 
    if(F_ISSET(ply_ptr, PSILNC)) {
        print(fd, "You can't seem to form the words.\n");
        return(0);
    } 

    found = 0;          /*if only inv dm in group */

    while(cp) {
        if((F_ISSET(cp->crt, PIGNOR) && (ply_ptr->class < CARETAKER)) &&
            !F_ISSET(cp->crt,PDMINV))
            print(fd, "%s is ignoring everyone.\n", cp->crt->name);
        else
            print(cp->crt->fd, "### %M group mentioned, \"%s\".\n", 
                ply_ptr, &cmnd->fullstr[i+1]);
        if (!F_ISSET(cp->crt,PDMINV))
            found = 1;
        cp = cp->next_tag;
    }
    if(!found)
    {
        print(fd, "You are not in a group.\n");
        return(0);
    }            
        
        if((F_ISSET(leader, PIGNOR) && (ply_ptr->class < CARETAKER)) &&
            !F_ISSET(leader,PDMINV))
            print(fd, "%s is ignoring everyone.\n", leader->name);
        else
            print(leader->fd, "### %M group mentioned, \"%s\".\n",
                ply_ptr, &cmnd->fullstr[i+1]);     

    if (found)
    broadcast_eaves("--- %s group talked: \"%s\".", ply_ptr->name,
             &cmnd->fullstr[i+1]);

    return(0);

}

     
/************************************************************************/
/*                              lower_prof                              */
/************************************************************************/
/* The lower_prof function adjusts a player's magical realm and weapon *
 * proficiency after a player loses (exp) amount of experience         */

void lower_prof(ply_ptr, exp)
creature   *ply_ptr;
long        exp;
{
    long    profloss, total;
    int     n, below=0;
             
	for(n=0;n<5;n++) {
		if(ply_ptr->proficiency[n]>-1)
			continue;
		ply_ptr->proficiency[n]=0;
	}
        for(n=0,total=0L; n<5; n++)
            total += ply_ptr->proficiency[n];
        for(n=0; n<4; n++)
            total += ply_ptr->realm[n];
 
        profloss = MIN(exp,total);
        while(profloss > 9L && below < 9) {
            below = 0;
            for(n=0; n<9; n++) {
                if(n < 5) {
                    ply_ptr->proficiency[n] -=
                       profloss/(9-n);
                    profloss -= profloss/(9-n);
                    if(ply_ptr->proficiency[n] < 0L) {
                        below++;
                        profloss -=
                           ply_ptr->proficiency[n];
                        ply_ptr->proficiency[n] = 0L;
                    }
                }
                else {
                    ply_ptr->realm[n-5] -= profloss/(9-n);
                    profloss -= profloss/(9-n);
                    if(ply_ptr->realm[n-5] < 0L) {
                        below++;
                        profloss -=
                            ply_ptr->realm[n-5];
                        ply_ptr->realm[n-5] = 0L;
                    }
                }
            }
        }
        for(n=1,total=0; n<5; n++)
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
 * between the player's 5 weapon proficiency and 4 magic realms.      */

void add_prof(ply_ptr, exp)
creature   *ply_ptr;
long        exp;
{
    int     n;
          
            if (exp) 
                exp  = exp/9L;
	    else return;
	      
            for(n=0; n<9; n++) 
                if(n < 5)
                    ply_ptr->proficiency[n] += exp;
                else
                    ply_ptr->realm[n-5]  += exp;

        return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                              Pledge                                  */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* The pledge command allows a player to pledge allegiance to a given  *
 * monster. A pledge players allows the player to use selected items,  *
 * and exits, as well a being able to kill players of the opposing     *
 * kingdoms for experience (regardless if one player is lawful.)  In   *
 * order for a player to pledge, the player needs to be in the correct *
 * room with a correct monster to pledge to. */

int pledge(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        creature        *crt_ptr;
        room            *rom_ptr;
        int             fd, amte, amtg;
 
        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
    
        amte = REWARD;
        amtg = REWARD *5;
 
        if(cmnd->num < 2) {
                print(fd, "Join whom's organization?\n");
                return(0);
        }

        if(F_ISSET(ply_ptr,PPLDGK)){
            print(fd, "You have already joined.\n");
            return(0);
        }

        if(!F_ISSET(rom_ptr,RPLDGK)){
            print(fd, "You can not join here. \n");
            return(0);
        }

        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                           cmnd->str[1], cmnd->val[1]);
 
        if(!crt_ptr) {
                print(fd, "I don't see that here.\n");
                return(0);
        }

if(ISENGARD) {
	if(!(ply_ptr->class == FIGHTER || ply_ptr->class == ASSASSIN || 
ply_ptr->class == THIEF || ply_ptr->class == PALADIN)) {
		print(fd, "You cannot join %m's organization.\n",crt_ptr);
            return(0);
        }
} /* ISENGARD */

        if (!F_ISSET(crt_ptr,MPLDGK))
        {
            print(fd, "You cannot join %m's organization.\n",crt_ptr);
            return(0);
        }

         broadcast_rom(fd, ply_ptr->rom_num, "%M pledges %s allegiance to %m.", 
                        ply_ptr,F_ISSET(crt_ptr, PMALES) ? "his":"her", crt_ptr);      
        print(fd, "You kneel before %m as you join the organization.\n",crt_ptr);
  /*      print(fd, "%M draws %s sword and dubs you a member of %s organization.\n", */
  /*      crt_ptr,*/
  /*          F_ISSET(crt_ptr, PMALES) ? "his":"her",*/
   /*         F_ISSET(crt_ptr, PMALES) ? "his":"her");*/
        print(fd,"The room erupts in cheers for the newest member of %m's organization.\n",crt_ptr); 
        print(fd, "You gain %d experience and %d gold!\n",REWARD, REWARD*5); 

        ply_ptr->experience += amte;
        add_prof(ply_ptr,amte);
        ply_ptr->gold += amtg;
        ply_ptr->hpcur = ply_ptr->hpmax;
        ply_ptr->mpcur = ply_ptr->mpmax;

        F_CLR(ply_ptr, PHIDDN);
        F_SET(ply_ptr,PPLDGK);
        if(F_ISSET(crt_ptr,MKNGDM))
            F_SET(ply_ptr,PKNGDM);

    return(0); 
 
}               

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                              rescind                                 */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* The rescind command allows a player to rescind allegiance to a given *
 * monster.  Once a player has rescinded his allegiance, he or she will *
 * lose all the privilliages of rescinded kingdom as well as a          *
 * specified amount of experience and gold. */

int rescind(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        creature        *crt_ptr;
        room            *rom_ptr;
        int             fd, amte, amtg;
 
        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
        amte = REWARD*10;
        amtg = REWARD*20;
 
        if(cmnd->num < 2) {
                print(fd, "Rescind to whom?\n");
                return(0);
        }

        if(!F_ISSET(ply_ptr,PPLDGK)){
            print(fd, "You have not joined an organization.\n");
            return(0);
        }

        if(!F_ISSET(rom_ptr,RRSCND)){
            print(fd, "You can not rescind your allegiance here.\n");
            return(0);
        }

        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                           cmnd->str[1], cmnd->val[1]);
 
        if(!crt_ptr) {
                print(fd, "I don't see that here.\n");
                return(0);
        }

        if(BOOL(F_ISSET(crt_ptr,MKNGDM)) != BOOL(F_ISSET(ply_ptr,PKNGDM))){
            print(fd, "You are in the wrong organization to rescind!\n");
            return(0);
        }

        if (ply_ptr->gold < amtg){
            print(fd, "You do not have sufficient gold to rescind.\n");
            return (0);
        }

        if (!F_ISSET(crt_ptr,MRSCND))
        {
            print(fd, "You can not rescind your allegiance to %m.\n",crt_ptr);
            return(0);
        }

         broadcast_rom(fd, ply_ptr->rom_num, "%M rescinds %s allegiance to %m.", 
                  ply_ptr,F_ISSET(crt_ptr, PMALES) ? "his":"her", crt_ptr);      
        print(fd, "%M scourns you as you are stripped of all your rights and privileges!\n", crt_ptr);
        print(fd, "\nThe room fills with boos and hisses as you are ostracized from %m's organization.\n", 
            crt_ptr); 

        amte = MIN(amte,ply_ptr->experience);
        print(fd, "\nYou lose %d experience and %d gold!\n",amte,amtg); 
        lower_prof(ply_ptr,amte);
        ply_ptr->experience -= amte;
        ply_ptr->gold -= amtg;
        ply_ptr->hpcur = ply_ptr->hpmax/3;
        ply_ptr->mpcur = 0;

        F_CLR(ply_ptr, PHIDDN);
        F_CLR(ply_ptr,PPLDGK);
        F_CLR(ply_ptr,PKNGDM);

 
    return(0); 
 
}               
/*====================================================================*/
/*              				lose_all				             */
/*====================================================================*/
/* lose_all causes the given player to lose all his or her possessions *
 * including any equipted items.                                       */

void lose_all(ply_ptr)
creature    *ply_ptr;
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
void dissolve_item (ply_ptr,crt_ptr)
creature    *ply_ptr;
creature    *crt_ptr;
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
            broadcast_rom(ply_ptr->fd,ply_ptr->rom_num,"%M destroys %m's %s.",
                crt_ptr, ply_ptr, ply_ptr->ready[n-1]->name);
	    print(ply_ptr->fd,"%M destroys your %s.\n",crt_ptr,
		ply_ptr->ready[n-1]->name); 
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

int purchase(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
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
                print(fd, "Purchase what?\n");
                return(0);
        }

        if(cmnd->num < 3 ) {
                print(fd, "Syntax: purchase <item> <monster>\n");
                return(0);
        }
 
        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[2],
                           cmnd->val[2]);
 
        if(!crt_ptr) {
                print(fd, "That is not here.\n");
                return(0);
        }
	
	if (!F_ISSET(crt_ptr,MPURIT)){
		print(fd, "You can not purchase objects from %m.\n",crt_ptr);
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
		print(fd, "%M has nothing to sell.\n",crt_ptr);
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
		print(fd, "%M says, \"Sorry, I don't have any of those to sell.\"\n", crt_ptr);
        return(0);
	}

 
	amt =  MAX(10,obj_ptr->value*1);
        if(ply_ptr->gold < amt){
                print(fd, 
		"%M says \"The price is $%d, and not a gold piece less!\"\n",crt_ptr,amt);
        }
        else {
		print(fd, "You pay %m %d gold pieces.\n",crt_ptr,amt);
                print(fd, "%M says, \"Thank you very much.  Here is your %s.\"\n",crt_ptr,obj_ptr->name);
    		broadcast_rom(fd, ply_ptr->rom_num, "%M pays %m $%d for %i.\n", ply_ptr, crt_ptr, amt, obj_ptr);          
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

int selection(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        creature        *crt_ptr;
        room            *rom_ptr;
	object		*obj_ptr[10];
	int		obj_list[10];
	int 		fd, i, j, found = 0, maxitem = 0;
 
        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
 
        if(cmnd->num < 2) {
                print(fd, "Ask who for a selection?\n");
                return(0);
        }
 
 
        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[1],
                           cmnd->val[1]);
 
        if(!crt_ptr) {
                print(fd, "That is not here.\n");
                return(0);
        }

	if(!F_ISSET(crt_ptr,MPURIT)){
		print(fd, "%M is not selling anything.\n",crt_ptr);
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
		print(fd, "%M has nothing to sell.\n",crt_ptr);
		return(0);
	}
	print(fd, "%M is currently selling:\n",crt_ptr);
	for (i=0;i<maxitem;i++)
		if ((load_obj(crt_ptr->carry[i], &(obj_ptr[i])) < 0)  ||
			(crt_ptr->carry[i] == 0))
			print(fd,"%d) Out of stock.\n",i+1);
		else 
			print(fd,"%d) %-22s    %ldgp\n",i+1, 
				(obj_ptr[i])->name,MAX(10,((obj_ptr[i])->value)));
	print(fd,"\n"); 
        return(0);
 
}                 

 
 
/**********************************************************************/
/*			trade					      */
/**********************************************************************/
int trade(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        creature    *crt_ptr;
        room        *rom_ptr;
	object		*obj_ptr, *trd_ptr;
        int         obj_list[5][2];
        int         fd, i, j, found = 0, maxitem = 0;
 
        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
 
        if(cmnd->num < 2) {
                print(fd, "Trade what?\n");
                return(0);
        }
 
        if(cmnd->num < 3) {
                print(fd, "Syntax: trade <item> <monster>\n");
                return(0);
        }
 
        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[2],
                           cmnd->val[2]);
 
        if(!crt_ptr) {
                print(fd, "That is not here.\n");
                return(0);
        }
 
        if(!F_ISSET(crt_ptr,MTRADE)){
                print(fd, "You can't trade with %m.\n",crt_ptr);
                return(0);
        }
 
        obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
                           cmnd->str[1], cmnd->val[1]);
 
        if(!obj_ptr) {
                print(fd, "You don't have that.\n");
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
                print(fd, "%M has nothing to trade.\n",crt_ptr);
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
		print(fd,"%M says, \"I don't want that!\"\n",crt_ptr);
	else{
		if (crt_ptr->carry[found+4] == 0){
        		del_obj_crt(obj_ptr, ply_ptr);
        		add_obj_crt(obj_ptr, crt_ptr); 
			print(fd, "%M says, \"Thanks!, I really needed %i.\n",
 				crt_ptr,obj_ptr);
 			print(fd, "Sorry I don't have anything for you.\"\n");
			broadcast_rom(fd, ply_ptr->rom_num,"%M gives %m %i.",
				ply_ptr,crt_ptr,obj_ptr);
		}
		else if(!(load_obj((obj_list[found-1][1]), &trd_ptr) < 0)){ 

              if(trd_ptr->questnum && Q_ISSET(ply_ptr, trd_ptr->questnum-1)) {
                        print(fd, "You have already fulfilled that quest.\n");
                        return(0);
                }                    
        		del_obj_crt(obj_ptr, ply_ptr);
        		add_obj_crt(obj_ptr, crt_ptr); 
        		add_obj_crt(trd_ptr, ply_ptr); 

			print(fd,"%M says, \"Thank you for retrieving %i for me.\n",
				crt_ptr,obj_ptr);
			print(fd,"For your deed I will give you %i.\"\n",trd_ptr);
			print(fd,"%M gives you %i.\n",crt_ptr,trd_ptr);
			broadcast_rom(fd, ply_ptr->rom_num,"%M gives %m %i.\n",
				ply_ptr,crt_ptr,obj_ptr);
	/*		broadcast_rom(fd, ply_ptr->rom_num,"%M gives %m %i.\n", */
	/*			ply_ptr,crt_ptr,obj_ptr); */

/* if return object is a quest item */
  			if(trd_ptr->questnum) {
                       	 print(fd, "Quest fulfilled!  Don't drop it.\n");
                       	 print(fd, "You won't be able to pick it up again.\n");  
                         Q_SET(ply_ptr, trd_ptr->questnum-1);
                         ply_ptr->experience += quest_exp[trd_ptr->questnum-1];
                         print(fd, "%ld experience granted.\n",
                                quest_exp[trd_ptr->questnum-1]);
                         add_prof(ply_ptr,quest_exp[trd_ptr->questnum-1]);
                }     
	   }

	}
	return(0);	
}             

int room_obj_count(first_obj)
otag 	*first_obj;
{

	long num = 0;

	while (first_obj)
{
		num++;
		first_obj = first_obj->next_tag;
}
	return (num);
}
