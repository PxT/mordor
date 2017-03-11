/*
 * MAGIC5.C:
 *
 *	Additional spell-casting routines.
 *
 *	Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"
#ifdef DMALLOC
  #include "/usr/local/include/dmalloc.h"
#endif
/**********************************************************************/
/*				recall				      */
/**********************************************************************/

/* This function allows a player to teleport himself or another player */
/* to room #1 (Town Square)					       */

int recall(ply_ptr, cmnd, how)
creature	*ply_ptr;
cmd		*cmnd;
int		how;
{
	creature	*crt_ptr;
	room		*rom_ptr, *new_rom;
	int		fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(ply_ptr->mpcur < 25 && how == CAST) {
		print(fd, "Not enough magic points.\n");
		return(0);
	}

	if(ply_ptr->class != CLERIC && 
	   ply_ptr->class < CARETAKER && how == CAST) {
		print(fd, "Only clerics may cast that spell.\n");
		return(0);
	}

	if(!S_ISSET(ply_ptr, SRECAL) && how == CAST) {
		print(fd, "You don't know that spell.\n");
		return(0);
	}

	/* Cast recall on self */
	if(cmnd->num == 2) {

		if(how == CAST)
			ply_ptr->mpcur -= 25;

		if(how == CAST || how == SCROLL || how == WAND) {
			print(fd, "Word of recall spell cast.\n");
			broadcast_rom(fd, ply_ptr->rom_num, 
				      "%M casts word of recall on %sself.", 
				      ply_ptr,
				      F_ISSET(ply_ptr, PMALES) ? "him":"her");
		}
		else if(how == POTION) {
			print(fd, "You phase in and out of existence.\n");
			broadcast_rom(fd, ply_ptr->rom_num,
				"%M disappears.", ply_ptr);
		}

		if(load_rom(1, &new_rom) < 0) {
			print(fd, "Spell failure.\n");
			return(0);
		}

		del_ply_rom(ply_ptr, rom_ptr);
		add_ply_rom(ply_ptr, new_rom);

		return(1);
	}

	/* Cast word of recall on another player */
	else {

		if(how == POTION) {
			print(fd, "You can only use a potion on yourself.\n");
			return(0);
		}

		cmnd->str[2][0] = up(cmnd->str[2][0]);
		crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
				   cmnd->str[2], cmnd->val[2]);

		if(!crt_ptr) {
			print(fd, "That player is not here.\n");
			return(0);
		}

		if(how == CAST)
			ply_ptr->mpcur -= 25;

		if(how == CAST || how == SCROLL || how == WAND) {
			print(fd, "Word of recall cast on %m.\n", crt_ptr);
			print(crt_ptr->fd, 
			      "%M casts a word of recall spell on you.\n",
			      ply_ptr);
			broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
				       "%M casts word of recall on %m.",
				       ply_ptr, crt_ptr);

			if(load_rom(1, &new_rom) < 0) {
				print(fd, "Spell failure.\n");
				return(0);
			}

			del_ply_rom(crt_ptr, rom_ptr);
			add_ply_rom(crt_ptr, new_rom);

			return(1);
		}
	}

	return(1);

}

/**********************************************************************/
/*				summon				      */
/**********************************************************************/

/* This function allows players to cast summon spells on anyone who is */
/* in the game, taking that person to your current room.	       */

