/*
 * COMMAND3.C:
 *
 *	Additional user routines.
 *
 *	Copyright (C) 1991, 1992, 1993, 1997 Brooke Paul & Brett Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"
#ifdef DMALLOC
  #include "/usr/local/include/dmalloc.h"
#endif
/**********************************************************************/
/*				wear				      */
/**********************************************************************/

/* This function allows the player pointed to by the first parameter */
/* to wear an item specified in the pointer to the command structure */
/* in the second parameter.  That is, if the item is wearable.       */

int wear(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	room	*rom_ptr;
	object	*obj_ptr;
	int	fd, i, cantwear=0, class;

	fd = ply_ptr->fd;

	if(cmnd->num < 2) {
		print(fd, "Wear what?\n");
		return(0);
	}

	rom_ptr = ply_ptr->parent_rom;
	F_CLR(ply_ptr, PHIDDN);

	if(cmnd->num > 1) {

		if(!strcmp(cmnd->str[1], "all")) {
			wear_all(ply_ptr);
			return(0);
		}

		obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
				   cmnd->str[1], cmnd->val[1]);

		if(!obj_ptr) {
			print(fd, "You don't have that.\n");
			return(0);
		}

		if(!obj_ptr->wearflag || obj_ptr->wearflag == WIELD ||
		   obj_ptr->wearflag == HELD) {
			print(fd, "You can't wear that.\n");
			return(0);
		}

		if(obj_ptr->type == ARMOR && F_ISSET(obj_ptr, ONOMAG) &&
		   ply_ptr->class == MAGE) {
			print(fd, "Mages may not wear that.\n");
			return(0);
		}


		if(obj_ptr->type == ARMOR && F_ISSET(obj_ptr, ONOFEM) &&
		   !F_ISSET(ply_ptr,PMALES)) {
			print(fd, "Only males may wear that.\n");
			return(0);
		}

		if(obj_ptr->type == ARMOR && F_ISSET(obj_ptr, ONOMAL) &&
		   F_ISSET(ply_ptr,PMALES)) {
			print(fd, "Only females may wear that.\n");
			return(0);
		}

		if(obj_ptr->wearflag == NECK && ply_ptr->ready[NECK1-1] && 
		   ply_ptr->ready[NECK2-1]) {
			print(fd, "You can't wear anymore on your neck.\n");
			return(0);
		}

		if(obj_ptr->wearflag == FINGER && ply_ptr->ready[FINGER1-1] &&
		   ply_ptr->ready[FINGER2-1] && ply_ptr->ready[FINGER3-1] &&
		   ply_ptr->ready[FINGER4-1] && ply_ptr->ready[FINGER5-1] &&
		   ply_ptr->ready[FINGER6-1] && ply_ptr->ready[FINGER7-1] &&
		   ply_ptr->ready[FINGER8-1]) {
			print(fd, "You don't have any more fingers left.\n");
			return(0);
		}

		if(obj_ptr->wearflag != NECK && obj_ptr->wearflag != FINGER &&
		   ply_ptr->ready[obj_ptr->wearflag-1]) {
			print(fd, "You're already wearing something there.\n");
			return(0);
		}

		if(obj_ptr->shotscur < 1) {
			print(fd, "You can't.  It's broken.\n");
			return(0);
		}

		if(F_ISSET(obj_ptr, OGOODO) && ply_ptr->alignment < -50) {
			print(fd, "%I shocks you and you drop it.\n", obj_ptr);
			broadcast_rom(fd, ply_ptr->rom_num, 
				      "%M is shocked by %i.", ply_ptr, obj_ptr);
			del_obj_crt(obj_ptr, ply_ptr);
			add_obj_rom(obj_ptr, ply_ptr->parent_rom);
			return(0);
		}

		if(F_ISSET(obj_ptr, OEVILO) && ply_ptr->alignment > 50) {
			print(fd, "%I shocks you and you drop it.\n", obj_ptr);
			broadcast_rom(fd, ply_ptr->rom_num, 
				      "%M is shocked by %i.", ply_ptr, obj_ptr);
			del_obj_crt(obj_ptr, ply_ptr);
			add_obj_rom(obj_ptr, ply_ptr->parent_rom);
			return(0);
		}

	if (F_ISSET(obj_ptr,OPLDGK) &&
                (BOOL(F_ISSET(obj_ptr,OKNGDM)) != BOOL(F_ISSET(ply_ptr,PKNGDM)))){
        	print(fd, "You are unable to wear %i.\n",obj_ptr);
        	return(0);
    }              

if(ply_ptr->class > 10)
	class = ply_ptr->class + 11;
else
	class = ply_ptr->class;

      if(F_ISSET(obj_ptr,OCLSEL))
	if(!F_ISSET(obj_ptr,OCLSEL + class) && ( ply_ptr->class < CARETAKER)){
        	print(fd, "Your class prevents you from wearing %i.\n",obj_ptr);
        	return(0);
	}
	if(!F_ISSET(obj_ptr, OCLSEL + class) && obj_ptr->armor >5 && (ply_ptr->class == MONK || ply_ptr->class == MAGE)){
                print(fd, "Your class prevents you from wearing %i.\n",obj_ptr);
                return(0);
        }

	if((obj_ptr->wearflag == FINGER||obj_ptr->wearflag ==SHIELD) && ply_ptr->class == MONK) {
                print(fd, "Your class prevents you from wearing %i.\n",obj_ptr);
                return(0);
        }


		i = (F_ISSET(obj_ptr, OSIZE1) ? 1:0) * 2 +
			(F_ISSET(obj_ptr, OSIZE2) ? 1:0);

		switch(i) {
		case 1:
			if(ply_ptr->race != GNOME &&
			   ply_ptr->race != HOBBIT &&
			   ply_ptr->race != DWARF) cantwear = 1;
			break;
		case 2:
			if(ply_ptr->race != HUMAN &&
			   ply_ptr->race != ELF &&
			   ply_ptr->race != DARKELF &&
			   ply_ptr->race != GOBLIN &&
			   ply_ptr->race != HALFORC &&
			   ply_ptr->race != HALFELF &&
			   ply_ptr->race != ORC) cantwear = 1;
			break;
		case 3:
			if(ply_ptr->race != HALFGIANT && ply_ptr->race != OGRE && ply_ptr->race != TROLL) cantwear = 1;
			break;
		}

		if(cantwear && ply_ptr->class < CARETAKER) {
			print(fd, "%I doesn't fit you.\n", obj_ptr);
			return(0);
		}

		del_obj_crt(obj_ptr, ply_ptr);
		
		switch(obj_ptr->wearflag) {
		case BODY:
		case ARMS:
		case LEGS:
		case HANDS:
		case HEAD:
		case FEET:
		case FACE:
		case HELD:
		case SHIELD:
			ply_ptr->ready[obj_ptr->wearflag-1] = obj_ptr;
			break;
		case NECK:
			if(ply_ptr->ready[NECK1-1])
				ply_ptr->ready[NECK2-1] = obj_ptr;
			else
				ply_ptr->ready[NECK1-1] = obj_ptr;
			break;
		case FINGER:
			for(i=FINGER1; i<FINGER8+1; i++) {
				if(!ply_ptr->ready[i-1]) {
					ply_ptr->ready[i-1] = obj_ptr;
					break;
				}
			}
			break;
		}
	}

	compute_ac(ply_ptr);
	print(fd, "You wear %1i.\n", obj_ptr);
	broadcast_rom(fd, rom_ptr->rom_num, "%M wore %1i.", ply_ptr, obj_ptr);
	if(obj_ptr->use_output[0])
		print(fd, "%s\n", obj_ptr->use_output);
	F_SET(obj_ptr, OWEARS);
	if(F_ISSET(obj_ptr, OCURSE))
		F_SET(obj_ptr, OCURSW);
	return(0);

}

