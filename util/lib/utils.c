/***************************************************************************
*  Mordor Database Utilities:
*	These functions allow for read/write access to Mordor MUD
*  database files.
*
*	(c) 1996 Brett Vickers & Brooke Paul
*/

#include "mstruct.h"
#include "mextern.h"
#include "utils.h"

/**********************************************************************/
/*				count_obj			      */
/**********************************************************************/

/* Return the total number of objects contained within an object.  */
/* If perm_only != 0, then only the permanent objects are counted. */

int count_obj(obj_ptr, perm_only)
object 	*obj_ptr;
char 	perm_only;
{
	otag	*op;
	int	total=0;

	op = obj_ptr->first_obj;
	while(op) {
		if(!perm_only || (perm_only && (F_ISSET(op->obj, OPERMT) || 
		   F_ISSET(op->obj, OPERM2))))
			total++;
		op = op->next_tag;
	}
	return(total);
}

/**********************************************************************/
/*				write_obj			      */
/**********************************************************************/

/* Save an object to a file that has already been opened and positioned. */
/* This function recursively saves the items that are contained inside   */
/* the object as well.  If perm_only != 0 then only permanent objects	 */
/* within the object are saved with it.					 */

int write_obj(fd, obj_ptr, perm_only)
int 	fd;
object 	*obj_ptr;
char 	perm_only;
{
	int 	n, cnt, cnt2=0, error=0;
	otag	*op;

	n = write(fd, obj_ptr, sizeof(object));
	if(n < sizeof(object))
		merror("write_obj", FATAL);
	
	cnt = count_obj(obj_ptr, perm_only);
	n = write(fd, &cnt, sizeof(int));
	if(n < sizeof(int))
		merror("write_obj", FATAL);

	if(cnt > 0) {
		op = obj_ptr->first_obj;
		while(op) {
			if(!perm_only || (perm_only && 
			   (F_ISSET(op->obj, OPERMT) || 
			   F_ISSET(op->obj, OPERM2)))) {
				if(write_obj(fd, op->obj, perm_only) < 0)
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

/**********************************************************************/
/*				count_inv			      */
/**********************************************************************/

/* Returns the number of objects a creature has in his inventory. */
/* If perm_only != 0 then only those objects which are permanent  */
/* will be counted.						  */

int count_inv(crt_ptr, perm_only)
creature 	*crt_ptr;
char 		perm_only;
{
	otag	*op;
	int	total=0;

	op = crt_ptr->first_obj;
	while(op) {
		if(!perm_only || (perm_only && (F_ISSET(op->obj, OPERMT) || 
		   F_ISSET(op->obj, OPERM2)))) 
			total++;
		op = op->next_tag;
	}
	return(total);
}

/**********************************************************************/
/*				write_crt			      */
/**********************************************************************/

/* Save a creature to an already open and positioned file.  This function */
/* also saves all the items the creature is holding.  If perm_only != 0   */
/* then only those items which the creature is carrying that are 	  */
/* permanent will be saved.						  */

int write_crt(fd, crt_ptr, perm_only)
int 		fd;
creature 	*crt_ptr;
char 		perm_only;
{
	int 	n, cnt, cnt2=0, error=0;
	otag	*op;

	n = write(fd, crt_ptr, sizeof(creature));
	if(n < sizeof(creature))
		merror("write_crt", FATAL);

	cnt = count_inv(crt_ptr, perm_only);
	n = write(fd, &cnt, sizeof(int));
	if(n < sizeof(int))
		merror("write_crt", FATAL);

	if(cnt > 0) {
		op = crt_ptr->first_obj;
		while(op) {
			if(!perm_only || (perm_only && 
			   (F_ISSET(op->obj, OPERMT) || 
			   F_ISSET(op->obj, OPERM2)))) {
				if(write_obj(fd, op->obj, perm_only) < 0)
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

/**********************************************************************/
/*				count_mon			      */
/**********************************************************************/

/* Returns the number of monsters in a given room.  If perm_only is */
/* nonzero, then only the number of permanent monsters in the room  */
/* is returned.							    */

int count_mon(rom_ptr, perm_only)
room 	*rom_ptr;
char 	perm_only;
{
	ctag	*cp;
	int	total=0;

	cp = rom_ptr->first_mon;
	while(cp) {
		if(!perm_only || (perm_only && F_ISSET(cp->crt, MPERMT)))
			total++;
		cp = cp->next_tag;
	}
	return(total);
}

/**********************************************************************/
/*				count_ite			      */
/**********************************************************************/

/* Returns the number of items in the room.  If perm_only is nonzero */
/* then only the number of permanent items in the room is returned.  */

int count_ite(rom_ptr, perm_only)
room 	*rom_ptr;
char 	perm_only;
{
	otag 	*op;
	int	total=0;

	op = rom_ptr->first_obj;
	while(op) {
		if(!perm_only || (perm_only && (F_ISSET(op->obj, OPERMT) ||
		   F_ISSET(op->obj, OPERM2))))
			total++;
		op = op->next_tag;
	}
	return(total);
}

/**********************************************************************/
/*				count_ext			      */
/**********************************************************************/

/* Returns the number of exits in the room */

int count_ext(rom_ptr)
room 	*rom_ptr;
{
	xtag	*xp;
	int	total = 0;

	xp = rom_ptr->first_ext;
	while(xp) {
		total++;
		xp = xp->next_tag;
	}
	return(total);
}

/**********************************************************************/
/*				count_ply			      */
/**********************************************************************/

/* Returns the number of players in the room */

int count_ply(rom_ptr)
room 	*rom_ptr;
{
	ctag	*pp;
	int	total = 0;

	pp = rom_ptr->first_ply;
	while(pp) {
		total++;
		pp = pp->next_tag;
	}
	return(total);
}

/**********************************************************************/
/*				write_rom			      */
/**********************************************************************/

/* Saves the room pointed to by rom_ptr with all its contents.  Its */
/* contents include exits, descriptions, items and monsters.  If    */
/* perm_only is nonzero, then only items and monsters in the room   */
/* which are permanent will be saved.				    */

int write_rom(fd, rom_ptr, perm_only)
int 	fd;
room 	*rom_ptr;
char 	perm_only;
{
	int 	n, cnt, error=0;
	xtag	*xp;
	ctag	*cp;
	otag	*op;

	n = write(fd, rom_ptr, sizeof(room));
	if(n < sizeof(room))
		merror("write_rom", FATAL);

	cnt = count_ext(rom_ptr);
	n = write(fd, &cnt, sizeof(int));
	if(n < sizeof(int))
		merror("write_rom", FATAL);

	xp = rom_ptr->first_ext;
	while(xp) {
		n = write(fd, xp->ext, sizeof(exit_));
		if(n < sizeof(exit_))
			merror("write_rom", FATAL);
		xp = xp->next_tag;
	}

	cnt = count_mon(rom_ptr, perm_only);
	n = write(fd, &cnt, sizeof(int));
	if(n < sizeof(int))
		merror("write_rom", FATAL);

	cp = rom_ptr->first_mon;
	while(cp) {
		if(!perm_only || (perm_only && F_ISSET(cp->crt, MPERMT)))
			if(write_crt(fd, cp->crt, 0) < 0)
				error = 1;
		cp = cp->next_tag;
	}

	cnt = count_ite(rom_ptr, perm_only);
	n = write(fd, &cnt, sizeof(int));
	if(n < sizeof(int))
		merror("write_rom", FATAL);

	op = rom_ptr->first_obj;
	while(op) {
		if(!perm_only || (perm_only && (F_ISSET(op->obj, OPERMT) ||
		   F_ISSET(op->obj, OPERM2))))
			if(write_obj(fd, op->obj, perm_only) < 0)
				error = 1;
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

/**********************************************************************/
/*				read_obj			      */
/**********************************************************************/

/* Loads the object from the open and positioned file described by fd */
/* and also loads every object which it might contain.  Returns -1 if */
/* there was an error.						      */

int read_obj(fd, obj_ptr)
int 	fd;
object 	*obj_ptr;
{
	int 		n, cnt, error=0;
	otag		*op;
	otag		**prev;
	object 		*obj;

	n = read(fd, obj_ptr, sizeof(object));
	if(n < sizeof(object))
		error = 1;

	obj_ptr->first_obj = 0;
	obj_ptr->parent_obj = 0;
	obj_ptr->parent_rom = 0;
	obj_ptr->parent_crt = 0;
	if(obj_ptr->shotscur > obj_ptr->shotsmax)
		obj_ptr->shotscur = obj_ptr->shotsmax;

	n = read(fd, &cnt, sizeof(int));
	if(n < sizeof(int)) {
		error = 1;
		cnt = 0;
	}

	prev = &obj_ptr->first_obj;
	while(cnt > 0) {
		cnt--;
		op = (otag *)malloc(sizeof(otag));
		if(op) {
			obj = (object *)malloc(sizeof(object));
			if(obj) {
				if(read_obj(fd, obj) < 0)
					error = 1;
				obj->parent_obj = obj_ptr;
				op->obj = obj;
				op->next_tag = 0;
				*prev = op;
				prev = &op->next_tag;
			}
			else
				merror("read_obj", FATAL);
		}
		else
			merror("read_obj", FATAL);
	}

	if(error)
		return(-1);
	else
		return(0);
}

/**********************************************************************/
/*				read_crt			      */
/**********************************************************************/

/* Loads a creature from an open and positioned file.  The creature is */
/* loaded at the mem location specified by the second parameter.  In   */
/* addition, all the creature's objects have memory allocated for them */
/* and are loaded as well.  Returns -1 on fail.			       */

int read_crt(fd, crt_ptr)
int 		fd;
creature 	*crt_ptr;
{
	int 		n, cnt, error=0;
	otag		*op;
	otag		**prev;
	object 		*obj;

	n = read(fd, crt_ptr, sizeof(creature));
	if(n < sizeof(creature))
		error = 1;

	crt_ptr->first_obj = 0;
	crt_ptr->first_fol = 0;
	crt_ptr->first_enm = 0;
	crt_ptr->parent_rom = 0;
	crt_ptr->following = 0;
	for(n=0; n<20; n++)
		crt_ptr->ready[n] = 0;
	if(crt_ptr->mpcur > crt_ptr->mpmax)
		crt_ptr->mpcur = crt_ptr->mpmax;
	if(crt_ptr->hpcur > crt_ptr->hpmax)
		crt_ptr->hpcur = crt_ptr->hpmax;

	n = read(fd, &cnt, sizeof(int));
	if(n < sizeof(int)) {
		error = 1;
		cnt = 0;
	}

	prev = &crt_ptr->first_obj;
	while(cnt > 0) {
		cnt--;
		op = (otag *)malloc(sizeof(otag));
		if(op) {
			obj = (object *)malloc(sizeof(object));
			if(obj) {
				if(read_obj(fd, obj) < 0) {
					printf("error 3\n");
					error = 1;
				}
				obj->parent_crt = crt_ptr;
				op->obj = obj;
				op->next_tag = 0;
				*prev = op;
				prev = &op->next_tag;
			}
			else
				merror("read_crt", FATAL);
		}
		else
			merror("read_crt", FATAL);
	}

	if(error)
		return(-1);
	else
		return(0);
}

/**********************************************************************/
/*				read_rom			      */
/**********************************************************************/

/* Loads a room from an already open and positioned file into the memory */
/* pointed to by the second parameter.  Also loaded are all creatures,   */
/* exits, objects and descriptions for the room.  -1 is returned upon    */
/* failure.								 */

int read_rom(fd, rom_ptr)
int 	fd;
room 	*rom_ptr;
{
	int 		n, cnt, error=0;
	xtag		*xp;
	xtag		**xprev;
	exit_		*ext;
	ctag		*cp;
	ctag		**cprev;
	creature	*crt;
	otag		*op;
	otag		**oprev;
	object 		*obj;
	char		*sh, *lo, *ob;

	n = read(fd, rom_ptr, sizeof(room));
	if(n < sizeof(room))
		error = 1;

	rom_ptr->short_desc = 0;
	rom_ptr->long_desc  = 0;
	rom_ptr->obj_desc   = 0;
	rom_ptr->first_ext = 0;
	rom_ptr->first_obj = 0;
	rom_ptr->first_mon = 0;
	rom_ptr->first_ply = 0;

	n = read(fd, &cnt, sizeof(int));
	if(n < sizeof(int)) {
		error = 1;
		cnt = 0;
	}

	/* Load the exits */

	xprev = &rom_ptr->first_ext;
	while(cnt > 0) {
		cnt--;
		xp = (xtag *)malloc(sizeof(xtag));
		if(xp) {
			ext = (exit_ *)malloc(sizeof(exit_));
			if(ext) {
				n = read(fd, ext, sizeof(exit_));
				if(n < sizeof(exit_))
					error = 1;
				xp->ext = ext;
				xp->next_tag = 0;
				*xprev = xp;
				xprev = &xp->next_tag;
			}
			else
				merror("read_rom", FATAL);
		}
		else
			merror("read_rom", FATAL);
	}

	/* Read the monsters */

	n = read(fd, &cnt, sizeof(int));
	if(n < sizeof(int)) {
		error = 1;
		cnt = 0;
	}

	cprev = &rom_ptr->first_mon;
	while(cnt > 0) {
		cnt--;
		cp = (ctag *)malloc(sizeof(ctag));
		if(cp) {
			crt = (creature *)malloc(sizeof(creature));
			if(crt) {
				if(read_crt(fd, crt) < 0)
					error = 1;
				crt->parent_rom = rom_ptr;
				cp->crt = crt;
				cp->next_tag = 0;
				*cprev = cp;
				cprev = &cp->next_tag;
			}
			else
				merror("read_rom", FATAL);
		}
		else
			merror("read_rom", FATAL);
	}

	/* Read the items */

	n = read(fd, &cnt, sizeof(int));
	if(n < sizeof(int)) {
		error = 1;
		cnt = 0;
	}

	oprev = &rom_ptr->first_obj;
	while(cnt > 0) {
		cnt--;
		op = (otag *)malloc(sizeof(otag));
		if(op) {
			obj = (object *)malloc(sizeof(object));
			if(obj) {
				if(read_obj(fd, obj) < 0)
					error = 1;
				obj->parent_rom = rom_ptr;
				op->obj = obj;
				op->next_tag = 0;
				*oprev = op;
				oprev = &op->next_tag;
			}
			else
				merror("read_rom", FATAL);
		}
		else
			merror("read_rom", FATAL);
	}

	/* Read the descriptions */

	n = read(fd, &cnt, sizeof(int));
	if(n < sizeof(int)) {
		error = 1;
		cnt = 0;
	}

	if(cnt) {
		sh = (char *)malloc(cnt);
		if(sh) {
			n = read(fd, sh, cnt);
			if(n < cnt)
				error = 1;
			rom_ptr->short_desc = sh;
		}
		else
			merror("read_rom", FATAL);
	}

	n = read(fd, &cnt, sizeof(int));
	if(n < sizeof(int)) {
		error = 1;
		cnt = 0;
	}

	if(cnt) {
		lo = (char *)malloc(cnt);
		if(lo) {
			n = read(fd, lo, cnt);
			if(n < cnt)
				error = 1;
			rom_ptr->long_desc = lo;
		}
		else
			merror("read_rom", FATAL);
	}

	n = read(fd, &cnt, sizeof(int));
	if(n < sizeof(int)) {
		error = 1;
		cnt = 0;
	}

	if(cnt) {
		ob = (char *)malloc(cnt);
		if(ob) {
			n = read(fd, ob, cnt);
			if(n < cnt)
				error = 1;
			rom_ptr->obj_desc = ob;
		}
		else
			merror("read_rom", FATAL);
	}

	if(error)
		return(-1);
	else
		return(0);
}

/**********************************************************************/
/*				free_obj			      */
/**********************************************************************/

/* This function is called to release the object pointed to by the first */
/* parameter from memory.  All objects contained within it are also      */
/* released from memory.  */

void free_obj(obj_ptr)
object	*obj_ptr;
{
	otag	*op, *temp;

	op = obj_ptr->first_obj;
	while(op) {
		temp = op->next_tag;
		free_obj(op->obj);
		free(op);
		op = temp;
	}
	free(obj_ptr);
}

/**********************************************************************/
/*				free_crt			      */
/**********************************************************************/

/* This function releases the creature pointed to by the first parameter */
/* from memory.  All items that creature has readied or carries will     */
/* also be released. */

void free_crt(crt_ptr)
creature	*crt_ptr;
{
	otag	*op, *temp;

	op = crt_ptr->first_obj;
	while(op) {
		temp = op->next_tag;
		free_obj(op->obj);
		free(op);
		op = temp;
	}

	free(crt_ptr);
}

/**********************************************************************/
/*				free_rom			      */
/**********************************************************************/

/* This function releases the a room and its contents from memory.  The */
/* function is passed the room's pointer as its parameter.  The exits,  */
/* descriptions, objects and monsters are all released from memory.     */

void free_rom(rom_ptr)
room	*rom_ptr;
{
	xtag 	*xp, *xtemp;
	otag	*op, *otemp;
	ctag	*cp, *ctemp;

	free(rom_ptr->short_desc);
	free(rom_ptr->long_desc);
	free(rom_ptr->obj_desc);

	xp = rom_ptr->first_ext;
	while(xp) {
		xtemp = xp->next_tag;
		free(xp->ext);
		free(xp);
		xp = xtemp;
	}

	op = rom_ptr->first_obj;
	while(op) {
		otemp = op->next_tag;
		free_obj(op->obj);
		free(op);
		op = otemp;
	}

	cp = rom_ptr->first_mon;
	while(cp) {
		ctemp = cp->next_tag;
		free_crt(cp->crt);
		free(cp);
		cp = ctemp;
	}

	free(rom_ptr);

}

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

        sprintf(file, "%s/r%05d", ROOMPATH, num);
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

void merror(str, errtype)
char    *str;
int     errtype;
{
        printf("Error: %s\n", str);
        if(errtype == FATAL)
                exit(-1);
}

void zero(ptr, size)
void    *ptr;
int     size;
{
        char    *chptr;
        int     i;
        
        chptr = (char *)ptr;
        for(i=0; i<size; i++)
                chptr[i] = 0;
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

        sprintf(file, "%s/m%02d", MONPATH, num/MFILESIZE);
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

        sprintf(file, "%s/o%02d", OBJPATH, num/OFILESIZE);
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

int save_rom_to_file(num, rom_ptr)
int     num;
room    *rom_ptr;
{
        int fd;
        long n; 
        char file[80];

        sprintf(file, "%s/r%05d", ROOMPATH, num);
        unlink(file);
        fd = open(file, O_RDWR | O_CREAT | O_BINARY, ACC);
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

void posit(x, y)
int x,y;
{
        printf("%c[%d;%df", 27, x, y);
}

void clearscreen()
{
        printf("%c[2J",27);
}

void spin(x)
{
	switch(x) {
    
				case 1:
                                        posit(8,10); printf("| \n");
                                        break;

                                case 2:
                                        posit(8,10); printf("/ \n");
                                        break;

				
				case 3:
					posit(8,10); printf("\\ \n");
					break;

				default:
					posit(8,10); printf("- \n");
					break;
	}
}

void message(str)
char	str[80];
{
static int count=0;
       int len, loc;


	if(strlen(str)<79) {
		len=79-strlen(str);
		while(len) {
			strcat(str, " ");
			len -= 1;
		}
	}
	count++;
	if(count>10)
		count=-1;
	loc = count+13;
	posit(loc, 10);
	printf("%s", str);
}

int load_ply_from_file(str, ply_ptr)
char            *str;
creature        **ply_ptr;
{
        int fd;
        long n;
        char file[80];
#ifdef COMPRESS
        char *a_buf, *b_buf;
        int size;
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
        if(!a_buf) merror(FATAL, "Memory allocation");
        size = read(fd, a_buf, 30000);
        if(size >= 30000) merror(FATAL, "Player too large");
        if(size < 1) {
                close(fd);
                return(-1);
        }
        b_buf = (char *)malloc(80000);
        if(!b_buf) merror(FATAL, "Memory allocation");
        n = uncompress(a_buf, b_buf, size);
        if(n > 80000) merror(FATAL, "Player too large");
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


/************************************************************************/
/*				write_crt_to_mem			*/
/************************************************************************/

/* Save a creature to memory.  This function 				*/
/* also saves all the items the creature is holding.  If perm_only != 0 */
/* then only those items which the creature is carrying that are	*/
/* permanent will be saved.						*/

int write_crt_to_mem(buf, crt_ptr, perm_only)
char		*buf;
creature 	*crt_ptr;
char 		perm_only;
{
	int 	n, cnt, cnt2=0, error=0;
	char	*bufstart;
	otag	*op;

	bufstart = buf;

	memcpy(buf, crt_ptr, sizeof(creature));
	buf += sizeof(creature);

	cnt = count_inv(crt_ptr, perm_only);
	memcpy(buf, &cnt, sizeof(int));
	buf += sizeof(int);

	if(cnt > 0) {
		op = crt_ptr->first_obj;
		while(op) {
			if(!perm_only || (perm_only && 
			   (F_ISSET(op->obj, OPERMT) || 
			   F_ISSET(op->obj, OPERM2)))) {
				if((n = write_obj_to_mem(buf, op->obj,
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

/************************************************************************/
/*				read_crt_from_mem			*/
/************************************************************************/

/* Loads a creature from memory & returns bytes read.  The creature is	*/
/* loaded at the mem location specified by the second parameter.  In	*/
/* addition, all the creature's objects have memory allocated for them	*/
/* and are loaded as well.  Returns -1 on fail.				*/

int read_crt_from_mem(buf, crt_ptr)
char		*buf;
creature 	*crt_ptr;
{
	int 		n, cnt, error=0;
	char		*bufstart;
	otag		*op;
	otag		**prev;
	object 		*obj;

	bufstart = buf;

	memcpy(crt_ptr, buf, sizeof(creature));
	buf += sizeof(creature);

	crt_ptr->first_obj = 0;
	crt_ptr->first_fol = 0;
	crt_ptr->first_enm = 0;
	crt_ptr->parent_rom = 0;
	crt_ptr->following = 0;
	for(n=0; n<20; n++)
		crt_ptr->ready[n] = 0;
	if(crt_ptr->mpcur > crt_ptr->mpmax)
		crt_ptr->mpcur = crt_ptr->mpmax;
	if(crt_ptr->hpcur > crt_ptr->hpmax)
		crt_ptr->hpcur = crt_ptr->hpmax;

	memcpy(&cnt, buf, sizeof(int));
	buf += sizeof(int);

	prev = &crt_ptr->first_obj;
	while(cnt > 0) {
		cnt--;
		op = (otag *)malloc(sizeof(otag));
		if(op) {
			obj = (object *)malloc(sizeof(object));
			if(obj) {
				if((n = read_obj_from_mem(buf, obj)) < 0)
					error = 1;
				else
					buf += n;
				obj->parent_crt = crt_ptr;
				op->obj = obj;
				op->next_tag = 0;
				*prev = op;
				prev = &op->next_tag;
			}
			else
				merror("read_crt", FATAL);
		}
		else
			merror("read_crt", FATAL);
	}

	if(error)
		return(-1);
	else
		return(buf - bufstart);
}

/************************************************************************/
/*				write_obj_to_mem			*/
/************************************************************************/

/* Save an object to a block of memory.					*/
/* This function recursively saves the items that are contained inside	*/
/* the object as well.  If perm_only != 0 then only permanent objects	*/
/* within the object are saved with it.  This function returns the	*/
/* number of bytes that were written.					*/

int write_obj_to_mem(buf, obj_ptr, perm_only)
char	*buf;
object 	*obj_ptr;
char 	perm_only;
{
	int 	n, cnt, cnt2=0, error=0;
	char	*bufstart;
	otag	*op;

	bufstart = buf;

	memcpy(buf, obj_ptr, sizeof(object));
	buf += sizeof(object);
	
	cnt = count_obj(obj_ptr, perm_only);
	memcpy(buf, &cnt, sizeof(int));
	buf += sizeof(int);

	if(cnt > 0) {
		op = obj_ptr->first_obj;
		while(op) {
			if(!perm_only || (perm_only && 
			   (F_ISSET(op->obj, OPERMT) || 
			   F_ISSET(op->obj, OPERM2)))) {
				if((n = write_obj_to_mem(buf, 
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

/************************************************************************/
/*				read_obj_from_mem			*/
/************************************************************************/

/* Loads the object from memory, returns the number of bytes read,	*/
/* and also loads every object which it might contain.  Returns -1 if	*/
/* there was an error.							*/

int read_obj_from_mem(buf, obj_ptr)
char	*buf;
object 	*obj_ptr;
{
	int 		n, cnt, error=0;
	char		*bufstart;
	otag		*op;
	otag		**prev;
	object 		*obj;

	bufstart = buf;

	memcpy(obj_ptr, buf, sizeof(object));
	buf += sizeof(object);

	obj_ptr->first_obj = 0;
	obj_ptr->parent_obj = 0;
	obj_ptr->parent_rom = 0;
	obj_ptr->parent_crt = 0;
	if(obj_ptr->shotscur > obj_ptr->shotsmax)
		obj_ptr->shotscur = obj_ptr->shotsmax;

	memcpy(&cnt, buf, sizeof(int));
	buf += sizeof(int);

	prev = &obj_ptr->first_obj;
	while(cnt > 0) {
		cnt--;
		op = (otag *)malloc(sizeof(otag));
		if(op) {
			obj = (object *)malloc(sizeof(object));
			if(obj) {
				if((n = read_obj_from_mem(buf, obj)) < 0)
					error = 1;
				else
					buf += n;
				obj->parent_obj = obj_ptr;
				op->obj = obj;
				op->next_tag = 0;
				*prev = op;
				prev = &op->next_tag;
			}
			else
				merror("read_obj", FATAL);
		}
		else
			merror("read_obj", FATAL);
	}

	if(error)
		return(-1);
	else
		return(buf - bufstart);
}

int save_obj_to_file(num, obj_ptr)
int     num;
object  *obj_ptr;
{
        int fd;
        long n;
        char file[80];

        sprintf(file, "%s/o%02d", OBJPATH, num/OFILESIZE);
        fd = open(file, O_RDWR | O_BINARY, 0);
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

int save_crt_to_file(num, crt_ptr)
int             num;
creature        *crt_ptr;
{
        int fd;
        long n;
        char file[80];

        sprintf(file, "%s/m%02d", MONPATH, num/MFILESIZE);
        fd = open(file, O_RDWR | O_BINARY, 0);
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

int save_ply_to_file(str, ply_ptr)
char            *str;
creature        *ply_ptr;
{
        int fd;
        long n;
        char file[80];
#ifdef COMPRESS
        char *a_buf, *b_buf;
        int size;
#endif

        sprintf(file, "%s/%s", PLAYERPATH, str);
        unlink(file);
        fd = open(file, O_RDWR | O_CREAT | O_BINARY, ACC);
        if(fd < 0)
                return(-1);

#ifdef COMPRESS
        a_buf = (char *)malloc(80000);
        if(!a_buf) merror(FATAL, "Memory allocation");
        n = write_crt_to_mem(a_buf, ply_ptr, 0);
        if(n > 80000) merror(FATAL, "Player too large");
        b_buf = (char *)malloc(n);
        if(!b_buf) merror(FATAL, "Memory allocation");
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

