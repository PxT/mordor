/*
 * Copyright (C) 1993 Brooke Paul
 * $Id: mordb.h,v 1.15 2001/07/25 02:48:55 develop Exp $
 *
 * $Log: mordb.h,v $
 * Revision 1.15  2001/07/25 02:48:55  develop
 * commented out LONG_MAX due to system header conflicts
 *
 * Revision 1.15  2001/07/25 01:13:13  develop
 * fixed OTHEFT
 *
 * Revision 1.14  2001/07/24 01:24:50  develop
 * added OTHEFT
 *
 * Revision 1.13  2001/07/22 22:29:40  develop
 * added LONG_MAX
 *
 * Revision 1.12  2001/07/22 01:22:55  develop
 * added OPVALU
 *
 * Revision 1.11  2001/07/21 17:29:57  develop
 * added ONOSEL
 *
 * Revision 1.10  2001/07/14 19:53:35  develop
 * added RPOWND and XUNIQP
 *
 * Revision 1.9  2001/07/04 02:44:08  develop
 * increased CMAX to 2000
 * er...3000 that is
 *
 * Revision 1.8  2001/07/01 14:48:21  develop
 * changed MGROUP to MTEAMM and added MTEAML
 *
 * Revision 1.7  2001/07/01 04:48:16  develop
 * added MGROUP
 *
 * Revision 1.6  2001/06/06 19:31:53  develop
 * Added conjure
 *
 * Revision 1.5  2001/03/09 05:10:09  develop
 * potential fix for memory overflow when saving players
 *
 * Revision 1.4  2001/03/08 16:05:42  develop
 * *** empty log message ***
 *
 */
#ifndef __MORDB_H_
#define __MORDB_H_

#include "../include/port.h"

/* comment this out to go back to mobs being active only */
/* when a player is in the room */
#define MOBS_ALWAYS_ACTIVE

/* memory limits */
#define RMAX            15000    /* Max number of these allowed to be created */
#define CMAX            3000
#define OMAX            2000
#define PMAX            256

#define RQMAX           600     /* Max number of these allowed in memory */
#define CQMAX           200     /* at any one time                       */
#define OQMAX           200

// #define LONG_MAX	500000	/* maximum amount that a long value will be */


/* creature types */
#define PLAYER          0
#define MONSTER         1
#define NPC             2


#define RACE_COUNT	14


/* Save flags */
#define PERMONLY        1
#define ALLITEMS        0

/* Monster and object files sizes (in terms of monsters or objects) */
#define MFILESIZE       100
#define OFILESIZE       100


/* Maximum number of items that can be worn/readied */
#define MAXWEAR         20

/* Wear locations */
#define BODY            1
#define ARMS            2
#define LEGS            3
#define NECK            4
#define NECK1           4
#define NECK2           5
#define HANDS           6
#define HEAD            7
#define FEET            8
#define FINGER          9
#define FINGER1         9
#define FINGER2         10
#define FINGER3         11
#define FINGER4         12 
#define FINGER5         13
#define FINGER6         14
#define FINGER7         15
#define FINGER8         16
#define HELD            17
#define SHIELD          18
#define FACE            19
#define WIELD           20


/* Room flags */
#define RSHOPP          0       /* Shoppe */
#define RDUMPR          1       /* Dump */
#define RPAWNS          2       /* Pawn Shoppe */
#define RTRAIN          3       /* Training class bits (3-6) */
#define RREPAI          7       /* Repair Shoppe */
#define RDARKR          8       /* Room is dark always */
#define RDARKN          9       /* Room is dark at night */
#define RPOSTO          10      /* Post office */
#define RNOKIL          11      /* Safe room, no playerkilling */
#define RNOTEL          12      /* Cannot teleport to this room */
#define RHEALR          13      /* Heal faster in this room */
#define RONEPL          14      /* 1-player only inside */
#define RTWOPL          15      /* 2-players only inside */
#define RTHREE          16      /* 3-players only inside */
#define RNOMAG          17      /* No magic allowed in room */
#define RPTRAK          18      /* Permanent tracks in room */
#define REARTH          19      /* Earth realm */
#define RWINDR          20      /* Wind realm */
#define RFIRER          21      /* Fire realm */
#define RWATER          22      /* Water realm */
#define RPLWAN          23      /* Player-dependent monster wanders */
#define RPHARM          24      /* player harming room */
#define RPPOIS          25      /* player poison room */
#define RPMPDR          26      /* player mp drain room */
#define RPBEFU          27      /* player befuddle room */
#define RNOLEA          28      /* player can not be summon out */
#define RPLDGK          29      /* player can pledge in room */
#define RRSCND          30      /* player can rescind in room */
#define RNOPOT          31      /* No potion room */
#define RPMEXT          32      /* Player magic spell extend */
#define RNOLOG          33      /* No player login */
#define RELECT          34      /* Election Booth */ 
#define RNDOOR		35	/* Indoors */
#define RISH2O		36	/* Room is a water source */
#define RJAILR		37	/* Jail room */
#define RBANKR		38	/* Bank room */
#define RGILDA         	39      /* GUILD   100 Circle    010 Masters 001 Brotherhood */      
#define RGILDB         	40      /* FLAGS   110 Clan      101 Company 011  Guard */ 
#define RGILDC         	41      /*         111 Champions 000 Council */
#define RPOWND		42	/* player owned room */



