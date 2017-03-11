/*
 * OBJECT.C:
 *
 *	Routines that deal with objects and items.
 *
 *	Copyright (C) 1991, 1992, 1993 Brooke Paul
 *
 * $Id: object.c,v 6.16 2001/07/25 03:23:12 develop Exp $
 *
 * $Log: object.c,v $
 * Revision 6.16  2001/07/25 03:23:12  develop
 * testing duplicate object to fix messed up flags
 *
 * Revision 6.15  2001/07/25 02:55:04  develop
 * added duplicate_object for thief dropping stolen items
 *
 * Revision 6.14  2001/07/17 19:28:44  develop
 * *** empty log message ***
 *
 * Revision 6.13  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

#include "../include/mordb.h"
#include "mextern.h"

/**********************************************************************/
/*				add_obj_obj			      */
/**********************************************************************/

/* This function adds the object pointed to by the first parameter to  */
/* the contents of the object pointed to by the second parameter.  It  */
/* is done alphabetically.					       */

void add_obj_obj(object	*obj_ptr1, object *obj_ptr2 )
{
	otag	*op, *temp, *prev;

	obj_ptr1->parent_obj = obj_ptr2;
	obj_ptr1->parent_crt = 0;
	obj_ptr1->parent_rom = 0;

	op = (otag *)malloc(sizeof(otag));
	if(!op)
		merror("add_obj_obj", FATAL);
	op->obj = obj_ptr1;
	op->next_tag = 0;

	if(!obj_ptr2->first_obj) {
		obj_ptr2->first_obj = op;
		return;
	}

	temp = obj_ptr2->first_obj;
	if(strcmp(temp->obj->name, obj_ptr1->name) > 0 ||
	   (!strcmp(temp->obj->name, obj_ptr1->name) &&
	   temp->obj->adjustment > obj_ptr1->adjustment)) {
		op->next_tag = temp;
		obj_ptr2->first_obj = op;
		return;
	}

	while(temp) {
		if(strcmp(temp->obj->name, obj_ptr1->name) > 0 ||
		   (!strcmp(temp->obj->name, obj_ptr1->name) &&
		   temp->obj->adjustment > obj_ptr1->adjustment))
			break;
		prev = temp;
		temp = temp->next_tag;
	}
	op->next_tag = prev->next_tag;
	prev->next_tag = op;

}

/**********************************************************************/
/*				del_obj_obj			      */
/**********************************************************************/

/* This function removes the object pointed to by the first parameter */
/* from the object pointed to by the second.			      */

void del_obj_obj(object	*obj_ptr1, object *obj_ptr2 )
{
	otag 	*temp, *prev;

	obj_ptr1->parent_crt = 0;
	if(obj_ptr2->first_obj->obj == obj_ptr1) {
		temp = obj_ptr2->first_obj->next_tag;
		free(obj_ptr2->first_obj);
		obj_ptr2->first_obj = temp;
		return;
	}

	prev = obj_ptr2->first_obj;
	temp = prev->next_tag;
	while(temp) {
		if(temp->obj == obj_ptr1) {
			prev->next_tag = temp->next_tag;
			free(temp);
			return;
		}
		prev = temp;
		temp = temp->next_tag;
	}
}

/**********************************************************************/
/*				find_obj			      */
/**********************************************************************/

/* This function is used to search through a room's, creature's or	*/
/* object's inventory in order to look for a particular item.  The	*/
/* first parameter contains a pointer to the creature who is doing the	*/
/* search.  The second is a pointer to the first object tag in the 	*/
/* list to be searched.  The third parameter is a string that contains	*/
/* the name of the object being searched for.  The last parameter 	*/
/* contains the version of the string being searched for.		*/

object *find_obj(creature *ply_ptr, otag *first_ot, char *str, int val )
{
	otag	*op;
	int	match=0, found=0;

	if(!first_ot)
		return(0);
	op = first_ot;
	while(op) {
		if(EQUAL(op->obj, str) &&
		   (F_ISSET(ply_ptr, PDINVI) ?
		   1:!F_ISSET(op->obj, OINVIS))) {
			match++;
			if(match == val) {
				found = 1;
				break;
			}
		}
		op = op->next_tag;
	}

	if(found)
		return(op->obj);
	else
		return(0);
}

/**********************************************************************/
/*				list_obj			      */
/**********************************************************************/

/* This function produces a string which lists all the objects in a	*/
/* player's, room's or object's inventory.  The first parameter holds	*/
/* a pointer to the string which will be produced.  The second is a	*/
/* pointer to the player who is having the list produced.  The last	*/
/* is a pointer to the first object tag in the list of items that	*/
/* it being listed.							*/

