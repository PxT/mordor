# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=Service - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Service - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "mordor - Win32 Release" && "$(CFG)" != "mordor - Win32 Debug"\
 && "$(CFG)" != "mordor - Win32 Service" && "$(CFG)" != "makecf - Win32 Release"\
 && "$(CFG)" != "makecf - Win32 Debug" && "$(CFG)" != "Service - Win32 Release"\
 && "$(CFG)" != "Service - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "mordor.mak" CFG="Service - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mordor - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "mordor - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "mordor - Win32 Service" (based on "Win32 (x86) Console Application")
!MESSAGE "makecf - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "makecf - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "Service - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Service - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "mordor - Win32 Debug"
RSC=rc.exe
CPP=cl.exe

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
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\mordor.exe"

CLEAN : 
	-@erase "$(INTDIR)\action.obj"
	-@erase "$(INTDIR)\bank.obj"
	-@erase "$(INTDIR)\combat.obj"
	-@erase "$(INTDIR)\command1.obj"
	-@erase "$(INTDIR)\command10.obj"
	-@erase "$(INTDIR)\command11.obj"
	-@erase "$(INTDIR)\command12.obj"
	-@erase "$(INTDIR)\command13.obj"
	-@erase "$(INTDIR)\command2.obj"
	-@erase "$(INTDIR)\command3.obj"
	-@erase "$(INTDIR)\command4.obj"
	-@erase "$(INTDIR)\command5.obj"
	-@erase "$(INTDIR)\command6.obj"
	-@erase "$(INTDIR)\command7.obj"
	-@erase "$(INTDIR)\command8.obj"
	-@erase "$(INTDIR)\command9.obj"
	-@erase "$(INTDIR)\compress.obj"
	-@erase "$(INTDIR)\creature.obj"
	-@erase "$(INTDIR)\dm1.obj"
	-@erase "$(INTDIR)\dm2.obj"
	-@erase "$(INTDIR)\dm3.obj"
	-@erase "$(INTDIR)\dm4.obj"
	-@erase "$(INTDIR)\dm5.obj"
	-@erase "$(INTDIR)\dm6.obj"
	-@erase "$(INTDIR)\dm7.obj"
	-@erase "$(INTDIR)\Errhandl.obj"
	-@erase "$(INTDIR)\files1.obj"
	-@erase "$(INTDIR)\files2.obj"
	-@erase "$(INTDIR)\files3.obj"
	-@erase "$(INTDIR)\files4.obj"
	-@erase "$(INTDIR)\global.obj"
	-@erase "$(INTDIR)\io.obj"
	-@erase "$(INTDIR)\logic.obj"
	-@erase "$(INTDIR)\magic1.obj"
	-@erase "$(INTDIR)\magic2.obj"
	-@erase "$(INTDIR)\magic3.obj"
	-@erase "$(INTDIR)\magic4.obj"
	-@erase "$(INTDIR)\magic5.obj"
	-@erase "$(INTDIR)\magic6.obj"
	-@erase "$(INTDIR)\magic7.obj"
	-@erase "$(INTDIR)\magic8.obj"
	-@erase "$(INTDIR)\magic9.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\memory.obj"
	-@erase "$(INTDIR)\misc.obj"
	-@erase "$(INTDIR)\object.obj"
	-@erase "$(INTDIR)\player.obj"
	-@erase "$(INTDIR)\post.obj"
	-@erase "$(INTDIR)\Readini.obj"
	-@erase "$(INTDIR)\room.obj"
	-@erase "$(INTDIR)\screen.obj"
	-@erase "$(INTDIR)\special1.obj"
	-@erase "$(INTDIR)\strstr.obj"
	-@erase "$(INTDIR)\talk.obj"
	-@erase "$(INTDIR)\update.obj"
	-@erase "$(OUTDIR)\mordor.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "COMPRESS" /D "WINNT" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D\
 "COMPRESS" /D "WINNT" /Fp"$(INTDIR)/mordor.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mordor.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib wsock32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)/mordor.pdb" /machine:I386 /out:"$(OUTDIR)/mordor.exe" 
