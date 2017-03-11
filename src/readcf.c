/*
 *  Read in configuration file mordor.cf
 *	Copyright 1996 Brooke Paul
 * 
 * $Id: readcf.c,v 1.1 2001/07/29 22:11:02 develop Exp $
 *
 * $Log: readcf.c,v $
 * Revision 1.1  2001/07/29 22:11:02  develop
 * Initial revision
 *
 * Revision 6.15  2001/04/23 03:52:29  develop
 * added NOCREATE flag to toggle character creation
 *
 * Revision 6.14  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */


#include "../include/mordb.h"
#include "mextern.h"

#include <string.h>


/********************************************************************/
/*		READCF						    */
/*	Reads the configuration file and sets variables accordingly.*/
/********************************************************************/

int readcf(void)

{
	char	filename[256];
	char	szBuffer[256];

	get_config_file( filename );


	if ( !bHavePort )
	{
		/* game settings */
		GetIniString( "Game Settings", "PortNum", "4040", szBuffer, 256, filename);
		PORTNUM = (unsigned short)atoi( szBuffer );
	}

	GetIniString( "Game Settings", "AnsiLine", "0", szBuffer, 256, filename);
	ANSILINE = convert_to_bool( szBuffer );

	GetIniString( "Game Settings", "AutoShutdown", "0", szBuffer, 256, filename);
	AUTOSHUTDOWN = convert_to_bool( szBuffer );

	GetIniString( "Game Settings", "CheckDouble", "0", szBuffer, 256, filename);
	CHECKDOUBLE = convert_to_bool( szBuffer );

	GetIniString( "Game Settings", "CrashTrap", "0", szBuffer, 256, filename);
	CRASHTRAP = convert_to_bool( szBuffer );

	GetIniString( "Game Settings", "EatNDrink", "0", szBuffer, 256, filename);
	EATNDRINK = convert_to_bool( szBuffer );

	GetIniString( "Game Settings", "GetHostByName", "0", szBuffer, 256, filename);
	GETHOSTBYNAME = convert_to_bool( szBuffer );


	GetIniString( "Game Settings", "Heaven", "0", szBuffer, 256, filename);
	HEAVEN = convert_to_bool( szBuffer );

	GetIniString( "Game Settings", "Isengard", "0", szBuffer, 256, filename);
	ISENGARD = convert_to_bool( szBuffer );

	GetIniString( "Game Settings", "NiceExp", "0", szBuffer, 256, filename);
	GUILDEXP = convert_to_bool( szBuffer );

	GetIniString( "Game Settings", "Paranoid", "0", szBuffer, 256, filename);
	PARANOID = convert_to_bool( szBuffer );

	GetIniString( "Game Settings", "Record_All", "0", szBuffer, 256, filename);
	RECORD_ALL = convert_to_bool( szBuffer );

	GetIniString( "Game Settings", "RFC1413", "0", szBuffer, 256, filename);
	RFC1413 = convert_to_bool( szBuffer );

	GetIniString( "Game Settings", "Sched", "0", szBuffer, 256, filename);
	SCHED = convert_to_bool( szBuffer );

	GetIniString( "Game Settings", "Secure", "0", szBuffer, 256, filename);
	SECURE = convert_to_bool( szBuffer );

	GetIniString( "Game Settings", "Suicide", "0", szBuffer, 256, filename);
	SUICIDE = convert_to_bool( szBuffer );

	GetIniString( "Game Settings", "SaveOnDrop", "0", szBuffer, 256, filename);
	SAVEONDROP = convert_to_bool( szBuffer );

	GetIniString( "Game Settings", "NoBullys", "0", szBuffer, 256, filename);
	NOBULLYS = convert_to_bool( szBuffer );

	GetIniString( "Game Settings", "LastCommand", "0", szBuffer, 256, filename);
	LASTCOMMAND = convert_to_bool( szBuffer );

	GetIniString( "Game Settings", "NoCreate", "0", szBuffer, 256, filename);
        NOCREATE = convert_to_bool( szBuffer );

	GetIniString( "Game Settings", "MudSound", "0", szBuffer, 256, filename);
	MSP = convert_to_bool( szBuffer );

	GetIniString( "Game Settings", "StartRoom", "1", szBuffer, 256, filename);
	start_room_num = atoi( szBuffer );

	GetIniString( "Game Settings", "ShopCost", "1000000", szBuffer, 256, filename);

	shop_cost = atol( szBuffer );

	GetIniString( "Game Settings", "DeathRoom", "50", szBuffer, 256, filename);
	death_room_num = atoi( szBuffer );
	
	GetIniString( "Game Settings", "JailRoom", "6972", szBuffer, 256, filename);
	jail_room_num = atoi( szBuffer );

	GetIniString( "Game Settings", "GuildRoom1", "1", szBuffer, 256, filename);
	cur_guilds[1].hazyroom = atoi( szBuffer );

	GetIniString( "Game Settings", "GuildRoom2", "1", szBuffer, 256, filename);
	cur_guilds[2].hazyroom = atoi( szBuffer );

	GetIniString( "Game Settings", "GuildRoom3", "1", szBuffer, 256, filename);
	cur_guilds[3].hazyroom = atoi( szBuffer );

	GetIniString( "Game Settings", "GuildRoom4", "1", szBuffer, 256, filename);
	cur_guilds[4].hazyroom = atoi( szBuffer );

	GetIniString( "Game Settings", "GuildRoom5", "1", szBuffer, 256, filename);
	cur_guilds[5].hazyroom = atoi( szBuffer );

	GetIniString( "Game Settings", "GuildRoom6", "1", szBuffer, 256, filename);
	cur_guilds[6].hazyroom = atoi( szBuffer );

	GetIniString( "Game Settings", "GuildRoom7", "1", szBuffer, 256, filename);
	cur_guilds[7].hazyroom = atoi( szBuffer );

	GetIniString( "Game Settings", "GuildRoom8", "1", szBuffer, 256, filename);
	cur_guilds[8].hazyroom = atoi( szBuffer );

	/* timeouts */
	GetIniString( "Timeout", "DM", "0", szBuffer, 256, filename);
	dm_timeout = atoi( szBuffer );

	GetIniString( "Timeout", "Caretaker", "1200", szBuffer, 256, filename);
	ct_timeout = atoi( szBuffer );

	GetIniString( "Timeout", "Builder", "1200", szBuffer, 256, filename);
	bld_timeout = atoi( szBuffer );

	GetIniString( "Timeout", "Player", "300", szBuffer, 256, filename);
	ply_timeout = atoi( szBuffer );

	/* text string */
	GetIniString( "Text Strings", "Title", "Mordor MUD Server", title, 80, filename);
	GetIniString( "Text Strings", "Authorization_questions_email", "auth@I.NEED.SET.THIS", auth_questions_email, 80, filename);
	GetIniString( "Text Strings", "Register_questions_email", "auth@I.NEED.SET.THIS", register_questions_email, 80, filename);
	GetIniString( "Text Strings", "Questions_to_email", "auth@I.NEED.SET.THIS", questions_to_email, 80, filename);
	GetIniString( "Text Strings", "Account_exists", "Your account already exists here", account_exists, 80, filename);
	GetIniString( "Game Settings", "DM_Password", "", dm_pass, 20, filename);


	/* DM names */
	GetIniString( "DM's Names", "dmname", "", dmname[0], 20, filename);
	GetIniString( "DM's Names", "dmname2", "", dmname[1], 20, filename);
	GetIniString( "DM's Names", "dmname3", "", dmname[2], 20, filename);
	GetIniString( "DM's Names", "dmname4", "", dmname[3], 20, filename);
	GetIniString( "DM's Names", "dmname5", "", dmname[4], 20, filename);
	GetIniString( "DM's Names", "dmname6", "", dmname[5], 20, filename);
	GetIniString( "DM's Names", "dmname7", "", dmname[6], 20, filename);


	/* Timed exit messages */
	GetIniString( "Timed Exit Settings", "tx_mesg1", "", tx_mesg1, 80, filename);
	GetIniString( "Timed Exit Settings", "tx_mesg2", "", tx_mesg2, 80, filename);


	/* weather */
	GetIniString( "Weather", "Sunrise", sunrise, sunrise, 80, filename);
	GetIniString( "Weather", "Sunsets", sunset, sunset, 80, filename);
	GetIniString( "Weather", "Earth_Trembles", earth_trembles, earth_trembles, 80, filename);
	GetIniString( "Weather", "Heavy_Fog", heavy_fog, heavy_fog, 80, filename);
	GetIniString( "Weather", "Beautiful_Day", beautiful_day, beautiful_day, 80, filename);
	GetIniString( "Weather", "Bright_Sun", bright_sun, bright_sun, 80, filename);
	GetIniString( "Weather", "Glaring_Sun", glaring_sun, glaring_sun, 80, filename);
	GetIniString( "Weather", "Heat", heat, heat, 80, filename);
	GetIniString( "Weather", "Still", still, still, 80, filename);
	GetIniString( "Weather", "Light_Breeze", light_breeze, light_breeze, 80, filename);
	GetIniString( "Weather", "String_Wind", strong_wind, strong_wind, 80, filename);
	GetIniString( "Weather", "wind_gusts", wind_gusts, wind_gusts, 80, filename);
	GetIniString( "Weather", "gale_force", gale_force, gale_force, 80, filename);
	GetIniString( "Weather", "clear_skies", clear_skies, clear_skies, 80, filename);
	GetIniString( "Weather", "light_clouds", light_clouds, light_clouds, 80, filename);
	GetIniString( "Weather", "thunderheads", thunderheads, thunderheads, 80, filename);
	GetIniString( "Weather", "light_rain", light_rain, light_rain, 80, filename);
	GetIniString( "Weather", "heavy_rain", heavy_rain, heavy_rain, 80, filename);
	GetIniString( "Weather", "sheets_rain", sheets_rain, sheets_rain, 80, filename);
	GetIniString( "Weather", "torrent_rain", torrent_rain, torrent_rain, 80, filename);
	GetIniString( "Weather", "no_moon", no_moon, no_moon, 80, filename);
	GetIniString( "Weather", "sliver_moon", sliver_moon, sliver_moon, 80, filename);
	GetIniString( "Weather", "half_moon", half_moon, half_moon, 80, filename);
	GetIniString( "Weather", "waxing_moon", waxing_moon, waxing_moon, 80, filename);
	GetIniString( "Weather", "full_moon", full_moon, full_moon, 80, filename);


	/* read the colors */
	GetIniString( "Colors", "Prompt", "magenta", szBuffer, 80, filename);
	PROMPTCOLOR = convert_to_color(szBuffer);

	GetIniString( "Colors", "Creature", "white", szBuffer, 80, filename);
	CREATURECOLOR = convert_to_color(szBuffer);

	GetIniString( "Colors", "Broadcast", "yellow", szBuffer, 80, filename);
	BROADCASTCOLOR = convert_to_color(szBuffer);

	GetIniString( "Colors", "MainText", "white", szBuffer, 80, filename);
	MAINTEXTCOLOR = convert_to_color(szBuffer);

	GetIniString( "Colors", "Player", "cyan", szBuffer, 80, filename);
	PLAYERCOLOR = convert_to_color(szBuffer);

	GetIniString( "Colors", "Item", "white", szBuffer, 80, filename);
	ITEMCOLOR = convert_to_color(szBuffer);

	GetIniString( "Colors", "Exit", "green", szBuffer, 80, filename);
	EXITCOLOR = convert_to_color(szBuffer);

	GetIniString( "Colors", "RoomName", "cyan", szBuffer, 80, filename);
	ROOMNAMECOLOR = convert_to_color(szBuffer);

	GetIniString( "Colors", "Echo", "cyan", szBuffer, 80, filename);
	ECHOCOLOR = convert_to_color(szBuffer);

	GetIniString( "Colors", "Weather", "white", szBuffer, 80, filename);
	WEATHERCOLOR = convert_to_color(szBuffer);

	GetIniString( "Colors", "Attack", "red", szBuffer, 80, filename);
	ATTACKCOLOR = convert_to_color(szBuffer);

	GetIniString( "Colors", "Missed", "cyan", szBuffer, 80, filename);
	MISSEDCOLOR = convert_to_color(szBuffer);

	GetIniString( "Colors", "Trap", "magenta", szBuffer, 80, filename);
	TRAPCOLOR = convert_to_color(szBuffer);

	GetIniString( "Colors", "Poison", "red", szBuffer, 80, filename);
	POISONCOLOR = convert_to_color(szBuffer);

	GetIniString( "Colors", "Disease", "red", szBuffer, 80, filename);
	DISEASECOLOR = convert_to_color(szBuffer);

	GetIniString( "Colors", "Blind", "red", szBuffer, 80, filename);
	BLINDCOLOR = convert_to_color(szBuffer);

	GetIniString( "Colors", "Silence", "yellow", szBuffer, 80, filename);
	SILENCECOLOR = convert_to_color(szBuffer);

	GetIniString( "Colors", "Error", "red", szBuffer, 80, filename);
	ERRORCOLOR = convert_to_color(szBuffer);

	/* not used yet */
	GetIniString( "Colors", "LogOnOff", "white", szBuffer, 80, filename);
	LOGONOFFCOLOR = convert_to_color(szBuffer);

	GetIniString( "Colors", "Title", "white", szBuffer, 80, filename);
	TITLECOLOR = convert_to_color(szBuffer);

	GetIniString( "Colors", "Talk", "white", szBuffer, 80, filename);
	TALKCOLOR = convert_to_color(szBuffer);

	GetIniString( "Colors", "Frost", "yellow", szBuffer, 80, filename);
	FROSTCOLOR = convert_to_color(szBuffer);

	/* Initial eq #'s */
	GetIniString( "Game Settings", "InitEQ1", "1505", szBuffer, 80, filename);
	init_eq[0] = atoi( szBuffer );
	GetIniString( "Game Settings", "InitEQ2", "1502", szBuffer, 80, filename);
	init_eq[1] = atoi( szBuffer );
	GetIniString( "Game Settings", "InitEQ3", "1503", szBuffer, 80, filename);
	init_eq[2] = atoi( szBuffer );
	GetIniString( "Game Settings", "InitEQ4", "1504", szBuffer, 80, filename);
	init_eq[3] = atoi( szBuffer );

	GetIniString( "Game Settings", "InitEQsh", "1", szBuffer, 80, filename);
	init_eq[4] = atoi( szBuffer );
	GetIniString( "Game Settings", "InitEQth", "3", szBuffer, 80, filename);
	init_eq[5] = atoi( szBuffer );
	GetIniString( "Game Settings", "InitEQbl", "2", szBuffer, 80, filename);
	init_eq[6] = atoi( szBuffer );
	GetIniString( "Game Settings", "InitEQpo", "5", szBuffer, 80, filename);
	init_eq[7] = atoi( szBuffer );
	GetIniString( "Game Settings", "InitEQmi", "6", szBuffer, 80, filename);
	init_eq[8] = atoi( szBuffer );

	/* Trophy Item */
	GetIniString( "Game Setting's", "Trophy", "1500", szBuffer, 80, filename);
	trophy_num = atoi( szBuffer );

	/* DO NOT READ PATHS HERE!!! This is done in load_paths() */


	return(0);
}