/**********************************************************************/
/*				wear_all			      */
/**********************************************************************/

/* This function allows a player to wear everything in his inventory that */
/* he possibly can.							  */

void wear_all(ply_ptr)
creature	*ply_ptr;
{
	object	*obj_ptr;
	otag	*op, *temp;
	char	str[4096], str2[85];
	int	fd, i, found=0, cantwear=0, class;

	str[0] = 0;
	fd = ply_ptr->fd;

	op = ply_ptr->first_obj;

	while(op) {

		temp = op->next_tag;

		if((F_ISSET(ply_ptr, PDINVI) ?
		   1:!F_ISSET(op->obj, OINVIS)) && op->obj->wearflag &&
		   op->obj->wearflag != HELD && op->obj->wearflag != WIELD) {

			obj_ptr = op->obj;

			if(obj_ptr->type == ARMOR && F_ISSET(obj_ptr, ONOMAG) &&
			   ply_ptr->class == MAGE) {
				op = temp;
				continue;
			}

		if(obj_ptr->type == ARMOR && F_ISSET(obj_ptr, ONOFEM) &&
		   !F_ISSET(ply_ptr,PMALES)) {
			op = temp;
			continue;
		}

		if(obj_ptr->type == ARMOR && F_ISSET(obj_ptr, ONOMAL) &&
		   F_ISSET(ply_ptr,PMALES)) {
			op = temp;
			continue;
		}


			if(obj_ptr->wearflag == NECK && 
			   ply_ptr->ready[NECK1-1] && 
			   ply_ptr->ready[NECK2-1]) {
				op = temp;
				continue;
			}
	
			if(obj_ptr->wearflag == FINGER && 
			   ply_ptr->ready[FINGER1-1] &&
			   ply_ptr->ready[FINGER2-1] && 
			   ply_ptr->ready[FINGER3-1] &&
			   ply_ptr->ready[FINGER4-1] && 
			   ply_ptr->ready[FINGER5-1] && 
			   ply_ptr->ready[FINGER6-1] && 
			   ply_ptr->ready[FINGER7-1] &&
			   ply_ptr->ready[FINGER8-1]) {
				op = temp;
				continue;
			}

			if(obj_ptr->wearflag != NECK && 
			   obj_ptr->wearflag != FINGER &&
			   ply_ptr->ready[obj_ptr->wearflag-1]) {
				op = temp;
				continue;
			}

			if(obj_ptr->shotscur < 1) {
				op = temp;
				continue;
			}


			if (F_ISSET(obj_ptr,OPLDGK) &&
   		     	(BOOL(F_ISSET(obj_ptr,OKNGDM)) != BOOL(F_ISSET(ply_ptr,PKNGDM)))){
				op = temp;
				continue;
	    	}              


if(ply_ptr->class > 10)
        class = ply_ptr->class + 11;
else
        class = ply_ptr->class;

      if(F_ISSET(obj_ptr,OCLSEL)) 
	if(!F_ISSET(obj_ptr,OCLSEL + class) && ( ply_ptr->class < CARETAKER)){
				op = temp;
				continue;
	}

	if(!F_ISSET(obj_ptr,OCLSEL + class) && (ply_ptr->class== MONK || ply_ptr->class == MAGE) && obj_ptr->armor >5){
                                op = temp;
                                continue;
        }
        if((obj_ptr->wearflag == FINGER||obj_ptr->wearflag ==SHIELD) && ply_ptr->class == MONK){
                                op=temp;
                                continue;
        }

			if(F_ISSET(obj_ptr, OGOODO) && 
			   ply_ptr->alignment < -50) {
				op = temp;
				continue;
			}

			if(F_ISSET(obj_ptr, OEVILO) && 
			   ply_ptr->alignment > 50) {
				op = temp;
				continue;
			}

			i = (F_ISSET(obj_ptr, OSIZE1) ? 1:0) * 2 +
				(F_ISSET(obj_ptr, OSIZE2) ? 1:0);

			switch(i) {
			case 1:
				if(ply_ptr->race != GNOME &&
				   ply_ptr->race != HOBBIT &&
				   ply_ptr->race != DWARF) cantwear = 1;
				break;
			case 2:
				if(ply_ptr->race != HUMAN &&
				   ply_ptr->race != ELF &&
				   ply_ptr->race != DARKELF &&
				   ply_ptr->race != GOBLIN &&
				   ply_ptr->race != HALFELF &&
				   ply_ptr->race != HALFORC &&
				   ply_ptr->race != ORC) cantwear = 1;
				break;
			case 3:
				if(ply_ptr->race != HALFGIANT && ply_ptr->race != OGRE && ply_ptr->race != TROLL) cantwear = 1;
				break;
			}

			if(cantwear) {
				op = temp;
				cantwear = 0;
				continue;
			}

			if(obj_ptr->wearflag == NECK) {
				if(ply_ptr->ready[NECK1-1])
					ply_ptr->ready[NECK2-1] = obj_ptr;
				else
					ply_ptr->ready[NECK1-1] = obj_ptr;
				F_SET(obj_ptr, OWEARS);
			}

			else if(obj_ptr->wearflag == FINGER && ply_ptr->class != MONK) {
				for(i=FINGER1; i<FINGER8+1; i++) {
					if(!ply_ptr->ready[i-1]) {
						ply_ptr->ready[i-1] = obj_ptr;
						F_SET(obj_ptr, OWEARS);
						break;
					}
				}
			}

			else {
				ply_ptr->ready[obj_ptr->wearflag-1] = obj_ptr;
				if(obj_ptr->use_output[0])
					print(fd, "%s\n", obj_ptr->use_output);
				F_SET(obj_ptr, OWEARS);
			}

			sprintf(str2, "%s, ", obj_str(obj_ptr, 1, 0));
			strcat(str, str2);
			del_obj_crt(obj_ptr, ply_ptr);
			if(F_ISSET(obj_ptr, OCURSE))
		              F_SET(obj_ptr, OCURSW);

			found = 1;

		}
		op = temp;
	}

	if(found) {
		compute_ac(ply_ptr);
		str[strlen(str)-2] = 0;
		print(fd, "You wear %s.\n", str);
		broadcast_rom(fd, ply_ptr->rom_num, "%M wears %s.",
			      ply_ptr, str);
	}
	else
		print(fd, "You have nothing you can wear.\n");

}

