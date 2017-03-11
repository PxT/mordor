#include "mstruct.h"
#include "mextern.h"
#include <time.h>

void merror(str)
char *str;
{
	printf("error %s\n", str);
	exit(-1);
}

main(argc, argv)
int argc;
char *argv[];
{
	int n;
	room *rom_ptr;
	int Lo, Hi, Start, End;

	if(argc < 3) {
		printf("Syntax: %s low:hi firstroom:lastroom\n", argv[0]);
		exit(0);
	}

	n = 0;
	while(1) {
		if(argv[1][n] == ':') {
			argv[1][n] = 0;
			Lo = atoi(argv[1]);
			Hi = atoi(&argv[1][n+1]);
			break;
		}
		else if(!argv[1][n]) {
		printf("Syntax: %s low:hi firstroom:lastroom\n", argv[0]);
		exit(0);
		}
		else n++;
	}

	n = 0;
	while(1) {
		if(argv[2][n] == ':') {
			argv[2][n] = 0;
			Start = atoi(argv[2]);
			End = atoi(&argv[2][n+1]);
			break;
		}
		else if(!argv[2][n]) {
		printf("Syntax: %s low:hi firstroom:lastroom\n", argv[0]);
		exit(0);
		}
		else n++;
	}

	for(n=Start; n<=End; n++) {
		if(load_rom(n, &rom_ptr) < 0) continue;
		rom_ptr->lolevel = Lo;
		rom_ptr->hilevel = Hi;
		save_rom(rom_ptr);
		printf("Room '%s'(%d) altered.\n", rom_ptr->name, rom_ptr->rom_num);
		free_rom(rom_ptr);
	}

}

int load_rom(index, rom_ptr)
short	index;
room	**rom_ptr;
{
	int	fd;
	char	file[256];

	sprintf(file, "%s/r%05d", ROOMPATH, index);
	fd = open(file, O_RDONLY, 0);
	if(fd < 0)
		return(-1);
	*rom_ptr = (room *)malloc(sizeof(room));
	if(!*rom_ptr)
		exit(-1);
	if(read_rom(fd, *rom_ptr) < 0) {
		close(fd);
		return(-1);
	}
	close(fd);

	(*rom_ptr)->rom_num = index;
	return(0);
}

int save_rom(rom_ptr)
room	*rom_ptr;
{
	int	fd;
	char	file[256];

	sprintf(file, "%s/r%05d", ROOMPATH, rom_ptr->rom_num);
	fd = open(file, O_RDWR, 0);
	if(fd < 0)
		return(-1);

	return(write_rom(fd, rom_ptr, 0));
}
