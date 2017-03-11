/*
 * FILES2.C:
 *
 *	Additional file routines, including memory management.
 *
 *	Copyright (C) 1991, 1992, 1993 Brooke Paul
 *	Copyright (c) 1998 John P. Freeman
 * $Id: files2.c,v 1.5 2001/03/09 05:10:09 develop Exp $
 *
 * $Log: files2.c,v $
 * Revision 1.5  2001/03/09 05:10:09  develop
 * potential fix for memory overflow when saving players
 *
 * Revision 1.4  2001/03/08 16:05:42  develop
 * *** empty log message ***
 *
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/morutil.h"

#include "compress.h"
#include "mordb.h"

extern int errno;


rsparse	Rom[RMAX];	/* Pointer array declared */
csparse	Crt[CMAX];
osparse	Obj[OMAX];

static qtag	*Romhead=0;	/* Queue header and tail pointers */
static qtag	*Romtail=0;
static qtag	*Crthead=0;
static qtag	*Crttail=0;
static qtag	*Objhead=0;
static qtag	*Objtail=0;

int	Rsize=0;		/* Queue sizes */
int	Csize=0;
int	Osize=0;

/**********************************************************************/
/*				load_rom			      */
/**********************************************************************/

/* This function accepts a room number as its first argument and then   */
/* returns a dblpointer to that room's data in the second parameter.    */
/* If the room has already been loaded, the pointer is simply returned. */
/* Otherwise, the room is loaded into memory.  If a maximal number of   */
/* rooms is already in the memory, then the least recently used room    */
/* is stored back to disk, and the memory is freed.		        */

int load_rom(int index, room **rom_ptr )
{
	int	fd;
	qtag	*qt;
	char	file[256], filebak[256];

	if(index >= RMAX || index < 0)
		return(-1);

	/* Check if room is already loaded, and if so return pointer */

	if(Rom[index].rom) {
		front_queue(&Rom[index].q_rom, &Romhead, &Romtail, &Rsize);
		*rom_ptr = Rom[index].rom;
	}

	/* Otherwise load the room, store rooms if queue size becomes */
	/* too big, and return a pointer to the newly loaded room     */

	else {
		/* call low level load room code */
		if ( load_rom_from_file(index, rom_ptr) != 0 )
			return(-1);

		qt = (qtag *)malloc(sizeof(qtag));
		if(!qt)
			merror("load_rom", FATAL);
		qt->index = index;
		Rom[index].rom = *rom_ptr;
		Rom[index].q_rom = qt;
		put_queue(&qt, &Romhead, &Romtail, &Rsize);

		while(Rsize > RQMAX) {

			pull_queue(&qt, &Romhead, &Romtail, &Rsize);
			if(Rom[qt->index].rom->first_ply) {
				put_queue(&qt, &Romhead, &Romtail, &Rsize);
				continue;
			}

			get_room_filename( qt->index, file );

			sprintf(filebak, "%s~", file);
			rename(file, filebak);
			fd = open(file, O_RDWR | O_CREAT | O_BINARY, ACC);
			if(fd < 0 )
				return(-1);
			if(!Rom[qt->index].rom)
				merror("ERROR - load_rom", NONFATAL);
			if(write_rom(fd, Rom[qt->index].rom, PERMONLY) < 0) {
				close(fd);
				unlink(file);
				rename(filebak, file);
				merror("ERROR - write_rom", NONFATAL);
				return(-1);
			}
			close(fd);
			unlink(filebak);
			free_rom(Rom[qt->index].rom);
			Rom[qt->index].rom = 0;
			free(qt);
		}
	}

	return(0);

}

/**********************************************************************/
/*				is_rom_loaded			      */
/**********************************************************************/
int is_rom_loaded( int num )
{
	return(Rom[num].rom != 0);
}

/**********************************************************************/
/* get_room_filename()							*/
/**********************************************************************/
void get_room_filename( int num, char *filename )
{

	char	rmfrstprt[10];

	if(get_hash_rooms()) {
		sprintf(rmfrstprt,"10/r");
		if(num<10000) sprintf(rmfrstprt,"09/r");
		if(num<9000) sprintf(rmfrstprt,"08/r");
		if(num<8000) sprintf(rmfrstprt,"07/r");
		if(num<7000) sprintf(rmfrstprt,"06/r");
		if(num<6000) sprintf(rmfrstprt,"05/r");
		if(num<5000) sprintf(rmfrstprt,"04/r");
		if(num<4000) sprintf(rmfrstprt,"03/r");
		if(num<3000) sprintf(rmfrstprt,"02/r");
		if(num<2000) sprintf(rmfrstprt,"01/r");
		if(num<1000) sprintf(rmfrstprt,"00/r");
		sprintf(filename, "%s/r%s%05d", get_room_path(), rmfrstprt, num);
	}
	else
		sprintf(filename, "%s/r%05d", get_room_path(), num);

	return;
}


