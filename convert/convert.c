/* convert.c for mordor */
/* Last modified: 08/05/00 */
/* <paul@droflet.net>
 * No Warranty, Use at your own risk, your mileage may vary, etc.
 *
 * Distribution of this work in source or binary form must be accompanied by the
 * README file which describes the license under which this software is released.
 * If you did not receive the README file, please write to the author at the 
 * address listed above.
 * $Id$
 *
 * $Log$
 *
 */

#include <stdio.h>
#include <stdlib.h>

/* These should be your _old_ includes */
#include "oldheaders/include/port.h"
#include "oldheaders/include/morutil.h"
#include "oldheaders/include/mordb.h"

#include "convert.h"

int verbose;
char spaces[4]="";
char version[]="1.0";

int main(int argc, char *argv[])
{
	char select[2],convertee[20];
	int choice,i;

	verbose = 0;

	printf("Mordor struct converter %s, please see README file\n", version);
	if(argc < 3) {
		printf("Usage: %s -[p|r|o|c] [-verbose] <convertee>\n", argv[0]);
		return(0);
	}
	strncpy(select, argv[1], 2);
	if(!strcmp(&select[0], "-")) {
		printf("Usage: %s -[p|r|o|c] [-verbose] <convertee>\n", argv[0]);
		return(0);
	}

	if(!strcmp(&argv[2][0], "-v")) {
		verbose = 1;
		strcpy(convertee, argv[3]);
	}
	else
		strcpy(convertee, argv[2]);

	choice = select[1];
	switch (choice) {
		case 'p':		/* player */
			conv_ply(convertee);
			break;

		case 'r':		/* room */
			i = atoi(convertee);
			conv_rom(i);
			break;

		case 'o':		/* object */
			i = atoi(convertee);
			conv_obj(i);
			break;

		case 'c':		/* creature */
			i = atoi(convertee);
			conv_crt(i);
			break;

		default:
			printf("Usage: %s -[p|r|o|c] [-verbose] <convertee>\n", argv[0]);
			break;

	}

	if(!verbose)
		printf("\n");

	return(0);
			
}

int conv_rom(int rom_num)
{ 

	int	filename;
        room    *rom_ptr;
        n_room      *new_ptr;
        int i;

	filename=rom_num;

        if (load_rom(filename, &rom_ptr) < 0) {
                printf("Error loading room %d\n", filename);
                return(0);
        } else {
        new_ptr = (n_room *)malloc(sizeof(n_room));
        if(!new_ptr) {
                printf("error in malloc\n");
                return(1);
        }
        zero(new_ptr, sizeof(n_room));
        copy_rom(rom_ptr, new_ptr);
        save_rom_to_file2(filename, new_ptr);
    /*    free(new_ptr); */

        }
        return(0);

}

int conv_crt(int crt_num)
{ 
        int    filename;
        creature    *crt_ptr;
        n_creature      *new_ptr;
        int i;

	filename=crt_num;

        if (load_crt(filename, &crt_ptr) < 0) {
                printf("Error loading %d\n", filename);
                return(0);
        } else {
        new_ptr = (n_creature *)malloc(sizeof(n_creature));
        if(!new_ptr) {
                printf("error in malloc\n");
                return(1);
        }
        zero(new_ptr, sizeof(n_creature));

        copy_ply(crt_ptr, new_ptr);
        save_crt_to_file2(filename, new_ptr);
        free(new_ptr);

        }
	return(0); 
}

int conv_obj(int obj_num)
{ 

        int    filename;
        object    *obj_ptr;
        n_object      *new_ptr;
        int i;

	filename=obj_num;

        if (load_obj(filename, &obj_ptr) < 0) {
                printf("Error loading %d\n", filename);
                return(0);
        } 
        new_ptr = (n_object *)malloc(sizeof(n_object));
        if(!new_ptr) {
                printf("error in malloc\n");
                return(1);
        }
        zero(new_ptr, sizeof(n_object));
        copy_obj(obj_ptr, new_ptr);
        save_obj_to_file2(filename, new_ptr);
        free(new_ptr);

        
        return(0);


}

