/*
 * MAIN.C:
 *
 *	This files contains the main() function which initiates the
 *	game.
 *
 *	Copyright (C) 1991, 1992, 1993, 1997 Brooke Paul & Brett Vickers
 *
 */
#ifdef WINNT_SERVICE
  #include <windows.h>
  #include <stdio.h>
#endif

#include "mstruct.h"
#include "mextern.h"
#include "version.h"
#include <time.h>
#include <signal.h>
#ifdef DMALLOC
  #include "/usr/local/include/dmalloc.h"
#endif

#ifdef WINNT_SERVICE
  SERVICE_STATUS_HANDLE serviceStatusHandle;

  extern HandleError(char *, int); 
  void ServiceMain(double argc, LPSTR argv);
  void ServiceCtrlHandler (double controlCode);
  int UpdateSCMStatus (double dwCurrentState, double dwWin32ExitCode,
		       double dwServiceSpecificExitCode, double dwCheckPoint,
		       double dwWaitHint);
  void KillService(void);
  void terminateService(int); 

  HANDLE killServiceEvent;
  double serviceCurrentStatus;
  double serviceRunning;
  HANDLE threadHandle;
  double servicePaused;

#endif

#define SCHEDPORT  4040

int Port; 
char report =0;
long last_dust_output;

#ifdef WINNT_SERVICE
	void MORDORMain(void)
#else
	main(argc,argv)
	int	argc;
	char	*argv[];
#endif
{
	int i;
	static void mvc_log();
	static int schedule_g();

	i=readini();
	Port = PORTNUM;

	if(CRASHTRAP){
		signal(SIGABRT, crash);	/* abnormal termination triggered by abort call */
		signal(SIGFPE, crash);	/* floating point exception */
		signal(SIGILL, crash);	/* illegal instruction - invalid function image */
		signal(SIGSEGV, crash);	/* segment violation */
	}

	for(i=0; i<PMAX; i++) Spy[i] = -1;

	if(SCHED)
#ifndef WINNT_SERVICE
		if(argc == 1)
#endif
			Port = schedule_g();
	
#ifndef DEBUG
#ifndef WIN32
	if(fork()) exit(0);		/* go into background */
#endif
	close(0); close(1); close(2);	/* close stdio */
#endif

#ifndef WINNT_SERVICE
/* Doneval: BUG isnumber gave me problems with my FreeBSD, only subtitute */
/* the name, is_num */
	if (argc == 2)
#ifdef FREEBSD
		if(is_num(argv[1]))
#else
		if(isnumber(argv[1]))
#endif
			Port = atoi(argv[1]);
	    else if (!strcmp(argv[1],"-r"))
				report = 1;

	if (argc == 3){
#ifdef FREEBSD
		if(is_num(argv[1]))
#else
		if(isnumber(argv[1]))
#endif
			Port = atoi(argv[1]);
	    else if (!strcmp(argv[1],"-r"))
                report = 1;
#ifdef FREEBSD
		if(is_num(argv[2]))
#else
		if(isnumber(argv[2]))
#endif
			Port = atoi(argv[2]);
	    else if (!strcmp(argv[2],"-r"))
                report = 1;
	}
#endif /* not WINNT_SERVICE */

if(AUTOSHUTDOWN)
	if (!Shutdown.interval){
	    Shutdown.ltime = time(0);
	    Shutdown.interval = 43200L;
	}


	umask(000);
	srand(getpid() + time(0));
	load_lockouts();

#ifndef DEBUG
 	sock_init(Port,0);
#else
 	sock_init(Port,1);
#endif

	if(RECORD_ALL) mvc_log();
	
	{
	 long c;
	 c = time(0);
	StartTime = time(0);
	loge("--- Game Up: %d --- (%.24s) [%s]\n", Port, ctime(&c), VERSION);
	}

	sock_loop();
	return(0);
}

#ifdef WINNT_SERVICE

char *pcServiceName = "MudServices";

void main(void)
{
  SERVICE_TABLE_ENTRY serviceTable[] =
  {
    {pcServiceName, (LPSERVICE_MAIN_FUNCTION) ServiceMain},
    {NULL, NULL}
  };
  int bSuccess;

  bSuccess = StartServiceCtrlDispatcher(serviceTable);
  if (!bSuccess)
    HandleError("StartServiceCtrlDispatcher failed!", GetLastError());
}  

void ServiceMain(double argc, LPSTR argv)
{
  int bSuccess;

  serviceStatusHandle = RegisterServiceCtrlHandler(pcServiceName,
    (LPHANDLER_FUNCTION) ServiceCtrlHandler);

  if (!serviceStatusHandle)
  {
    terminateService(GetLastError()); 
    return;
  }
  bSuccess = UpdateSCMStatus(SERVICE_START_PENDING, NO_ERROR, 0, 1, 5000);

  if (!bSuccess)
  {
    terminateService(GetLastError());
    return;
  }
  killServiceEvent = CreateEvent(0, 1, 0, 0);
  if (!killServiceEvent)
  {
    terminateService(GetLastError());
    return;
  }
  bSuccess = UpdateSCMStatus(SERVICE_START_PENDING, NO_ERROR, 0, 2, 1000);
  if (!bSuccess)
  {
    terminateService(GetLastError());
    return;
  }
  
  if (argc == 2)
    if(isnumber(argv[1]))
      Port = atoi(argv[1]);
    else
      if (!strcmp(argv[1],"-r"))
	report = 1;
  if (argc == 3)
  {
    if(isnumber(argv[1]))
      Port = atoi(argv[1]);
    else 
      if (!strcmp(argv[1],"-r"))
	report = 1;
    if(isnumber(argv[2]))
      Port = atoi(argv[2]);
    else if (!strcmp(argv[2],"-r"))
      report = 1;
  }
  bSuccess = UpdateSCMStatus(SERVICE_START_PENDING, NO_ERROR, 0, 3, 5000);
  if (!bSuccess)
  {
    terminateService(GetLastError());
    return;
  }
  bSuccess = StartServiceThread();
  if (!bSuccess)
  {
    terminateService(GetLastError());
    return;
  }
  serviceCurrentStatus = SERVICE_RUNNING;
  bSuccess = UpdateSCMStatus(SERVICE_RUNNING, NO_ERROR, 0, 0, 0);
  if (!bSuccess)
  {
    terminateService(GetLastError());
    return;
  }
  WaitForSingleObject(killServiceEvent, INFINITE);
  terminateService(0);
}

