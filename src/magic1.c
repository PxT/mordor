/*
 * MAGIC1.C:
 *
 *  User routines dealing with magic spells.  Potions, wands,
 *  scrolls, and casting are all covered.
 *
 *  Copyright (C) 1991, 1992, 1993 Brooke Paul
 *
 * $Id: magic1.c,v 6.13 2001/03/08 16:09:09 develop Exp $
 *
 * $Log: magic1.c,v $
 * Revision 6.13  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

#include "../include/mordb.h"
#include "mextern.h"


/**********************************************************************/
/*              cast                      */
/**********************************************************************/

/* This function allows a player to cast a magical spell.  It looks at */
/* the second parsed word to find out if the spell-name is valid, and  */
/* then calls the appropriate spell function.                  */

int cast(creature *ply_ptr, cmd *cmnd )
{
    time_t    i, t;
    int (*fn)();
    int fd, splno, c = 0, match = 0, n;

    fd = ply_ptr->fd;

    if(cmnd->num < 2) {
        output(fd, "Cast what?\n");
        return(0);
    }
    if(F_ISSET(ply_ptr, PBLIND)) {
		output_wc(fd, "You can't see to direct the incantation!\n", BLINDCOLOR);
		return(0);
    }
    if(F_ISSET(ply_ptr, PSILNC)) {
		output_wc(fd, "You can't recite the incantation!\n", SILENCECOLOR);
		return(0);
    }
    do {
        if(!strcmp(cmnd->str[1], get_spell_name(c))) {
            match = 1;
            splno = c;
            break;
        }
        else if(!strncmp(cmnd->str[1], get_spell_name(c), 
            strlen(cmnd->str[1]))) {
            match++;
            splno = c;
        }
        c++;
    } while(get_spell_num(c) != -1);

    if(match == 0) {
        output(fd, "That spell does not exist.\n");
        return(0);
    }

    else if(match > 1) {
        output(fd, "Spell name is not unique.\n");
        return(0);
    }

    if(F_ISSET(ply_ptr->parent_rom, RNOMAG) && ply_ptr->class < BUILDER) {
        output(fd, "Nothing happens.\n");
        return(0);
    }

	t = time(0);

	if ( ply_ptr->class < DM )
	{
		i = LT(ply_ptr, LT_SPELL);
		i = MAX(i, LT(ply_ptr, LT_READS) ); 

		if(t < i) {
			please_wait(fd, i-t);
			return(0);
		}
	}
    
    F_CLR(ply_ptr, PHIDDN);

    fn = get_spell_function(splno);

    if(fn == offensive_spell || fn == room_damage) {
        for(c=0; ospell[c].splno != get_spell_num(splno); c++)
            if(ospell[c].splno == -1) 
				return(0);
        n = (*fn)(ply_ptr, cmnd, CAST, get_spell_name(splno),
            &ospell[c]);
    }

    else
        n = (*fn)(ply_ptr, cmnd, CAST);

    if(n) {
	ply_ptr->lasttime[LT_READS].ltime = t;
        ply_ptr->lasttime[LT_SPELL].ltime = t;
        if(ply_ptr->class == ALCHEMIST || ply_ptr->class == MAGE)
            ply_ptr->lasttime[LT_SPELL].interval = 3;
        else if(ply_ptr->class == BARD || ply_ptr->class == CLERIC || ply_ptr->class == DRUID)
	    ply_ptr->lasttime[LT_SPELL].interval = 4;
	else
            ply_ptr->lasttime[LT_SPELL].interval = 5;
    }

    return(0);

}

/**********************************************************************/
/*              teach                     */
/**********************************************************************/

/* This function allows mages to teach other players the first six    */
/* spells.                                */

