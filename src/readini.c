/*
 *  Read in INI file for mordor 3.x
 *  Mordor 3.x
 * 
 */


#include "mstruct.h"
#include "mextern.h"
#ifdef DMALLOC
  #include "/usr/local/include/dmalloc.h"
#endif
#include <string.h>


/****************************************************************/
/*		READINI													*/
/*		       													*/
/*	Reads the INI file and sets variables accordingly.			*/
/****************************************************************/

int readini(void)

{
	char	filename[256];
	char	cmdstr[80];
	char	variable[80];
	char	varval[80];
	int		pos;
	FILE	*fp;

	/* strcpy(BINPATH,"../bin"); */
	sprintf(filename, "%s/mordor.cf", BINPATH);
	fp = fopen(filename,"rt");
  
	if(!fp) {
		loge("Problem with the INI file.\n");
		return(-1);
	}

/* We have to turn them all off to start with,
   so that they reload properly.  --PT */
ANSILINE=0; AUTOSHUTDOWN=0; CRASHTRAP=0; CHECKDOUBLE=0; EATNDRINK=0;
GETHOSTBYNAME=0; HASHROOMS=0; HEAVEN=0; ISENGARD=0; LASTCOMMAND=0; 
NICEEXP=0; PARANOID=0; RECORD_ALL=0; SECURE=0; SCHED=0; SUICIDE=0;
SAVEONDROP=0; 
strcpy(dm_pass, "\0"); 

	fgets(cmdstr,80,fp);
	while(!feof(fp)) {
		if(strstr(cmdstr,"=")) {
			pos=strcspn(cmdstr,"=");
			strcpy(variable,cmdstr);
			variable[pos]=0;
			strcpy(varval, &cmdstr[pos+1]);
			pos=strcspn(varval,"\n");
			varval[pos]=0;
			/* delimit and remove the carriage return */
			lowercize(variable,0);
			if(!strcmp(variable,"dmname")) strcpy(dmname[0],varval);
			if(!strcmp(variable,"dmname2")) strcpy(dmname[1],varval);
			if(!strcmp(variable,"dmname3")) strcpy(dmname[2],varval);
			if(!strcmp(variable,"dmname4")) strcpy(dmname[3],varval);
			if(!strcmp(variable,"dmname5")) strcpy(dmname[4],varval);
			if(!strcmp(variable,"dmname6")) strcpy(dmname[5],varval);
			if(!strcmp(variable,"dmname7")) strcpy(dmname[6],varval);
			if(!strcmp(variable,"portnum")) PORTNUM=atoi(varval);
			/* yes/no on/off settings */

			if(!strcmp(variable,"ansiline")) {
				lowercize(varval);
				if(!strcmp(varval,"yes"))
					ANSILINE=1;
			}
			if(!strcmp(variable,"autoshutdown")) {
				lowercize(varval);
				if(!strcmp(varval,"yes"))
					AUTOSHUTDOWN=1;
			}
			if(!strcmp(variable,"crashtrap")) {
				lowercize(varval);
				if(!strcmp(varval,"yes"))
					CRASHTRAP=1;
			}
			if(!strcmp(variable,"checkdouble")) {
				lowercize(varval);
				if(!strcmp(varval,"yes"))
					CHECKDOUBLE=1;
			}
			if(!strcmp(variable,"eatndrink")) {
				lowercize(varval);
				if(!strcmp(varval,"yes"))
					EATNDRINK=1;
			}
			if(!strcmp(variable,"gethostbyname")) {
				lowercize(varval);
				if(!strcmp(varval,"yes"))
					GETHOSTBYNAME=1;
			}
			if(!strcmp(variable,"hashrooms")) {
				lowercize(varval);
				if(!strcmp(varval,"yes"))
					HASHROOMS=1;
			}
			if(!strcmp(variable,"heaven")) {
				lowercize(varval);
				if(!strcmp(varval,"yes"))
					HEAVEN=1;
			}
			if(!strcmp(variable,"isengard")) {
				lowercize(varval);
				if(!strcmp(varval,"yes"))
					ISENGARD=1;
			}
			if(!strcmp(variable,"lastcommand")) {
				lowercize(varval);
				if(!strcmp(varval,"yes"))
					LASTCOMMAND=1;
			}
			if(!strcmp(variable,"niceexp")) {
				lowercize(varval);
				if(!strcmp(varval,"yes"))
					NICEEXP=1;
			}
			if(!strcmp(variable,"paranoid")) {
				lowercize(varval);
				if(!strcmp(varval,"yes"))
					PARANOID=1;
			}
			if(!strcmp(variable,"record_all")) {
				lowercize(varval);
				if(!strcmp(varval,"yes"))
					RECORD_ALL=1;
			}
			if(!strcmp(variable,"rfc1413")) {
				lowercize(varval);
				if(!strcmp(varval,"yes"))
					RFC1413=1;
			}
			if(!strcmp(variable,"secure")) {
				lowercize(varval);
				if(!strcmp(varval,"yes"))
					SECURE=1;
			}
			if(!strcmp(variable,"sched")) {
				lowercize(varval);
				if(!strcmp(varval,"yes"))
					SCHED=1;
			}
			if(!strcmp(variable,"suicide")) {
				lowercize(varval);
				if(!strcmp(varval,"yes"))
					SUICIDE=1;
			}
			if(!strcmp(variable,"saveondrop")) {
				lowercize(varval);
                                if(!strcmp(varval,"yes"))
					SAVEONDROP=1;
			}
			/* configurable strings */
			if(!strcmp(variable,"title")) 
				strcpy(title,varval);
			if(!strcmp(variable,"authorization_questions_email"))
				strcpy(auth_questions_email,varval);
			if(!strcmp(variable,"questions_to_email"))
				strcpy(questions_to_email,varval);
			if(!strcmp(variable,"register_questions_email"))
				strcpy(register_questions_email,varval);
			if(!strcmp(variable,"account_exists")) 
				strcpy(account_exists,varval);
			if(!strcmp(variable, "dm_password"))
				strcpy(dm_pass,varval);

			/* weather */
			if(!strcmp(variable,"sunrise")) strcpy(sunrise,varval);
			if(!strcmp(variable,"sunset")) strcpy(sunset,varval);
			if(!strcmp(variable,"earth_trembles")) strcpy(earth_trembles,varval);
			if(!strcmp(variable,"heavy_fog")) strcpy(heavy_fog,varval);
			if(!strcmp(variable,"beautiful_day")) strcpy(beautiful_day,varval);
			if(!strcmp(variable,"bright_sun")) strcpy(bright_sun,varval);
			if(!strcmp(variable,"glaring_sun")) strcpy(glaring_sun,varval);
			if(!strcmp(variable,"heat")) strcpy(heat,varval);
			if(!strcmp(variable,"still")) strcpy(still,varval);
			if(!strcmp(variable,"light_breeze")) strcpy(light_breeze,varval);
			if(!strcmp(variable,"strong_wind")) strcpy(strong_wind,varval);
			if(!strcmp(variable,"wind_gusts")) strcpy(wind_gusts,varval);
			if(!strcmp(variable,"gale_force")) strcpy(gale_force,varval);
			if(!strcmp(variable,"clear_skies")) strcpy(clear_skies,varval);
			if(!strcmp(variable,"light_clouds")) strcpy(light_clouds,varval);
			if(!strcmp(variable,"thunderheads")) strcpy(thunderheads,varval);
			if(!strcmp(variable,"light_rain")) strcpy(light_rain,varval);
			if(!strcmp(variable,"heavy_rain")) strcpy(heavy_rain,varval);
			if(!strcmp(variable,"sheets_rain")) strcpy(sheets_rain,varval);
			if(!strcmp(variable,"torrent_rain")) strcpy(torrent_rain,varval);
			if(!strcmp(variable,"no_moon")) strcpy(no_moon,varval);
			if(!strcmp(variable,"sliver_moon")) strcpy(sliver_moon,varval);
			if(!strcmp(variable,"half_moon")) strcpy(half_moon,varval);
			if(!strcmp(variable,"waxing_moon")) strcpy(waxing_moon,varval);
			if(!strcmp(variable,"full_moon")) strcpy(full_moon,varval);


		/* Timed exit messages */
			if(!strcmp(variable,"tx_mesg1")) strcpy(tx_mesg1,varval);
			if(!strcmp(variable,"tx_mesg2")) strcpy(tx_mesg2,varval);


/*
			if(!strcmp(variable,"roompath")) strcpy(ROOMPATH,varval);
			if(!strcmp(variable,"monpath")) strcpy(MONPATH,varval);
			if(!strcmp(variable,"objpath")) strcpy(OBJPATH,varval);
			if(!strcmp(variable,"playerpath")) strcpy(PLAYERPATH,varval);
			if(!strcmp(variable,"docpath")) strcpy(DOCPATH,varval);
			if(!strcmp(variable,"postpath")) strcpy(POSTPATH,varval);
			if(!strcmp(variable,"binpath")) strcpy(BINPATH,varval);
			if(!strcmp(variable,"logpath")) strcpy(LOGPATH,varval);
*/
		}
		fgets(cmdstr,80,fp);
	}
	fclose(fp);
	return(0);
}
