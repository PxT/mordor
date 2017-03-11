# Microsoft Developer Studio Project File - Name="mordb" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=mordb - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mordb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mordb.mak" CFG="mordb - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mordb - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mordb - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "mordb - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "COMPRESS" /FD /c
# SUBTRACT CPP /YX
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
SOURCE=$(InputPath)
PostBuild_Desc=Copying Extarnals
PostBuild_Cmds=copy release\mordb.lib ..\lib	copy mordb.h ..\include
# End Special Build Tool

!ELSEIF  "$(CFG)" == "mordb - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /D "_WINDOWS" /D "COMPRESS" /D "WIN32" /D "_DEBUG" /D "_CRTDBG_MAP_ALLOC" /FD /c
# SUBTRACT CPP /YX
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\mordbd.lib"
# Begin Special Build Tool
SOURCE=$(InputPath)
PostBuild_Desc=Copying Externals
PostBuild_Cmds=copy debug\mordbd.lib ..\lib	copy mordb.h ..\include
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "mordb - Win32 Release"
# Name "mordb - Win32 Debug"
# Begin Source File

SOURCE=.\access.c
# End Source File
# Begin Source File

SOURCE=.\active.c
# End Source File
# Begin Source File

SOURCE=.\compress.c
# End Source File
# Begin Source File

SOURCE=.\files1.c
# End Source File
# Begin Source File

SOURCE=.\files2.c
# End Source File
# Begin Source File

SOURCE=.\files3.c
# End Source File
# Begin Source File

SOURCE=.\files4.c
# End Source File
# Begin Source File

SOURCE=.\mordb.h
# End Source File
# End Target
# End Project