int teach(creature *ply_ptr, cmd *cmnd )
{
    room        *rom_ptr;
    creature    *crt_ptr;
    int     fd, splno, c = 0, match = 0;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(cmnd->num < 3) {
        output(fd, "Teach whom what?\n");
        return(0);
    }
    if(F_ISSET(ply_ptr, PBLIND)) {
        output_wc(fd, "You can't see to do that!\n", BLINDCOLOR);
        return(0);
    }
    if(F_ISSET(ply_ptr, PSILNC)) {
		output_wc(fd, "You can't speak to do that!\n", SILENCECOLOR);
		return(0);
    }

    if((ply_ptr->class != MAGE && ply_ptr->class != CLERIC) && ply_ptr->class < BUILDER)  {
        output(fd, "Only mages and clerics may teach spells.\n");
        return(0);
    }

    cmnd->str[1][0] = up(cmnd->str[1][0]);
    crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply, cmnd->str[1], 
               cmnd->val[1]);

    if(!crt_ptr) {
        output(fd, "I don't see that person here.\n");
        return(0);
    }

    do {
        if(!strcmp(cmnd->str[2], get_spell_name(c))) {
            match = 1;
            splno = c;
            break;
        }
        else if(!strncmp(cmnd->str[2], get_spell_name(c), 
            strlen(cmnd->str[2]))) {
            match++;
            splno = c;
        }
        c++;
    } while(get_spell_num(c) != -1);

    if(match == 0) {
        output(fd, "That spell does not exist.\n");
        return(0);
    }

    else if(match > 1) {
        output(fd, "Spell name is not unique.\n");
        return(0);
    }

    if(!S_ISSET(ply_ptr, get_spell_num(splno))) {
        output(fd, "You don't know that spell.\n");
        return(0);
    }

    if((get_spell_num(splno) > 3) && (ply_ptr->class != CLERIC && ply_ptr->class < BUILDER)) {
        output(fd, "You may not teach that spell to anyone.\n");
        return(0);
    }

    if((get_spell_num(splno) > 5) && (ply_ptr->class != DM)) {
        output(fd, "You may not teach that spell to anyone.\n");
        return(0);
    }

    if((get_spell_num(splno) < 3) && (ply_ptr->class != MAGE && ply_ptr->class < BUILDER)) {
        output(fd, "You may not teach that spell to anyone.\n");
        return(0);
    }
   

    F_CLR(ply_ptr, PHIDDN);

    S_SET(crt_ptr, get_spell_num(splno));

    sprintf(g_buffer, "%%M teaches you the %s spell.\n", 
          get_spell_name(splno));
    mprint(crt_ptr->fd, g_buffer, m1arg(ply_ptr));

    sprintf(g_buffer, "Spell \"%s\" taught to %%m.\n", get_spell_name(splno));
    mprint(fd, g_buffer, m1arg(crt_ptr));

    sprintf(g_buffer, "%%M taught %%m the %s spell.", 
		get_spell_name(splno));

    broadcast_rom2(fd, crt_ptr->fd, rom_ptr->rom_num,
               g_buffer, m2args(ply_ptr, crt_ptr));

    return(0);

}

/**********************************************************************/
/*              study                     */
/**********************************************************************/

/* This function allows a player to study a scroll, and learn the spell */
/* that is on it.                           */

