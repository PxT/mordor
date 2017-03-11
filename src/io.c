/*
 * IO.C:
 *
 *	Socket input/output/establishment functions.
 *
 *	Copyright (C) 1991-1999 Brooke Paul
 *
 * $Id: io.c,v 6.15.1.6 2001/07/29 22:11:02 develop Exp $
 *
 * $Log: io.c,v $
 * Revision 6.15.1.6  2001/07/29 22:11:02  develop
 * added shop_cost
 *
 * Revision 6.15.1.5  2001/07/22 19:03:06  develop
 * first run at alllowing thieves to steal gold from other players
 *
 * Revision 6.15.1.4  2001/07/13 22:37:15  develop
 * removed signal catch for SIGUSR1
 *
 * Revision 6.15.1.3  2001/07/05 22:41:45  develop
 * added trap for SIGTERM to cause a quick_shutdown
 *
 * Revision 6.15.1.2  2001/06/06 19:36:46  develop
 * conjure added
 *
 * Revision 6.15.1.1  2001/05/04 01:56:19  develop
 * added logging of disconnects due to lockout
 *
 * Revision 6.15  2001/04/29 02:30:26  develop
 * added check_proxies
 *
 * Revision 6.18  2001/04/27 17:32:26  develop
 * *** empty log message ***
 *
 * Revision 6.17  2001/04/11 21:57:32  develop
 * *** empty log message ***
 *
 * Revision 6.16  2001/04/11 21:46:39  develop
 * *** empty log message ***
 *
 * Revision 6.14  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 *
 */


#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

#ifdef HPUX
	#include <strings.h>
#endif

#ifndef WIN32
	#include <signal.h>
#endif
	


/*#include <sys/filio.h>*/

#include "../include/mordb.h"
#include "../include/version.h"
#include "mextern.h"

#define buflen(a,b,c)	(a-b + (a<b ? c:0))
#define saddr		addr.sin_addr.s_addr
#define MAXPLAYERS	100

typedef struct wq_tag {
	int		fd;
	struct wq_tag	*next_tag;
} wq_tag;

int				Numplayers;
int				Numwaiting;
int				Deadchildren;
static wq_tag			*First_wait;
static int			Waitsock;
static fd_set			Sockets;
extern int 			errno;


/* local function prototypes */
char *inetname(struct in_addr in );




/**********************************************************************/
/*				sock_init			      */
/**********************************************************************/

/* This function initializes the socket that is used to accept new  */
/* connections on.						    */

void sock_init(unsigned short port )
{
	struct sockaddr_in 	addr;
	struct linger		ling;
	int 			n, i,t,connected=0;
	extern char		report;


	init_sockets();


#ifdef DEBUG
	FD_SET(0, &Sockets);
	FD_SET(1, &Sockets);
	FD_SET(2, &Sockets);
#endif

#ifndef WIN32
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTERM, quick_shutdown);
	signal(SIGCHLD, child_died);
	signal(SIGHUP,  quick_readcf);
	// signal(SIGUSR1, flush_crier);
#endif

	Tablesize = getdtablesize();
	if (Tablesize > PMAX)
	{
		Tablesize = PMAX;
		sprintf(g_buffer, "Reducing Tablesize to match PMAX of %d", Tablesize);
		slog(g_buffer);
	}

	Waitsock = socket(AF_INET, SOCK_STREAM, 0);
	if(Waitsock < 0)
	{
		sprintf(g_buffer, "Unable to create socket. Error: %d", errno);
		elog_broad(g_buffer);
		exit(-1);
	}

	FD_ZERO(&Sockets);
	FD_SET(Waitsock, &Sockets); 

	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;

	if (report)
	{
		i = 1;
		if(setsockopt(Waitsock, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(int)) <0) {
			sprintf(g_buffer, "Error in setsockopt, %d (%s)", errno, strerror(errno));
			elog_broad(g_buffer);
		}
	}

	n = bind(Waitsock, (struct sockaddr *) &addr, sizeof(addr));
	if(n < 0) {
		sprintf(g_buffer, "Unable to bind socket. Error: %d (%s)", errno, strerror(errno));
		elog_broad(g_buffer);
		
	if (report) {
		printf("Unable to bind socket.  Still trying..."); fflush(NULL);
		
		/* Keep trying for 4 minutes, if we are stuck in TIME_WAIT it could take
		 * this long (according to the RFC standard)  Linux seems to take at
		 * most 2 minutes */
		for(t=0;t<48;t++) {
			usleep(5000000); printf("."); fflush(NULL);
			if(bind(Waitsock, (struct sockaddr *) &addr, sizeof(addr))==0){
				connected=1;break;
			}
		}
		if(connected==0) {
			printf("Giving up.\n"); fflush(NULL);
			exit(-1);
		}
	} else  { /* no report */
		printf("Unable to bind socket, giving up. (try -r to keep trying)\n");
		fflush(NULL);
		exit(-1);
	}

	} /* if(n < 0) */
	

	ling.l_onoff = ling.l_linger = 0;
	setsockopt(Waitsock, SOL_SOCKET, SO_LINGER, (char *)&ling,
		   sizeof(struct linger));

	listen(Waitsock, 5);

#ifdef IRIX
	{
	int	flags;

	flags = fcntl(Waitsock, F_GETFL);
	flags |= O_NONBLOCK;
	fcntl(Waitsock, flags);
	}
#else
	ioctl(Waitsock, FIONBIO, &i);
#endif /* IRIX */
}


/**********************************************************************/
/*				sock_loop			      */
/**********************************************************************/

/* This function is the main loop of the entire program.  It constantly */
/* checks for new input, outputs players' output buffers, handles the   */
/* players' commands and updates the game.				*/

void sock_loop()
{
	while(1) {
#ifndef WIN32
		if(Deadchildren) 
#endif
			reap_children();
		io_check();
		output_buf();
		handle_commands();
		update_game();
	}
}

/**********************************************************************/
/*				io_check			      */
/**********************************************************************/

/* This function takes a look at all the sockets that are being used at */
/* the time, and determines which ones have input waiting on them.  The */
/* ones that do call accept_input to have their input buffers updated.  */
/* If the wait socket indicates input is ready to read, that means a    */
/* new connection to the game must be accepted. 			*/

