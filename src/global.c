/*
 * GLOBAL.C:
 *
 *	Global variables.
 *
 *	Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#define MIGNORE
#include "mextern.h"

int		Tablesize;
int		Cmdnum;
long		Time;
struct lasttime	Shutdown;
int		Spy[PMAX];
int		Numlockedout;
lockout		*Lockout;

struct {
	creature	*ply;
	iobuf		*io;
	extra		*extr;
} Ply[PMAX];

struct {
	short		hpstart;
	short		mpstart;
	short		hp;
	short		mp;
	short		ndice;
	short		sdice;
	short		pdice;
} class_stats[11] = {
	{  0,  0,  0,  0,  0,  0,  0},
	{ 19,  2,  6,  2,  1,  6,  0},	/* assassin */
	{ 24,  1,  8,  1,  1,  3,  1},	/* barbarian */
	{ 16,  4,  5,  4,  1,  4,  0},	/* cleric */
	{ 22,  2,  7,  2,  1,  5,  0},	/* fighter */
	{ 14,  5,  4,  5,  1,  3,  0},	/* mage */
	{ 19,  3,  6,  3,  1,  4,  0},	/* paladin */
	{ 18,  3,  6,  3,  2,  2,  0},	/* ranger */
	{ 18,  3,  5,  2,  2,  2,  1},	/* thief */
	{ 30, 30, 10, 10,  5,  5,  5},	/* caretaker */
	{ 30, 30, 10, 10,  5,  5,  5}	/* DM */
};

