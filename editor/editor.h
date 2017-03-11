/*
 * Copyright (c) 1993 Brooke Paul
 *
 * $Id: editor.h,v 1.1 2001/08/14 23:13:08 develop Exp develop $
 *
 * $Log: editor.h,v $
 * Revision 1.1  2001/08/14 23:13:08  develop
 * Initial revision
 *
/* prototypes for editor.c */
extern void edit_object();
extern void edit_monster();
extern void edit_room();
extern void edit_exits(room *rom_ptr);
extern void getnum(int *i, int min, int max);
extern void getnums(long *i, long orig, long min, long max);
extern void getstr(char *str, char *origstr, int maxlen);


/* prototypes for editor2.c */
extern void edit_player();
extern void edit_items(creature *ply_ptr);

