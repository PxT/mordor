/*
 * AUTH.C:
 *
 *	Performs user authentication services.  Retrieves the
 *	host name of the user.  Also retrieves the user id if
 *	the host is running identd.
 *
 *	Copyright (C) 1994   Brett J. Vickers
 *	Copyright (C) 1995, 1999	 Brooke Paul
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>
#include <ctype.h>

#include "../include/port.h"
#include "../include/morutil.h"

#ifdef WIN32
#include <mmsystem.h>
#include <io.h>
#endif



char Output[2][80];
int proxies = 0;
FILE *fp;


/* local prototypes */
char *start_auth(struct in_addr *in, int oport, int iport, char *buf);
void parse_response( char *buf );
void my_abort(int sig);
void set_abort_timer( int seconds );
int check_proxies(char *addr, int port);
void sigalrm_handler(int signal);

#ifdef WIN32
void CALLBACK TimeProc( UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2 );
MMRESULT	mmID;
#endif



main(int argc, char *argv[])
{
	char ipaddr[80], filename[127];
	int oport, iport;
	int	return_code = 0;
	char buf[80];
	struct hostent *h;
	struct in_addr in;


	/* initialize these now in case the 20 second timer runs out */
	strcpy(Output[0], "no_port");
	strcpy(Output[1], "UNKNOWN");

	if(argc != 4) {
		fprintf(stderr, "auth v6.2\n");
		fprintf(stderr, "Syntax: %s ip_address oport iport\n", argv[0]);
		exit(0);
	}

	init_sockets();

	set_abort_timer(20);

	sprintf(filename, "%s/auth/lookup.%d", get_log_path(), getpid());
	fp = fopen(filename, "w");

	strcpy(ipaddr, argv[1]);
	oport = atoi(argv[2]);
	iport = atoi(argv[3]);
	in.s_addr = inet_addr(ipaddr);

	/* Perform the host name lookup on the IP address*/
	h = gethostbyaddr((char *)&in, sizeof(in), AF_INET);
	if(!h)
		strcpy(Output[1], "UNKNOWN");
	else
		strcpy(Output[1], h->h_name);


	if( strncmp(ipaddr, "127.0", 5)) {
		if(check_proxies(ipaddr, 1080)) {
			proxies = 1;
		}
		else if(check_proxies(ipaddr, 8088)) {
			proxies = 1;
		}
		else if(check_proxies(ipaddr, 23))  {
			proxies = 1;
		}
	}

	/* Begin the user id lookup */
	if(proxies != 1) {
		if(start_auth(&in, oport, iport, buf))
			strcpy(Output[0], buf);
		else
			strcpy(Output[0], "no_port");
	}

	/* Save the results to a file so frp can look them up */
	if(fp) 
	{
		fprintf(fp, "%s\n%s\n%d\n", Output[0], Output[1], proxies);
		fclose(fp);
	}
	else
	{
		return_code = -1;
	}

#ifdef WIN32
	timeKillEvent(mmID);
#endif

	cleanup_sockets();

	return(return_code);
}

/************************************************************************/
/* set_abort_timer()
/************************************************************************/
void set_abort_timer(int seconds )
{

#ifndef WIN32
	
	struct itimerval timer_value;

	timer_value.it_interval.tv_sec = 0L;
	timer_value.it_interval.tv_usec = 0L;
	timer_value.it_value.tv_sec = (long)seconds;
	timer_value.it_value.tv_usec = 0L;

	setitimer(ITIMER_REAL, &timer_value, 0);

	signal(SIGALRM, my_abort);

#else
	mmID = timeSetEvent(seconds * 1000L, 1000, TimeProc, 0, TIME_ONESHOT); 
#endif

	return;
}


/************************************************************************/
/*				start_auth				*/
/************************************************************************/

/* This function opens a socket to the identd port of the player's	*/
/* machine (port 113) and requests the user id of that player.		*/

