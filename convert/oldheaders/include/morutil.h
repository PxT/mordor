// (c) 1996 Brooke Paul
/* Mordor Utilities */
#ifndef __MORUTIL_H_
#define __MORUTIL_H_

#include "../include/port.h"


#define merror(a, b)	new_merror(a, b, __FILE__, __LINE__ );

/* merror() error types */
#define FATAL           1
#define NONFATAL        0


/* log files */
#define ERROR_LOG		"error.log"
#define PLAYER_LOG		"access.log"
#define IMMORTAL_LOG	"immortal.log"
#define JAIL_LOG		"jail.log"


/* ansi foreground colors */ 
#define AFC_BLACK           30
#define AFC_RED             31
#define AFC_GREEN           32
#define AFC_YELLOW          33
#define AFC_BLUE            34
#define AFC_MAGENTA         35
#define AFC_CYAN            36
#define AFC_WHITE           37

/* ansi background colors */
#define ABC_BLACK           40
#define ABC_RED             41
#define ABC_GREEN           42
#define ABC_YELLOW          43
#define ABC_BLUE            44
#define ABC_MAGENTA         45
#define ABC_CYAN            46
#define ABC_WHITE           47


/* ansi modes */
#define AM_NORMAL			0	  
#define AM_BOLD				1
#define AM_LOWINTESITY		2
#define AM_ITALIC			3
#define AM_UNDERLINE		4
#define AM_BLINK			5
#define AM_RAPIDBLINK		6
#define AM_REVERSE			7
#define AM_INVISIBLE		8


#define low( a )	tolower( a )
#define up( a )		toupper( a )



/* custom assert stuff to output to the log file */
void _assertlog( char * strExp, char *strFile, unsigned int nLine );

#undef  ASSERTLOG

#ifdef  NDEBUG

#define ASSERTLOG(exp)     ((void)0)

#else

#define ASSERTLOG(exp) (void)( (exp) || (_assertlog(#exp, __FILE__, __LINE__), 0) )

#endif  /* NDEBUG */


#define mrand(a,b)      ((a)+(rand()%((b)*10-(a)*10+10))/10)


extern void new_merror(char *str, char errtype, char *file, const int line );
extern void lowercize(char	*str, int flag );
extern void zero(void *ptr, int size);
extern void posit(int x, int y);
extern void clearscreen(void);
extern void savepos();
extern void restorepos();
extern void spin(int x);
extern void message(char *str);
extern int get_config_file(char *filename);
extern int load_paths();
extern int file_exists( char *filename );
extern int convert_to_bool(char *buf);
extern int convert_to_color(char *buf);
extern char *get_time_str();


extern char *get_log_path();
extern char *get_room_path();
extern char *get_monster_path();
extern char *get_object_path();
extern char *get_player_path();
extern char *get_doc_path();
extern char *get_post_path();
extern char *get_bin_path();
extern void logn(char *filename, char *str);
extern void ilog(char *str);
extern void log_dm_command(char *ply_name, char *fullstr);
extern void plog(char *str);
extern void elog(char *str);



#endif
