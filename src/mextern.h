/*
 * MEXTERN.H:
 *
 *	This file contains the external function and variable 
 *	declarations required by the rest of the program.
 *
 *	Copyright (C) 1991, 1992, 1993 Brooke Paul
 *
 * $Id: mextern.h,v 6.27 2001/07/29 22:11:02 develop Exp $
 *
 * $Log: mextern.h,v $
 * Revision 6.27  2001/07/29 22:11:02  develop
 * added shop_cost
 *
 * Revision 6.26  2001/07/25 02:55:04  develop
 * added duplicate_object for thief dropping stolen items
 *
 * Revision 6.25  2001/07/22 20:05:52  develop
 * gold theft changes
 *
 * Revision 6.24  2001/07/22 19:03:06  develop
 * first run at alllowing thieves to steal gold from other players
 *
 * Revision 6.23  2001/07/18 01:43:24  develop
 * changed cost of stores
 *
 * Revision 6.22  2001/07/17 19:28:44  develop
 * Added new bank functions
 *
 * Revision 6.23  2001/07/15 05:42:50  develop
 * set_rom_owner setup_shop created
 *
 * Revision 6.22  2001/07/14 21:26:44  develop
 * *** empty log message ***
 *
 * Revision 6.21  2001/07/04 00:13:17  develop
 * testing count_vis_ply_and_cnj
 *
 * Revision 6.20  2001/06/30 01:58:34  develop
 * added ENMLIST
 *
 * Revision 6.19  2001/06/29 03:22:12  develop
 * added struct for default enemy lists
 *
 * Revision 6.18  2001/06/06 19:36:46  develop
 * conjure added
 *
 * Revision 6.17  2001/04/29 02:30:26  develop
 * added check_proxies
 *
 * Revision 6.16  2001/04/23 03:52:29  develop
 * added NOCREATE flag to toggle character creation
 *
 * Revision 6.15  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

/* Options */


#include <stdio.h>
#include <assert.h>

#include "../include/port.h"
#include "../include/morutil.h"

#ifdef DMALLOC
  #include "/usr/local/include/dmalloc.h"
#endif

#include "mstruct.h"


#ifndef MIGNORE


/* configurable */
extern int		ANSILINE;	/* Ansi Bottom line ON/OFF */
extern int		AUTOSHUTDOWN; 	/* automatic shutdown */
extern int		CHECKDOUBLE; 	/* check for double playing */
extern int		EATNDRINK;	/* Food and Drink ON/OFF */
extern int		GETHOSTBYNAME; 	/* Enable gethostbyname() call */
extern int		HEAVEN;		/* Heaven settings */
extern int		ISENGARD;	/* Isengard settings */
extern int		LASTCOMMAND; 	/* Display last command who output */
extern int		GUILDEXP;	/* Experience for guild activities */
extern int		PARANOID;	/* Block non RFC1413 accounts */
extern int		RECORD_ALL;	/* Log all commands */
extern int		RFC1413;	/* limit non RFC1413 accounts */
extern int		SECURE;		/* disallow multi login on RFC1413 */
extern int		SCHED;		/* Runs schedule_g() */
extern int		SUICIDE;	/* log suicides */

extern unsigned short	PORTNUM;	/* duh... */
extern int		CRASHTRAP;	/* Enable crashtrapping with signals */
extern int		SAVEONDROP;	/* Save char. on 'drop' command */
extern int		NOBULLYS;	/* Impose level differentials for pkill */
extern int		MSP;		/* Mud Sound Protocol */
extern int		NOCREATE;	/* no new character creation allowed */

extern int    PROMPTCOLOR;
extern int    CREATURECOLOR;
extern int    BROADCASTCOLOR;
extern int    MAINTEXTCOLOR;
extern int    PLAYERCOLOR;
extern int    LOGONOFFCOLOR;
extern int    WEATHERCOLOR;
extern int    TITLECOLOR;
extern int    BLINDCOLOR;
extern int	  SILENCECOLOR;	
extern int    ECHOCOLOR;
extern int    TALKCOLOR;
extern int    ITEMCOLOR;
extern int    EXITCOLOR;
extern int	  ROOMNAMECOLOR;
extern int	  ERRORCOLOR;

extern int    ATTACKCOLOR;
extern int    MISSEDCOLOR;
extern int    TRAPCOLOR;
extern int    POISONCOLOR;
extern int    DISEASECOLOR;
extern int    FROSTCOLOR;


extern char		dmname[][10];

extern char		title[80];
extern char		auth_questions_email[80];
extern char		questions_to_email[80];
extern char		register_questions_email[80];
extern char		account_exists[80];
extern char		dm_pass[20];

/* weather strings */
extern char		sunrise[80];
extern char		sunset[80];
extern char		earth_trembles[80];
extern char		heavy_fog[80];
extern char		beautiful_day[80];
extern char		bright_sun[80];
extern char		glaring_sun[80];
extern char		heat[80];
extern char		still[80];
extern char		light_breeze[80];
extern char		strong_wind[80];
extern char		wind_gusts[80];
extern char		gale_force[80];
extern char		clear_skies[80];
extern char		light_clouds[80];
extern char		thunderheads[80];
extern char		light_rain[80];
extern char		heavy_rain[80];
extern char		sheets_rain[80];
extern char		torrent_rain[80];
extern char		no_moon[80];
extern char		sliver_moon[80];
extern char		half_moon[80];
extern char		waxing_moon[80];
extern char		full_moon[80];