/* Player flags */
#define PBLESS          0       /* Bless spell */
#define PHIDDN          1       /* Hidden */
#define PINVIS          2       /* Invisibility */
#define PNOBRD          3       /* Don't show broadcasts */
#define PNOLDS          4       /* Don't show long description */
#define PNOSDS          5       /* Don't show short description */
#define PNORNM          6       /* Don't show room name */
#define PNOEXT          7       /* Don't show exits */
#define PPROTE          8       /* Protection spell */
#define PNOAAT          9       /* no auto attack for players */
#define PDMINV          10      /* DM Invisibility */
#define PNOCMP          11      /* Non-compact */
#define PMALES          12      /* Sex == male */
#define PHEXLN          13      /* Hexline */
#define PWIMPY          14      /* Wimpy mode */
#define PEAVES          15      /* Eavesdropping mode */
#define PPOISN          16      /* Poisoned */
#define PLIGHT          17      /* Light spell cast */
#define PPROMP          18      /* Display status prompt */
#define PHASTE          19      /* Haste flag (for rangers) */
#define PDMAGI          20      /* Detect magic */
#define PDINVI          21      /* Detect invisible */
#define PPRAYD          22      /* Prayer activated */
#define PROBOT          23      /* Robot mode */
#define PPREPA          24      /* Prepared for trap */
#define PLEVIT          25      /* Levitation */
#define PANSIC          26      /* Ansi Color */
#define PSPYON          27      /* Spying on someone */
#define PCHAOS          28      /* Chaotic/!Lawful */
#define PREADI          29      /* Reading a file */
#define PRFIRE          30      /* Resisting fire */
#define PFLYSP          31      /* Flying */
#define PRMAGI          32      /* Resist magic */
#define PKNOWA          33      /* Know alignment */
#define PNOSUM          34      /* Nosummon flag */
#define PIGNOR          35      /* Ignore all send */ 
#define PRCOLD          36      /* Resist-cold flag */
#define PBRWAT          37      /* Breathe wateR flag */
#define PSSHLD          38      /* Resist-cold flag */
#define PPLDGK          39      /* player pledge to a prince */
/* The PKNGDM is no longer used */
#define PKNGDM          40      /* Pledge to prince 0/1 */
#define PDISEA          41      /* Player is diseased */
#define PBLIND          42      /* Player is blind */
/* Pfears no longer used */
#define PFEARS		43	/* Player is fearful */
#define	PSILNC		44	/* Player has been silenced */
#define PCHARM		45	/* Player is charmed */
#define PNLOGN		46	/* No login messages sent */
#define PLECHO		47	/* Echo comms to sender */
#define PSECOK		48	/* Player has passed security check */
#define PAUTHD		49	/* No-port that has been authorized */
#define	PALIAS		50	/* DM is aliasing */
#define PBESRK		51	/* Player is preparing an object */
#define PNSUSN		52	/* Player needs sustenance */
#define PIGCLA		53	/* Player is ignoring class comms. */
#define PIGGLD          54      /* Player is ignoring guild comms. */
#define PDMOWN          55      /* player is a trouble maker */
#define PAFK            56      /* player is afk */
#define PBRIEF          57      /* player is using brief combat mode*/
#define PBARKS          58      /* player has barkskin*/
#define PGILDA          59      /*   GUILD   100 Circle     100 Masters  001 Brotherhood */
#define PGILDB          60      /*   FLAGS   110 Clan       101 Company  011 Guard */
#define PGILDC          61      /*           111 Champions  000 Council  */
#define PSOUND		62	/* MSP use */
#define PSAVES		63	/* for internal use */

