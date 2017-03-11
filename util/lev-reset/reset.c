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
	int n, start, end;
	room *rom_ptr;
	long t;

	t = time(0);

	if(argc < 3) {
		printf("syntax: reset [low] [high]\n");
		exit(0);
	}

	start = atoi(argv[1]);
	end = atoi(argv[2]);

	for(n=start; n<=end; n++) {
		if(load_rom(n, &rom_ptr) < 0) continue;
		rom_ptr->established = t;
		rom_ptr->beenhere = 0L;
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
     int	fd, rtn;
	char	file[256];

	sprintf(file, "%s/r%05d", ROOMPATH, rom_ptr->rom_num);
	fd = open(file, O_RDWR, 0);
	if(fd < 0)
		return(-1);

	rtn = write_rom(fd, rom_ptr, 0);
	close(fd);
	return rtn;
	
}
