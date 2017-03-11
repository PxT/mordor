/*
 *      MAGIC9.C:
 *
 *      Additional spell-casting routines.
 *      (C) 1996 Brooke Paul
 *
 * $Id: magic9.c,v 6.42.2.13 2001/07/25 23:10:52 develop Exp $
 *
 * $Log: magic9.c,v $
 * Revision 6.42.2.13  2001/07/25 23:10:52  develop
 * fix for conjure
 *
 * Revision 6.42.2.12  2001/07/11 11:54:28  develop
 * conjure changes to make higher level pets
 *
 * Revision 6.42.2.11  2001/07/08 04:13:12  develop
 * made it so that druids can cast conjure at low levels
 * (adjusted mp cost for druids)
 *
 * Revision 6.42.2.10  2001/07/04 15:12:49  develop
 * adjusting timeouts for conjured creatures
 *
 * Revision 6.42.2.9  2001/07/04 14:09:21  develop
 * added dm_stat check for conjure time remaining
 *
 * Revision 6.42.2.8  2001/07/04 03:24:41  develop
 * increased the level for conjured monsters
 *
 * Revision 6.42.2.7  2001/07/04 02:57:17  develop
 * changing stock monster range from 1800 to 2000 and
 * making it so that each class has 20 pets to choose from
 *
 * Revision 6.42.2.6  2001/07/02 19:25:52  develop
 * incorporated patch from ptelford to fix problems with
 * del_conjured (let him fix his own code ;-))
 *
 * Revision 6.42.2.5  2001/07/02 13:02:20  develop
 * removed changed to del_conjured due to crash error
 *
 * Revision 6.42.2.4  2001/07/02 12:53:01  develop
 * added null on crt_ptr->following in del_conjured
 *
 * Revision 6.42.2.3  2001/07/02 02:24:43  develop
 * trying to fix del_conjured
 *
 * Revision 6.42.2.2  2001/07/02 02:10:37  develop
 * *** empty log message ***
 *
 * Revision 6.42.2.1  2001/07/02 02:04:59  develop
 * added clearing of conjured following link in del_conjured
 *
 * Revision 6.42  2001/07/01 19:01:46  develop
 * removed ability to have conjured creatures in RNOLEAV rooms
 *
 * Revision 6.41  2001/07/01 01:59:01  develop
 * monlvl adjustments and removal of MFOLLO from conjured
 * so they don't try to follow in battle and cause problems
 *
 * Revision 6.40  2001/07/01 01:54:32  develop
 * [more adjustments to lvlmod
 *
 * Revision 6.39  2001/07/01 01:48:39  develop
 * adjustments to monlvl formulas for conjure
 *
 * Revision 6.38  2001/07/01 01:32:15  develop
 * adjusted conjure timeouts
 *
 * Revision 6.37  2001/07/01 01:09:34  develop
 * lowered max level of conjured creatures from 25 to 14
 *
 * Revision 6.36  2001/06/30 19:12:51  develop
 * removed MFOLLO from conjure
 *
 * Revision 6.35  2001/06/30 05:49:23  develop
 * adjusted levels for casting of conjure
 *
 * Revision 6.34  2001/06/29 01:46:33  develop
 * conjure fine tuning
 *
 * Revision 6.33  2001/06/29 01:23:15  develop
 * conjured creatures cannot be killed by players if the conjurer
 * is lawful
 *
 * Revision 6.32  2001/06/28 05:04:03  develop
 * LT adjustment on conjure
 *
 * Revision 6.31  2001/06/28 04:52:33  develop
 * added ltime check on ply_ptr for conjure
 *
 * Revision 6.30  2001/06/28 01:44:59  develop
 * changed error output for fail to load crt in conjure
 *
 * Revision 6.29  2001/06/28 01:41:25  develop
 * added output on error in load of creature for conjure
 *
 * Revision 6.28  2001/06/28 01:30:03  develop
 * changed lifetime for conjure creatures and added error
 * output if conjure fails to load from DB
 *
 * Revision 6.27  2001/06/27 04:05:41  develop
 * minor pkill/conjure fix
 *
 * Revision 6.26  2001/06/27 01:22:05  develop
 * conjure debugging
 *
 * Revision 6.25  2001/06/26 22:30:48  develop
 * conjure bug fixes
 *
 * Revision 6.24  2001/06/23 17:29:15  develop
 * fixed bug in conjure that was calling a potentially null
 * crt_ptr when casting fails.
 *
 * Revision 6.23  2001/06/23 05:38:31  develop
 * enabled conjure with attack caster on failure
 *
 * Revision 6.22  2001/06/20 05:00:23  develop
 * *** empty log message ***
 *
 * Revision 6.21  2001/06/12 23:54:14  develop
 * blocked casting of conjure when you're possessing since
 * it caused a crash during testing
 *
 * Revision 6.20  2001/06/12 14:37:26  develop
 * removed ability to cast dispel, curse and drain
 * in RNOKILL rooms
 *
 * Revision 6.19  2001/06/11 00:05:46  develop
 * removed conjure from active use until testing is completed
 *
 * Revision 6.18  2001/06/07 15:44:49  develop
 * conjure fix -- always need to call add_active
 *
 * Revision 6.17  2001/06/07 05:18:11  develop
 * conjure mod -- give cnj_ptr vig and mend automatically
 *
 * Revision 6.16  2001/06/06 19:36:46  develop
 * conjure added
 *
 * Revision 6.15  2001/03/09 05:14:07  develop
 * changes to weight checking
 *
 * Revision 6.14  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 *
 */


