/*
	List players info
	Copyright 1993, 1999 Brooke Paul
 $Id: plist.c,v 1.8 2001/06/11 01:50:20 develop Exp $

 $Log: plist.c,v $
 Revision 1.8  2001/06/11 01:50:20  develop
 fixed memory leak in spell checking...duh

 Revision 1.7  2001/06/11 01:33:25  develop
 completed addition of spell checking to plist

 Revision 1.6  2001/06/11 01:29:05  develop
 *** empty log message ***

 Revision 1.5  2001/06/11 01:22:52  develop
 *** empty log message ***

 Revision 1.4  2001/06/11 01:17:29  develop
 *** empty log message ***

 Revision 1.3  2001/06/11 01:13:26  develop
 *** empty log message ***

 Revision 1.2  2001/06/11 01:12:31  develop
 adding spell checking ability to plist

 Revision 1.1  2001/06/10 23:56:08  develop
 Initial revision


*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#include "../include/morutil.h"
#include "../include/mordb.h"


#define set_flag(f,n) ((f) |= 1 << (n))
#define clr_flag(f,n) ((f) &= ~(1 << (n)))
#define is_fset(f,n) ((f) & 1 << (n))

/* globals */
int		lvl, spl=0, Spell[15], Spellnum=0;


/* prototypes */
void print_player(char *name, char flags);


/* functions */
void usage()
{
	printf("syntax: plist [[-pni] [-l #] [-s #]] <players>\n");
	return;
}



int main (int argc, char *argv[])
/* plist command provides a quick way to check a given players *
 * level, class, password and inventory without having to log  *
 * the player in or use the editor */
{
	int		i, adj;
	char	filename[256];
	char	flags = 0;
	HFINDFILE	hff;

	lvl=0;
	adj = 1;

 
    if (argc < 1) {
		usage();
        return(1);
    }
 
    for (i=1;i<argc;i++){
 
        if (argv[i][0] == '-'){
            switch(argv[i][1]){
                case 'i':
		    set_flag(flags,0);
                    continue;
	            break;
                case 'n':
		    set_flag(flags,1);
       	             continue;
                case 'p':
			set_flag(flags,2);
                    	continue;
		        break;  
            }
 
            if(i+1 < argc && isdigit(argv[i+1][0])){
                switch(argv[i][1]){
                    case 'l':
                        lvl = atoi(argv[i+1]);
                    break;
		    case 's':
                        Spellnum = atoi(argv[i+1]);
			spl=1;
                    break;
                    default:
                       set_flag(flags,3); 
                    break;
                }
                i++;
            }
            else
                set_flag(flags,3); 
        }
	else
	    break;
 
        if (is_fset(flags,3)){
            usage();
            return(1);
        }
            
    } 
	adj = i;

	if (adj > argc)
	{
       	usage();
		return(1);
	}
	else if ( adj == argc )
	{
		/* wants all players */
		hff = find_first_file(get_player_path(), filename, 256);
		if ( hff )
		{
			do 
			{
				if ( filename[0] != '.')
				{
					print_player( filename, flags );
				}

			} while( find_next_file( hff, filename, 256 ));

			close_find_file(hff);
		}
    }
	else
	{
		for(i= adj;i < argc; i++)
		{
			print_player( argv[i], flags );
		}
	}


	return(0);
}


void print_player(char *name, char flags)
{
	creature	*player;
	int		breakout, j;

	if(!isupper(name[0]))
		return;	
	if(load_ply_from_file(name, &player) < 0){
		printf("Player Load Error (%s).\n", name);
		return;
	}
	
	if(spl) {
		/* Check for certain spell */
                if(!S_ISSET(player, Spellnum)) {
			free_crt(player);
			return;
		}
	}

	/* not lvl check or player = lvl */
	if( !lvl || player->level == lvl){
	   printf("%s%s",(!is_fset(flags,1)) ? "Name: " : "",
		player->name);

	/*print  password */
	if (is_fset(flags,2))
		printf(" (%s)\n",player->password);
	else
		printf("\n");

	/* if not name only display */
	if(!is_fset(flags,1))
		printf("Class: %s  (%d) %s\n",get_class_string(player->class),
		   player->level, get_race_string(player->race));

	/* print players inventory */
	if(is_fset(flags,0)){
		otag *obj, *cnt;
		int len = 0;

		printf("Inventory:\n");
		obj = player->first_obj; 
		if(!obj) printf("none");

		while(obj){
		printf("%s%s",obj->obj->name, (obj->next_tag) ?
			(F_ISSET(obj->obj,OCONTN)) ? ":" :", " : ".");
		len += strlen(obj->obj->name);

		if(F_ISSET(obj->obj,OCONTN)){
			cnt = obj->obj->first_obj;
			printf("(");
			if (!cnt) printf("empty");
			while(cnt){
			printf("%s%s",cnt->obj->name, 
			  (cnt->next_tag) ?  ", " : "");
			len += strlen(cnt->obj->name);
			if (len > 50){
				printf("\n");
				len = 0; }
			cnt = cnt->next_tag;
			}
			printf(")%s",(obj->next_tag) ? ", " : ".");
		}
		if (len > 50){
			printf("\n");
			len = 0;
		}
				   obj = obj->next_tag;    
		}
		   printf("\n");
	}
			}

	free_crt(player);

	return;
}