int study(creature *ply_ptr, cmd *cmnd )
{
    object  *obj_ptr;
    int fd, n, match=0, class;

    fd = ply_ptr->fd;

    if(cmnd->num < 2) {
        output(fd, "Study what?\n");
        return(0);
    }
    if(F_ISSET(ply_ptr, PBLIND)) {
        output_wc(fd, "You can't see to do that!\n", BLINDCOLOR);
        return(0);
    }

    obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
               cmnd->str[1], cmnd->val[1]);

    if(!obj_ptr || !cmnd->val[1]) {
        for(n=0; n<MAXWEAR; n++) {
            if(!ply_ptr->ready[n]) continue;
            if(EQUAL(ply_ptr->ready[n], cmnd->str[1]))
                match++;
            else continue;
            if(match == cmnd->val[1] || !cmnd->val[1]) {
                obj_ptr = ply_ptr->ready[n];
                break;
            }
        }
    }

    if(!obj_ptr) {
        output(fd, "You don't have that.\n");
        return(0);
    }

    if(obj_ptr->type != SCROLL) {
        output(fd, "That's not a scroll.\n");
        return(0);
    }

    if((F_ISSET(obj_ptr, OGOODO) && ply_ptr->alignment < -100) ||
       (F_ISSET(obj_ptr, OEVILO) && ply_ptr->alignment > 100)) {
        mprint(fd, "%I burns you and you drop it.\n", m1arg(obj_ptr));
        del_obj_crt(obj_ptr, ply_ptr);
        add_obj_rom(obj_ptr, ply_ptr->parent_rom);
        return(0);
    }

	if (F_ISSET(obj_ptr,OPLDGK) && 
		(objectcheck_guild(obj_ptr) != check_guild(ply_ptr))){
		mprint(fd, "You guild allegiance prevents you from using %i.\n",
			m1arg(obj_ptr));
        return(0);
	}

	if(ply_ptr->class>10)
		class=ply_ptr->class+11;
	else
		class=ply_ptr->class;

	if(F_ISSET(obj_ptr,OCLSEL))
        if(!F_ISSET(obj_ptr,OCLSEL + class) && ( ply_ptr->class < BUILDER)){
                mprint(fd, "Your class prevents you from using %i.\n",
					m1arg(obj_ptr));
                return(0);
        }                 

    F_CLR(ply_ptr, PHIDDN);

    sprintf(g_buffer, "You learn the %s spell.\n", 
          get_spell_name(obj_ptr->magicpower-1));
    output(fd, g_buffer);
    mprint(fd, "%I disintegrates!\n", m1arg(obj_ptr));
    broadcast_rom(fd, ply_ptr->rom_num, "%M studies %1i.",
              m2args(ply_ptr, obj_ptr));

    S_SET(ply_ptr, obj_ptr->magicpower-1);
    del_obj_crt(obj_ptr, ply_ptr);
    free_obj(obj_ptr);

    return(0);

}

/**********************************************************************/
/*              readscroll                */
/**********************************************************************/

/* This function allows a player to read a scroll and cause its magical */
/* spell to be cast.  If a third word is used in the command, then that */
/* player or monster will be the target of the spell.           */

