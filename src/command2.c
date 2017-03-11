/*
 * COMMAND2.C:
 *
 *	Addition user command routines.
 *
 *	Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"

/**********************************************************************/
/*				look				      */
/**********************************************************************/

/* This function is called when a player tries to look at the room he */
/* is in, or at an object in the room or in his inventory.            */

int look(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	room		*rom_ptr;
	object		*obj_ptr;
	creature	*crt_ptr;
	char		str[2048];
	int		fd, n, match=0;

	if(cmnd->num < 2) {
		display_rom(ply_ptr, ply_ptr->parent_rom);
		return(0);
	}

	fd = ply_ptr->fd;
	if(F_ISSET(ply_ptr, PBLIND)) {
		ANSI(fd, BOLD);
		ANSI(fd, RED);
		print(fd, "You're blind!\n");
		ANSI(fd, WHITE);
		ANSI(fd, NORMAL);
		return(0);
	}
	rom_ptr = ply_ptr->parent_rom;

	obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj, cmnd->str[1], 
			   cmnd->val[1]);

	if(!obj_ptr || !cmnd->val[1]) {
		for(n=0; n<MAXWEAR; n++) {
			if(!ply_ptr->ready[n])
				continue;
			if(EQUAL(ply_ptr->ready[n], cmnd->str[1]))
				match++;
			else continue;
			if(cmnd->val[1] == match || !cmnd->val[1]) {
				obj_ptr = ply_ptr->ready[n];
				break;
			}
		}
	}

	if(!obj_ptr)
		obj_ptr = find_obj(ply_ptr, rom_ptr->first_obj,
				   cmnd->str[1], cmnd->val[1]);

	if(obj_ptr) {

		if(obj_ptr->special) {
			n = special_obj(ply_ptr, cmnd, SP_MAPSC);
			if(n != -2) return(MAX(n, 0));
		}

		if(obj_ptr->description[0])
			print(fd, "%s\n", obj_ptr->description);
		else
			print(fd, "You see nothing special about it.\n");

		if(F_ISSET(ply_ptr, PKNOWA)) {
			if(F_ISSET(obj_ptr, OGOODO))
				print(fd, "It has a blue aura.\n");
			if(F_ISSET(obj_ptr, OEVILO))
				print(fd, "It has a red aura.\n");
		}

		if(F_ISSET(obj_ptr, OCONTN)) {
			strcpy(str, "It contains: ");
			n = list_obj(&str[13], ply_ptr, obj_ptr->first_obj);
			if(n)
				print(fd, "%s.\n", str);
		}

		if(obj_ptr->type <= MISSILE) {
			print(fd, "%I is a ", obj_ptr);
			switch(obj_ptr->type) {
			case SHARP: print(fd, "sharp"); break;
			case THRUST: print(fd, "thrusting"); break;
			case POLE: print(fd, "pole"); break;
			case BLUNT: print(fd, "blunt"); break;
			case MISSILE: print(fd, "missile"); break;
			}
			print(fd, " weapon.\n");
		}

		if(obj_ptr->type <= MISSILE || obj_ptr->type == ARMOR ||
		   obj_ptr->type == LIGHTSOURCE || obj_ptr->type == WAND ||
		   obj_ptr->type == KEY) { 
			if(obj_ptr->shotscur < 1)
				print(fd, "It's broken or used up.\n");
			else if(obj_ptr->shotscur <= obj_ptr->shotsmax/10)
				print(fd, "It looks about ready to break.\n");
		}

		return(0);
	}

	crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[1],
			   cmnd->val[1]);
	if(crt_ptr) {

		print(fd, "You see %1m.\n", crt_ptr);
		if(crt_ptr->description[0])
			print(fd, "%s\n", crt_ptr->description);
		else
			print(fd, "There is nothing special about it.\n");
		if(F_ISSET(ply_ptr, PKNOWA)) {
			print(fd, "%s ",
				F_ISSET(crt_ptr, MMALES) ? "He":"She");
			if(crt_ptr->alignment < 0)
				print(fd, "has a red aura.\n");
			else if(crt_ptr->alignment == 0)
				print(fd, "has a grey aura.\n");
			else print(fd, "has a blue aura.\n");
		}
		if(crt_ptr->hpcur < (crt_ptr->hpmax*3)/10)
			print(fd, "%s has some nasty wounds.\n",
			      F_ISSET(crt_ptr, MMALES) ? "He":"She");
		if(is_enm_crt(ply_ptr->name, crt_ptr)) 
			print(fd, "%s looks very angry at you.\n",
			      F_ISSET(crt_ptr, MMALES) ? "He":"She");
		if(crt_ptr->first_enm) {
			if(!strcmp(crt_ptr->first_enm->enemy, ply_ptr->name))
				print(fd, "%s is attacking you.\n",
				      F_ISSET(crt_ptr, MMALES) ? "He":"She");
			else
				print(fd, "%s is attacking %s.\n",
				      F_ISSET(crt_ptr, MMALES) ? "He":"She",
				      crt_ptr->first_enm->enemy);
		}
		consider(ply_ptr, crt_ptr);
		equip_list(fd, crt_ptr);
		return(0);
	}

	cmnd->str[1][0] = up(cmnd->str[1][0]);
	crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply, cmnd->str[1],
			   cmnd->val[1]);

	if(crt_ptr) {
		print(fd, "You see %s the %s %s.\n", crt_ptr->name,
		      race_adj[crt_ptr->race], title_ply(crt_ptr));
		if(F_ISSET(ply_ptr, PKNOWA)) {
			print(fd, "%s ",
				F_ISSET(crt_ptr, MMALES) ? "He":"She");
			if(crt_ptr->alignment < -100)
				print(fd, "has a red aura.\n");
			else if(crt_ptr->alignment < 101)
				print(fd, "has a grey aura.\n");
			else print(fd, "has a blue aura.\n");
		}
		if(crt_ptr->hpcur < (crt_ptr->hpmax*3)/10)
			print(fd, "%s has some nasty wounds.\n",
			      F_ISSET(crt_ptr, PMALES) ? "He":"She");
		equip_list(fd, crt_ptr);
		return(0);
	}

	else
		print(fd, "You don't see that here.\n");

	return(0);

}