char *start_auth(struct in_addr *in, int oport, int iport, char *buf)
{

	struct sockaddr_in sin;
	int fd, n, Tablesize, len;
	fd_set Sockets, sockcheck;
	struct timeval t;
	char outstr[80];


	t.tv_sec = 10L;
	t.tv_usec = 0L;

	sin.sin_addr.s_addr = in->s_addr;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(113);

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd < 0) return((char *)0);

	if(connect(fd, (struct sockaddr *)&sin, sizeof(sin)) < 0)
		return((char *)0);

	FD_ZERO(&Sockets);
	FD_SET(fd, &Sockets);

	/* Generate request string */
	sprintf(outstr, "%d , %d\r\n", oport, iport);
	len = strlen(outstr);

	Tablesize = getdtablesize();
	sockcheck = Sockets;

	/* Wait until we can send our request */
	if(select(Tablesize, 0, &sockcheck, 0, &t) <= 0)
		return((char *)0);

	/* Send the request */
	n = scwrite(fd, outstr, len);
	if(n < len) return((char *)0);

	/* Wait until the other side responds */
	sockcheck = Sockets;
	if(select(Tablesize, &sockcheck, 0, 0, &t) <= 0)
		return((char *)0);

	/* Read the response */
	n = scread(fd, buf, 80);
	scclose(fd);
	if(n <= 0)
		return((char *)0);
	else {
		buf[n-1] = 0;
		parse_response(buf);
		return(buf);
	}
}

/************************************************************************/
/*				parse					*/
/************************************************************************/

/* This function parses the response from the identity server.  It	*/
/* essentially strips out the user id if there is one to be obtained.	*/

void parse_response( char *buf )
{
	char *p, temp[80];

	if(p = (char *)strstr(buf, "USERID")) {
		while(*p && *p != ':') p++; p++;
		while(*p && *p != ':') p++;
		while(*p &&
		      !(*p <= '9' && *p >= '0') &&
		      !(*p <= 'z' && *p >= 'a') &&
		      !(*p <= 'Z' && *p >= 'A')) p++;
		strcpy(temp, p);
		p = temp;
		while(*p &&
			((*p >= 'a' && *p <='z') ||
			 (*p >= 'A' && *p <='Z') ||
		         (*p >= '0' && *p <= '9'))) p++;
		*p = 0;
		strcpy(buf, temp);
		buf[8] = 0;
	}
	else {
		strcpy(buf, "unknown");
	}

}

void my_abort(int sig )
{

#ifdef WIN32
	/* clean up nicely */
	timeKillEvent(mmID);
#endif

	if(fp) {
		fprintf(fp, "%s\n%s\n%d\n", Output[0], Output[1], proxies);
		fclose(fp);
	}
	cleanup_sockets();

	exit(0);
}



#ifdef WIN32

void CALLBACK TimeProc( UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2 ) 
{

	/* we timed out */
	my_abort( 0 );
}

  
#endif



int check_proxies(char *host, int port)
{

        int e,sockfd, numbytes;
        struct hostent *he;
        struct sockaddr_in sin;
        fd_set gateset;
	struct timeval tv2;
	char	buf[42] = "This is some bogus data";


	/* 3 second timeout */
	tv2.tv_usec = 0L;
	tv2.tv_sec = 3L;

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        sin.sin_family = AF_INET;
        sin.sin_port = htons(port);
        sin.sin_addr.s_addr = inet_addr(host);

        /* Set socket to non-blocking */
	e = fcntl(sockfd, F_GETFL);
	e |= O_NONBLOCK;
        if (fcntl(sockfd, F_SETFL, e) < 0)
                return(0);


        if(sin.sin_addr.s_addr == INADDR_NONE)
        {
                he = gethostbyname(host);
                if(!he)
                {
                        close(sockfd);
                        return(0);
                }
                memcpy(&sin.sin_addr, he->h_addr, he->h_length);
        }

        if (connect (sockfd, (struct sockaddr *) &sin, sizeof (struct sockaddr_in)) < 0) {
                if (errno != EAGAIN && errno != EINPROGRESS) {
			close(sockfd);
                        return(0);
                }
	}

        FD_ZERO (&gateset);
        FD_SET (sockfd, &gateset);


        /* wait for timer to run out, or for fd to be ready */
        while (select (getdtablesize (), NULL, &gateset, NULL, &tv2) < 0) {
                if (errno != EINTR) {
			close(sockfd);
                        return 0;
                }
        }

        if (tv2.tv_sec == 0 && tv2.tv_usec == 0) {
		/* timeout */
		close(sockfd);
                return 0;
        }

	if(port == 23) {
		numbytes = read(sockfd, buf, sizeof(buf));
		buf[numbytes] = '\0';
		if(numbytes == 9) {
			numbytes = read(sockfd, buf, sizeof(buf));
			buf[numbytes] = '\0';
			if(!strcmp(buf, "WinGate>")) {
				close(sockfd);
				return(1);
			}
		}
		return(0); 

	} else {  /* SOCKS ports */
		e = write(sockfd, buf, 42);
		if(e < 0) {
			close(sockfd);
			return(0);
		} else {
			/* connection is established, we have an open proxy */
			close(sockfd);
			return(1);
		}
	}

}

void sigalrm_handler(int signal)
{
	return;
}
