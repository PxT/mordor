/*
 * GUILD.C:
 *
 *      This files contains miscellaneous guild-related routines
 *
 *      Copyright (C) 2000 Brooke Paul
 *
 * $Id: guild.c,v 6.13 2001/03/08 16:09:09 develop Exp $
 *
 * $Log: guild.c,v $
 * Revision 6.13  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */

#include "../include/mordb.h"
#include "mextern.h"
#include <ctype.h>
#include <string.h>



/*********************************************************************/
/*				init guild 			     */
/*********************************************************************/

// this function sets up the current guilds

int init_guild()
{
	strcpy(cur_guilds[1].name, "Circle of Vengeance");
	cur_guilds[1].good = 0;				// good kills
	cur_guilds[1].lawful = 0;			
	cur_guilds[1].score = 0;
	cur_guilds[1].pkilled = 0;
	cur_guilds[1].pkia = 0;
	cur_guilds[1].hazyroom = 1;

	strcpy(cur_guilds[2].name, "Hex Masters");
	cur_guilds[2].good = 0;				// mp tick
	cur_guilds[2].lawful = 1;			
	cur_guilds[2].score = 0;
	cur_guilds[2].pkilled = 0;
	cur_guilds[2].pkia = 0;
	cur_guilds[2].hazyroom = 1;

	strcpy(cur_guilds[3].name, "Dark Brotherhood");
	cur_guilds[3].good = 0;				// stealing
	cur_guilds[3].lawful = 0;			
	cur_guilds[3].score = 0;
	cur_guilds[3].pkilled = 0;
	cur_guilds[3].pkia = 0;
	cur_guilds[3].hazyroom = 1;

	strcpy(cur_guilds[4].name, "Black Dragon Clan");
	cur_guilds[4].good = 0;				// hp tick
	cur_guilds[4].lawful = 1;			
	cur_guilds[4].score = 0;
	cur_guilds[4].pkilled = 0;
	cur_guilds[4].pkia = 0;
	cur_guilds[5].hazyroom = 1;

	strcpy(cur_guilds[5].name, "Grey Company");
	cur_guilds[5].good = 1;				// selling 
	cur_guilds[5].lawful = 0;			
	cur_guilds[5].score = 0;
	cur_guilds[5].pkilled = 0;
	cur_guilds[5].pkia = 0;
	cur_guilds[5].hazyroom = 1;

	strcpy(cur_guilds[6].name, "Royal Guard");
	cur_guilds[6].good = 1;				// dmg bonus
	cur_guilds[6].lawful = 1;			
	cur_guilds[6].score = 0;
	cur_guilds[6].pkilled = 0;
	cur_guilds[6].pkia = 0;
	cur_guilds[6].hazyroom = 1;

	strcpy(cur_guilds[7].name, "Champions of the Red Blade");
	cur_guilds[7].good = 1;				// evil kills
	cur_guilds[7].lawful = 0;			 
	cur_guilds[7].score = 0;
	cur_guilds[7].pkilled = 0;
	cur_guilds[7].pkia = 0;
	cur_guilds[7].hazyroom = 1;

	strcpy(cur_guilds[8].name, "White Council");
	cur_guilds[8].good = 1;				// vigging exp
	cur_guilds[8].lawful = 1;			
	cur_guilds[8].score = 0;
	cur_guilds[8].pkilled = 0;
	cur_guilds[8].pkia = 0;
	cur_guilds[8].hazyroom = 1;

	return(0);
}
	
/**********************************************************************/
/*                              check_guild                           */
/**********************************************************************/

// This function return a number corressponding to one of the 8 guilds
// 0 No guild
// 1 Circle of Vengeance
// 2 Hex Masters
// 3 Dark Brotherhood
// 4 Black Dragon Clan
// 5 Grey Company
// 6 Royal Guard
// 7 Champions of the Red Blade
// 8 White Council 
 