int readscroll(creature *ply_ptr, cmd *cmnd )
{
    object  *obj_ptr;
    int (*fn)();
    time_t    i, t;
    int fd, n, match=0, c, splno, class;

    fd = ply_ptr->fd;
    fn = 0;

    if(cmnd->num < 2) {
        output(fd, "Read what?\n");
        return(0);
    }
    if(F_ISSET(ply_ptr, PBLIND)) {
        output_wc(fd, "You can't see to do that!\n", BLINDCOLOR);
        return(0);
    }
    obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
               cmnd->str[1], cmnd->val[1]);

    if(!obj_ptr || !cmnd->val[1]) {
        for(n=0; n<MAXWEAR; n++) {
            if(!ply_ptr->ready[n]) continue;
            if(EQUAL(ply_ptr->ready[n], cmnd->str[1]))
                match++;
            else continue;
            if(match == cmnd->val[1] || !cmnd->val[1]) {
                obj_ptr = ply_ptr->ready[n];
                break;
            }
        }
    }

    if(!obj_ptr) {
        output(fd, "You don't have that.\n");
        return(0);
    }

    if(obj_ptr->special) {
        n = special_obj(ply_ptr, cmnd, SP_MAPSC);
        if(n != -2) return(MAX(0, n));
    }

    if(obj_ptr->type != SCROLL) {
        output(fd, "That's not a scroll.\n");
        return(0);
    }

    if((F_ISSET(obj_ptr, OGOODO) && ply_ptr->alignment < -100) ||
       (F_ISSET(obj_ptr, OEVILO) && ply_ptr->alignment > 100)) {
        mprint(fd, "%I burns you and you drop it.\n", m1arg(obj_ptr));
        del_obj_crt(obj_ptr, ply_ptr);
        add_obj_rom(obj_ptr, ply_ptr->parent_rom);
        return(0);
    }

	if (F_ISSET(obj_ptr,OPLDGK) &&
		(objectcheck_guild(obj_ptr) != check_guild(ply_ptr))){
        mprint(fd, "You guild allegiance prevents you from reading %i.\n", m1arg(obj_ptr));
        return(0);
    }              

	if(ply_ptr->class>10)
		class=ply_ptr->class+11;
	else
		class=ply_ptr->class;

	if(F_ISSET(obj_ptr,OCLSEL))
        if(!F_ISSET(obj_ptr,OCLSEL + class) && ( ply_ptr->class < BUILDER)){
                mprint(fd, "Your class prevents you from reading %i.\n",
					m1arg(obj_ptr));
                return(0);
        }                 

    if(F_ISSET(ply_ptr->parent_rom, RNOMAG) || (obj_ptr->magicpower-1 < 0)) {
        output(fd, "Nothing happens.\n");
        return(0);
    }

    i = LT(ply_ptr, LT_READS);
	i = MAX(i, LT(ply_ptr, LT_SPELL) );
    t = time(0);

    if(i > t) {
        please_wait(fd, i-t);
        return(0);
    }

    F_CLR(ply_ptr, PHIDDN);

    ply_ptr->lasttime[LT_READS].ltime = t;
    ply_ptr->lasttime[LT_READS].interval = 3;
    ply_ptr->lasttime[LT_SPELL].ltime = t;

    if(ply_ptr->class == ALCHEMIST || ply_ptr->class == MAGE)
            ply_ptr->lasttime[LT_SPELL].interval = 3;
        else if(ply_ptr->class == BARD || ply_ptr->class == CLERIC)
            ply_ptr->lasttime[LT_SPELL].interval = 4;
        else
            ply_ptr->lasttime[LT_SPELL].interval = 5;

    if(spell_fail(ply_ptr, SCROLL)){
        mprint(fd, "%I disintegrates.\n", m1arg(obj_ptr));
        del_obj_crt(obj_ptr, ply_ptr);
        free_obj(obj_ptr);
        return(0);
    }

    splno = obj_ptr->magicpower-1;
    fn = get_spell_function(splno);

    if(fn == offensive_spell || fn == room_damage) {
        for(c=0; ospell[c].splno != get_spell_num(splno); c++)
            if(ospell[c].splno == -1) return(0);
        n = (*fn)(ply_ptr, cmnd, SCROLL, get_spell_name(splno),
            &ospell[c]);
    }

    else
        n = (*fn)(ply_ptr, cmnd, SCROLL);

    if(n) {
        if(obj_ptr->use_output[0])
		{
            output_nl(fd, obj_ptr->use_output);
		}

        mprint(fd, "%I disintegrates.\n", m1arg(obj_ptr));
        del_obj_crt(obj_ptr, ply_ptr);
        free_obj(obj_ptr);
    }

    return(0);

}

/**********************************************************************/
/*              drink                     */
/**********************************************************************/

/* This function allows players to drink potions, thereby casting any */
/* spell it was meant to contain.                     */