int summon(ply_ptr, cmnd, how)
creature	*ply_ptr;
cmd		*cmnd;
int		how;
{
	creature	*crt_ptr;
	room		*rom_ptr, *new_rom;
	int		fd, n;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(ply_ptr->mpcur < 30 && how == CAST) {
		print(fd, "Not enough magic points.\n");
		return(0);
	}

	if(!S_ISSET(ply_ptr, SSUMMO) && how == CAST) {
		print(fd, "You don't know that spell.\n");
		return(0);
	}

	if(cmnd->num == 2) {
		print(fd, "You may not use that on yourself.\n");
		return(0);
	}

	else {

		if(how == POTION) {
			print(fd, "You can only use a potion on yourself.\n");
			return(0);
		}

		cmnd->str[2][0] = up(cmnd->str[2][0]);
		crt_ptr = find_who(cmnd->str[2]);

		if(!crt_ptr || crt_ptr == ply_ptr || F_ISSET(crt_ptr, PDMINV)) {
			print(fd, 
			      "That player is not playing (Use full names).\n");
			return(0);
		}

		if(how == CAST)
			ply_ptr->mpcur -= 30;

		n = count_vis_ply(rom_ptr);
		if(F_ISSET(rom_ptr, RNOTEL) ||
		  (F_ISSET(rom_ptr, RONEPL) && n > 0) ||
		  (F_ISSET(rom_ptr, RTWOPL) && n > 1) ||
		  (F_ISSET(rom_ptr, RTHREE) && n > 2)) {
			print(fd, "The spell fizzles.\n");
			return(0);
		}

		if(rom_ptr->lolevel > crt_ptr->level ||
		   (crt_ptr->level > rom_ptr->hilevel && rom_ptr->hilevel) ||
		   F_ISSET(crt_ptr, PNOSUM)) {
			print(fd, "The spell fizzles.\n");
			return(0);
		}

		if(F_ISSET(crt_ptr->parent_rom,RNOLEA))
		{
			print(fd, "Your magic can not locate %s.\n",crt_ptr->name);
			return(0);
		}

		if(how == CAST || how == SCROLL || how == WAND) {
			print(fd, "You summon %m.\n", crt_ptr);
			print(crt_ptr->fd, 
			      "%M summons you.\n",
			      ply_ptr);
			broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
				       "%M summons %m.",
				       ply_ptr, crt_ptr);

			del_ply_rom(crt_ptr, crt_ptr->parent_rom);
			add_ply_rom(crt_ptr, rom_ptr);

			return(1);
		}
	}

	return(1);

}

/**********************************************************************/
/*				heal				      */
/**********************************************************************/

/* This function will cause the heal spell to be cast on a player or    */
/* another monster.  It heals all hit points damage but only works 3 	*/
/* times a day.								*/

int heal(ply_ptr, cmnd, how)
creature	*ply_ptr;
cmd		*cmnd;
int		how;
{
	creature	*crt_ptr;
	room		*rom_ptr;
	int		fd, heal;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(ply_ptr->mpcur < 20 && how == CAST) {
		print(fd, "Not enough magic points.\n");
		return(0);
	}

	if(ply_ptr->class != CLERIC && ply_ptr->class != PALADIN &&
	   ply_ptr->class < CARETAKER && how == CAST) {
		print(fd, "Only paladins and clerics may cast that spell.\n");
		return(0);
	}

	if(!S_ISSET(ply_ptr, SFHEAL) && how == CAST) {
		print(fd, "You don't know that spell.\n");
		return(0);
	}

	/* Heal self */
	if(cmnd->num == 2 ) {

		if(!dec_daily(&ply_ptr->daily[DL_FHEAL]) && how == CAST &&
		   ply_ptr->class < CARETAKER) {
			print(fd, "You have healed enough today.\n");
			return(0);
		}

		ply_ptr->hpcur = ply_ptr->hpmax;

		if(how == CAST) 
			ply_ptr->mpcur -= 20;

		if(how == CAST || how == SCROLL) {
			print(fd, "Heal spell cast.\n");
			broadcast_rom(fd, ply_ptr->rom_num, 
				      "%M casts a heal spell on %sself.", 
				      ply_ptr,
				      F_ISSET(ply_ptr, PMALES) ? "him":"her");
			return(1);
		}
		else {
			print(fd, "You feel incredibly better.\n");
			return(1);
		}
	}

	/* Cast heal on another player or monster */
	else {

		if(how == POTION) {
			print(fd, "You can only use a potion on yourself.\n");
			return(0);
		}

		cmnd->str[2][0] = up(cmnd->str[2][0]);
		crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
				   cmnd->str[2], cmnd->val[2]);

		if(!crt_ptr) {
			cmnd->str[2][0] = low(cmnd->str[2][0]);
			crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
					   cmnd->str[2], cmnd->val[2]);

			if(!crt_ptr) {
				print(fd, "That person is not here.\n");
				return(0);
			}
		}

		if(!dec_daily(&ply_ptr->daily[DL_FHEAL]) && how == CAST &&
		   ply_ptr->class < CARETAKER && ply_ptr->type != MONSTER) {
			print(fd, "You have healed enough today.\n");
			return(0);
		}

		crt_ptr->hpcur = crt_ptr->hpmax;

		if(how == CAST) 
			ply_ptr->mpcur -= 20;
		if(how == CAST || how == SCROLL || how == WAND) {
			print(fd, "Heal spell cast on %m.\n", crt_ptr);
			print(crt_ptr->fd, "%M casts a heal spell on you.\n",
			      ply_ptr);
			broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
				       "%M casts a heal spell on %m.",
				       ply_ptr, crt_ptr);
			return(1);
		}
	}

	return(1);

}

