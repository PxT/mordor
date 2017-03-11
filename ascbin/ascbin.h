/*
 * Copyright (C) 1993 Brooke Paul
 *
 * $Id: ascbin.h,v 1.3 2001/07/06 15:15:22 develop Exp $
 *
 * $Log: ascbin.h,v $
 * Revision 1.3  2001/07/06 15:15:22  develop
 * *** empty log message ***
 *
 * Revision 1.1  2001/04/01 17:54:35  develop
 * Initial revision
 *
 */

/* ascii1.c */
extern void usage();
extern void write_all_rom();
extern void write_all_crt();
extern void write_all_obj();
extern void write_all_ply();
extern void read_all_rom();
extern void read_all_ply();
extern void read_all_obj();
extern void read_all_crt();


extern int write_room(FILE *fp, room *rom);
extern int write_object(FILE *fp, int num, object *obj);
extern int write_creature(FILE *fp, int num, creature *crt);
extern int write_player(FILE *fp, char *str, creature *ply);
extern int write_object_basic(FILE *fp, int index, object *obj);
extern int write_creature_basic(FILE *fp, int index, creature *crt);
extern int read_room(FILE *fp, room *rom);
extern int read_player(FILE *fp, char *name, creature *ply);
extern int read_object(FILE *fp, object *obj);
extern int read_creature(FILE *fp, creature * crt);
extern int read_object_basic(FILE *fp, int *index, object *obj);
extern int read_creature_basic(FILE *fp, int *index, creature *crt);
extern void write_lasttime(FILE *fp, struct lasttime lt);
extern void read_lasttime(FILE *fp, struct lasttime *lt);


/* ascbin.c */
extern void write_short(FILE *fp, short s);
extern void read_short(FILE *fp, short *s);
extern void write_int(FILE *fp, int l);
extern void read_int(FILE *fp, int *l);
extern void write_long(FILE *fp, long l);
extern void read_long(FILE *fp, long *l);
extern void write_char(FILE *fp, char ch);
extern void read_char(FILE *fp, char *ch);
extern int write_chars(FILE *fp, char *str, int count);
extern void read_chars(FILE *fp, char *str, int count);
