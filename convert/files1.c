/*
 * Hacked versions of the standard mordb routines
 *
 * $Id$
 *
 * $Log$
 */

#include <stdlib.h>
#include <stdio.h>
#include "oldheaders/include/port.h"
#include "oldheaders/include/morutil.h"
#include "oldheaders/include/mordb.h"
#include "convert.h"



int count_inv2( n_creature *crt_ptr, char perm_only )
{
        n_otag    *op;
        int     total=0;

        op = crt_ptr->first_obj;
        while(op) {
                if(!perm_only || (perm_only && (F_ISSET(op->obj, OPERMT))))
                        total++;
                op = op->next_tag;
        }
        return(total);
}

int save_ply_to_file2(char *str, n_creature *ply_ptr)
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
    a_buf = (char *)malloc(80000);
    if(!a_buf)
                merror("Memory allocation", FATAL );
    n = write_crt_to_mem2(a_buf, ply_ptr, SAVE_ALL);
    if(n > 80000)
                merror("Player too large", FATAL );
    b_buf = (char *)malloc(n);
    if (!b_buf)
                merror("Memory allocation", FATAL );
    size = compress(a_buf, b_buf, n);
    n = write(fd, b_buf, size);
    free(a_buf);
    free(b_buf);
#else
    n = write_crt(fd, ply_ptr, SAVE_ALL);
    if(n < 0) {
        close(fd);
        return(-1);
    }
#endif

    close(fd);

    return(0);
}

int write_ply2(char *str, n_creature *ply_ptr )
{
        char    file[256], filebak[256];
        int     fd, n;
#ifdef COMPRESS
        char    *a_buf, *b_buf;
        int     size;
#endif

        sprintf(file, "%s/%s", get_player_path(), str);
        sprintf(filebak, "%s~", file);
        rename(file, filebak);
        fd = open(file, O_RDWR | O_CREAT | O_BINARY, ACC);
        if(fd < 0) {
                rename(filebak, file);
                return(-1);
        }

#ifdef COMPRESS
        a_buf = (char *)malloc(100000);
        if(!a_buf)
                merror("Memory allocation", FATAL);
        n = write_crt_to_mem2(a_buf, ply_ptr, SAVE_ALL);
        if(n > 100000)
                merror(ply_ptr->name, FATAL);
        b_buf = (char *)malloc(n);
        if(!b_buf)
                merror("Memory allocation", FATAL);
        size = compress(a_buf, b_buf, n);
        n = write(fd, b_buf, size);
        free(a_buf);
        free(b_buf);
#else
        n = write_crt(fd, ply_ptr, SAVE_ALL);
        if(n < 0) {
                close(fd);
                unlink(file);
                rename(filebak, file);
                return(-1);
        }
#endif

        close(fd);
        unlink(filebak);
        return(0);

}

int write_crt_to_mem2(char *buf, n_creature *crt_ptr, char perm_only )
{
        int     n, cnt, cnt2=0, error=0;
        char    *bufstart;
        n_otag    *op;

        bufstart = buf;

        memcpy(buf, crt_ptr, sizeof(n_creature));
        buf += sizeof(n_creature);

        cnt = count_inv2(crt_ptr, perm_only);
        memcpy(buf, &cnt, sizeof(int));
        buf += sizeof(int);

        if(cnt > 0) {
                op = crt_ptr->first_obj;
                while(op && cnt2<cnt) {
                        if(!perm_only || (perm_only &&
                           (F_ISSET(op->obj, OPERMT) ||
                           F_ISSET(op->obj, OPERM2)))) {
                                if((n = write_obj_to_mem2(buf, op->obj,
                                    perm_only)) < 0)
                                        error = 1;
                                else
                                        buf += n;
                                cnt2++;
                        }
                        op = op->next_tag;
                }
        }

        if(cnt != cnt2 || error)
                return(-1);
        else
                return(buf - bufstart);
}

int save_rom_to_file2(int num, n_room *rom_ptr)
{
        int fd;
        long n;
        char file[256];
        char str[512];


        get_room_filename( num, file );

	strcat(file, ".new");
        unlink(file);
        fd = open(file, O_RDWR | O_CREAT | O_BINARY, ACC);
        if(fd < 0)
        {
                sprintf(str, "Could not create room file: %s", file);
                elog(str);
                return(-1);
        }

        n = write_rom2(fd, rom_ptr, SAVE_ALL);
        if(n < 0) {
                sprintf(str, "write_rom() failed on saving file: %s", file);
                elog(str);
                close(fd);
                return(-1);
        }
        close(fd);

        return(0);
}

