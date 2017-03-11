/*
 * DM6.C:
 *
 *  DM functions
 *
 *  Copyright (C) 1995 Brooke Paul
 *
 * $Id: dm6.c,v 6.17 2001/07/03 23:12:35 develop Exp $
 *
 * $Log: dm6.c,v $
 * Revision 6.17  2001/07/03 23:12:35  develop
 * change msg for attempt to possess conjured creature
 *
 * Revision 6.16  2001/07/03 23:00:10  develop
 * change so you cant possess someones conjure
 *
 * Revision 6.15  2001/06/28 04:52:33  develop
 * no alias while conjuring
 *
 * Revision 6.14  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

#include "../include/mordb.h"
#include "mextern.h"


/**********************************************************************/
/*				dm_dust				      */
/**********************************************************************/

/* This function allows a DM to destroy a player.		      */

int dm_dust(creature *ply_ptr, cmd *cmnd )
{
        creature        *crt_ptr;
	int             cfd;
	char		file[80];

    if(ply_ptr->class < DM)
		return(PROMPT);

	if(cmnd->num < 2) {
		output(ply_ptr->fd, "\nDust whom?\n");
		return(PROMPT);
	}
    lowercize(cmnd->str[1], 1);
    crt_ptr = find_who(cmnd->str[1]);
    if(!crt_ptr) {
        sprintf(g_buffer, "%s is not on.\n", cmnd->str[1]);
        output(ply_ptr->fd, g_buffer);
        return(0);
	}
	if(crt_ptr->class > BUILDER) {
		ANSI(crt_ptr->fd, AFC_RED);
		sprintf(g_buffer, "%s tried to dust you!\n", ply_ptr->name);
		output(crt_ptr->fd, g_buffer);
		ANSI(crt_ptr->fd, AFC_WHITE);
		return(0);
	}
	cfd = crt_ptr->fd;
	if(!(Ply[cfd].io->fn == command && Ply[cfd].io->fnparam == 1)) {
		sprintf(g_buffer, "Can't dust %s right now.\n", cmnd->str[1]);
		output(ply_ptr->fd, g_buffer);
		return(0);
	}
	ANSI(cfd, AFC_MAGENTA);
	scwrite(cfd, "\n[35mLightning comes down from on high!  You have angered the gods![37m\n", 74);

	ANSI(cfd, AFC_WHITE);
	sprintf(g_buffer, "A bolt of lightning strikes %s from on high.\n", crt_ptr->name);
	broadcast_rom(cfd, crt_ptr->rom_num, g_buffer, NULL);
	
	sprintf(g_buffer,"%s has been turned to dust!\n", crt_ptr->name);
	broadcast_rom(cfd, crt_ptr->rom_num, g_buffer, NULL);

	last_dust_output=time(0)+5L;
    sprintf(file, "%s/%s", get_player_path(), Ply[cfd].ply->name);
    disconnect(cfd);
	unlink(file);
    return(0);

}

/**********************************************************************/
/*                              dm_alias                             */
/**********************************************************************/
/*	This function allows a DM to become a monster. 		      */

int dm_alias (creature *ply_ptr, cmd *cmnd )
{
	creature	*crt_ptr;
	room		*rom_ptr;
	ctag		*pp, *cp, *prev;
	int		fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(ply_ptr->class < CARETAKER )
	    return(PROMPT);	

	if (cmnd->num < 2) {
		output (fd, "Syntax: *possess <creature>\n");
		return(0);
	}
	cp = ply_ptr->first_fol;
	while(cp) {
		if(cp->crt->type == MONSTER && F_ISSET(cp->crt, MCONJU)) {
			output(fd, "You may not possess a conjured creature.\n");
			return(0);
		}
		cp = cp->next_tag;
	}
	
    crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[1],
                           cmnd->val[1]);

	if(!crt_ptr){
		output (fd, "Can't seem to locate that creature here.\n");
		return (0);
	}
	
	if(crt_ptr->type != MONSTER) {
		output (fd, "Their soul refuses to be displaced.\n");
		return (0);
	}

	if(F_ISSET(crt_ptr, MCONJU)) {
		output(fd, "Their soul refuses to be displaced.\n");
		return(0);
	}

	if(F_ISSET(crt_ptr, MPERMT)) {
		output (fd, "Their soul refuses to be displaced.\n");
		return(0);
	}
	if(F_ISSET(ply_ptr, PALIAS) && crt_ptr != Ply[fd].extr->alias_crt) {
                        output (fd, "You may only possess one creature at a time.\n");
                        return(0);
        }
	if(F_ISSET(crt_ptr, MDMFOL)) 
	{
		if(crt_ptr != Ply[fd].extr->alias_crt) {
			output (fd, "Their soul belongs to another.\n");
			return(0);
		}
		F_CLR(crt_ptr, MDMFOL);
		F_CLR(ply_ptr, PALIAS);
		mprint (fd, "You release %1m's body.\n", m1arg(crt_ptr));

		/* *cfollow code */
		cp = ply_ptr->first_fol;
		if(cp->crt == crt_ptr) 
		{
			ply_ptr->first_fol = cp->next_tag;
			free(cp);
		}
		else 
			while (cp) 
			{
				if (cp->crt == crt_ptr) 
				{
					prev->next_tag = cp->next_tag;
					free(cp);
					break;
				}
				prev = cp;
				cp = cp->next_tag;
			} 
		crt_ptr->following = 0;	
		Ply[fd].extr->alias_crt = 0;
		return(0);
	}
	/* *cfollow code */

	crt_ptr->following = ply_ptr;
	pp = 0;
	pp = (ctag *)malloc(sizeof(ctag));
	if(!pp)
		merror("dmalias", FATAL);
	pp->crt = crt_ptr;
	pp->next_tag = 0;
	
	if(!ply_ptr->first_fol) {
		ply_ptr->first_fol = pp;
		pp->next_tag = 0;
	}
	else {
		pp->next_tag = ply_ptr->first_fol;
		ply_ptr->first_fol = pp;
	} 

	Ply[fd].extr->alias_crt = crt_ptr;
	F_SET(ply_ptr, PALIAS);	
	F_SET(crt_ptr, MDMFOL);