#include "../include/mordb.h"
#include "mextern.h"
#include <stdlib.h>

/***************************************************************************
*	Recharge wand/Transmute
*
*  This allows an Alchemist to recharge a wand provided s/he knows the
*  spell of the wand and has enough gold.
*
*/
int recharge_wand(creature *ply_ptr, cmd *cmnd )
{
	object  *obj_ptr;
	int	fd, how=CAST, cost;

	fd=ply_ptr->fd;
	if(ply_ptr->class < BUILDER && ply_ptr->class != ALCHEMIST) {
		output(fd, "Only an alchemist can do that.\n");
		return(0);
	}

	if(ply_ptr->level < 6 && ply_ptr->class < BUILDER) {
		output(fd, "You are not experienced enough to do that yet.\n");
		return(0);
	}
	if(!dec_daily(&ply_ptr->daily[DL_RCHRG]) && ply_ptr->class < BUILDER) {
		output(fd, "You have recharged enough wands for one day.\n");
		return(0);
	}
	if(ply_ptr->ready[WIELD-1]) { 
		output(fd, "Your hands are too full to do that.\n");
		return(0);
	}
	if(F_ISSET(ply_ptr, PBLIND)) {
        output_wc(fd, "How do you do that?  You're blind.\n", BLINDCOLOR);
		return(0);
	}
	if(!ply_ptr->ready[HELD-1] || ply_ptr->ready[HELD-1]->type != WAND) {
		output(fd, "You must hold the wand you wish to recharge.\n");
		return(0);
	}
	obj_ptr=ply_ptr->ready[HELD-1];

	if(obj_ptr->shotscur) {
		mprint(fd, "That %i still has magic in it.\n", m1arg(obj_ptr));
		return(0);
	}
	if(!S_ISSET(ply_ptr, obj_ptr->magicpower-1)){
		output(fd, "You dont know the spell of this wand.\n");
		return(0);
	}
	cost=15*obj_ptr->shotsmax;
	if(F_ISSET(obj_ptr, ONOFIX))
		cost *=2;
	if(ply_ptr->mpcur-cost < 0 || cost <1) {
		output(fd, "You dont have enough magic points.\n");
		return(0);
	}
	ply_ptr->mpcur -= cost;

        if(spell_fail(ply_ptr, how)) {
		output(fd, "The wand glows bright red and explodes!\n");
		
		sprintf(g_buffer, "A wand explodes in %s's hand!\n", ply_ptr->name);
		broadcast_rom(fd, ply_ptr->rom_num, g_buffer, NULL);
		
		ply_ptr->hpcur -= mrand(10,20);
		if(ply_ptr->hpcur < 1) {
			ply_ptr->hpcur = 0;
			die(ply_ptr, ply_ptr);
		}
		dequip(ply_ptr,ply_ptr->ready[HELD-1]);
		ply_ptr->ready[HELD-1]=0;
		free_obj(obj_ptr);
		return(0);

	}

	/* success! */
	obj_ptr->shotscur=obj_ptr->shotsmax;
	sprintf(g_buffer, "You successfully recharge the %s.\n", obj_ptr->name);
	output(fd, g_buffer);

	sprintf(g_buffer, "%s recharges a %s.", ply_ptr->name, obj_ptr->name);
	broadcast_rom(fd, ply_ptr->rom_num, g_buffer, NULL);
	return(0);
}