int io_check()
{
	fd_set			sockcheck;
	short			rtn=0, i;
	static struct timeval	t = {0L, 75000L}; 

	sockcheck = Sockets;
	t.tv_sec = 0;	    
	t.tv_usec = 75000L;


 	if(select(Tablesize, &sockcheck, (fd_set *) 0, (fd_set *) 0, &t) > 0) { 
		for(i=0; i<Tablesize; i++) {
			if(FD_ISSET(i, &sockcheck)) {
				if(i != Waitsock)
					rtn |= accept_input(i);
				else
					accept_connect();
			}
		}
	}

	return(rtn);
}

/**********************************************************************/
/*				accept_connect			      */
/**********************************************************************/

/* This function accepts a new connection on the wait socket so that a  */
/* new player can begin playing.  The player's iobuf structure is init- */
/* ialized and a spot is marked in the player and socket arrays for the */
/* player.								*/

void accept_connect()
{
	int			len, fd, i=1, pid, xok;
	iobuf			*io;
	extra			*extr;
	struct linger		ling;
	struct sockaddr_in	addr;
	char *argv[5];

	char path[127], port1str[10], port2str[10];

	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORTNUM);
	addr.sin_addr.s_addr = INADDR_ANY;

	len = sizeof(struct sockaddr_in);
	fd = accept(Waitsock, (struct sockaddr *) &addr, &len);
	if (fd < 0) {
		strcpy(g_buffer, inetname(addr.sin_addr));
		elog_broad(g_buffer);
		sprintf(g_buffer, "%s", strerror(errno));
		elog_broad(g_buffer);
		merror("accept_connect", NONFATAL);
		return;
	}

	/* set the socket to non blocking */
	ioctl(fd, FIONBIO, &i);

	/* set socket to no linger i.e. close with no wait if disconnect */
	ling.l_onoff = ling.l_linger = 0;
	setsockopt(fd, SOL_SOCKET, SO_LINGER, (char *)&ling,
		   sizeof(struct linger));

	/* allocate buffers for input and output */
	io = (iobuf *)malloc(sizeof(iobuf));
	extr = (extra *)malloc(sizeof(extra));
	if(!io || !extr) 
	{
		if ( io )
			free( io );
		if ( extr )
			free( extr);
		
		merror("accept_connect", FATAL);
	}

	Ply[fd].io = io;
	Ply[fd].ply = 0;
	Ply[fd].extr = extr;
	Ply[fd].extr->loginattempts=0;

	zero(io, sizeof(iobuf));
	zero(extr, sizeof(extra));
	io->ltime = time(0);
	io->intrpt = 1;

	strcpy(io->address, inetname(addr.sin_addr));
	
        if(check_dos(fd)) {
                disconnect(fd);
                sprintf(g_buffer, "Possible DoS attack from %s.\n", inetname(addr.sin_addr))
;
                elog_broad(g_buffer);
                return;
	}

#ifndef WIN32
	sprintf(path, "%s/auth", get_bin_path());
#else
	sprintf(path, "%s/auth.exe", get_bin_path());
#endif

/* No idea if this is portable... */
#ifndef WIN32
  xok = access(path, X_OK);
  if (!xok) {
#endif
	sprintf(port1str, "%u", ntohs(addr.sin_port));
	sprintf(port2str, "%u", PORTNUM);

	argv[0] = path;
	argv[1] = io->address;
	argv[2] = port1str;
	argv[3] = port2str;
	argv[4] = NULL;

	pid = run_child(fd, argv);
	if ( pid )
	{
		io->lookup_pid = pid;
	}
#ifndef WIN32
  }
#endif

	FD_SET(fd, &Sockets); 


	strcpy(io->userid, "unknown");

	if(Numplayers > Tablesize-2) {
		output(fd, "Game full.  Try again later.\n");
		disconnect(fd);
		return;
	}

	else if(Numplayers >= MAXPLAYERS &&
		((unsigned)(ntohl(saddr))>>24) != 127) {
		if(Numwaiting > MAXPLAYERS) {
			scwrite(fd, "Queue full.\n\r", 13);
			disconnect(fd);
			return;
		}
		add_wait(fd);
		RETURN(fd, waiting, 1);
	}

	init_connect(fd);
}

/************************************************************************/
/*				init_connect				*/
/************************************************************************/

/* This function sets up the player using the fd'th input socket.	*/

void init_connect( int fd )
{
	int		i;

	/* Put your MUD name here */
	output(fd,"\n");
	output(fd, title);

	display_credits(fd);

	Ply[fd].io->intrpt |= 2;
	Numplayers++;

	if((i = locked_out(fd)) == 2) {
		output(fd, "\nA password is required to play from that site.");
		output(fd, "\nPlease enter site password: ");
		output_ply_buf(fd);
        Ply[fd].io->intrpt &= ~1; 
		RETURN(fd, login, 0);
	}
	else if(i) {
		disconnect(fd);
		return;
	}

	Ply[fd].io->ltime = time(0);
	output(fd, "\n\nPlease enter name: ");
	output_ply_buf(fd);
    Ply[fd].io->intrpt &= ~1; 
	RETURN(fd, login, 1);

}

/************************************************************************/
/*				locked_out				*/
/************************************************************************/

/* This function determines if the player on socket number, fd, is on	*/
/* a site that is being locked out.  If the site is password locked,	*/
/* then 2 is returned.  If it's completely locked, 1 is returned.  If	*/
/* it's not locked out at all, 0 is returned.				*/

int locked_out( int fd )
{
	int	i;

	for(i=0; i<Numlockedout; i++) {
		if(!addr_equal(Lockout[i].address, Ply[fd].io->address))
			continue;

		if(!strcmp(Lockout[i].userid, "all")) {
		   if(Lockout[i].password[0]) {
			strcpy(Ply[fd].extr->tempstr[0], Lockout[i].password);
			return(2);
		   }
		   else {
			scwrite(fd, "\n\rYour site is locked out.\n\r", 28);
			sprintf(g_buffer, "\n\rSend questions to %s.\n\r", questions_to_email);
			output(fd, g_buffer);
			sprintf(g_buffer, "%s disconnected due to lockout.", Ply[fd].io->address);
                        slog( g_buffer );
			return(1);
		   }
		}
	}

	return 0;
}

/************************************************************************/
/*				addr_equal				*/
/************************************************************************/

/* This function determines if two internet addresses are equal and	*/
/* allows for wild-cards.						*/

int addr_equal(char	*str1, char *str2 )
{

	while(*str1 && *str2) {
		if(*str1 == '*') {
			while(*str2 != '.' && *str2) str2++;
			str1++;
			continue;
		}
		else if(*str1 != *str2)
			return(0);
		str1++; str2++;
	}

	if(!*str1 && !*str2) return(1);
	else return(0);
}