/**********************************************************************/
/*				track				      */
/**********************************************************************/

/* This function allows rangers to cast the track spell which takes them */
/* to any other player in the game.					 */

int magictrack(ply_ptr, cmnd, how)
creature	*ply_ptr;
cmd		*cmnd;
int		how;
{
	creature	*crt_ptr;
	room		*rom_ptr, *new_rom;
	int		fd, n;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(ply_ptr->class != RANGER && ply_ptr->class < CARETAKER &&
	   how==CAST) {
		print(fd, "Only rangers may cast that spell.\n");
		return(0);
	}

	if(ply_ptr->mpcur < 13 && how == CAST) {
		print(fd, "Not enough magic points.\n");
		return(0);
	}

	if(!S_ISSET(ply_ptr, STRACK) && how == CAST) {
		print(fd, "You don't know that spell.\n");
		return(0);
	}

	if(cmnd->num == 2) {
		print(fd, "You may not use that on yourself.\n");
		return(0);
	}

	else {

		if(how == POTION) {
			print(fd, "You can only use a potion on yourself.\n");
			return(0);
		}

		cmnd->str[2][0] = up(cmnd->str[2][0]);
		crt_ptr = find_who(cmnd->str[2]);

		if(!crt_ptr || crt_ptr == ply_ptr || F_ISSET(crt_ptr, PDMINV)) {
			print(fd, 
			      "That player is not playing (Use full names).\n");
			return(0);
		}

		if(crt_ptr->class >= CARETAKER) {
			print(fd, "You may not track that player.\n");
			return(0);
		}

		n = count_vis_ply(crt_ptr->parent_rom);
		if(F_ISSET(crt_ptr->parent_rom, RNOTEL) ||
		  (F_ISSET(crt_ptr->parent_rom, RONEPL) && n > 0) ||
		  (F_ISSET(crt_ptr->parent_rom, RTWOPL) && n > 1) ||
		  (F_ISSET(crt_ptr->parent_rom, RTHREE) && n > 2)) {
			print(fd, "The spell fizzles.\n");
			return(0);
		}

		if(crt_ptr->parent_rom->lolevel > ply_ptr->level ||
		   (ply_ptr->level > crt_ptr->parent_rom->hilevel &&
		   crt_ptr->parent_rom->hilevel)) {
			print(fd, "The spell fizzles.\n");
			return(0);
		}

		if(!dec_daily(&ply_ptr->daily[DL_TRACK]) && how == CAST &&
		   ply_ptr->class < CARETAKER) {
			print(fd, "You have tracked enough today.\n");
			return(0);
		}

		if(how == CAST)
			ply_ptr->mpcur -= 13;

		if(how == CAST || how == SCROLL || how == WAND) {
			print(fd, "You track %m.\n", crt_ptr);
			print(crt_ptr->fd, 
			      "%M has tracked you.\n", ply_ptr);
			broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
				       "%M tracks %m.", ply_ptr, crt_ptr);

			del_ply_rom(ply_ptr, rom_ptr);
			add_ply_rom(ply_ptr, crt_ptr->parent_rom);

			return(1);
		}
	}

	return(1);

}

/**********************************************************************/
/*				levitate			      */
/**********************************************************************/

/* This function allows players to cast the levitate spell, thus allowing */
/* them to levitate over traps or up mountain cliffs.			  */

