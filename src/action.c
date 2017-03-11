/*
 * ACTION.C:
 *
 *      This file contains the routines necessary to achieve action
 *      commands.
 *
 *      Copyright (C) 1991, 1992, 1993, 1997 Brooke Paul 
 *		2000 additions added by Michael Kolb
 * $Id: action.c,v 6.15 2001/07/17 19:25:11 develop Exp $
 *
 * $Log: action.c,v $
 * Revision 6.15  2001/07/17 19:25:11  develop
 * *** empty log message ***
 *
 * Revision 6.14  2001/03/26 23:39:41  develop
 * added spam check for emotes/actions
 *
 * Revision 6.13  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 *
 */

#include "../include/mordb.h"
#include "mextern.h"

/**********************************************************************/
/*                              action                                */
/**********************************************************************/

/* This function allows a player to commit an action command.         */
#ifdef OUT
#undef OUT
#endif

#define OUT(a,b)        output(fd, a); \
			broadcast_rom(fd, ply_ptr->rom_num, b, m1arg(ply_ptr));

#define OUT2(a,b)       output(fd, a); \
			sprintf(g_buffer, b,  \
			F_ISSET(ply_ptr, PMALES) ? "his":"her"); \
			broadcast_rom(fd, ply_ptr->rom_num, g_buffer, m1arg(ply_ptr));

#define OUT3(a,b)       output(fd, a); \
			sprintf(g_buffer, b, F_ISSET(ply_ptr, PMALES) ? "he":"she"); \
			broadcast_rom(fd, ply_ptr->rom_num, g_buffer, m1arg(ply_ptr));

#define OUT4(a,b,c)     mprint(fd, a, m1arg(crt_ptr)); \
			mprint(crt_ptr->fd, b, m1arg(ply_ptr)); \
			broadcast_rom2(fd, crt_ptr->fd, rom_ptr->rom_num, \
				       c, m2args(ply_ptr, crt_ptr));

#define OUT5(a,b)       output(fd, a); \
			sprintf(g_buffer, b, F_ISSET(ply_ptr, PMALES) ? "him":"her"); \
			broadcast_rom(fd, ply_ptr->rom_num, g_buffer, m1arg(ply_ptr));


