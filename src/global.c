/* GLOBAL.C:
 *
 *	Global variables.
 *
 *	Copyright (C) 1991, 1992, 1993 Brooke Paul
 *
 * $Id: global.c,v 6.30 2001/07/29 22:11:02 develop Exp $
 *
 * $Log: global.c,v $
 * Revision 6.30  2001/07/29 22:11:02  develop
 * added shop_cost
 *
 * Revision 6.29  2001/07/25 02:55:04  develop
 * fixes for thieves dropping stolen items
 * fixes for gold dropping by pkills
 *
 * Revision 6.29  2001/07/23 04:04:09  develop
 * added alias to *st for dm_status
 *
 * Revision 6.28  2001/07/22 20:42:54  develop
 * added dm_stolen
 *
 * Revision 6.27  2001/07/18 01:43:24  develop
 * changed cost of stores
 *
 * Revision 6.26  2001/07/17 19:39:49  develop
 * re-arranged some help file numbers
 *
 * Revision 6.25  2001/07/17 19:28:44  develop
 * *** empty log message ***
 *
 * Revision 6.25  2001/07/15 05:42:50  develop
 * set_rom_owner setup_shop created
 *
 * Revision 6.24  2001/07/03 22:03:38  develop
 * testing intelligent monsters
 *
 * Revision 6.23  2001/07/03 22:00:22  develop
 * *** empty log message ***
 *
 * Revision 6.22  2001/07/03 21:52:28  develop
 * *** empty log message ***
 *
 * Revision 6.21  2001/06/30 01:58:34  develop
 * added ENMLIST
 *
 * Revision 6.20  2001/06/30 01:50:30  develop
 * removed experience loss when conjured creature kills a player
 *
 * Revision 6.19  2001/06/29 03:22:12  develop
 * added struct for default enemy lists
 *
 * Revision 6.18  2001/06/10 13:25:38  develop
 * set bonus table back to original
 *
 * Revision 6.17  2001/04/30 19:55:02  develop
 * renumbered a couple of help files
 *
 * Revision 6.16  2001/04/23 03:52:29  develop
 * added NOCREATE flag to toggle character creation
 *
 * Revision 6.15  2001/04/12 05:00:57  develop
 * STRMOR <-> SEQUAK (named wrong)
 *
 * Revision 6.14  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */
#include <stdio.h>

#include "../include/mordb.h"
#define MIGNORE
#include "mextern.h"

/*****************************************************************
*
* Configurable settings.  These are the defaults for setting that
* can be overridden from the mordor.cf file in the BINPATH
*
******************************************************************/