/**********************************************************************/
/*				remove				      */
/**********************************************************************/

/* This function allows the player pointed to by the first parameter to */
/* remove the item specified by the command structure in the second     */
/* parameter from those things which he is wearing.			*/

int remove_obj(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	room	*rom_ptr;
	object	*obj_ptr;
	int	fd, found=0, match=0, i;

	fd = ply_ptr->fd;

	if(cmnd->num < 2) {
		print(fd, "Remove what?\n");
		return(0);
	}

	rom_ptr = ply_ptr->parent_rom;
	F_CLR(ply_ptr, PHIDDN);

	i = 0;
	if(cmnd->num > 1) {

		if(!strcmp(cmnd->str[1], "all")) {
			remove_all(ply_ptr);
			return(0);
		}

		while(i<20) {
			obj_ptr = ply_ptr->ready[i];
			if(!obj_ptr) {
				i++;
				continue;
			}
			if(EQUAL(obj_ptr, cmnd->str[1]) &&
			   (F_ISSET(ply_ptr, PDINVI) ?
			   1:!F_ISSET(obj_ptr, OINVIS))) {
				match++;
				if(match == cmnd->val[1]) {
					found = 1;
					break;
				}
			}
			i++;
		}

		if(!found) {
			print(fd, "You aren't using that.\n");
			return(0);
		}

		if(F_ISSET(obj_ptr, OCURSE)) {
			print(fd, "You can't.  It's cursed!\n");
			return(0);
		}

		add_obj_crt(obj_ptr, ply_ptr);
		ply_ptr->ready[i] = 0;
		compute_ac(ply_ptr);
		compute_thaco(ply_ptr);

		print(fd, "You removed %1i.\n", obj_ptr);
		broadcast_rom(fd, rom_ptr->rom_num, "%M removed %1i.",
			      ply_ptr, obj_ptr);
		F_CLR(obj_ptr, OWEARS);
	}

	return(0);

}

