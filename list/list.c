/*
 * LIST.C:
 *
 *	This program is a utility that lists out monsters, objects and
 *	rooms.
 *
 *	Copyright (C) 1991 Brett J. Vickers
 *	Copyrigth (C) 1993, 1999 Brooke Paul
 * $Id: list.c,v 1.1 2001/08/02 01:10:20 develop Exp develop $
 *
 * $Log: list.c,v $
 * Revision 1.1  2001/08/02 01:10:20  develop
 * Initial revision
 *
 *
 */
#include <stdio.h>
#include <stdlib.h>


#include "../include/port.h"
#include "../include/morutil.h"
#include "../include/mordb.h"


#define PRINTF(a) new_write(Fd, a, strlen(a))
#define MISSMAX 1000


void list_crt();
void list_obj();
void list_rom();
void abort();
void handle_args(int argc, char *argv[]);
void new_write(int fd, char *str, int len);



char	Progtitle[80];
char	Str[240];
int	Rlo = 0, Rhi = 32000, Type = -1, Wear = -1,
	Levlo = 1, Levhi = 50, Quests, Short=0, Obj = -1, Mon = -1;
int	Flag[15], Notflag[15], Spell[15], Spellnum=0,
	Flagnum=0, Notflagnum=0, Fd=1;

int main(int argc, char *argv[])
{
	
	strcpy(Progtitle, argv[0]);

	if(argc < 2)
		abort();

	if(argc > 2) 
		handle_args(argc-2, &argv[2]);

	switch(argv[1][0]) {
	case 'm':
		list_crt();
		break;
	case 'o':
		list_obj();
		break;
	case 'r':
		list_rom();
		break;
	case 'a':
		list_obj();
		list_crt();
		list_rom();
		break;
	default:
		abort();
	}

	return(0);
}

void handle_args(int argc, char *argv[])
{
	char	rangestr[80];
	int	i, j, n;

	Rlo = 0;
	Rhi = 32000;

	for(i=0; i<argc; i++) {
		if(argv[i][0] == 0) continue;

		if(isdigit(argv[i][0])) {
			Rlo = atoi(argv[i]);
			Rhi = Rlo+10;
			continue;
		}

		if(argv[i][0] != '-')
			abort();

		switch(argv[i][1]) {

		case 'r':
			strcpy(rangestr, &argv[i][2]);
			n = strlen(rangestr);
			Rlo = -1; Rhi = -1;
			for(j=1; j<n; j++) {
				if(rangestr[j] == ':') {
					rangestr[j] = 0;
					Rlo = atoi(rangestr);
					Rhi = atoi(&rangestr[j+1]);
				}
			}
			if(Rlo == -1 || Rhi == -1 || Rlo > Rhi)
				abort();
			break;

		case 'l':
			strcpy(rangestr, &argv[i][2]);
			n = strlen(rangestr);
			Levlo = -1; Levhi = -1;
			for(j=1; j<n; j++) {
				if(rangestr[j] == ':') {
					rangestr[j] = 0;
					Levlo = atoi(rangestr);
					Levhi = atoi(&rangestr[j+1]);
				}
			}
			if(Levlo == -1 || Levhi == -1 || Levlo > Levhi)
				abort();
			break;

		case 't':
			Type = atoi(&argv[i][2]);
			break;

		case 'w':
			Wear = atoi(&argv[i][2]);
			break;

		case 'f':
			Flag[Flagnum++] = atoi(&argv[i][2]);
			break;

		case 'F':
			Notflag[Notflagnum++] = atoi(&argv[i][2]);
			break;

		case 'q':
			Quests = 1;
			break;

		case 'o':
			Obj = atoi(&argv[i][2]);
			break;

		case 'm':
			Mon = atoi(&argv[i][2]);
			break;

		case 'S':
			Spell[Spellnum++] = atoi(&argv[i][2]);
			break;

		case 's':
			init_sockets();
			Fd = atoi(&argv[i][2]);
			break;

		case 'z': /* undocumented, used internally by mordor */
			if(Rlo == 0 && Rhi == 32000)
				Short=1;
			break;

		default:
			break;
		}
	}

}

void abort()
{
	PRINTF("list <m|o|r> [options]\n");
	PRINTF("[options]:   #        output 10 slots, starting at #\n");
	PRINTF("            -r#:#     index range\n");
	PRINTF("            -s#       descriptor for output\n");
	PRINTF("            -l#:#     level range\n");
	PRINTF("            -t#       object type\n");
	PRINTF("            -w#       object wearflag\n");
 	PRINTF("            -f#       flag set\n");
 	PRINTF("            -F#       flag NOT set\n");
	PRINTF("            -S#       spell set\n");
	PRINTF("            -q        quest objects only\n");
	PRINTF("            -o#       monsters/rooms carrying object\n");
	PRINTF("            -m#       rooms with monster\n");

	if ( Fd != 1 )
	{
		cleanup_sockets();
	}

	exit(0);
}