/* DM's name */
	char dmname[][20] = {
		"Morgoth", "Sorahl", "China", "Sandman", "Ugluk", "Darwin", "Tesseract", "Erech", 
	};
	char title[80]="Mordor MUD Server";
	char auth_questions_email[80]="";
	char questions_to_email[80]="";
	char register_questions_email[80]="";
	char account_exists[80]="You already have an account here.";
	char dm_pass[20]="";  /* No DM creation by default */


	char tx_mesg1[80]="The Ithil Express has docked in Parth.";
	char tx_mesg2[80]="The Ithil Express has docked in Celduin.";

	char sunrise[80]="The sun rises.";
	char sunset[80]="The sun disappears over the horizon.";
	char earth_trembles[80]="The earth trembles under your feet.";
	char heavy_fog[80]="A heavy fog blankets the earth.";
	char beautiful_day[80]="It's a beautiful day today.";
	char bright_sun[80]="The sun shines brightly across the land.";
	char glaring_sun[80]="The glaring sun beats down upon the inhabitants of the world.";
	char heat[80]="The heat today is unbearable.";
	char still[80]="The air is still and quiet.";
	char light_breeze[80]="A light breeze blows from the south.";
	char strong_wind[80]="A strong wind blows across the land.";
	char wind_gusts[80]="The wind gusts, blowing debris through the streets.";
	char gale_force[80]="Gale force winds blow in from the sea.";
	char clear_skies[80]="Clear, blue skies cover the land.";
	char light_clouds[80]="Light clouds appear over the mountains.";
	char thunderheads[80]="Thunderheads roll in from the east.";
	char light_rain[80]="A light rain falls quietly.";
	char heavy_rain[80]="A heavy rain begins to fall.";
	char sheets_rain[80]="Sheets of rain pour down from the skies.";
	char torrent_rain[80]="A torrent soaks the ground.";
	char no_moon[80]="The sky is dark as pitch.";
	char sliver_moon[80]="A sliver of silver can be seen in the night sky.";
	char half_moon[80]="Half a moon lights the evening skies.";
	char waxing_moon[80]="The night sky is lit by the waxing moon.";
	char full_moon[80]="The full moon shines across the land.";




	int		ANSILINE=0;
	int		AUTOSHUTDOWN=0;
	int		CHECKDOUBLE=0;
	int		EATNDRINK=0;
	int		GETHOSTBYNAME=0;
	int		HEAVEN=0;
	int		ISENGARD=0;
	int		LASTCOMMAND=0;
	int		PARANOID=0;
	int		RECORD_ALL=0;
	int		RFC1413=0;
	int		SECURE=0;
	int		SCHED=0;
	int		SUICIDE=0;
	int		NOCREATE=0;

	unsigned short	PORTNUM=4040;
	int		CRASHTRAP=0;
	int		GUILDEXP=0;
	int		SAVEONDROP=0;
	int		NOBULLYS=0;
	int		MSP=0;


    int    PROMPTCOLOR = AFC_MAGENTA;
    int    CREATURECOLOR = AFC_WHITE;
    int    BROADCASTCOLOR = AFC_YELLOW;
    int    MAINTEXTCOLOR = AFC_WHITE;
    int    PLAYERCOLOR = AFC_CYAN;
    int    ITEMCOLOR = AFC_WHITE;
    int    EXITCOLOR = AFC_GREEN;
	int	   ROOMNAMECOLOR = AFC_CYAN;
    int    ECHOCOLOR = AFC_CYAN;
    int    WEATHERCOLOR = AFC_WHITE;


    int    ATTACKCOLOR = AFC_RED;
    int    MISSEDCOLOR = AFC_CYAN;
    int    TRAPCOLOR = AFC_MAGENTA;
    int    POISONCOLOR = AFC_RED;
    int    DISEASECOLOR = AFC_RED;
    int    BLINDCOLOR = AFC_RED;
	int	   SILENCECOLOR = AFC_YELLOW;	
	int	   ERRORCOLOR = AFC_RED;

	/* not used yet */
    int    LOGONOFFCOLOR = AFC_WHITE;
    int    TITLECOLOR = AFC_WHITE;
    int    TALKCOLOR = AFC_WHITE;
    int    FROSTCOLOR = AFC_YELLOW;

	int	start_room_num = 1;
	int	death_room_num = 50;
	int	jail_room_num = 6972;
	int 	pkill_in_battle = 1;
	int 	init_eq[9] = { 1505, 1502, 1503, 1504, 1, 3, 2, 5, 6 };
	int 	trophy_num = 1500;
	long	shop_cost = 1000000;

	time_t	dm_timeout = 0;
	time_t	ct_timeout = 1200;
	time_t	bld_timeout = 1200;
	time_t	ply_timeout = 300;

#ifdef TODO

	make these configurable flags in mordor.cf
	that then control whether that command is logged or not.

	*teleport
	*rm
	*reload
	*save
	*create
	*perm
	*invis
	*send
	*purge
	*ac
	*users
	*echo
	*flushrooms
	*shutdown
	*force
	*flushcrtobj
	*monster
	*status
	*add
	*set
	*log
	*spy
	*lock
	*finger
	*list
	*info
	*parameter
	*silence
	*broad
 	*replace
	*name
   	*append
   	*prepend
   	*gcast
	*group
	*notepad
 	*delete
	*oname
	*cname
	*active
	*dust
	*dmhelp
	*attack
	*enemy
	*charm
	*auth
	*possess
	*tell
	*memory
	*find
	*clear
	*talk
	*gamestat
	*advance
	*dmallocstats
	*war
#endif

/* end configurable settings */

char	g_buffer[4096];
int		Tablesize;
int		Cmdnum;
long	Time;
time_t	StartTime;
struct	lasttime	Shutdown;
struct	lasttime	Guildwar;
struct	lasttime	Weather[5];
int		Numlockedout;
lockout	*Lockout;

int	bHavePort;
short	Random_update_interval = 13;

plystruct Ply[PMAX];

class_stats_struct class_stats[CLASS_COUNT] = {
	{  0,  0,  0,  0,  0,  0,  0},
	{ 19,  2,  6,  2,  1,  6,  0},	/* assassin */
	{ 24,  1,  8,  1,  1,  3,  1},	/* barbarian */
	{ 16,  4,  5,  4,  1,  4,  0},	/* cleric */
	{ 22,  2,  7,  2,  1,  5,  0},	/* fighter */
	{ 14,  5,  4,  5,  1,  3,  0},	/* mage */
	{ 19,  3,  6,  3,  1,  4,  0},	/* paladin */
	{ 18,  3,  6,  3,  2,  2,  0},	/* ranger */
	{ 18,  3,  5,  2,  2,  2,  1},	/* thief */
	{ 15,  3,  5,  4,  2,  2,  1},  /* bard */
	{ 17,  3,  6,  2,  1,  3,  0},  /* monk */
	{ 15,  4,  5,  3,  1,  4,  0},  /* druid */
	{ 15,  4,  4,  4,  1,  3,  0},	/* alchemist */
	{ 30, 30, 10, 10,  5,  5,  5},	/* builder */
	{ 30, 30, 10, 10,  5,  5,  5},	/* caretaker */
	{ 30, 30, 10, 10,  5,  5,  5}	/* DM */
};