/**********************************************************************/
/*				remove_all			      */
/**********************************************************************/

/* This function allows the player pointed to in the first parameter */
/* to remove everything he is wearing all at once.		     */

void remove_all(ply_ptr)
creature	*ply_ptr;
{
	char	str[4096], str2[85];
	int	fd, i, found=0;

	fd = ply_ptr->fd;
	str[0] = 0;

	for(i=0; i<MAXWEAR; i++) {
		if(ply_ptr->ready[i] && !F_ISSET(ply_ptr->ready[i], OCURSE)) {
			sprintf(str2,"%s, ", obj_str(ply_ptr->ready[i], 1, 0));
			strcat(str, str2);
			F_CLR(ply_ptr->ready[i], OWEARS);
			add_obj_crt(ply_ptr->ready[i], ply_ptr);
			ply_ptr->ready[i] = 0;
			found = 1;
		}
	}

	if(!found) {
		print(fd,"You aren't wearing anything that can be removed.\n");
		return;
	}

	compute_ac(ply_ptr);
	compute_thaco(ply_ptr);

	str[strlen(str)-2] = 0;
	broadcast_rom(fd, ply_ptr->rom_num, "%M removes %s.", ply_ptr, str);
	print(fd, "You remove %s.\n", str);

}

/**********************************************************************/
/*				equipment			      */
/**********************************************************************/

