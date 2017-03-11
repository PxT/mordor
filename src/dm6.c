/*
 * DM6.C:
 *
 *  DM functions
 *
 *  Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *  Copyright (C) 1995 Brooke Paul
 *
 */

#include "mstruct.h"
#include "mextern.h"

/**********************************************************************/
/*				dm_dust				      */
/**********************************************************************/

/* This function allows a DM to destory a player.		      */

int dm_dust(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        creature        *crt_ptr;
        int             cfd, fd;
        char            str[IBUFSIZE+1]; 
	char		file[80];

        if(ply_ptr->class < DM)
                return(PROMPT);

	if(cmnd->num < 2) {
		print(ply_ptr->fd, "\nDust whom?\n");
		return(PROMPT);
	}
        lowercize(cmnd->str[1], 1);
        crt_ptr = find_who(cmnd->str[1]);
        if(!crt_ptr) {
                print(ply_ptr->fd, "%s is not on.\n", cmnd->str[1]);
                return(0);
	}
	if(crt_ptr->class > CARETAKER) {
		ANSI(crt_ptr->fd, RED);
		print(crt_ptr->fd, "%s tried to dust you!\n", ply_ptr->name);
		ANSI(crt_ptr->fd, WHITE);
		return(0);
	}
	cfd = crt_ptr->fd;
	if(!(Ply[cfd].io->fn == command && Ply[cfd].io->fnparam == 1)) {
		print(ply_ptr->fd, "Can't dust %s right now.\n", cmnd->str[1]);
		return(0);
	}
	ANSI(cfd, MAGENTA);
	print(cfd, "Lightning comes down from on high!  You have angered the gods!\n");
	ANSI(cfd, WHITE);
	broadcast_rom(cfd, crt_ptr->rom_num,"A bolt of lightning strikes %s from on high.\n",crt_ptr->name);
	broadcast("\n### %s has been turned to dust! We'll miss %s dearly.", Ply[cfd].ply->name, F_ISSET(Ply[cfd].ply, PMALES) ? "him":"her");
                sprintf(file, "%s/%s", PLAYERPATH, Ply[cfd].ply->name);
                disconnect(cfd);
        broadcast("### Ominous thunder rumbles in the distance.\n");
	        unlink(file);
                return(0);

}

/**********************************************************************/
/*                              dm_follow                             */
/**********************************************************************/
/*	This function allows a DM to force a monster to follow 	      */
/*	him, and has been made to allow for the movement of	      */
/*      custom monsters (made with the dm_crt_name function).	      */

int dm_follow (ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
	creature	*crt_ptr;
	room		*rom_ptr;
	ctag		*pp, *cp, *prev;
	int		fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(ply_ptr->class < CARETAKER)
                return(PROMPT);	

	if (cmnd->num < 2) {
		print (fd, "syntax: *cfollow <creature>\n");
		return(0);
	}

        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[1],
                           cmnd->val[1]);

	if(!crt_ptr) {
		print (fd, "Can't seem to locate that creature here.\n");
		return (0);
	}

	if(F_ISSET(crt_ptr, MPERMT)) {
		print (fd, "Perms can't follow.\n");
		return(0);
	}
	if(F_ISSET (crt_ptr, MDMFOL)) {
		F_CLR(crt_ptr, MDMFOL);
		print (fd, "%s stops following you.\n", crt_ptr->name);
		cp = ply_ptr->first_fol;
		if(cp->crt == crt_ptr) {
			ply_ptr->first_fol = cp->next_tag;
			free(cp);
		}
		else while (cp) {
			if (cp->crt == crt_ptr) {
				prev->next_tag = cp->next_tag;
				free(cp);
				break;
			}
		     prev = cp;
		     cp = cp->next_tag;
		     }

		crt_ptr->following = 0;
		return(0);
	}
	crt_ptr->following = ply_ptr;
	F_SET(crt_ptr, MDMFOL);
	pp = (ctag *)malloc(sizeof(ctag));
	if(!pp)
		merror("dmfollow", FATAL);
	pp->crt = crt_ptr;
	pp->next_tag = 0;
	
	if(!ply_ptr->first_fol)
		ply_ptr->first_fol = pp;
	else {
		pp->next_tag = ply_ptr->first_fol;
		ply_ptr->first_fol = pp;
	}	
	print (fd, "%s starts following you.\n", crt_ptr->name);
	return (0);
}