/**********************************************************************/
/*				accept_input			      */
/**********************************************************************/

/* This function is called when a player's socket indicates that there */
/* is input waiting.  The socket is read from, and the input is copied */
/* into that player's input buffer.  If the last character entered is  */
/* a carriage return, then the player's interrupt flag is set high.    */

int accept_input( int fd )
{
	char 	lastchar;
	unsigned char buf[128];
	int 	i, n, prev, itail, ihead;

	n = scread(fd, buf, 127);

	if(n<=0) {
		Ply[fd].io->commands = -1;	/* Connection dropped */
		disconnect(fd);
	}
	else {
		ihead = Ply[fd].io->ihead;
		lastchar = 0;
		itail = Ply[fd].io->itail;
		for(i=0; i<n; i++) {
		   if((buf[i] > 31 && buf[i] < 253)|| (buf[i] == '\n' && lastchar != '\r') 
			|| buf[i] == '\r' || buf[i] == '\b') 
		   {
				lastchar = buf[i];
				if(buf[i] == '\r') 
					buf[i] = '\n';
				if(buf[i] == '\n') 
				{
					Ply[fd].io->commands++;
					/* been waiting for an answer to a prompt */
					/* we must have gotten it */
					Ply[fd].io->intrpt |= 1;
				}

				else if(buf[i] == '\b' && ihead != itail) 
				{
					prev = ihead-1 < 0 ? IBUFSIZE-1:ihead-1;
					ihead--;
					if(ihead < 0)
						ihead = IBUFSIZE + ihead;
					continue;
				}
				else if(buf[i] == '\b') 
					continue;
				Ply[fd].io->input[ihead] = buf[i];
				ihead = (ihead + 1) % IBUFSIZE;
				if(ihead == itail)
					itail = (itail + 1) % IBUFSIZE;
		   }
		}
		Ply[fd].io->ihead = ihead;
		Ply[fd].io->itail = itail;
		Ply[fd].io->ltime = time(0);

		if(buf[n-1] == '\n' || buf[n-1] == '\r')
			Ply[fd].io->intrpt |= 1;
		else
			Ply[fd].io->intrpt &= ~1;

	}

	return(0);
}

/**********************************************************************/
/*				output_buf			      */
/**********************************************************************/
/* This function outputs the contents of all players' buffers when that  */
/* player is able to be interrupted, or when that player's output buffer */
/* has reached a specific high-water mark (75% of buffer size).          */
void output_buf()
{
	fd_set			sockcheck;
	short			i;
	static struct timeval	t = {0L, 75000L}; 

	sockcheck = Sockets;
	t.tv_sec = 0;	    
	t.tv_usec = 75000L;


#ifdef DEBUG
	FD_CLR(0, &sockcheck);
	FD_CLR(1, &sockcheck);
	FD_CLR(2, &sockcheck);
#endif

 	if(select(Tablesize, (fd_set *) 0, &sockcheck, (fd_set *) 0, &t) > 0) { 
		for(i=0; i<Tablesize; i++) {
			if(FD_ISSET(i, &sockcheck) && Ply[i].io) {
				/* is the player disconnecting */
				if(Ply[i].io->commands == -1) {
					disconnect(i);
					continue;
				}
				/* is there anythign to write */
				if ( Ply[i].io->ohead == Ply[i].io->otail )
					continue;
				/* can the player be interupted? */
				/* or is his buffer getting too close to ovrflowing? */
				if( Ply[i].io->intrpt & 1 
					|| buflen(Ply[i].io->ohead, Ply[i].io->otail, OBUFSIZE) 
					> (OBUFSIZE*3)/4) 
				{
					/* then do the output */
					output_ply_buf(i);
				}
			}
		}
	}

	return;
}



/**********************************************************************/
/*				output_ply_buf			      */
/**********************************************************************/
/* This function outputs the contents of a single players' buffers */
/* if the player is able to be interrupted */
void output_ply_buf(int fd)
{
	int 	n;
	int	otail, ohead;

	if(FD_ISSET(fd, &Sockets) && Ply[fd].io) 
	{
		otail = Ply[fd].io->otail;
		ohead = Ply[fd].io->ohead;

		if(Ply[fd].io->commands == -1) 
		{
			disconnect(fd);
			return;
		}

		if(ohead == otail)
			return;


		if(Ply[fd].io->intrpt & 1) {
			if(Ply[fd].ply)
				if(Ply[fd].ply->fd > -1 && 
				   F_ISSET(Ply[fd].ply, PNOCMP)) 
				{
					n = spy_write(fd, "\n\r", 2);
				}
			n = spy_write(fd, &Ply[fd].io->output[otail], 
				ohead>otail ? ohead-otail:OBUFSIZE-otail);
			if(otail > ohead) {
				n+= spy_write(fd, Ply[fd].io->output, ohead );
			}

			otail = ohead;
			Ply[fd].io->otail = otail;
			if(Ply[fd].ply) 
			{
				ply_prompt(Ply[fd].ply);
			}
		}
	}

	return;
}


/**********************************************************************/
/*				handle_commands			      */
/**********************************************************************/

/* This function strips out the first command in each player's input */
/* buffer, and then sends that command to the player's next function */
/* of input with the appropriate parameter.			     */

void handle_commands()
{
	int	i, j;
	int	itail, ihead;
	char	buf[IBUFSIZE+1];

	for(i=0; i<Tablesize; i++) {
		if(FD_ISSET(i, &Sockets) && Ply[i].io) {
			if(Ply[i].io->commands == -1) {
				disconnect(i);
				continue;
			}
			if(!Ply[i].io->commands) 
				continue;
			itail = Ply[i].io->itail;
			ihead = Ply[i].io->ihead;
			if(itail == ihead) 
				continue;
			for(j=0; j<IBUFSIZE; j++) {
				if(itail == ihead) {
					buf[j] = 0;
					break;
				}
				if(Ply[i].io->input[itail] == 13 ||
				   Ply[i].io->input[itail] == 10) {
					itail = (itail + 1) % IBUFSIZE;
					buf[j] = 0;
					break;
				}
				buf[j] = Ply[i].io->input[itail];
				itail = (itail + 1) % IBUFSIZE;
			}
			Ply[i].io->itail = itail;
			Ply[i].io->commands--;
			if(get_spy(i) > -1) {
				scwrite(get_spy(i), buf, strlen(buf));
				scwrite(get_spy(i), "\n\r", 2);
			}
			(*Ply[i].io->fn) (i, Ply[i].io->fnparam, buf);
		}
	}
}