/* Monster flags */
#define MPERMT          0       /* Permanent monster */
#define MHIDDN          1       /* Hidden */
#define MINVIS          2       /* Invisible */
#define MTOMEN          3       /* Man to men on plural */
#define MDROPS          4       /* Don't add s on plural */
#define MNOPRE          5       /* No prefix */
#define MAGGRE          6       /* Aggressive */
#define MGUARD          7       /* Guards treasure */
#define MBLOCK          8       /* Blocks exits */
#define MFOLLO          9       /* Monster follows attacker */
#define MFLEER          10      /* Monster flees */
#define MSCAVE          11      /* Monster is a scavenger */
#define MMALES          12      /* Sex == male */
#define MPOISS          13      /* Poisoner */
#define MUNDED          14      /* Undead */
#define MUNSTL          15      /* Cannot be stolen from */
#define MPOISN          16      /* Poisoned */
#define MMAGIC          17      /* Can cast spells */
#define MHASSC          18      /* Has already scavenged something */
#define MBRETH          19      /* Breath weapon */
#define MMGONL          20      /* Can only be harmed by magic */
#define MDINVI          21      /* Detect invisibility */
#define MENONL          22      /* Can only be harmed by magic/ench.weapon */
#define MTALKS          23      /* Monster can talk interactively */
#define MUNKIL          24      /* Monster cannot be harmed */
#define MNRGLD          25      /* Monster has fixed amt of gold */
#define MTLKAG          26      /* Becomes aggressive after talking */
#define MRMAGI          27      /* Resist magic */
#define MBRWP1          28      /* MBRWP1 & MBRWP2 type of breath */
#define MBRWP2          29      /* 00 =fire, 01= ice, 10 =gas, 11= acid */
#define MENEDR          30      /* Energy (exp) drain */
#define MGUILD          31      /* monster belongs to kingdom 0/1 */
#define MPLDGK          32      /* Players can pledge to monster */
#define MRSCND          33      /* Players can rescind to monster */
#define MDISEA          34      /* Monster causes disease */
#define MDISIT          35      /* Monster can dissolve items */
#define MPURIT          36      /* Player can purchase from monster */
#define MTRADE          37      /* Monster will give items */
#define MPGUAR          38      /* Passive exit guard */
#define MGAGGR          39      /* Monster aggro to good players */
#define MEAGGR          40      /* Monster aggro to evil players */
#define MDEATH          41      /* Monster has additon desc after death */
#define MMAGIO		42	/* Monster cast magic percent flag (prof 1) */
#define MRBEFD		43	/* Monster resists stun only */
#define MNOCIR		44	/* Monster cannot be circled */
#define MBLNDR		45	/* Monster blinds */
#define MDMFOL		46	/* Monster will follow DM */
/* Mfears no longer used */
#define MFEARS		47	/* Monster is fearful */
#define MSILNC		48	/* Monster is silenced */
#define MBLIND		49	/* Monster is blind */
#define MCHARM		50	/* Monster is charmed */
#define MMOBIL		51	/* Mobile monster */
#define MROBOT		52	/* Logic Monster */
#define MIREGP		53	/* Irregular plural name */
#define MGILDA          54      /* GUILD   100 Dark Brotherhood  010 Ancient Ones  001 Black Horse Clan*/ 
#define MGILDB          55      /* FLAGS   110 Dancers of Death  101  Hex Masters 011  Royal Guard */
#define MGILDC          56      /*         111 Order of the Golden Spear */
#define MSTOLE          57      /* Monster has been stolen from */
#define MCONJU		58	/* monster has been conjured */
#define MTEAMM		59	/* monster is a member of an attack group */
#define MTEAML		60	/* monster is the leader of an attack group */

