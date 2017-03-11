/***************************************************************************
*  Mordor Database Utilities:
*	These functions allow for read/write access to Mordor MUD
*  database files.
*
*	(c) 1996 Brooke Paul
* $Id: files4.c,v 1.3 2001/03/09 05:10:09 develop Exp $
*
* $Log: files4.c,v $
* Revision 1.3  2001/03/09 05:10:09  develop
* potential fix for memory overflow when saving players
*
* Revision 1.2  2001/03/08 16:05:42  develop
* *** empty log message ***
*
*/
#include <stdio.h>
#include <stdlib.h>

#include "../include/morutil.h"

#include "compress.h"
#include "mordb.h"

int load_rom_from_file(int num, room **rom_ptr )
{
    int fd;
    char file[256];

	get_room_filename(num, file );

	fd = open(file, O_RDWR | O_BINARY );
	if(fd < 0)
		return(-1);
	*rom_ptr = (room *)malloc(sizeof(room));
	if(!*rom_ptr)
		merror("load_rom", FATAL);
	if(read_rom(fd, *rom_ptr) < 0) {
		close(fd);
		return(-1);
	}
	close(fd);

	(*rom_ptr)->rom_num = num;

    return(0);
}


/****************************************************************/
/*	load_rom_struct_from_file()				*/
/*								*/
/* NOTE: this function does allocate any memory so does not	*/
/* require a corresponding free call.  It does not load a	*/
/* fully formed room.						*/
/* It is meant for very fast lookups only.			*/
/* JPF								*/
/****************************************************************/
int load_rom_struct_from_file(int num, room *rom_ptr )
{
    int fd;
    char file[256];
	int ret = -1;
	int	n;

	zero( rom_ptr, sizeof(room) );

	get_room_filename( num, file );

	fd = open(file, O_RDONLY | O_BINARY);

	if(fd >= 0) 
	{
		n = read( fd, rom_ptr, sizeof(room) );
		if ( n == sizeof(room) )
		{
			ret = 0;
		}

	close( fd );
	}


	return(ret);
}




int load_crt_from_file(int num, creature **crt_ptr )
{
	int fd;
	int	n;
	char file[256];

#ifdef TC
    sprintf(file, "%s\\m%02d", get_monster_path(), num/MFILESIZE);
#else
    sprintf(file, "%s/m%02d", get_monster_path(), num/MFILESIZE);
#endif
	fd = open(file, O_RDONLY | O_BINARY );
	if(fd < 0) {
		*crt_ptr = 0;
		return(-1);
	}
	*crt_ptr = (creature *)malloc(sizeof(creature));
	if(!*crt_ptr)
		merror("load_crt", FATAL);
	n = lseek(fd, (long)((num%MFILESIZE)*sizeof(creature)), 0);
	if(n < 0L) {
		free(*crt_ptr);
		close(fd);
		*crt_ptr = 0;
		return(-1);
	}

	if ( read_single_crt(fd, *crt_ptr ) != 0 )
	{
		free(*crt_ptr);
		close(fd);
		*crt_ptr = 0;
		return(-1);
	}

	(*crt_ptr)->fd = -1;

	sprintf((*crt_ptr)->password, "%d", num);

	close(fd);

	return(0);
}


/****************************************************************/
/*	load_crt_struct_from_file()				*/
/*								*/
/* NOTE: this function does allocate any memory so does not	*/
/* require a corresponding free call.  It does not load a	*/
/* fully formed creature.					*/
/* It is meant for very fast lookups only.			*/
/* JPF								*/
/****************************************************************/
int load_crt_struct_from_file(int num, creature *crt_ptr )
{
	int fd;
	int	n;
	char file[256];
	int ret = -1;

	zero( crt_ptr, sizeof(creature ) );

#ifdef TC
    sprintf(file, "%s\\m%02d", get_monster_path(), num/MFILESIZE);
#else
    sprintf(file, "%s/m%02d", get_monster_path(), num/MFILESIZE);
#endif
	fd = open(file, O_RDONLY | O_BINARY);
	if(fd >= 0) 
	{

		n = lseek(fd, (long)((num%MFILESIZE)*sizeof(creature)), 0);
		if(n >= 0L) 
		{
			n = read( fd, crt_ptr, sizeof(creature) );
			if ( n == sizeof(creature) )
			{
				ret = 0;
			}
		}

		close( fd );
	}


	return(ret);
}


int load_obj_from_file(int num, object  **obj_ptr )
{
    int fd;
    long n;
    char file[256];
    int	ret = 0;                

#ifdef TC
    sprintf(file, "%s\\o%02d", get_object_path(), num/OFILESIZE);
#else
	sprintf(file, "%s/o%02d", get_object_path(), num/OFILESIZE);
#endif

	fd = open(file, O_RDONLY | O_BINARY );
	if(fd < 0)
		return(-1);
	*obj_ptr = (object *)malloc(sizeof(object));
	if(!*obj_ptr)
		merror("load_obj", FATAL);
	n = lseek(fd, (long)((num%OFILESIZE)*sizeof(object)), 0);
	if(n < 0L) {
		close(fd);
		return(-1);
	}

	if ( read_single_obj(fd, *obj_ptr ) != 0 )
	{
		free(*obj_ptr);
		*obj_ptr = 0;
		ret = -1;
	}


	close(fd);

    return(ret);   
}



