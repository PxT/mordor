extern int	psearch();
extern int	offensive_spell(), vigor(), curepoison(), light();
extern int	protection(), mend();
extern int	bless(), invisibility(), restore(), turn(), befuddle();
extern int	detectinvis(), detectmagic(), teleport(), enchant();
extern int	recall(), summon(), heal(), magictrack(), levitate(),
		resist_fire(), fly();
extern int	resist_magic(), know_alignment(), remove_curse();
extern int	resist_cold(), breathe_water(), earth_shield(),
		locate_player(), drain_exp (), rm_disease(), object_send();
extern int	room_vigor(), fear(), rm_blind(), silence(), blind(),
		spell_fail(), fortune(); 

extern struct {
        char    	*splstr;
        int             splno;
        int             (*splfn)();
} spllist[]; 

/*
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
	{ "resist-cold", SRCOLD, resist_cold },
	{ "breathe-water", SBRWAT, breathe_water },
	{ "earth-shield", SSSHLD, earth_shield },
	{ "clairvoyance", SLOCAT, locate_player },
	{ "drain-exp", SDREXP, drain_exp },
	{ "remove-disease", SRMDIS, rm_disease },
	{ "cure-blindness", SRMBLD, rm_blind },
	{ "fear", SFEARS, fear }, 
	{ "room-vigor", SRVIGO, room_vigor }, 
	{ "transport", STRANO, object_send },
	{ "blind", SBLIND, blind },
	{ "silence", SSILNC, silence },
	{ "fortune", SFORTU, fortune },
	{ "@", -1,0 }
};

*/