/**********************************************************************/
/*				disconnect			      */
/**********************************************************************/

/* This function drops the connection to the player on the socket specified */
/* by the first parameter, clears his spot in the socket bit-array, and     */
/* removes him from the player array by freeing all memory taken by him.    */

void disconnect(int fd )
{
	int 	i;
	etag	*ign, *temp;
	wq_tag	*wq;
	ctag	*cst, *crm, *ctemp;

	scclose( fd );

	FD_CLR(fd, &Sockets);

	set_spy(fd, -1);

	if(Ply[fd].io) {
		if(Ply[fd].io->intrpt & 2)
			Numplayers--;
		free(Ply[fd].io);
		Ply[fd].io = 0;
	}
	if(Ply[fd].extr) {
		/* clear and free ignore list */
		ign = Ply[fd].extr->first_ignore;
		while(ign) {
			temp = ign;
			ign = ign->next_tag;
			free(temp);
		}
		/* clear and free charm list */
		crm = Ply[fd].extr->first_charm;
		while(crm) {
			ctemp = crm;
			crm = crm->next_tag;
			free(ctemp);
		}
		/* clear dm_alias */
		if(Ply[fd].extr->alias_crt) { 
			F_CLR(Ply[fd].extr->alias_crt, MDMFOL);
			Ply[fd].extr->alias_crt = 0;
		}
		/* clear and free list of players stolen from */
		cst = Ply[fd].extr->first_stolen;
		while(cst) {
			ctemp = cst;
			cst = cst->next_tag;
			free(ctemp);
		}
		free(Ply[fd].extr);
		Ply[fd].extr = 0;
			
	}
	if(Ply[fd].ply) {
		if(F_ISSET(Ply[fd].ply, PSPYON)) {
			for(i=0; i<Tablesize; i++)
				if(get_spy(i) == fd) 
					set_spy(i, -1);
			F_CLR(Ply[fd].ply, PSPYON);
		}
		if(Ply[fd].ply->fd > -1) {
			uninit_ply(Ply[fd].ply);
			save_ply(Ply[fd].ply);
		}
		free_crt(Ply[fd].ply);
		Ply[fd].ply = 0;
	}
	else {
		for(wq = First_wait, i=1; wq; wq = wq->next_tag, i++)
			if(wq->fd == fd) {
				remove_wait(i);
				break;
			}
	}
	if(Numwaiting && Numplayers < MAXPLAYERS) {
		i = remove_wait(1);
		if ( i != -1)
		{
			sprintf(g_buffer, "%c", 7);
			output(i, g_buffer);
			init_connect(i);
		}
	}
}

/**********************************************************************/
/*				broadcast			      */
/**********************************************************************/

/* This function broadcasts a message to all the players that are in the */
/* game.  If they have the NO-BROADCAST flag set, then they will not see */
/* it.									 */

void broadcast(char	*msg )
{
	char	str[1024];
	int	i;

	strcpy(str, msg);
	strcat(str, "\n");
	for(i=0; i<Tablesize; i++) {
		if(FD_ISSET(i, &Sockets) && Ply[i].ply)
			if(!F_ISSET(Ply[i].ply, PNOBRD) && Ply[i].ply->fd > -1)
				output(i, str);
	}
}


/**********************************************************************/
/*				broadcast_c			      */
/**********************************************************************/

/* This function broadcasts a message to all the players that are in the */
/* game.  If they have the NO-BROADCAST flag set, then they will not see */
/* it.									 */
void broadcast_c(char *msg, creature *crt_ptr )
{
	char	str[2048];
	int	i;

	strcpy(str, msg);
	strcat(str, "\n");
	for(i=0; i<Tablesize; i++) {
		if(FD_ISSET(i, &Sockets) && Ply[i].ply)
			if(!F_ISSET(Ply[i].ply, PNOBRD) && Ply[i].ply->fd > -1)
			{
				mprint(i, str, m1arg(crt_ptr) );
			}
	}
}



/**********************************************************************/
/*                             broadcast_wc */
/**********************************************************************/
/* This function broadcasts a message to all the players that are in the */
/* game.  If they have the NO-BROADCAST flag set, then they will not see */
/* it. Color specified as first param.  */
void broadcast_wc(int color, char *msg)
{

	char	str[2048];
	int	i;

	strcpy(str, msg);
	strcat(str, "\n");
	for(i=0; i<Tablesize; i++) {
		if(FD_ISSET(i, &Sockets) && Ply[i].ply)
			if(!F_ISSET(Ply[i].ply, PNOBRD) && Ply[i].ply->fd > -1)
			{
				ANSI(i, color);
				output(i, str);
				ANSI(i, MAINTEXTCOLOR);
			}
	}

	return;
}


/**********************************************************************/
/*                              broadcast_login                       */
/**********************************************************************/

/* This function broadcasts a message to all the players that are in the */
/* game.  If they have the NO-BROADCAST flag set, then they will not see */
/* it.																	 */

void broadcast_login( creature *ply_ptr, int login )
{
    char    fmt2[2048];
    int     i;

	ASSERTLOG( ply_ptr );

	if(!F_ISSET(ply_ptr, PDMINV)) {
		if ( login ) {
			sprintf( fmt2, "### %s the %s %s just logged in.", ply_ptr->name,
			get_race_adjective(ply_ptr->race), title_ply(ply_ptr));
		}
		else {
			sprintf(fmt2, "### %s just logged off.", ply_ptr->name);
		}

        for(i=0; i < Tablesize; i++) {
			if(FD_ISSET(i, &Sockets) && Ply[i].ply) {
				if(Ply[i].ply->fd > -1 && !F_ISSET(Ply[i].ply, PNLOGN) ) {
					output_nl(i, fmt2 );
				}
			}
		}
	}

	return;
}

/**********************************************************************/
/*				broadcast_wiz			      */
/**********************************************************************/

/* This function broadcasts a message to all the DM's who are on at the */
/* time.								*/

void broadcast_wiz(creature *ply_ptr, char *str )
{

	char	fmt2[2048];
	int	i;

	ASSERTLOG( ply_ptr );
	ASSERTLOG( str );

	sprintf( fmt2, "=> %s sent, \"%s\".", ply_ptr->name, str);
	strcat(fmt2, "\n");

	for(i=0; i<Tablesize; i++) {
		if(FD_ISSET(i, &Sockets) && Ply[i].ply) {
			if(Ply[i].ply->fd > -1 && Ply[i].ply->class >= CARETAKER && !F_ISSET(Ply[i].ply, PNOBRD) ) {
				output_wc(i, fmt2, BROADCASTCOLOR );
			}
		}
	}

	return;
}

