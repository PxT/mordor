/*
 * 	COMMAND12.C:
 *
 *	(C) Brooke Paul 1994
 * $Id: command12.c,v 6.19 2001/08/12 20:26:13 develop Exp $
 *
 * $Log: command12.c,v $
 * Revision 6.19  2001/08/12 20:26:13  develop
 * *** empty log message ***
 *
 * Revision 6.16  2001/06/23 05:38:31  develop
 * changed monk meditate properties to stop usage while
 * in battle
 *
 * Revision 6.15  2001/06/23 04:22:31  develop
 * removed meditation during combat
 *
 * Revision 6.14  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

#include "../include/mordb.h"
#include "mextern.h"


/***********************************************************************/
/*                              bard_song                             */
/***********************************************************************/

int bard_song( creature *ply_ptr, cmd *cmnd )
{          
   	int     fd;
	time_t	i, t;
	int		heal;
	ctag	*cp;
	ctag	*cp_tmp;

	fd = ply_ptr->fd;
	
        if(ply_ptr->class != BARD && ply_ptr->class < BUILDER) {
                output(fd, "Only bards have that skill.\n");
				return(PROMPT);
        }
	if(ply_ptr->level < 4 && ply_ptr->class < BUILDER) {
		output(fd, "You have not practiced enough to do that yet.\n");
		return(0);
	}
	if(F_ISSET(ply_ptr, PSILNC)) {
		output_wc( fd, "A spell has taken your voice, and you cannot sing.\n", SILENCECOLOR);
		return(0);
	}		
	
	/*i = ply_ptr->lasttime[LT_SINGS].ltime+ply_ptr->lasttime[LT_SINGS].interval; */
	i = LT(ply_ptr, LT_SINGS);
	t = time(0);
	if(i>t) 
	{
		please_wait( fd, i-t );
	 
	   return(0);
	}
	
	ply_ptr->lasttime[LT_SINGS].ltime = t;
	ply_ptr->lasttime[LT_SINGS].interval = 120L;

	cp = ply_ptr->parent_rom->first_ply;

		output(fd,"Your music rejuvenates everyone in the room.\n");
		
		heal = mrand(1,4) + ply_ptr->level;            

		if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                	heal += mrand(1,4);
                	output(fd,"The room's magical properties increase the power of your song.\n");
            	}		
		while(cp){
			cp_tmp = cp->next_tag;
			if(cp->crt->type != MONSTER) {
				if(cp->crt != ply_ptr) {
					mprint(cp->crt->fd,"%M's song rejuvinates your spirits.\n",
						m1arg(ply_ptr));
				}
			cp->crt->hpcur += heal;
			cp->crt->hpcur = MIN(cp->crt->hpmax, cp->crt->hpcur);
			}
			cp = cp_tmp;
		}

	return(0);
}
/**********************************************************************/
/*              bard_song2                                            */
/**********************************************************************/
/*	AKA Charm						      */

