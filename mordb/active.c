/*
 * ACTIVE.C:
 *
 *	Routines to manage the monster active list.
 *
 *	Copyright (C) 1992, 1993 Brooke Paul
 *	Copyright (c) 1998 John P. Freeman
 *
 * $Id: active.c,v 1.2 2001/03/08 16:05:42 develop Exp $
 *
 * $Log: active.c,v $
 * Revision 1.2  2001/03/08 16:05:42  develop
 * *** empty log message ***
 *
 *
 */




#include <stdio.h>

#include "../include/morutil.h"

#include "mordb.h"

static ctag *first_active;


/**********************************************************************/
/*              add_active                */
/**********************************************************************/

/* This function adds a monster to the active-monster list.  A pointer */
/* to the monster is passed in the first parameter.            */

void add_active(creature *crt_ptr )
{
    ctag    *ct;

/*	del_active(crt_ptr); */
	ASSERTLOG( crt_ptr );

    if(!crt_ptr || crt_ptr->type != MONSTER)
		return;

    if(is_crt_active(crt_ptr))
		return;

    ct = 0;
    ct = (ctag *)malloc(sizeof(ctag));
    if(!ct)
		merror("add_active", FATAL);

    ct->crt = crt_ptr;
    ct->next_tag = 0;

    if(!first_active)
        first_active = ct;
    else 
	{
        ct->next_tag = first_active;
        first_active = ct;
    }

}

/**********************************************************************/
/*							del_active								  */
/**********************************************************************/

/* This function removes a monster from the active-monster list.  The */
/* parameter contains a pointer to the monster which is to be removed */

void del_active( creature *crt_ptr )
{
    ctag    *cp, *prev;

    if(!(cp = first_active)) 
		return;

    if(cp->crt == crt_ptr) {
        first_active = cp->next_tag;
        free(cp);
        return;
    }

    prev = cp;
    cp = cp->next_tag;
    while(cp) 
	{
        if(cp->crt == crt_ptr) 
		{
            prev->next_tag = cp->next_tag;
            free(cp);
            return;
        }
        prev = cp;
        cp = cp->next_tag;
    }

	return;
}


/*********************************************************************/
/*			is_crt_active				     */
/*********************************************************************/
/*	This function returns 1 if the parameter passed is in the    */
/*	active list.						     */

int is_crt_active(creature *crt_ptr )
{
	ctag	*cp=0;
	int	  n=0;

	if(!(cp = first_active)) 
		return(0);

	while (cp) 
	{
		if(cp->crt == crt_ptr)
		{
			n = 1;	
			break;
		}
	cp = cp->next_tag;
	}
	return(n);
}		


/*********************************************************************/
/* get_first_active()									     */
/*	This function returns a pointer to the first active monster in	 */
/*  the active list												     */
/*********************************************************************/
ctag *get_first_active()
{
	return( first_active );
}


/*********************************************************************/
/* log_active()														 */
/*		This logs the active monster list for debugging	in a file 	 */
/*		called Active_list											 */
/*********************************************************************/
void log_active()
{
	ctag    *cp;
	char	rfile[256];
	char	buffer[256];
	
	sprintf(rfile,"%s/%s", get_log_path(), "Active_list");
	unlink(rfile);

	logn("Active_list", "Active monster list\n");
	logn("Active_list", "Monster    -    Room Number\n");
    cp = first_active;
    while(cp)
	{
		/* check before we sprintf() */
		/* the case where crt == NULL is one  */
		/* case when this function gets called */
		if ( cp->crt != NULL)
		{
			sprintf(buffer,"%s - %d.", cp->crt->name, cp->crt->parent_rom->rom_num);
		}
		else
		{
			strcpy(buffer,"cp->crt == NULL");
		}
		logn("Active_list", buffer);
		cp = cp->next_tag;
    }

	return;
}