/**********************************************************************/
/*				reload_rom			      */
/**********************************************************************/

/* This function reloads a room from disk, if it's already loaded.  This  */
/* allows you to make changes to a room, and then reload it, even if it's */
/* already in the memory room queue.					  */

int reload_rom(int num )
{
	room	*rom_ptr;
	ctag	*cp;
	otag	*op;
	char	file[256];
	int		fd;

	if(!Rom[num].rom)
		return(0);

	get_room_filename( num, file );

	fd = open(file, O_RDWR | O_BINARY );
	if(fd < 0)
		return(-1);
	rom_ptr = (room *)malloc(sizeof(room));
	if(!rom_ptr)
		merror("reload_rom", FATAL);
	if(read_rom(fd, rom_ptr) < 0) {
		close(fd);
		return(-1);
	}
	close(fd);

	rom_ptr->first_ply = Rom[num].rom->first_ply;
	Rom[num].rom->first_ply = 0;

	/* have to do this in dm_reload_rom() now */
	/* add_permcrt_rom(rom_ptr); */
	if(!rom_ptr->first_mon) {
		rom_ptr->first_mon = Rom[num].rom->first_mon;
		Rom[num].rom->first_mon = 0;
	}

	if(!rom_ptr->first_obj) {
		rom_ptr->first_obj = Rom[num].rom->first_obj;
		Rom[num].rom->first_obj = 0;
	}

	free_rom(Rom[num].rom);
	Rom[num].rom = rom_ptr;

	cp = rom_ptr->first_ply;
	while(cp) {
		cp->crt->parent_rom = rom_ptr;
		cp = cp->next_tag;
	}

	cp = rom_ptr->first_mon;
	while(cp) {
		cp->crt->parent_rom = rom_ptr;
		cp = cp->next_tag;
	}

	op = rom_ptr->first_obj;
	while(op) {
		op->obj->parent_rom = rom_ptr;
		op = op->next_tag;
	}

	return(0);

}

/**********************************************************************/
/*				resave_rom			      */
/**********************************************************************/

/* This function saves an already-loaded room back to memory without */
/* altering its position on the queue.				     */

int resave_rom( int num )
{
	char	file[256], filebak[256];
	int		fd;

	if(!Rom[num].rom)
		return(0);

	get_room_filename( num, file );

	sprintf(filebak, "%s~", file);
	rename(file, filebak);
	fd = open(file, O_RDWR | O_CREAT | O_BINARY, ACC);
	if(fd < 0)
		return(-1);
	if(write_rom(fd, Rom[num].rom, PERMONLY) < 0) {
		close(fd);
		unlink(file);
		rename(filebak, file);
		return(-1);
	}
	close(fd);
	unlink(filebak);

	return(0);

}

/**********************************************************************/
/*				resave_all_rom			      */
/**********************************************************************/

/* This function saves all memory-resident rooms back to disk.  If the */
/* permonly parameter is non-zero, then only permanent items in those  */
/* rooms are saved back.					       */

void resave_all_rom( char permonly )
{
	qtag 	*qt;
	char	file[256];
	int		fd;

	qt = Romhead;
	while(qt) {
		if(!Rom[qt->index].rom) {
			qt = qt->next;
			continue;
		}

		get_room_filename( qt->index, file );

		fd = open(file, O_RDWR | O_CREAT | O_BINARY, ACC);
		if(fd < 0)
			return;
		if(write_rom(fd, Rom[qt->index].rom, permonly) < 0) {
			close(fd);
			return;
		}
		close(fd);
		qt = qt->next;
	}
}


/**********************************************************************/
/*				flush_rom			      */
/**********************************************************************/

/* This function flushes out the room queue and clears the room sparse */
/* pointer array, without saving anything to file.  It also clears all */
/* memory used by loaded rooms.  Call this function before leaving the */
/* program.							       */