/* Boat sailing strings */
extern char		tx_mesg1[80];
extern char		tx_mesg2[80];



extern int start_room_num;
extern int death_room_num;
extern int jail_room_num;
extern int pkill_in_battle;
/* initial eq for new characters */
extern int init_eq[5];
/* outcast trophy item number */
extern int trophy_num;
/* cost of a player shop */
extern long shop_cost;

extern time_t	dm_timeout;
extern time_t	ct_timeout;
extern time_t	bld_timeout;
extern time_t	ply_timeout;

/* end configurable */


extern char		g_buffer[1024];
extern int		Tablesize;
extern int		Cmdnum;
extern long		Time;
extern time_t	StartTime;
extern struct lasttime	Shutdown;
extern struct lasttime	Guildwar;
extern struct lasttime  Weather[4];
extern int		Numlockedout;
extern lockout		*Lockout;
extern short	Random_update_interval;
extern int	bHavePort;

extern plystruct Ply[PMAX];
extern class_stats_struct class_stats[CLASS_COUNT];


extern struct cmdstruct cmdlist[];
extern struct creature_template monster_guide[];

extern int ENMLIST;
extern struct enemy_list monster_enmlist[];


extern osp_t ospell[];


extern guild cur_guilds[10];

extern short	level_cycle[][10];
extern short	thaco_list[][20];
extern int		bonus[35];
extern char		lev_title[][8][20];
extern char 	article[][10];
extern long		needed_exp[25];
extern time_t	last_dust_output;


extern DICE	monk_dice[26];

extern char ply_cond_desc[][50];
extern char obj_cond_desc[][60];
extern char dam_desc[][20];
extern char guilds[][30];

#endif


typedef int (*SPELLFN)();




/* ACCESS.C */
extern char *get_spell_name( int nIndex );
extern int get_spell_num( int nIndex );
extern SPELLFN get_spell_function( int nIndex );
extern int get_spell_list_size();
extern char *int_to_text( int nNumber );
extern long get_quest_exp( int nQuest );
extern int get_spy( int id );
extern void set_spy( int id, int fd );
extern void clear_spy();


/* ACTION.C */

extern int action(creature *ply_ptr, cmd *cmnd );


/* BANK.C */
extern int bank_balance( creature *ply_ptr, cmd *cmnd );
extern int bank_withdrawl( creature *ply_ptr, cmd *cmnd );
extern int bank_deposit( creature *ply_ptr, cmd *cmnd);
extern int transfer_balance( creature *ply_ptr, cmd *cmnd );
extern long net_worth(creature *ply_ptr );
extern int bank_account_modify(creature *ply_ptr, long amount);


/* COMBAT.C */
extern int update_combat( creature *crt_ptr );
extern int check_for_ply_flee(creature * ply_ptr);
extern int bw_spit_acid(creature *crt_ptr, creature *vic_ptr );
extern int bw_poison(creature *crt_ptr, creature *vic_ptr );
extern int bw_cone_frost( creature *crt_ptr, creature *vic_ptr );
extern int bw_cone_fire( creature *crt_ptr, creature *vic_ptr );
extern int power_energy_drain( creature *crt_ptr, creature *vic_ptr);
extern int power_curse( creature *crt_ptr, creature *vic_ptr);
extern int pkill_allowed(creature *ply_ptr, creature *crt_ptr );
extern void break_weapon(creature *ply_ptr);
extern void attack_with_weapon( creature * ply_ptr );
extern char *hit_description(int damage);

/* COMMAND1.C */
extern void login(int 	fd, int	param, char	*str );
extern void create_ply(int	fd, int	param, char	*str );
extern void command(int fd, int param, char *str );
extern void parse(char	*str, cmd *cmnd );
extern int process_cmd(int	fd, cmd	*cmnd );
extern int checkdouble( char *name );
extern void check_double_login(int fd);
extern int contains_bad_word(char *str);

/* COMMAND2.C */

extern int look(creature *ply_ptr, cmd	*cmnd );
extern char *crt_condition(creature *crt_ptr);
extern char *obj_condition(object *obj_ptr);
extern int move(creature *ply_ptr, cmd *cmnd );
extern int say( creature *ply_ptr, cmd *cmnd );
extern int get(creature *ply_ptr, cmd *cmnd );
extern void get_all_rom( creature *ply_ptr );
extern void get_all_obj(creature *ply_ptr, object *cnt_ptr );
extern void get_perm_obj( object *obj_ptr );
extern int about( creature *ply_ptr, cmd *cmnd );
extern int inventory( creature *ply_ptr, cmd *cmnd );
extern int drop(creature *ply_ptr, cmd	*cmnd );
extern void drop_all_rom( creature	*ply_ptr );
extern void drop_all_obj(creature *ply_ptr, object	*cnt_ptr );