/**********************************************************************/
/*				broadcast_eaves			      */
/**********************************************************************/

/* This function broadcasts a message to all the DM's who are on at the */
/* time and have the eavesdropping flag set.			 	*/

void broadcast_eaves(char *str)
{
	char	fmt2[2048];
	int	i;

	strcpy(fmt2, str);
	for(i=0; i<Tablesize; i++) {
		if(FD_ISSET(i, &Sockets) && Ply[i].ply)
			if(Ply[i].ply->fd > -1 && Ply[i].ply->class >= BUILDER 
				&& F_ISSET(Ply[i].ply, PEAVES))
				output_nl(i, fmt2);
	}
}



/**********************************************************************/
/*				broadcast_rom			      */
/**********************************************************************/

/* This function outputs a message to everyone in the room specified */
/* by the integer in the second parameter.  If the first parameter   */
/* is greater than -1, then if the player specified by that file     */
/* descriptor is present in the room, he is not given the message    */

void broadcast_rom(int ignore, int rm, char *fmt, void *args[] )
{
	char	*fmt2;
	int	i;

	fmt2 = malloc( strlen(fmt) + 3 );
	if ( fmt2 )
	{
		strcpy(fmt2, fmt);
		strcat(fmt2, "\n");
		for(i=0; i<Tablesize; i++) 
		{
			if(FD_ISSET(i, &Sockets) && Ply[i].ply)
			{
				if(Ply[i].ply->rom_num == rm && Ply[i].ply->fd > -1
					&& i != ignore )
				{
					mprint(i, fmt2, args);
				}
			}
		}

		/* dont forget to free it */
		free(fmt2);
	}
}

/**********************************************************************/
/*				broadcast_rom2			      */
/**********************************************************************/

/* This function is the same as broadcast_rom except that it will ignore */
/* two people in a room.						 */

void broadcast_rom2(int	ignore1, int ignore2, int rm, char *fmt, void *args[] )
{
	char	*fmt2;
	int	i;

	fmt2 = malloc( strlen(fmt) + 3 );
	if ( fmt2 )
	{
		strcpy(fmt2, fmt);
		strcat(fmt2, "\n");
		for(i=0; i<Tablesize; i++) 
		{
			if(FD_ISSET(i, &Sockets) && Ply[i].ply)
			{
				if(Ply[i].ply->rom_num == rm && Ply[i].ply->fd > -1
				   && i != ignore1 && i != ignore2)
				{
					mprint(i, fmt2, args);
				}
			}
		}

		/* dont forget to free it */
		free( fmt2 );
	}
}


/****************************************************************************/
/*			broadcast_dam					    */
/****************************************************************************/

/* This function is the same as broadcast_rom2 except that it will ignore */
/* any player who has their BRIEF flag set.  IT is used to output damage
   descriptions */                                                       

void broadcast_dam(int ignore1, int ignore2, int rm, char *fmt, void *args[] )
{
        char    *fmt2;
        int     i;

        fmt2 = malloc( strlen(fmt) + 3 );
        if ( fmt2 )
        {       
                strcpy(fmt2, fmt);
                strcat(fmt2, "\n");
                for(i=0; i<Tablesize; i++)
                {
                        if(FD_ISSET(i, &Sockets) && Ply[i].ply)
                        {  
                                if(Ply[i].ply->rom_num == rm && Ply[i].ply->fd > -1
                                   && i != ignore1 && i != ignore2 && !F_ISSET(Ply[i].ply, PBRIEF))
                                {
                                        output_wc(i, fmt2, AFC_RED);
                                }
                        } 
                }
        
                /* dont forget to free it */
                free( fmt2 );
        }
}

/**********************************************************************/
/*				broadcast_class									      */
/**********************************************************************/

/* This function broadcasts a message to all the same class on at the */
/* time.								*/

void broadcast_class(creature *ply_ptr, char *str)
{
	char	fmt2[1024];
	int	i;

	strcpy(fmt2, "(");
	strcat(fmt2, get_class_string(ply_ptr->class));
	strcat(fmt2, ") ### ");	
	strcat(fmt2, str);
	strcat(fmt2, "\n");
	for(i=0; i<Tablesize; i++) {
		if(FD_ISSET(i, &Sockets) && Ply[i].ply) {
			if(!F_ISSET(Ply[i].ply,PIGCLA) && Ply[i].ply->fd > -1 && Ply[i].ply->class == ply_ptr->class && !F_ISSET(Ply[i].ply, PNOBRD))
			{
				output_wc(i, fmt2, BROADCASTCOLOR ); 
			}
			else if(!F_ISSET(Ply[i].ply,PIGCLA) && Ply[i].ply->fd > -1 && Ply[i].ply->class > BUILDER &&
				ply_ptr->class == BUILDER && !F_ISSET(Ply[i].ply, PNOBRD))
			{
				output_wc(i, fmt2, BROADCASTCOLOR ); 
			}
			else if(!F_ISSET(Ply[i].ply,PIGCLA) && Ply[i].ply->fd > -1 && Ply[i].ply->class > CARETAKER &&
				ply_ptr->class == CARETAKER && !F_ISSET(Ply[i].ply, PNOBRD))
			{
				output_wc(i, fmt2, BROADCASTCOLOR ); 
			}
		}

	}
}
/**********************************************************************/
/*				broadcast_guild			      */
/**********************************************************************/

/* This function broadcasts a message to all the same class on at the */
/* time.								*/

void broadcast_guild(creature *ply_ptr, char *str)
{
	char	fmt2[1024];
	int	i;

	strcpy(fmt2, "(");
	strcat(fmt2, cur_guilds[check_guild(ply_ptr)].name );
	strcat(fmt2, ") ### ");	
	strcat(fmt2, str);
	strcat(fmt2, "\n");
	for(i=0; i<Tablesize; i++) {
		if(FD_ISSET(i, &Sockets) && Ply[i].ply) {
			if(!F_ISSET(Ply[i].ply,PIGGLD) && Ply[i].ply->fd > -1 && 
				check_guild(Ply[i].ply) == check_guild(ply_ptr) && !F_ISSET(Ply[i].ply, PNOBRD))
			{
				output_wc(i, fmt2, BROADCASTCOLOR ); 
			}
			else if(!F_ISSET(Ply[i].ply,PIGGLD) && Ply[i].ply->fd > -1 && Ply[i].ply->class > BUILDER &&
				ply_ptr->class == BUILDER && !F_ISSET(Ply[i].ply, PNOBRD))
			{
				output_wc(i, fmt2, BROADCASTCOLOR ); 
			}
			else if(!F_ISSET(Ply[i].ply,PIGGLD) && Ply[i].ply->fd > -1 && Ply[i].ply->class > CARETAKER &&
				ply_ptr->class == CARETAKER && !F_ISSET(Ply[i].ply, PNOBRD))
			{
				output_wc(i, fmt2, BROADCASTCOLOR ); 
			}
		}
	}
}