/**********************************************************************/
/*				move				      */
/**********************************************************************/

/* This function takes the player using the socket descriptor specified */
/* in the first parameter, and attempts to move him in one of the six   */
/* cardinal directions (n,s,e,w,u,d) if possible.			*/

int move(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	room		*rom_ptr, *old_rom_ptr;
	creature	*crt_ptr;
	char		tempstr[10];
	char		*str;
	ctag		*cp, *temp;
	xtag		*xp;
	int		found=0, fd, old_rom_num, fall, dmg, n;
	long		t;

	rom_ptr = ply_ptr->parent_rom;
	fd = ply_ptr->fd;
	str = cmnd->str[0];

	t = time(0);
	if(ply_ptr->lasttime[LT_MOVED].ltime == t) {
		if(++ply_ptr->lasttime[LT_MOVED].misc > 3) {
			please_wait(fd, 1);
			return(0);
		}
	}
	else if(ply_ptr->lasttime[LT_ATTCK].ltime+3 > t) {
		please_wait(fd, 3-t+ply_ptr->lasttime[LT_ATTCK].ltime);
		return(0);
	}
	else if(ply_ptr->lasttime[LT_SPELL].ltime+3 > t) {
		please_wait(fd, 3-t+ply_ptr->lasttime[LT_SPELL].ltime);
		return(0);
	}
	else {
		ply_ptr->lasttime[LT_MOVED].ltime = t;
		ply_ptr->lasttime[LT_MOVED].misc = 1;
	}
		
	if(!strcmp(str, "sw") || !strncmp(str, "southw", 6))
		strcpy(tempstr, "southwest");
	else if(!strcmp(str, "nw") || !strncmp(str, "northw", 6))
		strcpy(tempstr, "northwest");
	else if(!strcmp(str, "se") || !strncmp(str, "southe", 6))
		strcpy(tempstr, "southeast");
	else if(!strcmp(str, "ne") || !strncmp(str, "northe", 6))
		strcpy(tempstr, "northeast");
	else {
		switch(str[0]) {
		case 'n': strcpy(tempstr, "north"); break;
		case 's': strcpy(tempstr, "south"); break;
		case 'e': strcpy(tempstr, "east");  break;
		case 'w': strcpy(tempstr, "west");  break;
		case 'u': strcpy(tempstr, "up");    break;
		case 'd': strcpy(tempstr, "down");  break;
		case 'o':
		case 'l': strcpy(tempstr, "out");   break;
		}
	}

	xp = rom_ptr->first_ext;
	while(xp) {
		if(!strcmp(xp->ext->name, tempstr) && 
			!F_ISSET(xp->ext,XNOSEE)){
			found = 1;
			break;
		}
		xp = xp->next_tag;
	}

	if(!found) {
		print(fd, "You can't go that way.\n");
		return(0);
	}

	if(F_ISSET(xp->ext, XLOCKD)) {
		print(fd, "It's locked.\n");
		return(0);
	}
	else if(F_ISSET(xp->ext, XCLOSD)) {
		print(fd, "You have to open it first.\n");
		return(0);
	}

	if(F_ISSET(xp->ext, XFLYSP) && !F_ISSET(ply_ptr, PFLYSP)) {
		print(fd, "You must fly to get there.\n");
		return(0);
	}
 
        t = Time%24L;
	if(F_ISSET(xp->ext, XNGHTO) && (t>6 && t < 20)) {
       		print(fd, "That exit is open only at night.\n");
        	return(0);
    	}          

	if(F_ISSET(xp->ext, XDAYON) && (t<6 || t > 20)){
        	print(fd, "That exit is closed for the night.\n");
        	return(0);
    	}          
 
    if(F_ISSET(xp->ext,XPGUAR)){
        cp = rom_ptr->first_mon;
        while(cp) {
            if(F_ISSET(cp->crt, MPGUAR)) {
           	if(!F_ISSET(ply_ptr, PINVIS) && ply_ptr->class < CARETAKER) {
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

	if(F_ISSET(xp->ext, XPLDGK)) 
	 if (!F_ISSET(ply_ptr, PPLDGK)){
       		print(fd, "You do not have the proper authorization to go there.\n");
        	return(0);
	 }
	 else if (BOOL(F_ISSET(xp->ext,XKNGDM)) !=  BOOL(F_ISSET(ply_ptr, PKNGDM))){
        	print(fd, "You belong to the wrong organization to go there.\n");
        	return(0);
      	 }          

	if(F_ISSET(xp->ext, XFEMAL) && F_ISSET(ply_ptr, PMALES)){
		print(fd, "Sorry, only females are allowed to go there.\n");
		return(0); 
	}

	if(F_ISSET(xp->ext, XMALES) && !F_ISSET(ply_ptr, PMALES)){
		print(fd, "Sorry, only males are allowed to go there.\n");
		return(0); 
	}           

	if(F_ISSET(xp->ext, XNAKED) && weight_ply(ply_ptr)) {
		print(fd, "You cannot bring anything through that exit.\n");
		return(0);
	}

	if((F_ISSET(xp->ext, XCLIMB) || F_ISSET(xp->ext, XREPEL)) &&
	   !F_ISSET(ply_ptr, PLEVIT)) {
		fall = (F_ISSET(xp->ext, XDCLIM) ? 50:0) + 50 - 
		       fall_ply(ply_ptr);

		if(mrand(1,100) < fall) {
			dmg = mrand(5,15+fall/10);
			if(ply_ptr->hpcur <= dmg){
				print(fd, "You fell to your death.\n");
				ply_ptr->hpcur=0;
				broadcast_rom(fd, ply_ptr->rom_num, "%M died from the fall.\n", ply_ptr);
				die(ply_ptr, ply_ptr);
				return(0);
			}
			ply_ptr->hpcur -= dmg;
			print(fd, "You fell and hurt yourself for %d damage.\n",
			      dmg);
			broadcast_rom(fd, ply_ptr->rom_num, "%M fell down.", 
				      ply_ptr);
			if(F_ISSET(xp->ext, XCLIMB))
				return(0);
		}
	}

	F_CLR(ply_ptr, PHIDDN);

	cp = rom_ptr->first_mon;
	while(cp) {
		if(F_ISSET(cp->crt, MBLOCK)
		   && is_enm_crt(ply_ptr->name, cp->crt)
		   && !F_ISSET(ply_ptr, PINVIS)
		   && ply_ptr->class < CARETAKER) {
			print(fd, "%M blocks your exit.\n", cp->crt);
			return(0);
		}
		cp = cp->next_tag;
	}

	if(!F_ISSET(rom_ptr, RPTRAK))
		strcpy(rom_ptr->track, tempstr);

	old_rom_num = rom_ptr->rom_num;
	old_rom_ptr = rom_ptr;

	load_rom(xp->ext->room, &rom_ptr);
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
	if(ply_ptr->class == DM && !F_ISSET(ply_ptr, PDMINV)){
		broadcast_rom(fd, old_rom_ptr->rom_num, "%M just wandered to the %s.",
                              ply_ptr, tempstr);
        }
	if(!F_ISSET(ply_ptr, PDMINV) && ply_ptr->class < DM) {
		broadcast_rom(fd, old_rom_ptr->rom_num, "%M leaves %s.", 
			      ply_ptr, tempstr);
	}

	del_ply_rom(ply_ptr, ply_ptr->parent_rom);
	add_ply_rom(ply_ptr, rom_ptr);


	cp = ply_ptr->first_fol;
	while(cp) {
		if(cp->crt->rom_num == old_rom_num && cp->crt->type != MONSTER)
			move(cp->crt, cmnd);
		if(F_ISSET(cp->crt, MDMFOL) && cp->crt->rom_num == old_rom_num && cp->crt->type == MONSTER) {
		del_crt_rom(cp->crt, old_rom_ptr);
		broadcast_rom(fd, old_rom_ptr->rom_num, "%M just wandered to the %s.\n", cp->crt, tempstr); 
		add_crt_rom(cp->crt, rom_ptr, 1);
                add_active(cp->crt);
	        }	
	cp = cp->next_tag;
	}

	if(is_rom_loaded(old_rom_num)) {
		cp = old_rom_ptr->first_mon;
		while(cp) {
			if(!F_ISSET(cp->crt, MFOLLO) || F_ISSET(cp->crt, MDMFOL)) {
				cp = cp->next_tag;
				continue;
			}
			if(!cp->crt->first_enm) {
				cp = cp->next_tag;
				continue;
			}
			if(strcmp(cp->crt->first_enm->enemy, ply_ptr->name)) {
				cp = cp->next_tag;
				continue;
			}
			if(mrand(1,20) > 15 - ply_ptr->dexterity +
			   cp->crt->dexterity) {
				cp = cp->next_tag;
				continue;
			}
			print(fd, "%M followed you.\n", cp->crt);
			broadcast_rom(fd, old_rom_num, "%M follows %m.",
				      cp->crt, ply_ptr);
			temp = cp->next_tag;
			crt_ptr = cp->crt;
			if(F_ISSET(crt_ptr, MPERMT))
				die_perm_crt(crt_ptr);
			del_crt_rom(crt_ptr, old_rom_ptr);
			add_crt_rom(crt_ptr, rom_ptr, 1);
			add_active(crt_ptr);
			F_CLR(crt_ptr, MPERMT);
			cp = temp;

		}
	}

	check_traps(ply_ptr, rom_ptr);
	return(0);
}

/**********************************************************************/
/*				say				      */
/**********************************************************************/

/* This function allows the player specified by the socket descriptor */
/* in the first parameter to say something to all the other people in */
/* the room.							      */

int say(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	room		*rom_ptr;
	int		index = -1, i, fd;
	int		len;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	len = strlen(cmnd->fullstr);
	for(i=0; i<len && i < 256; i++) {
		if(cmnd->fullstr[i] == ' ') {
			index = i+1;
			break;
		}
	}
	cmnd->fullstr[255] = 0;

	if(index == -1 || strlen(&cmnd->fullstr[index]) < 1) {
		print(fd, "Say what?\n");
		return(0);
	}
	if(F_ISSET(ply_ptr, PSILNC)) {
		print(fd, "Your lips move but no sound comes forth.\n");
		return(0);
	}

	F_CLR(ply_ptr, PHIDDN);
	if(F_ISSET(ply_ptr, PLECHO)){
                ANSI(fd, CYAN);
                print(fd, "You say, \"%s\"\n", &cmnd->fullstr[index]);
                ANSI(fd, NORMAL);
        }
        else
		print(fd, "Ok.\n");

	broadcast_rom(fd, rom_ptr->rom_num, "%M says, \"%s\".", 
		      ply_ptr, &cmnd->fullstr[index]);

	return(0);

}

/**********************************************************************/
/*				get				      */
/**********************************************************************/

/* This function allows players to get things lying on the floor or   */
/* inside another object.					      */

int get(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	room		*rom_ptr;
	object		*obj_ptr, *cnt_ptr;
	ctag		*cp;
	int		fd, n, match=0;

	fd = ply_ptr->fd;

	if(cmnd->num < 2) {
		print(fd, "Get what?\n");
		return(0);
	}

	rom_ptr = ply_ptr->parent_rom;
	F_CLR(ply_ptr, PHIDDN);

	if(cmnd->num == 2) {

		cp = rom_ptr->first_mon;
		while(cp) {
			if(F_ISSET(cp->crt, MGUARD))
				break;
			cp = cp->next_tag;
		}

		if(cp && ply_ptr->class < CARETAKER) {
			print(fd, "%M won't let you take anything.\n", cp->crt);
			return(0);
		}
		if(F_ISSET(ply_ptr, PBLIND)) {
			print(fd, "You can't see to do that!\n");
			return(0);
		}
		if(!strcmp(cmnd->str[1], "all")) {
			get_all_rom(ply_ptr);
			return(0);
		}

		obj_ptr = find_obj(ply_ptr, rom_ptr->first_obj,
				   cmnd->str[1], cmnd->val[1]);

		if(!obj_ptr) {
			print(fd, "That isn't here.\n");
			return(0);
		}

		if(F_ISSET(obj_ptr, OINVIS)) {
			print(fd, "That isn't here.\n");
			return(0);
		}

		if(F_ISSET(obj_ptr, ONOTAK) || F_ISSET(obj_ptr, OSCENE)) {
			print(fd, "You can't take that!\n");
			return(0);
		}

		if(weight_ply(ply_ptr) + weight_obj(obj_ptr) > 
		   max_weight(ply_ptr)) {
			print(fd, "You can't carry anymore.\n");
			return(0);
		}

		if(obj_ptr->questnum && Q_ISSET(ply_ptr, obj_ptr->questnum-1)) {
			print(fd, "You may not take that. %s.\n",
			      "You have already fulfilled that quest");
			return(0);
		}

		if(F_ISSET(obj_ptr, OTEMPP)) {
			F_CLR(obj_ptr, OPERM2);
			F_CLR(obj_ptr, OTEMPP);
		}

		if(F_ISSET(obj_ptr, OPERMT))
			get_perm_obj(obj_ptr);

		F_CLR(obj_ptr, OHIDDN);
		del_obj_rom(obj_ptr, rom_ptr);
		print(fd, "You get %1i.\n", obj_ptr);
		if(obj_ptr->questnum) {
			print(fd, "Quest fulfilled!  Don't drop it.\n");
			print(fd, "You won't be able to pick it up again.\n");			
			Q_SET(ply_ptr, obj_ptr->questnum-1);
			ply_ptr->experience += quest_exp[obj_ptr->questnum-1];
			print(fd, "%ld experience granted.\n",
				quest_exp[obj_ptr->questnum-1]);
			add_prof(ply_ptr,quest_exp[obj_ptr->questnum-1]);
		}
		broadcast_rom(fd, rom_ptr->rom_num, "%M gets %1i.",
			      ply_ptr, obj_ptr);

		if(obj_ptr->type == MONEY) {
			ply_ptr->gold += obj_ptr->value;
			free_obj(obj_ptr);
			print(fd, "You now have %ld gold pieces.\n", 
				ply_ptr->gold);
		}
		else
			add_obj_crt(obj_ptr, ply_ptr);
		return(0);
	}

	else {

		cnt_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
				   cmnd->str[2], cmnd->val[2]);

		if(!cnt_ptr)
			cnt_ptr = find_obj(ply_ptr, rom_ptr->first_obj,
					   cmnd->str[2], cmnd->val[2]);

		if(!cnt_ptr || !cmnd->val[2]) {
			for(n=0; n<MAXWEAR; n++) {
				if(!ply_ptr->ready[n]) continue;
				if(EQUAL(ply_ptr->ready[n], cmnd->str[2]))
					match++;
				else continue;
				if(match == cmnd->val[2] || !cmnd->val[2]) {
					cnt_ptr = ply_ptr->ready[n];
					break;
				}
			}
		}

		if(!cnt_ptr) {
			print(fd, "That isn't here.\n");
			return(0);
		}

		if(!F_ISSET(cnt_ptr, OCONTN)) {
			print(fd, "That isn't a container.\n");
			return(0);
		}

		if(!strcmp(cmnd->str[1], "all")) {
			get_all_obj(ply_ptr, cnt_ptr);
			return(0);
		}

		obj_ptr = find_obj(ply_ptr, cnt_ptr->first_obj,
				   cmnd->str[1], cmnd->val[1]);

		if(!obj_ptr) {
			print(fd, "That isn't in there.\n");
			return(0);
		}

		if(weight_ply(ply_ptr) + weight_obj(obj_ptr) >
		   max_weight(ply_ptr) && cnt_ptr->parent_rom) {
			print(fd, "You can't carry anymore.\n");
			return(0);
		}

		if(F_ISSET(obj_ptr, OPERMT))
			get_perm_obj(obj_ptr);

		cnt_ptr->shotscur--;
		del_obj_obj(obj_ptr, cnt_ptr);
		print(fd, "You get %1i from %1i.\n", obj_ptr, cnt_ptr);
		broadcast_rom(fd, rom_ptr->rom_num, "%M gets %1i from %1i.",
			      ply_ptr, obj_ptr, cnt_ptr);

		if(obj_ptr->type == MONEY) {
			ply_ptr->gold += obj_ptr->value;
			free_obj(obj_ptr);
			print(fd, "You now have %ld gold pieces.\n",
				ply_ptr->gold);
		}
		else
			add_obj_crt(obj_ptr, ply_ptr);
	
		return(0);
	}

}

/**********************************************************************/
/*				get_all_rom			      */
/**********************************************************************/

/* This function will cause the player pointed to by the first parameter */
/* to get everything he is able to see in the room.			 */

void get_all_rom(ply_ptr)
creature	*ply_ptr;
{
	room	*rom_ptr;
	object	*obj_ptr, *last_obj;
	otag	*op;
	char	str[2048];
	int 	fd, n = 1, found = 0, heavy = 0, dogoldmsg = 0;

	last_obj = 0; str[0] = 0;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	op = rom_ptr->first_obj;
	while(op) {
		if(!F_ISSET(op->obj, OSCENE) &&
		   !F_ISSET(op->obj, ONOTAK) && !F_ISSET(op->obj, OHIDDN) && 
		   (F_ISSET(ply_ptr, PDINVI) ? 1:!F_ISSET(op->obj, OINVIS))) {
			found++;
			obj_ptr = op->obj;
			op = op->next_tag;
			if(weight_ply(ply_ptr) + weight_obj(obj_ptr) >
			  max_weight(ply_ptr)) {
				heavy++;
				continue;
			}
			if(obj_ptr->questnum && 
			   Q_ISSET(ply_ptr, obj_ptr->questnum-1)) {
				heavy++;
				continue;
			}
			if(F_ISSET(obj_ptr, OTEMPP)) {
				F_CLR(obj_ptr, OPERM2);
				F_CLR(obj_ptr, OTEMPP);
			}
			if(F_ISSET(obj_ptr, OPERMT))
				get_perm_obj(obj_ptr);
			F_CLR(obj_ptr, OHIDDN);
			if(obj_ptr->questnum) {
				print(fd,"Quest fulfilled!  Don't drop it.\n");
				print(fd,"You won't be able to pick it up again.\n");		
				Q_SET(ply_ptr, obj_ptr->questnum-1);
				ply_ptr->experience += 
					quest_exp[obj_ptr->questnum-1];
				print(fd, "%ld experience granted.\n",
					quest_exp[obj_ptr->questnum-1]);
			add_prof(ply_ptr,quest_exp[obj_ptr->questnum-1]);
			}
			del_obj_rom(obj_ptr, rom_ptr);
			if(last_obj && !strcmp(last_obj->name, obj_ptr->name) &&
			   last_obj->adjustment == obj_ptr->adjustment)
				n++;
			else if(last_obj) {
				strcat(str, obj_str(last_obj, n, 0));
				strcat(str, ", ");
				n=1;
			}
			if(obj_ptr->type == MONEY) {
				strcat(str, obj_str(obj_ptr, 1, 0));
				strcat(str, ", ");
				ply_ptr->gold += obj_ptr->value;
				free_obj(obj_ptr);
				last_obj = 0;
				dogoldmsg = 1;
			}
			else {
				add_obj_crt(obj_ptr, ply_ptr);
				last_obj = obj_ptr;
			}
		}
		else
			op = op->next_tag;
	}

	if(found && last_obj)
		strcat(str, obj_str(last_obj, n, 0));
	else if(!found) {
		print(fd, "There's nothing here.\n");
		return;
	}

	if(dogoldmsg && !last_obj)
		str[strlen(str)-2] = 0;

	if(heavy) {
		print(fd, "You weren't able to carry everything.\n");
		if(heavy == found) return;
	}

	if(!strlen(str)) return;

	broadcast_rom(fd, rom_ptr->rom_num, "%M gets %s.", ply_ptr, str);
	print(fd, "You get %s.\n", str);
	if(dogoldmsg)
		print(fd, "You now have %ld gold pieces.\n",
			ply_ptr->gold);

}

/**********************************************************************/
/*				get_all_obj			      */
/**********************************************************************/

/* This function allows a player to get the entire contents from a 	  */
/* container object.  The player is pointed to by the first parameter and */
/* the container by the second.						  */

void get_all_obj(ply_ptr, cnt_ptr)
creature	*ply_ptr;
object		*cnt_ptr;
{
	room	*rom_ptr;
	object	*obj_ptr, *last_obj;
	otag	*op;
	char	str[2048];
	int	fd, n = 1, found = 0, heavy = 0;

	last_obj = 0; str[0] = 0;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	op = cnt_ptr->first_obj;
	while(op) {
		if(!F_ISSET(op->obj, OSCENE) &&
		   !F_ISSET(op->obj, ONOTAK) && !F_ISSET(op->obj, OHIDDN) && 
		   (F_ISSET(ply_ptr, PDINVI) ? 1:!F_ISSET(op->obj, OINVIS))) {
			found++;
			obj_ptr = op->obj;
			op = op->next_tag;
			if(weight_ply(ply_ptr) + weight_obj(obj_ptr) >
			   max_weight(ply_ptr)) {
				heavy++;
				continue;
			}
			if(F_ISSET(obj_ptr, OTEMPP)) {
				F_CLR(obj_ptr, OPERM2);
				F_CLR(obj_ptr, OTEMPP);
			}
			if(F_ISSET(obj_ptr, OPERMT))
				get_perm_obj(obj_ptr);
			cnt_ptr->shotscur--;
			del_obj_obj(obj_ptr, cnt_ptr);
			if(last_obj && !strcmp(last_obj->name, obj_ptr->name) &&
				last_obj->adjustment == obj_ptr->adjustment)
				n++;
			else if(last_obj) {
				strcat(str, obj_str(last_obj, n, 0));
				strcat(str, ", ");
				n = 1;
			}
			if(obj_ptr->type == MONEY) {
				ply_ptr->gold += obj_ptr->value;
				free_obj(obj_ptr);
				last_obj = 0;
				print(fd, "You now have %ld gold pieces.\n",
					ply_ptr->gold);
			}
			else {
				add_obj_crt(obj_ptr, ply_ptr);
				last_obj = obj_ptr;
			}
		}
		else
			op = op->next_tag;
	}

	if(found && last_obj)
		strcat(str, obj_str(last_obj, n, 0));
	else if(!found) {
		print(fd, "There's nothing in it.\n");
		return;
	}

	if(heavy) {
		print(fd, "You weren't able to carry everything.\n");
		if(heavy == found) return;
	}

	if(!strlen(str)) return;

	broadcast_rom(fd, rom_ptr->rom_num, "%M gets %s from %1i.",
		      ply_ptr, str, cnt_ptr);
	print(fd, "You get %s from %1i.\n", str, cnt_ptr);

}

/**********************************************************************/
/*				get_perm_obj			      */
/**********************************************************************/

/* This function is called whenever someone picks up a permanent item */
/* from a room.  The item's room-permanent flag is cleared, and the   */
/* inventory-permanent flag is set.  Also, the room's permanent	      */
/* time for that item is updated.				      */

void get_perm_obj(obj_ptr)
object	*obj_ptr;
{
	object	*temp_obj;
	room	*rom_ptr;
	long	t;
	int	i;

	t = time(0);

	F_SET(obj_ptr, OPERM2);
	F_CLR(obj_ptr, OPERMT);

	rom_ptr = obj_ptr->parent_rom;
	if(!rom_ptr) return;
	
	for(i=0; i<10; i++) {
		if(!rom_ptr->perm_obj[i].misc) continue;
		if(rom_ptr->perm_obj[i].ltime + rom_ptr->perm_obj[i].interval >
		   t) continue;
		if(load_obj(rom_ptr->perm_obj[i].misc, &temp_obj) < 0)
			continue;
		if(!strcmp(temp_obj->name, obj_ptr->name)) {
			rom_ptr->perm_obj[i].ltime = t;
			free_obj(temp_obj);
			break;
		}
		free_obj(temp_obj);
	}
}

/**********************************************************************/
/*				inventory			      */
/**********************************************************************/

/* This function outputs the contents of a player's inventory.        */

int inventory(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	otag		*op;
	char		str[2048];
	int		m, n, fd, flags = 0;

	fd = ply_ptr->fd;

	if(F_ISSET(ply_ptr, PBLIND)) {
		ANSI(fd, BLUE);
		print(fd, "You're blind as a bat...how can you do that?\n");
		ANSI(fd, WHITE);
		return(0);
	}		
	if(F_ISSET(ply_ptr, PDINVI))
		flags |= INV;
	if(F_ISSET(ply_ptr, PDMAGI))
		flags |= MAG;

	op = ply_ptr->first_obj; n=0; str[0]=0;
	strcat(str, "You have: ");
	if(!op) {
		strcat(str, "nothing.");
		print(fd, "%s\n", str);
		return(0);
	}
	while(op) {
		if(F_ISSET(ply_ptr, PDINVI) ? 1:!F_ISSET(op->obj, OINVIS)) {
			m=1;
			while(op->next_tag) {
				if(!strcmp(op->next_tag->obj->name, 
					   op->obj->name) &&
				   op->next_tag->obj->adjustment ==
				   op->obj->adjustment &&
				   (F_ISSET(ply_ptr, PDINVI) ? 
				   1:!F_ISSET(op->next_tag->obj, OINVIS))) {
					m++;
					op = op->next_tag;
				}
				else
					break;
			}
			strcat(str, obj_str(op->obj, m, flags));
			strcat(str, ", ");
			n++;
		}
		op = op->next_tag;
	}
	if(n) {
		str[strlen(str)-2] = 0;
		print(fd, "%s.\n", str);
	}

	return(0);

}

/**********************************************************************/
/*				drop			   	      */
/**********************************************************************/

/* This function allows the player pointed to by the first parameter */
/* to drop an object in the room at which he is located.             */

int drop(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	room		*rom_ptr;
	object		*obj_ptr, *cnt_ptr;
	int		fd, n, match=0;

	fd = ply_ptr->fd;

	if(cmnd->num < 2) {
		print(fd, "Drop what?\n");
		return(0);
	}

	rom_ptr = ply_ptr->parent_rom;
	F_CLR(ply_ptr, PHIDDN);

	if(cmnd->num == 2) {

		if(!strcmp(cmnd->str[1], "all")) {
			drop_all_rom(ply_ptr);
			return(0);
		}

		obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
				   cmnd->str[1], cmnd->val[1]);

		if(!obj_ptr) {
			print(fd, "You don't have that.\n");
			return(0);
		}

		del_obj_crt(obj_ptr, ply_ptr);
		print(fd, "You drop %1i.\n", obj_ptr);
		broadcast_rom(fd, rom_ptr->rom_num, "%M dropped %1i.",
			      ply_ptr, obj_ptr);
		if(!F_ISSET(rom_ptr, RDUMPR))
			add_obj_rom(obj_ptr, rom_ptr);
		else {
			free_obj(obj_ptr);
			ply_ptr->gold += 5;
			print(fd, "Thanks for recycling.\nYou have %-ld gold.\n", ply_ptr->gold);
		}
		return(0);
	}

	else {

		cnt_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
				   cmnd->str[2], cmnd->val[2]);

		if(!cnt_ptr)
			cnt_ptr = find_obj(ply_ptr, rom_ptr->first_obj,
					    cmnd->str[2], cmnd->val[2]);

		if(!cnt_ptr || !cmnd->val[2]) {
			for(n=0; n<MAXWEAR; n++) {
				if(!ply_ptr->ready[n]) continue;
				if(EQUAL(ply_ptr->ready[n], cmnd->str[2]))
					match++;
				else continue;
				if(match == cmnd->val[2] || !cmnd->val[2]) {
					cnt_ptr = ply_ptr->ready[n];
					break;
				}
			}
		}
	
		if(!cnt_ptr) {
			print(fd, "You don't see that here.\n");
			return(0);
		}

		if(!F_ISSET(cnt_ptr, OCONTN)) {
			print(fd, "That isn't a container.\n");
			return(0);
		}

		if(!strcmp(cmnd->str[1], "all")) {
			drop_all_obj(ply_ptr, cnt_ptr);
			return(0);
		}

		obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
				   cmnd->str[1], cmnd->val[1]);

		if(!obj_ptr) {
			print(fd, "You don't have that.\n");
			return(0);
		}

		if(obj_ptr == cnt_ptr) {
			print(fd, "You can't put something in itself!\n");
			return(0);
		}



		if(cnt_ptr->shotscur >= cnt_ptr->shotsmax) {
			print(fd, "%I can't hold anymore.\n", cnt_ptr);
			return(0);
		}

		if(F_ISSET(obj_ptr, OCONTN)) {
			print(fd, "You can't put containers into containers.\n");
			return(0);
		}

                if(F_ISSET(cnt_ptr, OCNDES)) { 
		print(fd, "%1i is devoured by %1i!\n", obj_ptr, cnt_ptr);
		broadcast_rom(fd, rom_ptr->rom_num, "%M put %1i in %1i.",
			      ply_ptr, obj_ptr, cnt_ptr);
		del_obj_crt(obj_ptr, ply_ptr);
		free(obj_ptr);
			return(0);
		}

		del_obj_crt(obj_ptr, ply_ptr);
		add_obj_obj(obj_ptr, cnt_ptr);
		cnt_ptr->shotscur++;
		print(fd, "You put %1i in %1i.\n", obj_ptr, cnt_ptr);
		broadcast_rom(fd, rom_ptr->rom_num, "%M put %1i in %1i.",
			      ply_ptr, obj_ptr, cnt_ptr);
		return(0);
	}

}