LINK32_OBJS= \
	"$(INTDIR)\action.obj" \
	"$(INTDIR)\bank.obj" \
	"$(INTDIR)\combat.obj" \
	"$(INTDIR)\command1.obj" \
	"$(INTDIR)\command10.obj" \
	"$(INTDIR)\command11.obj" \
	"$(INTDIR)\command12.obj" \
	"$(INTDIR)\command13.obj" \
	"$(INTDIR)\command2.obj" \
	"$(INTDIR)\command3.obj" \
	"$(INTDIR)\command4.obj" \
	"$(INTDIR)\command5.obj" \
	"$(INTDIR)\command6.obj" \
	"$(INTDIR)\command7.obj" \
	"$(INTDIR)\command8.obj" \
	"$(INTDIR)\command9.obj" \
	"$(INTDIR)\compress.obj" \
	"$(INTDIR)\creature.obj" \
	"$(INTDIR)\dm1.obj" \
	"$(INTDIR)\dm2.obj" \
	"$(INTDIR)\dm3.obj" \
	"$(INTDIR)\dm4.obj" \
	"$(INTDIR)\dm5.obj" \
	"$(INTDIR)\dm6.obj" \
	"$(INTDIR)\dm7.obj" \
	"$(INTDIR)\Errhandl.obj" \
	"$(INTDIR)\files1.obj" \
	"$(INTDIR)\files2.obj" \
	"$(INTDIR)\files3.obj" \
	"$(INTDIR)\files4.obj" \
	"$(INTDIR)\global.obj" \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\logic.obj" \
	"$(INTDIR)\magic1.obj" \
	"$(INTDIR)\magic2.obj" \
	"$(INTDIR)\magic3.obj" \
	"$(INTDIR)\magic4.obj" \
	"$(INTDIR)\magic5.obj" \
	"$(INTDIR)\magic6.obj" \
	"$(INTDIR)\magic7.obj" \
	"$(INTDIR)\magic8.obj" \
	"$(INTDIR)\magic9.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\memory.obj" \
	"$(INTDIR)\misc.obj" \
	"$(INTDIR)\object.obj" \
	"$(INTDIR)\player.obj" \
	"$(INTDIR)\post.obj" \
	"$(INTDIR)\Readini.obj" \
	"$(INTDIR)\room.obj" \
	"$(INTDIR)\screen.obj" \
	"$(INTDIR)\special1.obj" \
	"$(INTDIR)\strstr.obj" \
	"$(INTDIR)\talk.obj" \
	"$(INTDIR)\update.obj"

"$(OUTDIR)\mordor.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

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
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\mordor.exe"

CLEAN : 
	-@erase "$(INTDIR)\action.obj"
	-@erase "$(INTDIR)\bank.obj"
	-@erase "$(INTDIR)\combat.obj"
	-@erase "$(INTDIR)\command1.obj"
	-@erase "$(INTDIR)\command10.obj"
	-@erase "$(INTDIR)\command11.obj"
	-@erase "$(INTDIR)\command12.obj"
	-@erase "$(INTDIR)\command13.obj"
	-@erase "$(INTDIR)\command2.obj"
	-@erase "$(INTDIR)\command3.obj"
	-@erase "$(INTDIR)\command4.obj"
	-@erase "$(INTDIR)\command5.obj"
	-@erase "$(INTDIR)\command6.obj"
	-@erase "$(INTDIR)\command7.obj"
	-@erase "$(INTDIR)\command8.obj"
	-@erase "$(INTDIR)\command9.obj"
	-@erase "$(INTDIR)\compress.obj"
	-@erase "$(INTDIR)\creature.obj"
	-@erase "$(INTDIR)\dm1.obj"
	-@erase "$(INTDIR)\dm2.obj"
	-@erase "$(INTDIR)\dm3.obj"
	-@erase "$(INTDIR)\dm4.obj"
	-@erase "$(INTDIR)\dm5.obj"
	-@erase "$(INTDIR)\dm6.obj"
	-@erase "$(INTDIR)\dm7.obj"
	-@erase "$(INTDIR)\Errhandl.obj"
	-@erase "$(INTDIR)\files1.obj"
	-@erase "$(INTDIR)\files2.obj"
	-@erase "$(INTDIR)\files3.obj"
	-@erase "$(INTDIR)\files4.obj"
	-@erase "$(INTDIR)\global.obj"
	-@erase "$(INTDIR)\io.obj"
	-@erase "$(INTDIR)\logic.obj"
	-@erase "$(INTDIR)\magic1.obj"
	-@erase "$(INTDIR)\magic2.obj"
	-@erase "$(INTDIR)\magic3.obj"
	-@erase "$(INTDIR)\magic4.obj"
	-@erase "$(INTDIR)\magic5.obj"
	-@erase "$(INTDIR)\magic6.obj"
	-@erase "$(INTDIR)\magic7.obj"
	-@erase "$(INTDIR)\magic8.obj"
	-@erase "$(INTDIR)\magic9.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\memory.obj"
	-@erase "$(INTDIR)\misc.obj"
	-@erase "$(INTDIR)\object.obj"
	-@erase "$(INTDIR)\player.obj"
	-@erase "$(INTDIR)\post.obj"
	-@erase "$(INTDIR)\Readini.obj"
	-@erase "$(INTDIR)\room.obj"
	-@erase "$(INTDIR)\screen.obj"
	-@erase "$(INTDIR)\special1.obj"
	-@erase "$(INTDIR)\strstr.obj"
	-@erase "$(INTDIR)\talk.obj"
	-@erase "$(INTDIR)\update.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\mordor.exe"
	-@erase "$(OUTDIR)\mordor.ilk"
	-@erase "$(OUTDIR)\mordor.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "DEBUG" /D "WIN32" /D "_CONSOLE" /D "COMPRESS" /D "WINNT" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "DEBUG" /D "WIN32" /D\
 "_CONSOLE" /D "COMPRESS" /D "WINNT" /Fp"$(INTDIR)/mordor.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mordor.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib wsock32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)/mordor.pdb" /debug /machine:I386 /out:"$(OUTDIR)/mordor.exe" 