/****************************************************************/
/*	load_obj_struct_from_file()				*/
/*								*/
/* NOTE: this function does not allocate any memory so does not	*/
/* require a corresponding free call.  It does not load a	*/
/* fully formed object.						*/
/* It is meant for very fast lookups only.			*/
/* JPF								*/
/****************************************************************/
int load_obj_struct_from_file(int num, object *obj_ptr )
{
	int fd;
	int	n;
	char file[256];
	int ret = -1;

	zero( obj_ptr, sizeof(object ) );

#ifdef TC
    sprintf(file, "%s\\o%02d", get_object_path(), num/OFILESIZE);
#else
	sprintf(file, "%s/o%02d", get_object_path(), num/OFILESIZE);
#endif

	fd = open(file, O_RDONLY | O_BINARY );
	if(fd >= 0) 
	{
		n = lseek(fd, (long)((num%OFILESIZE)*sizeof(object)), 0);
		if(n >= 0L) 
		{
			n = read( fd, obj_ptr, sizeof(object) );
			if ( n == sizeof(object) )
			{
				ret = 0;
			}
		}

		close( fd );
	}


	return(ret);
}



int load_ply_from_file(char *str, creature **ply_ptr)
{
	char	file[256];
	int	fd, n;
	int	ret = 0;

#ifdef COMPRESS
	char	*a_buf, *b_buf;
	int	size;
#endif

	sprintf(file, "%s/%s", get_player_path(), str);
	fd = open(file, O_RDONLY | O_BINARY );
	if(fd < 0) 
		return(-1);

	*ply_ptr = (creature *)malloc(sizeof(creature));
	if(!*ply_ptr)
		merror("load_ply", FATAL);

#ifdef COMPRESS
	a_buf = (char *)malloc(50000);
	if(!a_buf) merror("Memory allocation", FATAL);
	size = read(fd, a_buf, 50000);
	if(size >= 50000) 
		merror("Player too large", FATAL);
	if(size < 1) {
		close(fd);
		return(-1);
	}
	b_buf = (char *)malloc(100000);
	if(!b_buf) merror("Memory allocation", FATAL);
	n = uncompress(a_buf, b_buf, size);
	if(n > 100000) merror("Player too large", FATAL);
	n = read_crt_from_mem(b_buf, *ply_ptr );
	free(a_buf);
	free(b_buf);
#else
    n = read_crt(fd, ply_ptr);
    if(n < 0) {
		*ply_ptr = 0;
		ret = -1;
    }
#endif

    close(fd);
            
    return(ret);
}





int save_rom_to_file(int num, room *rom_ptr)
{
	int fd;
	long n; 
	char file[256];
	char str[512];


	get_room_filename( num, file );

	unlink(file);
	fd = open(file, O_RDWR | O_CREAT | O_BINARY, ACC);
	if(fd < 0)
	{
		sprintf(str, "Could not create room file: %s", file);
		elog(str);
		return(-1);
	}

	n = write_rom(fd, rom_ptr, 0);
	if(n < 0) {
		sprintf(str, "write_rom() failed on saving file: %s", file);
		elog(str);
		close(fd);
		return(-1);
	}
	close(fd);

	return(0);
}



int save_obj_to_file(int num, object  *obj_ptr)
{
    int fd;
    long n;
    char file[256];

    sprintf(file, "%s/o%02d", get_object_path(), num/OFILESIZE);
    fd = open(file, O_RDWR | O_BINARY );
    if(fd < 0) {
        fd = open(file, O_RDWR | O_CREAT | O_BINARY, ACC);
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

int save_crt_to_file(int num, creature *crt_ptr)
{
    int fd;
    long n;
    char file[256];

    sprintf(file, "%s/m%02d", get_monster_path(), num/MFILESIZE);
    fd = open(file, O_RDWR | O_BINARY );
    if(fd < 0) {
        fd = open(file, O_RDWR | O_CREAT | O_BINARY, ACC);
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

int save_ply_to_file(char *str, creature *ply_ptr)
{
    int fd;
    long n;
    char file[256];
#ifdef COMPRESS
    char *a_buf, *b_buf;
    int size;
#endif

    sprintf(file, "%s/%s", get_player_path(), str);
    unlink(file);
    fd = open(file, O_RDWR | O_CREAT | O_BINARY, ACC);
    if(fd < 0)
        return(-1);

#ifdef COMPRESS
    a_buf = (char *)malloc(100000);
    if(!a_buf) 
		merror("Memory allocation", FATAL );
    n = write_crt_to_mem(a_buf, ply_ptr, 0);
    if(n > 100000) 
		merror("Player too large", FATAL );
    if(n < 0) {
	free(a_buf);
	close(fd);
	return(-1);
    }

    b_buf = (char *)malloc(n);
    if (!b_buf) 
		merror("Memory allocation", FATAL );
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

