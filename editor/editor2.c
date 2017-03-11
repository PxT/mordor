/*
 * EDITOR2.C:
 *
 *      Object, creature, room and player editor routines (cont'd).
 *
 *      Copyright (C) 1991 Brett J. Vickers
 *	Copyright (C) 1992, 1999 Brooke Paul
 *
 */

#include <stdlib.h>
#include <stdio.h>


#include "../include/port.h"
#include "../include/morutil.h"
#include "../include/mordb.h"


#include "editor.h"


void edit_player()
{
	int 		i, k,x;
	long 		j;
	char		str[256];
	creature	*ply_ptr;

	clearscreen();
	posit(3,20); printf("Enter player name to edit: ");
	getstr(str, "", 80);
	if(!str[0]) return;
	
	load_ply_from_file(str, &ply_ptr);
	ply_ptr->type = 0;			/* player */

	while(1) {
		clearscreen();
		posit(2,25);  printf("Player: %s",ply_ptr->name);
		posit(3,30);  printf(" 1. Edit name/password");
		posit(4,30);  printf(" 2. Edit level/class/align/stats/race");
		posit(5,30);  printf(" 3. Edit HP/MP/armor/thaco");
		posit(6,30);  printf(" 4. Edit exp/gold/dmg/room");
		posit(7,30);  printf(" 5. Edit proficiencies");
		posit(8,30);  printf(" 6. Edit spells");
		posit(9,30);  printf(" 7. Edit flags");
		posit(10,30); printf(" 8. Edit daily-use variables");
		posit(11,30); printf(" 9. Edit last-time variables");
		posit(12,30); printf("10. Edit 2nd page of last-time");
		posit(13,30); printf("11. Edit 3rd page of last-time");
		posit(14,30); printf("12. Edit quests fulfilled");
		posit(15,30); printf("13. Edit inventory");
		posit(16,30); printf("14. Clear this player");
		posit(17,30); printf("15. Abort this player");
		posit(18,30); printf("16. Back to main menu");
		posit(20,30); printf("Choice: ");

		getnum(&i, 1, 16);

		switch(i) {
		case 1:			/* name */
			clearscreen();
			posit(2,1); printf("Name:");
			posit(3,1); printf("%s",ply_ptr->name);
			posit(5,1); printf("Password: %s",ply_ptr->password);
			posit(6,1); printf("Description: %s", ply_ptr->description);
			posit(3,1); getstr(str, ply_ptr->name, 80);
			strcpy(ply_ptr->name, str);
			posit(5,11); getstr(str, ply_ptr->password, 15);
			strcpy(ply_ptr->password, str);
			posit(6,14); getstr(str,ply_ptr->description,80);
			strcpy(ply_ptr->description,str);
			break;
		case 2:			/* level/class/align/stats */
			clearscreen();
			posit(2,1); printf("Level: %-3d", ply_ptr->level);
			posit(2,20); printf("Class: %-3d", ply_ptr->class);
			posit(2,40); printf("Align: %-5d", ply_ptr->alignment);
			posit(3,1); printf("  Str: %-2d", ply_ptr->strength);
			posit(3,20); printf("  Dex: %-2d", ply_ptr->dexterity);
			posit(3,40); printf("  Con: %-2d", ply_ptr->constitution);
			posit(4,1); printf("  Int: %-2d", ply_ptr->intelligence);
			posit(4,20); printf("  Pty: %-2d", ply_ptr->piety);
			posit(5,1); printf(" Race: %-3d", ply_ptr->race);
			posit(2,8); getnums(&j, (long)ply_ptr->level, 0L, 127L);
			ply_ptr->level = j;
			posit(2,27); getnums(&j, (long)ply_ptr->class, -127L, 127L);
			ply_ptr->class = j;
			posit(2,47); getnums(&j, (long)ply_ptr->alignment, -30000L, 30000L);
			ply_ptr->alignment = j;
			posit(3,8); getnums(&j, (long)ply_ptr->strength, 0L, 25L);
			ply_ptr->strength = j;
			posit(3,27); getnums(&j, (long)ply_ptr->dexterity, 0L, 25L);
			ply_ptr->dexterity = j;
			posit(3,47); getnums(&j, (long)ply_ptr->constitution, 0L, 25L);
			ply_ptr->constitution = j;
			posit(4,8); getnums(&j, (long)ply_ptr->intelligence, 0L, 25L);
			ply_ptr->intelligence = j;
			posit(4,27); getnums(&j, (long)ply_ptr->piety, 0L, 25L);
			ply_ptr->piety = j;
			posit(5,8); getnums(&j, (long)ply_ptr->race, 0L, 10L);
			ply_ptr->race = j;
			break;
		case 3:			/* HP/MP/Armor/Thac0 */
			clearscreen();
			posit(2,1); printf("   HP: %-5d/%-5d", ply_ptr->hpcur, ply_ptr->hpmax);
			posit(2,30); printf("   MP: %-5d/%-5d", ply_ptr->mpcur, ply_ptr->mpmax);
			posit(3,1); printf("Armor: %-3d", ply_ptr->armor);
			posit(3,30); printf("Thac0: %-3d", ply_ptr->thaco);
			posit(2,8); getnums(&j, (long)ply_ptr->hpcur, 0L, 32000L);
			ply_ptr->hpcur = j;
			posit(2,14); getnums(&j, (long)ply_ptr->hpmax, 0L, 32000L);
			ply_ptr->hpmax = j;
			posit(2,37); getnums(&j, (long)ply_ptr->mpcur, 0L, 32000L);
			ply_ptr->mpcur = j;
			posit(2,43); getnums(&j, (long)ply_ptr->mpmax, 0L, 32000L);
			ply_ptr->mpmax = j;
			posit(3,8); getnums(&j, (long)ply_ptr->armor, -127L, 127L);
			ply_ptr->armor = j;
			posit(3,37); getnums(&j, (long)ply_ptr->thaco, -127L, 127L);
			ply_ptr->thaco = j;
			break;
		case 4:			/* exp/gold/dmg/room */
			clearscreen();
			posit(2,1); printf("Experience: %ld", ply_ptr->experience);
			posit(2,30); printf("  Gold: %ld", ply_ptr->gold);
			posit(3,1); printf("    # Dice: %-5d", ply_ptr->ndice);
			posit(3,30); printf(" Sides: %-5d", ply_ptr->sdice);
			posit(3,60); printf("  Plus: %-5d", ply_ptr->pdice);
			posit(4,1); printf("      Room: %-5d", ply_ptr->rom_num);
			posit(2,13); getnums(&j, (long)ply_ptr->experience, 0L, 10000000L);
			ply_ptr->experience = j;
			posit(2,38); getnums(&j, (long)ply_ptr->gold, 0L, 20000000L);
			ply_ptr->gold = j;
			posit(3,13); getnums(&j, (long)ply_ptr->ndice, 0L, 32000L);
			ply_ptr->ndice = j;
			posit(3,38); getnums(&j, (long)ply_ptr->sdice, 0L, 32000L);
			ply_ptr->sdice = j;
			posit(3,68); getnums(&j, (long)ply_ptr->pdice, 0L, 32000L);
			ply_ptr->pdice = j;
			posit(4,13); getnums(&j, (long)ply_ptr->rom_num, 0L, 32000L);
			ply_ptr->rom_num = j;
			break;
		case 5:			/* proficiencies */
			clearscreen();
			posit(2,1); printf("Proficiencies:");
			posit(3,1); printf("1:%ld", ply_ptr->proficiency[0]);
			posit(3,27); printf("2:%ld", ply_ptr->proficiency[1]);
			posit(3,54); printf("3:%ld", ply_ptr->proficiency[2]);
			posit(4,1); printf("4:%ld", ply_ptr->proficiency[3]);
			posit(4,27); printf("5:%ld", ply_ptr->proficiency[4]);
			posit(6,1); printf("Realms:");
			posit(7,1); printf("1:%ld", ply_ptr->realm[0]);
			posit(7,27); printf("2:%ld", ply_ptr->realm[1]);
			posit(7,54); printf("3:%ld", ply_ptr->realm[2]);
			posit(8,1); printf("4:%ld", ply_ptr->realm[3]);
			posit(3,3); getnums(&j, (long)ply_ptr->proficiency[0], 0L, 1000000L);
			ply_ptr->proficiency[0] = j;
			posit(3,29); getnums(&j, (long)ply_ptr->proficiency[1], 0L, 1000000L);
			ply_ptr->proficiency[1] = j;
			posit(3,56); getnums(&j, (long)ply_ptr->proficiency[2], 0L, 1000000L);
			ply_ptr->proficiency[2] = j;
			posit(4,3); getnums(&j, (long)ply_ptr->proficiency[3], 0L, 1000000L);
			ply_ptr->proficiency[3] = j;
			posit(4,29); getnums(&j, (long)ply_ptr->proficiency[4], 0L, 1000000L);
			ply_ptr->proficiency[4] = j;
			posit(7,3); getnums(&j, (long)ply_ptr->realm[0], 0L, 1000000L);
			ply_ptr->realm[0] = j;
			posit(7,29); getnums(&j, (long)ply_ptr->realm[1], 0L, 1000000L);
			ply_ptr->realm[1] = j;
			posit(7,56); getnums(&j, (long)ply_ptr->realm[2], 0L, 1000000L);
			ply_ptr->realm[2] = j;
			posit(8,3); getnums(&j, (long)ply_ptr->realm[3], 0L, 1000000L);
			ply_ptr->realm[3] = j;
			break;
		case 6:		   	/* spells */
			clearscreen();
			posit(2,10); printf("1");
			posit(2,20); printf("2");
			posit(2,30); printf("3");
			posit(2,40); printf("4");
			posit(2,50); printf("5");
			posit(2,60); printf("6");
			posit(2,70); printf("7");
			posit(7,10); printf("8");
			posit(7,20); printf("9");
			posit(7,30); printf("0");
			posit(7,40); printf("1");
			posit(7,50); printf("2");
			posit(6,30); printf("1");
			posit(6,40); printf("1");
			posit(6,50); printf("1");
			posit(3,1); printf("1234567890123456789012345678901234567890123456789012345678901234567890");
			posit(8,1); printf("1234567890123456789012345678901234567890123456789012345678");
			posit(11,1); printf("Spell #: ");
			posit(12,1); printf("(0 to exit)");
			while(1) {
				for(j=0;j<128;j++)
					str[j] = (ply_ptr->spells[j/8] & 1<<(j%8)) ? '*':'.';
				str[128] = 0; j = str[70];
				str[70] = 0;
				posit(4,1); printf("%s",str);
				str[70] = j;
				posit(9,1); printf("%s",&str[70]);
				posit(11,10); getnum(&k, 0, 128);
				if(!k) break;
				if(ply_ptr->spells[(k-1)/8] & 1<<((k-1)%8))
					ply_ptr->spells[(k-1)/8] &= ~(1<<((k-1)%8));
				else
					ply_ptr->spells[(k-1)/8] |= 1<<((k-1)%8);
			}
			break;			
		case 7:			/* flags */
			clearscreen();
			posit(2,10); printf("1");
			posit(2,20); printf("2");
			posit(2,30); printf("3");
			posit(2,40); printf("4");
			posit(2,50); printf("5");
			posit(2,60); printf("6");
			posit(3,1); printf("1234567890123456789012345678901234567890123456789012345678901234");
			posit(7,1); printf("Flag #: ");
			posit(8,1); printf("(0 to exit)");
			while(1) {
				for(j=0;j<64;j++)
					str[j] = (ply_ptr->flags[j/8] & 1<<(j%8)) ? '*':'.';
				str[64] = 0;
				posit(4,1); printf("%s",str);
				posit(7,9); getnum(&k, 0, 64);
				if(!k) break;
				if(ply_ptr->flags[(k-1)/8] & 1<<((k-1)%8))
					ply_ptr->flags[(k-1)/8] &= ~(1<<((k-1)%8));
				else
					ply_ptr->flags[(k-1)/8] |= 1<<((k-1)%8);
			}
			break;
		case 8:			/* Daily-use variables */
			clearscreen();
			posit(2,1); printf("Daily-use variables:");
			posit(4,10); printf("Max");
			posit(4,20); printf("Current");
			posit(4,30); printf("Last Time");
			for(x=0; x<10; x++) {
				posit(x+5, 2); printf("%2d:",x+1);
				posit(x+5, 10); printf("%d",ply_ptr->daily[x].max);
				posit(x+5, 20); printf("%d",ply_ptr->daily[x].cur);
				posit(x+5, 30); printf("%lu",ply_ptr->daily[x].ltime);
			}
			posit(17,1); printf("Daily #:");
			while(1) {
				posit(17,10); getnum(&x, 0, 10);
				if(!x) break;
				posit(x+4, 10); getnums(&j, (long)ply_ptr->daily[x-1].max, -127L, 127L);
				ply_ptr->daily[x-1].max = j;
				posit(x+4, 20); getnums(&j, (long)ply_ptr->daily[x-1].cur, -127L, 127L);
				ply_ptr->daily[x-1].cur = j;
				posit(x+4, 30); getnums(&j, (long)ply_ptr->daily[x-1].ltime, 0L, 10000000L);
				ply_ptr->daily[x-1].ltime = j;
			}
			break;
		case 9:			/* Last-time variables */
			clearscreen();
			posit(2,1); printf("Last-time variables:");
			posit(4,10); printf("Interval");
			posit(4,20); printf("Lastreset");
			posit(4,30); printf("Misc");
			for(x=0; x<15; x++) {
				posit(x+5, 2); printf("%2d:",x+1);
				posit(x+5, 10); printf("%lu",ply_ptr->lasttime[x].interval);
				posit(x+5, 20); printf("%lu",ply_ptr->lasttime[x].ltime);
				posit(x+5, 30); printf("%-5d",ply_ptr->lasttime[x].misc);
			}
			posit(22,1); printf("Variable #:");
			while(1) {
				posit(22,13); getnum(&x, 0, 15);
				if(!x) break;
				posit(x+4, 10); getnums(&j, (long)ply_ptr->lasttime[x-1].interval, 0L, 1000000000L);
				ply_ptr->lasttime[x-1].interval = j;
				posit(x+4, 20); getnums(&j, (long)ply_ptr->lasttime[x-1].ltime, 0L, 1000000000L);
				ply_ptr->lasttime[x-1].ltime = j;
				posit(x+4, 30); getnums(&j, (long)ply_ptr->lasttime[x-1].misc, 0L, 32000L);
				ply_ptr->lasttime[x-1].misc = j;
			}
			break;
		case 10: 		/* page 2 of lasttime */
			clearscreen();
			posit(2,1); printf("Last-time variables:");
			posit(4,10); printf("Interval");
			posit(4,20); printf("Lastreset");
			posit(4,30); printf("Misc");
			for(x=15; x<30; x++) {
				posit(x-10, 2); printf("%2d:",x+1);
				posit(x-10, 10); printf("%lu",ply_ptr->lasttime[x].interval);
				posit(x-10, 20); printf("%lu",ply_ptr->lasttime[x].ltime);
				posit(x-10, 30); printf("%-5d",ply_ptr->lasttime[x].misc);
			}
			posit(22,1); printf("Variable #:");
			while(1) {
				posit(22,13); getnum(&x, 0, 30);
				if(x<16) break;
				posit(x-11, 10); getnums(&j, (long)ply_ptr->lasttime[x-1].interval, 0L, 1000000000L);
				ply_ptr->lasttime[x-1].interval = j;
				posit(x-11, 20); getnums(&j, (long)ply_ptr->lasttime[x-1].ltime, 0L, 1000000000L);
				ply_ptr->lasttime[x-1].ltime = j;
				posit(x-11, 30); getnums(&j, (long)ply_ptr->lasttime[x-1].misc, 0L, 32000L);
				ply_ptr->lasttime[x-1].misc = j;
			}
			break;
		case 11: 		/* page 3 of lasttime */
			clearscreen();
			posit(2,1); printf("Last-time variables:");
			posit(4,10); printf("Interval");
			posit(4,20); printf("Lastreset");
			posit(4,30); printf("Misc");
			for(x=30; x<45; x++) {
				posit(x-25, 2); printf("%2d:",x+1);
				posit(x-25, 10); printf("%lu",ply_ptr->lasttime[x].interval);
				posit(x-25, 20); printf("%lu",ply_ptr->lasttime[x].ltime);
				posit(x-25, 30); printf("%-5d",ply_ptr->lasttime[x].misc);
			}
			posit(22,1); printf("Variable #:");
			while(1) {
				posit(22,13); getnum(&x, 0, 45);
				if(x<31) break;
				posit(x-26, 10); getnums(&j, (long)ply_ptr->lasttime[x-1].interval, 0L, 1000000000L);
				ply_ptr->lasttime[x-1].interval = j;
				posit(x-26, 20); getnums(&j, (long)ply_ptr->lasttime[x-1].ltime, 0L, 1000000000L);
				ply_ptr->lasttime[x-1].ltime = j;
				posit(x-26, 30); getnums(&j, (long)ply_ptr->lasttime[x-1].misc, 0L, 32000L);
				ply_ptr->lasttime[x-1].misc = j;
			}
			break;
		case 12:		/* quests */
			clearscreen();
			posit(2,10); printf("1");
			posit(2,20); printf("2");
			posit(2,30); printf("3");
			posit(2,40); printf("4");
			posit(2,50); printf("5");
			posit(2,60); printf("6");
			posit(2,70); printf("7");
			posit(7,10); printf("8");
			posit(7,20); printf("9");
			posit(7,30); printf("0");
			posit(7,40); printf("1");
			posit(7,50); printf("2");
			posit(6,30); printf("1");
			posit(6,40); printf("1");
			posit(6,50); printf("1");
			posit(3,1); printf("1234567890123456789012345678901234567890123456789012345678901234567890");
			posit(8,1); printf("1234567890123456789012345678901234567890123456789012345678");
			posit(11,1); printf("Quest #: ");
			posit(12,1); printf("(0 to exit)");
			while(1) {
				for(j=0;j<128;j++)
					str[j] = (ply_ptr->quests[j/8] & 1<<(j%8)) ? '*':'.';
				str[128] = 0; j = str[70];
				str[70] = 0;
				posit(4,1); printf("%s",str);
				str[70] = j;
				posit(9,1); printf("%s",&str[70]);
				posit(11,10); getnum(&k, 0, 128);
				if(!k) break;
				if(ply_ptr->quests[(k-1)/8] & 1<<((k-1)%8))
					ply_ptr->quests[(k-1)/8] &= ~(1<<((k-1)%8));
				else
					ply_ptr->quests[(k-1)/8] |= 1<<((k-1)%8);
			}
			break;
		case 13:		/* edit items */
			edit_items(ply_ptr);
			break;
		case 14:		/* abort */
			clearscreen();
			posit(1,1);
			printf("Are you sure you want to clear it? ");
			getstr(str, "n", 2);
			if(str[0]=='y' || str[0]=='Y') {
				free_crt(ply_ptr);
				ply_ptr = (creature *)malloc(sizeof(creature));
				zero(ply_ptr, sizeof(creature));
			}
			break;
		case 15:		/* Abort */
			free_crt(ply_ptr);
			return;
		case 16:		/* Return to main menu */
			if(!ply_ptr->name[0]) break;
			save_ply_to_file(ply_ptr->name, ply_ptr);
			free_crt(ply_ptr);
			return;
		}
	}
}