int conv_ply(char *ply_name)
{
        char    filename[256];
        creature    *ply_ptr;
	n_creature	*new_ptr;
	int i;
		
		strcpy(filename, ply_name);
		if (load_ply(filename, &ply_ptr) < 0) {
			printf("Error loading %s\n", filename);
			return(0);
		} else {
				
        new_ptr = (n_creature *)malloc(sizeof(n_creature));
        if(!new_ptr) {
                printf("error in malloc\n");
		return(1);
	}

        zero(new_ptr, sizeof(n_creature));
	copy_ply(ply_ptr, new_ptr);	
	strcat(filename, ".new"); 
	save_ply_to_file2(filename, new_ptr);
	free(new_ptr); 

        }

return 0; 
}


int copy_rom(room *rom_ptr, n_room *new_ptr)
{
	int i;
	otag 	*ot;
	n_otag	*n_ot;
	n_otag  *n_ot_prev;
	xtag 	*xt;
	n_xtag	*n_xt;
	n_xtag	*n_xt_prev;

	if(verbose)
		printf("Room: %d\n", rom_ptr->rom_num);
	else
		printf(".");

	new_ptr->rom_num = rom_ptr->rom_num;
	strcpy(new_ptr->name, rom_ptr->name);
	new_ptr->short_desc=rom_ptr->short_desc;
	new_ptr->long_desc=rom_ptr->long_desc;
	new_ptr->obj_desc=rom_ptr->obj_desc;
	new_ptr->lolevel = rom_ptr->lolevel;
	new_ptr->hilevel = rom_ptr->hilevel;
	new_ptr->special = rom_ptr->special;
	new_ptr->trap = rom_ptr->trap;
	new_ptr->trapexit = rom_ptr->trapexit;
	strcpy(new_ptr->track, rom_ptr->track);
	for(i=0;i<8;i++)
		new_ptr->flags[i]=rom_ptr->flags[i];
	for(i=0;i<10;i++)
		new_ptr->random[i] = rom_ptr->random[i];
	new_ptr->traffic = rom_ptr->traffic;
	for(i=0;i<10;i++) {
		new_ptr->perm_mon[i].misc = rom_ptr->perm_mon[i].misc;

	    if(rom_ptr->perm_mon[i].misc == 0)
		new_ptr->perm_mon[i].interval = 0;
	    else
		new_ptr->perm_mon[i].interval = (rom_ptr->perm_mon[i].interval * 1.15);

		new_ptr->perm_mon[i].ltime = rom_ptr->perm_mon[i].ltime;

		new_ptr->perm_obj[i].misc = rom_ptr->perm_obj[i].misc;

	    if(rom_ptr->perm_obj[i].misc == 0)
		new_ptr->perm_obj[i].interval = 0;
	    else
		new_ptr->perm_obj[i].interval = rom_ptr->perm_obj[i].interval;

		new_ptr->perm_obj[i].ltime = rom_ptr->perm_obj[i].ltime;
	}
	new_ptr->beenhere = rom_ptr->beenhere;
	new_ptr->established = rom_ptr->established;

	new_ptr->first_mon = 0;
	new_ptr->first_ply = 0;

	xt = rom_ptr->first_ext;
	n_xt = (n_xtag *)malloc(sizeof(n_xtag));
	if(!n_xt) { printf("error in xt malloc\n"); return(1); }
	n_xt_prev = 0;

	if(!xt) {
		new_ptr->first_ext = 0;
	} else {
		
		new_ptr->first_ext = n_xt;

	while(xt) {
		
		n_xt->ext = (n_exit_ *)malloc(sizeof(n_exit_));
		if(!n_xt->ext) {
			printf("error in n_xt->ext malloc\n");
			return(1);
		}
                
                zero(n_xt->ext, sizeof(n_exit_));
                strcat(spaces,"    "); copy_xit(xt->ext, n_xt->ext);
		spaces[strlen(spaces)-4] = 0;
                n_xt->next_tag = 0;
                if(n_xt_prev != 0)
                        n_xt_prev->next_tag = n_xt;
                n_xt_prev = n_xt;

                xt = xt->next_tag;
                if(xt) {
                        n_xt->next_tag = (n_xtag *)malloc(sizeof(n_xtag));
                        if(!n_xt) { printf("error in malloc1\n"); return(1); }
                        n_xt = n_xt->next_tag;
                }
        } /* end while */
        } /* end else */


	ot = rom_ptr->first_obj;
	n_ot = (n_otag *)malloc(sizeof(n_otag));
	if(!n_ot) { printf("error in malloc1\n"); return(1); }
	n_ot_prev = 0;

	if(!ot) {
		new_ptr->first_obj = 0;
	} else {
	
		new_ptr->first_obj = n_ot;

	while(ot) {

		n_ot->obj = (n_object *)malloc(sizeof(n_object));
        	if(!n_ot->obj) {
                	printf("error in malloc\n");
                	return(1);
        	}
        	zero(n_ot->obj, sizeof(n_object));
		strcat(spaces,"    "); copy_obj(ot->obj, n_ot->obj);
		spaces[strlen(spaces)-4] = 0;
		n_ot->obj->parent_rom = new_ptr;
		n_ot->next_tag = 0;
		if(n_ot_prev != 0)
			n_ot_prev->next_tag = n_ot;
		n_ot_prev = n_ot;

		ot = ot->next_tag;
		if(ot) {
			n_ot->next_tag = (n_otag *)malloc(sizeof(n_otag));
			if(!n_ot) { printf("error in malloc1\n"); return(1); } 
			n_ot = n_ot->next_tag;
		}
	} /* end while */
	} /* end else */

	new_ptr->special1 = 0;
	new_ptr->special2 = 0;
	return(0);
}

