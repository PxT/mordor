/*
 * ACTION.C:
 *
 *      This file contains the routines necessary to achieve action
 *      commands.
 *
 *      Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"

#define SHIT 349

/**********************************************************************/
/*                              action                                */
/**********************************************************************/

/* This function allows a player to commit an action command.         */

#define OUT(a,b)        print(fd, a); \
			broadcast_rom(fd, ply_ptr->rom_num, b, ply_ptr);
#define OUT2(a,b)       print(fd, a); \
			broadcast_rom(fd, ply_ptr->rom_num, b, ply_ptr, \
			F_ISSET(ply_ptr, PMALES) ? "his":"her");
#define OUT3(a,b)       print(fd, a); \
			broadcast_rom(fd, ply_ptr->rom_num, b, ply_ptr, \
			F_ISSET(ply_ptr, PMALES) ? "he":"she");
#define OUT4(a,b,c)     print(fd, a, crt_ptr); \
			print(crt_ptr->fd, b, ply_ptr); \
			broadcast_rom2(fd, crt_ptr->fd, rom_ptr->rom_num, \
				       c, ply_ptr, crt_ptr);
#define OUT5(a,b)       print(fd, a); \
			broadcast_rom(fd, ply_ptr->rom_num, b, ply_ptr, \
			F_ISSET(ply_ptr, PMALES) ? "him":"her");

