# Microsoft Developer Studio Project File - Name="mordor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=mordor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mordor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mordor.mak" CFG="mordor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mordor - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "mordor - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mordor - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 mordb.lib morutil.lib port.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /machine:I386
# Begin Special Build Tool
SOURCE=$(InputPath)
PostBuild_Desc=Copying Executable
PostBuild_Cmds=xcopy release\mordor.exe ..\..\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_CONSOLE" /D "_MBCS" /D "FINGERACCT" /D "WIN32" /D "_DEBUG" /D "_CRTDBG_MAP_ALLOC" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 mordbd.lib morutild.lib portd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "mordor - Win32 Release"
# Name "mordor - Win32 Debug"
# Begin Source File

SOURCE=.\access.c
# End Source File
# Begin Source File

SOURCE=.\action.c
# End Source File
# Begin Source File

SOURCE=.\bank.c
# End Source File
# Begin Source File

SOURCE=.\combat.c
# End Source File
# Begin Source File

SOURCE=.\command1.c
# End Source File
# Begin Source File

SOURCE=.\command10.c
# End Source File
# Begin Source File

SOURCE=.\command11.c
# End Source File
# Begin Source File

SOURCE=.\command12.c
# End Source File
# Begin Source File

SOURCE=.\command13.c
# End Source File
# Begin Source File

SOURCE=.\command2.c
# End Source File
# Begin Source File

SOURCE=.\command3.c
# End Source File
# Begin Source File

SOURCE=.\command4.c
# End Source File
# Begin Source File

SOURCE=.\command5.c
# End Source File
# Begin Source File

SOURCE=.\command6.c
# End Source File
# Begin Source File

SOURCE=.\command7.c
# End Source File
# Begin Source File

SOURCE=.\command8.c
# End Source File
# Begin Source File

SOURCE=.\command9.c
# End Source File
# Begin Source File

SOURCE=.\creature.c
# End Source File
# Begin Source File

SOURCE=.\dm1.c
# End Source File
# Begin Source File

SOURCE=.\dm2.c
# End Source File
# Begin Source File

SOURCE=.\dm3.c
# End Source File
# Begin Source File

SOURCE=.\dm4.c
# End Source File
# Begin Source File

SOURCE=.\dm5.c
# End Source File
# Begin Source File

SOURCE=.\dm6.c
# End Source File
# Begin Source File

SOURCE=.\dm7.c
# End Source File
# Begin Source File

SOURCE=.\dm8.c
# End Source File
# Begin Source File

SOURCE=.\errhandl.c
# End Source File
# Begin Source File

SOURCE=.\global.c
# End Source File
# Begin Source File

SOURCE=.\io.c
# End Source File
# Begin Source File

SOURCE=.\logic.c
# End Source File
# Begin Source File

SOURCE=.\magic1.c
# End Source File
# Begin Source File

SOURCE=.\magic2.c
# End Source File
# Begin Source File

SOURCE=.\magic3.c
# End Source File
# Begin Source File

SOURCE=.\magic4.c
# End Source File
# Begin Source File

SOURCE=.\magic5.c
# End Source File
# Begin Source File

SOURCE=.\magic6.c
# End Source File
# Begin Source File

SOURCE=.\magic7.c
# End Source File
# Begin Source File

SOURCE=.\magic8.c
# End Source File
# Begin Source File

SOURCE=.\magic9.c
# End Source File
# Begin Source File

SOURCE=.\main.c
# End Source File
# Begin Source File

SOURCE=.\mextern.h
# End Source File
# Begin Source File

SOURCE=.\misc.c
# End Source File
# Begin Source File

SOURCE=.\msp.c
# End Source File
# Begin Source File

SOURCE=.\ntservic.c
# End Source File
# Begin Source File

SOURCE=.\object.c
# End Source File
# Begin Source File

SOURCE=.\player.c
# End Source File
# Begin Source File

SOURCE=.\post.c
# End Source File
# Begin Source File

SOURCE=.\readini.c
# End Source File
# Begin Source File

SOURCE=.\room.c
# End Source File
# Begin Source File

SOURCE=.\screen.c
# End Source File
# Begin Source File

SOURCE=.\special1.c
# End Source File
# Begin Source File

SOURCE=.\talk.c
# End Source File
# Begin Source File

SOURCE=.\update.c
# End Source File
# Begin Source File

SOURCE=.\update.h
# End Source File
# End Target
# End Project