int bonus[35] = { -4, -4, -4, -3, -3, -2, -2, -1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		  2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

char class_str[][15] = { "None", "Assassin", "Barbarian", "Cleric",
	"Fighter", "Mage", "Paladin", "Ranger", "Thief", "Caretaker",
	"Dungeonmaster" };

char race_str[][15] = { "Unknown", "Dwarf", "Elf", "Half-elf", "Halfling",
	"Human", "Orc", "Half-giant", "Gnome" };

char race_adj[][15] = { "Unknown", "Dwarven", "Elven", "Half-elven",
	"Halfling", "Mannish", "Orcish", "Half-giant", "Gnomish" };

short level_cycle[][10] = {
	{ 0,   0,   0,   0,   0,   0,   0,   0,   0,   0   },
	{ CON, PTY, STR, INT, DEX, INT, DEX, PTY, STR, DEX },
	{ INT, DEX, PTY, CON, STR, CON, DEX, STR, PTY, STR },
	{ STR, DEX, CON, PTY, INT, PTY, INT, DEX, CON, INT },
	{ PTY, INT, DEX, CON, STR, CON, INT, STR, DEX, STR },
	{ STR, DEX, PTY, CON, INT, CON, INT, DEX, PTY, INT },
	{ DEX, INT, CON, STR, PTY, STR, INT, PTY, CON, PTY },
	{ PTY, STR, INT, CON, DEX, CON, DEX, STR, INT, DEX },
	{ INT, CON, PTY, STR, DEX, STR, CON, DEX, PTY, DEX },
	{ STR, DEX, INT, CON, PTY, STR, DEX, INT, CON, PTY },
	{ STR, DEX, INT, CON, PTY, STR, DEX, INT, CON, PTY }
};

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
	{  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};

long quest_exp[] = {
	120, 500, 1000, 1000, 1000, 1000, 125, 125,
	1000, 1000, 125, 125, 125, 1000, 2500, 2500,
	2500, 5, 5, 5, 125, 125, 125, 125,
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
 
long needed_exp[] = {
    512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 100000,
    146410, 207360, 314171, 460992, 658125, 887504, 1202815,
    1699616, 2203457, 2799000, 3505139, 4465120, 5792661, 7319072,
    8874375, 2000000};   

char lev_title[][8][20] = {
	{ "", "", "", "", "", "", "", "" },
	{ "Minor Assassin", "Rutterkin", "Killer", "Cutthroat",
	  "Murderer", "Executioner", "Expert Assassin", "Master Assassin" },
	{ "Grunt", "Tribesman", "Savage", "Brutalizer", 
	  "Dragon-slayer", "Barbarian Chief", "Destroyer", "Master Barbarian" },
	{ "Acolyte", "Healer", "Adept", "Priest",
	  "Cardinal", "Lama", "Patriarch", "High Priest" },
	{ "Veteran", "Warrior", "Punisher", "Pugilist",
	  "Myrmidon", "Hero", "Champion", "Lord" },
	{ "Apprentice", "Prestidigitator", "Necromancer", "Occultate",
	  "Thaumaturgist", "Enchanter", "Sorcerer", "Wizard" },
	{ "Gallant", "Protector", "Warder", "Guardian",
	  "Ordained Paladin", "Chevalier", "Justiciar", "Holy Warrior" },
	{ "Searcher", "Scout", "Tracker", "Underhillsman", 
	  "Royal Guide", "Pathfinder", "Ranger Knight", "Ranger Lord" },
	{ "Rogue", "Footpad", "Dervish", "Burglar",
	  "Sharper", "Magsman", "High Thief", "Master Thief" },
	{ "Bumbling Idiot", "Immortal", "Slave", "Fool",
	  "Addict", "Egomaniac", "Laborer", "Caretaker" },
	{ "Bumbling Idiot", "Egomaniac", "Computer Nerd", "Dungeon Lord", 
	  "Arch Occultate", "Divine Entity", "Addict", "Dungeonmaster"  }
};

struct {
	char	*cmdstr;
	int	cmdno;
	int	(*cmdfn)();
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
	{ "examine", 2, look },
	{ "quit", 3, quit },
	{ "quit", 3, quit },
	{ "goodbye", 3, quit },
	{ "goodbye", 3, quit },
	{ "say", 4, say },
	{ "\"", 4, say },
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
	{ "send", 17, send },
	{ "sen", 17, send },
	{ "tell", 17, send },
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
	{ "value", 44, value },
	{ "cost", 44, value },
	{ "bs", 45, backstab },
	{ "backstab", 45, backstab },
	{ "train", 46, train },
	{ "give", 47, give },
	{ "repair", 48, repair },
	{ "fix", 48, repair },
	{ "time", 49, prt_time },
	{ "circle", 50, circle },
	{ "bash", 51, bash },
	{ "save", 52, savegame },
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
	{ "vote",79,vote},
	{ "finger", 80, pfinger},
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
	{ "warm", 100, action},
	{ "kick", 100, action },
	{ "tackle", 100, action },
	{ "knee", 100, action },
	{ "pounce", 100, action },
	{ "tickle", 100, action },
	{ "snicker", 100, action },
	{ "tap", 100, action },
	{ "smile", 100, action },
	{ "beam", 100, action},
	{ "masterbate", 100, action},
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
	{ "satisfied", 100, action },
	{ "wince", 100, action },
	{ "roll", 100, action },
	{ "raise", 100, action },
	{ "whine", 100, action },
	{ "growl", 100, action },
	{ "high5", 100, action },
	{ "moon", 100, action },
	{ "purr", 100, action },
	{ "taunt", 100, action },
	{ "eye", 100, action },
	{ "worship", 100, action },
	{ "flip", 100, action },
	{ "groan", 100, action },
	{ "*teleport", 101, dm_teleport },
	{ "*rm", 102, dm_rmstat },
	{ "*reload", 103, dm_reload_rom },
	{ "*save", 104, dm_resave },
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
	{ "*shutdown", 114, dm_shutdown },
	{ "*f", 115, dm_force },
	{ "*force", 115, dm_force },
	{ "*flushcrtobj", 116, dm_flush_crtobj },
	{ "*monster", 117, dm_create_crt },
	{ "*status", 118, dm_stat },
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
	{ "*cfollow", 142, dm_follow },
	{ "*dmhelp", 143, dm_help },
	{ "*attack", 144, dm_attack },
	{ "*enemy", 145, list_enm },
	{ "*charm", 146, list_charm },
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

struct {
	char 	*splstr;
	int	splno;
	int	(*splfn)();
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
	{ "resist-cold", SRCOLD, resist_cold},
	{ "breathe-water", SBRWAT, breathe_water},
	{ "earth-shield", SSSHLD, earth_shield},
	{ "clairvoyance", SLOCAT, locate_player},
	{ "drain-exp", SDREXP, drain_exp},
	{ "remove-disease", SRMDIS, rm_disease},
	{ "cure-blindness", SRMBLD, rm_blind},
	{ "fear", SFEARS, fear}, 
	{ "room-vigor", SRVIGO, room_vigor}, 
	{ "transport", STRANO, object_send},
	{ "blind", SBLIND, blind},
	{ "silence", SSILNC, silence},
	{ "charm", SCHARM, charm},
	{ "@", -1,0 }
};

struct osp_t ospell[] = {
	{ SHURTS,  WIND,  3, 1, 8,  0, 1 },	/* hurt */
	{ SRUMBL, EARTH,  3, 1, 8,  0, 1 },	/* rumble */
	{ SBURNS,  FIRE,  3, 1, 7,  1, 1 },	/* burn */
	{ SBLIST, WATER,  3, 1, 8,  0, 1 },	/* blister */

	{ SDUSTG,  WIND,  7, 2, 5,  7, 2 },	/* dustgust */
	{ SCRUSH, EARTH,  7, 2, 5,  7, 2 },	/* stonecrush */
	{ SFIREB,  FIRE,  7, 2, 5,  8, 2 },	/* fireball */
	{ SWBOLT, WATER,  7, 2, 5,  8, 2 },	/* waterbolt */

	{ SSHOCK,  WIND, 10, 2, 5, 13, 2 },	/* shockbolt */
	{ SENGUL, EARTH, 10, 2, 5, 13, 2 },	/* engulf */
	{ SBURST,  FIRE, 10, 2, 5, 13, 2 },	/* burstflame */
	{ SSTEAM, WATER, 10, 2, 5, 13, 2 },	/* steamblast */

	{ SLGHTN,  WIND, 15, 3, 4, 18, 3 },	/* lightning */
	{ SSHATT, EARTH, 15, 3, 4, 19, 3 },	/* shatterstone */
	{ SIMMOL,  FIRE, 15, 3, 4, 18, 3 },	/* immolate */
	{ SBLOOD, WATER, 15, 3, 4, 18, 3 },	/* bloodboil */

	{ STHUND,  WIND, 25, 4, 5, 30, 3 },	/* thuderbolt */
	{ SEQUAK, EARTH, 25, 4, 5, 30, 3 }, 	/* earthquake */
	{ SFLFIL,  FIRE, 25, 4, 5, 30, 3 },	/* flamefill */
	{ SICEBL, WATER, 25, 4, 5, 30, 3 },	/* iceblade */
	{ -1, 0, 0, 0, 0, 0, 0 }
};

char number[][10] = {
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