/* This function outputs to the player all of the equipment that he is */
/* wearing/wielding/holding on his body.			       */

int equipment(crt_ptr, cmnd)
creature	*crt_ptr;
cmd		*cmnd;
{
	int	fd = crt_ptr->fd;
	int	i, found=0;

	for(i=0; i<MAXWEAR; i++)
		if(crt_ptr->ready[i])
			found = 1;

	if(!found) {
		print(fd, "You aren't wearing anything.\n");
		return(0);
	}
	if(F_ISSET(crt_ptr, PBLIND)) {
                ANSI(fd, RED);
                print(fd, "You can't see anything...You're Blind!\n");
                ANSI(fd, WHITE);
                return(0);
        }	
	equip_list(fd, crt_ptr);
	return(0);

}

/**********************************************************************/
/*				equip_list			      */
/**********************************************************************/

void equip_list(fd, crt_ptr)
int		fd;
creature	*crt_ptr;
{

	if(crt_ptr->ready[BODY-1])
		print(fd, "On body:   %1i\n", crt_ptr->ready[BODY-1]);
	if(crt_ptr->ready[ARMS-1])
		print(fd, "On arms:   %1i\n", crt_ptr->ready[ARMS-1]);
	if(crt_ptr->ready[LEGS-1])
		print(fd, "On legs:   %1i\n", crt_ptr->ready[LEGS-1]);
	if(crt_ptr->ready[NECK1-1])
		print(fd, "On neck:   %1i\n", crt_ptr->ready[NECK1-1]);
	if(crt_ptr->ready[NECK2-1])
		print(fd, "On neck:   %1i\n", crt_ptr->ready[NECK2-1]);
	if(crt_ptr->ready[HANDS-1])
		print(fd, "On hands:  %1i\n", crt_ptr->ready[HANDS-1]);
	if(crt_ptr->ready[HEAD-1])
		print(fd, "On head:   %1i\n", crt_ptr->ready[HEAD-1]);
	if(crt_ptr->ready[FEET-1])
		print(fd, "On feet:   %1i\n", crt_ptr->ready[FEET-1]);
	if(crt_ptr->ready[FACE-1])
		print(fd, "On face:   %1i\n", crt_ptr->ready[FACE-1]);
	if(crt_ptr->ready[FINGER1-1])
		print(fd, "On finger: %1i\n", crt_ptr->ready[FINGER1-1]);
	if(crt_ptr->ready[FINGER2-1])
		print(fd, "On finger: %1i\n", crt_ptr->ready[FINGER2-1]);
	if(crt_ptr->ready[FINGER3-1])
		print(fd, "On finger: %1i\n", crt_ptr->ready[FINGER3-1]);
	if(crt_ptr->ready[FINGER4-1])
		print(fd, "On finger: %1i\n", crt_ptr->ready[FINGER4-1]);
	if(crt_ptr->ready[FINGER5-1])
		print(fd, "On finger: %1i\n", crt_ptr->ready[FINGER5-1]);
	if(crt_ptr->ready[FINGER6-1])
		print(fd, "On finger: %1i\n", crt_ptr->ready[FINGER6-1]);
	if(crt_ptr->ready[FINGER7-1])
		print(fd, "On finger: %1i\n", crt_ptr->ready[FINGER7-1]);
	if(crt_ptr->ready[FINGER8-1])
		print(fd, "On finger: %1i\n", crt_ptr->ready[FINGER8-1]);
	if(crt_ptr->ready[HELD-1])
		print(fd, "Holding:   %1i\n", crt_ptr->ready[HELD-1]);
	if(crt_ptr->ready[SHIELD-1])
		print(fd, "Shield:    %1i\n", crt_ptr->ready[SHIELD-1]);
	if(crt_ptr->ready[WIELD-1])
		print(fd, "Wielded:   %1i\n", crt_ptr->ready[WIELD-1]);
}