/**************************************************************************
*	This function enables alchemists to enchant an item based on level 
*  for a short period of time.
*
*/
int tmp_enchant(creature *ply_ptr, cmd *cmnd )
{
	int fd;
	object *obj_ptr;
	char	ench;
 
	fd=ply_ptr->fd;

	if(ply_ptr->class != ALCHEMIST && ply_ptr->class < BUILDER) {
		output(fd, "Only alchemists can do that.\n");
		return(0);
	}
	if(ply_ptr->level < 10 && ply_ptr->class < BUILDER) {
		output(fd, "You are not experienced enough to do that.\n");
		return(0);
	}
	if(F_ISSET(ply_ptr, PBLIND)) {
        output_wc(fd, "You can't do that.  You're blind!\n", BLINDCOLOR);
        return(0);
        }
	obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj, cmnd->str[1], cmnd->val[1]);	
	
	if(!obj_ptr) {
		output(fd, "That's not in your inventory.\n");
		return(0);
	}
	/* We have to use the magicpower and magicrealm fields to store 
	the duration and lasttime information.  This may be fixed by a 
	object struct change in the future. */
	if(obj_ptr->adjustment || obj_ptr->pdice ||obj_ptr->magicpower || obj_ptr->magicrealm || obj_ptr->type >5) { 
		output(fd, "You cannot enchant that.\n");
		return(0);
	}
	if(F_ISSET(obj_ptr, OENCHA) || F_ISSET(obj_ptr,ORENCH)) {
		sprintf(g_buffer, "The %s is already enchanted.\n", obj_ptr->name);
		output(fd, g_buffer);
		return(0);
	}
	if(!dec_daily(&ply_ptr->daily[DL_ENCHA]) && ply_ptr->class < BUILDER){
		output(fd, "You have enchanted enough today.\n");
		return(0);
	}

	obj_ptr->magicpower=(char)mrand(1, (ply_ptr->intelligence*4));
	obj_ptr->magicrealm=(char)mrand(1, (ply_ptr->constitution*4));

	/* enchant based on level */
	/* level 10-14 is + 1, 15-19 + 2 and 20+ = +3 */
	ench = MAX(3, MIN(1, (ply_ptr->level - 10) / 5 + 1)); 
	F_SET(obj_ptr, OTMPEN);
	enchant_obj(obj_ptr, ench );

	mprint(fd, "%I begins to glow brightly.\n", m1arg(obj_ptr));
    broadcast_rom(fd, ply_ptr->rom_num, "%M enchants %1i.", 
		m2args(ply_ptr,obj_ptr));
	return(0);

}

/* Removes the conjured monster specified in the first parameter from the follow *
 * list of its owner.								 */
int del_conjured(creature *crt_ptr)
{
	ctag *fol, *prev;


	if(!F_ISSET(crt_ptr, MCONJU))
		return(0);

	fol = crt_ptr->following->first_fol;
	if(!fol)
		return(0);	/* shouldnt happen, but just in case */

	prev = fol;

	/* Find who conjured us, and remove the crt from their follow queue */
	if(fol->crt == crt_ptr) {
		crt_ptr->following->first_fol = fol->next_tag;
		crt_ptr->following = 0;
	}
	else while(fol) {		
		if(fol->crt == crt_ptr) {
			prev->next_tag = fol->next_tag;
			fol->crt = 0;
			crt_ptr->following = 0;
			break;
		}
	prev = fol;
	fol = fol->next_tag;
	}

	return(0);
}
	
/*
*
*	Conjure
*
*	Brings a creature which will fight with you and vig/mend.
*	The creature number is based upon class:
*
*	   	Druid:          2001-2020
*               Alchemist:      2021-2040
*               Bard:           2041-2060
*               Mage:           2061-2080
*
*        Highest level is 14 
*/