/************************************************************************/
/*			dm_attack					*/
/************************************************************************/
/* 	This function allows a DM to make a monster attack a given 	*/
/*  player.								*/

int dm_attack (ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
	creature	*atr_ptr, *atd_ptr;
	room		*rom_ptr;
	ctag		*pp, *cp, *prev;
	int		fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(ply_ptr->class < CARETAKER)
                return(PROMPT);	

	if (cmnd->num < 3) {
		print (fd, "syntax: *attack <monster> <defender>\n");
		return(0);
	}

        atr_ptr = find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[1],
                           cmnd->val[1]);

	if(!atr_ptr) {
		print (fd, "Can't seem to locate that attacker here.\n");
		return (0);
	}

	if(F_ISSET(atr_ptr, MPERMT)) {
		print (fd, "Perms can't do that.\n");
		return(0);
	}

        atd_ptr = find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[2],
                           cmnd->val[2]);

	if(!atd_ptr) {
	lowercize(cmnd->str[2], 1);
	atd_ptr = find_who(cmnd->str[2]);
	}

	if(!atd_ptr) {
		print (fd, "Can't seem to locate that victim here.\n");
		return (0);
	}

	if(F_ISSET(atd_ptr, MPERMT)) {
		print (fd, "Perms can't do that.\n");
		return(0);
	}
	print(fd, "Adding %s to attack list of %s.\n", atd_ptr->name, atr_ptr->name);
	add_enm_crt(atd_ptr->name, atr_ptr);
	broadcast_rom(atd_ptr->fd, atd_ptr->rom_num, "%M attacks %m.", atr_ptr, atd_ptr);
	if(atd_ptr->type = PLAYER) 
		print (atd_ptr->fd, "%M attacked you!\n", atr_ptr);
	return(0);
}

/***************************************************************************/
/*			list_enm					   */ 
/***************************************************************************/
/*	This function lists the enemy list of a given monster.		   */

int list_enm(ply_ptr, cmnd)
creature *ply_ptr;
cmd     *cmnd;

{
etag	*first_enm;
etag    *ep;
room	*rom_ptr;
creature *crt_ptr;
int	fd, n=0;

	if (ply_ptr->class < DM)
        	return(0);
	
	rom_ptr= ply_ptr->parent_rom;
	fd = ply_ptr->fd;

	crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[1], cmnd->val[1]);

	if(!crt_ptr){
		print(fd, "Not here.\n");
		return(0);
	}
	print(ply_ptr->fd,"Enemy list for %s:\n", crt_ptr->name);

        ep = crt_ptr->first_enm;

    while(ep) {
	n +=1;
	print (fd,"%s.\n", ep->enemy);
        ep = ep->next_tag;
    }
    if (!n)
	print(fd, "None.\n");
    return(0);
}

/**********************************************************************/
/*                              list_charm                            */
/**********************************************************************/
/*      This function allows a DM to see a given players charm list   */
                
int list_charm (ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
                        
creature        *crt_ptr;
int             fd, cfd, n=0;
ctag            *cp;
                                
        if(ply_ptr->class < DM)
                return(PROMPT);
                         
        if(cmnd->num < 2) {
                print(ply_ptr->fd, "See whose charm list?\n");
                return(PROMPT);
        }
                
        fd = ply_ptr->fd; 
        lowercize(cmnd->str[1], 1);
        crt_ptr = find_who(cmnd->str[1]);
        if(!crt_ptr) {
                print(ply_ptr->fd, "%s is not on.\n", cmnd->str[1]);
                return(0);
        }
        
        cfd = crt_ptr->fd;
         
        cp = Ply[cfd].extr->first_charm;
        print (fd, "Charm list for %s:\n", crt_ptr->name);
        while(cp) {
		n += 1;
                print(fd,"%s.\n", cp->crt->name); 
                cp = cp->next_tag;
        }
        if(!n)
		print(fd, "Nobody.\n");
	return(0);
}