int check_guild(creature * ply_ptr)
{       
        if(!F_ISSET(ply_ptr, PPLDGK)) 
                return(0); // No guild
                        
        else if(F_ISSET(ply_ptr, PGILDA)) {
                if(F_ISSET(ply_ptr, PGILDB)) {
                        if(F_ISSET(ply_ptr, PGILDC))
                                return(7); // guild 7 Champions of Red Blade
                        else    
                                return(4); // guild 4 Black Dragon Clan
                }

                else {
                        if(F_ISSET(ply_ptr, PGILDC))
                                return(5); // guild 5 Grey Company
                        else  
                                return(1); // guild 1 Circle of Vengeance
                }
        }
 
        else
        {
                if(F_ISSET(ply_ptr, PGILDB)) {
                        if(F_ISSET(ply_ptr, PGILDC))
                                return(6); // guild 6 Royal Guard
                        else
                                return(2); // guild 2 Hex Masters
                }

                else
                {
                        if(F_ISSET(ply_ptr, PGILDC))
                                return(3); // guild 3 Dark Brotherhood
                        else
                                return(8); // guild 8 White Council
                } 
        }               
/* This should never happen */
return(-1);
}
                
/**********************************************************************/
/*                       monstercheck_guild                           */
/**********************************************************************/

// This function return a number corressponding to one of the 8 guilds
// 0 No guild
// 1 Circle of Vengeance
// 2 Hex Masters
// 3 Dark Brotherhood 
// 4 Black Dragon Clan
// 5 Grey Company
// 6 Royal Guard
// 7 Champions of the Red Blade
// 8 White Council

int monstercheck_guild(creature * ply_ptr)
{
        if(!F_ISSET(ply_ptr, MPLDGK)) 
                return(0); // No guild
        
        else if(F_ISSET(ply_ptr, MGILDA)) {
                if(F_ISSET(ply_ptr, MGILDB)) {
                        if(F_ISSET(ply_ptr, MGILDC))
                                return(7); // guild 7 Champions of Red Blade
                        else
                                return(4); // guild 4 Black Dragon Clan
                }
                
                else {
                        if(F_ISSET(ply_ptr, MGILDC))
                                return(5); // guild 5 Grey Company
                        else
                                return(1); // guild 1 Dancers of Death
                }
        }       
 
        else {       
                if(F_ISSET(ply_ptr, MGILDB)) {
                        if(F_ISSET(ply_ptr, MGILDC))
                                return(6); // guild 6 Royal Guard
                        else
                                return(2); // guild 2 Hex Masters
                }
 
                else {
                        if(F_ISSET(ply_ptr, MGILDC))
                                return(3); // guild 3 Dark Brotherhood
                        else
                                return(8); // guild 8 White Council
                } 
        }
/* This should never happen */
return(-1);
}        
/**********************************************************************/
/*                       objectcheck_guild                           */
/**********************************************************************/

// This function return a number corressponding to one of the 8 guilds
// 0 No guild
// 1 Dancers of Death
// 2 Hex Masters
// 3 Dark Brotherhood 
// 4 Black Dragon Clan
// 5 Grey Company
// 6 Royal Guard
// 7 Champions of the Red Blade
// 8 White Council

int objectcheck_guild(object * ply_ptr)
{
        if(!F_ISSET(ply_ptr,OPLDGK)) 
                return(0); //No guild
        
        else if(F_ISSET(ply_ptr, OGILDA)) {
                if(F_ISSET(ply_ptr, OGILDB)) {
                        if(F_ISSET(ply_ptr, OGILDC))
                                return(7); // guild 7 Champions of Red Blade
                        else
                                return(4); // guild 4 Black Dragon Clan
                }
                
                else {
                        if(F_ISSET(ply_ptr, OGILDC))
                                return(5); // guild 5 Grey Company
                        else
                                return(1); // guild 1 Circle of Vengeance
                }
        }       
 
        else {       
                if(F_ISSET(ply_ptr, OGILDB)) {
                        if(F_ISSET(ply_ptr, OGILDC))
                                return(6); // guild 6 Royal Guard
                        else
                                return(2); // guild 2 Hex Masters
                }
 
                else {
                        if(F_ISSET(ply_ptr, OGILDC))
                                return(3); // guild 3 Dark Brotherhood
                        else
                                return(8); // guild 8 white council
                } 
        }

/* This should never happen */
return(-1);
}        
/**********************************************************************/
/*                       roomcheck_guild                           */
/**********************************************************************/

// This function return a number corressponding to one of the 8 guilds
// 0 No guild
// 1 Circle of Vengeance
// 2 Hex Masters
// 3 Dark Brotherhood 
// 4 Black Dragon Clan
// 5 Grey Company
// 6 Royal Guard
// 7 Champions of the Red Blade
// 8 White Council