/**********************************************************************/
/*				drop_all_rom			      */
/**********************************************************************/

/* This function is called when a player wishes to drop his entire    */
/* inventory into the the room.					      */

void drop_all_rom(ply_ptr)
creature	*ply_ptr;
{
	object	*obj_ptr;
	room	*rom_ptr;
	otag	*op;
	char	str[2048], str2[2048];
	int	fd, found;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	found = list_obj(str, ply_ptr, ply_ptr->first_obj);

	if(!found) {
		print(fd, "You don't have anything.\n");
		return;
	}

	op = ply_ptr->first_obj;
	while(op) {
		if(F_ISSET(ply_ptr, PDINVI) ? 1:!F_ISSET(op->obj, OINVIS)) {
			obj_ptr = op->obj;
			op = op->next_tag;
			del_obj_crt(obj_ptr, ply_ptr);
			if(!F_ISSET(rom_ptr, RDUMPR))
				add_obj_rom(obj_ptr, rom_ptr);
			else {
				free_obj(obj_ptr);
				ply_ptr->gold+=5;
			}
		}
		else
			op = op->next_tag;
	}

	broadcast_rom(fd, rom_ptr->rom_num, "%M dropped %s.", ply_ptr, str);
	print(fd, "You drop %s.\n", str);

	if(F_ISSET(rom_ptr, RDUMPR))
		print(fd, "Thanks for recycling.\nYou have %-ld gold.\n",ply_ptr->gold);
}