int write_rom2(int fd, n_room *rom_ptr, char perm_only )
{
        int     n, cnt, error=0;
        xtag    *xp;
        n_ctag    *cp;
        n_otag    *op;
        char    pcontain;

        n = write(fd, rom_ptr, sizeof(n_room));
        if(n < sizeof(n_room))
                merror("write_rom", FATAL);

        cnt = count_ext2(rom_ptr);
        n = write(fd, &cnt, sizeof(int));
        if(n < sizeof(int))
                merror("write_rom", FATAL);

        xp = rom_ptr->first_ext;
        while(xp) {
                n = write(fd, xp->ext, sizeof(n_exit_));
                if(n < sizeof(n_exit_))
                        merror("write_rom", FATAL);
                xp = xp->next_tag;
        }
        cnt = count_mon2(rom_ptr, perm_only);
        n = write(fd, &cnt, sizeof(int));
        if(n < sizeof(int))
                merror("write_rom", FATAL);

        cp = rom_ptr->first_mon;
        while(cp) {
                if(!perm_only || (perm_only && F_ISSET(cp->crt, MPERMT)))
                        if(write_crt2(fd, cp->crt, 0) < 0)
                                error = 1;
                cp = cp->next_tag;
        }

        cnt = count_ite2(rom_ptr, perm_only);
        n = write(fd, &cnt, sizeof(int));
        if(n < sizeof(int))
                merror("write_rom", FATAL);

        op = rom_ptr->first_obj;
    while(op) {
        if(!perm_only || (perm_only && (F_ISSET(op->obj, OPERMT)))){
            if (F_ISSET(op->obj,OCONTN)
                    && (F_ISSET(op->obj, OPERMT)))
                pcontain =  ALLITEMS;
            else
                pcontain = perm_only;
                if(write_obj2(fd, op->obj, pcontain) < 0)
                            error = 1;
        }
        op = op->next_tag;
        }

        if(!rom_ptr->short_desc)
                cnt = 0;
        else
                cnt = strlen(rom_ptr->short_desc) + 1;
        n = write(fd, &cnt, sizeof(int));
        if(n < sizeof(int))
                merror("write_rom", FATAL);

        if(cnt) {
                n = write(fd, rom_ptr->short_desc, cnt);
                if(n < cnt)
                        merror("write_rom", FATAL);
        }

        if(!rom_ptr->long_desc)
                cnt = 0;
        else
                cnt = strlen(rom_ptr->long_desc) + 1;
        n = write(fd, &cnt, sizeof(int));
        if(n < sizeof(int))
                merror("write_rom", FATAL);

        if(cnt) {
                n = write(fd, rom_ptr->long_desc, cnt);
                if(n < cnt)
                        merror("write_rom", FATAL);
        }

        if(!rom_ptr->obj_desc)
                cnt = 0;
        else
                cnt = strlen(rom_ptr->obj_desc) + 1;
        n = write(fd, &cnt, sizeof(int));
        if(n < sizeof(int))
                merror("write_rom", FATAL);

        if(cnt) {
                n = write(fd, rom_ptr->obj_desc, cnt);
                if(n < cnt)
                        merror("write_rom", FATAL);
        }

        if(error)
                return(-1);
        else
                return(0);
}

int save_obj_to_file2(int num, n_object  *obj_ptr)
{
    int fd;
    long n;
    char file[256];

    sprintf(file, "%s/new.o%02d", get_object_path(), num/OFILESIZE);
    fd = open(file, O_RDWR | O_BINARY );
    if(fd < 0) {
        fd = open(file, O_RDWR | O_CREAT | O_BINARY, ACC);
        if(fd < 0)
            return(-1);
    }
    n = lseek(fd, (long)((num%OFILESIZE)*sizeof(n_object)), 0);
    if(n < 0L) {
            close(fd);
        return(-1);
    }
    n = write(fd, obj_ptr, sizeof(n_object));
    if(n < sizeof(n_object)) {
        close(fd);
        return(-1);
    }
    close(fd);

    return(0);
}

int save_crt_to_file2(int num, n_creature *crt_ptr)
{
    int fd;
    long n;
    char file[256];

    sprintf(file, "%s/new.m%02d", get_monster_path(), num/MFILESIZE);
    fd = open(file, O_RDWR | O_BINARY );
    if(fd < 0) {
        fd = open(file, O_RDWR | O_CREAT | O_BINARY, ACC);
        if(fd < 0)
            return(-1);
    }
    n = lseek(fd, (long)((num%MFILESIZE)*sizeof(n_creature)), 0);
    if(n < 0L) {
        close(fd);
        return(-1);
    }
    n = write(fd, crt_ptr, sizeof(n_creature));
    if(n < sizeof(n_creature)) {
        close(fd);
        return(-1);
    }
    close(fd);

    return(0);
}

