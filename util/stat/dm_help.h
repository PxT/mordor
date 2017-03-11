#define PATH "~/mordor/mordor/help"  /* location of text files */
#define BUFSIZE 2048                     /* file readin buffer */
#define MAXENTRY 15                     /* maxium number of menu entries */
#define TSUBMENU 5                      /* total number of submenus */
#define MARGL 25                        /* left margin for menu display */
#define MARGT 5                         /* top margin for menu display */

typedef struct {
    char *desc;
    char *fname;
} menu_t;

menu_t  main_menu[] = {
    {"General Mordor Information", "dm_intro"},
    {"How to create a monster", "SUBMENU0"},
    {"How to create a room", "SUBMENU1"},
    {"How to create an item", "SUBMENU2"},
    {"DM commands", "dm_cmnd"},
    {"Utility Companion programs", "SUBMENU3"},
    {"Miscellaneous", "SUBMENU4"},
    {NULL,NULL}
    };

menu_t  sub_menu[TSUBMENU][MAXENTRY] = { 
/* submenu 0 */
     {{"Creating a creature", "crt_make"},
      {"Creature flags","crt_flag"},
      {"Creature experience guide", "crt_expg"},
      {"Interactive talk monsters", "crt_talk"},
      {"Player flags","ply_flag"},
      {NULL,NULL}
    },

/* submenu 1 */
    {{"Creating a room","rom_make"},
     {"Room flags","rom_flag"}, 
     {"Creating a player storage room","rom_stor"},
     {"Exit flags","rom_xflg"},
     {"Combination locks","rom_comb"},
     {NULL,NULL}
    },

/* submenu 2 */
    {{"Creating an object","obj_make"},
     {"Object flags","obj_flag"},
     {NULL,NULL}
    },

/* submenu 3 */
    {{"gstat", "cmd_gsta"},
     {"italk", "cmd_ital"},
     {"list","cmd_list"},
     {"plist", "cmd_plis"},
     {"psearch", "cmd_psea"},
     {"startup", "cmd_star"},
     {"tdel", "cmd_tdel"},
     {NULL,NULL}
    },

/* submenu 4 */
    {{"Compiling Mordor","misc_make"},
     {"Locking out sites","misc_lock"},
     {"Double playing lock","misc_doub"},
     {"Mordor Game Policy","misc_policy"},
     {"Modifing Mordor uptime schedule","misc_schd"},
     {"Adding new menu options", "misc_menu"},
     {NULL,NULL}
    }
};