int bonus[35] = { -4, -4, -4, -3, -3, -2, -2, -1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		  2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

/* int bonus[35] = { -4, -4, -3,-3,-3,-2, -2, -1, 0, 0, 0, 0, 0, 1, 1, 1, 1,
                  2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 }; */


/* INT is a data type on some platforms so I had to change it */
/* but I did not want to mess up this nicely arranged array */
#define INT INTELLIGENCE

/*

The level_cycle has been severely tweaked, with this idea in mind:
P = Primary Requisite
S = Secondary Requisite
M = Important but not a requisite
N = Not a requisite
Z = Unneeded/unwanted stat

The core structure is this for all classes:
Level:  2   3   4   5   6   7   8   9   10  11
Stat:   S   M   N   Z   P   S   P   M   N   P

*/

short level_cycle[][10] = {
   /* 2    3    4    5    6    7    8    9    10   11 */
    { 0,   0,   0,   0,   0,   0,   0,   0,   0,   0   },
    { STR, CON, INT, PTY, DEX, STR, DEX, CON, INT, DEX },  /* assassin */
    { CON, DEX, PTY, INT, STR, CON, STR, DEX, PTY, STR },  /* barbarian */
    { INT, DEX, CON, STR, PTY, INT, PTY, DEX, CON, PTY },  /* cleric */
    { DEX, CON, PTY, INT, STR, DEX, STR, CON, PTY, STR },  /* fighter */
    { PTY, DEX, CON, STR, INT, PTY, INT, DEX, CON, INT },  /* mage */
    { STR, CON, DEX, INT, PTY, STR, PTY, CON, DEX, PTY },  /* paladin */
    { STR, CON, PTY, INT, DEX, STR, DEX, CON, PTY, DEX },  /* ranger */
    { INT, STR, CON, PTY, DEX, INT, DEX, STR, CON, DEX },  /* thief */
    { DEX, STR, PTY, CON, INT, DEX, INT, STR, PTY, INT },  /* bard */
    { DEX, PTY, INT, STR, CON, DEX, CON, PTY, INT, CON },  /* monk */
    { PTY, CON, INT, STR, DEX, PTY, DEX, CON, INT, DEX },  /* druid */
    { INT, PTY, DEX, STR, CON, INT, CON, PTY, DEX, CON },  /* alchemist */
    { STR, DEX, INT, CON, PTY, STR, DEX, INT, CON, PTY },  /* builder */
    { STR, DEX, INT, CON, PTY, STR, DEX, INT, CON, PTY },  /* caretaker */
    { STR, DEX, INT, CON, PTY, STR, DEX, INT, CON, PTY }   /* DM */
};

#undef INT

short thaco_list[][20] = { 
		{ 20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20 }, 
/*a*/	{ 18,18,18,17,17,16,16,15,15,14,14,13,13,12,12,11,10,10, 9, 9 },
/*b*/	{ 20,19,18,17,16,15,14,13,12,11,10, 9, 8, 7, 6, 5, 4, 3, 3, 2 },
/*c*/	{ 20,20,19,18,18,17,16,16,15,14,14,13,13,12,12,11,10,10, 9, 8 },
/*f*/	{ 20,19,18,17,16,15,14,13,12,11,10, 9, 8, 7, 6, 5, 4, 3, 3, 3 },
/*m*/	{ 20,20,19,19,18,18,18,17,17,16,16,16,15,15,14,14,14,13,13,11 },
/*p*/	{ 19,19,18,18,17,16,16,15,15,14,14,13,13,12,11,11,10, 9, 8, 7 },
/*r*/	{ 19,19,18,17,16,16,15,15,14,14,13,12,12,11,11,10, 9, 9, 8, 7 },
/*t*/	{ 20,20,19,19,18,18,17,17,16,16,15,15,14,14,13,13,12,12,11,11 },
/*bd*/  { 18,18,18,17,17,16,16,15,15,14,14,13,12,11,11,10, 9, 9, 8, 7 },
/*mn*/  { 18,18,17,17,16,16,15,15,14,14,13,12,11,11,10,10, 9, 8, 7, 6 },	
/*dr*/  { 20,19,19,18,18,17,17,16,15,14,14,13,13,12,12,11,10,10, 9, 8 },
/*al*/  { 20,20,19,19,18,18,17,17,16,16,15,15,15,14,14,13,13,12,11,10 },
/*bu*/	{  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
/*ct*/	{  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
/*dm*/	{  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};


 
long needed_exp[] = {
    /*2   3     4     5     6     7      8      9      10 */
    512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 100000,
    166410, 277360, 394171, 560992, 757125, 1087504, 1402815,
    1739616, 2203457, 2799000, 3505139, 4465120, 5792661, 7319072,
    8874375, 15000000};

guild cur_guilds[10];

char ply_cond_desc[][50] = {
        {" is in excellent condition.\n"},
        {" has a few small scratches.\n"},
        {" is wincing in pain.\n"},
        {" is slightly bruised and battered.\n"},
        {" has some minor wounds.\n"},
        {" is bleeding profusely.\n"},
        {" has a nasty and gaping wound.\n"},
        {" has many grevious wounds.\n"},
        {" is mortally wounded.\n"},
        {" is barely clinging to life.\n"}
        };

char obj_cond_desc[][60] = {
        {"It is in pristine condition.\n"},
        {"It is in excellent condition.\n"},
        {"It is in good condition.\n"},
        {"It has a few scratches.\n"},
        {"It is showing some wear.\n"},
        {"It has many scratches and dents.\n"},
        {"It is worn from heavy use.\n"},
        {"It is in bad condition.\n"},
        {"It is nearly broken.\n"},
        {"It looks like it could fall apart any moment now.\n"},
        {"It is broken and utterly useless.\n"}
	};

char dam_desc[][20] = {
	{"barely nicks"},
	{"scratches"},
	{"bruises"},
	{"hurts"},
	{"wounds"},
	{"smites"},
	{"maims"},
	{"pulverizes"},
	{"devestates"},
	{"mangles"},
	{"obliterates"},
	{"annihilates"},
	{"ferociously rends"}
	};



typedef int (* PFN_COMMAND ) (creature *, cmd * );

struct {
	char	*cmdstr;
	int	cmdno;
	PFN_COMMAND cmdfn;
} cmdlist[] = {
	{ "n", 1, move },
	{ "north", 1, move },
	{ "s", 1, move },
	{ "south", 1, move },
	{ "e", 1, move },
	{ "east", 1, move },
	{ "w", 1, move },
	{ "west", 1, move },
	{ "northeast", 1, move },
	{ "ne", 1, move },
	{ "northwest", 1, move },
	{ "nw", 1, move },
	{ "southeast", 1, move },
	{ "se", 1, move },
	{ "southwest", 1, move },
	{ "sw", 1, move },
	{ "u", 1, move },
	{ "up", 1, move },
	{ "d", 1, move },
	{ "down", 1, move },
	{ "out", 1, move },
	{ "leave", 1, move },
	{ "l" , 2, look },
	{ "look", 2, look },
	{ "consider", 2, look },
	{ "examine", 2, look },
	{ "quit", 3, quit },
	{ "quit", 3, quit },
	{ "goodbye", 3, quit },
	{ "goodbye", 3, quit },
	{ "say", 4, say },
/*	{ "\"", 4, say }, */
	{ "'", 4, say },
	{ "get", 5, get },
	{ "take", 5, get },
	{ "inventory", 6, inventory },
	{ "i", 6, inventory },
	{ "drop", 7, drop },
	{ "put", 7, drop },
	{ "who", 8, who },
	{ "wear", 9, wear },
	{ "remove", 10, remove_obj },
	{ "rm", 10, remove_obj },
	{ "equipment", 11, equipment },
	{ "hold", 12, hold },
	{ "wield", 13, ready },
	{ "ready", 13, ready },
	{ "help", 14, help },
	{ "?", 14, help },
	{ "health", 15, health },
	{ "score", 15, health },
	{ "sc", 15, health },
	{ "information", 16, info },
	{ "send", 17, psend },
	{ "sen", 17, psend },
	{ "tell", 17, psend },
	{ "follow", 18, follow },
	{ "lose", 19, lose },
	{ "group", 20, group },
	{ "party", 20, group },
	{ "track", 21, track },
	{ "peek", 22, peek },
	{ "attack", 23, attack },
	{ "kill", 23, attack },
	{ "k", 23, attack },
	{ "search", 24, search },
	{ "emote",25, emote},    
	{ ":",25, emote},    
	{ "hide", 26, hide },
	{ "set", 27, set },
	{ "clear", 28, clear },
	{ "yell", 29, yell },
	{ "go", 30, go },
	{ "enter", 30, go },
	{ "open", 31, openexit },
	{ "close", 32, closeexit },
	{ "shut", 32, closeexit },
	{ "unlock", 33, unlock },
	{ "lock", 34, lock },
	{ "pick", 35, picklock },
	{ "steal", 36, steal },
	{ "flee", 37, flee },
	{ "fl", 37, flee },
	{ "run", 37, flee },
	{ "c", 38, cast },
	{ "cast", 38, cast },
	{ "study", 39, study },
	{ "learn", 39, study },
	{ "read", 40, readscroll },
	{ "recite", 40, readscroll },
	{ "list", 41, list },
	{ "buy", 42, buy },
	{ "sell", 43, sell },
	{ "appraise", 44, appraise },
	{ "cost", 44, appraise },
	{ "value", 44, appraise },
	{ "bs", 45, backstab },
	{ "backstab", 45, backstab },
	{ "train", 46, train },
	{ "give", 47, give },
	{ "repair", 48, repair },
	{ "fix", 48, repair },
	{ "time", 49, prt_time },
	{ "circle", 50, circle },
	{ "bash", 51, bash },
	{ "save", 52, save_ply_cmd },
	{ "sendmail", 53, postsend },
	{ "readmail", 54, postread },
	{ "deletemail", 55, postdelete },
	{ "talk", 56, talk },
	{ "ask", 56, talk },
	{ "parley", 56, talk },
	{ "gtalk",57, gtalk },
	{ "drink", 58, drink },
	{ "quaff", 58, drink },
	{ "broadcast", 59, broadsend },
	{ "zap", 60, zap },
	{ "welcome", 61, welcome },
	{ "turn", 62, turn },
	{ "bribe", 63, bribe },
	{ "haste", 64, haste },
	{ "pray", 65, pray },
	{ "prepare", 66, prepare },
	{ "use", 67, use },
	{ "ignore", 68, ignore },
	{ "whois", 69, whois },
	{ "sneak", 70, sneak },
	{ "teach", 71, teach },
	{ "pledge",72, pledge },
	{ "rescind",73, rescind },
	{ "purchase",74, purchase },    
	{ "selection",75, selection }, 
	{ "trade",76, trade},    
	{ "suicide", 77, ply_suicide },
	{ "passwd", 78, passwd},
	{ "password", 78, passwd },
	{ "vote",79,vote},
	{ "finger", 80, pfinger},
	{ "charm", 81, bard_song2},
	{ "meditate", 82, meditate},
	{ "touch", 83, touch_of_death},
	{ "transform",84,transform },
	{ "apply",85,apply_herb },
	{ "ingest",86,ingest_herb },
	{ "eat",87,eat },
	{ "paste",88,paste_herb },
	{ "recharge", 89, recharge_wand },
	{ "description", 90, describe_me },
	{ "enchant", 91, tmp_enchant },
	{ "version", 92, about },
	{ "scout", 93, scout },
	{ "transmute", 94, transmute },
	{ "berserk", 95, berserk },
	{ "uptime", 96, uptime},
	{ "barkskin", 97, barkskin },
/*
	{ "channelemote", 97, channelemote},
	{ "chemote", 97, channelemote},
	{ "broe", 97, channelemote},
*/
	{ "classemote", 98, classemote},
	{ "clemote", 98, classemote},
	{ "classsend", 99, class_send},
	{ "clsend", 99, class_send},
	/* help 101-399 reserved for DM commands */
	{ "guildtalk", 401, guild_send},
	{ "gls", 401, guild_send},
	{ "guilds", 402, guilds_list},
	{ "offer", 403, offer},	
	{ "shop", 404, setup_shop },
	{ "balance", 405, bank_balance },
	{ "withdraw", 405, bank_withdrawl },
	{ "deposit", 405, bank_deposit },
	{ "transfer", 405, transfer_balance },
	
	{ "nod", 100, action },
	{ "sleep", 100, action },
	{ "grab", 100, action },
	{ "shove", 100, action },
	{ "stand", 100, action },
	{ "sit", 100, action },
	{ "nervous", 100, action },
	{ "bird", 100, action },
	{ "ogle", 100, action },
	{ "nod", 100, action },
	{ "relax", 100, action },
	{ "puke", 100, action },
	{ "think", 100, action },	
	{ "cheer", 100, action },
	{ "ponder", 100, action },
	{ "ack", 100, action },
	{ "laugh", 100, action },
	{ "burp", 100, action },
	{ "frustrate", 100, action },
	{ "kick", 100, action },
	{ "tackle", 100, action },
	{ "knee", 100, action },
	{ "pounce", 100, action },
	{ "tickle", 100, action },
	{ "snicker", 100, action },
	{ "tap", 100, action },
	{ "smile", 100, action },
	{ "beam", 100, action},
	{ "masturbate", 100, action},
	{ "smoke", 100, action },
	{ "shake", 100, action },
	{ "cackle", 100, action },
	{ "chuckle", 100, action },
	{ "wave", 100, action },
	{ "poke", 100, action },
	{ "yawn", 100, action },
	{ "sigh", 100, action },
	{ "bounce", 100, action },
	{ "shrug", 100, action },
	{ "twiddle", 100, action },
	{ "grin", 100, action },
	{ "frown", 100, action },
	{ "giggle", 100, action },
	{ "sing", 100, action },
	{ "hum", 100, action },
	{ "snap", 100, action },
	{ "jump", 100, action },
	{ "skip", 100, action },
	{ "dance", 100, action },
	{ "cry", 100, action },
	{ "bleed", 100, action },
	{ "sniff", 100, action },
	{ "whimper", 100, action },
	{ "cringe", 100, action },
	{ "whistle", 100, action },
	{ "smirk", 100, action },
	{ "gasp", 100, action },
	{ "grunt", 100, action },
	{ "stomp", 100, action },
	{ "flex", 100, action },
	{ "curtsy", 100, action },
	{ "blush", 100, action },
	{ "faint", 100, action },
	{ "hug", 100, action },
	{ "expose", 100, action },
	{ "wink", 100, action },
	{ "clap", 100, action },
	{ "drool", 100, action },
	{ "copulate", 100, action },
	{ "goose", 100, action },
	{ "fume", 100, action },
	{ "rage", 100, action },
	{ "pout", 100, action },
	{ "spit", 100, action },
	{ "fart", 100, action },
	{ "comfort", 100, action },
	{ "pat", 100, action },
	{ "pet", 100, action },
	{ "kiss", 100, action },
	{ "glare", 100, action },
	{ "slap", 100, action },
	{ "suck", 100, action },
	{ "defecate", 100, action },
	{ "bow", 100, action },
	{ "cough", 100, action },
	{ "confused", 100, action },
	{ "grumble", 100, action },
	{ "hiccup", 100, action },
	{ "mutter", 100, action },
	{ "scratch", 100, action },
	{ "strut", 100, action },
	{ "sulk", 100, action },
	{ "wince", 100, action },
	{ "roll", 100, action },
	{ "raise", 100, action },
	{ "whine", 100, action },
	{ "growl", 100, action },
	{ "grr", 100, action },
	{ "high5", 100, action },
	{ "moon", 100, action },
	{ "purr", 100, action },
	{ "taunt", 100, action },
	{ "eye", 100, action },
	{ "msmile", 100, action },
	{ "worship", 100, action },
	{ "flip", 100, action },
	{ "groan", 100, action },
	{ "salute", 100, action },
	{ "sneer", 100, action },
	{ "grovel", 100, action },
	{ "nose", 100, action },
	{ "cuddle", 100, action },
	{ "wsmile", 100, action },
	{ "ssmile", 100, action },
	{ "twirl", 100, action },
	{ "flaugh", 100, action },
	{ "rlaugh", 100, action },
	{ "hlaugh", 100, action },
	{ "snivel", 100, action },
	{ "smell", 100, action },
	{ "pace", 100, action },
	{ "sneeze", 100, action },
	{ "lick", 100, action },
	{ "snort", 100, action },
	{ "grind", 100, action },
	{ "kisshand", 100, action },
	{ "sneer", 100, action },
	{ "point", 100, action },
	{ "imitate", 100, action },
	{ "thumb", 100, action },
	{ "punch", 100, action },
	{ "plead", 100, action },
	{ "grovel", 100, action },
	{ "cower", 100, action },
	{ "cuddle", 100, action },
	{ "shush", 100, action },
	{ "beckon", 100, action },
	{ "greet", 100, action },
	{ "squirm", 100, action },
	{ "idea", 100, action },
	{ "collapse", 100, action },
	{ "sweat", 100, action },
	{ "attention", 100, action },
	{ "squirm", 100, action },
	{ "nosepick", 100, action },
	{ "blah", 100, action },
	/* help 101-399 reserved for DM commands */
	{ "*teleport", 101, dm_teleport },
	{ "*t", 101, dm_teleport },
	{ "*rm", 102, dm_rmstat },
	{ "*reload", 103, dm_reload_rom },
	{ "*save", 104, dm_save },
	{ "*create", 105, dm_create_obj },
	{ "*c", 105, dm_create_obj },
	{ "*perm", 106, dm_perm },
	{ "*invis", 107, dm_invis },
	{ "*i", 107, dm_invis },
	{ "*s", 108, dm_send },
	{ "*send", 108, dm_send },
	{ "*purge", 109, dm_purge },
	{ "*ac", 110, dm_ac },
	{ "*users", 111, dm_users },
	{ "*echo", 112, dm_echo },
	{ "*flushrooms", 113, dm_flushsave },
	{ "*sh", 114, shutdown_catch},
	{ "*shu", 114, shutdown_catch},
	{ "*shut", 114, dm_shutdown},
	{ "*shutd", 114, shutdown_catch},
	{ "*shutdo", 114, shutdown_catch},
	{ "*shutdow", 114, shutdown_catch},
	{ "*shutdown", 114, dm_shutdown },
	{ "*f", 115, dm_force },
	{ "*force", 115, dm_force },
	{ "*flushcrtobj", 116, dm_flush_crtobj },
	{ "*monster", 117, dm_create_crt },
	{ "*status", 118, dm_stat },
	{ "*st", 118, dm_stat },
	{ "*add", 119, dm_add_rom },
	{ "*set", 120, dm_set },
	{ "*log", 121, dm_log },
	{ "*spy", 122, dm_spy },
	{ "*lock", 123, dm_loadlockout },
	{ "*finger", 124, dm_finger },
	{ "*list", 125, dm_list },
	{ "*info", 126, dm_info },
	{ "*parameter", 127, dm_param },
	{ "*silence", 128, dm_silence},
	{ "*broad", 129, dm_broadecho},
 	{ "*replace",130, dm_replace},    
	{ "*name",131, dm_nameroom},    
   	{ "*append",132, dm_append},    
   	{ "*prepend",133, dm_prepend},    
   	{ "*gcast",134, dm_cast},    
	{ "*group",135, dm_group},    
	{ "*notepad",136, notepad},
 	{ "*delete",137, dm_delete},    
	{ "*oname", 138, dm_obj_name },
	{ "*cname", 139, dm_crt_name },
	{ "*active", 140, list_act },
	{ "*dust", 141, dm_dust },
	{ "*dmhelp", 142, dm_help },
	{ "*attack", 143, dm_attack },
	{ "*enemy", 144, list_enm },
	{ "*charm", 145, list_charm },
	{ "*auth", 146, dm_auth },
	{ "*possess", 147, dm_alias }, 
	{ "*tell", 148, dm_flash }, 
	{ "*memory", 149, dm_memory },
	{ "*find", 150, dm_find_db },
	{ "*clear", 151, dm_delete_db },
	{ "*talk", 152, dm_talk },
	{ "*gamestat", 153, dm_game_status },
	{ "*advance", 154, dm_advance },
	{ "*mallocstats", 155, dm_dmalloc_stats },
	{ "*builderhelp", 160, builder_help },
	{ "*own", 161, dm_own_ply },
	{ "*dumpio", 162, dm_dump_io },
	{ "*jail", 163, dm_jail },
	{ "*afk", 164, dm_afk },
	{ "*mstat", 165, dm_mstat },
	{ "*ostat", 166, dm_ostat },
	{ "*tdump", 167, dm_dump_db },
	{ "*war", 168, guild_war },
	{ "*welcome", 169, welcome_edit },
	{ "*shop", 170, dm_reset_shop },
	{ "*stolen", 171, dm_stolen },
	{ "push", -2, 0 },
	{ "press", -2, 0 },
	{ "@", 0, 0 }
};

char article[][10] = {
	"the",
	"from",
	"to",
	"with",
	"an",
	"in",
	"for",
	"@"
};




struct osp_t ospell[] = {
	/*
	int	splno;
	char	realm;
	int	mp;
	int	ndice;
	int	sdice;
	int	pdice;
	char	bonus_type;
	int	intcast;
	*/
	{ SHURTS,  WIND,  3, 1, 8,  0, 1, 1},	/* hurt */
	{ SRUMBL, EARTH,  3, 1, 8,  0, 1, 1},	/* rumble */
	{ SBURNS,  FIRE,  3, 1, 7,  1, 1, 1},	/* burn */
	{ SBLIST, WATER,  3, 1, 8,  0, 1, 1},	/* blister */

	{ SDUSTG,  WIND,  7, 2, 5,  7, 2, 4},	/* dustgust */
	{ SCRUSH, EARTH,  7, 2, 5,  7, 2, 4},	/* stonecrush */
	{ SFIREB,  FIRE,  7, 2, 5,  8, 2, 4},	/* fireball */
	{ SWBOLT, WATER,  7, 2, 5,  8, 2, 4},	/* waterbolt */

	{ SSHOCK,  WIND, 10, 2, 5, 13, 2, 7},	/* shockbolt */
	{ SENGUL, EARTH, 10, 2, 5, 13, 2, 7},	/* engulf */
	{ SBURST,  FIRE, 10, 2, 5, 13, 2, 7},	/* burstflame */
	{ SSTEAM, WATER, 10, 2, 5, 13, 2, 7},	/* steamblast */

	{ SLGHTN,  WIND, 15, 3, 4, 18, 3, 10},	/* lightning */
	{ SSHATT, EARTH, 15, 3, 4, 19, 3, 10},	/* shatterstone */
	{ SIMMOL,  FIRE, 15, 3, 4, 18, 3, 10},	/* immolate */
	{ SBLOOD, WATER, 15, 3, 4, 18, 3, 10},	/* bloodboil */

	{ STHUND,  WIND, 25, 4, 5, 30, 3, 13},	/* thuderbolt */
	{ SEQUAK, EARTH, 25, 4, 5, 30, 3, 13}, 	/* tremor */
	{ SFLFIL,  FIRE, 25, 4, 5, 30, 3, 13},	/* flamefill */
	{ SICEBL, WATER, 25, 4, 5, 30, 3, 13},	/* iceblade */

	{ STORNA,  WIND, 25, 4, 5, 30, 3, 16},	/* tornado */
	{ STRMOR, EARTH, 25, 4, 5, 30, 3, 16}, 	/* tremor */
	{ SINCIN,  FIRE, 25, 4, 5, 30, 3, 16},	/* incinerate */
	{ SFLOOD, WATER, 25, 4, 5, 30, 3, 16},	/* flood */

	{ -1, 0, 0, 0, 0, 0, 0, 0}
};

/*
	*** IMPORTANT NOTE REGARDING MONK HITTING ***
	These monk dice arrays are obsoleted by the new hitting formula
	in command5.c.
*/
#ifdef OLD_MONK_DICE

/* monk leveling code */
DICE monk_dice[26] = 
{
	{ 1, 3, 0 },
	{ 1, 3, 0 },
	{ 1, 5, 0 },
	{ 1, 5, 1 },
	{ 1, 6, 0 },
	{ 1, 6, 1 },
	{ 1, 6, 2 },
	{ 2, 3, 1 },
	{ 2, 4, 0 },
	{ 2, 4, 1 },
	{ 2, 5, 0 },
	{ 2, 5, 2 },
	{ 2, 6, 1 },
	{ 2, 6, 2 },
	{ 3, 6, 1 },
	{ 3, 7, 1 },
	{ 4, 7, 1 },
	{ 5, 7, 0 },
	{ 5, 8, 1 },
	{ 6, 7, 0 },
	{ 6, 7, 2 },
	{ 6, 8, 0 },
	{ 6, 8, 2 },
	{ 6, 9, 0 },
	{ 6, 9, 2 },
	{ 6, 10, 0 }
};

/* the original code had a 6, 8, 2 for level 24 */
/* it appears that it should be 6, 9, 2 so I changed it */

#else

DICE monk_dice[26] = 
{
	{ 1, 3, 0 },
	{ 1, 3, 0 },
	{ 1, 5, 0 },
	{ 2, 3, 0 },
	{ 1, 6, 1 },
	{ 2, 3, 1 },
	{ 1, 6, 2 },
	{ 2, 4, 1 },
	{ 1, 8, 2 },
	{ 2, 5, 1 },
	{ 4, 3, 0 },
	{ 3, 4, 1 },
	{ 2, 6, 2 },
	{ 1, 12, 3 },
	{ 3, 5, 1 },
	{ 3, 5, 2 },
	{ 4, 5, 1 },
	{ 4, 5, 2 },
	{ 4, 5, 3 },
	{ 5, 5, 1 },
	{ 5, 5, 2 },
	{ 5, 5, 3 },
	{ 6, 5, 1 },
	{ 6, 5, 2 },
	{ 6, 5, 3 },
	{ 6, 6, 2 }
};


#endif

/* This is for creating monster accoding to the monster guide */
struct {
	int 	stats;
	short 	hp;
	char	armor;
	char	thaco;
	long	experience;
	short	ndice;
	short	sdice;
	short	pdice;
} monster_guide[]= {
{55,10,90,19,10,1,4,1},
{58,20,80,18,15,1,4,2},
{61,30,70,17,35,2,3,1},
{65,45,60,16,50,2,4,0},
{68,60,50,15,65,2,4,2},
{71,75,40,14,100,2,6,0},
{75,90,30,13,140,2,6,2},
{78,105,20,12,200,2,7,2},
{81,120,10,11,250,2,8,2},
{85,135,0,10,350,3,6,2},
{88,150,-5,9,450,2,10,2},
{91,165,-10,8,550,3,8,0},
{95,180,-15,7,700,3,8,2},
{98,195,-20,6,850,3,9,1},
{101,210,-25,5,1000,3,9,3},
{105,225,-30,4,1150,3,10,3},
{108,240,-35,3,1300,6,6,0},
{111,255,-40,2,1450,6,6,4},
{115,270,-45,1,1600,8,5,5},
{118,285,-50,0,1800,9,5,5},
{121,300,-60,-2,2200,10,5,4},
{122,350,-70,-4,2600,11,5,3},
{123,400,-80,-6,3200,12,5,3},
{124,450,-90,-8,4000,13,5,3},
{125,500,-100,-10,5000,14,6,3}
};


/* This is to set default enemylists for monters */

int ENMLIST=7; /* set this to length of monster_enmlist array */

struct {
	char *attacker;
	char *target;
} monster_enmlist[] = {
{ "orc", "militiaman" },
{ "thief", "merchant" },
{ "thief", "nobleman" },
{ "goblin", "militiaman" },
{ "large goblin", "militiaman" },
{ "goblin warrior", "militiaman" },
{ "drunk", "hobbit" }
};