#define ISSET(a,b)  ((a).flags[(b)/8] & (1<<((b)%8)))
#define SP_ISSET(a,b)  ((a).spells[(b)/8] & (1<<((b)%8)))

void list_crt()
{
	creature	crt;
	int		i, j, missing=0, count=0,skipped=0, breakout=0, found=0;
	
	sprintf(Str, "%c\n%4s %-20s %-2s %4s %-14s %-3s %-3s %-2s %-5s %-4s %-8s\n", 12, "  # ", "Name", "Lv", "Algn", "Stats", "HP", "AC", "T0", "Exp", "Gold", " Dice");
	new_write(Fd, Str, strlen(Str));
	sprintf(Str, "------------------------------------------------------------------------------\n");
	new_write(Fd, Str, strlen(Str));
	for(i=Rlo; i<=Rhi; i++) {
		if(load_crt_struct_from_file(i, &crt) < 0) {
			missing++;
			if(missing > MISSMAX) break;
			continue;
		}
		if(crt.level < Levlo || crt.level > Levhi) {
			skipped++;
			continue;
		}
		if(Obj != -1) {
			for(j=0; j<10; j++)
				if(crt.carry[j] == Obj) {
					found = 1;
					break;
				}
			if(!found) {
				skipped++;
				continue;
			}
			else found = 0;
		}
		for(j=0; j<Flagnum; j++) {
			if(!ISSET(crt, Flag[j]-1)) {
				skipped++;
				breakout = 1;
				break;
			}
		}
		if(breakout) {
			breakout = 0;
			continue;
		}
		for(j=0; j<Spellnum; j++) {
			if(!SP_ISSET(crt, Spell[j]-1)) {
				skipped++;
				breakout = 1;
				break;
			}
		}
		if(breakout) {
			breakout = 0;
			continue;
		}
		for(j=0; j<Notflagnum; j++) {
			if(ISSET(crt, Notflag[j]-1)) {
				skipped++;
				breakout = 1;
				break;
			}
		}
		if(breakout) {
			breakout = 0;
			continue;
		}
		sprintf(Str, "%3d. %-20.20s %02d %4d %02d/%02d/%02d/%02d/%02d %03d %03d %02d %05ld %04ld %2dd%-2d+%-2d\n", i, crt.name, crt.level, crt.alignment, crt.strength, crt.dexterity, crt.constitution, crt.intelligence, crt.piety, crt.hpmax, crt.armor, crt.thaco, 
crt.experience, crt.gold, crt.ndice, crt.sdice, crt.pdice);
		new_write(Fd, Str, strlen(Str));
		count++;
		if(Short && count == 10)
			return;

		if(!((i-(missing+skipped)-Rlo+1)%50)) {
			sprintf(Str, "%c\n%4s %-20s %-2s %4s %-14s %-3s %-3s %-2s %-5s %-4s %-8s\n", 12, "  # ", "Name", "Lv", "Algn", "Stats", "HP", "AC", "T0", "Exp", "Gold", " Dice");
			new_write(Fd, Str, strlen(Str));
			sprintf(Str, "------------------------------------------------------------------------------\n");
			new_write(Fd, Str, strlen(Str));
		}
	}

}

