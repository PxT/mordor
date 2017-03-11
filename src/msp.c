/*************************************************************************
*  Mud Sound Protocol
*	These functions enable MSP.
*
*	Copyright (c) 1998 John P. Freeman, Paul Telford
*
* $Id: msp.c,v 6.13 2001/03/08 16:09:09 develop Exp $
*
* $Log: msp.c,v $
* Revision 6.13  2001/03/08 16:09:09  develop
* *** empty log message ***
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/morutil.h"
#include "../include/mordb.h"

#include "mextern.h"



/************************************************************************/
/* play_sound()								*/
/*           "fname" is the name of the sound file to be played
          "vol" is the volume level to play the sound at
          "repeats" is the number of times to play the sound
          "priority" is the priority of the sound
          "type" is the sound class
          "URL" is the optional download URL for the sound file

*************************************************************************/
void play_sound(int fd, char *fname, int vol, int repeat, int priority, char *type, char *url)
{
	char	buffer[2048];

	sprintf(buffer, "!!SOUND(%s V=%d L=%d P=%d T=%s U=%s)\n",
		fname, vol, repeat, priority, type, url);
	output(fd, buffer );
	return;
}



/************************************************************************/
/* play_music()								*/
/*          "fname" is the name of the music file to be played
          "vol" is the volume level to play the music at "repeats" is the
          number of times to play the music file
          "cont" specifies whether the file should be restarted if
          requested again or continued
          "type" is the sound class
          "URL" is the optional download URL for the sound file

*************************************************************************/
void play_music(int fd, char *fname, int vol, int repeat, int cont, char *type, char *url)
{

        char buffer[2048];

        sprintf(buffer, "!!MUSIC(%s V=%d L=%d C=%d T=%s U=%s)\n",
                        fname, vol, repeat, cont, type, url);
        output(fd, buffer);
        return;
}