/* COMMAND3.C */
extern int wear(creature *ply_ptr, cmd *cmnd );
extern void wear_all( creature *ply_ptr );
extern int remove_obj( creature *ply_ptr, cmd *cmnd );
extern void remove_all( creature *ply_ptr );
extern int equipment(creature *crt_ptr, cmd *cmnd );
extern void equip_list(int	fd, creature *crt_ptr );
extern int ready(creature *ply_ptr, cmd *cmnd );
extern int hold(creature *ply_ptr, cmd	*cmnd );


/* COMMAND4.C */
extern int health( creature *ply_ptr, cmd *cmnd );
extern int help( creature *ply_ptr, cmd *cmnd );
extern int welcome( creature *ply_ptr, cmd *cmnd );
extern int info( creature *ply_ptr, cmd *cmnd );
extern void info_2(int	fd, int param, char *instr );
extern void show_spells( int fd, creature *ply_ptr );
extern int psend( creature	*ply_ptr, cmd *cmnd );
extern int broadsend(creature *ply_ptr, cmd *cmnd );
extern int follow( creature *ply_ptr, cmd *cmnd );
extern int lose( creature *ply_ptr, cmd *cmnd );
extern int group( creature	*ply_ptr, cmd *cmnd );
extern int track( creature *ply_ptr, cmd *cmnd );
extern int peek( creature *ply_ptr, cmd *cmnd );


/* COMMAND5.C */
extern int attack(creature *ply_ptr, cmd *cmnd );
extern int attack_crt(creature *ply_ptr, creature *crt_ptr );
extern int who( creature *ply_ptr, cmd *cmnd );
extern int whois(creature *ply_ptr, cmd *cmnd );
extern int search(creature *ply_ptr, cmd *cmnd );
extern int ply_suicide(creature *ply_ptr, cmd *cmnd );
extern void suicide(int fd, int param, char *str );
extern int hide(creature *ply_ptr, cmd *cmnd );
extern int flag_list(creature *ply_ptr, cmd *cmnd );
extern int set( creature *ply_ptr, cmd *cmnd );
extern int clear(creature *ply_ptr, cmd *cmnd );
extern int quit(creature *ply_ptr, cmd *cmnd );


/* COMMAND6.C */
extern int yell(creature *ply_ptr, cmd	*cmnd );
extern int go( creature *ply_ptr, cmd *cmnd );
extern int openexit( creature *ply_ptr, cmd *cmnd );
extern int closeexit( creature *ply_ptr, cmd *cmnd );
extern int unlock( creature *ply_ptr, cmd *cmnd );
extern int lock( creature *ply_ptr, cmd *cmnd );
extern int picklock(creature *ply_ptr, cmd *cmnd );
extern int steal(creature *ply_ptr, cmd *cmnd );
extern int steal_money(creature *ply_ptr, cmd *cmnd);
extern int theft_chance(creature *ply_ptr, creature *crt_ptr);

/* COMMAND7.C */
extern int flee( creature *ply_ptr, cmd *cmnd );
extern int list( creature *ply_ptr, cmd *cmnd );
extern int buy(creature *ply_ptr, cmd *cmnd );
extern int sell( creature *ply_ptr, cmd *cmnd );
extern int appraise( creature *ply_ptr, cmd *cmnd );
extern int backstab( creature *ply_ptr, cmd *cmnd );
extern int train(creature *ply_ptr, cmd *cmnd );
extern void courageous( creature *ply_ptr );


/* COMMAND8.C */
extern int give( creature *ply_ptr, cmd *cmnd );
extern void give_money( creature *ply_ptr, cmd *cmnd );
extern int repair( creature *ply_ptr, cmd *cmnd );
extern int prt_time( creature *ply_ptr, cmd *cmnd );
extern int circle( creature *ply_ptr, cmd *cmnd );
extern int bash( creature *ply_ptr, cmd *cmnd );
extern int save_ply_cmd( creature *ply_ptr, cmd *cmnd );
extern int save_ply( creature *ply_ptr );
extern int talk( creature *ply_ptr, cmd *cmnd );
extern void talk_action( creature *ply_ptr, creature *crt_ptr, ttag *tt );


/* COMMAND9.C */
extern int bribe(creature *ply_ptr, cmd *cmnd );
extern int haste( creature *ply_ptr, cmd *cmnd );
extern int berserk( creature *ply_ptr, cmd *cmnd );
extern int barkskin( creature *ply_ptr, cmd *cmnd );
extern int pray( creature *ply_ptr, cmd *cmnd );
extern int prepare( creature *ply_ptr, cmd	*cmnd );
extern int use( creature *ply_ptr, cmd *cmnd );
extern int ignore( creature *ply_ptr, cmd *cmnd );


/* COMMAND10.C */
extern int sneak( creature *ply_ptr, cmd *cmnd);
extern int gtalk( creature *ply_ptr, cmd *cmnd );
extern void lower_prof( creature *ply_ptr, long exp );
extern void add_prof( creature *ply_ptr, long exp );
//extern int pledge( creature *ply_ptr, cmd *cmnd );
//extern int rescind( creature *ply_ptr, cmd *cmnd );
extern void lose_all( creature *ply_ptr );
extern void dissolve_item (creature *ply_ptr, creature *crt_ptr);
extern int purchase(creature *ply_ptr, cmd *cmnd );
extern int selection(creature *ply_ptr, cmd *cmnd);
extern int trade( creature *ply_ptr, cmd *cmnd );
extern int room_obj_count( otag *first_obj );