/* Object flags */
#define OPERMT          0       /* Permanent item (not yet taken) */
#define OHIDDN          1       /* Hidden */
#define OINVIS          2       /* Invisible */
#define OSOMEA          3       /* "some" prefix */
#define ODROPS          4       /* Don't add s on plural */
#define ONOPRE          5       /* No prefix */
#define OCONTN          6       /* Container object */
#define OWTLES          7       /* Container of weightless holding */
#define OTEMPP          8       /* Temporarily permanent */
#define OPERM2          9       /* Permanent INVENTORY item */
#define ONOMAG          10      /* Mages cannot wear/use it */
#define OLIGHT          11      /* Object serves as a light */
#define OGOODO          12      /* Usable only by good players */
#define OEVILO          13      /* Usable only by evil players */
#define OENCHA          14      /* Object enchanted */
#define ONOFIX          15      /* Cannot be repaired */
#define OCLIMB          16      /* Climbing gear */
#define ONOTAK          17      /* Cannot be taken */
#define OSCENE          18      /* Part of room description/scenery */
#define OSIZE1          19      /* OSIZE: 00=all wear, 01=small wear, */
#define OSIZE2          20      /* 10=medium wear, 11=large wear */
#define ORENCH          21      /* Random enchantment flag */
#define OCURSE          22      /* The item is cursed */
#define OWEARS          23      /* The item is being worn */
#define OUSEFL          24      /* Can be used from the floor */
#define OCNDES          25      /* Container devours items */
#define ONOMAL          26      /* Usable by only females */
#define ONOFEM          27      /* Usable by only males */
#define ODDICE          28      /* damage based on object nds */
#define OPLDGK          29      /* pledge players may only use */
#define OBKSTA          30      /* Backstab weapon */
#define OCLSEL          31      /* class selective weapon */
#define OASSNO          32      /* class selective: assassin */
#define OBARBO          33      /* class selective: barbarian */
#define OCLERO          34      /* class selective: cleric */
#define OFIGHO          35      /* class selective: fighter */
#define OMAGEO          36      /* class selective: mage */
#define OPALAO          37      /* class selective: paladin */
#define ORNGRO          38      /* class selective: ranger */
#define OTHIEO          39      /* class selective: thief */
#define OBARDO		40	/* class selective: bard */
#define OMONKO		41	/* class selective: monk */
#define ONSHAT		42	/* weapon will never shatter */
#define OALCRT		43	/* weapon will always critical */
#define OLUCKY		44	/* Item is a luck charm */
#define OCURSW          45      /* Item is cursed & worn */
#define OHBREW          46      /* Herb brew */
#define OHNGST          47      /* Herb ingest */
#define OHPAST          48      /* Herb paste */
#define OHAPLY          49      /* Herb apply */
#define ODRUDT          50      /* Druid Item */
#define OALCHT		51	/* Alchemist Item */
#define OTMPEN		52	/* Object is temporarily enchanted */
#define ODRUDO		53	/* class selective: druid */
#define OALCHO		54	/* class selective: alchemist */
#define OIREGP		55	/* Irregualr plural name */
#define OGILDA          56      /*   GUILD   100 Circle 010  Masters 001 Brotherhood */        
#define OGILDB          57      /*   FLAGS   110 Clan   101  Company 011 Guard */ 
#define OGILDC          58      /*           111 Champs 000  Council */
#define OSTOLE          59      /* Stolen object - guild theft*/
#define OADDSA		60	/* Adds attribute */
#define ONOSEL		61	/* cannot be sold at pawn */
#define OTHEFT		62	/* Stolen object - non-guild theft */

/* Exit flags */
#define XSECRT          0       /* Secret */
#define XINVIS          1       /* Invisible */
#define XLOCKD          2       /* Locked */
#define XCLOSD          3       /* Closed */
#define XLOCKS          4       /* Lockable */
#define XCLOSS          5       /* Closable */
#define XUNPCK          6       /* Un-pickable lock */
#define XNAKED          7       /* Naked exit */
#define XCLIMB          8       /* Climbing gear required to go up */
#define XREPEL          9       /* Climbing gear require to repel */
#define XDCLIM          10      /* Very difficult climb */
#define XFLYSP          11      /* Must fly to go that way */
#define XFEMAL          12      /* female only exit */
#define XMALES          13      /* male only exit */
#define XPLDGK          14      /* pledge player exit only */
#define XKNGDM          15      /* exit for kingdom 0/1 */
#define XNGHTO          16      /* only open at night */
#define XDAYON          17      /* only open during day */
#define XPGUAR          18      /* passive guarded exit */
#define XNOSEE          19      /* Can not use / see exit */
#define XPLSEL		20	/* Class selective exit */
#define XPASSN		21
#define XPBARB		22	
#define XPCLER		23
#define XPFGHT		24
#define XPMAGE		25
#define XPPALA		26
#define XPRNGR		27
#define XPTHEF		28
#define XPBARD		29
#define XPMONK		30
#define XPDRUD		31
#define XPALCH		32
#define XBUILD          33
#define XGILDA          34      /*   GUILD   100 Circle 010 Masters  001 Brotherhood */
#define XGILDB          35      /*   FLAGS   110 Clan   101  Company 011  Guard */
#define XGILDC          36      /*           111 Champs 000 Council */
#define XRANDM		37	/* Random dissapear exit */
#define XRANDB		38	/* rand dissapear which globally broads message */
#define XRAND2		39
#define XUNIQP		40	/* player selective exit - uses name of target room for strcmp to ply_ptr->name */