int roomcheck_guild(room * ply_ptr)
{
        if(!F_ISSET(ply_ptr, RPLDGK)) 
                return(0); // No guild
        
        else if(F_ISSET(ply_ptr, RGILDA)) {
                if(F_ISSET(ply_ptr, RGILDB)) {
                        if(F_ISSET(ply_ptr, RGILDC))
                                return(7); // guild 7 Champions of Red Blade
                        else
                                return(4); // guild 4 Black Dragon Clan
                }
                
                else {
                        if(F_ISSET(ply_ptr, RGILDC))
                                return(5); // guild 5 Grey Company
                        else
                                return(1); // guild 1 Circle of Vengeance
                }
        }       
 
        else {       
                if(F_ISSET(ply_ptr, RGILDB)) {
                        if(F_ISSET(ply_ptr, RGILDC))
                                return(6); // guild 6 Royal Guard
                        else
                                return(2); // guild 2 Hex Masters
                }
 
                else {
                        if(F_ISSET(ply_ptr, RGILDC))
                                return(3); // guild 3 Dark Brotherhood
                        else
                                return(8); // guild 8 White Council
                } 
        }
/* This should never happen */
return(-1);
}        
/**********************************************************************/
/*                       exitcheck_guild                           */
/**********************************************************************/

// This function return a number corressponding to one of the 8 guilds
// 0 No guild
// 1 Circle of Vengeance
// 2 Hex Masters
// 3 Dark Brotherhood 
// 4 Black Dragon Clan
// 5 Grey Company
// 6 Royal Guard
// 7 Champions of the Red Blade
// 8 White Council

int exitcheck_guild(exit_ * ext_ptr)
{
        if(!F_ISSET(ext_ptr, XPLDGK)) 
                return(0); // No guild
        
        else if(F_ISSET(ext_ptr, XGILDA)) {
                if(F_ISSET(ext_ptr, XGILDB)) {
                        if(F_ISSET(ext_ptr, XGILDC))
                                return(7); // guild 7 Champions of Red Blade
                        else
                                return(4); // guild 4 Black Dragon Clan
                }
                
                else {
                        if(F_ISSET(ext_ptr, XGILDC))
                                return(5); // guild 5 Grey Company
                        else
                                return(1); // guild 1 Circle of Vengeance
                }
        }       
 
        else {       
                if(F_ISSET(ext_ptr, XGILDB)) {
                        if(F_ISSET(ext_ptr, XGILDC))
                                return(6); // guild 6 Royal Guard
                        else
                                return(2); // guild 2 Hex Masters
                }
 
                else {
                        if(F_ISSET(ext_ptr, XGILDC))
                                return(3); // guild 3 Dark Brotherhood
                        else
                                return(8); // guild 8 White Council
                } 
        }
/* This should never happen */
return(-1);
}        

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                              Pledge                                  */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* The pledge command allows a player to pledge allegiance to a given  *
 * monster. A pledge players allows the player to use selected items,  *
 * and exits, as well a being able to kill players of the opposing     *
 * kingdoms for experience (regardless if one player is lawful.)  In   *
 * order for a player to pledge, the player needs to be in the correct *
 * room with a correct monster to pledge to. */
         