/************************************************************************/
/*	elog_broad							*/
/*  logs an error message with elog but also broadcasts the error	*/
/*  to all dms currently logged in					*/
/************************************************************************/
void elog_broad( char *str )
{
	int	i;

	ASSERTLOG(str);

	for(i=0; i<Tablesize; i++) 
	{
		if(FD_ISSET(i, &Sockets) && Ply[i].ply)
		{
			if(Ply[i].ply->fd > -1 && Ply[i].ply->class == DM)
			{
				output(i, "\n\n"); 
				output_wc(i, str, ERRORCOLOR ); 
				output(i, "\n\n"); 
			}
		}
	}

	elog(str);

	return;
}


/************************************************************************/
/*				inetname				*/
/************************************************************************/

/* This function returns the internet address of the address structure	*/
/* passed in as the first parameter.					*/

char *inetname(struct in_addr in )
{

	char *cp=0;
	static char line[50];
	static char domain[81];
	static int first = 1;
	struct hostent *hp;


	if(GETHOSTBYNAME) {
		if (first) {
			first = 0;
			if (gethostname(domain, 80) == 0 && (cp = index(domain, '.')))
				(void) strcpy(domain, cp + 1);
			else
				domain[0] = 0;
		}
		cp = 0;
		if (in.s_addr != INADDR_ANY) {
#ifndef WIN32
#ifndef CYGWIN
			int net, lna;
			struct netent *np;

			net = inet_netof(in);
			lna = inet_lnaof(in);

			if (lna == INADDR_ANY) {
				np = getnetbyaddr(net, AF_INET);
				if (np)
					cp = np->n_name;
			}
#endif
#endif
			if (cp == 0) {
				hp = gethostbyaddr((char *)&in, sizeof (in), AF_INET);
				if (hp) {
					if ((cp = index(hp->h_name, '.')) && !strcmp(cp + 1, domain))
						*cp = 0;
					cp = hp->h_name;
				}
			}
		}

	}


	if (in.s_addr == INADDR_ANY)
		strcpy(line, "*");
	else if (cp)
		strcpy(line, cp);
	else {
		in.s_addr = ntohl(in.s_addr);
#define C(x)	((x) & 0xff)
#ifdef HPUX
		sprintf(line, "%lu.%lu.%lu.%lu", C(in.s_addr >> 24),
			C(in.s_addr >> 16), C(in.s_addr >> 8), C(in.s_addr));
#else
		sprintf(line, "%u.%u.%u.%u", C(in.s_addr >> 24),
			C(in.s_addr >> 16), C(in.s_addr >> 8), C(in.s_addr));
#endif
	}
	return (line);

}


/************************************************************************/
/*				add_wait				*/
/************************************************************************/

/* This function adds the descriptor in the first parameter to the	*/
/* waiting queue.							*/

void add_wait(int fd )
{
	wq_tag	*new_wq, *wq;

	new_wq = (wq_tag *)malloc(sizeof(wq_tag));
	new_wq->next_tag = 0;
	new_wq->fd = fd;

	if(!First_wait) {
		First_wait = new_wq;
		Numwaiting = 1;
	}
	else {
		wq = First_wait;
		while(wq->next_tag)
			wq = wq->next_tag;
		wq->next_tag = new_wq;
		Numwaiting++;
	}
	sprintf(g_buffer, "The game is full.\nYou are player #%d in the waiting queue.\n", Numwaiting);
	output(fd, g_buffer);
}

/************************************************************************/
/*				remove_wait				*/
/************************************************************************/

/* This function removes the i'th player from the waiting queue.	*/

int remove_wait( int i )
{
	int	j, fd;
	wq_tag	*wq, *prev;

	wq = First_wait;

/* write is being used here to check to see if the given
  file desc. is still valid.  Theorically, only one write is
  needed, but for some reason 2 writes are needed */
   scwrite(wq->fd," ",1 );
        if(i == 1) {
		if (scwrite(wq->fd,"\n",1 ) == -1){
                fd = -1;
		}
                else
                fd = wq->fd;
                First_wait = wq->next_tag;
        }
	else {
		for(j=1; j<i; j++) {
			prev = wq;
			wq = wq->next_tag;
		}
		fd = wq->fd;
		prev->next_tag = wq->next_tag;
	}

	free(wq);
	Numwaiting--;

	for(wq=First_wait, j=1; wq; wq=wq->next_tag, j++)
		if(j >= i) 
		{
			sprintf(g_buffer, "You are player #%d in the waiting queue.\n", j);
			output(wq->fd, g_buffer);
		}

	output_ply_buf(fd);
	return(fd);
}

void waiting(int fd, int param, char *str )
{
	RETURN(fd, waiting, 1);
}

/************************************************************************/
/*				child_died				*/
/************************************************************************/

/* This function gets called when a SIGCHLD signal is sent to the	*/
/* program.								*/

void child_died()
{
	Deadchildren++;

}
/***********************************************************************
	
*	This causes the game to shutdown in 20 seconds.  It is used
*  by signal to force a  shutdown in response to a TERM signal 
*  (i.e. kill -TERM pid) from the system.
*/
void quick_shutdown()
{
        Shutdown.ltime = time(0);   
        Shutdown.interval = 20;
}

/***********************************************************************/
/*									*/
/*  This causes the configuration file to be reread.			*/
/***********************************************************************/	
void quick_readcf()
{
	readcf();
}

/************************************************************************/
/*				reap_children				*/
/************************************************************************/

/* This program goes through and kills off (waits for) each child	*/
/* that has completed processing to avoid zombie processes.		*/
/* If the child was an authentication process, then the information	*/
/* it returned is looked up in the file it saved.			*/