void list_obj()
{
	object	obj;
	int	i, j, count=0,missing=0, skipped=0, breakout=0;

	sprintf(Str, "%c\n%4s %-20.20s %-6s %-3s %-2s %-3s %-3s %-8s %-2s %-2s %-2s %-3s\n", 12, "  # ", "Name", "Value", "Wgt", "Tp", "Adj", "Sht", " Dice", "AC", "Wr", "Mg", "Qst");
	new_write(Fd, Str, strlen(Str));
	sprintf(Str, "------------------------------------------------------------------------------\n");
	new_write(Fd, Str, strlen(Str));
	for(i=Rlo; i<=Rhi; i++) {
		if(load_obj_struct_from_file(i, &obj) < 0) {
			missing++;
			if(missing > MISSMAX) break;
			continue;
		}
		if(Type != -1 && obj.type != Type) {
			skipped++;
			continue;
		}
		if(Wear != -1 && obj.wearflag != Wear) {
			skipped++;
			continue;
		}
		if(Quests && !obj.questnum) {
			skipped++;
			continue;
		}
		for(j=0; j<Flagnum; j++) {
			if(!ISSET(obj, Flag[j]-1)) {
				skipped++;
				breakout = 1;
				break;
			}
		}
		if(breakout) {
			breakout = 0;
			continue;
		}
		for(j=0; j<Notflagnum; j++) {
			if(ISSET(obj, Notflag[j]-1)) {
				skipped++;
				breakout = 1;
				break;
			}
		}
		if(breakout) {
			breakout = 0;
			continue;
		}
		sprintf(Str, "%3d. %-20.20s %06ld %03d %02d +%-2d %03d %2dd%-2d+%-2d %02d %02d %02d %03d\n", i, obj.name, obj.value, obj.weight, obj.type, obj.adjustment, obj.shotsmax, obj.ndice, obj.sdice, obj.pdice, obj.armor, obj.wearflag, obj.magicpower, obj.questnum);
		new_write(Fd, Str, strlen(Str));
		count++;
		if(Short && count == 10)
			return;

		if(!((i-(missing+skipped)-Rlo+1)%50)) {
			sprintf(Str,"%c\n%4s %-20s %-6s %-3s %-2s %-3s %-3s %-8s %-2s %-2s %-2s %-3s\n", 12, "  # ", "Name", "Value", "Wgt", "Tp", "Adj", "Sht", " Dice", "AC", "Wr", "Mg", "Qst");
			new_write(Fd, Str, strlen(Str));
			sprintf(Str, "------------------------------------------------------------------------------\n");
			new_write(Fd, Str, strlen(Str));
		}
	}

}

void list_rom()
{
	int	i, j, count=0, missing=0, breakout=0, skipped=0, found=0;
	room	rom;

	sprintf(Str, "%c\n%4s %-20s %-39s %-4s\n", 12, "  # ", "Name", "Random Monsters", "Traf");
	new_write(Fd, Str, strlen(Str));
	sprintf(Str,"------------------------------------------------------------------------------\n");
	new_write(Fd, Str, strlen(Str));
	for(i=Rlo; i<=Rhi; i++) {
		if(load_rom_struct_from_file(i, &rom) < 0) {
			missing++;
			if(missing > MISSMAX && i>8999) break;
			continue;
		}
		for(j=0; j<Flagnum; j++) {
			if(!ISSET(rom, Flag[j]-1)) {
				skipped++;
				breakout = 1;
				break;
			}
		}
		if(breakout) {
			breakout = 0;
			continue;
		}
		for(j=0; j<Notflagnum; j++) {
			if(ISSET(rom, Notflag[j]-1)) {
				skipped++;
				breakout = 1;
				break;
			}
		}
		if(breakout) {
			breakout = 0;
			continue;
		}
		if(Obj != -1) {
			for(j=0; j<10; j++)
				if(rom.perm_obj[j].misc == Obj) {
					found = 1;
					break;
				}
			if(!found) {
				skipped++;
				continue;
			}
			else found = 0;
		}
		if(Mon != -1) {
			for(j=0; j<10; j++)
				if(rom.random[j] == Mon) {
					found = 1;
					break;
				}
			for(j=0; j<10; j++)
				if(rom.perm_mon[j].misc == Mon) {
					found = 1;
					break;
				}
			if(!found) {
				skipped++;
				continue;
			}
			else found = 0;
		}
		rom.name[20]=0;
		sprintf(Str, "%3d. %-20.20s %03d/%03d/%03d/%03d/%03d/%03d/%03d/%03d/%03d/%03d %03d%%\n", i, rom.name, rom.random[0], rom.random[1], rom.random[2], rom.random[3], rom.random[4], rom.random[5], rom.random[6], rom.random[7], rom.random[8], rom.random[9], rom.traffic);
		new_write(Fd, Str, strlen(Str));
		count++;
		if(Short && count == 10)
			return;

		if(!((i-(missing+skipped)-Rlo+1)%50)) {
			sprintf(Str, "%c\n%4s %-20s %-39s %-4s\n", 12, "  # ", "Name", "Random Monsters", "Traf");
			new_write(Fd, Str, strlen(Str));
			sprintf(Str, "------------------------------------------------------------------------------\n");
			new_write(Fd, Str, strlen(Str));
		}
	}
}





void new_write(int fd, char *str, int len)
{
	if ( fd == 1 )
	{
		write( fd, str, len);
	}
	else
	{
		scwrite(fd, str, len);
	}

	return;
}
