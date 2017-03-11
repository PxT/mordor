/*
 *      MAGIC9.C:
 *
 *      Additional spell-casting routines.
 *      (C) 1996 Brooke Paul
 */

#include "mstruct.h"
#include "mextern.h"
#include <stdlib.h>
#ifdef DMALLOC
  #include "/usr/local/include/dmalloc.h"
#endif

/***************************************************************************
*	Recharge wand/Transmute
*
*  This allows an Alchemist to recharge a wand provided s/he knows the
*  spell of the wand and has enough gold.
*
*/
int recharge_wand(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	ctag	*cp;
	otag	*op; 
	object  *obj_ptr;
	int	fd, how=CAST, cost;

	fd=ply_ptr->fd;
	if(ply_ptr->class < CARETAKER && ply_ptr->class != ALCHEMIST) {
		print(fd, "Only an alchemist can do that.\n");
		return(0);
	}

	if(ply_ptr->level < 6 && ply_ptr->class < CARETAKER) {
		print(fd, "You are not experienced enough to do that yet.\n");
		return(0);
	}
	if(!dec_daily(&ply_ptr->daily[DL_RCHRG]) && ply_ptr->class < CARETAKER) {
		print(fd, "You have recharged enough wands for one day.\n");
		return(0);
	}
	if(ply_ptr->ready[WIELD-1]) { 
		print(fd, "Your hands are too full to do that.\n");
		return(0);
	}
	if(F_ISSET(ply_ptr, PBLIND)) {
        print(fd, "How do you do that?  You're blind.\n");
		return(0);
	}
	if(!ply_ptr->ready[HELD-1] || ply_ptr->ready[HELD-1]->type != WAND) {
		print(fd, "You must hold the wand you wish to recharge.\n");
		return(0);
	}
	obj_ptr=ply_ptr->ready[HELD-1];

	if(obj_ptr->shotscur) {
		print(fd, "That %i still has magic in it.\n", obj_ptr);
		return(0);
	}
	if(!S_ISSET(ply_ptr, obj_ptr->magicpower-1)){
		print(fd, "You dont know the spell of this wand.\n");
		return(0);
	}
	cost=1000*obj_ptr->shotsmax;
	if(F_ISSET(obj_ptr, ONOFIX))
		cost *=2;
	if(ply_ptr->gold-cost < 0 || cost <1) {
		print(fd, "You dont have enough gold.\n");
		return(0);
	}
	ply_ptr->gold -= cost;

        if(spell_fail(ply_ptr, how)) {
		print(fd, "The wand glows bright red and explodes!\n");
		broadcast_rom(fd, ply_ptr->rom_num, "A wand explodes in %s's hand!\n", ply_ptr->name);
		ply_ptr->hpcur -= mrand(10,20);
		if(ply_ptr->hpcur < 1) {
			ply_ptr->hpcur = 0;
			die(ply_ptr, ply_ptr);
		}
		ply_ptr->ready[HELD-1]=0;
		free_obj(obj_ptr);
		return(0);

	}

	/* success! */
	obj_ptr->shotscur=obj_ptr->shotsmax;
	print(fd, "You successfully recharge the %s.\n", obj_ptr->name);
	return(0);
}

/**************************************************************************
*	This function enables alchemists to randomly enchant an item
*  for a short period of time.
*
*/
int tmp_enchant(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	int fd, i, j;
	object *obj_ptr;
	char	*temp, head, tail;
	long 	t, x;
 
	fd=ply_ptr->fd;

	if(ply_ptr->class != ALCHEMIST && ply_ptr->class < CARETAKER) {
		print(fd, "Only alchemists can do that.\n");
		return(0);
	}
	if(ply_ptr->level < 10 && ply_ptr->class < CARETAKER) {
		print(fd, "You are not experienced enough to do that.\n");
		return(0);
	}
	if(F_ISSET(ply_ptr, PBLIND)) {
                print(fd, "You can't do that.  You're blind!\n");
                return(0);
        }
	obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj, cmnd->str[1], cmnd->val[1]);	
	
	if(!obj_ptr) {
		print(fd, "That's not in your inventory.\n");
		return(0);
	}
	/* We have to use the magicpower and magicrealm fields to store 
	the duration and lasttime information.  This may be fixed by a 
	object struct change in the future. */
	if(obj_ptr->adjustment || obj_ptr->pdice ||obj_ptr->magicpower || obj_ptr->magicrealm || obj_ptr->type >5) { 
		print(fd, "You cannot enchant that.\n");
		return(0);
	}
	if(F_ISSET(obj_ptr, OENCHA) || F_ISSET(obj_ptr,ORENCH)) {
		print(fd, "The %s is already enchanted.\n", obj_ptr);
		return(0);
	}
	if(!dec_daily(&ply_ptr->daily[DL_ENCHA]) && ply_ptr->class < CARETAKER){
		print(fd, "You have enchanted enough today.\n");
		return(0);
	}

	obj_ptr->magicpower=(char)mrand(1, (ply_ptr->intelligence*4));
	obj_ptr->magicrealm=(char)mrand(1, (ply_ptr->constitution*4));
	rand_enchant(obj_ptr);
	print(fd, "%I begins to glow brightly.\n", obj_ptr);
    broadcast_rom(fd, ply_ptr->rom_num, "%M enchants %1i.", ply_ptr,obj_ptr);
	F_SET(obj_ptr, OTMPEN);
	return(0);

}
