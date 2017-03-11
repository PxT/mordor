# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=editor - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to editor - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "editor - Win32 Release" && "$(CFG)" != "editor - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "editor.mak" CFG="editor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "editor - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "editor - Win32 Debug" (based on "Win32 (x86) Console Application")
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
RSC=rc.exe
CPP=cl.exe

!IF  "$(CFG)" == "editor - Win32 Release"

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

ALL : "$(OUTDIR)\editor.exe"

CLEAN : 
	-@erase "$(INTDIR)\compress.obj"
	-@erase "$(INTDIR)\editor.obj"
	-@erase "$(INTDIR)\editor2.obj"
	-@erase "$(INTDIR)\utils.obj"
	-@erase "$(OUTDIR)\editor.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/editor.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/editor.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)/editor.pdb" /machine:I386 /out:"$(OUTDIR)/editor.exe" 
LINK32_OBJS= \
	"$(INTDIR)\compress.obj" \
	"$(INTDIR)\editor.obj" \
	"$(INTDIR)\editor2.obj" \
	"$(INTDIR)\utils.obj"

"$(OUTDIR)\editor.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "editor - Win32 Debug"

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

ALL : "$(OUTDIR)\editor.exe"

CLEAN : 
	-@erase "$(INTDIR)\compress.obj"
	-@erase "$(INTDIR)\editor.obj"
	-@erase "$(INTDIR)\editor2.obj"
	-@erase "$(INTDIR)\utils.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\editor.exe"
	-@erase "$(OUTDIR)\editor.ilk"
	-@erase "$(OUTDIR)\editor.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/editor.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/editor.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)/editor.pdb" /debug /machine:I386 /out:"$(OUTDIR)/editor.exe" 
LINK32_OBJS= \
	"$(INTDIR)\compress.obj" \
	"$(INTDIR)\editor.obj" \
	"$(INTDIR)\editor2.obj" \
	"$(INTDIR)\utils.obj"

"$(OUTDIR)\editor.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "editor - Win32 Release"
# Name "editor - Win32 Debug"

!IF  "$(CFG)" == "editor - Win32 Release"

!ELSEIF  "$(CFG)" == "editor - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\utils.c
DEP_CPP_UTILS=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	".\utils.h"\
	

"$(INTDIR)\utils.obj" : $(SOURCE) $(DEP_CPP_UTILS) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\editor.c
DEP_CPP_EDITO=\
	".\editor.h"\
	".\mstruct.h"\
	".\mtype.h"\
	".\utils.h"\
	{$(INCLUDE)}"\sys\stat.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\editor.obj" : $(SOURCE) $(DEP_CPP_EDITO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\editor2.c
DEP_CPP_EDITOR=\
	".\editor.h"\
	".\mstruct.h"\
	".\mtype.h"\
	".\utils.h"\
	{$(INCLUDE)}"\sys\stat.h"\
	{$(INCLUDE)}"\sys\types.h"\
	

"$(INTDIR)\editor2.obj" : $(SOURCE) $(DEP_CPP_EDITOR) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\compress.c
DEP_CPP_COMPR=\
	".\mextern.h"\
	".\mstruct.h"\
	".\mtype.h"\
	

"$(INTDIR)\compress.obj" : $(SOURCE) $(DEP_CPP_COMPR) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
