/* Modify this as necessary to reflect your new struct */
/*
 * $Id$
 *
 * $Log$
 */
/* define SAVE_ALL to 0 if you want all items in a room/creature to be saved
 * define to 1 if you want only perm items saved 
 * yeah, its counterintuitive, but i misnamed it and now i dont
 * want to fix it.  :)   */
#define SAVE_ALL 0


typedef struct n_ext_tag {                /* Exit list tags */
        struct n_ext_tag  *next_tag;
        struct n_exit_    *ext;
} n_xtag;


typedef struct n_obj_tag {                /* Object list tags */
        struct n_obj_tag  *next_tag;
        struct n_object   *obj;
} n_otag;

typedef struct n_crt_tag {                /* Creature list tags */
        struct n_crt_tag  *next_tag;
        struct n_creature *crt;
} n_ctag;

typedef struct n_rom_tag {                /* Room list tags */
        struct n_rom_tag  *next_tag;
        struct n_room     *rom;
} n_rtag;

typedef struct n_exit_ {
        char            name[20];
        short           room;
        char            flags[8];
        struct          lasttime        ltime;          /* Timed open/close */
	struct		lasttime	random;		/* randomness */
	char            rand_mesg[2][80]; 		/* message for random */
        unsigned char key;              /* Key required */
} n_exit_;

typedef struct n_creature {
        char            name[80];
        char            description[80];
        char            talk[80];
        char            password[15];
        char            key[3][20];
        short           fd;             /* Socket number */
        char            level;
        char            type;           /* Creature type */
        char            class;
        char            race;
        char            numwander;
        short           alignment;
        char            strength;
        char            dexterity;
        char            constitution;
        char            intelligence;
        char            piety;
        short           hpmax;          /* Max/Current hp and mp */
        short           hpcur;
        short           mpmax;
        short           mpcur;
        char            armor;          /* Armor Class */
        char            thaco;          /* Thac0 */
        long            experience;
        long            gold;
        short           ndice;          /* Bare-handed damage */
        short           sdice;
        short           pdice;
        short           special;
                                        /* extra prof. for hand */
        long            proficiency[6]; /* Weapon proficiencies */
                                        /* increase realms */
        long            realm[8];       /* Magic Spell realms */
        char            spells[16];     /* Known spells */
        char            flags[16];
        char            quests[16];     /* Quests fulfilled (P) */
#define                 NUMHITS quests[0]
        char            questnum;       /* Quest fulfillment number (M) */
        short           carry[10];      /* Random items monster carries */
#define                 WIMPYVALUE carry[0]
        short           rom_num;
        struct n_object   *ready[MAXWEAR];/* Worn/readied items */
                                        /* More Dailys and lasttimes */
        struct daily    daily[20];      /* Daily use variables */
        struct lasttime lasttime[65];   /* Last-time-used variables */
        struct n_creature *following;     /* Creature being followed */
        n_ctag            *first_fol;     /* List of followers */
        n_otag            *first_obj;     /* List of inventory */
        etag            *first_enm;     /* List of enemies */
        ttag            *first_tlk;     /* List of talk responses */
        struct n_room     *parent_rom;    /* Room creature is in */
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
} n_creature;

typedef struct n_room {
        short           rom_num;
        char            name[80];
        char            *short_desc;    /* Descriptions */
        char            *long_desc;
        char            *obj_desc;
        char            lolevel;        /* Lowest level allowed in */
        char            hilevel;        /* Highest level allowed in */
        short           special;
        char            trap;
        short           trapexit;
        char            track[80];      /* Track exit */
        char            flags[16];
        short           random[10];     /* Random monsters */
        char            traffic;        /* R. monster traffic */
        struct lasttime perm_mon[10];   /* Permanent/reappearing monsters */
        struct lasttime perm_obj[10];   /* Permanent/reappearing items */
        long            beenhere;       /* # times room visited */
        time_t          established;    /* Time room was established */
        xtag            *first_ext;     /* Exits */
        n_otag            *first_obj;     /* Items */
        n_ctag            *first_mon;     /* Monsters */
        n_ctag            *first_ply;     /* Players */
                                        /* NEW here down */
        short           death_rom;      /* This room's limbo */
        char            zone_type;      /* Define a zone */
        char            zone_subtype;   /* and a subzone */
        char            env_type;       /* and an environment */
	short		special1;	/* reserved for future use */
	long		special2;	
} n_room;

typedef struct n_object {
        char            name[80];
        char            description[80];
        char            key[3][20];     /* key 3 hold object index */
        char            use_output[80]; /* String output by successful use */
        long            value;
        short           weight;
        char            type;
        char            adjustment;
        short           shotsmax;       /* Shots before breakage */
        short           shotscur;
        short           ndice;          /* Number/Sides/Plus dice */
        short           sdice;
        short           pdice;
        char            armor;          /* AC adjustment */
        char            wearflag;       /* Where/If it can be worm */
        char            magicpower;
        char            magicrealm;
        short           special;
        char            flags[16];
        char            questnum;       /* Quest fulfillment number */
        n_otag            *first_obj;     /* Objects contained inside */
        struct n_object   *parent_obj;    /* Object this is in */
        struct n_room     *parent_rom;    /* Room this is in */
        struct n_creature *parent_crt;    /* Creature this is in */
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
} n_object;




int conv_ply();
int conv_obj();
int conv_rom();
int conv_crt();
int copy_rom();
int copy_ply();
int copy_obj();
int save_ply_to_file2();
int compress();