void flush_rom()
{
	qtag *qt;

	while(1) {
		pull_queue(&qt, &Romhead, &Romtail, &Rsize);
		if(!qt) break;
		free_rom(Rom[qt->index].rom);
		Rom[qt->index].rom = 0;
		free(qt);
	}
}

/**********************************************************************/
/*				flush_crt			      */
/**********************************************************************/

/* This function flushes out the monster queue and clears the monster */
/* sparse pointer array without saving anything to file.  It also     */
/* clears all memory used by loaded creatures.  Call this function    */
/* before leaving the program.					      */

void flush_crt()
{
	qtag *qt;

	while(1) {
		pull_queue(&qt, &Crthead, &Crttail, &Csize);
		if(!qt) 
			break;
		free_crt(Crt[qt->index].crt);
		Crt[qt->index].crt = 0;
		free(qt);
	}
}




/**********************************************************************/
/*				flush_obj			      */
/**********************************************************************/

/* This function flushes out the object queue and clears the object */
/* sparse pointer array without saving anything to file.  It also   */
/* clears all memory used by loaded objects.  Call this function    */
/* leaving the program.						    */

void flush_obj()
{
	qtag *qt;

	while(1) {
		pull_queue(&qt, &Objhead, &Objtail, &Osize);
		if(!qt) break;
		free_obj(Obj[qt->index].obj);
		Obj[qt->index].obj = 0;
		free(qt);
	}
}




/**********************************************************************/
/*				replace_crt_in_queue			      */
/**********************************************************************/
void replace_crt_in_queue(int index, creature *crt_ptr)
{

	/* is it in the queue? */
	if(Crt[index].crt) {
		*Crt[index].crt = *crt_ptr;
	}

	return;
}

/**********************************************************************/
/*				replace_obj_in_queue			      */
/**********************************************************************/
void replace_obj_in_queue(int index, object *obj_ptr)
{

	/* is it in the queue? */
	if(Obj[index].obj) 
	{
		/* then just copy it */
		 *Obj[index].obj = *obj_ptr;
	}

	return;
}

/**********************************************************************/
/*				load_crt			      */
/**********************************************************************/

/* This function returns a pointer to the monster given by the index in */
/* the first parameter.  The pointer is returned in the second.  If the */
/* monster is already in memory, then a pointer is merely returned.     */
/* Otherwise, the monster is loaded into memory and a pointer is re-    */
/* turned.  If there are too many monsters in memory, then the least    */
/* recently used one is freed from memory.				*/

int load_crt(int index, creature **mon_ptr )
{
	qtag	*qt;

	if(index >= CMAX || index < 0)
		return(-1);

	/* Check if monster is already loaded, and if so return pointer */
	if(Crt[index].crt) 
	{
		/* since this is the most recently loaded monster */
		/* move it to the front of the queue */
		front_queue(&Crt[index].q_crt, &Crthead, &Crttail, &Csize);
		*mon_ptr = (creature *)malloc(sizeof(creature));
		**mon_ptr = *Crt[index].crt;
	}

	/* Otherwise load the monster, erase monsters if queue size          */
	/* becomes too big, and return a pointer to the newly loaded monster */

	else {
		if ( load_crt_from_file(index, mon_ptr ) != 0 )
		{
			return(-1);
		}

		qt = (qtag *)malloc(sizeof(qtag));
		if(!qt)
			merror("load_crt", FATAL);
		qt->index = index;
		Crt[index].crt = (creature *)malloc(sizeof(creature));
		*Crt[index].crt = **mon_ptr;
		Crt[index].q_crt = qt;
		put_queue(&qt, &Crthead, &Crttail, &Csize);

		while(Csize > CQMAX) {
			pull_queue(&qt, &Crthead, &Crttail, &Csize);
			free_crt(Crt[qt->index].crt);
			Crt[qt->index].crt = 0;
			free(qt);
		}
	}

	sprintf((*mon_ptr)->password, "%d", index);
	(*mon_ptr)->lasttime[LT_HEALS].ltime = time(0);
	(*mon_ptr)->lasttime[LT_HEALS].interval = 60L;
	(*mon_ptr)->first_enm= 0;

#ifdef MOBS_ALWAYS_ACTIVE
	add_active( *mon_ptr );
#endif

	return(0);

}

/**********************************************************************/
/*				load_obj			      */
/**********************************************************************/