int pledge(creature *ply_ptr, cmd *cmnd)
{
        creature        *crt_ptr;
        room            *rom_ptr;
        int             fd, amte, amtg, guildchoice;
         
        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
                        
        amte = REWARD;
        amtg = REWARD *5;
                
        if(cmnd->num < 2) {
                output(fd, "Join whom's organization?\n");
                return(0);
        }
         
        if(F_ISSET(ply_ptr, PPLDGK)) {
            output(fd, "You have already joined a guild.\n");
            return(0);
        }
                        
        if(!F_ISSET(rom_ptr, RRSCND)) {
            output(fd, "You can not join here.\n");
            return(0);
   	}
        
        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[1], cmnd->val[1]);
        
        if(!crt_ptr) {       
                output(fd, "I don't see that here.\n");
                return(0);
        }
                
         
        if (!F_ISSET(crt_ptr,MPLDGK)) {
            mprint(fd, "%M has no idea what you are talking about!\n", m1arg(crt_ptr));
            return(0);
        }
         
        guildchoice = monstercheck_guild(crt_ptr);

        if(ply_ptr->level < 7) {       
                mprint(fd, "You are not experienced enough to join %m's guild.\n", m1arg(crt_ptr));
                return(0);
        }
                        
         
        if(ISENGARD) {

		/* Dark Brotherhood */
                if(guildchoice == 3  && (ply_ptr->class != THIEF)) {
                        mprint(fd, "Your class prevents you from joining %m's guild.\n", m1arg(crt_ptr));
                        return(0);
                }
         
                if(guildchoice != 3 && (ply_ptr->class == THIEF)) {
			mprint(fd, "%M's guild does not associate with thieves.\n", m1arg(crt_ptr));
			return 0;
		}

		if(guildchoice > 2 && (ply_ptr->class == ASSASSIN)) {
                        mprint(fd, "Your class prevents you from joining %m's guild.\n", m1arg(crt_ptr));
                        return(0);
                }
                        
                if(guildchoice < 6 && (ply_ptr->class == PALADIN)) {   
                        mprint(fd, "Your class prevents you from joining %m's guild.\n", m1arg(crt_ptr));
           		return(0);
                }
/*
	*** Beginning of alignment code for pledging ***

		if(!F_ISSET(ply_ptr, PCHAOS)) {
	                if(guildchoice == 1 || guildchoice == 3 ||
			   guildchoice == 5 || guildchoice == 7) {   
                        	mprint(fd, "Your alignment prevents you from joining %m's %m.\n", m2args(crt_ptr, cur_guilds[guildchoice].name));
           			return(0);
                	}
		}
	        else {
	                if(guildchoice == 2 || guildchoice == 4 ||
			   guildchoice == 6 || guildchoice == 8) {   
                        	mprint(fd, "Your alignment prevents you from joining %m's %m.\n", m2args(crt_ptr, cur_guilds[guildchoice].name));
           			return(0);
                	}
		}
	*** End of alignment code for pledging ***
*/
                 
        } /* ISENGARD */
                        
        sprintf(g_buffer, "%%M pledges %s allegiance to %%m.", F_ISSET(crt_ptr, PMALES) ? "his":"her");
        broadcast_rom(fd, ply_ptr->rom_num, g_buffer, m2args(ply_ptr, crt_ptr));
        mprint(fd, "You kneel before %m as you join the organization.\n", m1arg(crt_ptr));

/*
	*** Begin old pledge output ***

	print(fd, "%M draws %s sword and dubs you a member of %s organization.\n", crt_ptr,
	F_ISSET(crt_ptr, PMALES) ? "his":"her",
	F_ISSET(crt_ptr, PMALES) ? "his":"her");

	*** End old pledge output ***
*/
                 
        mprint(fd,"The room erupts in cheers for the newest member of %m's organization.\n", m1arg(crt_ptr));
                
        sprintf(g_buffer, "You gain %d experience and %d gold!\n",REWARD, REWARD*5);
        output(fd, g_buffer);

        ply_ptr->experience += amte;
        add_prof(ply_ptr,amte);
        ply_ptr->gold += amtg;
        ply_ptr->hpcur = ply_ptr->hpmax;
        ply_ptr->mpcur = ply_ptr->mpmax;
                
        F_CLR(ply_ptr, PHIDDN);
        F_SET(ply_ptr,PPLDGK);
         
        if(F_ISSET(crt_ptr,MGILDA))
                F_SET(ply_ptr,PGILDA);
  
        if(F_ISSET(crt_ptr,MGILDB))
                F_SET(ply_ptr,PGILDB);
        
        if(F_ISSET(crt_ptr,MGILDC))
                F_SET(ply_ptr,PGILDC);
return(0);          
        
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                              rescind                                 */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* The rescind command allows a player to rescind allegiance to a given *
 * monster.  Once a player has rescinded his allegiance, he or she will *
 * lose all the privilliages of rescinded kingdom as well as a          *
 * specified amount of experience and gold.				*/