int action(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
	room            *rom_ptr;
	creature        *crt_ptr = 0;
	object          *obj_ptr;
	int             match=0, cmdno=0, c=0, n, fd, num;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	F_CLR(ply_ptr, PHIDDN);

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
		crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
				   cmnd->str[1], cmnd->val[1]);
		if(!crt_ptr) {
			cmnd->str[1][0] = up(cmnd->str[1][0]);
			crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
					   cmnd->str[1], cmnd->val[1]);
		}
		if(!crt_ptr || crt_ptr == ply_ptr) {
			print(fd, "That is not here.\n");
			return(0);
		}
	}

	if(!strcmp(cmdlist[cmdno].cmdstr, "nod")) {
		OUT("You nod.\n", "%M nods.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "laugh")) {
		OUT("You fall down laughing.\n", "%M falls down laughing.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "smile")) {
		OUT("You smile happily.\n", "%M smiles happily.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "warm")) {
		OUT("You smile warmly.\n", "%M smiles warmly.");
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
	else if(!strcmp(cmdlist[cmdno].cmdstr, "ponder")) {
                OUT("You ponder the situation.\n", "%M ponders the situation.\n.");
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "think")) {
                OUT("You think carefully.\n", "%M thinks carefully.\n.");
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "ack")) {
                OUT("You ack.\n", "%M acks.\n.");
        }
	else if(!strcmp(cmdlist[cmdno].cmdstr, "nervous")) {
		OUT("You titter nervously.\n", "%M titters nervously.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "sleep")) {
		OUT("You take a nap.\n", "%M dozes off.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "masterbate")) {
		OUT("You masterbate to orgasm.\n", "%M masterbates to orgasm.");
		/*if(mrand(1,100)<1  && !F_ISSET(ply_ptr, PBLIND)) {
		 *	F_SET(ply_ptr,PBLIND);
		 *	ANSI(fd, RED);
		 *	print(fd, "You've gone blind!\n");
		 *	ANSI(fd, WHITE);
		}*/
		
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "shake")) {
		if(crt_ptr) {
			OUT4("You shake %m's hand.\n", "%M shakes your hand.\n", "%M shakes %m's hand.");
		}
		else {
			OUT2("You shake your head.\n", "%M shakes %s head.");
		}
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "knee")) {
                if(crt_ptr) {
                        OUT4("You knee %m in the crotch.\n",
                             "%M painfully knees you in the crotch.\n",
                             "%M painfully knees %m.");
                }
                else
                        print(fd, "Knee whom?\n");
        }
	else if(!strcmp(cmdlist[cmdno].cmdstr, "pounce")) {
                if(crt_ptr) {
                        OUT4("You pounce on %m.\n",
                             "%M pounces on you.\n",
                             "%M pounces on %m.");
                }
                else
                        print(fd, "Pounce on whom?\n");
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "tickle")) {
                if(crt_ptr) {
                        OUT4("You tickle %m mercilessly.\n",
                             "%M tickles you mercilessly.\n",
                             "%M tickles %m mercilessly.");
                }
                else
                        print(fd, "Tickle whom?\n");
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "kick")) {
                if(crt_ptr) {
                        OUT4("You kick %m.\n",
                             "%M kicks you.\n",
                             "%M kicks %m.");
                }
                else
                        print(fd, "Kick whom?\n");
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "tackle")) {
                if(crt_ptr) {
                        OUT4("You tackle %m to the ground.\n",
                             "%M tackles you to the ground.\n",
                             "%M tackles %m to the ground.");
                }
                else
                        print(fd, "Tackle whom?\n");
        }
	else if(!strcmp(cmdlist[cmdno].cmdstr, "frustrate")) {
                OUT2("You pull your hair out.\n",
                     "%M pulls %s hair out in frustration.");
        }
	else if(!strcmp(cmdlist[cmdno].cmdstr, "tap")) {
                OUT2("You tap your foot impatiently.\n",
                     "%M taps %s foot impatiently.");
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
			print(fd, "Ogle whom?\n");
		
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
	else if(!strcmp(cmdlist[cmdno].cmdstr, "expose")) {
		if(crt_ptr) {
			OUT4("You expose yourself in front of %m.\n", 
			     "%M gets naked in front of you.\n",
			     "%M gets naked in front of %m.");
		}
		else {
			OUT5("You expose yourself.\n", "%M exposes %sself.");
		}
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "wink")) {
		if(crt_ptr) {
			OUT4("You wink at %m.\n", "%M winks at you.\n",
			     "%M winks at %m.");
		}
		else {
			OUT("You wink.\n", "%M winks.");
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
		     "%M is so excited, %s can hardly keep still!");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "shrug")) {
		OUT("You shrug your shoulders.\n", "%M shrugs helplessly.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "twiddle")) {
		OUT2("You twiddle your thumbs.\n",
		     "%M twiddles %s thumbs.");
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
		OUT("You giggle inanely.\n", "%M giggles inanely.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "sing")) {
		OUT("You sing a song.\n", "%M sings a song.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "hum")) {
		OUT("You hum a little tune.\n", "%M hums a little tune.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "snap")) {
		OUT2("You snap your fingers.\n", "%M snaps %s fingers.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "jump")) {
		OUT("You jump for joy.\n", "%M jumps for joy.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "skip")) {
		OUT("You skip like a girl.\n", "%M skips like a girl.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "dance")) {
		OUT("You dance about the room.\n", "%M dances about the room.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "cry")) {
		OUT("You burst into tears.\n", "%M bursts into tears.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "bleed")) {
		OUT("You bleed profusely.\n", "%M bleeds profusely.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "sniff")) {
		OUT("You sniff the air.\n", "%M sniffs the air.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "whimper")) {
		OUT("You whimper like a beat dog.\n", 
		    "%M whimpers like a beat dog.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "cringe")) {
		OUT("You cringe fearfully.\n", "%M cringes fearfully.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "whistle")) {
		OUT("You whistle a tune.\n", "%M whistles a tune.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "smirk")) {
		OUT("You smirk wryly.\n", "%M smirks wryly.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "gasp")) {
		OUT("Your jaw drops.\n", "%M gasps in amazement.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "grunt")) {
		OUT("You grunt.\n", "%M grunts agonizingly.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "flex")) {
		OUT2("You flex your muscles.\n", "%M flexes %s muscles.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "blush")) {
		OUT("You blush.\n", "%M turns beet-red.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "stomp")) {
		OUT("You stomp around.\n", "%M stomps around ostentatiously.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "fume")) {
		OUT("You fume.\n", "%M fumes. You can almost see the steam.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "clap")) {
		OUT2("You clap your hands.\n", "%M claps %s hands.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "rage")) {
		OUT("You rage like a madman.\n", "%M rages likes a madman.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "defecate")) {
		if(!dec_daily(&ply_ptr->daily[DL_DEFEC])) {
			print(fd,"You don't have to go.\n");
			return(0);
		}
		n = load_obj(SHIT, &obj_ptr);
		if(n > -1) {
			sprintf(obj_ptr->name, "piece of %s's shit",
				ply_ptr->name);
			add_obj_rom(obj_ptr, ply_ptr->parent_rom);
		}
		OUT("You squat down and shit.\n", "%M squats down and shits.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "pout")) {
		OUT("You pout.\n", "%M pouts like a child.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "drool")) {
		OUT5("You drool.\n", "%M drools all over %sself.");
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
			print(fd, "Goose whom?\n");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "copulate")) {
		if(crt_ptr) {
			OUT4("You copulate with %m.\n", 
			     "%M copulates with you.\n",
			     "%M copulates with %m.");
		}
		else
			print(fd, "Copulate with whom?\n");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "hug")) {
		if(crt_ptr) {
			OUT4("You hug %m.\n", "%M hugs you close.\n",
			     "%M hugs %m close.");
		}
		else
			print(fd, "Hug whom?\n");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "comfort")) {
		if(crt_ptr) {
			OUT4("You comfort %m.\n", "%M comforts you.\n",
			     "%M comforts %m.");
		}
		else
			print(fd, "Comfort whom?\n");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "suck")) {
		if(crt_ptr) {
			OUT4("You suck %m.\n", "%M sucks you.\n", 
			     "%M sucks %m.");
		}
		else
			print(fd, "Suck whom?\n");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "kiss")) {
		if(crt_ptr) {
			OUT4("You kiss %m gently.\n", 
			     "%M kisses you gently.\n",
			     "%M kisses %m.");
		}
		else
			print(fd, "Kiss whom?\n");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "slap")) {
		if(crt_ptr) {
			OUT4("You slap %m.\n", 
			     "%M slaps you across the face.\n",
			     "%M slaps %m across the face.");
		}
		else
			print(fd, "Slap whom?\n");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "glare")) {
		if(crt_ptr) {
			OUT4("You glare menacingly at %m.\n", 
			     "%M glares menacingly at you.\n",
			     "%M glares menacingly at %m.");
		}
		else
			print(fd, "Glare at whom?\n");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "pat")) {
		if(crt_ptr) {
			OUT4("You pat %m on the head.\n", 
			     "%M pats you on the head.\n",
			     "%M pats %m on the head.");
		}
		else
			print(fd, "Pat whom?\n");
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
	else if(!strcmp(cmdlist[cmdno].cmdstr, "cough")) {
		OUT5("You cough politely.\n", "%M coughs politely.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "confused")) {
		OUT("You look bewildered.\n", "%M looks bewildered.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "grumble")) {
		OUT5("You grumble darkly.\n", "%M grumbles darkly.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "hiccup")) {
		OUT("You hiccup.\n", "%M hiccups noisily.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "mutter")) {
		OUT2("You mutter.\n", 
		     "%M mutters obscenities under %s breath.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "scratch")) {
		OUT2("You scratch your head cluelessly.\n", 
		    "%M scratches %s head cluelessly.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "strut")) {
		OUT("You strut around vainly.\n", 
		     "%M struts around vainly.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "sulk")) {
		OUT("You sulk.\n", "%M sulks in dejection.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "satisfied")) {
		OUT("You smile with satisfaction.\n", 
		     "%M smiles with satisfaction.");
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
		     "%M rolls %s eyes in exasperation.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "raise")) {
		OUT("You raise an eyebrow questioningly.\n", 
		    "%M raises an eyebrow questioningly.");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "whine")) {
		OUT("You whine annoyingly.\n", "%M whines annoyingly.");
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
			print(fd, "Grab whom?\n");
	
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "shove")) {
		if(crt_ptr) {
			OUT4("You push %m away.\n", 
			     "%M pushes you away.\n",
			     "%M shoves %m away.");
		}
		else
			print(fd, "Shove whom?\n");
		
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "high5")) {
		if(crt_ptr) {
			OUT4("You slap %m a triumphant highfive.\n", 
			     "%M slaps you a triumphant highfive.\n",
			     "%M slaps %m a triumphant highfive.");
		}
		else
			print(fd, "Give whom a highfive?\n");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "moon")) {
		if(crt_ptr) {
			OUT4("You moon %m.\n", 
			     "%M moons you.  It's a full moon tonight!\n",
			     "%M moons %m.  It's a full moon tonight!");
		}
		else {
			OUT2("You moon the world.\n", 
			    "%M drops %s pants and moons the world.");
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
			print(fd, "Taunt whom?\n"); 
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "eye")) {
		if(crt_ptr) {
			OUT4("You eye %m suspiciously.\n", 
			     "%M eyes you suspiciously.\n",
			     "%M eyes %m suspiciously.");
		}
		else
			print(fd, "Eye whom?\n");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "worship")) {
		if(crt_ptr) {
			OUT4("You worship %m.\n", 
			     "%M kneels and gives praise to you.\n",
			     "%M kneels and gives praise to %m.");
		}
		else
			print(fd, "Worship whom?\n");
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "groan")) {
		OUT("You groan miserabily.\n",
		    "%M groans miserabily\n.");
	
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "flip")) {
		num = mrand(1,100);
		if(crt_ptr) {
			print(fd, "You flip a coin: %s.\n",
				(num >50 ) ? "heads" : "tails");
			print(crt_ptr->fd, "%M flips a coin: %s.\n",
				ply_ptr,(num >50 ) ? "heads" : "tails");
			broadcast_rom2(fd, crt_ptr->fd, rom_ptr->rom_num, 
				"%M flips a coin and shows it to %m.\n", 
				ply_ptr, crt_ptr);
		}
		else {
			print(fd, "You flip a coin: %s.\n",
				(num >50 ) ? "heads" : "tails");
			broadcast_rom(fd, ply_ptr->rom_num,
				"%M flips a coin.\n",ply_ptr);
		}
	}
	return(0);

}
 