int bard_song2( creature *ply_ptr, cmd *cmnd )
{
    room        *rom_ptr;
    creature    *crt_ptr;
    int			fd, dur, chance;
	time_t		i, t;

    fd = ply_ptr->fd;
	
	if(ply_ptr->class != BARD && ply_ptr->class < BUILDER) {
		output(fd, "Only bards have that skill.\n");
		return(0);
	}
    	
	if(F_ISSET(ply_ptr, PSILNC)) {
        output_wc(fd, "A spell has taken your voice and you cannot sing.\n", SILENCECOLOR);
        return(0);
   	}

	if(cmnd->num < 2) {
	    output(fd, "Charm whom?\n");
        return(0);
    }


#ifdef OLD_CHARM
 	if(!dec_daily(&ply_ptr->daily[DL_CHARM]) && ply_ptr->class < BUILDER) {
		output(fd, "You have charmed enough today.\n");
		return(0);
	}   	
#else
	i = LT(ply_ptr, LT_CHARM);
	t = time(0);
	if(i>t) 
	{
		please_wait( fd, i-t );
		return(0);
	}
	
	ply_ptr->lasttime[LT_CHARM].ltime = t;
	ply_ptr->lasttime[LT_CHARM].interval = 600L;
#endif

	rom_ptr = ply_ptr->parent_rom;

    dur =  300 + mrand(1,30)*10  + bonus[(int)ply_ptr->constitution]*30 + ply_ptr->level*5;

    crt_ptr = find_crt_in_rom(ply_ptr, rom_ptr,
               cmnd->str[1], cmnd->val[1], PLY_FIRST);

    if(!crt_ptr) {
        output(fd, "That's not here.\n");
        return(0);
    }

	if(!is_crt_killable(crt_ptr, ply_ptr)) 
	{
		return(0);
	}

	chance = MIN(90, ((ply_ptr->level)-(crt_ptr->level))*20+4*bonus[(int)ply_ptr->piety]);

	if(F_ISSET(crt_ptr, MUNDED) || 	F_ISSET(crt_ptr, MPERMT) || chance < mrand(1,100))
	{
		mprint(fd, "Your charm has no effect on %m.\n", m1arg(crt_ptr));
		broadcast_rom(fd, ply_ptr->rom_num, "%M sings way off key.\n", m1arg(ply_ptr));
		if(crt_ptr->type == MONSTER) {
                	add_enm_crt(ply_ptr->name, crt_ptr);
			return(0);
		}
		ANSI(crt_ptr->fd, AFC_MAGENTA);
		mprint(crt_ptr->fd, "%M tried to charm you.\n", m1arg(ply_ptr));
		ANSI(crt_ptr->fd, MAINTEXTCOLOR);
		return(0);
	}		
	
        if((crt_ptr->type == PLAYER && F_ISSET(crt_ptr, PRMAGI)) ||
           (crt_ptr->type != PLAYER && F_ISSET(crt_ptr, MRMAGI)))
            dur /= 2;


            mprint(fd, "Your song charms %m.\n", m1arg(crt_ptr));
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "%M sings to %m.",
                       m2args(ply_ptr, crt_ptr));
            mprint(crt_ptr->fd, "%M's song charms you.\n", m1arg(ply_ptr));
            add_charm_crt(crt_ptr, ply_ptr);

			crt_ptr->lasttime[LT_CHRMD].ltime = time(0);
            crt_ptr->lasttime[LT_CHRMD].interval = dur;

	    if(crt_ptr->type == PLAYER)
		   	F_SET(crt_ptr, PCHARM);
	    else 
			F_SET(crt_ptr, MCHARM);


    return(0);
}


/********************************************************************/
/*			meditate				    */
/********************************************************************/

/* This command is for monks.					  */

int meditate( creature	*ply_ptr, cmd *cmnd )
{
	int	fd, chance;
	time_t	i,t;

	fd = ply_ptr->fd;

	if(ply_ptr->class != MONK && ply_ptr->class < BUILDER){
		output(fd, "Only monks may meditate.\n");
		return(0);
	}

	i = LT(ply_ptr, LT_MEDIT );
    	t = time(0);

    	if(i>t) {
		please_wait( fd, i-t );
		return(0);
    	}

	if(LT(ply_ptr,LT_ATTCK) > t) {
                output(ply_ptr->fd,"You cannot meditate while fighting!\n");
                ply_ptr->lasttime[LT_MEDIT].ltime = t;
                ply_ptr->lasttime[LT_ATTCK].ltime = t;
                ply_ptr->lasttime[LT_MEDIT].interval = 120L;
                return(0);
        }

	chance = MIN(70, ply_ptr->level*10+bonus[(int)ply_ptr->piety]);

	if(mrand(1,100) <= chance) {
		output(fd, "You feel at one with the universe.\n");
		broadcast_rom(fd, ply_ptr->rom_num, "%M meditates.", m1arg(ply_ptr));
		ply_ptr->hpcur += mrand(4,10)+ply_ptr->level+bonus[(int)ply_ptr->constitution];
		ply_ptr->hpcur = MIN(ply_ptr->hpmax, ply_ptr->hpcur);
		ply_ptr->lasttime[LT_MEDIT].ltime = t;
		ply_ptr->lasttime[LT_MEDIT].interval = 120L;
	}
	else {
		output(fd, "Your spirit is not at peace.\n");
		broadcast_rom(fd, ply_ptr->rom_num, "%M meditates.",
			      m1arg(ply_ptr));
		ply_ptr->lasttime[LT_MEDIT].ltime = t;
		ply_ptr->lasttime[LT_MEDIT].interval = 5L;
	}

	return(0);

}
	

/**********************************************************************/
/*			touch_of_death				      */
/**********************************************************************/

/* This function allows monks to kill nonundead creatures.             */
/* If they succeed then the creature is either killed or harmed        */
/* for approximately half of its hit points.			       */