int rescind( creature *ply_ptr, cmd *cmnd )
{
        creature        *crt_ptr;
        room            *rom_ptr;
        int             fd, amte, amtg, guildmonster, guildplayer;
                
        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
        amte = REWARD*10;
        amtg = REWARD*20;
  
        if(cmnd->num < 2) {
                output(fd, "Rescind to whom?\n");
                return(0);
        }

        if(!F_ISSET(ply_ptr,PPLDGK)){
            output(fd, "You are not a member of a guild.\n");
            return(0);
        }
 
        if(!F_ISSET(rom_ptr,RRSCND)) {
            output(fd, "You can not rescind your guild allegiance here.\n");
            return(0);
        }
        
        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                           cmnd->str[1], cmnd->val[1]);
        
        if(!crt_ptr) {
        output(fd, "I don't see that here.\n");
                return(0); 
        }
                
        guildmonster = monstercheck_guild(crt_ptr);
        guildplayer =  check_guild(ply_ptr);

        if(guildplayer != guildmonster) {
            output(fd, "You are in the wrong guild to rescind!\n");
            return(0);
        }
            
        if (ply_ptr->gold < amtg){
            output(fd, "You do not have sufficient gold to rescind.\n");
            return (0);
        }
        
        if (!F_ISSET(crt_ptr,MRSCND)) {
   		mprint(fd,"You can not rescind your allegiance to %m.\n", m1arg(crt_ptr));
            return(0);
        }       
        
        sprintf(g_buffer, "%%M rescinds %s allegiance to %%m.",
                  F_ISSET(crt_ptr, PMALES) ? "his":"her");
        broadcast_rom(fd, ply_ptr->rom_num, g_buffer,
                  m2args(ply_ptr, crt_ptr));
        mprint(fd, "%M revokes all your rights and privileges!\n",
                        m1arg(crt_ptr));
        mprint(fd, "\nThe room fills with boos and hisses as you are ostracized from %m's organization.\n", m1arg(crt_ptr));
         
        amte = MIN(amte,ply_ptr->experience);
        sprintf(g_buffer, "\nYou lose %d experience and %d gold!\n",amte,amtg);
        output(fd, g_buffer);
        lower_prof(ply_ptr,amte);
        ply_ptr->experience -= amte;
        ply_ptr->gold -= amtg;
        ply_ptr->hpcur = ply_ptr->hpmax/3;
        ply_ptr->mpcur = 0;
          
        F_CLR(ply_ptr,PHIDDN);
        F_CLR(ply_ptr,PPLDGK);
        F_CLR(ply_ptr,PGILDA);
        F_CLR(ply_ptr,PGILDB);
        F_CLR(ply_ptr,PGILDC);
        
                  
    return(0);
                        
}

/**********************************************************************/
/*                      guild_send                                    */
/**********************************************************************/
 
/* This function allows guild to send messages that only they can see. */
/* It is similar to a broadcast, but there are no limits.            */
                
int guild_send( creature *ply_ptr, cmd *cmnd )
{
        int     fd, found = 0;
        unsigned int i;
         
        fd = ply_ptr->fd;
        
        for(i=0; i<strlen(cmnd->fullstr); i++) {
                if(cmnd->fullstr[i] == ' ' && cmnd->fullstr[i+1] != ' ') {
                        found++; 
                        break;
                }
        }
                
        if(found < 1 || strlen(&cmnd->fullstr[i+1]) < 1) {
                output(fd, "Send what?\n");
                return(0);
        }

        if(F_ISSET(ply_ptr, PSILNC)){
                output_wc(fd, "You are unable to do that right now.\n", SILENCECOLOR);
                return(0);
        }

        if(!F_ISSET(ply_ptr, PPLDGK)){
                output(fd, "You are not in a guild!\n");
                return(0);
        }

        /* spam check */
        if ( check_for_spam( ply_ptr ) ) {
                return(0);
        }
         
        output(fd, "Ok.\n");
        
        sprintf(g_buffer, "%s sent, \"%s\".",
		ply_ptr->name, &cmnd->fullstr[i+1]);
        broadcast_guild(ply_ptr, g_buffer);
 
       if (ply_ptr->class == DM)
                return(0);
         
        sprintf(g_buffer, "--- %s class sent, \"%s\".",
		ply_ptr->name, &cmnd->fullstr[i+1]);
        broadcast_eaves(g_buffer);
                
        return(0);
         
}

/**********************************************************************/
/*                             guild_list                             */
/**********************************************************************/
                
/* This function outputs a list of all the players who are currently */
/* logged into the game.  It includes their titles and the last      */
/* command they typed.                           */
            