int list_obj(char *str, creature *ply_ptr, otag	*first_otag ) 
{
	otag	*op;
	int	m, n=0;
	char 	*str2;
	int		ct;

	ct = ply_ptr->class > BUILDER; 

	str[0] = 0;
	op = first_otag;
	while(op && strlen(str) < 2000) {
		if( ct || ((F_ISSET(ply_ptr, PDINVI) ? 1:!F_ISSET(op->obj, OINVIS)) &&
		   !F_ISSET(op->obj, OHIDDN) && !F_ISSET(op->obj, OSCENE))) {
			m=1;
			while(op->next_tag) {
				if(!strcmp(op->next_tag->obj->name, op->obj->name) &&
				   (op->next_tag->obj->adjustment == op->obj->adjustment ||
				   !F_ISSET(ply_ptr, PDMAGI)) 
				   && ( ct || ((F_ISSET(ply_ptr, PDINVI) ?
				   1:!F_ISSET(op->next_tag->obj, OINVIS)) &&
				   !F_ISSET(op->next_tag->obj, OHIDDN) &&
				   !F_ISSET(op->next_tag->obj, OSCENE)))) {

					m++;
					op = op->next_tag;
				}
				else
					break;
			}
			str2=obj_str(ply_ptr, op->obj, m, 0);
			if(strlen(str2)+strlen(str) < 2000){
				strcat(str, str2);
				strcat(str, ", ");
				n++;
			}
		}
		op = op->next_tag;
	}
	if(n) {
		str[strlen(str)-2] = 0;
		return(1);
	}
	else
		return(0);

}

/**********************************************************************/
/*				weight_obj			      */
/**********************************************************************/

/* This function computes the total amount of weight of an object and */
/* all its contents.						      */

int weight_obj(object *obj_ptr )
{
	int	n;
	otag	*op;

	n = obj_ptr->weight;
	op = obj_ptr->first_obj;
	while(op) {
		if(!F_ISSET(op->obj, OWTLES))
			n += weight_obj(op->obj);
		op = op->next_tag;
	}

	return(n);

}



/************************************************************************/
/*				rand_enchant				*/
/************************************************************************/
/* This function randomly enchants an object if its random-enchant flag */
/* is set.								*/
void rand_enchant(object *obj_ptr )
{
	char m;

	m = mrand(1,100);
	if(m > 98) 
	{
		enchant_obj( obj_ptr, 3 );
	}
	else if(m > 90) 
	{
		enchant_obj( obj_ptr, 2 );
	}
	else if(m > 50) 
	{
		enchant_obj( obj_ptr, 1 );
	}

	return;
}



/************************************************************************/
/* enchant_obj								*/
/*	This is the baseline enchant call that all forms of enchant	*/
/*		should go through so the formula is consistant.		*/
/************************************************************************/
void enchant_obj( object *obj_ptr, int value )
{

	F_SET(obj_ptr, OENCHA);
	obj_ptr->adjustment = value;
	obj_ptr->pdice += value; 
	obj_ptr->pdice = MAX(obj_ptr->pdice, obj_ptr->adjustment);

	return;
}


/************************************************************************/
/* clear_enchant							*/
/*	This is the opposite of enchant_obj() and should be called to	*/
/*	clear temp enchantments.					*/
/************************************************************************/
void clear_enchant( object *obj_ptr )
{

	F_CLR(obj_ptr, OENCHA);
	obj_ptr->pdice = MAX(obj_ptr->pdice - obj_ptr->adjustment, 0);
	obj_ptr->adjustment = 0;
 

	return;
}


#ifdef TODO
/**********************************************************************/
/*	add_obj_to_list	                                              */
/* This function adds the object pointer to by the first parameter to */
/* the object list pointed to by the second parameter.                */
/**********************************************************************/
void add_obj_to_list(object	*obj_ptr, otag *obj_list )
{
	otag	*op, *temp, *prev;

	op = (otag *)malloc(sizeof(otag));
	if(!op)
		merror("add_obj_crt", FATAL);
	op->obj = obj_ptr;
	op->next_tag = 0;

	/* is the list empty */
	if(!obj_list) {
		obj_list = op;
		return;
	}

	/* otherwise, insert sorted by alpha */
	temp = obj_list;
	if(strcmp(temp->obj->name, obj_ptr->name) > 0 ||
	   (!strcmp(temp->obj->name, obj_ptr->name) &&
	   temp->obj->adjustment > obj_ptr->adjustment)) {
		op->next_tag = temp;
		obj_list = op;
		return;
	}

	while(temp) {
		if(strcmp(temp->obj->name, obj_ptr->name) > 0 ||
		   (!strcmp(temp->obj->name, obj_ptr->name) &&
		   temp->obj->adjustment > obj_ptr->adjustment))
			break;
		prev = temp;
		temp = temp->next_tag;
	}
	op->next_tag = prev->next_tag;
	prev->next_tag = op;

	return;
}