/* COMMAND11.C */ 
extern int emote( creature *ply_ptr, cmd *cmnd );
extern int passwd (creature *ply_ptr, cmd *cmnd );   
extern void chpasswd(int fd, int param, char *str );
extern int vote (creature *ply_ptr, cmd *cmnd );
extern void vote_cmnd(int fd, int param, char *str );
extern int pfinger(creature *ply_ptr, cmd *cmnd );


/* COMMAND12.C */
extern int bard_song( creature *ply_ptr, cmd *cmnd );
extern int bard_song2( creature *ply_ptr, cmd *cmnd );
extern int meditate( creature	*ply_ptr, cmd *cmnd );
extern int touch_of_death( creature *ply_ptr, cmd *cmnd );
extern int uptime(creature *ply_ptr, cmd *cmnd);


/* COMMAND13.C */
extern int channelemote( creature *ply_ptr, cmd *cmnd );
extern int classemote( creature *ply_ptr, cmd *cmnd );
extern int class_send( creature *ply_ptr, cmd *cmnd );
extern int transform( creature *ply_ptr, cmd *cmnd );
extern int transmute( creature *ply_ptr, cmd *cmnd );
extern int apply_herb( creature *ply_ptr, cmd *cmnd );
extern int ingest_herb( creature *ply_ptr, cmd *cmnd );
extern int eat_herb( creature *ply_ptr, cmd *cmnd );
extern int paste_herb( creature *ply_ptr, cmd *cmnd );
extern int use_herb( creature *ply_ptr, object *obj_ptr, int herb_type );
extern int eat( creature *ply_ptr, cmd	*cmnd );
extern int describe_me( creature *ply_ptr, cmd *cmnd );
extern int scout( creature *ply_ptr, cmd *cmnd );

/* COMMAND14.C */
extern int setup_shop(creature *ply_ptr, cmd *cmnd);

/* CREATURE.C */
extern creature *find_crt( creature *ply_ptr, ctag *first_ct, char *str, int val );
extern creature *find_exact_crt( creature *ply_ptr, ctag *first_ct, char *str, int val );
extern creature *find_crt_in_rom( creature *ply_ptr, room *rom_ptr, const char *str, int val, int flag );
extern int add_enm_crt(char *enemy, creature *crt_ptr );
extern int del_enm_crt(char *enemy, creature *crt_ptr );
extern void end_enm_crt(char *enemy, creature *crt_ptr );
extern void add_enm_dmg(char *enemy, creature *crt_ptr, int dmg );
extern int is_enm_crt(char *enemy, creature *crt_ptr );
extern void die(creature *crt_ptr, creature *att_ptr ); 
extern void temp_perm(object  *obj_ptr );
extern void die_perm_crt(creature *crt_ptr );
extern void check_for_flee( creature *crt_ptr );
extern void consider(creature *ply_ptr, creature *crt_ptr );
extern int is_charm_crt(char *charmed, creature *crt_ptr );
extern int add_charm_crt(creature *crt_ptr, creature *ply_ptr );
extern int del_charm_crt(creature *crt_ptr, creature *ply_ptr );
extern int attack_mon(creature	*att_ptr, creature *atd_ptr );
extern int mobile_crt(creature	*crt_ptr );
extern int is_crt_killable( creature *crt_ptr, creature *att_ptr );
extern void remove_conjure_enemy(char *enemy);
extern int add_stolen_crt(creature *crt_ptr, creature *ply_ptr );
extern int del_stolen_crt(creature *crt_ptr, creature *ply_ptr );
extern int is_stolen_crt(char *stolen, creature *crt_ptr );


/* DM1.C */
extern int dm_teleport(creature *ply_ptr, cmd *cmnd );
extern int dm_send(creature *ply_ptr, cmd *cmnd );
extern int dm_purge(creature *ply_ptr, cmd	*cmnd );
extern int dm_users(creature *ply_ptr, cmd	*cmnd );
extern int dm_echo(creature *ply_ptr, cmd *cmnd );
extern int dm_flushsave( creature *ply_ptr, cmd *cmnd );
extern int dm_shutdown(creature *ply_ptr, cmd *cmnd );
extern int shutdown_catch(creature *ply_ptr, cmd *cmnd);
extern int dm_rmstat( creature *ply_ptr, cmd *cmnd );
extern int dm_flush_crtobj(creature *ply_ptr, cmd *cmnd );
extern int dm_reload_rom( creature *ply_ptr, cmd *cmnd );
extern int dm_save(creature *ply_ptr, cmd *cmnd );
extern int dm_create_obj(creature *ply_ptr, cmd *cmnd );
extern int dm_create_crt( creature *ply_ptr, cmd *cmnd );
extern int dm_perm(creature *ply_ptr, cmd *cmnd );
extern int dm_invis(creature *ply_ptr, cmd *cmnd );
extern int dm_ac(creature *ply_ptr, cmd *cmnd );
extern int dm_force(creature *ply_ptr, cmd	*cmnd );