/* Creature stats */
#define STR             1
#define DEX             2
#define CON             3
#define INTELLIGENCE    4
#define PTY             5


/* Character classes */
#define ASSASSIN        1
#define BARBARIAN       2
#define CLERIC          3
#define FIGHTER         4
#define MAGE            5
#define PALADIN         6
#define RANGER          7
#define THIEF           8
#define BARD			9
#define MONK			10
#define DRUID			11
#define ALCHEMIST		12
#define BUILDER			13
#define CARETAKER       14
#define DM              15

/* start of the immortals */
#define IMMORTAL	(BUILDER)

#define CLASS_COUNT		(DM + 1)


/* Character races */
#define DWARF           1
#define ELF             2
#define HALFELF         3
#define HOBBIT          4
#define HUMAN           5
#define ORC             6
#define HALFGIANT       7
#define GNOME           8
#define TROLL		9
#define HALFORC		10
#define OGRE		11
#define DARKELF		12
#define GOBLIN		13


/* object types */
#define ARMOR           5
#define POTION          6
#define SCROLL          7
#define WAND            8
#define CONTAINER       9
#define MONEY           10
#define KEY             11
#define LIGHTSOURCE     12
#define MISC            13
#define HERB            14 
#define FOOD		15
#define DRINK		16

/* Proficiencies */
#define SHARP           0
#define THRUST          1
#define BLUNT           2
#define POLE            3
#define MISSILE         4
#define HAND		5

/* Spell Realms */
#define EARTH           1
#define WIND            2
#define FIRE            3
#define WATER           4
#define	DETECT		5
#define PROTECT		6
#define	CLERICAL	7
#define	SORCERY		8

/* Daily use variables */
#define DL_BROAD        0       /* Daily broadcasts */
#define DL_ENCHA        1       /* Daily enchants */
#define DL_FHEAL        2       /* Daily heals */
#define DL_TRACK        3       /* Daily track spells */
#define DL_DEFEC        4       /* Daily defecations */
#define DL_CHARM	5	/* Daily charms */
#define DL_RCHRG	6	/* Daily wand recharges */
#define DL_TELEP	7	/* Daily tports */
#define DL_BROAE	8	/* Daily broad-emotes */
#define DL_RMGIC	9	/* Daily resist-magic's */

/* Last-time specifications */
#define LT_INVIS        0
#define LT_PROTE        1
#define LT_BLESS        2
#define LT_ATTCK        3
#define LT_TRACK        4
#define LT_STEAL        5
#define LT_PICKL        6
#define LT_MWAND        6
#define LT_SERCH        7
#define LT_HEALS        8
#define LT_SPELL        9
#define LT_PEEKS        10
#define LT_PLYKL        11
#define LT_READS        12
#define LT_LIGHT        13
#define LT_HIDES        14
#define LT_TURNS        15
#define LT_HASTE        16
#define LT_DINVI        17
#define LT_DMAGI        18
#define LT_PRAYD        19
#define LT_PREPA        20
#define LT_LEVIT        21
#define LT_PSAVE        22
#define LT_RFIRE        23
#define LT_FLYSP        24
#define LT_RMAGI        25
#define LT_MOVED        26
#define LT_KNOWA        27
#define LT_HOURS        28
#define LT_RCOLD        29
#define LT_BRWAT        30
#define LT_SSHLD        31
#define LT_SCOUT	33
#define LT_SILNC	34
#define LT_SINGS	35
#define LT_CHRMD	36		/* last time you were charmed */
#define LT_MEDIT	37
#define LT_TOUCH	38
#define LT_SECCK	39
#define LT_BSRKN       	40
#define LT_SUSTN	41
#define LT_PSEND	42
#define LT_CHARM	43		/* last time you charmed */
#define LT_MSCAV       	44
#define LT_CONJU	45

