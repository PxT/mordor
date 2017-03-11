/*
 * MAIN.C:
 *
 *	This files contains the main() function which initiates the
 *	game.
 *
 *	Copyright (C) 1991, 1992, 1993 Brooke Paul
 *
 * $Id: main.c,v 6.17 2001/07/29 22:11:02 develop Exp $
 *
 * $Log: main.c,v $
 * Revision 6.17  2001/07/29 22:11:02  develop
 * added shop_cost
 *
 * Revision 6.16  2001/07/08 18:56:23  develop
 * fixed punctuation error in display_credits
 *
 * Revision 6.15  2001/04/17 18:56:39  develop
 * *** empty log message ***
 *
 * Revision 6.14  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */
#include <stdio.h>


#include "../include/mordb.h"
#include "../include/version.h"

#include "mextern.h"


#define SCHEDPORT  4040

char report =0;
time_t last_dust_output;

#ifndef WINNT_SERVICE
int main(int argc,char *argv[])
{
	/* static unsigned short schedule_g();*/

	if(SCHED)
		if(argc == 1)
			PORTNUM = schedule_g();

	handle_args(argc, argv );

	startup_mordor();


	return(0);
}
#else

void main(void)
{
	SERVICE_TABLE_ENTRY serviceTable[] =
	{
		{pcServiceName, ServiceMain},
		{NULL, NULL}
	};
	int bSuccess;

	bSuccess = StartServiceCtrlDispatcher(serviceTable);
	if (!bSuccess)
		HandleError("StartServiceCtrlDispatcher failed!", GetLastError());
}  

#endif


void startup_mordor(void)
{
	int i;
	/* static void mvc_log(); */

	/* paths are now configurable in mordor.cf */
	/* but not read i during the normal readcf */
	/* to avoid problems when reloading the .cf file */
	load_paths();
	
	init_guild();

	i=readcf();

#ifndef _DEBUG
	if(CRASHTRAP){
		signal(SIGABRT, crash);	/* abnormal termination triggered by abort call */
		signal(SIGFPE, crash);	/* floating point exception */
		signal(SIGILL, crash);	/* illegal instruction - invalid function image */
		signal(SIGSEGV, crash);	/* segment violation */
	}
#endif

	/* clear the spy flags */
	clear_spy();

	/* clear the player array */
	zero( Ply, sizeof(plystruct) * PMAX );

	if(AUTOSHUTDOWN)
		if (!Shutdown.interval){
			Shutdown.ltime = time(0);
			Shutdown.interval = 43200L;
		}


	umask(000);
	srand(getpid() + time(0));
	load_lockouts();


 	sock_init(PORTNUM);


#ifndef DEBUG
#ifndef WIN32
	if(fork()) exit(0);		/* go into background */
#else
	FreeConsole();
#endif
	close(0); close(1); close(2);	/* close stdio */
#endif /* DEBUG */


	if(RECORD_ALL) 
		mvc_log();

	StartTime = time(0);
	sprintf(g_buffer, "--- Game Up: %u --- [%s]\n", (int)PORTNUM, VERSION );
	elog_broad(g_buffer);
	plog(g_buffer);

	/* function to catch exit calls */
	atexit(log_exit);

	sock_loop();

	cleanup_sockets();
	
	return;
}

void display_credits(int fd)
{

/*********************************************************************/
/*  As part of the copyright agreement this section must be left     */
/* intact.  Please give credit where credit is due, and feel free to */
/* add your name once you begin adding to the Mordor codebase.       */
/*********************************************************************/

        sprintf(g_buffer, "\n  Mordor v%s", VERSION);
        output( fd, g_buffer );
        output(fd, "\nProgrammed by:");
        output(fd, "\n  Brooke Paul, Paul Telford & John P. Freeman\n\n");
                     /* brooke@nazgul.com */ 
		     		/* paul@droflet.net  */
        output(fd, "\nBased upon Quest for Mordor by:");
        output(fd, "\n  Brett J. Vickers");
        output(fd, "\nContributions by:");
        output(fd, "\n  Steve Smith, Eric Krichbaum, Charles Marchant,");
        output(fd, "\n  Michael Kolb, Anthony Kulig, Mark Stoltzfus.");
#ifdef WIN32
        output(fd, "\nWIN32 by:\n  John P. Freeman & Eric Krichbaum"); 
                                        /* ekrich@iolinc.net */
#endif /* WIN32 */

        output(fd, "\n");

}