/* DM2.C */
extern int dm_stat(creature *ply_ptr, cmd *cmnd );
extern int stat_rom(creature *ply_ptr, room *rom_ptr );
extern int stat_crt(creature *ply_ptr, creature *crt_ptr );
extern int stat_obj(creature *ply_ptr, object *obj_ptr );
extern int dm_add_rom(creature *ply_ptr, cmd *cmnd );
extern int dm_spy(creature *ply_ptr, cmd *cmnd );
extern int dm_mstat(creature *ply_ptr, cmd *cmnd );
extern int dm_ostat(creature *ply_ptr, cmd *cmnd );

/* DM3.C */
extern int dm_set(creature *ply_ptr, cmd *cmnd );
extern int dm_set_rom(creature *ply_ptr, cmd *cmnd );
extern int dm_set_crt(creature *ply_ptr, cmd *cmnd );
extern int dm_set_ext(creature	*ply_ptr, cmd *cmnd );
extern int dm_set_obj( creature *ply_ptr, cmd *cmnd );
extern int dm_set_xflg( creature *ply_ptr, cmd *cmnd );
extern int dm_set_xkey(creature *ply_ptr, cmd *cmnd );
extern void link_rom(room **rom_ptr, short tonum, char *dir );
extern int del_exit(room **rom_ptr, char *dir );
extern void expand_exit_name( char *name );
extern int opposite_exit_name(char *name, char *name2 );
extern int dm_log(creature *ply_ptr, cmd *cmnd );
extern int dm_loadlockout(creature	*ply_ptr, cmd *cmnd );
extern int dm_finger( creature *ply_ptr, cmd *cmnd );
extern int dm_list(creature *ply_ptr, cmd *cmnd );
extern int dm_info(creature *ply_ptr, cmd *cmnd );
extern int next_available_key(void);
extern int dm_add_lockout(creature *crt_ptr, cmd *cmnd);


/* DM4.C */
extern int dm_param(creature *ply_ptr, cmd *cmnd );
extern int dm_silence(creature *ply_ptr, cmd *cmnd);
extern int dm_broadecho(creature *ply_ptr, cmd *cmnd );
extern int dm_cast(creature *ply_ptr, cmd *cmnd );
extern int dm_group(creature *ply_ptr, cmd *cmnd );
extern int nirvana(creature *ply_ptr, cmd *cmnd );
extern int dm_view(creature *ply_ptr, cmd *cmnd );
extern int dm_obj_name(creature *ply_ptr, cmd *cmnd );
extern int dm_crt_name(creature *ply_ptr, cmd *cmnd );


/* DM5.C */
extern int dm_replace(creature *ply_ptr, cmd *cmnd );
extern int dm_delete(creature *ply_ptr, cmd *cmnd );
extern int desc_search(char *desc, char *pattern, int *val );
extern void txt_parse(char *str, char **pattern, int *val, char **replace );
extern int dm_nameroom(creature *ply_ptr, cmd *cmnd );
extern int dm_append(creature *ply_ptr, cmd *cmnd );
extern int dm_prepend(creature *ply_ptr, cmd *cmnd );
extern int dm_help(creature *ply_ptr, cmd *cmnd );
extern int builder_help(creature *ply_ptr, cmd *cmnd );


/* DM6.C */
extern int dm_dust(creature *ply_ptr, cmd *cmnd );
extern int dm_alias (creature *ply_ptr, cmd *cmnd );
extern int dm_attack (creature *ply_ptr, cmd *cmnd );
extern int list_enm(creature *ply_ptr, cmd *cmnd );
extern int list_charm (creature *ply_ptr, cmd *cmnd );
extern int dm_auth(creature *ply_ptr, cmd *cmnd );
extern int dm_flash(creature *ply_ptr, cmd *cmnd );


/* DM7.C */
extern int dm_save_crt(creature *ply_ptr, cmd *cmnd );
extern int dm_add_crt(creature	*ply_ptr, cmd *cmnd );
extern int dm_add_obj( creature *ply_ptr, cmd *cmnd );
extern int dm_save_obj(creature *ply_ptr, cmd *cmnd );
extern int dm_add_talk(creature *ply_ptr, cmd *cmnd );
extern int dm_add_talk_key(creature *ply_ptr, cmd *cmnd );
extern int dm_add_talk_resp(creature *ply_ptr, cmd *cmnd );
extern int dm_add_talk_show(creature *ply_ptr, cmd *cmnd );
extern int dm_add_talk_save(creature *ply_ptr, cmd *cmnd );
extern int dm_find_db(creature *ply_ptr, cmd *cmnd );
extern int dm_delete_db(creature *ply_ptr, cmd *cmnd );
extern int dm_game_status(creature *ply_ptr, cmd *cmnd);
extern void gamestat2(int fd, int param, char *instr);
extern int dm_advance(creature	*ply_ptr, cmd *cmnd );
extern int dm_dmalloc_stats(creature *ply_ptr, cmd *cmnd );

/* DM8.C */
extern int dm_familiar(creature *ply_ptr, cmd *cmnd );
extern int dm_own_ply(creature *ply_ptr, cmd *cmnd );
extern int dm_dump_io(creature *ply_ptr, cmd *cmnd );
extern int dm_jail(creature *crt_ptr, cmd *cmnd );
extern int dm_afk(creature *crt_ptr, cmd *cmnd );
extern int dm_dump_db(creature *crt_ptr, cmd *cmnd );
extern int dm_dump_rom(creature *ply_ptr);
extern int dm_dump_crt(creature *ply_ptr);
extern int dm_dump_obj(creature *crt_ptr);