#define F_ISSET(p,f)    (((p)->flags[(f)/8] & 1<<((f)%8)) ? 1 : 0 )
#define F_SET(p,f)      ((p)->flags[(f)/8] |= 1<<((f)%8))
#define F_CLR(p,f)      ((p)->flags[(f)/8] &= ~(1<<((f)%8)))


#define FS_ISSET(p,f)   (((p)->sets_flag[(f)/8] & 1<<((f)%8)) ? 1 : 0 )
#define FS_SET(p,f)     ((p)->sets_flag[(f)/8] |= 1<<((f)%8))
#define FS_CLR(p,f)     ((p)->sets_flag[(f)/8] &= ~(1<<((f)%8)))

#define S_ISSET(p,f)    ((p)->spells[(f)/8] & 1<<((f)%8))
#define S_SET(p,f)      ((p)->spells[(f)/8] |= 1<<((f)%8))
#define S_CLR(p,f)      ((p)->spells[(f)/8] &= ~(1<<((f)%8)))

#define Q_ISSET(p,f)    ((p)->quests[(f)/8] & 1<<((f)%8))
#define Q_SET(p,f)      ((p)->quests[(f)/8] |= 1<<((f)%8))
#define Q_CLR(p,f)      ((p)->quests[(f)/8] &= ~(1<<((f)%8)))


typedef struct obj_tag {		/* Object list tags */
	struct obj_tag 	*next_tag;
	struct object	*obj;
} otag;

typedef struct crt_tag {		/* Creature list tags */
	struct crt_tag	*next_tag;
	struct creature	*crt;
} ctag;

typedef struct rom_tag {		/* Room list tags */
	struct rom_tag	*next_tag;
	struct room	*rom;
} rtag;

typedef struct ext_tag {		/* Exit list tags */
	struct ext_tag	*next_tag;
	struct exit_	*ext;
} xtag;

typedef struct enm_tag {		/* Enemy list tags */
	struct enm_tag	*next_tag;
	char		enemy[80];
	int		damage;
} etag;

typedef struct tlk_tag {		/* Talk list tags */
	struct tlk_tag	*next_tag;
	char		*key;
	char		*response;
	char		type;
	char		*action;
	char		*target;
        char            on_cmd;          /* action functions */
        char            if_cmd;          /* if # command succesful */         
        char            test_for;        /* test for condition in room */
        char            do_act;          /* do action */
        char            success;         /* command was succesful */
        char            if_goto_cmd;     /* used to jump to new cmd point */
        char            not_goto_cmd;    /* jump to cmd if not success */
        char            goto_cmd;        /* unconditional goto cmd */
        int             arg1;
        int             arg2;
} ttag;


typedef struct queue_tag {		/* General queue tag data struct */
	int			index;
	struct queue_tag	*next;
	struct queue_tag	*prev;
} qtag;


typedef struct daily {			/* Daily-use operation struct */
	char		max;
	char		cur;
	time_t		ltime;
} daily;


typedef struct lasttime {		/* Timed operation struct */
	long		interval;
	time_t		ltime;
	short		misc;
} lasttime;


typedef struct room {
	short		rom_num;
	char		name[80];
	char		*short_desc;	/* Descriptions */
	char		*long_desc;
	char		*obj_desc;
	char		lolevel;	/* Lowest level allowed in */
	char		hilevel;	/* Highest level allowed in */
	short		special;
	char		trap;
	short		trapexit;
	char		track[80];	/* Track exit */
	char		flags[16];
	short		random[10];	/* Random monsters */
	char		traffic;	/* R. monster traffic */
	struct lasttime	perm_mon[10];	/* Permanent/reappearing monsters */
	struct lasttime	perm_obj[10];	/* Permanent/reappearing items */
	long		beenhere;	/* # times room visited */
	time_t		established;	/* Time room was established */
	xtag		*first_ext;	/* Exits */
	otag		*first_obj;	/* Items */
	ctag		*first_mon;	/* Monsters */
	ctag		*first_ply;	/* Players */
                                        /* NEW here down */
        short           death_rom;      /* This room's limbo */
        char            zone_type;      /* Define a zone */
        char            zone_subtype;   /* and a subzone */
        char            env_type;       /* and an environment */
	short 		special1;	/* reserved for future use */
	long		special2;
} room;