int conjure(creature *ply_ptr, cmd *cmnd, int how)
{
        creature	*crt_ptr,*cnj_ptr;
        int     	fd, cnjnum,clsmod,algmod,monlvl,monnum,mpcost;
	ctag		*cp,*pp;
	time_t		t,i;
	room		*rom_ptr;

        fd=ply_ptr->fd;
	rom_ptr=ply_ptr->parent_rom;

	if(!S_ISSET(ply_ptr, SCONJU) && how == CAST) {
        	output(fd, "You don't know that spell.\n");
        	return(0);
	}

	/* only players may conjure */
        if(ply_ptr->type != PLAYER)
                return(0);

	/* don't allow conjuring in no summon rooms */
	if(F_ISSET(rom_ptr, RNOLEA) && ply_ptr->class < IMMORTAL) {
		output(fd, "The spell fizzles.\n");
		return(0);
	}

	/* Don't cast while possessing....*crash* */
	if(F_ISSET(ply_ptr, PALIAS)) {
		output(fd, "You may not cast that now.\n");
		return(0);
	}

	monlvl=MIN(15,ply_ptr->level-2);
	monlvl -= mrand(0,5);

	monnum=MIN(19,ply_ptr->level);
	monnum -= mrand(0,5);

	if(monnum < 0) 
		monnum=0;

	if(ply_ptr->class == DRUID) {
		if(ply_ptr->level < 5) {
			output(fd, "You are not experienced enough to cast that.\n");
			return(0);
		}
		clsmod=0;
	} else if(ply_ptr->class == ALCHEMIST) {
		if(ply_ptr->level < 8) {
                        output(fd, "You are not experienced enough to cast that.\n");
			return(0);
		}
		clsmod=21;
		monlvl -= 1;
	} else if(ply_ptr->class == BARD) {
		if(ply_ptr->level < 12) {
                        output(fd, "You are not experienced enough to cast that.\n");
                        return(0);
                }
		clsmod=41;
		monlvl -= 2;
	} else if(ply_ptr->class == MAGE) {
		if(ply_ptr->level < 15) {
			output(fd, "You are not experienced enough to cast that.\n");
			return(0);
                }
		clsmod=61;
		monlvl -= 3;
	} else if(ply_ptr->class > IMMORTAL) {
		clsmod=0;
	} else {
		output(fd, "That is beyond your understanding.\nThe spell fizzles.\n");
		return(0);
	}
	
	t = time(0);
	i = ply_ptr->lasttime[LT_CONJU].ltime;
	if(t-i < ply_ptr->lasttime[LT_CONJU].interval && ply_ptr->class < DM) {
		please_wait(fd, (ply_ptr->lasttime[LT_CONJU].interval - (t-i)));
		return(0);
	}


	cp=ply_ptr->first_fol;
	while (cp) {
		if (cp->crt->type == MONSTER && F_ISSET(cp->crt, MCONJU)) {
			output(fd, "You are only allowed one conjured creature at a time.\n");
			return(0);
		}
		cp = cp->next_tag;
	}

	/* Grab a template monster in the 2000-2080 range.  
		Monster selection
		depends upon class, alignment and a random factor.

               Druid:          2001-2020
               Alchemist:      2021-2040
               Bard:           2041-2060
               Mage:           2061-2080

	Highest level conjurable is 14 (can change this pretty easily) */

	algmod = 0;

	/* Make the monster weaker if caster's align is skewed */
	if ((ply_ptr->alignment > 100) || (ply_ptr->alignment < -100)) {
		monlvl -= 1; 
		algmod = 10;
	}

	/* Make the monster weaker if caster's align is skewed */
	if ((ply_ptr->alignment > 200) || (ply_ptr->alignment < -200)) {
		monlvl -= 1; 
		algmod = 20;
	}

	if(monlvl<1) {
               monlvl=1;
        }

	if(ply_ptr->class == DRUID) {
		mpcost=monlvl;
	} else {
		mpcost=10+monlvl;
	}

	if(ply_ptr->mpcur < mpcost && how == CAST) {
        	output(fd, "Not enough magic points.\n");
        	return(0);
    	}

	if(how == CAST) {
		ply_ptr->mpcur -= mpcost;
	}

	/* Select the conjured monster */
	cnjnum=monnum + clsmod + 2000; /* adjust this last number 
						to set the starting range                                                            for the canned pets */

	if(spell_fail(ply_ptr, how)) 
                return(0);
       
	if(cmnd->num == 3) {
		crt_ptr = find_crt_in_rom(ply_ptr, rom_ptr,
                cmnd->str[2], cmnd->val[2], PLY_FIRST);

		if(!crt_ptr) {
            		output(fd, "That's not here.\n");
            		return(0);
        	}

		if(how == POTION) {
			output(fd, "You can only use a potion on yourself.\n");
			return(0);
		}
	
		if ( crt_ptr->type == PLAYER && !pkill_allowed(ply_ptr, crt_ptr) ) {
                        return(0);
                }

		if (F_ISSET(crt_ptr, MUNKIL) && crt_ptr->type==MONSTER) {
			output(fd, "That is beyond your understanding.\nThe spell fizzles.\n");
			return(0);
		}
		if(F_ISSET(crt_ptr, PRMAGI) && crt_ptr->type==PLAYER) {
			output(fd, "Your magic is not powerful enough!\n");
			return(0);
		}
				
	}
	else {
		crt_ptr=0;
	}


	if(load_crt(cnjnum, &cnj_ptr) < 0) {
          	sprintf(g_buffer, "Conjure Error for %s\nNo creature (%d)", ply_ptr->name, cnjnum);
		elog_broad(g_buffer);
		sprintf(g_buffer, "You fail to conjure a creature from the void.\n");
               	output(ply_ptr->fd, g_buffer);
               	return(0);
        }

	sprintf(g_buffer, "A creature conjured by the power of %s.",
		ply_ptr->name);
	strcpy(cnj_ptr->description, g_buffer);
	cnj_ptr->parent_rom=rom_ptr;
	cnj_ptr->level=monlvl;
	cnj_ptr->hpcur=monster_guide[monlvl-1].hp;
	cnj_ptr->hpmax=monster_guide[monlvl-1].hp;
	cnj_ptr->mpcur=monlvl*3;
	cnj_ptr->mpmax=monlvl*3;
	cnj_ptr->armor=monster_guide[monlvl-1].armor;
	cnj_ptr->thaco=monster_guide[monlvl-1].thaco;
	cnj_ptr->experience=1;
	cnj_ptr->ndice=monster_guide[monlvl-1].ndice;
	cnj_ptr->sdice=monster_guide[monlvl-1].sdice;
	cnj_ptr->pdice=monster_guide[monlvl-1].pdice;
	cnj_ptr->strength=monlvl;
	cnj_ptr->dexterity=monlvl;
	cnj_ptr->constitution=monlvl;
	cnj_ptr->intelligence=monlvl;
	cnj_ptr->piety=monlvl;

	if(!F_ISSET(ply_ptr, PCHAOS)) {
		F_SET(cnj_ptr, MUNKIL);
	}

	/* don't have them follow or it could cause a crash */
	F_CLR(cnj_ptr, MFOLLO);

	F_SET(cnj_ptr, MMAGIC);
	S_SET(cnj_ptr, SVIGOR);
	S_SET(cnj_ptr, SMENDW);

        t = time(0);
		cnj_ptr->lasttime[LT_ATTCK].ltime =
		cnj_ptr->lasttime[LT_MSCAV].ltime =
		cnj_ptr->lasttime[LT_MWAND].ltime =
		cnj_ptr->lasttime[LT_CONJU].ltime = 
		ply_ptr->lasttime[LT_CONJU].ltime = t;
	cnj_ptr->lasttime[LT_CONJU].interval = 200L - (5L*monlvl); 
	ply_ptr->lasttime[LT_CONJU].interval = 380L + (5L*monlvl);

	if(cnj_ptr->dexterity < 20)
		cnj_ptr->lasttime[LT_ATTCK].interval = 3;
	else
		cnj_ptr->lasttime[LT_ATTCK].interval = 2;	

	F_SET(cnj_ptr, MCONJU);

	add_crt_rom(cnj_ptr, rom_ptr, 0);

/*#ifndef MOBS_ALWAYS_ACTIVE */
            add_active(cnj_ptr);
/* #endif */

	mprint(fd, "You conjure %1m.\n", 
		m1arg(cnj_ptr));
	broadcast_rom(fd, ply_ptr->rom_num, "%M conjures %1m.",
		m2args(ply_ptr, cnj_ptr));

	cnj_ptr->following = ply_ptr;
	pp = (ctag *)malloc(sizeof(ctag));
        if(!pp)
                merror("follow", FATAL);
        pp->crt = cnj_ptr;
        pp->next_tag = 0;

        if(!ply_ptr->first_fol)
                ply_ptr->first_fol = pp;
        else {
                pp->next_tag = ply_ptr->first_fol;
                ply_ptr->first_fol = pp;
        }
	
	if(mrand(1,100) < 15 + algmod + (monlvl - ply_ptr->intelligence) &&  ply_ptr->class <CARETAKER) {
        	output(fd, "Your conjure failed!\n");
        	broadcast_rom(fd, ply_ptr->rom_num, "%M's conjure failed!", m1arg(ply_ptr));
        	/* attack caster */
		del_conjured(cnj_ptr);
		F_CLR(cnj_ptr, MCONJU);
		F_CLR(cnj_ptr, MUNKIL);
        	add_enm_crt(ply_ptr->name, cnj_ptr);
		attack_crt(cnj_ptr, ply_ptr);
		if(crt_ptr) {
			add_enm_crt(cnj_ptr->name, crt_ptr);
        		attack_crt(cnj_ptr, crt_ptr);
		}

        	return(1);
	}
        else {
        	sprintf(g_buffer, "The %s starts following you.\n", cnj_ptr->name);
		output(fd, g_buffer);
	}

	if(crt_ptr) {
			add_enm_crt(crt_ptr->name, cnj_ptr);
			add_enm_crt(ply_ptr->name, crt_ptr);  /* conjurer also gets put on enemy list */
			end_enm_crt(ply_ptr->name, crt_ptr);
			attack_mon(cnj_ptr, crt_ptr);
	}


	return(1);
}