/*  Mike Kolb doesn't like this *//*
	ply_ptr->strength = crt_ptr->strength;
	ply_ptr->dexterity = crt_ptr->dexterity;
	ply_ptr->constitution = crt_ptr->constitution;
	ply_ptr->intelligence = crt_ptr->intelligence;
	ply_ptr->piety = crt_ptr->piety;
	ply_ptr->level = crt_ptr->level;
	ply_ptr->ndice = crt_ptr->ndice;
	ply_ptr->sdice = crt_ptr->sdice;
	ply_ptr->pdice = crt_ptr->pdice; */
	
	/* if(F_ISSET(crt_ptr, MMALES))
		F_SET(ply_ptr, PMALES);
	else
		F_CLR(ply_ptr, PMALES); */

	mprint (fd, "You possess %1m.\n", m1arg(crt_ptr));

	/* make sure we are invis at this point */
	if(!F_ISSET(ply_ptr, PDMINV)) 
	{
		/* dm_invis does not use the secoond param */
		dm_invis(ply_ptr, NULL);
	}

	return (0);
}

/************************************************************************/
/*			dm_attack					*/
/************************************************************************/
/* 	This function allows a DM to make a monster attack a given 	*/
/*  player.								*/

int dm_attack (creature *ply_ptr, cmd *cmnd )
{
	creature	*atr_ptr, *atd_ptr;
	room		*rom_ptr;
	int		fd, inroom=1;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(ply_ptr->class < CARETAKER )
                return(PROMPT);	

	if (cmnd->num < 3) {
		output (fd, "syntax: *attack <monster> <defender>\n");
		return(0);
	}

    atr_ptr = find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[1],
                           cmnd->val[1]);

	if(!atr_ptr) {
		output (fd, "Can't seem to locate that attacker here.\n");
		return (0);
	}

	if(F_ISSET(atr_ptr, MPERMT)) {
		output (fd, "Perms can't do that.\n");
		return(0);
	}

  	atd_ptr = find_crt(ply_ptr, rom_ptr->first_mon,cmnd->str[2], cmnd->val[2]);

	if(!atd_ptr) {
		lowercize(cmnd->str[2], 1);
		atd_ptr = find_who(cmnd->str[2]);
		inroom=0;
	}
	if(!atd_ptr) {
		output (fd, "Can't seem to locate that victim here.\n");
			output(fd, "Please use full names.\n");
			return (0);
	}

	if(F_ISSET(atd_ptr, MPERMT)) {
		output (fd, "Perms can't do that.\n");
		return(0);
	}
	sprintf(g_buffer, "Adding %s to attack list of %s.\n", atd_ptr->name, atr_ptr->name);
	output(fd, g_buffer);
 	add_enm_crt(atd_ptr->name, atr_ptr); 
	attack_crt(atr_ptr, atd_ptr); 
	
	if(inroom) {
		broadcast_rom(atd_ptr->fd, atd_ptr->rom_num, "%M attacks %m.", m2args(atr_ptr, atd_ptr));
		mprint (atd_ptr->fd, "%M attacked you!\n", m1arg(atr_ptr)); 
	}
	return(0);
}

/***************************************************************************/
/*			list_enm					   */ 
/***************************************************************************/
/*	This function lists the enemy list of a given monster.		   */

int list_enm(creature *ply_ptr, cmd *cmnd )
{
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
		output(fd, "Not here.\n");
		return(0);
	}
	sprintf(g_buffer, "Enemy list for %s:\n", crt_ptr->name);
	output(fd, g_buffer);

    ep = crt_ptr->first_enm;

    while(ep) {
		n +=1;
		sprintf(g_buffer, "%s.\n", ep->enemy);
		output(fd, g_buffer);
        ep = ep->next_tag;
    }
    if (!n)
	output(fd, "None.\n");
    return(0);
}

