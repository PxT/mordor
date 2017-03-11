/*
 * NTSERVIC.C:
 *
 *	This files contains the NT service routines necessary to run the game
 *  as a service
 *
 *	Copyright (C) 1995 Brooke Paul
 *	Copyright (c) 1998 John P. Freeman
 *
 * $Id: ntservic.c,v 6.13 2001/03/08 16:09:09 develop Exp $
 *
 * $Log: ntservic.c,v $
 * Revision 6.13  2001/03/08 16:09:09  develop
 * *** empty log message ***
 *
 */
#include <stdio.h>


#include "../include/mordb.h"
#include "../include/version.h"

#include "mextern.h"



#ifdef WIN32

SERVICE_STATUS_HANDLE serviceStatusHandle;


void ServiceMain(DWORD argc, LPSTR *argv);
BOOL StartServiceThread();
void ServiceCtrlHandler (DWORD controlCode);
int UpdateSCMStatus (DWORD dwCurrentState, DWORD dwWin32ExitCode,
DWORD dwServiceSpecificExitCode, DWORD dwCheckPoint, DWORD dwWaitHint);
void KillService(void);
void terminateService(int); 
LPTSTR GetLastErrorText( LPTSTR lpszBuf, DWORD dwSize );

HANDLE	killServiceEvent;
DWORD	serviceCurrentStatus;
DWORD	serviceRunning;
HANDLE	threadHandle;
DWORD	servicePaused;
TCHAR	szErr[256];


char *szServiceName = "Mordor MUD";


void ServiceMain(DWORD argc, LPSTR *argv)
{
	int bSuccess;

	serviceStatusHandle = RegisterServiceCtrlHandler(szServiceName,
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

	handle_args( argc, argv );

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
		startup_mordor();
	}

	return 0;
}

BOOL StartServiceThread()
{
	DWORD id;

	threadHandle = CreateThread(0, 0, 
		(LPTHREAD_START_ROUTINE) ServiceExecutionThread, 0, 0, &id);
	if (threadHandle != 0)
	{
		serviceRunning = 1;
	}

	return(threadHandle == 0);
}

int UpdateSCMStatus (DWORD dwCurrentState, DWORD dwWin32ExitCode,
		     DWORD dwServiceSpecificExitCode, DWORD dwCheckPoint,
		     DWORD dwWaitHint)
{
	int bSuccess;
	SERVICE_STATUS serviceStatus;

	serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	serviceStatus.dwCurrentState = dwCurrentState;

	if (dwCurrentState == SERVICE_START_PENDING)
		serviceStatus.dwControlsAccepted = 0;
	else
		serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP |
		   SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_SHUTDOWN;

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


/****************************************************************/
/* InstallService()												*/
/*		installs mordor as a service from the command line.		*/
/****************************************************************/
void InstallService()
{
    SC_HANDLE   schService;
    SC_HANDLE   schSCManager;

    TCHAR szPath[512];

    if ( GetModuleFileName( NULL, szPath, 512 ) == 0 )
    {
        printf("Unable to install %s - %s\n", szServiceName, GetLastErrorText(szErr, 256));
        return;
    }

    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS );
    if ( schSCManager )
    {
        schService = CreateService(
            schSCManager,               /* SCManager database */
			szServiceName,				/* name of service */
            szServiceName,				/* name to display */
            SERVICE_ALL_ACCESS,         /* desired access */
            SERVICE_WIN32_OWN_PROCESS,  /* service type */
            SERVICE_AUTO_START,          /* start type */
            SERVICE_ERROR_NORMAL,       /* error control type */
            szPath,                     /* service's binary */
            NULL,                       /* no load ordering group */
            NULL,                       /* no tag identifier */
            NULL,						/* dependencies */
            NULL,                       /* LocalSystem account */
            NULL);                      /* no password */

        if ( schService )
        {
            printf("%s installed.\n", szServiceName);
            CloseServiceHandle(schService);
        }
        else
        {
            printf("CreateService failed - %s\n", GetLastErrorText(szErr, 256));
        }

        CloseServiceHandle(schSCManager);
    }
    else
	{
        printf("OpenSCManager failed - %s\n", GetLastErrorText(szErr,256));
		printf("Could not install service.\n");
	}

	return;
}



/*****************************************************************/
/* RemoveService()												*/
/*		removes mordor as a service from the command line.*/
/****************************************************************/
void RemoveService()
{
    SC_HANDLE   schService;
    SC_HANDLE   schSCManager;
	SERVICE_STATUS	ssStatus;

    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS );
    if ( schSCManager )
    {
        schService = OpenService(schSCManager, szServiceName, SERVICE_ALL_ACCESS);

        if (schService)
        {
            /* try to stop the service */
            if ( ControlService( schService, SERVICE_CONTROL_STOP, &ssStatus ) )
            {
                printf("Stopping %s.", szServiceName);
                Sleep( 1000 );

                while( QueryServiceStatus( schService, &ssStatus ) )
                {
                    if ( ssStatus.dwCurrentState == SERVICE_STOP_PENDING )
                    {
                        printf(".");
                        Sleep( 1000 );
                    }
                    else
                        break;
                }

                if ( ssStatus.dwCurrentState == SERVICE_STOPPED )
                    printf("\n%s stopped.\n", szServiceName );
                else
                    printf("\n%s failed to stop.\n", szServiceName );

            }

            /* now remove the service */
            if( DeleteService(schService) )
                printf("%s removed.\n", szServiceName );
            else
                printf("DeleteService failed - %s\n", GetLastErrorText(szErr,256));


            CloseServiceHandle(schService);
        }
        else
            printf("OpenService failed - %s\n", GetLastErrorText(szErr,256));

        CloseServiceHandle(schSCManager);
    }
    else
        printf("OpenSCManager failed - %s\n", GetLastErrorText(szErr,256));

	return;
}

/****************************************************************/
/* GetLastErrorText()												*/
/****************************************************************/
LPTSTR GetLastErrorText( LPTSTR lpszBuf, DWORD dwSize )
{
    DWORD dwRet;
    LPTSTR lpszTemp = NULL;

    dwRet = FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_ARGUMENT_ARRAY,
		NULL, GetLastError(), LANG_NEUTRAL, (LPTSTR)&lpszTemp, 
		0, NULL );

    /* supplied buffer is not long enough */
    if ( !dwRet || ( (long)dwSize < (long)dwRet+14 ) )
        lpszBuf[0] = '\0';
    else
    {
		/* remove cr and newline character */
        lpszTemp[lstrlen(lpszTemp)-2] = '\0';
        sprintf( lpszBuf, "%s (0x%x)", lpszTemp, GetLastError() );
    }

    if ( lpszTemp )
        LocalFree((HLOCAL) lpszTemp );

    return lpszBuf;
}

#endif