void edit_items(creature *ply_ptr)
{
	int 	i, x, error,k;
	long 	j;
	object	*obj_ptr;
	otag	*op, *ot, *prev;

	while(1) {
		clearscreen();
		posit(5,25);  printf("Player: %s",ply_ptr->name);
		posit(6,30);  printf("1. Add item");
		posit(7,30);  printf("2. Remove item");
		posit(8,30);  printf("3. List items");
		posit(9,30);  printf("4. Back to player menu");
		posit(11,30); printf("Choice: ");
	
		getnum(&i, 1, 4);
	
		switch(i) {
		case 1: 	/* add */
			clearscreen();
			posit(2,1); printf("Enter item number: ");
			getnum(&k, 0, 32000);
			if(load_obj_from_file((int)k, &obj_ptr) < 0)
				free(obj_ptr);
			else {
				ot = (otag *)malloc(sizeof(otag));
				ot->obj = obj_ptr;
				ot->next_tag = 0;
				op = ply_ptr->first_obj;
				if(!op) {
					ply_ptr->first_obj = ot;
					break;
				}
				if(strcmp(op->obj->name,ot->obj->name) >= 0 ||
				   (strcmp(op->obj->name,ot->obj->name)==0 &&
				   op->obj->adjustment > ot->obj->adjustment) ) {
					ot->next_tag = op;
					ply_ptr->first_obj = ot;
					break;
				}
				while(op) {
					if(strcmp(op->obj->name,ot->obj->name) > 0 || 
					  (strcmp(op->obj->name,ot->obj->name)==0 && 
					  op->obj->adjustment > ot->obj->adjustment) ) break;
					prev = op;
					op = op->next_tag;
				}
				ot->next_tag = prev->next_tag;
				prev->next_tag = ot;
			}
			break;
		case 2:
			clearscreen();
			posit(2,1); printf("Delete item #");
			getnum(&i, 0, 90);
			if(!i) break;
			op = ply_ptr->first_obj; x = 1; error = 0;
			while(x <= i) {
				if(!op) {
					error = 1;
					break;
				}
				if(x == i)
					break;
				x++;
				prev = op;
				op = op->next_tag;
			}
			if(error) break;
			if(op == ply_ptr->first_obj) {
				ply_ptr->first_obj = op->next_tag;
				free_obj(op->obj);
				free(op);
			}
			else {
				prev->next_tag = op->next_tag;
				free_obj(op->obj);
				free(op);
			}
			break;
		case 3:			/* list */
			clearscreen();
			op = ply_ptr->first_obj; x = 1;
			while(op) {
				posit(x%24, (x/24>0)?40:1);
				printf("%2d: %s", x, op->obj->name);
				x++;
				op = op->next_tag;
			}
			posit(24,40); printf("Hit Enter:");
			getnum(&x, 0, 0);
			break;
		case 4:
			return;
		}
	}
}