int guilds_list( creature *ply_ptr, cmd *cmnd )
{
    int fd, i;
                        
    fd = ply_ptr->fd;
    
	if(F_ISSET(ply_ptr, PBLIND)) {
	        output_wc(fd, "You're blind!\n", BLINDCOLOR);
		return(0);
        }


	ANSI (fd, AM_BOLD);
	ANSI (fd, AFC_BLUE);
	sprintf(g_buffer, "%-30s  %-7s %-5s %-5s %-5s %-5s\n",
		"Guild", "Align", "Kills", " Kia ", "PKscore", "Score");
	output(fd, g_buffer );
	output(fd, "-----------------------------------------------------------------------\n");
	ANSI(fd, AM_NORMAL);
	ANSI(fd, AFC_WHITE);
	for(i=1; i<9; i++) {
		ANSI(fd, AFC_WHITE);

		sprintf(g_buffer, "%-30s  ", cur_guilds[i].name);
		output(fd, g_buffer);
                 
		ANSI(fd, AFC_GREEN);
		if (cur_guilds[i].lawful == 1 || cur_guilds[i].lawful == 3)
			sprintf(g_buffer, "Law       ");
                else
			sprintf(g_buffer, "Chaos     ");
		output(fd, g_buffer);
		ANSI(fd, AFC_CYAN);
		sprintf(g_buffer, "%-5d  ", cur_guilds[i].pkilled);
		output(fd, g_buffer);
		sprintf(g_buffer, "%-5d  ", cur_guilds[i].pkia);
		output(fd, g_buffer);
		sprintf(g_buffer, "%-5d  ", (int) cur_guilds[i].pkilled - cur_guilds[i].pkia);
		output(fd, g_buffer);
		sprintf(g_buffer, "%-5d\n", (int) cur_guilds[i].score);
		output(fd, g_buffer);
		ANSI(fd, AFC_WHITE);
	}            
	output(fd, "\n");
	return(0);
}

/**********************************************************************/
/*                              guild_score                           */
/**********************************************************************/
        
// This function compares guild good and lawful rating to get a relative score
        
int guild_score(creature * ply_ptr, creature * crt_ptr)
{

	int score;

	if (crt_ptr->type == PLAYER) {	
		if (cur_guilds[check_guild(ply_ptr)].lawful
		  > cur_guilds[check_guild(crt_ptr)].lawful)
			score = cur_guilds[check_guild(ply_ptr)].lawful
			      - cur_guilds[check_guild(crt_ptr)].lawful;
		else
			score = cur_guilds[check_guild(crt_ptr)].lawful
		 	      - cur_guilds[check_guild(ply_ptr)].lawful;

		if (cur_guilds[check_guild(ply_ptr)].good
		  > cur_guilds[check_guild(crt_ptr)].good)
			score += cur_guilds[check_guild(ply_ptr)].good
			       - cur_guilds[check_guild(crt_ptr)].good;
		else
			score += cur_guilds[check_guild(crt_ptr)].good
			       - cur_guilds[check_guild(ply_ptr)].good;
	}
	else if (crt_ptr->type == MONSTER) {	
		if (cur_guilds[check_guild(ply_ptr)].lawful
		  > cur_guilds[monstercheck_guild(crt_ptr)].lawful)
			score = cur_guilds[check_guild(ply_ptr)].lawful
			      - cur_guilds[monstercheck_guild(crt_ptr)].lawful;
		else
			score = cur_guilds[monstercheck_guild(crt_ptr)].lawful
			      - cur_guilds[check_guild(ply_ptr)].lawful;

		if (cur_guilds[check_guild(ply_ptr)].good
		  > cur_guilds[monstercheck_guild(crt_ptr)].good)
			score += cur_guilds[check_guild(ply_ptr)].good
			       - cur_guilds[monstercheck_guild(crt_ptr)].good;
		else
			score += cur_guilds[monstercheck_guild(crt_ptr)].good
			       - cur_guilds[check_guild(ply_ptr)].good;
	}

	if(score < 0)
		score = score * (-1);
	if(score > 1)
		return(2);
	if(score == 1)
		return(1);
	if(score < 1)
		return(0);


	return(0);  /* this should never happen */
}