/* This function loads the object specified by the first parameter, and */
/* returns a pointer to it in the second parameter.  If the object has  */
/* already been loaded before, then a pointer is merely returned.       */
/* Otherwise, the object is loaded into memory and the pointer is       */
/* returned.  If there are too many objects in memory, then the least   */
/* recently used objects are freed from memory.				*/

int load_obj( int index, object	**obj_ptr )
{
	qtag	*qt;

	if(index >= OMAX || index < 0)
		return(-1);

	/* Check if object is already loaded, and if so return pointer */

	if(Obj[index].obj) {
		front_queue(&Obj[index].q_obj, &Objhead, &Objtail, &Osize);
		*obj_ptr = (object *)malloc(sizeof(object));
		**obj_ptr = *Obj[index].obj;
	}

	/* Otherwise load the object, erase objects if queue size           */
	/* becomes too big, and return a pointer to the newly loaded object */

	else {
		if ( load_obj_from_file(index, obj_ptr ) != 0 )
			return(-1);

		/* now put it in the queue */
		qt = (qtag *)malloc(sizeof(qtag));
		if(!qt)
			merror("load_obj", FATAL);
		qt->index = index;
		Obj[index].obj = (object *)malloc(sizeof(object));
		*Obj[index].obj = **obj_ptr;
		Obj[index].q_obj = qt;
		put_queue(&qt, &Objhead, &Objtail, &Osize);
		while(Osize > OQMAX) {
			pull_queue(&qt, &Objhead, &Objtail, &Osize);
			free_obj(Obj[qt->index].obj);
			Obj[qt->index].obj = 0;
			free(qt);
		}
	}

	return(0);

}

/***********************************************************************/
/*				write_ply			       */
/***********************************************************************/

/* This function saves the player specified by the string in the first */
/* parameter, and uses the player in the second parameter.	       */
/* NOTE: This should never be called except from save_ply! JPF */
/* if a player is wearing armor or other items, he will lose it here */

