#include "mstruct.h" 
#include "mextern.h" 
#include <time.h>
#include <stdio.h>

typedef struct node {
	int num;
	char name[80];
	long beenhere;
	long established;
	float freq;
	struct node *next;
} node;

node *Top;
long Time;

main(argc, argv)
int argc;
char *argv[];
{
	int n;
	room *rom_ptr;
	int Lo, Hi, Start, End;

	Time = time(0);

	if(argc < 2) {
		printf("Syntax: %s firstroom:lastroom\n", argv[0]);
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

	for(n=Lo; n<=Hi; n++) {
		if(load_rom_from_file(n, &rom_ptr) < 0) continue;
		printf("."); fflush(stdout);
		add_rom(rom_ptr);
		free_rom(rom_ptr);
	}

	printf("\n");
	list_rom();

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

add_rom(rom_ptr)
room	*rom_ptr;
{
	node *this_node, *current, *prev;
	
	this_node = (node *)malloc(sizeof(node));
	strncpy(this_node->name, rom_ptr->name, 79);
	this_node->num = rom_ptr->rom_num;
	this_node->established = rom_ptr->established;
	this_node->beenhere = rom_ptr->beenhere;
	this_node->freq = (float)rom_ptr->beenhere / 
		((Time-rom_ptr->established)/3600.0);
	this_node->next = 0;

	current = Top;
	prev = 0;
	while(current && current->beenhere > this_node->beenhere) {
		prev = current;
		current = current->next;
	}

	if(!prev) {
		this_node->next = Top;
		Top = this_node;
	}
	else {
		this_node->next = prev->next;
		prev->next = this_node;
	}
}

list_rom()
{
	node *current;
	int num = 0;

	current = Top;
	while(current) {
		printf("%4d. (#%04d) %20.20s %7.2f/hour %5ld total\n", ++num, current->num, current->name, current->freq, current->beenhere);
		current = current->next;
	}
	
}