/**********************************************************************/
/*                          curse                                     */
/**********************************************************************/
        
int curse(creature *ply_ptr, cmd *cmnd, int how, object *obj_ptr )
{
    room        *rom_ptr;
    creature    *crt_ptr;
    int     fd, chance, mpcost;
            
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;
    mpcost = 12;
    
    if(!S_ISSET(ply_ptr, SDISPL) && how == CAST) {
        output(fd, "You don't know that spell.\n");
        return(0);
    }

    if(ply_ptr->mpcur < mpcost && how == CAST) {
        output(fd, "Not enough magic points.\n");
        return(0);
    }
    
    if(F_ISSET(ply_ptr, PINVIS)) {
        F_CLR(ply_ptr, PINVIS);
        output(fd, "Your invisibility fades.\n");
        broadcast_rom(fd, ply_ptr->rom_num, "%M fades into view.",
                  m1arg(ply_ptr));
    }
    
   /* curse on self */
    if(cmnd->num == 2) {
            
        output(fd, "You cast curse on yourself.\n");
        sprintf(g_buffer, "%%M casts curse on %sself.",
                 F_ISSET(ply_ptr, PMALES) ? "him":"her");
        broadcast_rom(fd, ply_ptr->rom_num, 
                  g_buffer, m1arg(ply_ptr));
        sprintf(g_buffer, "You no longer ");
        if(F_ISSET(ply_ptr, PBLESS)) {
            F_CLR(ply_ptr, PBLESS);
            output(fd, "You no longer feel holy.\n");
            broadcast_rom(fd, ply_ptr->rom_num, "%M no longer feels holy.", m1arg(ply_ptr));
        } 
        if(F_ISSET(ply_ptr, PPROTE)) {
            F_CLR(ply_ptr, PPROTE);
            output(fd, "You no longer feel watched.\n");
            broadcast_rom(fd, ply_ptr->rom_num, "%M no longer feels watched.", m1arg(ply_ptr));
        } 
        if(F_ISSET(ply_ptr, PLIGHT)) {
            F_CLR(ply_ptr, PLIGHT);
            output(fd, "You no longer have a magical light.\n");
            broadcast_rom(fd, ply_ptr->rom_num, "%M no longer has a magic light.", m1arg(ply_ptr));
        } 
        if(F_ISSET(ply_ptr, PPRAYD)) {
            F_CLR(ply_ptr, PPRAYD);
            output(fd, "Your no longer feel pious.\n");
            broadcast_rom(fd, ply_ptr->rom_num, "%M no longer feels pious.", m1arg(ply_ptr));
        } 
        if(F_ISSET(ply_ptr, PDMAGI  && (ply_ptr->class != ALCHEMIST))) {
            F_CLR(ply_ptr, PDMAGI);
            output(fd, "You can no longer detect magic.\n");
            broadcast_rom(fd, ply_ptr->rom_num, "%M no longer detects magic.", m1arg(ply_ptr));
        } 
        if(F_ISSET(ply_ptr, PDINVI)) {
            F_CLR(ply_ptr, PDINVI);
            output(fd, "You can no longer detect invisible.\n");
            broadcast_rom(fd, ply_ptr->rom_num, "%M no longer detects invisible.", m1arg(ply_ptr));
        } 

        if(how == CAST)
            ply_ptr->mpcur -= mpcost;
  
  }

  /* dispel on a monster or player */
  
  else {
            
        if(how == POTION) {
            output(fd, "You can only use a potion on yourself.\n");
            return(0);
        }
            
        cmnd->str[2][0] = up(cmnd->str[2][0]);     
        crt_ptr = find_crt_in_rom(ply_ptr, rom_ptr,
                   cmnd->str[2], cmnd->val[2], PLY_FIRST);
            
        if(!crt_ptr) {
            output(fd, "That's not here.\n");
            return(0);
        }
        
	if(F_ISSET(ply_ptr->parent_rom, RNOKIL)) {
		output(fd, "You can't cast that here.\n");
		return(0);
	}

        if(crt_ptr->type != PLAYER)  
            add_enm_crt(ply_ptr->name, crt_ptr);
        
        chance =  50 + (ply_ptr->level - crt_ptr->level)*5  +
        (bonus[(int)ply_ptr->intelligence] - bonus[(int)crt_ptr->intelligence])*5;
        chance += (ply_ptr->class == CLERIC) ? 20 : 0;
        chance += (ply_ptr->class == PALADIN) ? 20 : 0;
        chance += (ply_ptr->class == DRUID) ? 10 : 0;
        chance = MIN(85,chance);
        mprint(fd, "You cast curse on %m.\n", m1arg(crt_ptr));
        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                   "%M casts curse on %m.",
                   m2args(ply_ptr, crt_ptr));
        mprint(crt_ptr->fd, "%M casts curse on you.\n", m1arg(ply_ptr));

        if(F_ISSET(crt_ptr, PBLESS)) {
            if ((crt_ptr->class < DM) && (mrand(1,100) < chance)) {
                F_CLR(crt_ptr, PBLESS);
                output(crt_ptr->fd, "You no longer feel holy.\n");
                broadcast_rom(crt_ptr->fd, crt_ptr->rom_num, "%m is no longer holy.", m1arg(crt_ptr));
            }
        } 
        if(F_ISSET(crt_ptr, PPROTE)) {
            if ((crt_ptr->class < DM) && (mrand(1,100) < chance)) {
                F_CLR(crt_ptr, PPROTE);
                output(crt_ptr->fd, "You no longer feel watched.\n");
                broadcast_rom(crt_ptr->fd, crt_ptr->rom_num, "%m is no longer watched.", m1arg(crt_ptr));
          }
        } 
        if(F_ISSET(crt_ptr, PLIGHT)) {
            if ((crt_ptr->class < DM) && (mrand(1,100) < chance)) {
                F_CLR(crt_ptr, PLIGHT);
                output(crt_ptr->fd, "You no longer have a magical light.\n");
                broadcast_rom(crt_ptr->fd, crt_ptr->rom_num, "%m no longer has a magical light.", m1arg(crt_ptr));
            }
        } 
        if(F_ISSET(crt_ptr, PPRAYD)) {
            if ((crt_ptr->class < DM) && (mrand(1,100) < chance)) {
                F_CLR(crt_ptr, PPRAYD);
                output(crt_ptr->fd, "Your no longer feel pious.\n");
                broadcast_rom(crt_ptr->fd, crt_ptr->rom_num, "%m is no longer pious.", m1arg(crt_ptr));
            }
        } 
        if(F_ISSET(crt_ptr, PDMAGI)) {
            if ((crt_ptr->class < DM) && (mrand(1,100) < chance)) {
                if (crt_ptr->class != ALCHEMIST) {
                   F_CLR(crt_ptr, PDMAGI);
                   output(crt_ptr->fd, "You can no longer detect magic.\n");
                   broadcast_rom(crt_ptr->fd, crt_ptr->rom_num, "%m no longer detects magic.", m1arg(crt_ptr));
                }
            }
        } 
        if(F_ISSET(crt_ptr, PDINVI)) {
            if ((crt_ptr->class < DM) && (mrand(1,100) < chance)) {
                F_CLR(crt_ptr, PDINVI); 
                output(crt_ptr->fd, "You can no longer detect invisible.\n");
                broadcast_rom(crt_ptr->fd, crt_ptr->rom_num, "%m no longer detects invisible.", m1arg(crt_ptr));
            }
        }

        if(how == CAST)
            ply_ptr->mpcur -= mpcost;
 
    }
        
    return(1);
}
/*                          dispel                                    */
/**********************************************************************/
/* The dispel spell removed magical ability spells.  Success against  */
/* an opponent is dependent on the relative levels of the caster and  */
/* the opponent.                                                      */
        
