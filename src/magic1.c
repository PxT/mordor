/*
 * MAGIC1.C:
 *
 *  User routines dealing with magic spells.  Potions, wands,
 *  scrolls, and casting are all covered.
 *
 *  Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"
#ifdef DMALLOC
  #include "/usr/local/include/dmalloc.h"
#endif

/**********************************************************************/
/*              cast                      */
/**********************************************************************/

/* This function allows a player to cast a magical spell.  It looks at */
/* the second parsed word to find out if the spell-name is valid, and  */
/* then calls the appropriate spell function.                  */

int cast(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    long    i, t, z, chance;
    int (*fn)();
    int fd, splno, c = 0, match = 0, n;

    fd = ply_ptr->fd;

    if(cmnd->num < 2) {
        print(fd, "Cast what?\n");
        return(0);
    }
    if(F_ISSET(ply_ptr, PBLIND)) {
	ANSI(fd, RED);
	print(fd, "You can't see to direct the incantation!\n");
	ANSI(fd, WHITE);
	return(0);
    }
    if(F_ISSET(ply_ptr, PSILNC)) {
	ANSI(fd, YELLOW);
	print(fd, "You can't recite the incantation!\n");
	ANSI(fd, WHITE);
	return(0);
    }
    do {
        if(!strcmp(cmnd->str[1], spllist[c].splstr)) {
            match = 1;
            splno = c;
            break;
        }
        else if(!strncmp(cmnd->str[1], spllist[c].splstr, 
            strlen(cmnd->str[1]))) {
            match++;
            splno = c;
        }
        c++;
    } while(spllist[c].splno != -1);

    if(match == 0) {
        print(fd, "That spell does not exist.\n");
        return(0);
    }

    else if(match > 1) {
        print(fd, "Spell name is not unique.\n");
        return(0);
    }

    if(F_ISSET(ply_ptr->parent_rom, RNOMAG) && ply_ptr->class < CARETAKER) {
        print(fd, "Nothing happens.\n");
        return(0);
    }

    i = LT(ply_ptr, LT_SPELL);
    t = time(0);

    if(t < i) {
        please_wait(fd, i-t);
        return(0);
    }

    F_CLR(ply_ptr, PHIDDN);

    fn = spllist[splno].splfn;

    if(fn == offensive_spell) {
        for(c=0; ospell[c].splno != spllist[splno].splno; c++)
            if(ospell[c].splno == -1) return(0);
        n = (*fn)(ply_ptr, cmnd, CAST, spllist[splno].splstr,
            &ospell[c]);
    }

    else
        n = (*fn)(ply_ptr, cmnd, CAST);

    if(n) {
        ply_ptr->lasttime[LT_SPELL].ltime = t;
        if(ply_ptr->class == CLERIC || ply_ptr->class == MAGE)
            ply_ptr->lasttime[LT_SPELL].interval = 3;
        else if(ply_ptr->class == BARD)
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

int teach(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    room        *rom_ptr;
    creature    *crt_ptr;
    int     fd, splno, c = 0, match = 0;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(cmnd->num < 3) {
        print(fd, "Teach whom what?\n");
        return(0);
    }
    if(F_ISSET(ply_ptr, PBLIND)) {
        ANSI(fd, RED);
        print(fd, "You can't see to do that!\n");
        ANSI(fd, WHITE);
        return(0);
    }
    if(F_ISSET(ply_ptr, PSILNC)) {
	ANSI(fd, YELLOW);
	print(fd, "You can't speak to do that!\n");
	ANSI(fd, WHITE);
	return(0);
    }

    if((ply_ptr->class != MAGE && ply_ptr->class != CLERIC) && ply_ptr->class < CARETAKER)  {
        print(fd, "Only mages and clerics may teach spells.\n");
        return(0);
    }

    cmnd->str[1][0] = up(cmnd->str[1][0]);
    crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply, cmnd->str[1], 
               cmnd->val[1]);

    if(!crt_ptr) {
        print(fd, "I don't see that person here.\n");
        return(0);
    }

    do {
        if(!strcmp(cmnd->str[2], spllist[c].splstr)) {
            match = 1;
            splno = c;
            break;
        }
        else if(!strncmp(cmnd->str[2], spllist[c].splstr, 
            strlen(cmnd->str[2]))) {
            match++;
            splno = c;
        }
        c++;
    } while(spllist[c].splno != -1);

    if(match == 0) {
        print(fd, "That spell does not exist.\n");
        return(0);
    }

    else if(match > 1) {
        print(fd, "Spell name is not unique.\n");
        return(0);
    }

    if(!S_ISSET(ply_ptr, spllist[splno].splno)) {
        print(fd, "You don't know that spell.\n");
        return(0);
    }

    if((spllist[splno].splno > 3) && (ply_ptr->class != CLERIC && ply_ptr->class < CARETAKER)) {
        print(fd, "You may not teach that spell to anyone.\n");
        return(0);
    }

    if((spllist[splno].splno > 5) && (ply_ptr->class != DM)) {
        print(fd, "You may not teach that spell to anyone.\n");
        return(0);
    }

    if((spllist[splno].splno < 3) && (ply_ptr->class != MAGE && ply_ptr->class < CARETAKER)) {
        print(fd, "You may not teach that spell to anyone.\n");
        return(0);
    }
   

    F_CLR(ply_ptr, PHIDDN);

    S_SET(crt_ptr, spllist[splno].splno);
    print(crt_ptr->fd, "%M teaches you the %s spell.\n", ply_ptr,
          spllist[splno].splstr);
    print(fd, "Spell \"%s\" taught to %m.\n", spllist[splno].splstr,
          crt_ptr);

    broadcast_rom2(fd, crt_ptr->fd, rom_ptr->rom_num,
               "%M taught %m the %s spell.", ply_ptr, crt_ptr,
               spllist[splno].splstr);

    return(0);

}

/**********************************************************************/
/*              study                     */
/**********************************************************************/

/* This function allows a player to study a scroll, and learn the spell */
/* that is on it.                           */

int study(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    object  *obj_ptr;
    int fd, n, match=0;

    fd = ply_ptr->fd;

    if(cmnd->num < 2) {
        print(fd, "Study what?\n");
        return(0);
    }
    if(F_ISSET(ply_ptr, PBLIND)) {
        ANSI(fd, RED);
        print(fd, "You can't see to do that!\n");
        ANSI(fd, WHITE);
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
        print(fd, "You don't have that.\n");
        return(0);
    }

    if(obj_ptr->type != SCROLL) {
        print(fd, "That's not a scroll.\n");
        return(0);
    }

    if((F_ISSET(obj_ptr, OGOODO) && ply_ptr->alignment < -100) ||
       (F_ISSET(obj_ptr, OEVILO) && ply_ptr->alignment > 100)) {
        print(fd, "%I burns you and you drop it.\n", obj_ptr);
        del_obj_crt(obj_ptr, ply_ptr);
        add_obj_rom(obj_ptr, ply_ptr->parent_rom);
        return(0);
    }

	if (F_ISSET(obj_ptr,OPLDGK) && 
		(BOOL(F_ISSET(obj_ptr,OKNGDM)) != BOOL(F_ISSET(ply_ptr,PKNGDM)))){
		print(fd, "You are unable to use %i.\n",obj_ptr);
        return(0);
	}

  if(F_ISSET(obj_ptr,OCLSEL))
        if(!F_ISSET(obj_ptr,OCLSEL + ply_ptr->class) && ( ply_ptr->class < CARETAKER)){
                print(fd, "Your class prevents you from using %i.\n",obj_ptr);
                return(0);
        }                 

    F_CLR(ply_ptr, PHIDDN);

    print(fd, "You learn the %s spell.\n", 
          spllist[obj_ptr->magicpower-1].splstr);
    print(fd, "%I disintegrates!\n", obj_ptr);
    broadcast_rom(fd, ply_ptr->rom_num, "%M studies %1i.",
              ply_ptr, obj_ptr);

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

int readscroll(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    object  *obj_ptr;
    int (*fn)();
    long    i, t;
    int fd, n, match=0, c, splno;

    fd = ply_ptr->fd;
    fn = 0;

    if(cmnd->num < 2) {
        print(fd, "Read what?\n");
        return(0);
    }
    if(F_ISSET(ply_ptr, PBLIND)) {
        ANSI(fd, RED);
        print(fd, "You can't see to do that!\n");
        ANSI(fd, WHITE);
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
        print(fd, "You don't have that.\n");
        return(0);
    }

    if(obj_ptr->special) {
        n = special_obj(ply_ptr, cmnd, SP_MAPSC);
        if(n != -2) return(MAX(0, n));
    }

    if(obj_ptr->type != SCROLL) {
        print(fd, "That's not a scroll.\n");
        return(0);
    }

    if((F_ISSET(obj_ptr, OGOODO) && ply_ptr->alignment < -100) ||
       (F_ISSET(obj_ptr, OEVILO) && ply_ptr->alignment > 100)) {
        print(fd, "%I burns you and you drop it.\n", obj_ptr);
        del_obj_crt(obj_ptr, ply_ptr);
        add_obj_rom(obj_ptr, ply_ptr->parent_rom);
        return(0);
    }

	if (F_ISSET(obj_ptr,OPLDGK) &&
        (BOOL(F_ISSET(obj_ptr,OKNGDM)) != BOOL(F_ISSET(ply_ptr,PKNGDM)))){
        print(fd, "You are unable to read %i.\n",obj_ptr);
        return(0);
    }              


  if(F_ISSET(obj_ptr,OCLSEL))
        if(!F_ISSET(obj_ptr,OCLSEL + ply_ptr->class) && ( ply_ptr->class < CARETAKER)){
                print(fd, "Your class prevents you from reading %i.\n",obj_ptr);
                return(0);
        }                 

    if(F_ISSET(ply_ptr->parent_rom, RNOMAG) || (obj_ptr->magicpower-1 < 0)) {
        print(fd, "Nothing happens.\n");
        return(0);
    }

    i = LT(ply_ptr, LT_READS);
    t = time(0);

    if(i > t) {
        please_wait(fd, i-t);
        return(0);
    }

    F_CLR(ply_ptr, PHIDDN);

    ply_ptr->lasttime[LT_READS].ltime = t;
    ply_ptr->lasttime[LT_READS].interval = 3;


    if(spell_fail(ply_ptr, SCROLL)){
        print(fd, "%I disintegrates.\n", obj_ptr);
        del_obj_crt(obj_ptr, ply_ptr);
        free_obj(obj_ptr);
        return(0);
    }

    splno = obj_ptr->magicpower-1;
    fn = spllist[splno].splfn;

    if(fn == offensive_spell) {
        for(c=0; ospell[c].splno != spllist[splno].splno; c++)
            if(ospell[c].splno == -1) return(0);
        n = (*fn)(ply_ptr, cmnd, SCROLL, spllist[splno].splstr,
            &ospell[c]);
    }

    else
        n = (*fn)(ply_ptr, cmnd, SCROLL);

    if(n) {
        if(obj_ptr->use_output[0])
            print(fd, "%s\n", obj_ptr->use_output);

        print(fd, "%I disintegrates.\n", obj_ptr);
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

int drink(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    object  *obj_ptr;
    int (*fn)();
    long    i, t;
    int fd, n, match=0, c, splno;

    fd = ply_ptr->fd;
    fn = 0;

    if(cmnd->num < 2) {
        print(fd, "Drink what?\n");
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
        print(fd, "You don't have that.\n");
        return(0);
    }

    if(obj_ptr->type != POTION) {
        print(fd, "That's not a potion.\n");
        return(0);
    }

    if(obj_ptr->shotscur < 1 || (obj_ptr->magicpower-1 < 0)) {
        print(fd, "It's empty.\n");
        return(0);
    }

	if(F_ISSET(ply_ptr->parent_rom, RNOPOT)){
        print(fd, "%I starts to evaporates before you drink it.\n",obj_ptr);
        return(0);
	}
	
    if((F_ISSET(obj_ptr, OGOODO) && ply_ptr->alignment < -100) ||
       (F_ISSET(obj_ptr, OEVILO) && ply_ptr->alignment > 100)) {
        print(fd, "%I burns you and you drop it.\n", obj_ptr);
        del_obj_crt(obj_ptr, ply_ptr);
        add_obj_rom(obj_ptr, ply_ptr->parent_rom);
        return(0);
    }

	if (F_ISSET(obj_ptr,OPLDGK) &&
        (BOOL(F_ISSET(obj_ptr,OKNGDM)) != BOOL(F_ISSET(ply_ptr,PKNGDM)))){
        print(fd, "You are unable to drink %i.\n",obj_ptr);
        return(0);
    }              


  if(F_ISSET(obj_ptr,OCLSEL))
        if(!F_ISSET(obj_ptr,OCLSEL + ply_ptr->class) && ( ply_ptr->class < CARETAKER)){
                print(fd, "Your class prevents you from drinking %i.\n",obj_ptr);
                return(0);
        }                 

    F_CLR(ply_ptr, PHIDDN);

    splno = obj_ptr->magicpower-1;
    fn = spllist[splno].splfn;

    if(fn == offensive_spell) {
        for(c=0; ospell[c].splno != spllist[splno].splno; c++)
            if(ospell[c].splno == -1) return(0);
        n = (*fn)(ply_ptr, cmnd, POTION, spllist[splno].splstr,
            &ospell[c]);
    }

    else
        n = (*fn)(ply_ptr, cmnd, POTION);

    if(n) {
        if(obj_ptr->use_output[0])
            print(fd, "%s\n", obj_ptr->use_output);

        print(fd, "Potion drank.\n");
        broadcast_rom(fd, ply_ptr->rom_num, "%M drank %1i.", 
                  ply_ptr, obj_ptr);

        if(--obj_ptr->shotscur < 1) {
            print(fd, "%I disintegrates.\n", obj_ptr);
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

int zap(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    object  *obj_ptr;
    long    i, t;
    int fd, n, match=0;

    fd = ply_ptr->fd;

    if(cmnd->num < 2) {
        print(fd, "Use what?\n");
        return(0);
    }
    if(F_ISSET(ply_ptr, PBLIND)) {
        ANSI(fd, RED);
        print(fd, "You can't see to use that!\n");
        ANSI(fd, WHITE);
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
        print(fd, "You don't have that.\n");
        return(0);
    }

    if(obj_ptr->type != WAND) {
        print(fd, "That's not a wand or staff.\n");
        return(0);
    }

    if(obj_ptr->shotscur < 1) {
        print(fd, "It's used up.\n");
        return(0);
    }

    if((F_ISSET(obj_ptr, OGOODO) && ply_ptr->alignment < -100) ||
       (F_ISSET(obj_ptr, OEVILO) && ply_ptr->alignment > 100)) {
        print(fd, "%I burns you and you drop it.\n", obj_ptr);
        del_obj_crt(obj_ptr, ply_ptr);
        add_obj_rom(obj_ptr, ply_ptr->parent_rom);
        return(0);
    }

	if (F_ISSET(obj_ptr,OPLDGK) &&
        (BOOL(F_ISSET(obj_ptr,OKNGDM)) != BOOL(F_ISSET(ply_ptr,PKNGDM)))){
        print(fd, "You are unable to use %i.\n",obj_ptr);
        return(0);
    }              


  if(F_ISSET(obj_ptr,OCLSEL))
        if(!F_ISSET(obj_ptr,OCLSEL + ply_ptr->class) && ( ply_ptr->class < CARETAKER)){
                print(fd, "Your class prevents you from using %i.\n",obj_ptr);
                return(0);
        }                 


    if(F_ISSET(ply_ptr->parent_rom, RNOMAG) || (obj_ptr->magicpower < 1)) {
        print(fd, "Nothing happens.\n");
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

int zap_obj(ply_ptr, obj_ptr, cmnd)
creature    *ply_ptr;
object      *obj_ptr;
cmd     *cmnd;
{
    int splno, c, fd, n;
    int (*fn)();

    fd = ply_ptr->fd;
    splno = obj_ptr->magicpower-1;
    if (splno < 0) 
	return(0);
    fn = spllist[splno].splfn;

    if(fn == offensive_spell) {
        for(c=0; ospell[c].splno != spllist[splno].splno; c++)
            if(ospell[c].splno == -1) return(0);
        n = (*fn)(ply_ptr, cmnd, WAND, spllist[splno].splstr,
            &ospell[c]);
    }
    else
        if (!F_ISSET(obj_ptr,ODDICE))
            n = (*fn)(ply_ptr, cmnd, WAND);
        else
            n = (*fn)(ply_ptr, cmnd, WAND, obj_ptr);

    if(n) {
        if(obj_ptr->use_output[0])
            print(fd, "%s\n", obj_ptr->use_output);

        obj_ptr->shotscur--;
    }

    return(0);

}

/************************************************************************/
/*              offensive_spell             */
/************************************************************************/

/* This function is called by all spells whose sole purpose is to do    */
/* damage to a given creature.                      */

int offensive_spell(ply_ptr, cmnd, how, spellname, osp)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
char        *spellname;
osp_t       *osp;
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int     m, fd, dmg, bns=0;
    long        addrealm, f,t;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(ply_ptr->mpcur < osp->mp && how == CAST) {
        print(fd, "Not enough magic points.\n");
        return(0);
    }

    if(!S_ISSET(ply_ptr, osp->splno) && how == CAST) {
        print(fd, "You don't know that spell.\n");
        return(0);
    }


    if(F_ISSET(ply_ptr, PINVIS)) {
        F_CLR(ply_ptr, PINVIS);
        print(fd, "Your invisibility fades.\n");
        broadcast_rom(fd, ply_ptr->rom_num, "%M fades into view.",
                  ply_ptr);
    }

    if(how == CAST) switch(osp->bonus_type) {
    case 1:
        bns = bonus[ply_ptr->intelligence] +
            mprofic(ply_ptr, osp->realm)/10;
        break;
    case 2:
        bns = bonus[ply_ptr->intelligence] +
            mprofic(ply_ptr, osp->realm)/6;
        break;
    case 3:
        bns = bonus[ply_ptr->intelligence] +
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
            print(fd, "You cast a %s spell on yourself.\n",
                spellname);
            print(fd, "The spell did %d damage.\n", dmg);
            broadcast_rom(fd, ply_ptr->rom_num, 
                      "%M casts a %s spell on %sself.", 
                      ply_ptr, spellname,
                      F_ISSET(ply_ptr, PMALES) ? "him":"her");
        }
        else if(how == POTION) {
            print(fd, "Yuck! That's terrible!\n");
            print(fd, "%d hit points removed.\n", dmg);
        }

        if(ply_ptr->hpcur < 1) {
            print(fd, "Don't be stupid.\n");
            ply_ptr->hpcur = 1;
            return(2);
        }

    }

    /* Cast on monster or player */
    else {

        if(how == POTION) {
            print(fd, "You can only use a potion on yourself.\n");
            return(0);
        }

        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                   cmnd->str[2], cmnd->val[2]);

        if(!crt_ptr) {
            cmnd->str[2][0] = up(cmnd->str[2][0]);
            crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                       cmnd->str[2], cmnd->val[2]);

            if(!crt_ptr || crt_ptr == ply_ptr || 
               strlen(cmnd->str[2]) < 3) {
                print(fd, "That's not here.\n");
                return(0);
            }

        }

        if(crt_ptr->type == MONSTER && F_ISSET(crt_ptr, MUNKIL)) {
            print(fd, "You cannot harm %s.\n",
                F_ISSET(crt_ptr, MMALES) ? "him":"her");
            return(0);
        }

        if(ply_ptr->type == PLAYER && crt_ptr->type == PLAYER &&
            crt_ptr != ply_ptr) {
            if(F_ISSET(rom_ptr, RNOKIL) && ply_ptr->class < DM) {
                print(fd,"No killing allowed in this room.\n");
                return;
            }
	    if(ply_ptr->level<4 && crt_ptr->level>9){
		print(fd, "That would be foolish.\n");
		return;
	    }
            if((!F_ISSET(ply_ptr,PPLDGK) || !F_ISSET(crt_ptr,PPLDGK)) ||
                (BOOL(F_ISSET(ply_ptr,PKNGDM)) == BOOL(F_ISSET(crt_ptr,PKNGDM))) ||
                (! AT_WAR)) {
                if(!F_ISSET(ply_ptr, PCHAOS) && ply_ptr->class < DM) {
                    print(fd, "Sorry, you're lawful.\n");
                    return(0);
                }
                if(!F_ISSET(crt_ptr, PCHAOS) && ply_ptr->class < DM) {
                    print(fd, "Sorry, that player is lawful.\n");
                    return(0);
                }     
            }
        }
	if(crt_ptr->type != MONSTER)	
        if(is_charm_crt(ply_ptr->name, crt_ptr) && F_ISSET(ply_ptr, PCHARM)){
                print(fd, "You just can't bring yourself to do that.\n");
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
            dmg -= (dmg * 2 *
              MIN(50, crt_ptr->piety + crt_ptr->intelligence)) /
              100;

        m = MIN(crt_ptr->hpcur, dmg);

        addrealm = (m * crt_ptr->experience) / MAX(1,crt_ptr->hpmax);
        addrealm = MIN(addrealm, crt_ptr->experience);
        if(crt_ptr->type != PLAYER)
            ply_ptr->realm[osp->realm-1] += addrealm;
        if(crt_ptr->type != PLAYER) {
		/* if(is_charm_crt(crt_ptr->name, ply_ptr))
			del_charm_crt(crt_ptr, ply_ptr); */
            
	    add_enm_crt(ply_ptr->name, crt_ptr);
            add_enm_dmg(ply_ptr->name, crt_ptr, m);
        }

        crt_ptr->hpcur -= dmg;
        if(how == CAST || how == SCROLL || how == WAND) {
            print(fd, "You cast a %s spell on %s.\n", spellname,
                crt_ptr->name);
            print(fd, "The spell did %d damage.\n", dmg);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "%M casts a %s spell on %m.",
                       ply_ptr, spellname, crt_ptr);
            print(crt_ptr->fd, 
                  "%M casts a %s spell on you for %d damage.\n",
                  ply_ptr, spellname, dmg);
        }


        if(crt_ptr->hpcur < 1) {
            print(fd, "You killed %m.\n", crt_ptr);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                      "%M killed %m.", ply_ptr, crt_ptr);
            die(crt_ptr, ply_ptr);
            return(2);
        }

    }

    return(1);

}