int drink(creature *ply_ptr, cmd *cmnd )
{
    object  *obj_ptr;
    int (*fn)();
    int fd, n, match=0, c, splno, class;

    fd = ply_ptr->fd;
    fn = 0;

    if(cmnd->num < 2) {
        output(fd, "Drink what?\n");
        return(0);
    }

    obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
               cmnd->str[1], cmnd->val[1]);

    if(!obj_ptr || !cmnd->val[1]) {
        for(n=0; n<MAXWEAR; n++) {
            if(!ply_ptr->ready[n]) continue;
            if(EQUAL(ply_ptr->ready[n], cmnd->str[1]))
                match++;
            else continue;
            if(match == cmnd->val[1] || !cmnd->val[1]) {
                obj_ptr = ply_ptr->ready[n];
                break;
            }
        }
    }

    if(!obj_ptr) {
        output(fd, "You don't have that.\n");
        return(0);
    }

    if(obj_ptr->type != POTION) {
	eat(ply_ptr, cmnd);
        return(0);
    }

    if(obj_ptr->shotscur < 1 || (obj_ptr->magicpower-1 < 0)) {
        output(fd, "It's empty.\n");
        return(0);
    }

	if(F_ISSET(ply_ptr->parent_rom, RNOPOT)){
        mprint(fd, "%I starts to evaporates before you drink it.\n",
			m1arg(obj_ptr));
        return(0);
	}
	
    if((F_ISSET(obj_ptr, OGOODO) && ply_ptr->alignment < -100) ||
       (F_ISSET(obj_ptr, OEVILO) && ply_ptr->alignment > 100)) {
        mprint(fd, "%I burns you and you drop it.\n", m1arg(obj_ptr));
        del_obj_crt(obj_ptr, ply_ptr);
        add_obj_rom(obj_ptr, ply_ptr->parent_rom);
        return(0);
    }

	if (F_ISSET(obj_ptr,OPLDGK) &&
		(objectcheck_guild(obj_ptr) != check_guild(ply_ptr))){
        mprint(fd, "You guild allegiance prevents you from drinking %i.\n",
			m1arg(obj_ptr));
        return(0);
    }              

	if(ply_ptr->class>10)
		class=ply_ptr->class+11;
	else
		class=ply_ptr->class;

	if(F_ISSET(obj_ptr,OCLSEL))
        if(!F_ISSET(obj_ptr,OCLSEL + class) && ( ply_ptr->class < BUILDER)){
                mprint(fd, "Your class prevents you from drinking %i.\n",
					m1arg(obj_ptr));
                return(0);
        }                 

    F_CLR(ply_ptr, PHIDDN);

    splno = obj_ptr->magicpower-1;
    fn = get_spell_function(splno);

    if(fn == offensive_spell || fn == room_damage) {
        for(c=0; ospell[c].splno != get_spell_num(splno); c++)
            if(ospell[c].splno == -1) return(0);
        n = (*fn)(ply_ptr, cmnd, POTION, get_spell_name(splno),
            &ospell[c]);
    }

    else
        n = (*fn)(ply_ptr, cmnd, POTION);

    if(n) {
        if(obj_ptr->use_output[0])
		{
            output_nl(fd, obj_ptr->use_output);
		}

        output(fd, "Potion drank.\n");
        broadcast_rom(fd, ply_ptr->rom_num, "%M drank %1i.", 
                  m2args(ply_ptr, obj_ptr));

        if(--obj_ptr->shotscur < 1) {
            mprint(fd, "%I disintegrates.\n", m1arg(obj_ptr));
            del_obj_crt(obj_ptr, ply_ptr);
            free_obj(obj_ptr);
        }
    }

    return(0);

}

/**********************************************************************/
/*              zap                   */
/**********************************************************************/

/* This function allows players to zap a wand or staff at another player */
/* or monster.                               */