/**********************************************************************/
/*	del_obj_from_list					      */
/* This function removes the object pointer to by the first parameter */
/* from the player pointed to by the second.			      */
/**********************************************************************/
void del_obj_from_list(object *obj_ptr, otag *obj_list )
{
	otag 	*temp, *prev;

	if(obj_list->obj == obj_ptr) {
		temp = obj_list->next_tag;
		free(obj_list);
		obj_list = temp;
		return;
	}

	prev = obj_list;
	temp = prev->next_tag;
	while(temp) {
		if(temp->obj == obj_ptr) {
			prev->next_tag = temp->next_tag;
			free(temp);
			return;
		}
		prev = temp;
		temp = temp->next_tag;
	}

	return;
}

#endif

/* this will return the value of a given object based upon remaining shots */
long find_obj_value(object *obj_ptr)
{
	if(obj_ptr->shotscur == obj_ptr->shotsmax)
		return(obj_ptr->value);

	if(obj_ptr->shotscur < 1)
		return(0);

	if(obj_ptr->shotscur > (obj_ptr->shotsmax * 9 / 10))
        	return(obj_ptr->value * 9 / 10);
        else if(obj_ptr->shotscur > (obj_ptr->shotsmax * 8 / 10))
        	return(obj_ptr->value * 8 / 10);
	else if(obj_ptr->shotscur > (obj_ptr->shotsmax * 7 / 10))
		return(obj_ptr->value * 7 /10);
	else if(obj_ptr->shotscur > (obj_ptr->shotsmax * 6 / 10))
		return(obj_ptr->value * 6 /10); 
	else if(obj_ptr->shotscur > (obj_ptr->shotsmax * 5 / 10))
		return(obj_ptr->value / 2);
	else if(obj_ptr->shotscur > (obj_ptr->shotsmax * 4 / 10))
		return(obj_ptr->value * 4 / 10);
	else if(obj_ptr->shotscur > (obj_ptr->shotsmax * 3 / 10))
		return(obj_ptr->value * 3 / 10);
	else if(obj_ptr->shotscur > (obj_ptr->shotsmax * 2 / 10))
		return(obj_ptr->value * 2 / 10);
	else if(obj_ptr->shotscur > 0)
		return(obj_ptr->value / 10);
	else
		return(5);
}

/* this function will copy an existing object into a new one
	and return the duplicate */
object *duplicate_object(object *old_obj)
{

	object *new_obj;
	int	i;

	new_obj = (object *)malloc(sizeof(object));
        if(!new_obj) {
                merror("duplicate_object", FATAL);
                return(0);
        }

	strcpy(new_obj->name, old_obj->name);
	strcpy(new_obj->description, old_obj->description);
	strcpy(new_obj->key[0], old_obj->key[0]);
	strcpy(new_obj->key[1], old_obj->key[1]);
	strcpy(new_obj->key[2], old_obj->key[2]);
	strcpy(new_obj->use_output, old_obj->use_output);
	new_obj->value = old_obj->value;
	new_obj->weight =  old_obj->weight;
	new_obj->type = old_obj->type;
	new_obj->adjustment = old_obj->adjustment;
	new_obj->shotsmax = old_obj->shotsmax;
	new_obj->shotscur = old_obj->shotscur;
	new_obj->ndice = old_obj->ndice;
	new_obj->sdice = old_obj->sdice;
	new_obj->pdice = old_obj->pdice;
	new_obj->armor = old_obj->armor;
        new_obj->wearflag= old_obj->wearflag;
	new_obj->magicpower= old_obj->magicpower;
	new_obj->magicrealm= old_obj->magicrealm;
	new_obj->special = old_obj->special;

/*	for(i=0; i<128; i++)
		if(F_ISSET(old_obj, i))
			F_SET(new_obj, i); */

	for(i=0; i<16; i++)
                new_obj->flags[i] = old_obj->flags[i];

	new_obj->questnum= old_obj->questnum;
        new_obj->first_obj = old_obj->first_obj;
        new_obj->parent_obj = old_obj->parent_obj;
        new_obj->parent_rom = old_obj->parent_rom;
	new_obj->parent_crt = old_obj->parent_crt;

	new_obj->strength= old_obj->strength;
	new_obj->dexterity= old_obj->dexterity;
	new_obj->constitution= old_obj->constitution;
	new_obj->intelligence= old_obj->intelligence;
	new_obj->piety= old_obj->piety;

	for(i=0; i<16; i++)
		new_obj->sets_flag[i] = old_obj->sets_flag[i];

	new_obj->special1 = old_obj->special1;
	new_obj->special2 = old_obj->special2;

	return(new_obj);
}
