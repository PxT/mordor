#include "mstruct.h"
#include "mextern.h"
#include <time.h>
#include <stdio.h>

#define gets(x)		fgets(x, 10, stdin)

/* void merror(str)
char *str;
{
	printf("error %s\n", str);
	exit(-1);
}
*/
main(argc, argv)
int argc;
char *argv[];
{
	int n;
	creature *crt_ptr;
	int Lo, Hi, Start, End;
	char str[10];

	if(argc < 2) {
		printf("Syntax: %s firstitem:lastitem\n", argv[0]);
		exit(0);
	}

	n = 0;
	while(1) {
		if(argv[1][n] == ':') {
			argv[1][n] = 0;
			Start = atoi(argv[1]);
			End = atoi(&argv[1][n+1]);
			break;
		}
		else if(!argv[1][n]) {
		printf("Syntax: %s low:hi firstroom:lastroom\n", argv[0]);
		exit(0);
		}
		else n++;
	}

	for(n=Start; n<=End; n++) {
		if(load_mon(n, &crt_ptr) < 0) continue;
		printf("%2d. %-20s Wt: %-4d.  New alignment: ", n,
			crt_ptr->name, crt_ptr->alignment);
		gets(str);
		if(atoi(str)) crt_ptr->alignment = atoi(str);
		save_mon(n, crt_ptr);
		free_crt(crt_ptr);
	}

}

int load_obj(index, obj_ptr)
short	index;
object	**obj_ptr;
{
	int	fd;
	char	file[256];

	sprintf(file, "%s/o%02d", OBJPATH, index/100);
	fd = open(file, O_RDONLY, 0);
	if(fd < 0)
		return(-1);
	*obj_ptr = (object *)malloc(sizeof(object));
	if(!*obj_ptr)
		exit(-1);
	lseek(fd, (long)(index%100)*(long)sizeof(object), 0);
	if(read(fd, *obj_ptr, sizeof(object)) < sizeof(object)) {
		close(fd);
		return(-1);
	}
	close(fd);
	return(0);
}

int save_obj(index, obj_ptr)
int index;
object *obj_ptr;
{
	int	fd;
	char	file[256];

	sprintf(file, "%s/o%02d", OBJPATH, index/100);
	fd = open(file, O_RDWR, 0);
	if(fd < 0)
		return(-1);

	lseek(fd, (long)(index%100)*(long)sizeof(object), 0);
	write(fd, obj_ptr, sizeof(object));
	close(fd);
	return(0);
}

int load_mon(index, crt_ptr)
short index;
creature **crt_ptr;
{
	int	fd;
	char	file[256];

	sprintf(file, "%s/m%02d", MONPATH, index/100);
	fd = open(file, O_RDONLY, 0);
	if(fd < 0)
		return(-1);
	*crt_ptr = (creature *)malloc(sizeof(creature));
	if(!*crt_ptr)
		exit(-1);
	lseek(fd, (long)(index%100)*(long)sizeof(creature), 0);
	if(read(fd, *crt_ptr, sizeof(creature)) < sizeof(creature)) {
		close(fd);
		return(-1);
	}
	close(fd);
	return(0);
}

int save_mon(index, crt_ptr)
int index;
creature *crt_ptr;
{
	int	fd;
	char	file[256];

	sprintf(file, "%s/m%02d", MONPATH, index/100);
	fd = open(file, O_RDWR, 0);
	if(fd < 0)
		return(-1);

	lseek(fd, (long)(index%100)*(long)sizeof(creature), 0);
	write(fd, crt_ptr, sizeof(creature));
	close(fd);
	return(0);
}

