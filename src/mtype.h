/*
 * MTYPE.H:
 *
 *      #defines required by the rest of the program
 *
 *      Copyright (C) 1991, 1992, 1993 Brooke Paul
 *
 * $Id: mtype.h,v 6.18 2001/07/22 20:05:52 develop Exp $
 *
 * $Log: mtype.h,v $
 * Revision 6.18  2001/07/22 20:05:52  develop
 * gold theft changes
 *
 * Revision 6.17  2001/07/22 19:03:06  develop
 * first run at alllowing thieves to steal gold from other players
 *
 * Revision 6.16  2001/07/20 12:10:00  develop
 * added pawnshops
 *
 * Revision 6.15  2001/07/17 19:28:44  develop
 * *** empty log message ***
 *
 * Revision 6.14  2001/04/12 05:00:57  develop
 * STRMOR <-> SEQUAK (named wrong)
 *
 * Revision 6.13  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

#include <stdlib.h>
#include <string.h>


/* for find_crt_in_rom() */
#define MON_FIRST		0		/* looks for a monster first then player */
#define PLY_FIRST		1		/* looks for a player first then monster */

/* I/O buffer sizes */
#define IBUFSIZE        1024
#define OBUFSIZE        8192
 
 
#define MAX_TOKEN_SIZE	50        
#define COMMANDMAX      5



/* how often (in seconds) players get saved */
#define SAVEINTERVAL    1200


/* Command status returns */
#define DISCONNECT      1
#define PROMPT          2
#define DOPROMPT        3

/* Warring Kingdoms */
#define AT_WAR 1                        /* princes at war =1 / 0=peace */
#define REWARD 1000                     /* base value for pledge and rescind */
#define MAXALVL 25                      /*max size of exp level array */

/* shop types */
#define ARMORY		0
#define GENERALSTORE	1
#define EMPORIUM	2
#define OUTFITTER	3
#define PAWNSHOP	4
#define MARKETPLACE	5

/* the object created when you use the 'defecate' command */
#define SHIT 	349

/* a generic storage item (i.e. chest) that is added to a storeroom */
#define CHEST 	499

/* sales tax and interest rate for player/player commerce */
#define TAXRATE .08
#define INTRATE	.02

/* Spell flags */
#define SVIGOR          0       /* vigor */
#define SHURTS          1       /* hurt */
#define SLIGHT          2       /* light */
#define SCUREP          3       /* curepoison */
#define SBLESS          4       /* bless */
#define SPROTE          5       /* protection */
#define SFIREB          6       /* fireball */
#define SINVIS          7       /* invisibility */
#define SRESTO          8       /* restore */
#define SDINVI          9       /* detect-invisibility */
#define SDMAGI          10      /* detect-magic */
#define STELEP          11      /* teleport */
#define SBEFUD          12      /* befuddle */
#define SLGHTN          13      /* lightning */
#define SICEBL          14      /* iceblade */
#define SENCHA          15      /* enchant */
#define SRECAL          16      /* word-of-recall */
#define SSUMMO          17      /* summon */
#define SMENDW          18      /* mend-wounds */
#define SFHEAL          19      /* heal */
#define STRACK          20      /* track */
#define SLEVIT          21      /* levitation */
#define SRFIRE          22      /* resist-fire */
#define SFLYSP          23      /* fly */
#define SRMAGI          24      /* resist-magic */
#define SSHOCK          25      /* shockbolt */
#define SRUMBL          26      /* rumble */
#define SBURNS          27      /* burn */
#define SBLIST          28      /* blister */
#define SDUSTG          29      /* dustgust */
#define SWBOLT          30      /* waterbolt */
#define SCRUSH          31      /* stonecrush */
#define SENGUL          32      /* engulf */
#define SBURST          33      /* burstflame */
#define SSTEAM          34      /* steamblast */
#define SSHATT          35      /* shatterstone */
#define SIMMOL          36      /* immolate */
#define SBLOOD          37      /* bloodboil */
#define STHUND          38      /* thunderbolt */
#define SEQUAK          39      /* earthquake */
#define SFLFIL          40      /* flamefill */
#define SKNOWA          41      /* know-alignment */
#define SREMOV          42      /* remove-curse */
#define SRCOLD          43      /* resist-cold */
#define SBRWAT          44      /* breathe water */
#define SSSHLD          45      /* stone shield */
#define SLOCAT          46      /* locate player */
#define SDREXP          47      /* drain energy (exp) */
#define SRMDIS          48      /* cure disease */
#define SRMBLD          49      /* cure blindess */
#define SFEARS          50      /* fear */
#define SRVIGO		51	/* room vigor */
#define STRANO		52	/* item transport */
#define SBLIND		53	/* cause blindness */
#define SSILNC		54	/* cause silence */
#define SFORTU		55	/* fortune */
#define SCONJU		56	/* conjure */
#define SCURSE		57	/* curse */
#define SDISPL		58	/* dispel */
#define STORNA		59	/* tornado */
#define SINCIN		60	/* incinerate */
#define STRMOR		61	/* tremor */
#define SFLOOD		62	/* flood */


/*Trap types */
#define TRAP_PIT        1       /* Pit trap */
#define TRAP_DART       2       /* Poison dart trap */
#define TRAP_BLOCK      3       /* Falling block */
#define TRAP_MPDAM      4       /* Mp damaging trap */
#define TRAP_RMSPL      5       /* Spell loss trap */
#define TRAP_NAKED      6       /* player loses all items */
#define TRAP_ALARM      7       /* monster alarm trap */

/* Spell casting types */
#define CAST            0


/* specials */
#define SP_MAPSC        	1       /* Map or scroll */
#define SP_COMBO        	2       /* Combination lock */
#define SP_HERB_HEAL          	3	/* Various Herbs */
#define SP_HERB_HARM          	4
#define SP_HERB_POISON        	5
#define SP_HERB_DISEASE       	6
#define SP_HERB_CURE_POISON   	7
#define SP_HERB_CURE_DISEASE  	8

/* obj_str and crt_str flags */
#define CAP             1
#define INV             2
#define MAG             4
#define ISIM			8
#define ISCT			16
#define ISDM			32			


#define RETURN(a,b,c)		Ply[a].io->fn = b; Ply[a].io->fnparam = c; return;
#define RETURNV(a,b,c,d)	Ply[a].io->fn = b; Ply[a].io->fnparam = c; return(d);

/* used on some platforms as data type */
/*#define BOOL(a)         ((a) ? 1 : 0) */

#define EQUAL(a,b)      ((a) && (b) && \
                         (!strncmp((a)->name,(b),strlen(b)) || \
                          !strncmp((a)->key[0],(b),strlen(b)) || \
                          !strncmp((a)->key[1],(b),strlen(b)) || \
                          !strncmp((a)->key[2],(b),strlen(b))))       

#define LT(a,b)         ((a)->lasttime[(b)].ltime + (a)->lasttime[(b)].interval)

#define mdice(a)        (dice((a)->ndice,(a)->sdice,(a)->pdice))


