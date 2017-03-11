/*
 * MSTRUCT.H:
 *
 *	Main data structures and type definitions.
 *
 *	Copyright (C) 1991, 1992, 1993 Brooke Paul
 *
 * $Id: mstruct.h,v 6.16 2001/07/22 19:03:06 develop Exp $
 *
 * $Log: mstruct.h,v $
 * Revision 6.16  2001/07/22 19:03:06  develop
 * first run at alllowing thieves to steal gold from other players
 *
 * Revision 6.15  2001/06/30 01:09:05  develop
 * added struct for default enemy list
 *
 * Revision 6.14  2001/06/29 03:22:12  develop
 * added struct for default enemy lists
 *
 * Revision 6.13  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */


#include "mtype.h"




typedef struct iobuf {			/* I/O buffers for players */
	char		input[IBUFSIZE];
	char		output[OBUFSIZE];
	short		ihead, itail;
	short		ohead, otail;
	void		(*fn)();
	char		fnparam;
	time_t		ltime;
	char		intrpt;
	char		commands;
	int			lookup_pid;
	char		address[40];
	char		userid[9];
	int			loginattempts;
} iobuf;

typedef struct extra {			/* Extra (non-saved) player fields */
	char		tempstr[4][80];
	char		lastcommand[80];
	ctag		*first_charm;
	etag		*first_ignore;
	int		luck;
	struct creature	*alias_crt;
	int		*scared_of;	
	int		loginattempts;
	ctag		*first_stolen;
} extra;

typedef struct lockout {
	char		address[80];
	char		password[20];
	char		userid[9];
} lockout;

typedef struct cmd {
	int		num;
	char		fullstr[256];
	char		str[COMMANDMAX][MAX_TOKEN_SIZE];
	long		val[COMMANDMAX];
} cmd;

typedef struct osp_t {
	int		splno;
	char		realm;
	int		mp;
	int		ndice;
	int		sdice;
	int		pdice;
	char		bonus_type;
	int		intcast;
} osp_t;



/* for monk leveling code */
/* but could be used elsewhere if needed */
typedef struct tagDice
{
	int	nDice;
	int	nSides;
	int	nPlus;
} DICE, *PDICE;


typedef struct tagPlayer 
{
	creature	*ply;
	iobuf		*io;
	extra		*extr;
} plystruct;


typedef struct {
	short		hpstart;
	short		mpstart;
	short		hp;
	short		mp;
	short		ndice;
	short		sdice;
	short		pdice;
} class_stats_struct;


typedef int (*PFNCOMPARE)(const void *, const void *);

struct cmdstruct {
	char		*cmdstr;
	int		cmdno;
	int		(*cmdfn)();
};

struct creature_template {
	int     stats;
        short   hp;
        char    armor;
        char    thaco;
        long    experience;
        short   ndice;
	short	sdice;
	short	pdice;
};

typedef struct tagAnsiColor
{
	short	attribute;
	short	forground;
	short	background;
} ANSICOLOR, *PANSICOLOR;

struct enemy_list {
        char *attacker;
        char *target;
};