LINK32_OBJS= \
	"$(INTDIR)\action.obj" \
	"$(INTDIR)\bank.obj" \
	"$(INTDIR)\combat.obj" \
	"$(INTDIR)\command1.obj" \
	"$(INTDIR)\command10.obj" \
	"$(INTDIR)\command11.obj" \
	"$(INTDIR)\command12.obj" \
	"$(INTDIR)\command13.obj" \
	"$(INTDIR)\command2.obj" \
	"$(INTDIR)\command3.obj" \
	"$(INTDIR)\command4.obj" \
	"$(INTDIR)\command5.obj" \
	"$(INTDIR)\command6.obj" \
	"$(INTDIR)\command7.obj" \
	"$(INTDIR)\command8.obj" \
	"$(INTDIR)\command9.obj" \
	"$(INTDIR)\compress.obj" \
	"$(INTDIR)\creature.obj" \
	"$(INTDIR)\dm1.obj" \
	"$(INTDIR)\dm2.obj" \
	"$(INTDIR)\dm3.obj" \
	"$(INTDIR)\dm4.obj" \
	"$(INTDIR)\dm5.obj" \
	"$(INTDIR)\dm6.obj" \
	"$(INTDIR)\dm7.obj" \
	"$(INTDIR)\Errhandl.obj" \
	"$(INTDIR)\files1.obj" \
	"$(INTDIR)\files2.obj" \
	"$(INTDIR)\files3.obj" \
	"$(INTDIR)\files4.obj" \
	"$(INTDIR)\global.obj" \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\logic.obj" \
	"$(INTDIR)\magic1.obj" \
	"$(INTDIR)\magic2.obj" \
	"$(INTDIR)\magic3.obj" \
	"$(INTDIR)\magic4.obj" \
	"$(INTDIR)\magic5.obj" \
	"$(INTDIR)\magic6.obj" \
	"$(INTDIR)\magic7.obj" \
	"$(INTDIR)\magic8.obj" \
	"$(INTDIR)\magic9.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\memory.obj" \
	"$(INTDIR)\misc.obj" \
	"$(INTDIR)\object.obj" \
	"$(INTDIR)\player.obj" \
	"$(INTDIR)\post.obj" \
	"$(INTDIR)\Readini.obj" \
	"$(INTDIR)\room.obj" \
	"$(INTDIR)\screen.obj" \
	"$(INTDIR)\special1.obj" \
	"$(INTDIR)\strstr.obj" \
	"$(INTDIR)\talk.obj" \
	"$(INTDIR)\update.obj"

"$(OUTDIR)\mordor.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mordor - Win32 Service"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "mordor__"
# PROP BASE Intermediate_Dir "mordor__"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\mordornt.exe"

CLEAN : 
	-@erase "$(INTDIR)\action.obj"
	-@erase "$(INTDIR)\bank.obj"
	-@erase "$(INTDIR)\combat.obj"
	-@erase "$(INTDIR)\command1.obj"
	-@erase "$(INTDIR)\command10.obj"
	-@erase "$(INTDIR)\command11.obj"
	-@erase "$(INTDIR)\command12.obj"
	-@erase "$(INTDIR)\command13.obj"
	-@erase "$(INTDIR)\command2.obj"
	-@erase "$(INTDIR)\command3.obj"
	-@erase "$(INTDIR)\command4.obj"
	-@erase "$(INTDIR)\command5.obj"
	-@erase "$(INTDIR)\command6.obj"
	-@erase "$(INTDIR)\command7.obj"
	-@erase "$(INTDIR)\command8.obj"
	-@erase "$(INTDIR)\command9.obj"
	-@erase "$(INTDIR)\compress.obj"
	-@erase "$(INTDIR)\creature.obj"
	-@erase "$(INTDIR)\dm1.obj"
	-@erase "$(INTDIR)\dm2.obj"
	-@erase "$(INTDIR)\dm3.obj"
	-@erase "$(INTDIR)\dm4.obj"
	-@erase "$(INTDIR)\dm5.obj"
	-@erase "$(INTDIR)\dm6.obj"
	-@erase "$(INTDIR)\dm7.obj"
	-@erase "$(INTDIR)\Errhandl.obj"
	-@erase "$(INTDIR)\files1.obj"
	-@erase "$(INTDIR)\files2.obj"
	-@erase "$(INTDIR)\files3.obj"
	-@erase "$(INTDIR)\files4.obj"
	-@erase "$(INTDIR)\global.obj"
	-@erase "$(INTDIR)\io.obj"
	-@erase "$(INTDIR)\logic.obj"
	-@erase "$(INTDIR)\magic1.obj"
	-@erase "$(INTDIR)\magic2.obj"
	-@erase "$(INTDIR)\magic3.obj"
	-@erase "$(INTDIR)\magic4.obj"
	-@erase "$(INTDIR)\magic5.obj"
	-@erase "$(INTDIR)\magic6.obj"
	-@erase "$(INTDIR)\magic7.obj"
	-@erase "$(INTDIR)\magic8.obj"
	-@erase "$(INTDIR)\magic9.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\memory.obj"
	-@erase "$(INTDIR)\misc.obj"
	-@erase "$(INTDIR)\object.obj"
	-@erase "$(INTDIR)\player.obj"
	-@erase "$(INTDIR)\post.obj"
	-@erase "$(INTDIR)\Readini.obj"
	-@erase "$(INTDIR)\room.obj"
	-@erase "$(INTDIR)\screen.obj"
	-@erase "$(INTDIR)\special1.obj"
	-@erase "$(INTDIR)\strstr.obj"
	-@erase "$(INTDIR)\talk.obj"
	-@erase "$(INTDIR)\update.obj"
	-@erase "$(OUTDIR)\mordornt.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "COMPRESS" /D "DEBUG" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WINNT_SERVICE" /D "WIN32" /D "_CONSOLE" /D "COMPRESS" /D "WINNT" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "NDEBUG" /D "WINNT_SERVICE" /D "WIN32" /D\
 "_CONSOLE" /D "COMPRESS" /D "WINNT" /Fp"$(INTDIR)/mordor.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mordor.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /machine:I386 /out:"release/mordornt.exe"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib wsock32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)/mordornt.pdb" /machine:I386 /out:"$(OUTDIR)/mordornt.exe" 