typedef struct exit_ {
	char		name[20];
	short		room;
	char		flags[8];
	struct		lasttime	ltime;		/* Timed open/close */
	struct		lasttime	random;
	char		rand_mesg[2][80];		/* message for random */
	unsigned 	char 		key;		/* Key required */
} exit_;

typedef struct object {
	char 		name[80];
	char		description[80];
	char		key[3][20];     /* key 3 hold object index */
	char		use_output[80];	/* String output by successful use */
	long 		value;
	short 		weight;
	char 		type;
	char		adjustment;
	short		shotsmax;	/* Shots before breakage */
	short		shotscur;
	short		ndice;		/* Number/Sides/Plus dice */
	short		sdice;
	short		pdice;
	char		armor;		/* AC adjustment */
	char		wearflag;	/* Where/If it can be worm */
	char		magicpower;
	char		magicrealm;
	short		special;
	char		flags[16];
	char		questnum;	/* Quest fulfillment number */
	otag		*first_obj;	/* Objects contained inside */
	struct object	*parent_obj;	/* Object this is in */
	struct room	*parent_rom;	/* Room this is in */
	struct creature	*parent_crt;	/* Creature this is in */
                                        /* NEW here down */
        char            strength;       /* Add attributes when used */
        char            dexterity;
        char            constitution;
        char            intelligence;
        char            piety;
        short           sets_flag[16];  /* Allow an object to turn on some
                                        player flags when used - shots can be
                                        decremented each time held/worn */
	short		special1;	/* reserved for future use */
	long		special2;
} object;



typedef struct creature {
	char		name[80];
	char		description[80];
	char		talk[80];
	char		password[15];
	char		key[3][20];
	short		fd;		/* Socket number */
	char		level;
	char		type;		/* Creature type */
	char		class;
	char		race;
	char		numwander;
	short		alignment;
	char		strength;
	char		dexterity;
	char		constitution;
	char		intelligence;
	char		piety;
	short		hpmax;		/* Max/Current hp and mp */
	short		hpcur;
	short		mpmax;
	short		mpcur;
	char		armor;		/* Armor Class */
	char		thaco;		/* Thac0 */
	long		experience;
	long		gold;
	short		ndice;		/* Bare-handed damage */
	short		sdice;
	short		pdice;
	short		special;
					/* extra prof. for hand */
	long		proficiency[6]; /* Weapon proficiencies */
					/* increase realms */
	long		realm[8];	/* Magic Spell realms */
	char		spells[16];	/* Known spells */
	char		flags[16];
	char		quests[16];	/* Quests fulfilled (P) */
#define 		NUMHITS quests[0]
	char		questnum;	/* Quest fulfillment number (M) */
	short		carry[10];	/* Random items monster carries */
#define			WIMPYVALUE carry[0] 
	short		rom_num;
	struct object	*ready[MAXWEAR];/* Worn/readied items */
					/* More Dailys and lasttimes */
	struct daily	daily[20];	/* Daily use variables */
	struct lasttime	lasttime[65];	/* Last-time-used variables */
	struct creature	*following;	/* Creature being followed */
	ctag		*first_fol;	/* List of followers */
	otag		*first_obj;	/* List of inventory */
	etag		*first_enm;	/* List of enemies */
	ttag		*first_tlk;	/* List of talk responses */
	struct room	*parent_rom;	/* Room creature is in */
                                        /* NEW here down */
        long            bank_balance;
        char            title[20];
        short           misc_stats[5];  /* pkill ratios etc */
        short           clanindex;      /* clan player is in */
        long            clanexp;        /* clan exp */
        char            guildtype;      /* guild player is in */
        long            guildexp;
	short		special1;	/* reserved for future use */
	long		special2;
} creature;

typedef struct guild {
        char            name[50];
        short           good; // 0 good anything else evil
        short           lawful; // 0 good anything else chaotic
        long            score; // 0 guild score
        int             pkilled; // 0 pkilling score
        int             pkia; // 0 pkilling score
        long            hazyroom; // 0 hazy room
} guild;


typedef struct tagMemCount
{
	int		count;
	long	mem_used;
} MEM_COUNT;

typedef struct tagMemUsage
{
	MEM_COUNT	creatures;
	MEM_COUNT	rooms;
	MEM_COUNT	objects;
	MEM_COUNT	actions;
	MEM_COUNT	talks;
	MEM_COUNT	bad_talks;
} MEM_USAGE;


typedef struct rsparse {		/* Sparse pointer array for rooms */
	room 			*rom;
	qtag			*q_rom;
} rsparse;