int copy_obj(object *obj_ptr, n_object *new_ptr)
{
	int i;
	otag *ot;
	n_otag *n_ot;
	n_otag *n_ot_prev;

	if(verbose) 
		printf("%sObj : %s\n", spaces,obj_ptr->name);
	else
		printf(".");

	strcpy(new_ptr->name, obj_ptr->name);
	strcpy(new_ptr->description, obj_ptr->description);
	for(i=0;i<3;i++)
		strcpy(new_ptr->key[i], obj_ptr->key[i]);
	strcpy(new_ptr->use_output, obj_ptr->use_output);
	new_ptr->value=obj_ptr->value;
	new_ptr->weight=obj_ptr->weight;
	new_ptr->type=obj_ptr->type;
	new_ptr->adjustment=obj_ptr->adjustment;
	new_ptr->shotsmax=obj_ptr->shotsmax;
	new_ptr->shotscur=obj_ptr->shotscur;
	new_ptr->ndice=obj_ptr->ndice;
	new_ptr->sdice=obj_ptr->sdice;
	new_ptr->pdice=obj_ptr->pdice;
	new_ptr->armor=obj_ptr->armor;
	new_ptr->wearflag=obj_ptr->wearflag;
	new_ptr->magicpower=obj_ptr->magicpower;
	new_ptr->magicrealm=obj_ptr->magicrealm;
	new_ptr->special=obj_ptr->special;
	for(i=0;i<8;i++)
		new_ptr->flags[i]=obj_ptr->flags[i];
	new_ptr->questnum=obj_ptr->questnum;

	new_ptr->parent_rom=0;
	new_ptr->parent_crt=0;

	if(F_ISSET(obj_ptr, OCONTN)) {

		ot = obj_ptr->first_obj;
		n_ot = (n_otag *)malloc(sizeof(n_otag));
		if(!n_ot) { printf("error in malloc1\n"); return(1); }
        	n_ot_prev = 0;

        	if(!ot) {
                	new_ptr->first_obj = 0;
        	} else {
                	new_ptr->first_obj = n_ot;
			strcat(spaces, "    ");
		while(ot) {

			n_ot->obj = (n_object *)malloc(sizeof(n_object));
                	if(!n_ot->obj) {
                        	printf("error in malloc\n");
                        	return(1);
                	}			
			zero(n_ot->obj, sizeof(n_object));
                copy_obj(ot->obj, n_ot->obj);
		n_ot->obj->parent_crt = new_ptr->parent_crt;
		n_ot->obj->parent_obj = new_ptr;
                n_ot->next_tag = 0;
                if(n_ot_prev != 0)
                        n_ot_prev->next_tag = n_ot;
                n_ot_prev = n_ot;
		ot = ot->next_tag;
		if(ot) {
                        n_ot->next_tag = (n_otag *)malloc(sizeof(n_otag));
                        if(!n_ot) { printf("error in malloc1\n"); return(1); } 
                        n_ot = n_ot->next_tag;
                }
		} /* end while */
	spaces[strlen(spaces)-4] = 0;
	} /* end else */
}  else  				/* not a container */
	new_ptr->first_obj = 0;
	
	new_ptr->special1 = 0;
	new_ptr->special2 = 0;
	return(0);
}	
	

