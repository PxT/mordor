/***************************************************************************
*  Mordor Database Utilities:
*	These functions allow for read/write access to Mordor MUD
*  database files.
*
*	(c) 1996 Brett Vickers & Brooke Paul
*/

#include "mstruct.h"
#include "mtype.h"
#include "mextern.h"

int load_rom_from_file(num, rom_ptr)
int     num;
room    **rom_ptr;
{
        int fd;
        long n;
        char file[80];

        *rom_ptr = (room *)malloc(sizeof(room));
        if(!*rom_ptr)
                merror("load_from_file", FATAL);
        zero(*rom_ptr, sizeof(room));

#ifdef TC
        sprintf(file, "%s\\r%05d", ROOMPATH, num);
#else
        sprintf(file, "%s/r%05d", ROOMPATH, num);
#endif
        fd = open(file, O_RDONLY | O_BINARY, 0);
        if(fd < 0)
                return(-1);
        n = read_rom(fd, *rom_ptr);
        if(n < 0) {
                close(fd);
                return(-1);   
        }
        close(fd);

        return(0);
}

int load_crt_from_file(num, crt_ptr)
int             num;
creature        **crt_ptr;
{
        int fd;
        long n;
        char file[80];
                        
        *crt_ptr = (creature *)malloc(sizeof(creature));
        if(!*crt_ptr)
                merror("load_from_file", FATAL);
        zero(*crt_ptr, sizeof(creature));

#ifdef TC
        sprintf(file, "%s\\m%02d", MONPATH, num/MFILESIZE);
#else
        sprintf(file, "%s/m%02d", MONPATH, num/MFILESIZE);
#endif
        fd = open(file, O_RDONLY | O_BINARY, 0);
        if(fd < 0)
                return(-1);
        n = lseek(fd, (long)((num%MFILESIZE)*sizeof(creature)), 0);
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

int load_obj_from_file(num, obj_ptr)
int     num;
object  **obj_ptr;
{
        int fd;
        long n;
        char file[80];
                        
        *obj_ptr = (object *)malloc(sizeof(object));
        if(!*obj_ptr)
                merror("load_from_file", FATAL);
        zero(*obj_ptr, sizeof(object));

#ifdef TC
        sprintf(file, "%s\\o%02d", OBJPATH, num/OFILESIZE);
#else
        sprintf(file, "%s/o%02d", OBJPATH, num/OFILESIZE);
#endif
        fd = open(file, O_RDONLY | O_BINARY, 0);
        if(fd < 0)
                return(-1);
        n = lseek(fd, (long)((num%OFILESIZE)*sizeof(object)), 0);
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