/**********************************************************************/
/*				ready				      */
/**********************************************************************/

/* This function allows the player pointed to by the first parameter to */
/* ready a weapon specified in the second, if it is a weapon.		*/

int ready(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	object	*obj_ptr;
	int	fd, i, cantwear=0,class;

	fd = ply_ptr->fd;

	if(cmnd->num < 2) {
		print(fd, "Wield what?\n");
		return(0);
	}

	F_CLR(ply_ptr, PHIDDN);

	if(cmnd->num > 1) {

		obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
				   cmnd->str[1], cmnd->val[1]);

		if(!obj_ptr) {
			print(fd, "You don't have that.\n");
			return(0);
		}

		if(obj_ptr->wearflag != WIELD) {
			print(fd, "You can't wield that.\n");
			return(0);
		}

		if(ply_ptr->ready[WIELD-1]) {
			print(fd, "You're already wielding something.\n");
			return(0);
		}

	if (F_ISSET(obj_ptr,OPLDGK) &&
        (BOOL(F_ISSET(obj_ptr,OKNGDM)) != BOOL(F_ISSET(ply_ptr,PKNGDM)))){
        print(fd, "You are unable to wield %i.\n",obj_ptr);
        return(0);
    }              

if(ply_ptr->class > 10)
        class = ply_ptr->class + 11;
else
        class = ply_ptr->class;

      if(F_ISSET(obj_ptr,OCLSEL))
	if(!F_ISSET(obj_ptr,OCLSEL + class) && ( ply_ptr->class < CARETAKER)){
        	print(fd, "Your class prevents you from wielding %i.\n",obj_ptr);
        	return(0);
	}

		if(F_ISSET(obj_ptr, OGOODO) && ply_ptr->alignment < -50) {
			print(fd, "%I shocks you and you drop it.\n", obj_ptr);
			broadcast_rom(fd, ply_ptr->rom_num, 
				      "%M is shocked by %i.", ply_ptr, obj_ptr);
			del_obj_crt(obj_ptr, ply_ptr);
			add_obj_rom(obj_ptr, ply_ptr->parent_rom);
			return(0);
		}

		if(F_ISSET(obj_ptr, OEVILO) && ply_ptr->alignment > 50) {
			print(fd, "%I shocks you and you drop it.\n", obj_ptr);
			broadcast_rom(fd, ply_ptr->rom_num, 
				      "%M is shocked by %i.", ply_ptr, obj_ptr);
			del_obj_crt(obj_ptr, ply_ptr);
			add_obj_rom(obj_ptr, ply_ptr->parent_rom);
			return(0);
		}

		i = (F_ISSET(obj_ptr, OSIZE1) ? 1:0) * 2 +
			(F_ISSET(obj_ptr, OSIZE2) ? 1:0);

		switch(i) {
		case 1:
			if(ply_ptr->race != GNOME &&
			   ply_ptr->race != HOBBIT &&
			   ply_ptr->race != DWARF) cantwear = 1;
			break;
		case 2:
			if(ply_ptr->race != HUMAN &&
			   ply_ptr->race != ELF &&
			   ply_ptr->race != HALFELF &&
			   ply_ptr->race != DARKELF &&
                           ply_ptr->race != GOBLIN &&
                           ply_ptr->race != HALFORC &&
			   ply_ptr->race != ORC) cantwear = 1;
			break;
		case 3:
			if(ply_ptr->race != HALFGIANT && ply_ptr->race != OGRE && ply_ptr->race != TROLL) cantwear = 1;
			break;
		}

		if(cantwear && ply_ptr->class < CARETAKER) {
			print(fd, "%I isn't the right size for you.\n",
				obj_ptr);
			return(0);
		}

		ply_ptr->ready[WIELD-1] = obj_ptr;
		compute_thaco(ply_ptr);
		del_obj_crt(obj_ptr, ply_ptr);

		print(fd, "You wield %1i.\n", obj_ptr);
		broadcast_rom(fd, ply_ptr->rom_num, "%M wields %1i.",
			      ply_ptr, obj_ptr);

		if(obj_ptr->use_output[0])
			print(fd, "%s\n", obj_ptr->use_output);
		if(F_ISSET(obj_ptr, OCURSE))
                	F_SET(obj_ptr, OCURSW);

		F_SET(obj_ptr, OWEARS);
	}

	return(0);

}