/**********************************************************************/
/*				drop_all_obj			      */
/**********************************************************************/

/* This function drops all the items in a player's inventory into a	 */
/* container object, if possible.  The player is pointed to by the first */
/* parameter, and the container by the second.				 */

void drop_all_obj(ply_ptr, cnt_ptr)
creature	*ply_ptr;
object		*cnt_ptr;
{
	object	*obj_ptr, *last_obj;
	room	*rom_ptr;
	otag	*op;
	char	str[2048];
	int	fd, n = 1, found = 0, full = 0;

	fd = ply_ptr->fd;

	last_obj = 0; str[0] = 0;
	rom_ptr = ply_ptr->parent_rom;

	op = ply_ptr->first_obj;
	while(op) {
		if((F_ISSET(ply_ptr, PDINVI) ? 1:!F_ISSET(op->obj, OINVIS)) &&
		   op->obj != cnt_ptr) {
			found++;
			obj_ptr = op->obj;
			op = op->next_tag;
			if(F_ISSET(obj_ptr, OCONTN)) {
				full++;
				continue;
			}
			if(F_ISSET(cnt_ptr, OCNDES))
			{
				full++;
				del_obj_crt(obj_ptr, ply_ptr);
				free(obj_ptr);
				continue;

			}
	
			if(cnt_ptr->shotscur >= cnt_ptr->shotsmax) {
				full++;
				continue;
			}
			cnt_ptr->shotscur++;
			del_obj_crt(obj_ptr, ply_ptr);
			add_obj_obj(obj_ptr, cnt_ptr);
			if(last_obj && !strcmp(last_obj->name, obj_ptr->name) &&
			   last_obj->adjustment == obj_ptr->adjustment)
				n++;
			else if(last_obj) {
				strcat(str, obj_str(last_obj, n, 0));
				strcat(str, ", ");
				n = 1;
			}
			last_obj = obj_ptr;
		}
		else
			op = op->next_tag;
	}

	if(found && last_obj)
		strcat(str, obj_str(last_obj, n, 0));
	else {
		print(fd, "You don't have anything to put into it.\n");
		return;
	}

	if(full)
		print(fd, "%I couldn't hold everything.\n", cnt_ptr);

	if(full == found) return;

	broadcast_rom(fd, rom_ptr->rom_num, "%M put %s into %1i.", ply_ptr,
		      str, cnt_ptr);
	print(fd, "You put %s into %1i.\n", str, cnt_ptr);

}