#ifndef WIN32
void reap_children()
{
	int pid, i, found, status, proxies;
	char filename[256], userid[80], address[80];
	FILE *fp;

	while(Deadchildren > 0) {
		Deadchildren--;
		found = -1;
		pid = wait(&status);
		sprintf(filename, "%s/auth/lookup.%d", get_log_path(), pid);
		for(i=0; i<Tablesize; i++) {
			if(Ply[i].io && Ply[i].io->lookup_pid == pid) {
				found = i;
				break;
			}
		}
		if(found < 0) {
			if(file_exists(filename)) unlink(filename);
			continue;
		}
		fp = fopen(filename, "r");
		if(!fp) 
			continue;
		fscanf(fp, "%s %s %d", userid, address, &proxies);
		if(strlen(userid))
			userid[8] = 0;
		if(strlen(address))
			address[39] = 0;
		fclose(fp);
		unlink(filename);
		strcpy(Ply[found].io->userid, userid);
		if(proxies) {
			sprintf(g_buffer, "%s@%s (%s@%s) disconnected due to open proxy.",
			userid, address, userid, Ply[found].io->address);
			slog( g_buffer );
			scwrite(found, "You are not allowed to connect.",31);
			disconnect(found);
			continue;
		}
		sprintf(g_buffer, "%s@%s (%s@%s) connected.",
			userid, address, userid, Ply[found].io->address);
		plog( g_buffer );
		if(strcmp(address, "UNKNOWN"))
			strcpy(Ply[found].io->address, address);
	}

	/* just in case, kill off any zombies */	
	wait3(&status, WNOHANG, (struct rusage *)0);
	/* Try this wait if wait3 doesnt work */
	/*wait4(-1, &status, WNOHANG, (struct rusage *)0); */
}

#else

void reap_children()
{
	int i;
	char filename[256], userid[80], address[80];
	FILE *fp;

	for(i=0; i<Tablesize; i++) {
		if(Ply[i].io &&  Ply[i].io->lookup_pid != 0 )
		{
			DWORD	dwReturn;
			HANDLE	hProcess;

			hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE,  
				Ply[i].io->lookup_pid);

			if ( hProcess )
			{
				GetExitCodeProcess(hProcess, &dwReturn);

				if ( dwReturn != STILL_ACTIVE )
				{
					sprintf(filename, "%s/auth/lookup.%d", get_log_path(), 
						Ply[i].io->lookup_pid);

					fp = fopen(filename, "r");
					if(!fp) 
					{
						/* need to close after calling OpenProcess(); */
						CloseHandle(hProcess);
						continue;
					}

					fscanf(fp, "%s %s", userid, address);
					if(strlen(userid))
						userid[8] = 0;
					if(strlen(address))
						address[39] = 0;

					fclose(fp);
					unlink(filename);

					strcpy(Ply[i].io->userid, userid);

					sprintf(g_buffer, "%s@%s (%s@%s) connected.", 
						userid, address, userid, Ply[i].io->address);
					plog( g_buffer );

					if(strcmp(address, "UNKNOWN"))
						strcpy(Ply[i].io->address, address);

					/* dont need to look him up anymore */
					Ply[i].io->lookup_pid = 0;
					
				}

				/* need to close after calling OpenProcess(); */
				CloseHandle(hProcess);
			}
		}
	}

	return;
}
#endif



 

/* Doneval: Here a little function to "ask" somthing with ANSI codes. */
void ask_for(int fd, char *str)
{
    char    tempstr[80];
	
	ASSERTLOG( str );

	output(fd, "\n");

	if ( ANSILINE ) {
		gotoxy(fd, 24, 1);
	}

	output( fd, str );


	if ( ANSILINE ) {
		sprintf(tempstr, "%c[24;%dH", 27, (int) strlen(str));
		scwrite(fd, tempstr, strlen(tempstr));
	}

/*
        if(F_ISSET(Ply[fd].ply, PANSIC)) 
		{
			sprintf(str, "%c[24;%dH", 27, strlen(fmt));
			output(fd, str);
		}
*/
}

/**********************************************************************/
/*                      disconnect_all_ply                            */
/**********************************************************************/

/* This function disconnects all players.                             */
void disconnect_all_ply()
{
        int i;

        for(i=0; i<Tablesize; i++) {
                if(Ply[i].ply && Ply[i].io && Ply[i].ply->name[0])
                        disconnect(Ply[i].ply->fd);
        }
}



#ifdef JUNK

/**********************************************************************/
/*				print				      */
/**********************************************************************/

/* This function acts just like printf, except it outputs the     */
/* formatted text string to a given socket's output buffer.  The  */
/* socket number is the first parameter.			  */
/* now formatting only is done here then passed to output */

void print(int fd, char *fmt, int i1, int i2, int i3, int i4, int i5, int i6 )
{
	char 	msg[4096];
	char	*fmt2;
	int	i = 0, j = 0, k; 
	int	num, loc, ind = -1, len, flags = 0;
	int		arg[6];
	char	type;

	if(fd < 0 || fd > Tablesize)
		return;
	if(!Ply[fd].io)
		return;


	len = strlen(fmt);
	fmt2 = (char *)malloc(len+1);
	if(!fmt2)
		merror("print", FATAL);

	arg[0] = i1; arg[1] = i2; arg[2] = i3; 
	arg[3] = i4; arg[4] = i5; arg[5] = i6;

	/* Check for %m, %M, %i and %I and modify arguments as necessary */
	do {
		if(fmt[i] == '%') {
			fmt2[j++] = fmt[i];
			num = 0; k = i;
			do {
				k++;
				if((fmt[k] >= 'a' && fmt[k] <= 'z') ||
				   (fmt[k] >= 'A' && fmt[k] <= 'Z') ||
				   fmt[k] == '%') {
					loc = k;
					type = fmt[k];
					break;
				}
				else if(fmt[k] >= '0' && fmt[k] <= '9')
					num = num*10 + fmt[k]-'0';
			} while(k < len);
			if(type == '%') {
				fmt2[j++] = '%';
				i++; i++;
				continue;
			}
			ind++;
			if(type != 'm' && type != 'M' &&
			   type != 'i' && type != 'I') {
				i++;
				continue;
			}

			i = loc + 1;
			fmt2[j++] = 's';

			switch(type) {
			case 'm':
				arg[ind] = (int)crt_str(Ply[fd].ply, (creature *)arg[ind], num, 0);
				continue;
			case 'M':
				arg[ind] = (int)crt_str(Ply[fd].ply, (creature *)arg[ind], num, CAP);
				continue;
			case 'i':
				arg[ind] = (int)obj_str(Ply[fd].ply, (object *)arg[ind], num, 0);
				continue;
			case 'I':
				arg[ind] = (int)obj_str(Ply[fd].ply, (object *)arg[ind], num, CAP);
				continue;
			}
		}
		fmt2[j++] = fmt[i++];
	} while (i < len);

	fmt2[j] = 0;

	sprintf(msg, fmt2, arg[0], arg[1], arg[2], arg[3], arg[4], arg[5]);
	free(fmt2);

	output( fd, msg );

	return;
}

