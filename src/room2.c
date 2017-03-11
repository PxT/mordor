/*
 * ROOM2.C
 * (c) 2000 Paul Telford
 * $Id: room2.c,v 6.13 2001/03/08 16:09:09 develop Exp $
 *
 * $Log: room2.c,v $
 * Revision 6.13  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

#include "../include/mordb.h"
#include "mextern.h"

/**********************************************************************/
/*				create_a_room			      */
/**********************************************************************/
/* creates an extremely minimal room				      */

int create_a_room(int num, room *rom_ptr)
{

	rom_ptr= (room *)malloc(sizeof(room));
	zero(rom_ptr, sizeof(room));

	sprintf(rom_ptr->name, "Room %d", num);
	rom_ptr->rom_num=num;
	if(save_rom_to_file(num, rom_ptr) < 0)
		return(-1);
	sprintf(g_buffer, "Creating room %d!\n", num);
	elog_broad(g_buffer);

	return(0);
}

/**********************************************************************/
/*				check_random_exits		      */
/**********************************************************************/

void check_random_exits(room *rom_ptr)
{

	xtag *xt;
	exit_ *xp = NULL;
	char temp[20];
	int i,j;
	short exits[50] = { 0 };

	xt = rom_ptr->first_ext;

	while(xt) {
		if(F_ISSET(xt->ext, XRAND2)) {
			for(i=1;;i++) {
				sprintf(temp, "%s%d", xt->ext->name,i);
				xp = find_all_ext(xt,temp);
				if(xp)
					exits[i-1] = xp->room;
				else
					break;
			}
			i--;
			if(i <= 0)
				return;

			j = mrand(1,i);
			xt->ext->room = exits[j-1];	
			
		}
		xt = xt->next_tag;
	}
}

exit_ *find_all_ext(xtag *first_xt, char *str)
{
	xtag	*xp;
	int	found = 0;

	xp = first_xt;
	while(xp) {
		if(!strncmp(xp->ext->name, str, strlen(str))) {
			found = 1;
			break;
		}
		
		xp = xp->next_tag;
	}

	if(!found)
		return(0);

	return(xp->ext);

}

void flush_crier()
{
	ctag *cp,*ctemp,*fol, *folprev;
	room *rom_ptr;

	flush_crt();
		
	elog_broad("Town Crier talk file updated.");

	if(load_rom(2, &rom_ptr) < 0)
		return;

	cp = rom_ptr->first_mon;
        rom_ptr->first_mon = 0;
        while(cp) {
                ctemp = cp->next_tag;
                if(F_ISSET (cp->crt, MDMFOL)) {
                        /* clear relevant follow lists */
                        F_CLR(cp->crt->following, PALIAS);
                        Ply[cp->crt->following->fd].extr->alias_crt = 0;
                        fol = cp->crt->following->first_fol;
                        if(fol->crt == cp->crt) {
                                cp->crt->following->first_fol = fol->next_tag;
                                free(fol);
                        }
                       else {
                                while(fol) {
                                        if(fol->crt == cp->crt) {
                                                folprev = fol->next_tag;
                                               free(fol);
                                                break;
                                        }
                                        folprev = fol;
                                        fol = fol->next_tag;
                                }
                        }
                }
                free_crt(cp->crt);
                free(cp);

                cp = ctemp;
        }               

	add_permcrt_rom(rom_ptr);

}