/* DM9.C */
extern int dm_reset_shop(creature *ply_ptr, cmd *cmnd);
extern int dm_stolen(creature *ply_ptr, cmd *cmnd);

/* GUILD.C */
extern int init_guild();
extern int check_guild(creature *ply_ptr);
extern int monstercheck_guild(creature *ply_ptr);
extern int objectcheck_guild(object *ply_ptr);
extern int roomcheck_guild(room *ply_ptr);
extern int exitcheck_guild(exit_ *exit_ptr);
extern int pledge( creature *ply_ptr, cmd *cmnd );
extern int rescind( creature *ply_ptr, cmd *cmnd );
extern int guild_send( creature *ply_ptr, cmd *cmnd );
extern int guilds_list( creature *ply_ptr, cmd *cmnd );
extern int guild_score(creature *ply_ptr, creature *crt_ptr );
extern int guild_war(creature *ply_ptr, cmd *cmnd );
extern int pk_exp(creature *ply_ptr, creature *crt_ptr);
extern int offer(creature *ply_ptr, cmd *cmnd);

/* IO.C */
extern void sock_init(unsigned short port );
extern void sock_loop();
extern int io_check();
extern void accept_connect();
extern void init_connect( int fd );
extern int locked_out( int fd );
extern int addr_equal(char *str1, char *str2 );
extern int accept_input( int fd );
extern void output_buf();
extern void output_ply_buf(int fd);
extern void handle_commands();
extern void disconnect(int fd );
extern void add_wait(int fd );
extern int remove_wait( int i );
extern void waiting(int fd, int param, char *str );
extern void child_died();
extern void quick_shutdown();
extern void quick_readcf();
extern void reap_children();
extern void disconnect_all_ply();
extern void broadcast_login( creature *ply_ptr, int login );
extern void broadcast_wiz(creature *ply_ptr, char *str );
extern void elog_broad( char *str );
extern void output(int fd, char *str );
extern void output_nl(int fd, char *str );
extern void output_wc(int fd, char *str, int color );
extern void ask_for(int fd, char *str);
extern int spy_write(int fh, const void *lpvBuffer, unsigned int nLen );
extern void broadcast_class(creature *ply_ptr, char *fmt);
extern void broadcast_guild(creature *ply_ptr, char *fmt);
extern void broadcast_eaves(char *fmt );
extern void broadcast(char *msg);
extern void broadcast_c(char *msg, creature *crt_ptr );
extern void broadcast_wc(int color, char *msg );
extern void msprint(creature *viewpoint, char *buffer, char *fmt, void *args[] );
extern void mprint(int fd, char *fmt, void *args[] );
extern void broadcast_rom(int ignore, int rm, char *fmt, void *args[] );
extern void broadcast_rom2(int	ignore1, int ignore2, int rm, char *fmt, void *args[] );
extern void broadcast_dam(int ignore1, int ignore2, int rm, char*fmt, void *args[]);
extern int check_dos(int fd);


/* LOGIC.C */
extern int load_crt_actions( creature *crt_ptr );

 
/* MAGIC1.C */
extern int cast(creature *ply_ptr, cmd *cmnd );
extern int teach(creature *ply_ptr, cmd *cmnd );
extern int study(creature *ply_ptr, cmd *cmnd );
extern int readscroll(creature *ply_ptr, cmd *cmnd );
extern int drink(creature *ply_ptr, cmd *cmnd );
extern int zap(creature *ply_ptr, cmd *cmnd );
extern int zap_obj(creature *ply_ptr, object *obj_ptr, cmd *cmnd );
extern int offensive_spell(creature *ply_ptr, cmd *cmnd, int how, char *spellname, osp_t *osp );


/* MAGIC2.C */
extern int vigor(creature *ply_ptr, cmd *cmnd, int how );
extern int curepoison(creature *ply_ptr, cmd *cmnd, int how );
extern int light(creature *ply_ptr, cmd *cmnd, int how );
extern int protection(creature *ply_ptr, cmd *cmnd, int how );
extern int mend(creature *ply_ptr, cmd *cmnd, int how );

/* MAGIC3.C */
extern int bless(creature *ply_ptr, cmd *cmnd, int how );
extern int turn(creature *ply_ptr, cmd *cmnd );
extern int invisibility(creature *ply_ptr, cmd *cmnd, int how);
extern int restore(creature *ply_ptr, cmd *cmnd, int how );
extern int befuddle(creature *ply_ptr, cmd	*cmnd, int how );

/* MAGIC4.C */
extern int detectinvis(creature *ply_ptr, cmd *cmnd, int how );
extern int detectmagic(creature *ply_ptr, cmd *cmnd, int how );
extern int teleport(creature *ply_ptr, cmd	*cmnd, int how );
extern int enchant(creature *ply_ptr, cmd *cmnd, int how );