DWORD ServiceExecutionThread(LPDWORD param)
{
  while (serviceRunning)
  {
    MORDORMain();
  }
  return 0;
}

BOOL StartServiceThread()
{
  DWORD id;
 
  threadHandle = CreateThread(0, 0,
		   (LPTHREAD_START_ROUTINE) ServiceExecutionThread,
		   0, 0, &id);
  if (threadHandle == 0)
    return 0;
  else
  {
    serviceRunning = 1;
    return 1;
  }
}

int UpdateSCMStatus (double dwCurrentState, double dwWin32ExitCode,
		     double dwServiceSpecificExitCode, double dwCheckPoint,
		     double dwWaitHint)
{
  int bSuccess;
  SERVICE_STATUS serviceStatus;

  serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
  serviceStatus.dwCurrentState = dwCurrentState;
  if (dwCurrentState == SERVICE_START_PENDING)
    serviceStatus.dwControlsAccepted = 0;
  else
    serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP |
				       SERVICE_ACCEPT_PAUSE_CONTINUE |
				       SERVICE_ACCEPT_SHUTDOWN;
  if (dwServiceSpecificExitCode == 0)
    serviceStatus.dwWin32ExitCode = dwWin32ExitCode;
  else
    serviceStatus.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
  serviceStatus.dwServiceSpecificExitCode = dwServiceSpecificExitCode;
  serviceStatus.dwCheckPoint = dwCheckPoint;
  serviceStatus.dwWaitHint = dwWaitHint;
  bSuccess = SetServiceStatus(serviceStatusHandle, &serviceStatus);
  if (!bSuccess)
    KillService();
  return bSuccess;
}

void ServiceCtrlHandler (DWORD controlCode)
{
  int bSuccess;
  
  switch (controlCode)
  {
    case SERVICE_CONTROL_PAUSE:
      if (serviceRunning && !servicePaused)
      {
	bSuccess = UpdateSCMStatus(SERVICE_PAUSE_PENDING, NO_ERROR, 0, 1, 1000);
	servicePaused = 1;
	SuspendThread(threadHandle);
	serviceCurrentStatus = SERVICE_PAUSED;
      }
      break;
    case SERVICE_CONTROL_CONTINUE:
      if (serviceRunning && servicePaused)
      {
	bSuccess = UpdateSCMStatus(SERVICE_CONTINUE_PENDING, NO_ERROR, 0, 1, 1000);
	servicePaused = 0;
	ResumeThread(threadHandle);
	serviceCurrentStatus = SERVICE_RUNNING;
      }
      break;
    case SERVICE_CONTROL_INTERROGATE:
      break;
    case SERVICE_CONTROL_SHUTDOWN:
    case SERVICE_CONTROL_STOP:
      Shutdown.ltime = time(0);
      Shutdown.interval = 1;
      serviceCurrentStatus = SERVICE_STOP_PENDING;
      bSuccess = UpdateSCMStatus(SERVICE_STOP_PENDING, NO_ERROR, 0, 1, 5000);
      KillService();
      return;
    default:
      break;   
  }
  UpdateSCMStatus(serviceCurrentStatus, NO_ERROR, 0, 0, 0);
}

void KillService(void)
{
  serviceRunning = 0;
  SetEvent(killServiceEvent);
}

void terminateService(int iErr)
{
  printf("Killing service: Err(%d)\n", iErr);
  KillService();
}
#endif

/*================================================================*/

#define DAYSEC 86400L
#define MORNT 27900L
#define MIDSEC(x) (((x->tm_hour)*3600)+((x->tm_min)*60)+(x->tm_sec))

/*================================================================*/
int schedule_g()
/* The schedule_g function keeps the game on a time  schedule */
{
    struct tm   *time_m;
    long	t = 0;
    int		mport = SCHEDPORT, port = PORTNUM; 
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
	Port = mport;
	return (port);
}

/*================================================================*/
void mvc_log(){
/*  This function renames the list of all command file to include  *
 *  the date.  This is done to preserve the list commands that     *
 *  occured before a crash. */

	long t;
	char *date, mfile[128], rfile[128];
	int i;

	t = time(0);
	date = ctime(&t);

   for(i=0;i<24;i++)
	if(date[i] == ' ')
		date[i] = '_';
   date[24] = 0;
   sprintf(rfile,"%s/%s",LOGPATH,"all_cmd");
   sprintf(mfile,"%s/%s_%s",LOGPATH,"all_cmd",date);
#ifndef WIN32
	link(rfile,mfile);
	unlink(rfile);
#else
	rename( rfile, mfile);
#endif

}