void usage( char *szName )
{
#ifdef WIN32
	printf(" %s [port number] [-r] [-i] [-u]\n", szName );
#else
	printf(" %s [port number] [-r]\n", szName );
#endif
	return;
}

/****************************************************************/
/* log_exit()							*/
/*		can be used to help trap erronous exits		*/
/****************************************************************/
void log_exit()
{
	/* this function should be called during normal and abnormal */
	/* exit situations.  Might be a good idea to make this  */
	/* configurable in the mordor.cf file so we can dump useful */
	/* information here to debug crashes without cluttering up  */
	/* the normal exits */

	elog("--- Game is exiting. ---\n");

	return;
}

/*================================================================*/
/*================================================================*/
void handle_args( int argc, char *argv[] )
{
	int	i;

	bHavePort = 0;
	for ( i = 1; i < argc; i++ )
	{
		switch (argv[i][0] )
		{
			case '-':
			case '/':
				switch ( argv[i][1] )
				{
					case 'r':
					case 'R':
						report = 1;
						break;

#ifdef WIN32
					case 'i':
					case 'I':
						/* install NT Service */
						InstallService();
						break;

					case 'u':
					case 'U':
						/* uninstall NT Service */
						RemoveService();
						break;
#endif

					default:
						printf("Unknown option./n");
						usage( argv[0] );
						break;
				}
				break;
			default:
				if(is_num(argv[i]))
				{
					PORTNUM = (unsigned short)atoi(argv[i]);
					bHavePort = 1;
				}
				else
				{
				printf("Unknown option\n");
				usage( argv[0] );
				}
		}
	}	

}




/*================================================================*/

#define DAYSEC 86400L
#define MORNT 27900L
#define MIDSEC(x) (((x->tm_hour)*3600)+((x->tm_min)*60)+(x->tm_sec))

/*================================================================*/
unsigned short schedule_g()
/* The schedule_g function keeps the game on a time  schedule */
{
    struct tm   *time_m;
    time_t	t = 0;
    unsigned short		mport = SCHEDPORT, port = PORTNUM; 
    enum {Sun, Mon, Tue, Wed, Thu, Fri, Sat};

    t = time(0);
    time_m = localtime(&t);
    t = 0;

    switch(time_m->tm_wday){
	case Sun:
 		t = DAYSEC - MIDSEC(time_m) + MORNT;
		break;
	case Mon: 
	case Tue: 
	case Wed: 
	case Thu:
		if (time_m->tm_hour >= 18)
			t = MORNT + DAYSEC - MIDSEC(time_m);
		else if (MIDSEC(time_m) < MORNT)
			t =  MORNT - MIDSEC(time_m);
		else{
			t = (18L*3600L) - MIDSEC(time_m);
			port =  SCHEDPORT;
			mport  = PORTNUM;
		}
		break;
	case Fri: 
		if (time_m->tm_hour >= 18)
			t = DAYSEC*3 - MIDSEC(time_m) + MORNT;
		else if (MIDSEC(time_m) < MORNT) 
			t =  MORNT - MIDSEC(time_m);
		else{
			t = (18L*3600L) - MIDSEC(time_m);
			port =  SCHEDPORT;
			mport = PORTNUM;
		}
		break;
	case Sat:
		t = DAYSEC*2 - MIDSEC(time_m) + MORNT;
		break;
		}

	if (!port)
		exit(0);


	if(AUTOSHUTDOWN)
		t = MIN(21600L,t);
	Shutdown.ltime = time(0);
	Shutdown.interval =  t;
	PORTNUM = mport;
	return (port);
}

/*================================================================*/
void mvc_log(){
/*  This function renames the list of all command file to include  *
 *  the date.  This is done to preserve the list commands that     *
 *  occured before a crash. */

	char *date, mfile[128], rfile[128];
	time_t i,t;

	t = time(0);
	date = ctime(&t);

	for(i=0;i<24;i++)
		if(date[i] == ' ')
			date[i] = '_';
	date[24] = 0;
	sprintf(rfile,"%s/%s", get_log_path(), "all_cmd");
	sprintf(mfile,"%s/%s_%s", get_log_path(), "all_cmd",date);

	rename( rfile, mfile);

	return;
}