int levitate(ply_ptr, cmnd, how)
creature	*ply_ptr;
cmd		*cmnd;
int		how;
{
	creature	*crt_ptr;
	room		*rom_ptr, *new_rom;
	int		fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(ply_ptr->mpcur < 10 && how == CAST) {
		print(fd, "Not enough magic points.\n");
		return(0);
	}

	if(!S_ISSET(ply_ptr, SLEVIT) && how == CAST) {
		print(fd, "You don't know that spell.\n");
		return(0);
	}
	if(spell_fail(ply_ptr, how)) {
                if(how == CAST)
                     ply_ptr->mpcur -= 10;
                return(0);
        }

	if(cmnd->num == 2) {
		ply_ptr->lasttime[LT_LEVIT].ltime = time(0);
		F_SET(ply_ptr, PLEVIT);
		broadcast_rom(fd, ply_ptr->rom_num, 
			"%M levitates off the floor.", ply_ptr);
		if(how == CAST) {
			print(fd, "You cast a levitation spell.\n");
			ply_ptr->mpcur -= 10;
			ply_ptr->lasttime[LT_LEVIT].interval = 2400L +
				bonus[ply_ptr->intelligence]*600L;
if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,"The room's magical properties increase the power of your spell.\n");
            ply_ptr->lasttime[LT_LEVIT].interval += 800L;
        }                                
		}
		else {
			print(fd, "You begin to float.\n");
			ply_ptr->lasttime[LT_LEVIT].interval = 1200L;
		}
		return(1);
	}
	else {

		if(how == POTION) {
			print(fd, "You can only use a potion on yourself.\n");
			return(0);
		}

		cmnd->str[2][0] = up(cmnd->str[2][0]);
		crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
				   cmnd->str[2], cmnd->val[2]);
		if(!crt_ptr) {
			print(fd, "I don't see that player here.\n");
			return(0);
		}

		F_SET(crt_ptr, PLEVIT);
		crt_ptr->lasttime[LT_LEVIT].ltime = time(0);
		broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
			"%M casts a levitate spell on %m.", ply_ptr, crt_ptr);
		print(crt_ptr->fd, "%M casts levitate on you.\n", ply_ptr);

		if(how == CAST) {
			print(fd, "You cast a levitate spell on %s.\n",crt_ptr);
			ply_ptr->mpcur -= 10;
			crt_ptr->lasttime[LT_LEVIT].interval = 2400L +
				bonus[ply_ptr->intelligence]*600L;
		if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            	  print(fd,"The room's magical properties increase the power of your spell.\n");
            	  crt_ptr->lasttime[LT_LEVIT].interval += 800L;
        	}                                
		}

		else {
			print(fd, "%M begins to float.\n", crt_ptr);
			crt_ptr->lasttime[LT_LEVIT].interval = 1200L;
		}

		return(1);
	}

}

/************************************************************************/
/*				resist_fire				*/
/************************************************************************/

/* This function allows players to cast the resist fire spell.  It will	*/
/* allow the player to resist fire breathed on them by dragons and 	*/
/* other breathers.							*/

int resist_fire(ply_ptr, cmnd, how)
creature	*ply_ptr;
cmd		*cmnd;
int		how;
{
	creature	*crt_ptr;
	room		*rom_ptr, *new_rom;
	int		fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(ply_ptr->mpcur < 12 && how == CAST) {
		print(fd, "Not enough magic points.\n");
		return(0);
	}

	if(!S_ISSET(ply_ptr, SRFIRE) && how == CAST) {
		print(fd, "You don't know that spell.\n");
		return(0);
	}
	if(spell_fail(ply_ptr, how)) {
                if(how == CAST)
                     ply_ptr->mpcur -= 12;
                return(0);
        }

	if(cmnd->num == 2) {
		ply_ptr->lasttime[LT_RFIRE].ltime = time(0);
		F_SET(ply_ptr, PRFIRE);
		broadcast_rom(fd, ply_ptr->rom_num, 
			"%M resists fire.", ply_ptr);
		if(how == CAST) {
			print(fd, "You cast a resist-fire spell.\nYour skin toughens.\n");
			ply_ptr->mpcur -= 12;
			ply_ptr->lasttime[LT_RFIRE].interval = MAX(300, 1200 +
				bonus[ply_ptr->intelligence]*600);
		if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            	  print(fd,"The room's magical properties increase the power of your spell.\n");
            	  ply_ptr->lasttime[LT_RFIRE].interval += 800L;
        	}                                
		}
		else {
			print(fd, "You feel your skin toughen.\n");
			ply_ptr->lasttime[LT_RFIRE].interval = 1200L;
		}
		return(1);
	}
	else {

		if(how == POTION) {
			print(fd, "You can only use a potion on yourself.\n");
			return(0);
		}

		cmnd->str[2][0] = up(cmnd->str[2][0]);
		crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
				   cmnd->str[2], cmnd->val[2]);
		if(!crt_ptr) {
			print(fd, "I don't see that player here.\n");
			return(0);
		}

		F_SET(crt_ptr, PRFIRE);
		crt_ptr->lasttime[LT_RFIRE].ltime = time(0);
		broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
			"%M casts a resist-fire spell on %m.", 
			ply_ptr, crt_ptr);
		print(crt_ptr->fd, "%M casts resist-fire on you.\n", ply_ptr);

		if(how == CAST) {
			print(fd, "You cast a resist-fire spell on %s.\n",
				crt_ptr);
			ply_ptr->mpcur -= 12;
			crt_ptr->lasttime[LT_RFIRE].interval = MAX(300, 1200 +
				bonus[ply_ptr->intelligence]*600);
		if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            	  print(fd,"The room's magical properties increase the power of your spell.\n");
            	  crt_ptr->lasttime[LT_RFIRE].interval += 800L;
        	}                                
		}

		else {
			print(fd, "%M resists fire.\n", crt_ptr);
			crt_ptr->lasttime[LT_RFIRE].interval = 1200L;
		}

		return(1);
	}

}


