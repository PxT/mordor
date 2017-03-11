/*
 * MEXTERN.H:
 *
 *	This file contains the external function and variable 
 *	declarations required by the rest of the program.
 *
 *	Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include <stdio.h>
#include <sys/file.h>
#ifndef MIGNORE

extern int		Tablesize;
extern int		Cmdnum;
extern long		Time;
extern long		StartTime;
extern struct lasttime	Shutdown;
extern struct lasttime  Weather[4];
extern int		Spy[PMAX];
extern int		Numlockedout;
extern lockout		*Lockout;

extern struct {
	creature	*ply;
	iobuf		*io;
	extra		*extr;
} Ply[PMAX];

extern struct {
	short		hpstart;
	short		mpstart;
	short		hp;
	short		mp;
	short		ndice;
	short		sdice;
	short		pdice;
} class_stats[11];

extern struct cmdstruct {
	char	*cmdstr;
	int	cmdno;
	int	(*cmdfn)();
} cmdlist[];

extern struct {
	char	*splstr;
	int	splno;
	int	(*splfn)();
} spllist[];

extern struct {
	int	splno;
	char	realm;
	int	mp;
	int	ndice;
	int	sdice;
	int	pdice;
	char	bonus_type;
} ospell[];

extern short	level_cycle[][10];
extern short	thaco_list[][20];
extern long	quest_exp[];
extern int	bonus[35];
extern char	class_str[][15];
extern char	race_str[][15];
extern char	race_adj[][15];
extern char	lev_title[][8][20];
extern char 	article[][10];
extern char 	number[][10];
extern long	needed_exp[25];

#endif

/* FILES1.C */

extern int	count_obj(), write_obj(), count_inv(), write_crt(),
		count_mon(), count_ite(), count_ext(), count_ply(),
		write_rom(), read_obj(), read_crt(), read_rom();
extern void	free_obj(), free_crt(), free_rom();

/* FILES2.C */

extern int	load_rom(), load_mon(), load_obj(), load_ply(), save_ply(),
		is_rom_loaded(), reload_rom(), resave_rom();
extern void	put_queue(), pull_queue(), front_queue(), flush_rom(),
		flush_crt(), flush_obj(), resave_all_rom(), save_all_ply();

/* FILES3.C */

extern int	write_obj_to_mem(), write_crt_to_mem(), read_obj_from_mem(),
		read_crt_from_mem(), load_crt_tlk(), talk_crt_act();

/* IO.C */

extern int	io_check(), accept_input(), locked_out(), addr_equal(),
		remove_wait();
extern void	sock_init(), sock_loop(), accept_connect(), output_buf(), 
		print(), handle_commands(), disconnect(), broadcast(), broadcast_login(),
		broadcast_wiz(), broadcast_rom(), broadcast_rom2(),
		broadcast_robot_rom(), add_wait(), init_connect(),
		waiting(), child_died(), reap_children();

/* COMMAND1.C */

extern void	login(), create_ply(), command(), parse();
extern int	process_cmd();

/* COMMAND2.C */

extern int	look(), move(), say(), get(), inventory(), drop();
extern void	get_all_rom(), get_all_obj(), drop_all_rom(), drop_all_obj(),
		get_perm_obj();

/* COMMAND3.C */

extern int	wear(), remove_obj(), equipment(), ready(), hold();
extern void	wear_all(), remove_all(), equip_list();

/* COMMAND4.C */

extern int	health(), info(), follow(), lose(), group(), track(), peek(),
		help(), welcome(), psend(), broadsend();
extern void	info_2();

/* COMMAND5.C */

extern int	attack(), who(), search(), hide(), set(), clear(), 
		quit(), ply_suicide(), attack_crt(), whois();
extern void	suicide();

/* COMMAND6.C */

extern int	yell(), go(), openexit(), closeexit(), unlock(), lock(),
		picklock(), steal();

/* COMMAND7.C */

extern int	flee(), list(), buy(), sell(), value(), backstab(), train();

/* COMMAND8.C */

extern int	give(), repair(), prt_time(), circle(), bash(), savegame(), 
		talk();
extern void	give_money(), talk_action();

/* COMMAND9.C */

extern int	bribe(), haste(), pray(), prepare(), use(), ignore(), pfinger();

/* COMMAND10.C */

extern int	sneak(), gtalk(), pledge(), rescind(), purchase(),
		selection(), trade(), room_obj_count();

extern void 	lower_prof(), add_prof(), lose_all(), dissolve_item();

/* COMMAND11.C */ 

extern int	emote();

extern int	passwd(), vote();

extern int	pfinger();

/* COMMAND12.C */

extern int	bard_song(), bard_song2(), meditate(), touch_of_death();

/* MAGIC1.C */

extern int	cast(), teach(), study(), readscroll(), drink(), zap(),
		zap_obj(), offensive_spell();