int touch_of_death( creature *ply_ptr, cmd *cmnd )
{
	creature	*crt_ptr;
	room		*rom_ptr;
	time_t		i, t;
	int			chance, m, dmg, fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(cmnd->num < 2) {
		output(fd, "Touch whom?\n");
		return(0);
	}

	if(ply_ptr->class != MONK && ply_ptr->class < BUILDER) {
		output(fd, "Only monks may use that skill.\n");
		return(0);
	}
	if(ply_ptr->level < 10) {
		output(fd, "You are not experienced enough to do that.\n");
		return(0);
	}
	if(ply_ptr->ready[WIELD-1]){
		output(fd, "How can you do that with your hands full?\n");
		return(0);
	}
	crt_ptr = find_crt_in_rom(ply_ptr, rom_ptr, cmnd->str[1], 
		cmnd->val[1], MON_FIRST);

	if(!crt_ptr) {
		output(fd, "That is not here.\n");
		return(0);
	}


	if ( crt_ptr->type == PLAYER )
	{
		if ( !pkill_allowed(ply_ptr, crt_ptr) )
		{
			return(0);
		}
	}

    if(F_ISSET(ply_ptr, PBLIND)) {
		output(fd, "How do you do that?  You're blind.\n");
        return(0);
    }

	
	if(F_ISSET(crt_ptr, MUNDED) && crt_ptr->type == MONSTER) {
		output(fd, "That wont work on the undead.\n");
		add_enm_crt(ply_ptr->name, crt_ptr);		
		return(0);
	}

	if(F_ISSET(ply_ptr, PINVIS)) {
		F_CLR(ply_ptr, PINVIS);
		output(fd, "Your invisibility fades.\n");
		broadcast_rom(fd, ply_ptr->rom_num, "%M fades into view.",
			      m1arg(ply_ptr));
	}

	t = time(0);
	i = LT(ply_ptr, LT_TOUCH);

	if(t < i) {
		please_wait(fd, i-t);
		return(0);
	}

	if(!is_crt_killable(crt_ptr, ply_ptr)) 
	{
		return(0);
	}
	
	if(crt_ptr->type == MONSTER)
		add_enm_crt(ply_ptr->name, crt_ptr);

	ply_ptr->lasttime[LT_TOUCH].ltime = t;
	ply_ptr->lasttime[LT_ATTCK].ltime = t;
	ply_ptr->lasttime[LT_TOUCH].interval = 600L;

	chance = (ply_ptr->level - crt_ptr->level)*20+bonus[(int)ply_ptr->constitution]*10;
	chance = MIN(chance, 85);

	if(mrand(1,100) > chance) {
		mprint(fd, "You failed to harm %m.\n", m1arg(crt_ptr));
		broadcast_rom(fd, ply_ptr->rom_num, "%M failed the touch of death on %m.\n",
			      m2args(ply_ptr, crt_ptr));
		return(0);
	}

	if((mrand(1,100) > 80 - bonus[(int)ply_ptr->constitution]) && crt_ptr->type == MONSTER && !F_ISSET(crt_ptr, MPERMT)) {
		mprint(fd, "You fatally wound  %m.\n", m1arg(crt_ptr));
		broadcast_rom(fd, ply_ptr->rom_num, "%M fatally wounds %m.",
			      m2args(ply_ptr, crt_ptr));
		add_enm_dmg(ply_ptr->name, crt_ptr, crt_ptr->hpcur);
		die(crt_ptr, ply_ptr);	
	}

	else {
		dmg = MAX(1, crt_ptr->hpcur / 2);
		m = MIN(crt_ptr->hpcur, dmg);
		crt_ptr->hpcur -= dmg;
		add_enm_dmg(ply_ptr->name, crt_ptr, m);
		sprintf(g_buffer, "You touched %%m for %d damage.\n", dmg);
		mprint(fd, g_buffer, m1arg(crt_ptr));

		if ( crt_ptr == PLAYER )
		{
			mprint(crt_ptr->fd, "%M uses the touch of death on you!", m1arg(ply_ptr) );
			broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, "%M uses the touch of death on %m.", 
				m2args(ply_ptr, crt_ptr));
		}
		else
		{
			broadcast_rom(fd, ply_ptr->rom_num, "%M uses the touch of death on %m.", 
				m2args(ply_ptr, crt_ptr));
		}

		if(crt_ptr->hpcur < 1) {
			mprint(fd, "You killed %m.\n", m1arg(crt_ptr));
			broadcast_rom(fd, ply_ptr->rom_num, "%M killed %m.",
				      m2args(ply_ptr, crt_ptr));
			die(crt_ptr, ply_ptr);
		}
	}

	return(0);

}

int uptime(creature *ply_ptr, cmd *cmnd)
{
	
	int count=0,i;
	time_t t;
	char str[256];

	for(i=0;i<Tablesize;i++) {
		if(!Ply[i].ply) continue;
		if(Ply[i].ply->fd < 0) continue;
		if(Ply[i].ply->class >= BUILDER && F_ISSET(Ply[i].ply, PDMINV)) continue;
		count++;
	}
	t = time(0);
	convert_seconds_to_str(t-StartTime,str);
	sprintf(g_buffer, "Uptime: %s   Players: %d\n", str, count);
	output(ply_ptr->fd, g_buffer);

	return(0);
}