/**********************************************************************/
/*                              list_charm                            */
/**********************************************************************/
/*      This function allows a DM to see a given players charm list   */
                
int list_charm (creature *ply_ptr, cmd *cmnd )
{
                        
	creature        *crt_ptr;
	int             fd, cfd, n=0;
	ctag            *cp;
                                
    if(ply_ptr->class < DM)
	    return(PROMPT);
                     
    if(cmnd->num < 2) {
        output(ply_ptr->fd, "See whose charm list?\n");
        return(PROMPT);
    }
            
    fd = ply_ptr->fd; 
    lowercize(cmnd->str[1], 1);
    crt_ptr = find_who(cmnd->str[1]);
    if(!crt_ptr) {
        sprintf(g_buffer, "%s is not on.\n", cmnd->str[1]);
        output(ply_ptr->fd, g_buffer);
        return(0);
    }
    
    cfd = crt_ptr->fd;
     
    cp = Ply[cfd].extr->first_charm;
	sprintf( g_buffer, "Charm list for %s:\n", crt_ptr->name);
    output(fd, g_buffer);
    while(cp) {
	n += 1;
        sprintf(g_buffer,"%s.\n", cp->crt->name); 
        output(fd, g_buffer); 
        cp = cp->next_tag;
    }

    if(!n)
		output(fd, "Nobody.\n");

	return(0);
}

/**********************************************************************/
/*                              dm_auth                               */
/**********************************************************************/
/*      This function allows a DM to authorize a character online.    */
        
int dm_auth(creature *ply_ptr, cmd *cmnd )
{
creature        *crt_ptr;
int	fd;
                
        if(ply_ptr->class < DM)
                return(PROMPT);

	fd =ply_ptr->fd;

	lowercize(cmnd->str[1], 1);
    crt_ptr = find_who(cmnd->str[1]);
    if(!crt_ptr) {
        sprintf(g_buffer, "%s is not on.\n", cmnd->str[1]);
        output(fd, g_buffer);
        return(0);
    }
	
	if(!F_ISSET(crt_ptr, PAUTHD)) {
		F_SET(crt_ptr, PAUTHD);
		sprintf(g_buffer, "%s given authorization.\n", crt_ptr->name);
		output(fd, g_buffer);
		return(0);
	}
	else {
		F_CLR(crt_ptr, PAUTHD);
		F_CLR(crt_ptr, PSECOK);
		sprintf(g_buffer, "Authorization removed from %s.\n", crt_ptr->name);
		output(fd, g_buffer);
		return(0);
	}
}

/**********************************************************************/
/*			dm_flash				      */
/**********************************************************************/

/*  This function allows a DM to output a string to an individual     */
/*  players screen. 						      */

int dm_flash(creature *ply_ptr, cmd *cmnd )
{
	creature        *crt_ptr = 0;
    int             spaces=0, i, fd, j;
    int             len;

    fd = ply_ptr->fd;

    if(ply_ptr->class < DM)
	return(PROMPT);

    if(cmnd->num < 2) {
            output(fd, "DM flash to whom?\n");
            return 0;
    }

    cmnd->str[1][0] = up(cmnd->str[1][0]);
    for(i=0; i<Tablesize; i++) {
            if(!Ply[i].ply) continue;
            if(Ply[i].ply->fd == -1) continue;
            if(!strncmp(Ply[i].ply->name, cmnd->str[1],
               strlen(cmnd->str[1])))
                    crt_ptr = Ply[i].ply;
            if(!strcmp(Ply[i].ply->name, cmnd->str[1]))
                    break;
    }
    if(!crt_ptr) {
	    output(fd, "Send to whom?\n");
        return(0);
	}

	len = strlen(cmnd->fullstr);
    for(j=0; j< len && j<256; j++) {
		if(cmnd->fullstr[j] == ' ' && cmnd->fullstr[j+1] != ' ')
			spaces++;
        if(spaces==2) 
			break;
    }
    cmnd->fullstr[255] = 0;
                
    if(spaces < 2 || strlen(&cmnd->fullstr[j+1]) < 1) {
            output(fd, "Send what?\n");
            return(0);
    }

	if(F_ISSET(ply_ptr, PLECHO)){
		ANSI(fd, ECHOCOLOR);
        sprintf(g_buffer, "You flashed: \"%s\" to %%M.\n", &cmnd->fullstr[j+1]);
        mprint(fd, g_buffer, m1arg(crt_ptr));
        ANSI(fd, MAINTEXTCOLOR);
    }
    else
	{
        sprintf(g_buffer, "Message flashed to %s.\n", crt_ptr->name);
        output(fd, g_buffer);
	}

    sprintf(g_buffer, "%s.\n", &cmnd->fullstr[j+1]);
    output(crt_ptr->fd, g_buffer);
    return(0);

}