typedef struct csparse {		/* Sparse pointer array for creatures */
	creature		*crt;
	qtag			*q_crt;
} csparse;

typedef struct osparse {		/* Sparse pointer array for objects */
	object			*obj;
	qtag			*q_obj;
} osparse;



/* ACCESS.C */
extern char *get_class_string( int nIndex );
extern char *get_race_string( int nIndex );
extern char *get_race_adjective( int nIndex );
extern char *title_ply(creature *ply_ptr );
extern int get_hash_rooms();


/* ACTIVE.C */
extern void add_active(creature *crt_ptr );
extern void del_active(creature *crt_ptr );
extern int is_crt_active(creature *crt_ptr );
extern ctag *get_first_active();
extern void log_active();

/* FILES1.C */
extern int count_obj( object *obj_ptr, char perm_only );
extern int write_obj(int fd, object *obj_ptr, char perm_only );
extern int count_inv( creature *crt_ptr, char perm_only );
extern int count_full_inv( creature *crt_ptr );
extern int write_crt(int fd, creature *crt_ptr, char perm_only );
extern int count_mon( room *rom_ptr, char perm_only );
extern int count_ite(room *rom_ptr, char perm_only );
extern int count_ext( room *rom_ptr );
extern int count_ply( room *rom_ptr );
extern int write_rom(int fd, room *rom_ptr, char perm_only );
extern int read_obj( int fd, object *obj_ptr );
extern int read_single_obj( int fd, object *obj_ptr );
extern int read_crt( int fd, creature *crt_ptr );
extern int read_single_crt( int fd, creature *crt_ptr );
extern int read_rom( int fd, room *rom_ptr );
extern int fixup_room(int fd, room *rom_ptr );
extern void free_obj( object *obj_ptr );
extern void free_crt( creature *crt_ptr );
extern void free_rom( room *rom_ptr );
extern int find_crt_num( creature *crt_ptr );
int find_obj_num(object *obj_ptr );

/* FILES2.C */
extern int load_rom( int index, room **rom_ptr );
extern int is_rom_loaded( int num );
extern void get_room_filename( int num, char *filename );
extern int reload_rom( int num );
extern int resave_rom( int num );
extern void resave_all_rom( char permonly );
extern void save_all_ply();
extern void flush_rom();
extern void flush_crt();
extern void flush_obj();
extern void replace_crt_in_queue(int index, creature *crt_ptr);
extern void replace_obj_in_queue(int index, object *obj_ptr);
extern int load_crt(int index, creature **mon_ptr );
extern int load_obj( int index, object	**obj_ptr );
extern int write_ply(char *str, creature *ply_ptr );
extern int load_ply(char *str, creature **ply_ptr );
extern void put_queue(qtag **qt, qtag **headptr, qtag **tailptr, int *sizeptr );
extern void pull_queue( qtag **qt, qtag **headptr, qtag **tailptr, int *sizeptr );
extern void front_queue(qtag **qt, qtag **headptr, qtag **tailptr, int	*sizeptr );
extern void free_all_queues();
extern int get_memory_usage( MEM_USAGE *pmu );

/* FILES3.C */
extern int write_obj_to_mem(char *buf, object *obj_ptr, char perm_only, char *bufbegin );
extern int write_crt_to_mem(char *buf, creature *crt_ptr, char	perm_only);
extern int read_obj_from_mem(char *buf, object *obj_ptr ); 
extern int read_crt_from_mem(char *buf, creature *crt_ptr );
extern int load_crt_tlk( creature *crt_ptr );
extern int talk_crt_act(char *str, ttag *tlk );


/* FILES4.C */
extern int load_rom_from_file(int num, room **rom_ptr );
extern int load_rom_struct_from_file(int num, room *rom_ptr );
extern int load_crt_from_file(int num, creature **crt_ptr );
extern int load_crt_struct_from_file(int num, creature *crt_ptr );
extern int load_obj_from_file(int num, object  **obj_ptr );
extern int load_obj_struct_from_file(int num, object *obj_ptr );
extern int load_ply_from_file(char *str, creature **ply_ptr);
extern int save_rom_to_file(int num, room *rom_ptr);
extern int save_obj_to_file(int num, object  *obj_ptr);
extern int save_crt_to_file(int num, creature *crt_ptr);
extern int save_ply_to_file(char *str, creature *ply_ptr);



#endif