#endif


/********************************************************************/
/*	msprint()						    */
/*								    */
/* This function acts just like printf, except it outputs the	    */
/* formatted text string to a string.				    */
/* viewpoint -  The char that is veiwing this			    */
/* str - the buffer to return the formatted string in		    */
/* fmt - the format string ( only accepts variations of %m and %i   */
/* args - an array of pointers to the creatures and objects that    */
/*    are used in replcaement.					    */
/*								    */
/* NOTE: this does not do ANY sprintf replacements anymore	    */
/********************************************************************/
void msprint(creature *viewpoint, char *buffer, char *fmt, void *args[] )
{
	char 	msg[4096];
	int	i = 0; 
	int	num = 0;
	int	ind = 0;
	char *p;
	char *p2;
	char *temp;
	int	count;

	count = 0;
	if ( args != NULL )
	{
		while ( args[count] != NULL)
			count++;
	}

	if(fmt == NULL || buffer == NULL)
		return;

	/* start with an empty string */
	msg[0] = '\0';

	p = fmt;
	while (*p)
	{
		/* look for substitution */
		if ( *p == '%' && ind < count )
		{
			num = 0;

			/* switch to p2 in case we have to back out */
			p2 = p;
			p2++;

			/* now look for a count */
			while ( *p2 && isdigit((int)*p2) )
			{
				num = num * 10 + *p2 - '0';
				p2++;
			}
			temp = NULL;

			if ( *p2 )
			{
				switch ( *p2 )
				{
					case 'm':
						temp = crt_str(viewpoint, (creature *)args[ind++], num, 0);
						break;
					case 'M':
						temp = crt_str(viewpoint, (creature *)args[ind++], num, CAP);
						break;
					case 'i':
						temp = obj_str(viewpoint, (object *)args[ind++], num, 0);
						break;
					case 'I':
						temp = obj_str(viewpoint, (object *)args[ind++], num, CAP);
						break;
					default:
						/* we got a problem here, not valid replacement specifier */
						/* we have to bail and copy all the chars we just skipped */
						temp = NULL;
						break;
				}
			}

			if ( temp )
			{
				strcpy(&msg[i], temp );
				i += strlen(temp);
				/* no backing out needed, found a valid replacement */
				p = p2;
				p++;
				continue;
			}
		}

		/* just copy it into the buffer */
		msg[i] = *p;
		i++;
		p++;
	}

	/* don't forget to terminate the string stupido! */
	msg[i] = '\0';

	/* now copy it into the buffer provided */
	strcpy(buffer, msg );
	return;
}


/********************************************************************/
/*	mprint()						    */	
/*								    */
/* Now just calls msprintf to put the formatted string in the msg   */
/* buffer							    */
/* fd -  the socket to write to					    */
/* fmt - the format string ( only accepts variations of %m and %i   */
/* args - an array of pointers to the creatures and objects that    */
/*    are used in replcaement.					    */
/*								    */
/* NOTE: this does not do ANY sprintf replacements anymore	    */
/********************************************************************/
void mprint(int fd, char *fmt, void *args[] )
{
	char 	msg[4096];

	if(fmt == NULL)
		return;

	if(fd < 0 || fd > Tablesize)
		return;
	if(!Ply[fd].io)
		return;

	msprint(Ply[fd].ply, msg, fmt, args );
	output(fd, msg );

	return;
}



/**********************************************************************/
/*				output				      */
/**********************************************************************/

/* This function takes a string and outputs it to the socket described */
/* This is used as a replacement for print calls for no formatting necessary */

void output(int fd, char *str )
{
	char 	msg[4096];
	int	i = 0, n, otail, ohead;

	ASSERTLOG( str );

	if(fd < 0 || fd > Tablesize)
		return;
	if(!Ply[fd].io)
		return;

	strcpy( msg, str );

	n = strlen(msg);
	if(n > 78) {
		delimit(msg);
		n = strlen(msg);
	}

	ohead = Ply[fd].io->ohead;
	otail = Ply[fd].io->otail;

	for(i=0; i<n; i++) {
		Ply[fd].io->output[ohead] = msg[i];
		ohead = (ohead + 1) % OBUFSIZE;
		if(ohead == otail)
			otail = (otail + 1) % OBUFSIZE;
		if(msg[i] == '\n') {
			Ply[fd].io->output[ohead] = '\r';
			ohead = (ohead + 1) % OBUFSIZE;
			if(ohead == otail)
				otail = (otail + 1) % OBUFSIZE;
		}
	}
	Ply[fd].io->ohead = ohead;
	Ply[fd].io->otail = otail;

	return;
}


/**********************************************************************/
/*				output_nl			      */
/**********************************************************************/

/* This function takes a string and outputs it to the socket described */
/* it then tacks on a newline at the end */

void output_nl(int fd, char *str )
{

	ASSERTLOG( str );

	if(fd < 0 || fd > Tablesize)
		return;
	if(!Ply[fd].io)
		return;

	output(fd, str );
	output(fd, "\n");

	return;
}



/**********************************************************************/
/*				output_wc			      */
/**********************************************************************/

/* This function takes a string and outputs it to the socket described */
/* This is used as a replacement for print calls for no formatting necessary */
/* this is the color version of output */
void output_wc(int fd, char *str, int color )
{

	ANSI( fd, color );
	output( fd, str );
	ANSI(fd, MAINTEXTCOLOR );

	return;
}




/**********************************************************************/
/*				spy_write											  */
/**********************************************************************/
int spy_write(int fh, const void *lpvBuffer, unsigned int nLen )
{
	scwrite(fh, lpvBuffer, nLen );
	
	if(get_spy(fh) > -1) 
		{
		scwrite(get_spy(fh), lpvBuffer, nLen ); 
		}
	
	return(0);

}

/*
 *              check_dos(int fd )
 *  This function returns 1 if there are more than 5 connections from the same IP
 *
 */
int check_dos(int fd )
{
        int i,j=0;

                for(i=0; i<Tablesize ;i++)
                {
                        if(fd!=i)
                        {
                                if(Ply[i].io)
                                {
                                        if(!strcmp(Ply[i].io->address,Ply[fd].io->address))
                                        {
                                                j++;
                                                if(j >= 5)
                                                        return(1);
                                        }
                                }
                        }
                }
        return(0);
}