/* MAGIC5.C */
extern int recall(creature	*ply_ptr, cmd *cmnd, int how );
extern int summon(creature	*ply_ptr, cmd *cmnd, int how );
extern int heal(creature *ply_ptr, cmd *cmnd, int how );
extern int magictrack(creature	*ply_ptr, cmd *cmnd, int how );
extern int levitate(creature *ply_ptr, cmd *cmnd, int how );
extern int resist_fire(creature *ply_ptr, cmd *cmnd, int how );
extern int fly(creature *ply_ptr, cmd *cmnd, int how );


/* MAGIC6.C */
extern int resist_magic(creature *ply_ptr, cmd *cmnd, int how );
extern int know_alignment(creature	*ply_ptr, cmd *cmnd, int how );
extern int remove_curse(creature *ply_ptr, cmd *cmnd, int how );

/* MAGIC7.C */
extern int resist_cold(creature *ply_ptr, cmd *cmnd, int how );
extern int breathe_water(creature *ply_ptr, cmd *cmnd, int how );
extern int earth_shield(creature *ply_ptr, cmd *cmnd, int how );
extern int locate_player(creature *ply_ptr, cmd *cmnd, int how );
extern int drain_exp(creature *ply_ptr, cmd *cmnd, int how, object *obj_ptr );
extern int rm_disease(creature *ply_ptr, cmd *cmnd, int how );
extern int object_send(creature *ply_ptr, cmd *cmnd, int how );


/* MAGIC8.C */
extern int room_damage(creature *ply_ptr, cmd *cmnd, int how, char *spellname, osp_t *osp );
extern int room_vigor(creature *ply_ptr, cmd *cmnd, int how );
extern int rm_blind(creature *ply_ptr, cmd *cmnd, int how );
extern int blind(creature *ply_ptr, cmd *cmnd, int how );
extern int fear(creature *ply_ptr, cmd *cmnd, int how );
extern int silence(creature *ply_ptr, cmd *cmnd, int how );
extern int rm_blindness(creature *ply_ptr, cmd *cmnd, int how );
extern int spell_fail( creature *ply_ptr, int how );
extern int fortune(creature *ply_ptr, cmd *cmnd, int how );

/* MAGIC9.C */
extern int recharge_wand(creature *ply_ptr, cmd *cmnd );
extern int tmp_enchant(creature *ply_ptr, cmd *cmnd );
extern int conjure(creature *ply_ptr, cmd *cmnd, int how );
extern int curse(creature *ply_ptr, cmd *cmnd, int how, object *obj_ptr );
extern int dispel(creature *ply_ptr, cmd *cmnd, int how, object *obj_ptr );
extern int del_conjured(creature *crt_ptr);

/* MAIN.C */
extern void mvc_log(void);
extern unsigned short schedule_g(void);
extern void startup_mordor(void);
extern void display_credits(int fd);
extern void handle_args( int argc, char *argv[] );
extern void log_exit(void);

/* MEMORY.C */
extern int dm_memory( creature *ply_ptr, cmd *cmnd );


/* MISC.C */
extern int get_ply_flags(creature *ply_ptr );
extern char *crt_str(creature *viewpoint, creature	*crt, int num, int flags );
extern char *obj_str(creature *viewpoint, object *obj, int num, int flags );
extern void delimit( char *str );
extern void view_file(int fd, int param, char *str );
extern int dice(int n, int s, int p);
extern int exp_to_lev(long	exp );
extern int dec_daily( struct daily *dly_ptr );
extern void sort_cmds();
extern int sort_cmp( struct cmdstruct *first, struct cmdstruct *second );
extern void load_lockouts();
extern void please_wait(int fd, time_t duration );
extern int is_num( char *str );
extern void **m1arg(void *arg1);
extern void **m2args(void *arg1, void *arg2);
extern void **m3args(void *arg1, void *arg2, void *arg3);
extern void clean_str(char *str, int strip_count );
extern int convert_seconds_to_str(long interval, char *str );
extern int in_group(creature *crt_ptr, creature *ply_ptr);
extern void view_file_reverse(int fd, int param, char *str);
extern int create_a_room(int num, room *rom_ptr);

/* MSP.C */
extern void play_sound(int fd, char *fname, int vol, int repeat, int priority, char *type, char *url);
extern void play_music();

#ifdef WIN32
/* NTSERVIC.C */
extern void InstallService();
extern void RemoveService();
#endif

/* OBJECT.C */
extern void add_obj_obj(object *obj_ptr1, object *obj_ptr2 );
extern void del_obj_obj(object *obj_ptr1, object *obj_ptr2 );
extern object *find_obj(creature *ply_ptr, otag *first_ot, char *str, int val );
extern int list_obj(char *str, creature *ply_ptr, otag	*first_otag );
extern int weight_obj(object *obj_ptr );
extern void rand_enchant(object *obj_ptr );
extern void enchant_obj( object *obj_ptr, int value );
extern long find_obj_value(object *obj_ptr);
extern object *duplicate_object(object *old_obj);


