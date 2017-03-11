
#include "mstruct.h"
#include "mextern.h"

int load_rom_from_file(file, rom_ptr)
room	**rom_ptr;
char	*file;
{
	int fd;
	long n;

	*rom_ptr = (room *)malloc(sizeof(room));
	if(!*rom_ptr)
		merror("load_from_file", FATAL);
	zero(*rom_ptr, sizeof(room));

	fd = open(file, O_RDONLY | O_BINARY, 0);
	if(fd < 0) 
		return(-1);
	n = read_rom(fd, *rom_ptr);
	if(n < 0) {
		close(fd);
		return(-1);
	}
	if (atoi(file+1)) (*rom_ptr)->rom_num = atoi(file+1);
	close(fd);

	return(0);
}

int save_rom_to_file(num, rom_ptr)
int 	num;
room	*rom_ptr;
{
	int fd;
	long n;
	char file[80];

	sprintf(file, "%s/r%05d", ROOMPATH, num);
	unlink(file);
	fd = open(file, O_RDWR | O_CREAT, 00666);
	if(fd < 0)
		return(-1);

	n = write_rom(fd, rom_ptr);
	if(n < 0) {
		close(fd);
		return(-1);
	}
	close(fd);

	return(0);
}

int load_ply_from_file(str, ply_ptr)
char		*str;
creature	**ply_ptr;
{
	int fd;
	long n;
	char file[80];
#ifdef COMPRESS
	char	*a_buf, *b_buf;
	int	size;
#endif

	*ply_ptr = (creature *)malloc(sizeof(creature));
	if(!*ply_ptr)
		merror("load_from_file", FATAL);
	zero(*ply_ptr, sizeof(creature));

	sprintf(file, "%s/%s", PLAYERPATH, str);
	fd = open(file, O_RDONLY | O_BINARY, 0);
	if(fd < 0) 
		return(-1);

#ifdef COMPRESS
	a_buf = (char *)malloc(30000);
	if(!a_buf) merror("Memory allocation", FATAL);
	size = read(fd, a_buf, 30000);
	if(size >= 30000) merror("Player too large", FATAL);
	if(size < 1) {
		close(fd);
		return(-1);
	}
	b_buf = (char *)malloc(80000);
	if(!b_buf) merror("Memory allocation", FATAL);
	n = uncompress(a_buf, b_buf, size);
	if(n > 80000) merror("Player too large", FATAL);
	n = read_crt_from_mem(b_buf, *ply_ptr, 0);
	free(a_buf);
	free(b_buf);
#else
	n = read_crt(fd, *ply_ptr);
	if(n < 0) {
		close(fd);
		return(-1);
	}
#endif
	close(fd);

	return(0);
}

int save_ply_to_file(str, ply_ptr)
char		*str;
creature	*ply_ptr;
{
	int fd;
	long n;
	char file[80];
#ifdef COMPRESS
	char	*a_buf, *b_buf;
	int	size;
#endif

	sprintf(file, "%s/%s", PLAYERPATH, str);
	unlink(file);
	fd = open(file, O_RDWR | O_CREAT, 00666);
	if(fd < 0)
		return(-1);

#ifdef COMPRESS
	a_buf = (char *)malloc(80000);
	if(!a_buf) merror("Memory allocation", FATAL);
	n = write_crt_to_mem(a_buf, ply_ptr, 0);
	if(n > 80000) merror("Player too large", FATAL);
	b_buf = (char *)malloc(n);
	if(!b_buf) merror("Memory allocation", FATAL);
	size = compress(a_buf, b_buf, n);
	n = write(fd, b_buf, size);
	free(a_buf);
	free(b_buf);
#else
	n = write_crt(fd, ply_ptr, 0);
	if(n < 0) {
		close(fd);
		return(-1);
	}
#endif
	close(fd);

	return(0);
}

int load_obj_from_file(num, obj_ptr, file)
/* num specifies the relative object # in the file named by file */
int 	num;
object 	**obj_ptr;
char	*file;
{
	int fd;
	long n;

	*obj_ptr = (object *)malloc(sizeof(object));
	if(!*obj_ptr)
		merror("load_from_file", FATAL);
	zero(*obj_ptr, sizeof(object));

	fd = open(file, O_RDONLY | O_BINARY, 0);
	if(fd < 0) 
		return(-1);
	n = lseek(fd, (long)((num)*sizeof(object)), 0);
	if(n < 0L) {
		close(fd);
		return(-1);
	}
	n = read(fd, *obj_ptr, sizeof(object));
	if(n < sizeof(object)) {
		close(fd);
		return(-1);
	}
	close(fd);

	return(0);
}

int save_obj_to_file(num, obj_ptr)
int 	num;
object  *obj_ptr;
{
	int fd;
	long n;
	char file[80];
	sprintf(file, "%s/o%02d", OBJPATH, num/OFILESIZE);
	fd = open(file, O_RDWR | O_BINARY, 0);
	if(fd < 0) {
		fd = open(file, O_RDWR | O_CREAT, 00666);
		if(fd < 0)
			return(-1);
	}
	n = lseek(fd, (long)((num%OFILESIZE)*sizeof(object)), 0);
	if(n < 0L) {
		close(fd);
		return(-1);
	}
	n = write(fd, obj_ptr, sizeof(object));
	if(n < sizeof(object)) {
		close(fd);
		return(-1);
	}
	close(fd);

	return(0);
}

int load_crt_from_file(num, crt_ptr, file)
int 		num;
creature 	**crt_ptr;
char		*file;
{
	int fd;
	long n;

	*crt_ptr = (creature *)malloc(sizeof(creature));
	if(!*crt_ptr)
		merror("load_from_file", FATAL);
	zero(*crt_ptr, sizeof(creature));
	fd = open(file, O_RDONLY | O_BINARY, 0);
	if(fd < 0) 
		return(-1);
	n = lseek(fd, (long)((num)*sizeof(creature)), 0);
	if(n < 0L) {
		close(fd);
		return(-1);
	}
	n = read(fd, *crt_ptr, sizeof(creature));
	if(n < sizeof(creature)) {
		close(fd);
		return(-1);
	}
	close(fd);

	return(0);
}

int save_crt_to_file(num, crt_ptr)
int 		num;
creature	*crt_ptr;
{
	int fd;
	long n;
	char file[80];

	sprintf(file, "%s/m%02d", MONPATH, num/MFILESIZE);
	fd = open(file, O_RDWR | O_BINARY, 0);
	if(fd < 0) {
		fd = open(file, O_RDWR | O_CREAT, 00666);
		if(fd < 0)
			return(-1);
	}
	n = lseek(fd, (long)((num%MFILESIZE)*sizeof(creature)), 0);
	if(n < 0L) {
		close(fd);
		return(-1);
	}
	n = write(fd, crt_ptr, sizeof(creature));
	if(n < sizeof(creature)) {
		close(fd);
		return(-1);
	}
	close(fd);

	return(0);
}
