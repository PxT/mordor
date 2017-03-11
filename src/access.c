/*
 * ACCESS.C:
 *
 *      This file contains the routines necessary to access arrays
 *
 *      Copyright (C) 1991, 1992, 1993 Brooke Paul
 *      Copyright (C) 1998 John P. Freeman
 * $Id: access.c,v 6.14 2001/04/12 05:00:57 develop Exp $
 *
 * $Log: access.c,v $
 * Revision 6.14  2001/04/12 05:00:57  develop
 * readded room_damage spells to spellist
 *
 * Revision 6.13  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 *
 */

#include "../include/mordb.h"

#include "mextern.h"


int		Spy[PMAX];


static struct {
	char 	*splstr;
	int	splno;
	SPELLFN splfn;
} spllist[] = {
	{ "vigor", SVIGOR, vigor },
	{ "hurt", SHURTS, offensive_spell },
	{ "light", SLIGHT, light },
	{ "curepoison", SCUREP, curepoison },
	{ "bless", SBLESS, bless },
	{ "protection", SPROTE, protection },
	{ "fireball", SFIREB, offensive_spell },
	{ "invisibility", SINVIS, invisibility },
	{ "restore", SRESTO, restore },
	{ "detect-invisible", SDINVI, detectinvis },
	{ "detect-magic", SDMAGI, detectmagic },
	{ "teleport", STELEP, teleport },
	{ "stun", SBEFUD, befuddle },
	{ "lightning", SLGHTN, offensive_spell },
	{ "iceblade", SICEBL, offensive_spell },
	{ "enchant", SENCHA, enchant },
	{ "word-of-recall", SRECAL, recall },
	{ "summon", SSUMMO, summon },
	{ "mend-wounds", SMENDW, mend },
	{ "heal", SFHEAL, heal },
	{ "track", STRACK, magictrack },
	{ "levitate", SLEVIT, levitate },
	{ "resist-fire", SRFIRE, resist_fire },
	{ "fly", SFLYSP, fly },
	{ "resist-magic", SRMAGI, resist_magic },
	{ "shockbolt", SSHOCK, offensive_spell },
	{ "rumble", SRUMBL, offensive_spell },
	{ "burn", SBURNS, offensive_spell },
	{ "blister", SBLIST, offensive_spell },
	{ "dustgust", SDUSTG, offensive_spell },
	{ "waterbolt", SWBOLT, offensive_spell },
	{ "crush", SCRUSH, offensive_spell },
	{ "shatterstone", SENGUL, offensive_spell },
	{ "burstflame", SBURST, offensive_spell },
	{ "steamblast", SSTEAM, offensive_spell },
	{ "engulf", SSHATT, offensive_spell },
	{ "immolate", SIMMOL, offensive_spell },
	{ "bloodboil", SBLOOD, offensive_spell },
	{ "thunderbolt", STHUND, offensive_spell },
	{ "earthquake", SEQUAK, offensive_spell },
	{ "flamefill", SFLFIL, offensive_spell },
	{ "know-aura", SKNOWA, know_alignment },
	{ "remove-curse", SREMOV, remove_curse },
	{ "resist-cold", SRCOLD, resist_cold },
	{ "breathe-water", SBRWAT, breathe_water },
	{ "resist-earth", SSSHLD, earth_shield },
	{ "clairvoyance", SLOCAT, locate_player },
	{ "drain", SDREXP, drain_exp },
	{ "remove-disease", SRMDIS, rm_disease },
	{ "cure-blindness", SRMBLD, rm_blind },
	{ "fear", SFEARS, fear }, 
	{ "room-vigor", SRVIGO, room_vigor }, 
	{ "transport", STRANO, object_send },
	{ "blind", SBLIND, blind },
	{ "silence", SSILNC, silence },
	{ "fortune", SFORTU, fortune },
	{ "conjure", SCONJU, conjure },
	{ "curse", SCURSE, curse },
	{ "dispel", SDISPL, dispel },
	{ "tornado", STORNA, room_damage },
	{ "incinerate", SINCIN, room_damage },
	{ "tremor", STRMOR, room_damage },
	{ "flood", SFLOOD, room_damage },
	{ "@", -1,0 }
};

static int spllist_size = sizeof(spllist)/sizeof(*spllist);