/* PLAYER.C */
extern void init_ply( creature	*ply_ptr );
extern void uninit_ply( creature *ply_ptr );
extern void update_ply( creature *ply_ptr );
extern void up_level( creature *ply_ptr );
extern void down_level( creature *ply_ptr );
extern void add_obj_crt(object	*obj_ptr, creature *ply_ptr );
extern void del_obj_crt(object *obj_ptr, creature *ply_ptr );
extern void compute_ac(creature *ply_ptr );
extern void compute_thaco( creature *ply_ptr );
extern int mod_profic( creature *ply_ptr );
extern int weight_ply( creature *ply_ptr );
extern int max_weight( creature *ply_ptr );
extern int profic( creature *ply_ptr, int index );
extern int mprofic( creature *ply_ptr, int index );
extern int fall_ply( creature *ply_ptr );
extern creature *find_who( char *name );
extern creature *lowest_piety(room	*rom_ptr, int invis );
extern int has_light( creature *crt_ptr );
extern void ply_prompt( creature *ply_ptr );
extern creature *low_piety_alg(room *rom_ptr, int invis, int alg, int lvl );
extern int luck( creature *ply_ptr );
extern void set_monk_dice(creature *ply_ptr);
extern void validate_player(creature *ply_ptr);
extern int check_for_spam(creature *ply_ptr );
extern void silence_spammer( creature *ply_ptr );

/* PLAYER2.C */
extern int dequip(creature *ply_ptr, object *obj_ptr);
extern int equip(creature *ply_ptr, object *obj_ptr);
extern int equipment_sets(object *obj_ptr, int flag);
extern int check_illegal_stats(creature *ply_ptr, object *obj_ptr);
extern int check_ply_ext(char *name, exit_ *ext_ptr);

/* POST.C */
extern int postsend(creature *ply_ptr, cmd *cmnd );
extern void postedit(int fd, int param, char *str );
extern int postread(creature *ply_ptr, cmd	*cmnd );
extern int postdelete( creature *ply_ptr, cmd *cmnd );
extern int notepad( creature *ply_ptr, cmd *cmnd );
extern void noteedit(int fd, int param, char *str );
extern int welcome_edit( creature *ply_ptr, cmd *cmnd );




/* READCF.C */

extern int readcf();


/* ROOM.C */
extern void add_ply_rom(creature *ply_ptr, room *rom_ptr );
extern void del_ply_rom(creature *ply_ptr, room *rom_ptr );
extern void add_obj_rom(object	*obj_ptr, room *rom_ptr );
extern void del_obj_rom(object	*obj_ptr, room *rom_ptr );
extern void add_crt_rom(creature *crt_ptr, room *rom_ptr, int num );
extern void del_crt_rom(creature *crt_ptr, room *rom_ptr );
extern void add_permcrt_rom(room *rom_ptr );
extern void add_permobj_rom(room *rom_ptr );
extern void check_exits(room *rom_ptr );
extern void display_rom(creature *ply_ptr, room *rom_ptr );
extern exit_ *find_ext(creature *ply_ptr, xtag *first_xt, char *str, int val );
extern void check_traps(creature *ply_ptr, room *rom_ptr );
extern int count_vis_ply(room *rom_ptr );
extern int count_vis_mon(room *rom_ptr );
extern int count_vis_ply_and_cnj(room *rom_ptr );
extern creature *find_rom_owner(room *rom_ptr);
extern int set_rom_owner(creature *ply_ptr, int type);


/* ROOM2.C */
extern int create_a_room(int num, room *rom_ptr);
extern void check_random_exits(room *rom_ptr);
extern exit_ *find_all_ext(xtag *first_xt, char *str);
extern void flush_crier();

/* SCREEN.C */
extern void ANSI( int fd, int color );
extern void gotoxy(int fd, int x, int y );
extern void scroll(int fd, int y1, int y2 );
extern void delline(int fd );
extern void clrscr(int fd );
extern void save_cursor(int fd );
extern void load_cursor( int fd );


/* SPECIAL1.C */
extern int special_obj(creature *ply_ptr, cmd *cmnd, int special );
extern int special_cmd(creature *ply_ptr, int special, cmd	*cmnd );
extern int combo_box(creature *ply_ptr, object *obj_ptr );


/* TALK.C */
extern int dm_talk(creature *ply_ptr, cmd *cmnd );
extern int init_talk(creature *ply_ptr);
extern int set_keys_talk(creature *ply_ptr, cmd *cmnd );
extern int set_response_talk(creature *ply_ptr, cmd *cmnd );
extern int list_talk(creature *ply_ptr);
extern int save_talk(creature *ply_ptr, cmd *cmnd );
extern int delete_talk(creature *ply_ptr, cmd *cmnd );


/* UPDATE.C */
extern void update_game();
extern void update_users(long t);
extern void update_random(long t);
extern void update_active(long t);
extern int choose_item(creature *ply_ptr );
extern int crt_spell(creature *crt_ptr, creature *att_ptr);
extern void update_time(long t);
extern void update_shutdown(long t);
extern void update_guildwar(long t);
extern void update_exit(long t);
extern void update_allcmd(long t);
extern int list_act(creature *ply_ptr, cmd	*cmnd );
extern void update_weather(long t);
extern void update_security(long t);
extern void update_action(long t);
extern void update_dust_output(long t);
extern void crash( int sig );
extern void heal_crt( creature * crt_ptr );
extern void update_random_exit(long t);
extern int check_conjure_vigor(creature *crt_ptr);
