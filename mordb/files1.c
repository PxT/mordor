/*
 * FILES1.C:
 *
 *	File I/O Routines.
 *
 *	Copyright (C) 1991, 1992, 1993 Brooke Paul
 *
 * $Id: files1.c,v 1.4 2001/03/08 16:05:42 develop Exp $
 *
 * $Log: files1.c,v $
 * Revision 1.4  2001/03/08 16:05:42  develop
 * *** empty log message ***
 *
 */
#include <stdio.h>
#include <stdlib.h>


#include "../include/morutil.h"

#include "mordb.h"


/**********************************************************************/
/*				count_obj			      */
/**********************************************************************/

/* Return the total number of objects contained within an object.  */
/* If perm_only != 0, then only the permanent objects are counted. */

int count_obj( object *obj_ptr, char perm_only )
{
	otag	*op;
	int	total=0;

	op = obj_ptr->first_obj;
	while(op) {
		if(!perm_only || (perm_only && (F_ISSET(op->obj, OPERMT))))
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

int write_obj(int fd, object *obj_ptr, char perm_only )
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
			   (F_ISSET(op->obj, OPERMT)))) {
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

int count_inv( creature *crt_ptr, char perm_only )
{
	otag	*op;
	int	total=0;

	op = crt_ptr->first_obj;
	while(op) {
		if(!perm_only || (perm_only && (F_ISSET(op->obj, OPERMT)))) 
			total++;
		op = op->next_tag;
	}
	return(total);
}

int count_full_inv( creature *crt_ptr )
{
	otag	*op, *xop;
	int	total=0;

	op = crt_ptr->first_obj;
	while(op) {
		total++;
		if(F_ISSET(op->obj, OCONTN)) {
			xop = op->obj->first_obj;
			while(xop) {
				total++;
				xop = xop->next_tag;
			}
		}
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

int write_crt(int fd, creature *crt_ptr, char perm_only )
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
		while(op && cnt2<cnt) {
			if(!perm_only || (perm_only && 
			   (F_ISSET(op->obj, OPERMT)))) {
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

int count_mon( room *rom_ptr, char 	perm_only )
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

int count_ite(room *rom_ptr, char perm_only )
{
	otag 	*op;
	int	total=0;

	op = rom_ptr->first_obj;
	while(op) {
		if(!perm_only || (perm_only && (F_ISSET(op->obj, OPERMT))))
			total++;
		op = op->next_tag;
	}
	return(total);
}

/**********************************************************************/
/*				count_ext			      */
/**********************************************************************/

/* Returns the number of exits in the room */

int count_ext( room *rom_ptr )
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

int count_ply( room *rom_ptr )
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

int write_rom(int fd, room *rom_ptr, char perm_only )
{
	int 	n, cnt, error=0;
	xtag	*xp;
	ctag	*cp;
	otag	*op;
	char	pcontain;

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
        if(!perm_only || (perm_only && (F_ISSET(op->obj, OPERMT)))){
            if (F_ISSET(op->obj,OCONTN)
	            && (F_ISSET(op->obj, OPERMT)))
                pcontain =  ALLITEMS;
            else
                pcontain = perm_only;
                if(write_obj(fd, op->obj, pcontain) < 0)
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

/**********************************************************************/
/*				read_obj			      */
/**********************************************************************/

/* Loads the object from the open and positioned file described by fd */
/* and also loads every object which it might contain.  Returns -1 if */
/* there was an error.						      */

int read_obj( int fd, object *obj_ptr )
{
	int 		n, cnt, ret=0;
	otag		*op;
	otag		**prev;
	object 		*obj;

	if ( read_single_obj(fd, obj_ptr ) != 0 )
	{
		return(-1);
	}

	/* now look for things contained in it */
	n = read(fd, &cnt, sizeof(int));
	if(n < sizeof(int)) {
		/* no more objects to read is not an error */
		return(0);
	}

	prev = &obj_ptr->first_obj;
	while(cnt > 0) {
		cnt--;
		op = (otag *)malloc(sizeof(otag));
		if(op) {
			obj = (object *)malloc(sizeof(object));
			if(obj) {
				if(read_obj(fd, obj) < 0)
				{
					free(obj);
					ret = -1;
					break;
				}
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

	return(ret);
}


/**********************************************************************/
/*				read_single_obj										  */
/**********************************************************************/

/* Loads the object from the open and positioned file described by fd */
/* and but does not loads every object which it might contain.  Returns -1 if */
/* there was an error.						      */

int read_single_obj( int fd, object *obj_ptr )
{
	int 		n, ret=0;

	n = read(fd, obj_ptr, sizeof(object));
	if(n < sizeof(object))
		return(-1 );

	obj_ptr->first_obj = 0;
	obj_ptr->parent_obj = 0;
	obj_ptr->parent_rom = 0;
	obj_ptr->parent_crt = 0;
	if(obj_ptr->shotscur > obj_ptr->shotsmax)
		obj_ptr->shotscur = obj_ptr->shotsmax;


	return(ret);
}

/**********************************************************************/
/*				read_crt			      */
/**********************************************************************/

/* Loads a creature from an open and positioned file.  The creature is */
/* loaded at the mem location specified by the second parameter.  In   */
/* addition, all the creature's objects have memory allocated for them */
/* and are loaded as well.  Returns -1 on fail.			       */

int read_crt( int fd, creature *crt_ptr )
{
	int 		n, cnt, ret = 0;
	otag		*op;
	otag		**prev;
	object 		*obj;

	/* first clear the buffer */
	memset( crt_ptr, 0, sizeof(creature) );

	/* now read */
	n = read(fd, crt_ptr, sizeof(creature));
	if(n < sizeof(creature))
		return(-1);

	crt_ptr->first_obj = 0;
	crt_ptr->first_fol = 0;
	crt_ptr->first_enm = 0;
	crt_ptr->first_tlk = 0;
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
		return(0);
	}

	prev = &crt_ptr->first_obj;
	while(cnt > 0) {
		cnt--;
		op = (otag *)malloc(sizeof(otag));
		if(op) {
			obj = (object *)malloc(sizeof(object));
			if(obj) {
				if(read_obj(fd, obj) < 0)
				{
					free( obj );
					ret = -1;
					break;
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

	return(ret);
}


/**********************************************************************/
/*				read_single_crt										  */
/**********************************************************************/

/* Loads a creature from an open and positioned file.  The creature is */
/* loaded at the mem location specified by the second parameter.  It   */
/* does not look for additional item to load						   */
/* Returns -1 on fail.												   */

int read_single_crt( int fd, creature *crt_ptr )
{
	int 		n, ret = 0;

	n = read(fd, crt_ptr, sizeof(creature));
	if(n < sizeof(creature))
		return(-1);

	if ( crt_ptr->type != MONSTER )
	{
		return( -1 );
	}
		
	crt_ptr->first_obj = 0;
	crt_ptr->first_fol = 0;
	crt_ptr->first_enm = 0;
	crt_ptr->first_tlk = 0;
	crt_ptr->parent_rom = 0;
	crt_ptr->following = 0;
	for(n=0; n<20; n++)
		crt_ptr->ready[n] = 0;
	if(crt_ptr->mpcur > crt_ptr->mpmax)
		crt_ptr->mpcur = crt_ptr->mpmax;
	if(crt_ptr->hpcur > crt_ptr->hpmax)
		crt_ptr->hpcur = crt_ptr->hpmax;


	return(ret);
}

/**********************************************************************/
/*				read_rom			      */
/**********************************************************************/

/* Loads a room from an already open and positioned file into the memory */
/* pointed to by the second parameter.  Also loaded are all creatures,   */
/* exits, objects and descriptions for the room.  -1 is returned upon    */
/* failure.								 */

/* NOTE: This should be called with a file handle that is writable so	*/
/* the routine can try to write out a good room if it is fixable.	*/
/* This can automatically correct some invalid rooms.  - JPF		*/

int read_rom( int fd, room *rom_ptr )
{
	int		n, cnt;
	xtag		*xp;
	xtag		**xprev;
	exit_		*ext;
	ctag		*cp;
	ctag		**cprev;
	creature	*crt;
	otag		*op;
	otag		**oprev;
	object 		*obj;
	char		*sh, *lo, *ob, errstr[80];
	char *strfunc = "read_rom";

	n = read(fd, rom_ptr, sizeof(room));
	if(n < sizeof(room))
	{
		merror("Failed reading room struct.\n", FATAL );
		return(-1);
	}

	rom_ptr->short_desc = 0;
	rom_ptr->long_desc  = 0;
	rom_ptr->obj_desc   = 0;
	rom_ptr->first_ext = 0;
	rom_ptr->first_obj = 0;
	rom_ptr->first_mon = 0;
	rom_ptr->first_ply = 0;

	n = read(fd, &cnt, sizeof(int));
	if(n < sizeof(int)) 
	{
		merror("Failed reading room exit count.\n", FATAL );
		return(-1);
	}

	/* Load the exits */

	xprev = &rom_ptr->first_ext;
	while(cnt > 0) 
	{
		cnt--;
		xp = (xtag *)malloc(sizeof(xtag));
		if(xp) 
		{
			ext = (exit_ *)malloc(sizeof(exit_));
			if(ext) 
			{
				n = read(fd, ext, sizeof(exit_));
				if(n < sizeof(exit_))
				{
					sprintf( errstr, "Bad exit while loading room %d", 
						rom_ptr->rom_num );
					elog( errstr );
					free(ext);
					break;
				}
				else
				{
					xp->ext = ext;
					xp->next_tag = 0;
					*xprev = xp;
					xprev = &xp->next_tag;
				}
			}
			else 
			{
				sprintf( errstr, "%s in room %d in loading exits.", strfunc, 
					rom_ptr->rom_num);
				merror(errstr, FATAL);
			}
		}
		else 
		{
			sprintf( errstr, "%s in room %d in loading exits.", strfunc, 
				rom_ptr->rom_num);
			merror(errstr, FATAL);
		}
	}

	/* any error after this might be recoverable */

	/* Read the monsters */

	n = read(fd, &cnt, sizeof(int));
	if(n < sizeof(int)) 
	{
		/* we can assume there is no more data */
		/* write out a new file with all the correct fields */
		/* to fixup the room */
		if ( fixup_room(fd, rom_ptr ) )
		{
			return(0);
		}

		/* couldn't fix room for some reason */
		return(-1);
	}

	cprev = &rom_ptr->first_mon;
	while(cnt > 0) 
	{
		cnt--;
		cp = (ctag *)malloc(sizeof(ctag));
		if(cp) 
		{
			crt = (creature *)malloc(sizeof(creature));
			if(crt) 
			{
				if(read_crt(fd, crt) < 0)
				{
					sprintf( errstr, "Bad creature while loading room %d", 
						rom_ptr->rom_num );
					elog( errstr );
					free(crt);
				}
				else
				{
					crt->parent_rom = rom_ptr;
					cp->crt = crt;
					cp->next_tag = 0;
					*cprev = cp;
					cprev = &cp->next_tag;
#ifdef MOBS_ALWAYS_ACTIVE
					add_active( crt );
#endif
				}
			}
			else 
			{
				sprintf( errstr, "%s in room %d in loading monsters.", strfunc, 
					rom_ptr->rom_num);
				merror(errstr, FATAL);
			}
		}
		else 
		{
			sprintf( errstr, "%s in room %d in loading monsters.", strfunc, 
				rom_ptr->rom_num);
			merror(errstr, FATAL);
		}
	}

	/* Read the items */

	n = read(fd, &cnt, sizeof(int));
	if(n < sizeof(int)) 
	{
		/* we can assume there is no more data */
		/* write out a new file with all the correct fields */
		/* to fixup the room */
		if ( fixup_room(fd, rom_ptr ) )
		{
			return(0);
		}

		/* couldn't fix room for some reason */
		return(-1);
	}

	oprev = &rom_ptr->first_obj;
	while(cnt > 0) 
	{
		cnt--;
		op = (otag *)malloc(sizeof(otag));
		if(op) 
		{
			obj = (object *)malloc(sizeof(object));
			if(obj) 
			{
				if(read_obj(fd, obj) < 0)
				{
					sprintf( errstr, "Bad object while loading room %d", 
						rom_ptr->rom_num );
					elog( errstr );
					free(obj);
				}
				else
				{
					obj->parent_rom = rom_ptr;
					op->obj = obj;
					op->next_tag = 0;
					*oprev = op;
					oprev = &op->next_tag;
				}
			}
			else 
			{
				sprintf( errstr, "%s in room %d in loading items.", strfunc, 
					rom_ptr->rom_num);
				merror(errstr, FATAL);
			}
		}
		else 
		{
			sprintf( errstr, "%s in room %d in loading items.", strfunc, 
				rom_ptr->rom_num);
			merror(errstr, FATAL);
		}
	}

	/* Read the descriptions */

	n = read(fd, &cnt, sizeof(int));
	if(n < sizeof(int)) 
	{
		/* we can assume there is no more data */
		/* write out a new file with all the correct fields */
		/* to fixup the room */
		if ( fixup_room(fd, rom_ptr ) )
		{
			return(0);
		}

		/* couldn't fix room for some reason */
		return(-1);
	}

	if(cnt) {
		sh = (char *)malloc(cnt);
		if(sh) 
		{
			n = read(fd, sh, cnt);
			if(n < cnt)
			{
				sprintf( errstr, "Bad short desc while loading room %d", 
					rom_ptr->rom_num );
				elog( errstr );
				free(sh);
			}
			else
			{
				rom_ptr->short_desc = sh;
			}
		}
		else 
		{
			sprintf( errstr, "%s in room %d in loading descriptions.", strfunc, 
				rom_ptr->rom_num);
			merror(errstr, FATAL);
		}
	}

	n = read(fd, &cnt, sizeof(int));
	if(n < sizeof(int)) 
	{
		/* we can assume there is no more data */
		/* write out a new file with all the correct fields */
		/* to fixup the room */
		if ( fixup_room(fd, rom_ptr ) )
		{
			return(0);
		}

		/* couldn't fix room for some reason */
		return(-1);
	}

	if(cnt) 
	{
		lo = (char *)malloc(cnt);
		if(lo) 
		{
			n = read(fd, lo, cnt);
			if(n < cnt)
			{
				sprintf( errstr, "Bad long desc while loading room %d", 
					rom_ptr->rom_num );
				elog( errstr );
				free( lo );
			}
			else
			{
				rom_ptr->long_desc = lo;
			}
		}
		else 
		{
			sprintf( errstr, "%s in room %d in loading descriptions.", strfunc, 
				rom_ptr->rom_num);
			merror(errstr, FATAL);
		}
	}

	n = read(fd, &cnt, sizeof(int));
	if(n < sizeof(int)) 
	{
		/* we can assume there is no more data */
		/* write out a new file with all the correct fields */
		/* to fixup the room */
		if ( fixup_room(fd, rom_ptr ) )
		{
			return(0);
		}

		/* couldn't fix room for some reason */
		return(-1);
	}

	if(cnt) 
	{
		ob = (char *)malloc(cnt);
		if(ob) {
			n = read(fd, ob, cnt);
			if(n < cnt)
			{
				sprintf( errstr, "Bad obj_desc at bottom while loading room %d", 
					rom_ptr->rom_num );
				elog( errstr );
				free( ob );
			}
			else
			{
				rom_ptr->obj_desc = ob;
			}
		}
		else 
		{
			sprintf( errstr, "%s in room %d in loading at the bottom.", strfunc, 
				rom_ptr->rom_num);
			merror(errstr, FATAL);
		}
	}

	return(0);
}


/**********************************************************************/
/*	fixup_room													      */
/*		we assume we have enough of the room to make it playable but  */
/*		for some reason, the file ended prematurely.  To correct this */
/*		this we will rewind the file pointer and rewrite the room	  */
/*		after reporting it in the error log.						  */
/* This returns 1 (or TRUE) if successful or 0 is not.				  */
/* JPF																  */
/**********************************************************************/
int fixup_room(int fd, room *rom_ptr )
{
	/* assume the worst */
	int nReturn = 0;
	int	num;
	int	size;
	int	fd_bad;

	char	filename[256];
	char	filebad[256];
	char	str[128];
	char	*buffer;
	int		ndx;

	ASSERTLOG( rom_ptr );

	num = rom_ptr->rom_num;
	get_room_filename( num, filename );
	
	sprintf(str, "Attempting to fix corrupt room number %d", num );
	elog( str );

	/* find a name that doesn't exist */
	sprintf( filebad, "%s.bad", filename );

	ndx = 0;
	while ( file_exists( filebad ) )
	{
		sprintf( filebad, "%s.ba%d", filename, ndx );
		ndx++;
	}

	sprintf(str, "Copying file %s to %s", filename, filebad);
	elog( str );

	size = lseek(fd, 0, SEEK_END);
	buffer = malloc( size );
	if ( buffer )
	{
		/* rewind */
		if ( lseek( fd, 0, SEEK_SET ) == 0)
		{
			if ( size != read(fd, buffer, size ) )
			{
				elog("Unable to read room to make a backup of room" );
				free(buffer );
				return(0);
			}

			fd_bad = open(filebad, O_CREAT | O_RDWR | O_BINARY );
			if(fd_bad < 0)
			{
				elog("Unable to open backup of room" );
				free(buffer );
				return(0);
			}

			if ( size != write(fd_bad, buffer, size ) )
			{
				elog("Unable to write backup of room" );
				free(buffer );
				close( fd_bad );
				return(0);
			}

			/* done with backup */
			close( fd_bad );
		}
		else
		{
			elog("Unable to rewind file.");
			free(buffer);
			return(0);
		}

		free(buffer);
	}


	/* now write out a good copy to the original file handle */
	
	/* start back at the beginning */
	if ( lseek(fd, 0, SEEK_SET) == 0 )
	{
		if ( write_rom(fd, rom_ptr, 1 ) >= 0 )
		{
			/* it worked */
			nReturn = 1;
			sprintf(str, "Saved a good copy of file %s.", filename);
			elog( str );
		}
	}
	else
	{
		elog("Unable to rewind file.");
		return(0);
	}



	return(nReturn);
}



/**********************************************************************/
/*				free_obj			      */
/**********************************************************************/

/* This function is called to release the object pointed to by the first */
/* parameter from memory.  All objects contained within it are also      */
/* released from memory.  *ASSUMPTION*:  This function will only be      */
/* called from free_rom() or free_crt().  Otherwise there may be 	 */
/* unresolved links in memory which would cause a game crash.		 */

void free_obj( object *obj_ptr )
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
/* also be released.  *ASSUMPTION*:  This function will only be called   */
/* from free_rom().  If it is called from somewhere else, unresolved     */
/* links may remain and cause a game crash.  *EXCEPTION*: This function  */
/* can be called independently to free a player's information from       */
/* memory (but not a monster).						 */

void free_crt( creature	*crt_ptr )
{
	otag	*op, *tempo;
	etag	*ep, *tempe;
	ctag	*cp, *tempc;
	ttag	*tp, *tempt;
	int	i;
	for(i=0; i<20; i++)
		if(crt_ptr->ready[i]) {
			free_obj(crt_ptr->ready[i]);
			crt_ptr->ready[i] = 0;
		}

	op = crt_ptr->first_obj;
	while(op) {
		tempo = op->next_tag;
		free_obj(op->obj);
		free(op);
		op = tempo;
	}

	cp = crt_ptr->first_fol;
	while(cp) {
		tempc = cp->next_tag;
		free(cp);
		cp = tempc;
	}

	ep = crt_ptr->first_enm;
	while(ep) {
		tempe = ep->next_tag;
		free(ep);
		ep = tempe;
	}

	tp = crt_ptr->first_tlk;
	crt_ptr->first_tlk = 0;
	while(tp) {
		tempt = tp->next_tag;
		if(tp->key) 
			free(tp->key);
		if(tp->response) 
			free(tp->response);
		if(tp->action) 
			free(tp->action);
		if(tp->target) 
			free(tp->target);
		free(tp);
		tp = tempt;
	}

	/* make sure the creature is not active before we free it */
	if (crt_ptr->type == MONSTER )
		del_active( crt_ptr );

	free(crt_ptr);
}

/**********************************************************************/
/*				free_rom			      */
/**********************************************************************/

/* This function releases the a room and its contents from memory.  The */
/* function is passed the room's pointer as its parameter.  The exits,  */
/* descriptions, objects and monsters are all released from memory.     */

void free_rom( room *rom_ptr )
{
	xtag 	*xp, *xtemp;
	otag	*op, *otemp;
	ctag	*cp, *ctemp;

#ifdef DMALLOC
	dmalloc_verify (rom_ptr);
#endif /* DMALLOC */
	
	if(rom_ptr->short_desc)
		free(rom_ptr->short_desc);
	if(rom_ptr->long_desc)
		free(rom_ptr->long_desc);
	if(rom_ptr->obj_desc)
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

	cp = rom_ptr->first_ply;
	while(cp) {
		ctemp = cp->next_tag;
		free(cp);
		cp = ctemp;
	}

	free(rom_ptr);

}


/***********************************************************************
*       This function finds the creature number of the given creature
*       from the database
*/
        
int find_crt_num( creature *crt_ptr )
{
	int i;
    creature  crt_src;

	ASSERTLOG( crt_ptr );


    for (i=0;i<CMAX;i++) 
	{
		/* call load_ctr_struct_from_file() to avoid any memory */
		/* allocations, thus no frees.  It should be the fastest */
		/* way to loop through the creature database */
	    if(load_crt_struct_from_file(i, &crt_src) < 0)
		    continue;
        if(!strcmp(crt_ptr->name, crt_src.name) && crt_ptr->experience == crt_src.experience) {
			break;
		}
    }

    if(i >= CMAX)
		i = 0;

	return(i);

}

/***********************************************************************
*       This function finds the object number of the given object
*       from the database
*/

int find_obj_num(object *obj_ptr )
{
    int i;
    object  obj_src;

	ASSERTLOG( obj_ptr );

    for (i=0;i<OMAX;i++) 
	{
		/* call load_obj_struct_from_file() to avoid any memory */
		/* allocations.  This no frees.  It shoudl be the fastest */
		/* way to loop through the greature database */
        if(load_obj_struct_from_file(i, &obj_src) < 0)
	        continue;
        if(!strcmp(obj_ptr->name, obj_src.name) && obj_ptr->ndice == obj_src.ndice) 
            break;
	}
	
    if(i >= OMAX)
		i = 0;

	return(i);

}