int count_ext2( n_room *rom_ptr )
{
        xtag    *xp;
        int     total = 0;

        xp = rom_ptr->first_ext;
        while(xp) {
                total++;
                xp = xp->next_tag;
        }
        return(total);
}

int count_ite2(n_room *rom_ptr, char perm_only )
{
        n_otag    *op;
        int     total=0;

        op = rom_ptr->first_obj;
        while(op) {
                if(!perm_only || (perm_only && (F_ISSET(op->obj, OPERMT))))
                        total++;
                op = op->next_tag;
        }
        return(total);
}

int count_mon2( n_room *rom_ptr, char      perm_only )
{
        n_ctag    *cp;
        int     total=0;

        cp = rom_ptr->first_mon;
        while(cp) {
                if(!perm_only || (perm_only && F_ISSET(cp->crt, MPERMT)))
                        total++;
                cp = cp->next_tag;
        }
        return(total);
}

int count_ply2( n_room *rom_ptr )
{
        n_ctag    *pp;
        int     total = 0;

        pp = rom_ptr->first_ply;
        while(pp) {
                total++;
                pp = pp->next_tag;
        }
        return(total);
}

int write_crt2(int fd, n_creature *crt_ptr, char perm_only )
{
        int     n, cnt, cnt2=0, error=0;
        n_otag    *op;

        n = write(fd, crt_ptr, sizeof(n_creature));
        if(n < sizeof(n_creature))
                merror("write_crt", FATAL);

        cnt = count_inv2(crt_ptr, perm_only);
        n = write(fd, &cnt, sizeof(int));
        if(n < sizeof(int))
                merror("write_crt", FATAL);

        if(cnt > 0) {
                op = crt_ptr->first_obj;
                while(op && cnt2<cnt) {
                        if(!perm_only || (perm_only &&
                           (F_ISSET(op->obj, OPERMT)))) {
                                if(write_obj2(fd, op->obj, perm_only) < 0)
                                        error = 1;
                                cnt2++;
                        }
                        op = op->next_tag;
                }
        }

        if(cnt != cnt2 || error)
                return(-1);
        else
                return(0);
}

int write_obj2(int fd, n_object *obj_ptr, char perm_only )
{
        int     n, cnt, cnt2=0, error=0;
        n_otag    *op;

        n = write(fd, obj_ptr, sizeof(n_object));
        if(n < sizeof(n_object))
                merror("write_obj", FATAL);

        cnt = count_obj2(obj_ptr, perm_only);
        n = write(fd, &cnt, sizeof(int));
        if(n < sizeof(int))
                merror("write_obj", FATAL);

        if(cnt > 0) {
                op = obj_ptr->first_obj;
                while(op) {
                        if(!perm_only || (perm_only &&
                           (F_ISSET(op->obj, OPERMT)))) {
                                if(write_obj2(fd, op->obj, perm_only) < 0)
                                        error = 1;
                                cnt2++;
                        }
                        op = op->next_tag;
                }
        }

        if(cnt != cnt2 || error)
                return(-1);
        else
                return(0);

}

int count_obj2( n_object *obj_ptr, char perm_only )
{
        n_otag    *op;
        int     total=0;

        op = obj_ptr->first_obj;
        while(op) {
                if(!perm_only || (perm_only && (F_ISSET(op->obj, OPERMT))))
                        total++;
                op = op->next_tag;
        }
        return(total);
}

int write_obj_to_mem2(char *buf, n_object *obj_ptr, char perm_only )
{
        int     n, cnt, cnt2=0, error=0;
        char    *bufstart;
        n_otag    *op;

        bufstart = buf;

        memcpy(buf, obj_ptr, sizeof(n_object));
        buf += sizeof(n_object);

        cnt = count_obj2(obj_ptr, perm_only);
        memcpy(buf, &cnt, sizeof(int));
        buf += sizeof(int);

        if(cnt > 0) {
                op = obj_ptr->first_obj;
                while(op) {
                        if(!perm_only || (perm_only &&
                           (F_ISSET(op->obj, OPERMT) ||
                           F_ISSET(op->obj, OPERM2)))) {
                                if((n = write_obj_to_mem2(buf,
                                    op->obj, perm_only)) < 0)
                                        error = 1;
                                else
                                        buf += n;
                                cnt2++;
                        }
                        op = op->next_tag;
                }
        }

        if(cnt != cnt2 || error)
                return(-1);
        else
                return(buf - bufstart);
}
