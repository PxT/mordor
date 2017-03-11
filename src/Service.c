#include <stdio.h>
#include <windows.h>

extern void HandleError(char *pcMsg, int iErr);
void InstService();
void UninstService();
char path[80];

void main(int argc, char *argv[])
{
	if (argc>2) {
		sprintf(path,"%s/mordornt.exe",argv[2]);
		if (strcmp(argv[1],"-install")==0) {
			printf("Installing Service to %s\n",path);
		}
		return;
	}
	if (argc=2) {
		if (strcmp(argv[1],"-install")==0) {
			sprintf(path,"d:/mud/bin/mordornt.exe");
			printf("Installing Service\n");
			InstService();
		}
		if (strcmp(argv[1],"-uninstall")==0) {
			printf("Uninstalling Service\n");
			UninstService();
		}
		if (strcmp(argv[1],"-?")==0) {
			printf("Syntax:  Service [-install [path]|-unistall]\n");
			printf("  i.e.:  Service -install d:/mud/bin (note:  use front slashes!)");
			printf("  i.e.:  Service -install   (uses d:/mud)");
			printf("         Service -uninstall");
		}
		return;
	}
}

void InstService()
{

  SC_HANDLE myService, scm;
  printf("Installing MORDOR Server...\n");
  scm = OpenSCManager(0, 0, SC_MANAGER_CREATE_SERVICE);
  if (!scm)
    HandleError("OpenSCManager failed!", GetLastError());
  printf("Open Service Control Manager...\n");
  myService = CreateService(scm, "MudServices", "Mud Services",
                SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, 
                SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
                path, 0, 0, 0, 0, 0);
  if (!myService)
    HandleError("CreateService failed!", GetLastError());
  else
    printf("Service successfully installed.\n");
  CloseServiceHandle(myService);
  CloseServiceHandle(scm);
}


void UninstService()
{
  SC_HANDLE myService, scm;
  BOOL bSuccess;
  SERVICE_STATUS status;
  printf("Removing Service...\n");
  scm = OpenSCManager(0, 0, SC_MANAGER_CREATE_SERVICE);
  if (!scm)
    HandleError("OpenSCManager failed!", GetLastError());
  printf("Open Service Control Manager...\n");
  myService = OpenService(scm, "MudServices", SERVICE_ALL_ACCESS | DELETE);
  if (!myService)
    HandleError("OpenService failed!", GetLastError());
  if (status.dwCurrentState != SERVICE_STOPPED)
  {
    printf("Service currently active.  Stopping service...\n");
    bSuccess = ControlService(myService, SERVICE_CONTROL_STOP, &status);
    if (!bSuccess)
		if (!(GetLastError()==1062))
			HandleError("ControlService failed to stop service!", GetLastError());
    Sleep(500);
  }
  bSuccess = DeleteService(myService);
  if (!bSuccess)
    HandleError("DeleteService failed!", GetLastError());
  else
    printf("Service successfully removed.\n");
  CloseServiceHandle(myService);
  CloseServiceHandle(scm);
}