/**********************************************************************/
/*                             guild_war                             */
/**********************************************************************/
                
/* This function allows a DM to initiate a guild war in a given         */ 
/* number of minutes.                                                   */
        
int guild_war(creature *ply_ptr, cmd *cmnd )
{
	int i, j;

        if(ply_ptr->class <= CARETAKER )
                return(PROMPT);
                        
        output(ply_ptr->fd, "Ok.\n");
	
	j = cmnd->val[0];
	if (j < 1)  i = 1;
	if (j > 30) i = 30;

        sprintf(g_buffer, "### A %d-minute Guild war has now begun.\n", j );
                broadcast(g_buffer);
                 
        /* book it Danno */
        log_dm_command( ply_ptr->name, cmnd->fullstr);
                        
 	for(i=1; i<9; i++) {
		cur_guilds[i].pkilled = 0;
		cur_guilds[i].pkia = 0;
	}

        Guildwar.ltime = time(0);
        Guildwar.interval = cmnd->val[0] * 60 + 1;
             
        return(0);
                        
}

int pk_exp(creature *ply_ptr, creature *crt_ptr)
{

float exp;


        if ((cur_guilds[check_guild(ply_ptr)].lawful
               == cur_guilds[check_guild(crt_ptr)].lawful) &&
                 (cur_guilds[check_guild(ply_ptr)].good
               == cur_guilds[check_guild(crt_ptr)].good)) {
                output(ply_ptr->fd,
                        "You recieve 0 experience for slaying your allies.\n");
                return 0;
        }

        exp = crt_ptr->level * 10;
        if (exp > 250) exp = 250;
        return(exp);                                                           
}

int offer(creature *ply_ptr, cmd *cmnd)
{

creature 	*crt_ptr;
object 		*obj_ptr;
room 		*rom_ptr;
int 		fd = ply_ptr->fd;
int 		exp;

        rom_ptr = ply_ptr->parent_rom;

	if(check_guild(ply_ptr) != 3) {
		output(fd, "Only members of the Dark Brotherhood can offer stolen items.\n");
		return 0;
	}

        if(cmnd->num < 2) {
                output(fd, "Offer what?\n");
                return 0;
	} 
        
	obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj, cmnd->str[1],
			   cmnd->val[1]);
                
        if(!obj_ptr) {
        	output(fd, "You don't have that item.\n");
        	return(0);
      	}

	if(cmnd->num < 3) {
		output(fd, "Offer that to whom?\n");
		return 0;
	}

	crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[2],
			   cmnd->val[2]);

        if(!crt_ptr) {
        	output(fd, "That person isn't here.\n");
                return(0); 
        }

	
	if(monstercheck_guild(crt_ptr) != 3) {
		output(fd, "You can only offer stolen items to a guildmaster of the Dark Brotherhood.\n");
		return 0;
	}

	if(!F_ISSET(obj_ptr, OSTOLE)) {
		sprintf(g_buffer, "%s says \"Your %s was not stolen from an adequate adversary.\"\n",
			crt_ptr->name, obj_ptr->name);
		output(fd, g_buffer);
		return 0;
	}

	
	if(obj_ptr->shotsmax < 1) {
		exp = (obj_ptr->value * 0.05);
		
		if(exp > 500)
                	exp = 500;
        	else if(exp < 5)
                	exp = 5;
	}

	else {
		if(obj_ptr->shotscur == 0) {
			mprint(fd, "%M says \"I will award you nothing for offering broken items.\"\n",
				m1arg(crt_ptr));
			return 0;
		}

		else {
			exp = ((obj_ptr->value * 0.05) * (obj_ptr->shotscur /
				obj_ptr->shotsmax));
			if(exp > 500)
				exp = 500;
			else if(exp < 5)
				exp = 5;
		}
	}

	mprint(fd, "%M says \"You have done well!\"\n", m1arg(crt_ptr));
	sprintf(g_buffer,
		"You recieve %d experience for your clever thievery.\n", exp);
	output(fd, g_buffer);	

	ply_ptr->experience += exp;
	broadcast_rom(fd, ply_ptr->rom_num,
		     "%M offers %m %i that he stole from his enemies.\n", 
		     m3args(ply_ptr, crt_ptr, obj_ptr));
 	del_obj_crt(obj_ptr, ply_ptr);
	
	return 0;
}