int copy_ply(creature *ply_ptr, n_creature *new_ptr)
{
	int i;
	otag *ot;
	n_otag *n_ot,*n_ot_prev;
	n_object *n_obj_ptr;

	if(verbose)
		printf("%sCrt : %s\n", spaces, ply_ptr->name);
	else
		printf(".");


	strcpy(new_ptr->name,ply_ptr->name);
	strcpy(new_ptr->description, ply_ptr->description);
	strcpy(new_ptr->password, ply_ptr->password);
	strcpy(new_ptr->talk, ply_ptr->talk);
	for(i=0;i<=3;i++) 
		strcpy(new_ptr->key[i], ply_ptr->key[i]);
	new_ptr->fd=ply_ptr->fd;
	new_ptr->level=ply_ptr->level;
	new_ptr->type=ply_ptr->type;
	new_ptr->class=ply_ptr->class;
	new_ptr->race=ply_ptr->race;
	new_ptr->numwander=ply_ptr->numwander;
	new_ptr->alignment=ply_ptr->alignment;
	new_ptr->strength=ply_ptr->strength;
	new_ptr->dexterity=ply_ptr->dexterity;
	new_ptr->constitution=ply_ptr->constitution;
	new_ptr->intelligence=ply_ptr->intelligence;
	new_ptr->piety=ply_ptr->piety;
	new_ptr->hpmax=ply_ptr->hpmax;
	new_ptr->hpcur=ply_ptr->hpcur;
	new_ptr->mpmax=ply_ptr->mpmax;
	new_ptr->mpcur=ply_ptr->mpcur;
	new_ptr->armor=ply_ptr->armor;
	new_ptr->thaco=ply_ptr->thaco;
	new_ptr->experience=ply_ptr->experience;
	new_ptr->gold=ply_ptr->gold;
	new_ptr->ndice=ply_ptr->ndice;
	new_ptr->sdice=ply_ptr->sdice;
	new_ptr->pdice=ply_ptr->pdice;
	new_ptr->special=ply_ptr->special;
	for(i=0;i<5;i++)
		new_ptr->proficiency[i]=ply_ptr->proficiency[i];

	if(ply_ptr->class == MONK){
		new_ptr->proficiency[5]=ply_ptr->proficiency[BLUNT];
		new_ptr->proficiency[BLUNT]=1024L;
	} else 
		new_ptr->proficiency[5] = 1024L;
		

	for(i=0;i<4;i++)
		new_ptr->realm[i]=ply_ptr->realm[i];
	for(i=4;i<8;i++)
		new_ptr->realm[i]=0;
	strcpy(new_ptr->spells, ply_ptr->spells); 
	for(i=0;i<8;i++)
		new_ptr->flags[i]=ply_ptr->flags[i]; 
	strcpy(new_ptr->quests, ply_ptr->quests);
	new_ptr->questnum=ply_ptr->questnum;
	for(i=0;i<10;i++)
		new_ptr->carry[i]=ply_ptr->carry[i];
	new_ptr->bank_balance = (long) ply_ptr->carry[1];
	new_ptr->rom_num=ply_ptr->rom_num;
	for(i=0;i<MAXWEAR;i++)
		new_ptr->ready[i] = ply_ptr->ready[i];

	for(i=0;i<10;i++) {
		new_ptr->daily[i].max = ply_ptr->daily[i].max;
		new_ptr->daily[i].cur = ply_ptr->daily[i].cur;
		new_ptr->daily[i].ltime = ply_ptr->daily[i].ltime;
	}
	for(i=0;i<45;i++) {
		new_ptr->lasttime[i].interval = ply_ptr->lasttime[i].interval;
		new_ptr->lasttime[i].ltime = ply_ptr->lasttime[i].ltime;
		new_ptr->lasttime[i].misc = ply_ptr->lasttime[i].misc;
	}

	for(i=45;i<65;i++) {
		new_ptr->lasttime[i].interval = 0;
		new_ptr->lasttime[i].ltime = 0;
		new_ptr->lasttime[i].misc = 0;
	}

	new_ptr->following = NULL;
	new_ptr->first_fol = NULL;

	ot = ply_ptr->first_obj;
	n_ot = (n_otag *)malloc(sizeof(n_otag));
	if(!n_ot) { printf("error in malloc1\n"); return(1); }
	n_ot_prev = 0;

	if(!ot) {
		new_ptr->first_obj = 0;
	} else {
	
		new_ptr->first_obj = n_ot;

	while(ot) {

		n_ot->obj = (n_object *)malloc(sizeof(n_object));
        	if(!n_ot->obj) {
                	printf("error in malloc\n");
                	return(1);
        	}
        	zero(n_ot->obj, sizeof(n_object));
		strcpy(spaces,"    "); 
		copy_obj(ot->obj, n_ot->obj);
		spaces[strlen(spaces)-4] = 0;
		n_ot->obj->parent_crt = new_ptr;
		n_ot->next_tag = 0;
		if(n_ot_prev != 0)
			n_ot_prev->next_tag = n_ot;
		n_ot_prev = n_ot;

		ot = ot->next_tag;
		if(ot) {
			n_ot->next_tag = (n_otag *)malloc(sizeof(n_otag));
			if(!n_ot) { printf("error in malloc1\n"); return(1); } 
			n_ot = n_ot->next_tag;
		}
	} /* end while */
	} /* end else */

	new_ptr->first_enm = NULL;
	new_ptr->first_tlk = ply_ptr->first_tlk;
	new_ptr->parent_rom = NULL;

	new_ptr->special1 = 0;
	new_ptr->special2 = 0;

	return(0);

}

int copy_xit(exit_ *ext_ptr, n_exit_ *new_ptr)
{

	int i;

	if(verbose)
		printf("%sExit: %s\n", spaces, ext_ptr->name);
	else
		printf(".");

	strcpy(new_ptr->name, ext_ptr->name);
	new_ptr->room = ext_ptr->room;
	for(i=0;i<4;i++)
		new_ptr->flags[i]=ext_ptr->flags[i];
	
	new_ptr->ltime.interval = ext_ptr->ltime.interval;
	new_ptr->ltime.ltime = ext_ptr->ltime.ltime;
	new_ptr->ltime.misc = ext_ptr->ltime.misc;

	new_ptr->random.interval = 0;
	new_ptr->random.ltime = 0;
	new_ptr->random.misc = 0;
	
	sprintf(new_ptr->rand_mesg[0],"");
	sprintf(new_ptr->rand_mesg[1],"");
	new_ptr->key = ext_ptr->key;

	return(0);
}