/**********************************************************************/
/*				hold				      */
/**********************************************************************/

/* This function allows a player to hold an item if it is designated */
/* as a hold-able item.						     */

int hold(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	object	*obj_ptr;
	int	fd, found=0, match=0,class;

	fd = ply_ptr->fd;

	if(cmnd->num < 2) {
		print(fd, "Hold what?\n");
		return(0);
	}

	F_CLR(ply_ptr, PHIDDN);

	if(cmnd->num > 1) {

		obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
				   cmnd->str[1], cmnd->val[1]);

		if(!obj_ptr) {
			print(fd, "You don't have that.\n");
			return(0);
		}

		if(obj_ptr->wearflag != HELD) {
			print(fd, "You can't hold that.\n");
			return(0);
		}

		if(ply_ptr->ready[HELD-1]) {
			print(fd, "You're already holding something.\n");
			return(0);
		}

	if (F_ISSET(obj_ptr,OPLDGK) &&
        (BOOL(F_ISSET(obj_ptr,OKNGDM)) != BOOL(F_ISSET(ply_ptr,PKNGDM)))){
        print(fd, "You are unable to hold %i.\n",obj_ptr);
        return(0);
    }              

if(ply_ptr->class > 10)
        class = ply_ptr->class + 11;
else
        class = ply_ptr->class;

      if(F_ISSET(obj_ptr,OCLSEL))
	if(!F_ISSET(obj_ptr,OCLSEL + class) && ( ply_ptr->class < CARETAKER)){
        	print(fd, "Your class prevents you from holding %i.\n",obj_ptr);
        	return(0);
	}

		if(F_ISSET(obj_ptr, OGOODO) && ply_ptr->alignment < -50) {
			print(fd, "%I shocks you and you drop it.\n", obj_ptr);
			broadcast_rom(fd, ply_ptr->rom_num, 
				      "%M is shocked by %i.", ply_ptr, obj_ptr);
			del_obj_crt(obj_ptr, ply_ptr);
			add_obj_rom(obj_ptr, ply_ptr->parent_rom);
			return(0);
		}

		if(F_ISSET(obj_ptr, OEVILO) && ply_ptr->alignment > 50) {
			print(fd, "%I shocks you and you drop it.\n", obj_ptr);
			broadcast_rom(fd, ply_ptr->rom_num, 
				      "%M is shocked by %i.", ply_ptr, obj_ptr);
			del_obj_crt(obj_ptr, ply_ptr);
			add_obj_rom(obj_ptr, ply_ptr->parent_rom);
			return(0);
		}

		ply_ptr->ready[HELD-1] = obj_ptr;
		del_obj_crt(obj_ptr, ply_ptr);

		print(fd, "You hold %1i.\n", obj_ptr);
		broadcast_rom(fd, ply_ptr->rom_num, "%M holds %1i.",
			      ply_ptr, obj_ptr);
		if(obj_ptr->use_output[0] && (obj_ptr->type != POTION || obj_ptr->type != WAND))
			print(fd, "%s\n", obj_ptr->use_output);
		if(F_ISSET(obj_ptr, OCURSE))
                	F_SET(obj_ptr, OCURSW);

		F_SET(obj_ptr, OWEARS);
	}

	return(0);

}