int dispel(creature *ply_ptr, cmd *cmnd, int how, object *obj_ptr )
{
    room        *rom_ptr;
    creature    *crt_ptr;
    int     fd, chance, mpcost;
            
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;
    mpcost = 15;
    
    if(!S_ISSET(ply_ptr, SDISPL) && how == CAST) 
    {
        output(fd, "You don't know that spell.\n");
        return(0);
    }

    if(ply_ptr->mpcur < mpcost && how == CAST) {
        output(fd, "Not enough magic points.\n");
        return(0);
    }
    
    if(F_ISSET(ply_ptr, PINVIS)) {
        F_CLR(ply_ptr, PINVIS);
        output(fd, "Your invisibility fades.\n");
        broadcast_rom(fd, ply_ptr->rom_num, "%M fades into view.",
                  m1arg(ply_ptr));
    }
    
   /* dispel on self */
    if(cmnd->num == 2) {
            
        if(F_ISSET(ply_ptr, PRMAGI)) {
            output(fd, "Your magic shield resists the dispel spell.\n");
            sprintf(g_buffer, "%%M casts dispel on %sself.",
                      F_ISSET(ply_ptr, PMALES) ? "him":"her");
            broadcast_rom(fd, ply_ptr->rom_num,
                      g_buffer, m1arg(ply_ptr));
            if(how == CAST)
                ply_ptr->mpcur -= mpcost;
            return(0);
        }
        output(fd, "You cast an dispel spell on yourself.\n");
        sprintf(g_buffer, "%%M casts dispel on %sself.",
                 F_ISSET(ply_ptr, PMALES) ? "him":"her");
        broadcast_rom(fd, ply_ptr->rom_num, 
                  g_buffer, m1arg(ply_ptr));   
        if(F_ISSET(ply_ptr, PLEVIT)) {
            F_CLR(ply_ptr, PLEVIT);
            output(fd, "You are no longer levitated.\n");
            broadcast_rom(fd, ply_ptr->rom_num, "%M stops levitating.", m1arg(ply_ptr));
        } 
        if(F_ISSET(ply_ptr, PFLYSP)) {
            F_CLR(ply_ptr, PFLYSP);
            output(fd, "You are no longer flying.\n");
            broadcast_rom(fd, ply_ptr->rom_num, "%M stops flying.", m1arg(ply_ptr));
        } 
        if(F_ISSET(ply_ptr, PRCOLD)) {
            F_CLR(ply_ptr, PRCOLD);
            output(fd, "You no longer resist cold.\n");
            broadcast_rom(fd, ply_ptr->rom_num, "%M no longer resists cold.", m1arg(ply_ptr));
        } 
        if(F_ISSET(ply_ptr, PBRWAT)) {
            F_CLR(ply_ptr, PBRWAT);
            output(fd, "You no longer breathe water.\n");
            broadcast_rom(fd, ply_ptr->rom_num, "%M stops breathing water.", m1arg(ply_ptr));
        } 
        if(F_ISSET(ply_ptr, PRFIRE)) {
            F_CLR(ply_ptr, PRFIRE);
            output(fd, "You no longer resist fire.\n");
            broadcast_rom(fd, ply_ptr->rom_num, "%M no longer resists cold.", m1arg(ply_ptr));
        } 
        if(F_ISSET(ply_ptr, PSSHLD)) {
            F_CLR(ply_ptr, PSSHLD);
            output(fd, "You no longer resist earth.\n");
            broadcast_rom(fd, ply_ptr->rom_num, "%M no longer resists earth.", m1arg(ply_ptr));
        } 
        if(how == CAST)
            ply_ptr->mpcur -= mpcost;
  
  }

  /* dispel on a monster or player */
  
  else {
            
        if(how == POTION) {
            output(fd, "You can only use a potion on yourself.\n");
            return(0);
        }
            
        cmnd->str[2][0] = up(cmnd->str[2][0]);     
        crt_ptr = find_crt_in_rom(ply_ptr, rom_ptr,
                   cmnd->str[2], cmnd->val[2], PLY_FIRST);
            
        if(!crt_ptr) {
            output(fd, "That's not here.\n");
            return(0);
        }
        
        if(crt_ptr->type != PLAYER)  
            add_enm_crt(ply_ptr->name, crt_ptr);
        
        chance =  50 + (ply_ptr->level - crt_ptr->level)*5  +
        (bonus[(int)ply_ptr->intelligence] - bonus[(int)crt_ptr->intelligence])*5;
        chance += (ply_ptr->class == MAGE) ? 5 : 0;
        chance += (ply_ptr->class == ALCHEMIST) ? 5 : 0;
        chance += (ply_ptr->class == DRUID) ? 20 : 0;
        chance = MIN(85,chance);
        mprint(fd, "You cast dispel on %m.\n", m1arg(crt_ptr));
        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                   "%M casts dispel on %m.",
                   m2args(ply_ptr, crt_ptr));
        mprint(crt_ptr->fd, "%M casts dispel on you.\n", m1arg(ply_ptr));

        if(F_ISSET(crt_ptr, PLEVIT)) {
            if ((crt_ptr->class < DM) && (mrand(1,100) < chance)) {
                F_CLR(crt_ptr, PLEVIT);
                output(crt_ptr->fd, "You no longer levitate.\n");
                broadcast_rom(crt_ptr->fd, crt_ptr->rom_num, "%m stops levitating.", m1arg(crt_ptr));
            }
        } 
        if(F_ISSET(crt_ptr, PRFIRE)) {
            if ((crt_ptr->class < DM) && (mrand(1,100) < chance)) {
                F_CLR(crt_ptr, PRFIRE);
                output(crt_ptr->fd, "You no longer resist fire.\n");
                broadcast_rom(crt_ptr->fd, crt_ptr->rom_num, "%m no longer resists fire.", m1arg(crt_ptr));
            }
        } 
        if(F_ISSET(crt_ptr, PFLYSP)) {
            if ((crt_ptr->class < DM) && (mrand(1,100) < chance)) {
                F_CLR(crt_ptr, PFLYSP); 
                output(crt_ptr->fd, "You no longer fly.\n");
                broadcast_rom(crt_ptr->fd, crt_ptr->rom_num, "%m stops flying.", m1arg(crt_ptr));
            }
        }
        if(F_ISSET(crt_ptr, PRCOLD)) {
            if ((crt_ptr->class < DM) && (mrand(1,100) < chance)) {
                F_CLR(crt_ptr, PRCOLD); 
                output(crt_ptr->fd, "You no longer resist cold.\n");
                broadcast_rom(crt_ptr->fd, crt_ptr->rom_num, "%m no longer resists cold.", m1arg(crt_ptr));

           }  
        }
        if(F_ISSET(crt_ptr, PBRWAT)) {
            if ((crt_ptr->class < DM) && (mrand(1,100) < chance)) {
                F_CLR(crt_ptr, PBRWAT); 
                output(crt_ptr->fd, "You no longer breathe water.\n");
                broadcast_rom(crt_ptr->fd, crt_ptr->rom_num, "%m stop breathing water.", m1arg(crt_ptr));
            }
        }
        if(F_ISSET(crt_ptr, PSSHLD)) {
            if ((crt_ptr->class < DM) && (mrand(1,100) < chance)) {
                F_CLR(crt_ptr, PSSHLD); 
                output(crt_ptr->fd, "You no longer resist earth.\n");
                 broadcast_rom(crt_ptr->fd, crt_ptr->rom_num, "%m no longer resists earth.", m1arg(crt_ptr));
           }
        }
       if(F_ISSET(crt_ptr, PRMAGI)) {
            if ((crt_ptr->class < DM) && (mrand(1,100) < chance)) {
                F_CLR(crt_ptr, PRMAGI);
                output(crt_ptr->fd, "You no longer resist magic.\n");
                broadcast_rom(crt_ptr->fd, crt_ptr->rom_num, "%m no longer resists magic.", m1arg(crt_ptr));
            }
        }

        if(how == CAST)
            ply_ptr->mpcur -= mpcost;
 
    }
        
    return(1);
}