int action(creature *ply_ptr, cmd *cmnd )
{
	room            *rom_ptr;
	creature        *crt_ptr = 0;
	object          *obj_ptr;
	int             match=0, cmdno=0, c=0, n, fd, num;

	ASSERTLOG( ply_ptr );
	ASSERTLOG( cmnd );

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	F_CLR(ply_ptr, PHIDDN);

	/* Check for emote spamming */
	if ( check_for_spam( ply_ptr ) )
        {
                return(0);
        }

	do {
		if(!strcmp(cmnd->str[0], cmdlist[c].cmdstr)) {
			match = 1;
			cmdno = c;
			break;
		}
		else if(!strncmp(cmnd->str[0], cmdlist[c].cmdstr, 
			strlen(cmnd->str[0]))) {
			match++;
			cmdno = c;
		}
		c++;
	} while(cmdlist[c].cmdno);

	if(cmnd->num == 2) {
		crt_ptr = find_crt_in_rom(ply_ptr, rom_ptr, cmnd->str[1], 
			cmnd->val[1], MON_FIRST);

		if(!crt_ptr || crt_ptr == ply_ptr) {
			output(fd, "That is not here.\n");
			return(0);
		}
	}

	if(!strcmp(cmdlist[cmdno].cmdstr, "nod")) {
		OUT("You nod.\n", "%M nods.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "smile")) {				/* AZOG */
		OUT("You smile happily.\n", "%M smiles happily.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "wsmile") ||
		!strcmp(cmdlist[cmdno].cmdstr, "warm")) {			/* AZOG */
		OUT("You smile warmly.\n", "%M smiles warmly.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "msmile")) {				/* AZOG */
		OUT("You smile mischievously.\n", "%M smiles mischievously.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "ssmile") ||				/* AZOG */
		!strcmp(cmdlist[cmdno].cmdstr, "satisfied")) {
		OUT("You smile with satisfaction.\n", 
		     "%M smiles with satisfaction.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "snicker")) {
                OUT("You snicker.\n", "%M snickers.");
        }
	else if(!strcmp(cmdlist[cmdno].cmdstr, "beam")) {
		OUT("You beam happily.\n", "%M beams happily.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "relax")) {
		OUT("You breath deeply.\n", "%M takes a deep breath and sighs comfortably.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "smoke")) {
		OUT("You take a puff.\n", "%M puffs on a pipe.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "puke")) {
                OUT("You blow chunks.\n", "%M vomits on the ground.");
        }
	else if(!strcmp(cmdlist[cmdno].cmdstr, "burp")) {
                OUT("You belch loudly.\n", "%M belches rudely.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "blah")){
		OUT("Something got you down?\n", "%M feels blah about life.");
	}	

	else if(!strcmp(cmdlist[cmdno].cmdstr, "nosepick")){
		OUT2("You digging for gold?\n", "%%M picks %s nose.");
	}	

	else if(!strcmp(cmdlist[cmdno].cmdstr, "ponder")) {
                OUT("You ponder the situation.\n", "%M ponders the situation.");
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "think")) {
                OUT("You think carefully.\n", "%M thinks carefully.");
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "ack")) {
                OUT("You ack.\n", "%M acks.");
        }
	else if(!strcmp(cmdlist[cmdno].cmdstr, "nervous")) {
		OUT("You fidget nervously.\n", "%M fidgets nervously.");   /* AZOG */
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "sleep")) {
		OUT("You take a nap.\n", "%M dozes off.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "masturbate")) {			/* AZOG */
		OUT5("You fondle yourself.\n", "%%M fondles %sself.");
		/*if(mrand(1,100)<1  && !F_ISSET(ply_ptr, PBLIND)) {
		 *	F_SET(ply_ptr,PBLIND);
		 *	output_wc(fd, "You've gone blind!\n", BLINDCOLOR);
		}*/
		
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "shake")) {
		if(crt_ptr) {
			OUT4("You shake %m's hand.\n", "%M shakes your hand.\n", "%M shakes %m's hand.");
		}
		else {
			OUT2("You shake your head.\n", "%%M shakes %s head.");
		}
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "knee")) {
                if(crt_ptr) {
                        OUT4("You knee %m in the crotch.\n",
                             "%M painfully knees you in the crotch.\n",
                             "%M painfully knees %m.");
                }
                else
                        output(fd, "Knee whom?\n");
        }
	else if(!strcmp(cmdlist[cmdno].cmdstr, "pounce")) {		/* AZOG */
                if(crt_ptr) {
                        OUT4("You pounce mischieviously on %m.\n",
                             "%M pounces mischieviously on you.\n",
                             "%M pounces mischieviously on %m.");
                }
                else
                        output(fd, "Pounce on whom?\n");
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "tickle")) {
                if(crt_ptr) {
                        OUT4("You tickle %m mercilessly.\n",
                             "%M tickles you mercilessly.\n",
                             "%M tickles %m mercilessly.");
                }
                else
                        output(fd, "Tickle whom?\n");
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "kick")) {
                if(crt_ptr) {
                        OUT4("You kick %m.\n",
                             "%M kicks you.\n",
                             "%M kicks %m.");
                }
                else
		OUT5("You kick yourself in frustration.\n", "%%M kicks %sself in frustration.");	/* AZOG */
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "tackle")) {
                if(crt_ptr) {
                        OUT4("You tackle %m to the ground.\n",
                             "%M tackles you to the ground.\n",
                             "%M tackles %m to the ground.");
                }
                else
                        output(fd, "Tackle whom?\n");
        }
	else if(!strcmp(cmdlist[cmdno].cmdstr, "frustrate")) {
                OUT2("You pull your hair out.\n",
                     "%%M pulls %s hair out in frustration.");
        }
	else if(!strcmp(cmdlist[cmdno].cmdstr, "tap")) {
                OUT2("You tap your foot impatiently.\n",
                     "%%M taps %s foot impatiently.");
        }
	else if(!strcmp(cmdlist[cmdno].cmdstr, "cheer")) {
                if(crt_ptr) {
                        OUT4("You cheer for %m.\n", "%M cheers for you.\n",
                             "%M cheers for %m.");
                }
                else {
                        OUT("You cheer.\n", "%M yells like a cheerleader.");
                }
        }
	else if(!strcmp(cmdlist[cmdno].cmdstr, "poke")) {
		if(crt_ptr) {
			OUT4("You poke %m.\n", "%M pokes you.\n",
			     "%M pokes %m.");
		}
		else {
			OUT("You poke everyone.\n", "%M pokes you.");
		}
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "ogle")) {
		if(crt_ptr) {
			OUT4("You ogle %m with carnal intent.\n", "%M ogles you salaciously.\n",
			     "%M ogles %m salaciously.");
		}
		else
			output(fd, "Ogle whom?\n");
		
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "fart")) {
		if(crt_ptr) {
			OUT4("You fart on %m.\n", "%M farts on you.\n",
			     "%M farts on %m.");
		}
		else {
			OUT("You fart.\n", "%M breaks wind.");
		}
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "spit")) {
		if(crt_ptr) {
			OUT4("You spit on %m.\n", "%M spits on you.\n",
			     "%M spits on %m.");
		}
		else {
			OUT("You spit.\n", "%M spits.");
		}
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "expose")) {					/* AZOG */
		OUT5("You publicly expose yourself.\n", "%%M exposes %sself.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "wink")) {
		if(crt_ptr) {
			OUT4("You wink at %m.\n", "%M winks at you.\n",
			     "%M winks at %m.");
		}
		else {
			OUT("You wink mischeviously.\n", "%M winks mischeviously.");
		}
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "wave")) {
		if(crt_ptr) {
			OUT4("You wave to %m.\n", "%M waves to you.\n",
			     "%M waves to %m.");
		}
		else {
			OUT("You wave happily.\n", "%M waves happily.");
		}
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "chuckle")) {
		OUT("You chuckle.\n", "%M chuckles.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "cackle")) {
		OUT("You cackle gleefully.\n",
		    "%M cackles out loud with glee.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "sigh")) {
		OUT("You sigh sadly.\n", "%M lets out a long, sad sigh.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "bounce")) {
		OUT3("You bounce around wildly!\n", 
		     "%%M is so excited, %s can hardly keep still!");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "shrug")) {
		OUT("You shrug your shoulders.\n", "%M shrugs helplessly.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "twiddle")) {
		OUT2("You twiddle your thumbs.\n",
		     "%%M twiddles %s thumbs.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "yawn")) {
		OUT("You yawn loudly.\n", "%M yawns out loud.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "grin")) {
		OUT("You grin evilly.\n", "%M grins evilly.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "frown")) {
		OUT("You frown.\n", "%M frowns.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "giggle")) {
		OUT("You giggle mirthfully.\n", "%M giggles mirthfully.");  /* AZOG */
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "sing")) {
		if(ply_ptr->class == BARD || ply_ptr->class >= CARETAKER) {  /* AZOG */
		bard_song(ply_ptr, cmnd);
		return(0);
		}
		else 
		OUT("You sing a song.\n", "%M sings a song.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "hum")) {
		OUT("You hum a little tune.\n", "%M hums a little tune.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "snap")) {
		OUT2("You snap your fingers.\n", "%%M snaps %s fingers.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "jump")) {
		OUT("You jump for joy.\n", "%M jumps for joy.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "skip")) {
		OUT("You skip like a girl.\n", "%M skips like a girl.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "twirl")) {				/* AZOG */
		OUT("You twirl about in joy.\n", "%M twirl about in joy.");  
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "cry")) {
		OUT("You burst into tears.\n", "%M bursts into tears.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "bleed")) {
		OUT("You bleed profusely.\n", "%M bleeds profusely.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "whimper")) {
		OUT("You whimper like a beat dog.\n", 
			"%M whimpers like a beat dog.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "cringe")) {
		OUT("You cringe fearfully.\n", "%M cringes fearfully.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "smirk")) {
		OUT("You smirk wryly.\n", "%M smirks wryly.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "gasp")) {
		OUT("Your jaw drops.\n", "%M gasps in amazement.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "grunt")) {
		OUT("You grunt agonizingly.\n", "%M grunts agonizingly.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "flex")) {
		OUT2("You flex your muscles.\n", "%%M flexes %s muscles.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "blush")) {
		OUT("You blush.\n", "%M turns beet-red.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "stomp")) {
		OUT("You stomp around in frustration.\n", "%M stomps around in frustration.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "fume")) {
		OUT("You fume.\n", "%M fumes. You can almost see the steam.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "rage")) {
		OUT("You rage like a madman.\n", "%M rages likes a madman.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "flaugh")) {			/* AZOG FROM HERE DOWN */
		OUT("You fall down laughing.\n", "%M falls down laughing.");
	}
 	else if(!strcmp(cmdlist[cmdno].cmdstr, "rlaugh")) {                     /* AZOG FROM HERE DOWN */
                OUT("You roll on the floor laughing.\n", "%M rolls on the floor laughing.");
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "hlaugh")) {                     /* AZOG FROM HERE DOWN */
                OUT("You laugh hysterically.\n", "%M laughs hysterically.");
        }
	else if(!strcmp(cmdlist[cmdno].cmdstr, "sniff")) {
		OUT("You start to cry.\n", "%M starts to cry.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "snivel")) {
		OUT("You snivel in despair.\n", "%M snivels in despair.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "smell")) {
		OUT("You sniff the air.\n", "%M sniffs the air.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "sneeze")) {
		OUT("You sneeze.\n", "%M sneezes.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "lick")) {
		OUT2("You lick your lips in anticipation.\n", "%%M licks %s lips in anticipation.");
	}
	else if (!strcmp(cmdlist[cmdno].cmdstr, "grind")) {
		OUT("You grind your teeth.\n", "%M grinds %s teeth.");
	}
	else if (!strcmp(cmdlist[cmdno].cmdstr, "pace")) {
		OUT("You pace to and fro.\n", "%M paces to and fro.");
	}
	else if (!strcmp(cmdlist[cmdno].cmdstr, "snort")) {
		OUT("You snort indignantly.\n", "%M snorts indignantly.");
	}
	else if (!strcmp(cmdlist[cmdno].cmdstr, "collapse")) {
		OUT("You collapse from exhaustion.\n", "%M collapses from exhaustion.");
	}
	else if (!strcmp(cmdlist[cmdno].cmdstr, "idea")) {
		OUT("Your eyes light up with a brilliant idea!\n", "%M gets a brilliant idea!");
	}
	else if (!strcmp(cmdlist[cmdno].cmdstr, "sweat")) {
		OUT("You begin to sweat nervously.\n", "%M begins to sweat nervously");
	}
	else if (!strcmp(cmdlist[cmdno].cmdstr, "squirm")) {
		OUT("You squirm uncomfortably.\n", "%M squirms uncomfortably");
	}
	else if (!strcmp(cmdlist[cmdno].cmdstr, "nose")) {
		OUT2("You pick your nose.\n", "%%M picks %s nose.");
	}
	else if (!strcmp(cmdlist[cmdno].cmdstr, "attention")) {
		OUT("You snap to attention.\n", "%M snaps to attention.");	
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "defecate")) {
		if(!dec_daily(&ply_ptr->daily[DL_DEFEC]) || ply_ptr->level < 4 || F_ISSET(ply_ptr->parent_rom, RPOWND)) {
			output(fd,"You don't have to go.\n");
			return(0);
		}
		n = load_obj(SHIT, &obj_ptr);
		if(n > -1) {
			sprintf(obj_ptr->name, "piece of %s's shit",
				(F_ISSET(ply_ptr, PALIAS) ? Ply[ply_ptr->fd].extr->alias_crt->name : ply_ptr->name));
			add_obj_rom(obj_ptr, ply_ptr->parent_rom);
		}
		OUT("You squat down and shit.\n", "%M squats down and shits.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "pout")) {
		OUT("You pout.\n", "%M pouts like a child.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "drool")) {
		OUT5("You drool.\n", "%%M drools all over %sself.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "faint")) {
		OUT("You faint.\n", "%M faints.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "goose")) {
		if(crt_ptr) {
			OUT4("You goose %m.\n", "%M gooses you.\n",
			     "%M gooses %m.");
		}
		else
			output(fd, "Goose whom?\n");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "copulate")) {
		if(crt_ptr) {
			OUT4("You copulate with %m.\n", 
			     "%M copulates with you.\n",
			     "%M copulates with %m.");
		}
		else
			output(fd, "Copulate with whom?\n");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "hug")) {
		if(crt_ptr) {
			OUT4("You hug %m.\n", "%M hugs you close.\n",
			     "%M hugs %m close.");
		}
		else
			output(fd, "Hug whom?\n");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "comfort")) {
		if(crt_ptr) {
			OUT4("You comfort %m.\n", "%M comforts you.\n",
			     "%M comforts %m.");
		}
		else
			output(fd, "Comfort whom?\n");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "suck")) {
		if(crt_ptr) {
			OUT4("You suck %m.\n", "%M sucks you.\n", 
			     "%M sucks %m.");
		}
		else {
			OUT2("You suck your thumb.\n", "%%M sucks %s thumb.");		/* AZOG */
		}
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "kiss")) {
		if(crt_ptr) {
			OUT4("You kiss %m gently.\n", 
			     "%M kisses you gently.\n",
			     "%M kisses %m.");
		}
		else
			output(fd, "Kiss whom?\n");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "kisshand")) {
		if(crt_ptr) {
			OUT4("You kiss %m on the hand.\n", 
			     "%M bows and kisses your hand.\n",
			     "%M bows and kisses %m on the hand.");
		}
		else
			output(fd, "Kiss whom?\n");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "slap")) {
		if(crt_ptr) {
			OUT4("You slap %m.\n", 
			     "%M slaps you across the face.\n",
			     "%M slaps %m across the face.");
		}
		else
			output(fd, "Slap whom?\n");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "glare")) {
		if(crt_ptr) {
			OUT4("You glare menacingly at %m.\n", 
			     "%M glares menacingly at you.\n",
			     "%M glares menacingly at %m.");
		}
		else
			output(fd, "Glare at whom?\n");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "pat")) {
		if(crt_ptr && crt_ptr->class < CARETAKER) {                  /* AZOG */
			OUT4("You pat %m on the head.\n", 
			     "%M pats you on the head.\n",
			     "%M pats %m on the head.");
		}
		else
			output(fd, "Pat whom?\n");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "pet")) {
	    if(crt_ptr) {
		    OUT4("You pet %m on the back of the head.\n",
			    "%M pets you on the back of the head.\n",
                "%M pets %m on the back of the head.");
        }
        else
			output(fd, "Pet whom?\n");
    }
	else if(!strcmp(cmdlist[cmdno].cmdstr, "bow")) {
		if(crt_ptr) {
			OUT4("You bow before %m.\n", 
			     "%M bows before you.\n",
			     "%M bows before %m.");
		}
		else {
			OUT("You make a full-sweeping bow.\n",
			    "%M makes a full-sweeping bow.");
		}
	}
    else if(!strcmp(cmdlist[cmdno].cmdstr, "curtsy")) {
	    if(crt_ptr) {
		    OUT4("You curtsy before %m.\n",
			    "%M curtsies before you.\n",
                "%M curtsies before %m.");
        }
        else {
            OUT("You curtsy rather daintily.\n",
                "%M curtsies.");
        }
    }
	else if(!strcmp(cmdlist[cmdno].cmdstr, "cough")) {
		OUT("You cough politely.\n", "%M coughs politely.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "confused")) {
		OUT("You look bewildered.\n", "%M looks bewildered.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "grumble")) {
		OUT("You grumble darkly.\n", "%M grumbles darkly.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "hiccup")) {
		OUT("You hiccup.\n", "%M hiccups noisily.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "mutter")) {
		OUT2("You mutter.\n", 
		     "%%M mutters obscenities under %s breath.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "scratch")) {
		OUT2("You scratch your head cluelessly.\n", 
		    "%%M scratches %s head cluelessly.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "strut")) {
		OUT("You strut around vainly.\n", 
		     "%M struts around vainly.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "sulk")) {
		OUT("You sulk.\n", "%M sulks in dejection.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "sit")) {
		OUT("You sit down.\n", 
		     "%M takes a seat.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "stand")) {
		OUT("You stand up.\n", 
		     "%M stands up.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "wince")) {
		OUT("You wince painfully.\n", "%M winces painfully.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "roll")) {
		OUT2("You roll your eyes in exasperation.\n", 
		     "%%M rolls %s eyes in exasperation.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "raise")) {
		OUT("You raise an eyebrow questioningly.\n", 
		    "%M raises an eyebrow questioningly.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "whine")) {
		OUT("You whine annoyingly.\n", "%M whines annoyingly.");
	}
    else if(!strcmp(cmdlist[cmdno].cmdstr, "grr")) {
	    if(crt_ptr) {
		    OUT4("You stare menacingly at %m.\n",
			    "%M threatens you with a look of death.\n",
                "%M stares at %m with a look of hatred.");
        }
        else {
            OUT("You start looking angry.\n",
                "%M looks angrily around.");
        }
    }
	else if(!strcmp(cmdlist[cmdno].cmdstr, "growl")) {
		if(crt_ptr) {
			OUT4("You growl at %m.\n", 
			     "%M threatens you with a growl.\n",
			     "%M growls at %m threateningly.");
		}
		else {
			OUT("You growl.\n", 
			    "%M growls threateningly.");
		}
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "bird")) {
		if(crt_ptr) {
			OUT4("You flip off %m.\n", 
			     "%M flips you the bird.\n",
			     "%M gestures indignantly at %m.");
		}
		else {
			OUT("You gesture indignantly.\n", 
			    "%M gestures indignantly.");
		}
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "grab")) {
		if(crt_ptr) {
			OUT4("You pull on %m.\n", 
			     "%M pulls on you eagerly.\n",
			     "%M pulls on %m eagerly.");
		}
		else
			output(fd, "Grab whom?\n");
	
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "shove")) {
		if(crt_ptr) {
			OUT4("You push %m away.\n", 
			     "%M pushes you away.\n",
			     "%M shoves %m away.");
		}
		else
			output(fd, "Shove whom?\n");
		
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "high5")) {
		if(crt_ptr) {
			OUT4("You slap %m a triumphant highfive.\n", 
			     "%M slaps you a triumphant highfive.\n",
			     "%M slaps %m a triumphant highfive.");
		}
		else
			output(fd, "Give whom a highfive?\n");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "moon")) {
		if(crt_ptr) {
			OUT4("You moon %m.\n", 
			     "%M moons you.  It's a full moon tonight!\n",
			     "%M moons %m.  It's a full moon tonight!");
		}
		else {
			OUT2("You moon the world.\n", 
			    "%%M drops %s pants and moons the world.");
		}
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "purr")) {
		if(crt_ptr) {
			OUT4("You purr at %m.\n", 
			     "%M purrs provocatively at you.\n",
			     "%M purrs provocatively at %m.");
		}
		else {
			OUT("You purr provocatively.\n",
			    "%M purrs provocatively.");
		}
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "taunt")) {
		if(crt_ptr) {
			OUT4("You taunt and jeer at %m.\n", 
			     "%M taunts and jeers at you.\n",
			     "%M taunts and jeers at %m.");
		}
		else 
			output(fd, "Taunt whom?\n"); 
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "eye")) {
		if(crt_ptr) {
			OUT4("You eye %m suspiciously.\n", 
			     "%M eyes you suspiciously.\n",
			     "%M eyes %m suspiciously.");
		}
		else
			output(fd, "Eye whom?\n");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "worship")) {			/* AZOG */
		if(crt_ptr) {
			OUT4("You worship %m.\n", 
			     "%M bows down and gives praise to you.\n",
			     "%M bows down and gives praise to %m.");
		}
		else
			OUT("You bow down and give praise.\n",
			    "%M bows down and worships.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "groan")) {
		OUT("You groan miserabily.\n",
		    "%M groans miserabily.\n");
	
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "flip")) {
		num = mrand(1,100);

		sprintf(g_buffer, "You flip a coin: %s.\n",
			(num >50 ) ? "heads" : "tails");
		output(fd, g_buffer);
		if(crt_ptr) {
			sprintf(g_buffer, "%%M flips a coin: %s.\n",
				(num >50 ) ? "heads" : "tails");
			mprint(crt_ptr->fd, g_buffer,
				m1arg(ply_ptr));
			broadcast_rom2(fd, crt_ptr->fd, rom_ptr->rom_num, 
				"%M flips a coin and shows it to %m.", 
				m2args(ply_ptr, crt_ptr));
		}
		else {
			broadcast_rom(fd, ply_ptr->rom_num,
				"%M flips a coin.", m1arg(ply_ptr));
		}
	}
	else if (!strcmp(cmdlist[cmdno].cmdstr, "point"))			/*ALL BELOW ARE AZOG'S */
	{
		if(crt_ptr)
		{
			OUT4("You point at %m.\n",
				"%M points at you.\n",
				"%M points at %m.");
		}
		else
		{
			OUT5("You point at yourself.\n",
			"%%M points at %sself.");
		}
	}
	else if (!strcmp(cmdlist[cmdno].cmdstr, "sneer"))
	{
		if(crt_ptr)
		{
			OUT4("You sneer contemptuously at %m.\n",
				"%M sneers at you contemptuously.\n",
				"%M sneers contemptuously at %m.");
		}
		else
		{
			OUT("You sneer contemptuously.\n",
			"%M sneers contemptuously.");
		}
	}	
	else if (!strcmp(cmdlist[cmdno].cmdstr, "punch"))
	{
		if(crt_ptr)
		{
			OUT4("You punch %m in the arm.\n",
				"%M punches you in the arm.\n",
				"%M punches %m in the arm.");
		}
		else
		{
			OUT("You shadow box.\n",
			"%M shadow boxes.");
		}
	}
	else if (!strcmp(cmdlist[cmdno].cmdstr, "laugh"))
	{
		if(crt_ptr)
		{
			OUT4("You laugh at %m.\n",
				"%M laughs at you.\n",
				"%M laughs at %m.");
		}
		else
		{
			OUT("You laugh.\n",
			"%M laughs.");
		}
	}
	else if (!strcmp(cmdlist[cmdno].cmdstr, "whistle"))
	{
		if(crt_ptr)
		{
			OUT4("You whistle appreciatively at %m.\n",
				"%M whistles appreciatively at you.\n",
				"%M whistles appreciatively at %m.");
		}
		else
		{
			OUT("You whistle innocently.\n",
			"%M whistles innocently.");
		}
	}
	else if (!strcmp(cmdlist[cmdno].cmdstr, "dance"))
	{
		if(crt_ptr)
		{
			OUT4("You dance with %m.\n",
				"%M dances with you.\n",
				"%M dances with %m.");
		}
		else
		{
			OUT("You dance about.\n",
			"%M dances about.");
		}
	}
	else if (!strcmp(cmdlist[cmdno].cmdstr, "thumb"))
	{
		if(crt_ptr)
		{
			OUT4("You give %m the thumbs up sign.\n",
				"%M gives you the thumbs up sign.\n",
				"%M gives %m the thumbs up sign.");
		}
		else
		{
			OUT("You give a thumbs up.\n",
			"%M gives a thumbs up.");
		}
	}		
	else if (!strcmp(cmdlist[cmdno].cmdstr, "imitate"))
	{
		if(crt_ptr)
		{
			OUT4("You imitate %m.\n",
				"%M imitates you.\n",
				"%M imitates %m.");
		}
		else
		{
			output(fd,"Imitate whom?\n");
		}
	}
	else if (!strcmp(cmdlist[cmdno].cmdstr, "plead"))
	{
		if(crt_ptr)
		{
			OUT4("You plead before %m.\n",
				"%M pleads pathetically with you.\n",
				"%M begs pathetically to %m.");
		}
		else
		{
			OUT("You plead pathetically.\n",
			"%M pleads pathetically.");
		}
	}	
	else if (!strcmp(cmdlist[cmdno].cmdstr, "grovel"))
	{
		if(crt_ptr)
		{
			OUT4("You grovel before %m.\n",
				"%M grovels before you.\n",
				"%M grovels before %m.");
		}
		else
		{
			OUT5("You throw yourself on the ground and grovel pathetically.\n",
			"%%M throws %sself on the ground and grovels pathetically.");
		}
	}
	else if (!strcmp(cmdlist[cmdno].cmdstr, "cower"))
	{
		if(crt_ptr)
		{
			OUT4("You cower in fear before %m.\n",
				"%M cowers in fear before you.\n",
				"%M cowers in fear before %m.");
		}
		else
		{
			OUT("You cower in fear.\n",
			"%M cowers in fear.");
		}
	}
	else if (!strcmp(cmdlist[cmdno].cmdstr, "cuddle"))
	{
		if(crt_ptr)
		{
			OUT4("You cuddle up with %m.\n",
				"%M cuddles up with you.\n",
				"%M cuddles up with %m.");
		}
		else
		{
			output(fd,"Cuddle with whom?\n");			
		}
	}	
	else if(!strcmp(cmdlist[cmdno].cmdstr, "shush")) {
		if(crt_ptr) {
			OUT4("You shush %m.\n", 
			     "%M shushes you!\n",
			     "%M shushes %m.");
		}
		else 
			OUT("You shush everyone.\n",
			"%M shushes everyone.");
	}
	else if (!strcmp(cmdlist[cmdno].cmdstr, "scowl")) {
		if(crt_ptr) {
			OUT4("You scowl scornfully at %m.\n",
				"%M scowls scornfully at you.\n",
				"%M scowls scornfully at %m.");
		}
		else
		{
			OUT("You scowl.\n",
			"%M scowls in contempt.");
		}
	}	
	else if (!strcmp(cmdlist[cmdno].cmdstr, "salute"))
	{
		if(crt_ptr)
		{
			OUT4("You salute %m.\n",
				"%M salutes you.\n",
				"%M salutes %m.");
		}
		else
		{
			output(fd,"Salute whom?\n");
		}
	}	
	else if (!strcmp(cmdlist[cmdno].cmdstr, "clap"))
	{
		if(crt_ptr)
		{
			OUT4("You applaud %m.\n",
				"%M applauds for you.\n",
				"%M applauds %m.");
		}
		else
		{
			OUT2("You clap your hands.\n", "%%M claps %s hands.");
		}
	}
	else if (!strcmp(cmdlist[cmdno].cmdstr, "beckon"))
	{
		if(crt_ptr)
		{
			OUT4("You beckon to %m.\n",
				"%M beckons you to follow.\n",
				"%M beckons %m to follow.");
		}
		else
		{
			OUT("You beckon to everyone.\n", "%M beckons everyone to follow.");
		}
	}
	else if (!strcmp(cmdlist[cmdno].cmdstr, "greet"))
	{
		if(crt_ptr)
		{
			OUT4("You greet %m warmly.\n",
				"%M greets you warmly.\n",
				"%M greets %m warmly.");
		}
		else
		{
			OUT("You greet everyone cordially.\n", "%M greets everyone cordially.");
		}
	}

	return(0);

}
 