int write_ply(char *str, creature *ply_ptr )
{
	char	file[256], filebak[256];
	int	fd, n;
#ifdef COMPRESS
	char	*a_buf, *b_buf;
	int	size;
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
	n = write_crt_to_mem(a_buf, ply_ptr, 0);
	if(n > 100000)
		merror(ply_ptr->name, FATAL);
	if(n < 0) {
		close(fd);
		free(a_buf);
		rename(filebak,file);
		return(-1);
	}	

	b_buf = (char *)malloc(n);
	if(!b_buf) 
		merror("Compress Memory allocation", FATAL);

	size = compress(a_buf, b_buf, n);
	n = write(fd, b_buf, size);
	free(a_buf);
	free(b_buf);
#else
	n = write_crt(fd, ply_ptr, 0);
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

/***********************************************************************/
/*				load_ply			       */
/***********************************************************************/

/* This function loads the player specified by the string in the first */
/* parameter, and returns the player in the second parameter.	       */

int load_ply(char *str, creature **ply_ptr )
{
	/* no queue to worry about here */
	return(load_ply_from_file(str, ply_ptr));


}

/**********************************************************************/
/*				put_queue			      */
/**********************************************************************/

/* put_queue places the queue tag pointed to by the first paramater onto */
/* a queue whose head and tail tag pointers are the second and third     */
/* parameters.  If parameters 2 & 3 are 0, then a new queue is created.  */
/* The fourth parameter points to a queue size counter which is 	 */
/* incremented.							         */

void put_queue(qtag **qt, qtag **headptr, qtag **tailptr, int *sizeptr )
{
	*sizeptr = *sizeptr + 1;

	if(!*headptr) {
		*headptr = *qt;
		*tailptr = *qt;
		(*qt)->next = 0;
		(*qt)->prev = 0;
	}

	else {
		(*headptr)->prev = *qt;
		(*qt)->next = *headptr;
		(*qt)->prev = 0;
		*headptr = *qt;
	}
}

/**********************************************************************/
/*				pull_queue			      */
/**********************************************************************/

/* pull_queue removes the last queue tag on the queue specified by the */
/* second and third parameters and returns that tag in the first       */
/* parameter.  The fourth parameter points to a queue size counter     */
/* which is decremented.					       */

void pull_queue( qtag **qt, qtag **headptr, qtag **tailptr, int *sizeptr )
{
	if(!*tailptr)
		*qt = 0;
	else {
		*sizeptr = *sizeptr - 1;
		*qt = *tailptr;
		if((*qt)->prev) {
			(*qt)->prev->next = 0;
			*tailptr = (*qt)->prev;
		}
		else {
			*headptr = 0;
			*tailptr = 0;
		}
	}
}

/**********************************************************************/
/*				front_queue			      */
/**********************************************************************/

/* front_queue removes the queue tag pointed to by the first parameter */
/* from the queue (specified by the second and third parameters) and   */
/* places it back at the head of the queue.  The fourth parameter is a */
/* pointer to a queue size counter, and it remains unchanged.          */

void front_queue(qtag **qt, qtag **headptr, qtag **tailptr, int	*sizeptr )
{
	if((*qt)->prev) {
		((*qt)->prev)->next = (*qt)->next;
		if(*qt == *tailptr)
			*tailptr = (*qt)->prev;
	}
	if((*qt)->next) {
		((*qt)->next)->prev = (*qt)->prev;
		if(*qt == *headptr)
			*headptr = (*qt)->next;
	}
	if(!(*qt)->prev && !(*qt)->next) {
		*headptr = 0;
		*tailptr = 0;
	}
	(*qt)->next = 0;
	(*qt)->prev = 0;
	*sizeptr = *sizeptr - 1;

	put_queue(qt, headptr, tailptr, sizeptr);
}




void free_all_queues()
{
	flush_rom();
	flush_crt();
	flush_obj();
}


/*
 *  Memory added by Charles Marchant for 
 *  Mordor 3.x
 *  Shows memory status of mordor
 */
int get_memory_usage( MEM_USAGE *pmu )
{
	char buf[80];
	int  i = 0; 
	ctag        *cp;
	otag        *op;
	ttag        *tlk;

	zero( pmu, sizeof(MEM_USAGE) );

	for(i=0;i<RMAX;i++)
	{
		if(Rom[i].rom)
		{
			pmu->rooms.count++;
			pmu->rooms.mem_used += sizeof(room);

		    for(cp = Rom[i].rom->first_mon;cp;cp = cp->next_tag)
			{
				if(cp->crt)
				{
					pmu->creatures.count++;
					pmu->creatures.mem_used += sizeof(creature);

					/* add object counting on crts */
					/* and object wear on crts */
		    
					if(cp->crt->first_tlk)
					{
						tlk = cp->crt->first_tlk;
						if(F_ISSET(cp->crt,MTALKS))
						{		    
							for(;tlk;tlk = tlk->next_tag)
							{

								pmu->talks.count++;
								pmu->talks.mem_used += sizeof(ttag);

								if(tlk->key) 
								  pmu->talks.mem_used += strlen(tlk->key);
								if(tlk->response) 
								  pmu->talks.mem_used += strlen(tlk->response);
								if(tlk->action) 
								  pmu->talks.mem_used += strlen(tlk->action);
								if(tlk->target) 
								  pmu->talks.mem_used += strlen(tlk->target);
							 }
						}
						else if(F_ISSET(cp->crt,MROBOT))
						{
							for(;tlk;tlk = tlk->next_tag)
							{
								pmu->actions.count++;
								pmu->actions.mem_used += sizeof(ttag);
								if(tlk->response) 
								  pmu->actions.mem_used += strlen(tlk->response);
								if(tlk->action) 
								  pmu->actions.mem_used += strlen(tlk->action);
								if(tlk->target) 
								  pmu->actions.mem_used += strlen(tlk->target);
							}	      
						}
						else
						{
							sprintf(buf,"%s has a talk and should not.",
							cp->crt->name);
							elog(buf);
							for(;tlk;tlk = tlk->next_tag)
							{
								pmu->bad_talks.count++;
								pmu->bad_talks.mem_used += sizeof(ttag);

								if(tlk->key) 
								  pmu->bad_talks.mem_used += strlen(tlk->key);
								if(tlk->response) 
								  pmu->bad_talks.mem_used += strlen(tlk->response);
								if(tlk->action) 
								  pmu->bad_talks.mem_used += strlen(tlk->action);
								if(tlk->target) 
								  pmu->bad_talks.mem_used += strlen(tlk->target);
							}
						}
					}
				}		
			}
			for(op = Rom[i].rom->first_obj;op;op = op->next_tag)
			{
				if(op->obj)
				{
					pmu->objects.count++;
					pmu->objects.mem_used += sizeof(object);

					/* and contents counting */
				}
			}
		}
     }


  
   return 0;
}