int zap(creature *ply_ptr, cmd *cmnd )
{
    object  *obj_ptr;
    time_t    i, t;
    int fd, n, match=0, class;

    fd = ply_ptr->fd;

    if(cmnd->num < 2) {
        output(fd, "Use what?\n");
        return(0);
    }
    if(F_ISSET(ply_ptr, PBLIND)) {
        output_wc(fd, "You can't see to use that!\n", BLINDCOLOR);
        return(0);
    }

    obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
               cmnd->str[1], cmnd->val[1]);

    if(!obj_ptr || !cmnd->val[1]) {
        for(n=0; n<MAXWEAR; n++) {
            if(!ply_ptr->ready[n]) continue;
            if(EQUAL(ply_ptr->ready[n], cmnd->str[1]))
                match++;
            else continue;
            if(match == cmnd->val[1] || !cmnd->val[1]) {
                obj_ptr = ply_ptr->ready[n];
                break;
            }
        }
    }

    if(!obj_ptr) {
        output(fd, "You don't have that.\n");
        return(0);
    }

    if(obj_ptr->type != WAND) {
        output(fd, "That's not a wand or staff.\n");
        return(0);
    }

    if(obj_ptr->shotscur < 1) {
        output(fd, "It's used up.\n");
        return(0);
    }

    if((F_ISSET(obj_ptr, OGOODO) && ply_ptr->alignment < -100) ||
       (F_ISSET(obj_ptr, OEVILO) && ply_ptr->alignment > 100)) {
        mprint(fd, "%I burns you and you drop it.\n", 
			m1arg(obj_ptr));
        del_obj_crt(obj_ptr, ply_ptr);
        add_obj_rom(obj_ptr, ply_ptr->parent_rom);
        return(0);
    }

	if (F_ISSET(obj_ptr,OPLDGK) &&
		(objectcheck_guild(obj_ptr) != check_guild(ply_ptr))){
        mprint(fd, "Your guild allegiance preents you from using %i.\n",
			m1arg(obj_ptr));
        return(0);
    }              

	if(ply_ptr->class>10)
		class=ply_ptr->class+11;
	else
		class=ply_ptr->class;

	if(F_ISSET(obj_ptr,OCLSEL))
        if(!F_ISSET(obj_ptr,OCLSEL + class) && ( ply_ptr->class < BUILDER)){
                mprint(fd, "Your class prevents you from using %i.\n",
					m1arg(obj_ptr));
                return(0);
        }                 


    if(F_ISSET(ply_ptr->parent_rom, RNOMAG) || (obj_ptr->magicpower < 1)) {
        output(fd, "Nothing happens.\n");
        return(0);
    }

    i = LT(ply_ptr, LT_SPELL);
    t = time(0);

    if(i > t) {
        please_wait(fd, i-t);
        return(0);
    }

    F_CLR(ply_ptr, PHIDDN);

    ply_ptr->lasttime[LT_SPELL].ltime = t;
    ply_ptr->lasttime[LT_SPELL].interval = 3;

    if(spell_fail(ply_ptr, WAND)){
        obj_ptr->shotscur--;
        return(0);
    }
    return(zap_obj(ply_ptr, obj_ptr, cmnd));
}

/************************************************************************/
/*              zap_obj                 */
/************************************************************************/

/* This function is a subfunction of zap that accepts a player and  */
/* an object as its parameters.                     */

int zap_obj(creature *ply_ptr, object *obj_ptr, cmd *cmnd )
{
    int splno, c, fd, n;
    int (*fn)();

    fd = ply_ptr->fd;
    splno = obj_ptr->magicpower-1;
    if (splno < 0) 
	return(0);
    fn = get_spell_function(splno);

    if(fn == offensive_spell || fn == room_damage) {
        for(c=0; ospell[c].splno != get_spell_num(splno); c++)
            if(ospell[c].splno == -1) return(0);
        n = (*fn)(ply_ptr, cmnd, WAND, get_spell_name(splno),
            &ospell[c]);
    }
    else
        if (!F_ISSET(obj_ptr,ODDICE))
            n = (*fn)(ply_ptr, cmnd, WAND);
        else
            n = (*fn)(ply_ptr, cmnd, WAND, obj_ptr);

    if(n) {
        if(obj_ptr->use_output[0])
		{
            output_nl(fd, obj_ptr->use_output);
		}

        obj_ptr->shotscur--;
    }

    return(0);

}

/************************************************************************/
/*              offensive_spell             */
/************************************************************************/

/* This function is called by all spells whose sole purpose is to do    */
/* damage to a given creature.                      */