static char number[][10] = {
	"zero",
	"one",
	"two",
	"three",
	"four",
	"five",
	"six",
	"seven",
	"eight",
	"nine",
	"ten",
	"eleven",
	"twelve",
	"thirteen",
	"fourteen",
	"fifteen",
	"sixteen",
	"seventeen",
	"eighteen",
	"nineteen",
	"twenty"
};



static long quest_exp[] = {
	120, 500, 1000, 1000, 1000, 1000, 125, 125,
	1000, 1000, 125, 125, 125, 1000, 2500, 2500,
	500, 5, 5, 5, 125, 125, 125, 125,
	125, 125, 125, 125, 125, 125, 125, 125,
	125, 125, 125, 125, 125, 125, 125, 125,
	125, 125, 125, 125, 125, 125, 125, 125,
	125, 125, 125, 125, 125, 125, 125, 125,
	125, 125, 125, 125, 125, 125, 125, 125,
	125, 125, 125, 125, 125, 125, 125, 125,
	125, 125, 125, 125, 125, 125, 125, 125,
	125, 125, 125, 125, 125, 125, 125, 125,
	125, 125, 125, 125, 125, 125, 125, 125,
	125, 125, 125, 125, 125, 125, 125, 125,
	125, 125, 125, 125, 125, 125, 125, 125,
	125, 125, 125, 125, 125, 125, 125, 125,
	125, 125, 125, 125, 125, 125, 125, 125
};











/**********************************************************************/
/* get_spell_name()												  */
/**********************************************************************/
char *get_spell_name( int nIndex )
{
	/* do bounds checking */
	ASSERTLOG( nIndex >= 0 );
	ASSERTLOG(nIndex < spllist_size );

	nIndex =   MAX( 0, MIN(nIndex, spllist_size ) );

	return( spllist[nIndex].splstr );
}


/**********************************************************************/
/* get_spell_num()												  */
/**********************************************************************/
int get_spell_num( int nIndex )
{
	/* do bounds checking */
	ASSERTLOG( nIndex >= 0 );
	ASSERTLOG(nIndex < spllist_size );

	nIndex =   MAX( 0, MIN(nIndex, spllist_size ) );

	return( spllist[nIndex].splno );
}



/**********************************************************************/
/* get_spell_function()												  */
/**********************************************************************/
SPELLFN get_spell_function( int nIndex )
{
	/* do bounds checking */
	ASSERTLOG( nIndex >= 0 );
	ASSERTLOG(nIndex < spllist_size );

	nIndex =   MAX( 0, MIN(nIndex, spllist_size ) );

	return( spllist[nIndex].splfn );
}


/**********************************************************************/
/* get_spell_list_size()					      */
/**********************************************************************/
int get_spell_list_size()
{
	return( spllist_size );
}



/**********************************************************************/
/* int_to_test()	  												  */
/**********************************************************************/
char *int_to_text( int nNumber )
{
	static	char strNum[10];
	char *strReturn;

	/* check for array bounds */
	if ( nNumber < 21 && nNumber >= 0)
	{
		strReturn = number[nNumber];
	}
	else
	{
		sprintf( strNum, "%d", nNumber );
		strReturn = strNum;
	}

	return(strReturn);
}



/**********************************************************************/
/* get_quest_exp()	  												  */
/**********************************************************************/
long get_quest_exp( int nQuest )
{
	int nMax = sizeof(quest_exp)/sizeof(long);

	/* quests are 1 based and this array is zero based */
	/* so subtract one first */
	nQuest--;

	nQuest = MAX(0, MIN(nQuest, nMax - 1 ) );

	return( quest_exp[nQuest ]);
}



/**********************************************************************/
/* get_spy()	  												  */
/**********************************************************************/
int get_spy( int id )
{
	ASSERTLOG(id >= 0);
	ASSERTLOG(id < PMAX );

	id = MAX(0, MIN(id, PMAX - 1));

	return( Spy[id] );
}


/**********************************************************************/
/* set_spy()	  												  */
/**********************************************************************/
void set_spy( int id, int fd )
{
	ASSERTLOG(id >= 0);
	ASSERTLOG(id < PMAX );

	id = MAX(0, MIN(id, PMAX - 1));

	Spy[id ] = fd;

	return;
}

/**********************************************************************/
/* clear_spy()	  												  */
/**********************************************************************/
void clear_spy()
{
	int	i;

	for( i = 0; i < PMAX; i++) 
		Spy[i] = -1;

	return;
}