LINK32_OBJS= \
	"$(INTDIR)\action.obj" \
	"$(INTDIR)\bank.obj" \
	"$(INTDIR)\combat.obj" \
	"$(INTDIR)\command1.obj" \
	"$(INTDIR)\command10.obj" \
	"$(INTDIR)\command11.obj" \
	"$(INTDIR)\command12.obj" \
	"$(INTDIR)\command13.obj" \
	"$(INTDIR)\command2.obj" \
	"$(INTDIR)\command3.obj" \
	"$(INTDIR)\command4.obj" \
	"$(INTDIR)\command5.obj" \
	"$(INTDIR)\command6.obj" \
	"$(INTDIR)\command7.obj" \
	"$(INTDIR)\command8.obj" \
	"$(INTDIR)\command9.obj" \
	"$(INTDIR)\compress.obj" \
	"$(INTDIR)\creature.obj" \
	"$(INTDIR)\dm1.obj" \
	"$(INTDIR)\dm2.obj" \
	"$(INTDIR)\dm3.obj" \
	"$(INTDIR)\dm4.obj" \
	"$(INTDIR)\dm5.obj" \
	"$(INTDIR)\dm6.obj" \
	"$(INTDIR)\dm7.obj" \
	"$(INTDIR)\Errhandl.obj" \
	"$(INTDIR)\files1.obj" \
	"$(INTDIR)\files2.obj" \
	"$(INTDIR)\files3.obj" \
	"$(INTDIR)\files4.obj" \
	"$(INTDIR)\global.obj" \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\logic.obj" \
	"$(INTDIR)\magic1.obj" \
	"$(INTDIR)\magic2.obj" \
	"$(INTDIR)\magic3.obj" \
	"$(INTDIR)\magic4.obj" \
	"$(INTDIR)\magic5.obj" \
	"$(INTDIR)\magic6.obj" \
	"$(INTDIR)\magic7.obj" \
	"$(INTDIR)\magic8.obj" \
	"$(INTDIR)\magic9.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\memory.obj" \
	"$(INTDIR)\misc.obj" \
	"$(INTDIR)\object.obj" \
	"$(INTDIR)\player.obj" \
	"$(INTDIR)\post.obj" \
	"$(INTDIR)\Readini.obj" \
	"$(INTDIR)\room.obj" \
	"$(INTDIR)\screen.obj" \
	"$(INTDIR)\special1.obj" \
	"$(INTDIR)\strstr.obj" \
	"$(INTDIR)\talk.obj" \
	"$(INTDIR)\update.obj"

"$(OUTDIR)\mordornt.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "makecf - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "makecf\Release"
# PROP BASE Intermediate_Dir "makecf\Release"
# PROP BASE Target_Dir "makecf"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir "makecf"
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\makecf.exe"

CLEAN : 
	-@erase "$(INTDIR)\makecf.obj"
	-@erase "$(OUTDIR)\makecf.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/makecf.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/makecf.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)/makecf.pdb" /machine:I386 /out:"$(OUTDIR)/makecf.exe" 
LINK32_OBJS= \
	"$(INTDIR)\makecf.obj"

"$(OUTDIR)\makecf.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "makecf - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "makecf\Debug"
# PROP BASE Intermediate_Dir "makecf\Debug"
# PROP BASE Target_Dir "makecf"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir "makecf"
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\makecf.exe"

CLEAN : 
	-@erase "$(INTDIR)\makecf.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\makecf.exe"
	-@erase "$(OUTDIR)\makecf.ilk"
	-@erase "$(OUTDIR)\makecf.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/makecf.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/makecf.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)/makecf.pdb" /debug /machine:I386 /out:"$(OUTDIR)/makecf.exe" 
LINK32_OBJS= \
	"$(INTDIR)\makecf.obj"

"$(OUTDIR)\makecf.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Service - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Service\Release"
# PROP BASE Intermediate_Dir "Service\Release"
# PROP BASE Target_Dir "Service"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir "Service"
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\Service.exe"

CLEAN : 
	-@erase "$(INTDIR)\Errhandl.obj"
	-@erase "$(INTDIR)\Service.obj"
	-@erase "$(OUTDIR)\Service.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/Service.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Service.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)/Service.pdb" /machine:I386 /out:"$(OUTDIR)/Service.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Errhandl.obj" \
	"$(INTDIR)\Service.obj"