/* MAGIC2.C */

extern int	vigor(), curepoison(), light(), protection(), mend();

/* MAGIC3.C */

extern int	bless(), invisibility(), restore(), turn(), befuddle();

/* MAGIC4.C */

extern int	detectinvis(), detectmagic(), teleport(), enchant();

/* MAGIC5.C */

extern int	recall(), summon(), heal(), magictrack(), levitate(),
		resist_fire(), fly();

/* MAGIC6.C */

extern int	resist_magic(), know_alignment(), remove_curse();

/* MAGIC7.C */

extern int	resist_cold(), breathe_water(), earth_shield(),
		locate_player(), drain_exp (), rm_disease(), object_send();

/* MAGIC8.C */

extern int	room_vigor(), fear(), rm_blind(), silence(), blind(),
		spell_fail(), fortune(); 

/* DM1.C */

extern int	dm_teleport(), dm_send(), dm_purge(), dm_users(),
		dm_echo(), dm_flushsave(), dm_rmstat(), dm_reload_rom(),
		dm_resave(), dm_create_obj(), dm_perm(), dm_invis(),
		dm_ac(), dm_shutdown(), dm_force(), dm_flush_crtobj(),
		dm_create_crt();

/* DM2.C */

extern int	stat_rom(), stat_crt(), stat_obj(), dm_stat(),
		dm_add(), dm_add_rom(), dm_add_ext(), dm_add_crt(),
		dm_add_obj(), dm_spy();

/* DM3.C */

extern void	link_rom(), expand_exit_name();
extern int	dm_set(), dm_set_rom(), dm_set_ext(), dm_set_crt(),
		dm_set_obj(), del_exit(), dm_log(), dm_loadlockout(),
		dm_finger(), dm_list(), dm_info(), dm_set_xflg();

/* DM4.C */

extern int	dm_param(), dm_silence(), dm_broadecho(), 
		dm_cast(), dm_group(), dm_view(), dm_obj_name(),
		dm_crt_name();

/* DM5.C */
extern int     dm_replace(), desc_search(), dm_nameroom(), dm_append(), 
               dm_prepend(), dm_delete(), dm_help();

extern void 	txt_parse();

/* DM6.C */
extern int	dm_dust(), dm_attack(), list_enm(), list_charm(), 
		dm_alias(), dm_auth(), dm_flash(), dm_memory();

/* PLAYER.C */

extern void	init_ply(), uninit_ply(), update_ply(), up_level(),
		down_level(), add_obj_crt(), del_obj_crt(), compute_ac(), 
		compute_thaco();
extern char	*title_ply(), *ply_prompt();
extern int	weight_ply(), max_weight(), profic(), mprofic(),
		mod_profic(), has_light(), fall_ply(), luck(), update_combat();
extern creature	*find_who(), *lowest_piety(), *low_piety_alg();

/* CREATURE.C */

extern creature	*find_crt();
extern int 	add_enm_crt(), del_enm_crt(), is_enm_crt(), add_charm_crt(), 
		del_charm_crt(), is_charm_crt(), attack_mon(), mobile_crt();
extern void	end_enm_crt(), die(), temp_perm(), die_perm_crt(), 
		check_for_flee(), consider(), add_enm_dmg(), monster_combat();

/* ROOM.C */

extern void	add_ply_rom(), del_ply_rom(), add_obj_rom(),
		del_obj_rom(), add_crt_rom(), del_crt_rom(), 
		add_permcrt_rom(), add_permobj_rom(), 
		check_exits(), display_rom(), check_traps();
extern exit_	*find_ext();
extern int	count_vis_ply();

/* OBJECT.C */

extern void	add_obj_obj(), del_obj_obj(), rand_enchant();
extern int	list_obj(), weight_obj();
extern object	*find_obj();

/* UPDATE.C */

extern void	update_game(), update_users(), update_random(),
		update_active(), update_time(), update_exit(),
		update_shutdown(), add_active(), del_active(), update_weather(), update_security();
extern int	crt_spell(), choose_item(), is_crt_active();

/* POST.C */

extern int	postsend(), postread(), postdelete(), notepad();
extern void	postedit(), noteedit();

/* ACTION.C */

extern int	action();

/* MISC.C */

extern void	merror(), lowercize(), zero(), delimit(), view_file(), loge(),
		sort_cmds(), load_lockouts(), please_wait(), logn();
extern int	low(), up(), dice(), exp_to_lev(), dec_daily(), sort_cmp(),
		file_exists(), isnumber();
extern char	*crt_str(), *obj_str();

/* SPECIAL1.C */

extern int	special_read(), special_cmd();

#ifdef COMPRESS

/* COMPRESS.C */

extern int	compress(), uncompress();


#endif
extern int nirvana(),list_act();
extern void log_act();