/************************************************************************/
/*				fly					*/
/************************************************************************/

/* This function allows players to cast the fly spell.  It will		*/
/* allow the player to fly to areas that are otherwise unreachable	*/
/* by foot.								*/

int fly(ply_ptr, cmnd, how)
creature	*ply_ptr;
cmd		*cmnd;
int		how;
{
	creature	*crt_ptr;
	room		*rom_ptr;
	int		fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(ply_ptr->mpcur < 15 && how == CAST) {
		print(fd, "Not enough magic points.\n");
		return(0);
	}

	if(!S_ISSET(ply_ptr, SFLYSP) && how == CAST) {
		print(fd, "You don't know that spell.\n");
		return(0);
	}
	if(spell_fail(ply_ptr, how)) {
                if(how == CAST)
                     ply_ptr->mpcur -= 15;
                return(0);
        }

	if(cmnd->num == 2) {
		ply_ptr->lasttime[LT_FLYSP].ltime = time(0);
		F_SET(ply_ptr, PFLYSP);
		broadcast_rom(fd, ply_ptr->rom_num, 
			"%M begins to fly.", ply_ptr);
		if(how == CAST) {
			print(fd, "You can fly!\n");
			ply_ptr->mpcur -= 15;
			ply_ptr->lasttime[LT_FLYSP].interval = MAX(300, 1200 +
				bonus[ply_ptr->intelligence]*600);
		if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            	  print(fd,"The room's magical properties increase the power of your spell.\n");
            	  ply_ptr->lasttime[LT_FLYSP].interval += 600L;
        	}                                
		}
		else {
			print(fd, "You can fly!\n");
			ply_ptr->lasttime[LT_FLYSP].interval = 1200L;
		}
		return(1);
	}
	else {

		if(how == POTION) {
			print(fd, "You can only use a potion on yourself.\n");
			return(0);
		}

		cmnd->str[2][0] = up(cmnd->str[2][0]);
		crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
				   cmnd->str[2], cmnd->val[2]);
		if(!crt_ptr) {
			print(fd, "I don't see that player here.\n");
			return(0);
		}

		F_SET(crt_ptr, PFLYSP);
		crt_ptr->lasttime[LT_FLYSP].ltime = time(0);
		broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
			"%M casts a fly spell on %m.", 
			ply_ptr, crt_ptr);
		print(crt_ptr->fd, "%M casts fly on you.\n", ply_ptr);

		if(how == CAST) {
			print(fd, "You cast a fly spell on %s.\n",
				crt_ptr);
			ply_ptr->mpcur -= 15;
			crt_ptr->lasttime[LT_FLYSP].interval = MAX(300, 1200 +
				bonus[ply_ptr->intelligence]*600);
		if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            	  print(fd,"The room's magical properties increase the power of your spell.\n");
            	  crt_ptr->lasttime[LT_FLYSP].interval += 600L;
        	}                                
		}

		else {
			print(fd, "%M can fly.\n", crt_ptr);
			crt_ptr->lasttime[LT_FLYSP].interval = 1200L;
		}

		return(1);
	}

}