int offensive_spell(creature *ply_ptr, cmd *cmnd, int how, char *spellname, osp_t *osp )
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int     m, fd, dmg, bns=0;
    long        addrealm;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(ply_ptr->mpcur < osp->mp && how == CAST) {
        output(fd, "Not enough magic points.\n");
        return(0);
    }

    if(!S_ISSET(ply_ptr, osp->splno) && how == CAST) {
        output(fd, "You don't know that spell.\n");
        return(0);
    }

    if(ply_ptr->level < osp->intcast - (bonus[(int)ply_ptr->intelligence] * 3)) {
        output(fd, "You are not experienced enough to cast that spell.\n");
        return(0);
    }

    if(F_ISSET(ply_ptr, PINVIS)) {
        F_CLR(ply_ptr, PINVIS);
        output(fd, "Your invisibility fades.\n");
        broadcast_rom(fd, ply_ptr->rom_num, "%M fades into view.",
                  m1arg(ply_ptr));
    }

    if(how == CAST) switch(osp->bonus_type) {
    case 1:
        bns = bonus[(int)ply_ptr->intelligence] +
            mprofic(ply_ptr, osp->realm)/10;
        break;
    case 2:
        bns = bonus[(int)ply_ptr->intelligence] +
            mprofic(ply_ptr, osp->realm)/6;
        break;
    case 3:
        bns = bonus[(int)ply_ptr->intelligence] +
            mprofic(ply_ptr, osp->realm)/4;
        break;
    }

    if(F_ISSET(rom_ptr, RWATER)) {
        switch(osp->realm) {
        case WATER: bns *= 2; break;
        case FIRE:  bns = MIN(-bns, -5); break;
        }
    }
    else if(F_ISSET(rom_ptr, RFIRER)) {
        switch(osp->realm) {
        case FIRE:  bns *= 2; break;
        case WATER: bns = MIN(-bns, -5); break;
        }
    }
    else if(F_ISSET(rom_ptr, RWINDR)) {
        switch(osp->realm) {
        case WIND:  bns *= 2; break;
        case EARTH: bns = MIN(-bns, -5); break;
        }
    }
    else if(F_ISSET(rom_ptr, REARTH)) {
        switch(osp->realm) {
        case EARTH: bns *= 2; break;
        case WIND:  bns = MIN(-bns, -5); break;
        }
    }

    /* Cast on self */
    if(cmnd->num == 2) {

        dmg = dice(osp->ndice, osp->sdice, osp->pdice+bns);
        dmg = MAX(1, dmg);

        ply_ptr->hpcur -= dmg;

        if(how == CAST)
            ply_ptr->mpcur -= osp->mp;

        if(how == CAST || how == SCROLL || how == WAND)  {
            sprintf(g_buffer, "You cast a %s spell on yourself.\n",
                spellname);
            output(fd, g_buffer);
            sprintf(g_buffer, "The spell did %d damage.\n", dmg);
            output(fd, g_buffer);
            sprintf(g_buffer, "%%M casts a %s spell on %sself.", 
				spellname, F_ISSET(ply_ptr, PMALES) ? "him":"her");
            broadcast_rom(fd, ply_ptr->rom_num, 
                      g_buffer, m1arg(ply_ptr));
        }
        else if(how == POTION) {
            output(fd, "Yuck! That's terrible!\n");
            sprintf(g_buffer, "%d hit points removed.\n", dmg);
            output(fd, g_buffer);
        }

        if(ply_ptr->hpcur < 1) {
            output(fd, "Don't be stupid.\n");
            ply_ptr->hpcur = 1;
            return(2);
        }

    }

    /* Cast on monster or player */
    else {

        if(how == POTION) {
            output(fd, "You can only use a potion on yourself.\n");
            return(0);
        }

	    crt_ptr = find_crt_in_rom(ply_ptr, rom_ptr,
               cmnd->str[2], cmnd->val[2], MON_FIRST);


		if(!crt_ptr || crt_ptr == ply_ptr 
			|| (crt_ptr->type == PLAYER  && strlen(cmnd->str[2]) < 3)) {
			output(fd, "That's not here.\n");
			return(0);
		}


		/* fix same name bug here - JPF */
		if( ply_ptr->type == MONSTER )
		{
			/* for monster casting we need to make sure its not on itself */
			if( ply_ptr == crt_ptr )
			{
				/* look for second creature with same name */
				crt_ptr = find_crt_in_rom(ply_ptr, rom_ptr, cmnd->str[2], 2,
					MON_FIRST);

				if(!crt_ptr || crt_ptr == ply_ptr ) 
					return(0);
		    }
		}

		if(!is_crt_killable(crt_ptr, ply_ptr)) 
		{
			return(0);
		}

        if(ply_ptr->type == PLAYER && crt_ptr->type == PLAYER && crt_ptr != ply_ptr) {
		    if(ply_ptr->level<4 && crt_ptr->level>9){
				output(fd, "That would be foolish.\n");
				return(0);
			}

			if ( !pkill_allowed(ply_ptr, crt_ptr) )
			{
				return(0);
			}
		}


	if(crt_ptr->type != MONSTER)	
        if(is_charm_crt(ply_ptr->name, crt_ptr) && F_ISSET(ply_ptr, PCHARM)){
                output(fd, "You just can't bring yourself to do that.\n");
                return(0);
                }

        if(how == CAST)
            ply_ptr->mpcur -= osp->mp;

		if(spell_fail(ply_ptr, how)){
            return(0);
        }

        dmg = dice(osp->ndice, osp->sdice, osp->pdice+bns);
        dmg = MAX(1, dmg);

        if((crt_ptr->type == PLAYER && F_ISSET(crt_ptr, PRMAGI)) ||
           (crt_ptr->type != PLAYER && F_ISSET(crt_ptr, MRMAGI)))
            dmg -= (dmg * 2 * MIN(50, crt_ptr->piety + crt_ptr->intelligence)) / 100;

        m = MIN(crt_ptr->hpcur, dmg);

        addrealm = (m * crt_ptr->experience) / MAX(1,crt_ptr->hpmax);
        addrealm = MIN(addrealm, crt_ptr->experience);
        if(crt_ptr->type != PLAYER)
            ply_ptr->realm[osp->realm-1] += addrealm;
        if(crt_ptr->type != PLAYER) 
		{
			/* if(is_charm_crt(crt_ptr->name, ply_ptr))
				del_charm_crt(crt_ptr, ply_ptr); */
            
			add_enm_crt(ply_ptr->name, crt_ptr);
			add_enm_dmg(ply_ptr->name, crt_ptr, m);
        }

        crt_ptr->hpcur -= dmg;
        if(how == CAST || how == SCROLL || how == WAND) {
            sprintf(g_buffer, "You cast a %s spell on %s.\n", spellname,
                crt_ptr->name);
            output(fd, g_buffer);
            sprintf(g_buffer, "The spell did %d damage.\n", dmg);
            output(fd, g_buffer);

            sprintf(g_buffer, "%%M casts a %s spell on %%m.",
                       spellname);

            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       g_buffer, m2args(ply_ptr, crt_ptr));


            sprintf(g_buffer, 
                  "%%M casts a %s spell on you for %d damage.\n",
                  spellname, dmg);
            mprint(crt_ptr->fd, g_buffer, m1arg(ply_ptr));

			/* give players exp for being hit if the monster is killed */
			if ( crt_ptr->type == PLAYER && ply_ptr->type == MONSTER )
			{
				add_enm_dmg(crt_ptr->name, ply_ptr, dmg);
			}
        }


        if(crt_ptr->hpcur < 1) {
            mprint(fd, "You killed %m.\n", m1arg(crt_ptr));
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                      "%M killed %m.", m2args(ply_ptr, crt_ptr));
            die(crt_ptr, ply_ptr);
            return(2);
        }

    }

    return(1);

}