"$(OUTDIR)\Service.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Service - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Service\Debug"
# PROP BASE Intermediate_Dir "Service\Debug"
# PROP BASE Target_Dir "Service"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir "Service"
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\Service.exe"

CLEAN : 
	-@erase "$(INTDIR)\Errhandl.obj"
	-@erase "$(INTDIR)\Service.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\Service.exe"
	-@erase "$(OUTDIR)\Service.ilk"
	-@erase "$(OUTDIR)\Service.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/Service.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Service.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)/Service.pdb" /debug /machine:I386 /out:"$(OUTDIR)/Service.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Errhandl.obj" \
	"$(INTDIR)\Service.obj"

"$(OUTDIR)\Service.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "mordor - Win32 Release"
# Name "mordor - Win32 Debug"
# Name "mordor - Win32 Service"

!IF  "$(CFG)" == "mordor - Win32 Release"

!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"

!ELSEIF  "$(CFG)" == "mordor - Win32 Service"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\command9.c
DEP_CPP_COMMA=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_COMMA=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\command9.obj" : $(SOURCE) $(DEP_CPP_COMMA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\command9.obj" : $(SOURCE) $(DEP_CPP_COMMA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\command9.obj" : $(SOURCE) $(DEP_CPP_COMMA) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\command10.c
DEP_CPP_COMMAN=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_COMMAN=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\command10.obj" : $(SOURCE) $(DEP_CPP_COMMAN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\command10.obj" : $(SOURCE) $(DEP_CPP_COMMAN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\command10.obj" : $(SOURCE) $(DEP_CPP_COMMAN) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\command11.c
DEP_CPP_COMMAND=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	{$(INCLUDE)}"\Sys\Stat.h"\
	{$(INCLUDE)}"\Sys\Types.h"\
	
NODEP_CPP_COMMAND=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\command11.obj" : $(SOURCE) $(DEP_CPP_COMMAND) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\command11.obj" : $(SOURCE) $(DEP_CPP_COMMAND) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\command11.obj" : $(SOURCE) $(DEP_CPP_COMMAND) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\command12.c
DEP_CPP_COMMAND1=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_COMMAND1=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\command12.obj" : $(SOURCE) $(DEP_CPP_COMMAND1) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\command12.obj" : $(SOURCE) $(DEP_CPP_COMMAND1) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\command12.obj" : $(SOURCE) $(DEP_CPP_COMMAND1) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\command2.c
DEP_CPP_COMMAND2=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\command2.obj" : $(SOURCE) $(DEP_CPP_COMMAND2) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\command2.obj" : $(SOURCE) $(DEP_CPP_COMMAND2) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\command2.obj" : $(SOURCE) $(DEP_CPP_COMMAND2) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\command3.c
DEP_CPP_COMMAND3=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_COMMAND3=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\command3.obj" : $(SOURCE) $(DEP_CPP_COMMAND3) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\command3.obj" : $(SOURCE) $(DEP_CPP_COMMAND3) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\command3.obj" : $(SOURCE) $(DEP_CPP_COMMAND3) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\command4.c
DEP_CPP_COMMAND4=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_COMMAND4=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\command4.obj" : $(SOURCE) $(DEP_CPP_COMMAND4) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\command4.obj" : $(SOURCE) $(DEP_CPP_COMMAND4) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\command4.obj" : $(SOURCE) $(DEP_CPP_COMMAND4) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\command5.c
DEP_CPP_COMMAND5=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_COMMAND5=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\command5.obj" : $(SOURCE) $(DEP_CPP_COMMAND5) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\command5.obj" : $(SOURCE) $(DEP_CPP_COMMAND5) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\command5.obj" : $(SOURCE) $(DEP_CPP_COMMAND5) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\command6.c
DEP_CPP_COMMAND6=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_COMMAND6=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\command6.obj" : $(SOURCE) $(DEP_CPP_COMMAND6) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\command6.obj" : $(SOURCE) $(DEP_CPP_COMMAND6) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\command6.obj" : $(SOURCE) $(DEP_CPP_COMMAND6) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\command7.c
DEP_CPP_COMMAND7=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_COMMAND7=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\command7.obj" : $(SOURCE) $(DEP_CPP_COMMAND7) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\command7.obj" : $(SOURCE) $(DEP_CPP_COMMAND7) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\command7.obj" : $(SOURCE) $(DEP_CPP_COMMAND7) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\command8.c
DEP_CPP_COMMAND8=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_COMMAND8=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\command8.obj" : $(SOURCE) $(DEP_CPP_COMMAND8) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\command8.obj" : $(SOURCE) $(DEP_CPP_COMMAND8) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\command8.obj" : $(SOURCE) $(DEP_CPP_COMMAND8) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\command1.c
DEP_CPP_COMMAND1_=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_COMMAND1_=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\command1.obj" : $(SOURCE) $(DEP_CPP_COMMAND1_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\command1.obj" : $(SOURCE) $(DEP_CPP_COMMAND1_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\command1.obj" : $(SOURCE) $(DEP_CPP_COMMAND1_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\combat.c
DEP_CPP_COMBA=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_COMBA=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\combat.obj" : $(SOURCE) $(DEP_CPP_COMBA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\combat.obj" : $(SOURCE) $(DEP_CPP_COMBA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\combat.obj" : $(SOURCE) $(DEP_CPP_COMBA) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\compress.c
DEP_CPP_COMPR=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_COMPR=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\compress.obj" : $(SOURCE) $(DEP_CPP_COMPR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\compress.obj" : $(SOURCE) $(DEP_CPP_COMPR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\compress.obj" : $(SOURCE) $(DEP_CPP_COMPR) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\creature.c
DEP_CPP_CREAT=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_CREAT=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\creature.obj" : $(SOURCE) $(DEP_CPP_CREAT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\creature.obj" : $(SOURCE) $(DEP_CPP_CREAT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\creature.obj" : $(SOURCE) $(DEP_CPP_CREAT) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dm1.c
DEP_CPP_DM1_C=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_DM1_C=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\dm1.obj" : $(SOURCE) $(DEP_CPP_DM1_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\dm1.obj" : $(SOURCE) $(DEP_CPP_DM1_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\dm1.obj" : $(SOURCE) $(DEP_CPP_DM1_C) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dm2.c
DEP_CPP_DM2_C=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_DM2_C=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\dm2.obj" : $(SOURCE) $(DEP_CPP_DM2_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\dm2.obj" : $(SOURCE) $(DEP_CPP_DM2_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\dm2.obj" : $(SOURCE) $(DEP_CPP_DM2_C) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dm3.c
DEP_CPP_DM3_C=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_DM3_C=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\dm3.obj" : $(SOURCE) $(DEP_CPP_DM3_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\dm3.obj" : $(SOURCE) $(DEP_CPP_DM3_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\dm3.obj" : $(SOURCE) $(DEP_CPP_DM3_C) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dm4.c
DEP_CPP_DM4_C=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_DM4_C=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\dm4.obj" : $(SOURCE) $(DEP_CPP_DM4_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\dm4.obj" : $(SOURCE) $(DEP_CPP_DM4_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\dm4.obj" : $(SOURCE) $(DEP_CPP_DM4_C) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dm5.c
DEP_CPP_DM5_C=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_DM5_C=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\dm5.obj" : $(SOURCE) $(DEP_CPP_DM5_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\dm5.obj" : $(SOURCE) $(DEP_CPP_DM5_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\dm5.obj" : $(SOURCE) $(DEP_CPP_DM5_C) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dm6.c
DEP_CPP_DM6_C=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_DM6_C=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\dm6.obj" : $(SOURCE) $(DEP_CPP_DM6_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\dm6.obj" : $(SOURCE) $(DEP_CPP_DM6_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\dm6.obj" : $(SOURCE) $(DEP_CPP_DM6_C) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\files1.c
DEP_CPP_FILES=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_FILES=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\files1.obj" : $(SOURCE) $(DEP_CPP_FILES) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\files1.obj" : $(SOURCE) $(DEP_CPP_FILES) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\files1.obj" : $(SOURCE) $(DEP_CPP_FILES) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\files2.c
DEP_CPP_FILES2=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_FILES2=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\files2.obj" : $(SOURCE) $(DEP_CPP_FILES2) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\files2.obj" : $(SOURCE) $(DEP_CPP_FILES2) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\files2.obj" : $(SOURCE) $(DEP_CPP_FILES2) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\files3.c
DEP_CPP_FILES3=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_FILES3=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\files3.obj" : $(SOURCE) $(DEP_CPP_FILES3) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\files3.obj" : $(SOURCE) $(DEP_CPP_FILES3) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\files3.obj" : $(SOURCE) $(DEP_CPP_FILES3) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\global.c
DEP_CPP_GLOBA=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_GLOBA=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\global.obj" : $(SOURCE) $(DEP_CPP_GLOBA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\global.obj" : $(SOURCE) $(DEP_CPP_GLOBA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\global.obj" : $(SOURCE) $(DEP_CPP_GLOBA) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\io.c

!IF  "$(CFG)" == "mordor - Win32 Release"

DEP_CPP_IO_C32=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	".\version.h"\
	{$(INCLUDE)}"\Sys\Types.h"\
	
NODEP_CPP_IO_C32=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\io.obj" : $(SOURCE) $(DEP_CPP_IO_C32) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"

DEP_CPP_IO_C32=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	".\version.h"\
	{$(INCLUDE)}"\Sys\Types.h"\
	
NODEP_CPP_IO_C32=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\io.obj" : $(SOURCE) $(DEP_CPP_IO_C32) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"

DEP_CPP_IO_C32=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	".\version.h"\
	{$(INCLUDE)}"\Sys\Types.h"\
	
NODEP_CPP_IO_C32=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\io.obj" : $(SOURCE) $(DEP_CPP_IO_C32) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\logic.c
DEP_CPP_LOGIC=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_LOGIC=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\logic.obj" : $(SOURCE) $(DEP_CPP_LOGIC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\logic.obj" : $(SOURCE) $(DEP_CPP_LOGIC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\logic.obj" : $(SOURCE) $(DEP_CPP_LOGIC) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\magic1.c
DEP_CPP_MAGIC=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_MAGIC=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\magic1.obj" : $(SOURCE) $(DEP_CPP_MAGIC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\magic1.obj" : $(SOURCE) $(DEP_CPP_MAGIC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\magic1.obj" : $(SOURCE) $(DEP_CPP_MAGIC) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\magic2.c
DEP_CPP_MAGIC2=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_MAGIC2=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\magic2.obj" : $(SOURCE) $(DEP_CPP_MAGIC2) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\magic2.obj" : $(SOURCE) $(DEP_CPP_MAGIC2) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\magic2.obj" : $(SOURCE) $(DEP_CPP_MAGIC2) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\magic3.c
DEP_CPP_MAGIC3=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_MAGIC3=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\magic3.obj" : $(SOURCE) $(DEP_CPP_MAGIC3) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\magic3.obj" : $(SOURCE) $(DEP_CPP_MAGIC3) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\magic3.obj" : $(SOURCE) $(DEP_CPP_MAGIC3) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\magic4.c
DEP_CPP_MAGIC4=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_MAGIC4=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\magic4.obj" : $(SOURCE) $(DEP_CPP_MAGIC4) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\magic4.obj" : $(SOURCE) $(DEP_CPP_MAGIC4) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\magic4.obj" : $(SOURCE) $(DEP_CPP_MAGIC4) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\magic5.c
DEP_CPP_MAGIC5=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_MAGIC5=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\magic5.obj" : $(SOURCE) $(DEP_CPP_MAGIC5) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\magic5.obj" : $(SOURCE) $(DEP_CPP_MAGIC5) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\magic5.obj" : $(SOURCE) $(DEP_CPP_MAGIC5) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\magic6.c
DEP_CPP_MAGIC6=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_MAGIC6=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\magic6.obj" : $(SOURCE) $(DEP_CPP_MAGIC6) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\magic6.obj" : $(SOURCE) $(DEP_CPP_MAGIC6) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\magic6.obj" : $(SOURCE) $(DEP_CPP_MAGIC6) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\magic7.c
DEP_CPP_MAGIC7=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_MAGIC7=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\magic7.obj" : $(SOURCE) $(DEP_CPP_MAGIC7) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\magic7.obj" : $(SOURCE) $(DEP_CPP_MAGIC7) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\magic7.obj" : $(SOURCE) $(DEP_CPP_MAGIC7) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\magic8.c
DEP_CPP_MAGIC8=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_MAGIC8=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\magic8.obj" : $(SOURCE) $(DEP_CPP_MAGIC8) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\magic8.obj" : $(SOURCE) $(DEP_CPP_MAGIC8) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\magic8.obj" : $(SOURCE) $(DEP_CPP_MAGIC8) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\main.c

!IF  "$(CFG)" == "mordor - Win32 Release"

DEP_CPP_MAIN_=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	".\version.h"\
	
NODEP_CPP_MAIN_=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"

DEP_CPP_MAIN_=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	".\version.h"\
	
NODEP_CPP_MAIN_=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"

DEP_CPP_MAIN_=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	".\version.h"\
	
NODEP_CPP_MAIN_=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\memory.c
DEP_CPP_MEMOR=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_MEMOR=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\memory.obj" : $(SOURCE) $(DEP_CPP_MEMOR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\memory.obj" : $(SOURCE) $(DEP_CPP_MEMOR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\memory.obj" : $(SOURCE) $(DEP_CPP_MEMOR) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc.c
DEP_CPP_MISC_=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	{$(INCLUDE)}"\Sys\Stat.h"\
	{$(INCLUDE)}"\Sys\Types.h"\
	
NODEP_CPP_MISC_=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\misc.obj" : $(SOURCE) $(DEP_CPP_MISC_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\misc.obj" : $(SOURCE) $(DEP_CPP_MISC_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\misc.obj" : $(SOURCE) $(DEP_CPP_MISC_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\object.c
DEP_CPP_OBJEC=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_OBJEC=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\object.obj" : $(SOURCE) $(DEP_CPP_OBJEC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\object.obj" : $(SOURCE) $(DEP_CPP_OBJEC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\object.obj" : $(SOURCE) $(DEP_CPP_OBJEC) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\player.c
DEP_CPP_PLAYE=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_PLAYE=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\player.obj" : $(SOURCE) $(DEP_CPP_PLAYE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\player.obj" : $(SOURCE) $(DEP_CPP_PLAYE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\player.obj" : $(SOURCE) $(DEP_CPP_PLAYE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\post.c
DEP_CPP_POST_=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_POST_=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\post.obj" : $(SOURCE) $(DEP_CPP_POST_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\post.obj" : $(SOURCE) $(DEP_CPP_POST_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\post.obj" : $(SOURCE) $(DEP_CPP_POST_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\room.c
DEP_CPP_ROOM_=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_ROOM_=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\room.obj" : $(SOURCE) $(DEP_CPP_ROOM_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\room.obj" : $(SOURCE) $(DEP_CPP_ROOM_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\room.obj" : $(SOURCE) $(DEP_CPP_ROOM_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\special1.c
DEP_CPP_SPECI=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_SPECI=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\special1.obj" : $(SOURCE) $(DEP_CPP_SPECI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\special1.obj" : $(SOURCE) $(DEP_CPP_SPECI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\special1.obj" : $(SOURCE) $(DEP_CPP_SPECI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\strstr.c

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\strstr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\strstr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\strstr.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\update.c
DEP_CPP_UPDAT=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	".\update.h"\
	
NODEP_CPP_UPDAT=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\update.obj" : $(SOURCE) $(DEP_CPP_UPDAT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\update.obj" : $(SOURCE) $(DEP_CPP_UPDAT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\update.obj" : $(SOURCE) $(DEP_CPP_UPDAT) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\action.c

!IF  "$(CFG)" == "mordor - Win32 Release"

DEP_CPP_ACTIO=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_ACTIO=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\action.obj" : $(SOURCE) $(DEP_CPP_ACTIO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"

DEP_CPP_ACTIO=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_ACTIO=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\action.obj" : $(SOURCE) $(DEP_CPP_ACTIO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"

DEP_CPP_ACTIO=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_ACTIO=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\action.obj" : $(SOURCE) $(DEP_CPP_ACTIO) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\command13.c
DEP_CPP_COMMAND13=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	{$(INCLUDE)}"\Sys\Stat.h"\
	{$(INCLUDE)}"\Sys\Types.h"\
	
NODEP_CPP_COMMAND13=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\command13.obj" : $(SOURCE) $(DEP_CPP_COMMAND13) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\command13.obj" : $(SOURCE) $(DEP_CPP_COMMAND13) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\command13.obj" : $(SOURCE) $(DEP_CPP_COMMAND13) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dm7.c
DEP_CPP_DM7_C=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_DM7_C=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\dm7.obj" : $(SOURCE) $(DEP_CPP_DM7_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\dm7.obj" : $(SOURCE) $(DEP_CPP_DM7_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\dm7.obj" : $(SOURCE) $(DEP_CPP_DM7_C) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\screen.c
DEP_CPP_SCREE=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\screen.obj" : $(SOURCE) $(DEP_CPP_SCREE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\screen.obj" : $(SOURCE) $(DEP_CPP_SCREE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\screen.obj" : $(SOURCE) $(DEP_CPP_SCREE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\magic9.c
DEP_CPP_MAGIC9=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_MAGIC9=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\magic9.obj" : $(SOURCE) $(DEP_CPP_MAGIC9) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\magic9.obj" : $(SOURCE) $(DEP_CPP_MAGIC9) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\magic9.obj" : $(SOURCE) $(DEP_CPP_MAGIC9) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Readini.c
DEP_CPP_READI=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_READI=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\Readini.obj" : $(SOURCE) $(DEP_CPP_READI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\Readini.obj" : $(SOURCE) $(DEP_CPP_READI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\Readini.obj" : $(SOURCE) $(DEP_CPP_READI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Errhandl.c

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\Errhandl.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\Errhandl.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\Errhandl.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\files4.c
DEP_CPP_FILES4=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\files4.obj" : $(SOURCE) $(DEP_CPP_FILES4) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\files4.obj" : $(SOURCE) $(DEP_CPP_FILES4) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\files4.obj" : $(SOURCE) $(DEP_CPP_FILES4) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\talk.c
DEP_CPP_TALK_=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_TALK_=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\talk.obj" : $(SOURCE) $(DEP_CPP_TALK_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\talk.obj" : $(SOURCE) $(DEP_CPP_TALK_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\talk.obj" : $(SOURCE) $(DEP_CPP_TALK_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\bank.c
DEP_CPP_BANK_=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	
NODEP_CPP_BANK_=\
	".\..\..\..\usr\local\include\dmalloc.h"\
	

!IF  "$(CFG)" == "mordor - Win32 Release"


"$(INTDIR)\bank.obj" : $(SOURCE) $(DEP_CPP_BANK_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"


"$(INTDIR)\bank.obj" : $(SOURCE) $(DEP_CPP_BANK_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mordor - Win32 Service"


"$(INTDIR)\bank.obj" : $(SOURCE) $(DEP_CPP_BANK_) "$(INTDIR)"


!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "makecf - Win32 Release"
# Name "makecf - Win32 Debug"

!IF  "$(CFG)" == "makecf - Win32 Release"

!ELSEIF  "$(CFG)" == "makecf - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\makecf.c

!IF  "$(CFG)" == "makecf - Win32 Release"

DEP_CPP_MAKEC=\
	".\mtype.h"\
	".\utsname.h"\
	

"$(INTDIR)\makecf.obj" : $(SOURCE) $(DEP_CPP_MAKEC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "makecf - Win32 Debug"

DEP_CPP_MAKEC=\
	".\mtype.h"\
	".\utsname.h"\
	

"$(INTDIR)\makecf.obj" : $(SOURCE) $(DEP_CPP_MAKEC) "$(INTDIR)"


!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "Service - Win32 Release"
# Name "Service - Win32 Debug"

!IF  "$(CFG)" == "Service - Win32 Release"

!ELSEIF  "$(CFG)" == "Service - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Service.c

"$(INTDIR)\Service.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Errhandl.c

"$(INTDIR)\Errhandl.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
