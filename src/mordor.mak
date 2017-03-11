# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

!IF "$(CFG)" == ""
CFG=mordor - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to mordor - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "mordor - Win32 Release" && "$(CFG)" != "mordor - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "mordor.mak" CFG="mordor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mordor - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "mordor - Win32 Debug" (based on "Win32 (x86) Console Application")
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
	-@erase ".\Release\mordor.exe"
	-@erase ".\Release\strstr.obj"
	-@erase ".\Release\command3.obj"
	-@erase ".\Release\magic1.obj"
	-@erase ".\Release\dm6.obj"
	-@erase ".\Release\compress.obj"
	-@erase ".\Release\files3.obj"
	-@erase ".\Release\command7.obj"
	-@erase ".\Release\memory.obj"
	-@erase ".\Release\dm1.obj"
	-@erase ".\Release\magic5.obj"
	-@erase ".\Release\action.obj"
	-@erase ".\Release\command2.obj"
	-@erase ".\Release\dm5.obj"
	-@erase ".\Release\command12.obj"
	-@erase ".\Release\files2.obj"
	-@erase ".\Release\update.obj"
	-@erase ".\Release\player.obj"
	-@erase ".\Release\command6.obj"
	-@erase ".\Release\magic4.obj"
	-@erase ".\Release\creature.obj"
	-@erase ".\Release\global.obj"
	-@erase ".\Release\misc.obj"
	-@erase ".\Release\command1.obj"
	-@erase ".\Release\logic.obj"
	-@erase ".\Release\dm4.obj"
	-@erase ".\Release\magic8.obj"
	-@erase ".\Release\command11.obj"
	-@erase ".\Release\files1.obj"
	-@erase ".\Release\command5.obj"
	-@erase ".\Release\magic3.obj"
	-@erase ".\Release\room.obj"
	-@erase ".\Release\object.obj"
	-@erase ".\Release\special1.obj"
	-@erase ".\Release\command9.obj"
	-@erase ".\Release\dm3.obj"
	-@erase ".\Release\magic7.obj"
	-@erase ".\Release\command10.obj"
	-@erase ".\Release\command4.obj"
	-@erase ".\Release\magic2.obj"
	-@erase ".\Release\post.obj"
	-@erase ".\Release\combat.obj"
	-@erase ".\Release\main.obj"
	-@erase ".\Release\command8.obj"
	-@erase ".\Release\dm2.obj"
	-@erase ".\Release\io.obj"
	-@erase ".\Release\magic6.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "COMPRESS" /D "DEBUG" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D\
 "COMPRESS" /D "DEBUG" /Fp"$(INTDIR)/mordor.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mordor.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib wsock32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)/mordor.pdb" /machine:I386 /out:"$(OUTDIR)/mordor.exe" 
LINK32_OBJS= \
	"$(INTDIR)/strstr.obj" \
	"$(INTDIR)/command3.obj" \
	"$(INTDIR)/magic1.obj" \
	"$(INTDIR)/dm6.obj" \
	"$(INTDIR)/compress.obj" \
	"$(INTDIR)/files3.obj" \
	"$(INTDIR)/command7.obj" \
	"$(INTDIR)/memory.obj" \
	"$(INTDIR)/dm1.obj" \
	"$(INTDIR)/magic5.obj" \
	"$(INTDIR)/action.obj" \
	"$(INTDIR)/command2.obj" \
	"$(INTDIR)/dm5.obj" \
	"$(INTDIR)/command12.obj" \
	"$(INTDIR)/files2.obj" \
	"$(INTDIR)/update.obj" \
	"$(INTDIR)/player.obj" \
	"$(INTDIR)/command6.obj" \
	"$(INTDIR)/magic4.obj" \
	"$(INTDIR)/creature.obj" \
	"$(INTDIR)/global.obj" \
	"$(INTDIR)/misc.obj" \
	"$(INTDIR)/command1.obj" \
	"$(INTDIR)/logic.obj" \
	"$(INTDIR)/dm4.obj" \
	"$(INTDIR)/magic8.obj" \
	"$(INTDIR)/command11.obj" \
	"$(INTDIR)/files1.obj" \
	"$(INTDIR)/command5.obj" \
	"$(INTDIR)/magic3.obj" \
	"$(INTDIR)/room.obj" \
	"$(INTDIR)/object.obj" \
	"$(INTDIR)/special1.obj" \
	"$(INTDIR)/command9.obj" \
	"$(INTDIR)/dm3.obj" \
	"$(INTDIR)/magic7.obj" \
	"$(INTDIR)/command10.obj" \
	"$(INTDIR)/command4.obj" \
	"$(INTDIR)/magic2.obj" \
	"$(INTDIR)/post.obj" \
	"$(INTDIR)/combat.obj" \
	"$(INTDIR)/main.obj" \
	"$(INTDIR)/command8.obj" \
	"$(INTDIR)/dm2.obj" \
	"$(INTDIR)/io.obj" \
	"$(INTDIR)/magic6.obj"

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
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\mordor.exe"
	-@erase ".\Debug\dm4.obj"
	-@erase ".\Debug\magic8.obj"
	-@erase ".\Debug\files1.obj"
	-@erase ".\Debug\command8.obj"
	-@erase ".\Debug\magic3.obj"
	-@erase ".\Debug\creature.obj"
	-@erase ".\Debug\command3.obj"
	-@erase ".\Debug\object.obj"
	-@erase ".\Debug\command11.obj"
	-@erase ".\Debug\memory.obj"
	-@erase ".\Debug\dm3.obj"
	-@erase ".\Debug\magic7.obj"
	-@erase ".\Debug\post.obj"
	-@erase ".\Debug\command7.obj"
	-@erase ".\Debug\io.obj"
	-@erase ".\Debug\magic2.obj"
	-@erase ".\Debug\main.obj"
	-@erase ".\Debug\combat.obj"
	-@erase ".\Debug\special1.obj"
	-@erase ".\Debug\command2.obj"
	-@erase ".\Debug\command10.obj"
	-@erase ".\Debug\player.obj"
	-@erase ".\Debug\dm2.obj"
	-@erase ".\Debug\magic6.obj"
	-@erase ".\Debug\global.obj"
	-@erase ".\Debug\room.obj"
	-@erase ".\Debug\command6.obj"
	-@erase ".\Debug\strstr.obj"
	-@erase ".\Debug\magic1.obj"
	-@erase ".\Debug\dm6.obj"
	-@erase ".\Debug\files3.obj"
	-@erase ".\Debug\command1.obj"
	-@erase ".\Debug\dm1.obj"
	-@erase ".\Debug\magic5.obj"
	-@erase ".\Debug\command5.obj"
	-@erase ".\Debug\action.obj"
	-@erase ".\Debug\compress.obj"
	-@erase ".\Debug\dm5.obj"
	-@erase ".\Debug\files2.obj"
	-@erase ".\Debug\update.obj"
	-@erase ".\Debug\command9.obj"
	-@erase ".\Debug\logic.obj"
	-@erase ".\Debug\misc.obj"
	-@erase ".\Debug\magic4.obj"
	-@erase ".\Debug\command4.obj"
	-@erase ".\Debug\command12.obj"
	-@erase ".\Debug\mordor.ilk"
	-@erase ".\Debug\mordor.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "COMPRESS" /D "DEBUG" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE"\
 /D "COMPRESS" /D "DEBUG" /Fp"$(INTDIR)/mordor.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/mordor.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib wsock32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)/mordor.pdb" /debug /machine:I386 /out:"$(OUTDIR)/mordor.exe" 
LINK32_OBJS= \
	"$(INTDIR)/dm4.obj" \
	"$(INTDIR)/magic8.obj" \
	"$(INTDIR)/files1.obj" \
	"$(INTDIR)/command8.obj" \
	"$(INTDIR)/magic3.obj" \
	"$(INTDIR)/creature.obj" \
	"$(INTDIR)/command3.obj" \
	"$(INTDIR)/object.obj" \
	"$(INTDIR)/command11.obj" \
	"$(INTDIR)/memory.obj" \
	"$(INTDIR)/dm3.obj" \
	"$(INTDIR)/magic7.obj" \
	"$(INTDIR)/post.obj" \
	"$(INTDIR)/command7.obj" \
	"$(INTDIR)/io.obj" \
	"$(INTDIR)/magic2.obj" \
	"$(INTDIR)/main.obj" \
	"$(INTDIR)/combat.obj" \
	"$(INTDIR)/special1.obj" \
	"$(INTDIR)/command2.obj" \
	"$(INTDIR)/command10.obj" \
	"$(INTDIR)/player.obj" \
	"$(INTDIR)/dm2.obj" \
	"$(INTDIR)/magic6.obj" \
	"$(INTDIR)/global.obj" \
	"$(INTDIR)/room.obj" \
	"$(INTDIR)/command6.obj" \
	"$(INTDIR)/strstr.obj" \
	"$(INTDIR)/magic1.obj" \
	"$(INTDIR)/dm6.obj" \
	"$(INTDIR)/files3.obj" \
	"$(INTDIR)/command1.obj" \
	"$(INTDIR)/dm1.obj" \
	"$(INTDIR)/magic5.obj" \
	"$(INTDIR)/command5.obj" \
	"$(INTDIR)/action.obj" \
	"$(INTDIR)/compress.obj" \
	"$(INTDIR)/dm5.obj" \
	"$(INTDIR)/files2.obj" \
	"$(INTDIR)/update.obj" \
	"$(INTDIR)/command9.obj" \
	"$(INTDIR)/logic.obj" \
	"$(INTDIR)/misc.obj" \
	"$(INTDIR)/magic4.obj" \
	"$(INTDIR)/command4.obj" \
	"$(INTDIR)/command12.obj"

"$(OUTDIR)\mordor.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

!IF  "$(CFG)" == "mordor - Win32 Release"

!ELSEIF  "$(CFG)" == "mordor - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\command9.c
DEP_CPP_COMMA=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_COMMA=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\command9.obj" : $(SOURCE) $(DEP_CPP_COMMA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\command10.c
DEP_CPP_COMMAN=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_COMMAN=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\command10.obj" : $(SOURCE) $(DEP_CPP_COMMAN) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\command11.c
DEP_CPP_COMMAND=\
	".\mstruct.h"\
	".\mextern.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\mtype.h"\
	
NODEP_CPP_COMMAND=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\command11.obj" : $(SOURCE) $(DEP_CPP_COMMAND) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\command12.c
DEP_CPP_COMMAND1=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_COMMAND1=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\command12.obj" : $(SOURCE) $(DEP_CPP_COMMAND1) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\command2.c
DEP_CPP_COMMAND2=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_COMMAND2=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\command2.obj" : $(SOURCE) $(DEP_CPP_COMMAND2) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\command3.c
DEP_CPP_COMMAND3=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_COMMAND3=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\command3.obj" : $(SOURCE) $(DEP_CPP_COMMAND3) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\command4.c
DEP_CPP_COMMAND4=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_COMMAND4=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\command4.obj" : $(SOURCE) $(DEP_CPP_COMMAND4) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\command5.c
DEP_CPP_COMMAND5=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_COMMAND5=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\command5.obj" : $(SOURCE) $(DEP_CPP_COMMAND5) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\command6.c
DEP_CPP_COMMAND6=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_COMMAND6=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\command6.obj" : $(SOURCE) $(DEP_CPP_COMMAND6) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\command7.c
DEP_CPP_COMMAND7=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_COMMAND7=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\command7.obj" : $(SOURCE) $(DEP_CPP_COMMAND7) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\command8.c
DEP_CPP_COMMAND8=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_COMMAND8=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\command8.obj" : $(SOURCE) $(DEP_CPP_COMMAND8) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\command1.c
DEP_CPP_COMMAND1_=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_COMMAND1_=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\command1.obj" : $(SOURCE) $(DEP_CPP_COMMAND1_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\combat.c
DEP_CPP_COMBA=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_COMBA=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\combat.obj" : $(SOURCE) $(DEP_CPP_COMBA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\compress.c
DEP_CPP_COMPR=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_COMPR=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\compress.obj" : $(SOURCE) $(DEP_CPP_COMPR) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\creature.c
DEP_CPP_CREAT=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_CREAT=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\creature.obj" : $(SOURCE) $(DEP_CPP_CREAT) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dm1.c
DEP_CPP_DM1_C=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_DM1_C=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\dm1.obj" : $(SOURCE) $(DEP_CPP_DM1_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dm2.c
DEP_CPP_DM2_C=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_DM2_C=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\dm2.obj" : $(SOURCE) $(DEP_CPP_DM2_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dm3.c
DEP_CPP_DM3_C=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_DM3_C=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\dm3.obj" : $(SOURCE) $(DEP_CPP_DM3_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dm4.c
DEP_CPP_DM4_C=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_DM4_C=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\dm4.obj" : $(SOURCE) $(DEP_CPP_DM4_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dm5.c
DEP_CPP_DM5_C=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_DM5_C=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\dm5.obj" : $(SOURCE) $(DEP_CPP_DM5_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dm6.c
DEP_CPP_DM6_C=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_DM6_C=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\dm6.obj" : $(SOURCE) $(DEP_CPP_DM6_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\files1.c
DEP_CPP_FILES=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_FILES=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\files1.obj" : $(SOURCE) $(DEP_CPP_FILES) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\files2.c
DEP_CPP_FILES2=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_FILES2=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\files2.obj" : $(SOURCE) $(DEP_CPP_FILES2) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\files3.c
DEP_CPP_FILES3=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_FILES3=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\files3.obj" : $(SOURCE) $(DEP_CPP_FILES3) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\global.c
DEP_CPP_GLOBA=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_GLOBA=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\global.obj" : $(SOURCE) $(DEP_CPP_GLOBA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\io.c
DEP_CPP_IO_C32=\
	{$(INCLUDE)}"\sys\Types.h"\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_IO_C32=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\io.obj" : $(SOURCE) $(DEP_CPP_IO_C32) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\logic.c
DEP_CPP_LOGIC=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_LOGIC=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\logic.obj" : $(SOURCE) $(DEP_CPP_LOGIC) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\magic1.c
DEP_CPP_MAGIC=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_MAGIC=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\magic1.obj" : $(SOURCE) $(DEP_CPP_MAGIC) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\magic2.c
DEP_CPP_MAGIC2=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_MAGIC2=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\magic2.obj" : $(SOURCE) $(DEP_CPP_MAGIC2) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\magic3.c
DEP_CPP_MAGIC3=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_MAGIC3=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\magic3.obj" : $(SOURCE) $(DEP_CPP_MAGIC3) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\magic4.c
DEP_CPP_MAGIC4=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_MAGIC4=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\magic4.obj" : $(SOURCE) $(DEP_CPP_MAGIC4) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\magic5.c
DEP_CPP_MAGIC5=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_MAGIC5=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\magic5.obj" : $(SOURCE) $(DEP_CPP_MAGIC5) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\magic6.c
DEP_CPP_MAGIC6=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_MAGIC6=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\magic6.obj" : $(SOURCE) $(DEP_CPP_MAGIC6) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\magic7.c
DEP_CPP_MAGIC7=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_MAGIC7=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\magic7.obj" : $(SOURCE) $(DEP_CPP_MAGIC7) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\magic8.c
DEP_CPP_MAGIC8=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_MAGIC8=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\magic8.obj" : $(SOURCE) $(DEP_CPP_MAGIC8) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\main.c
DEP_CPP_MAIN_=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_MAIN_=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\memory.c
DEP_CPP_MEMOR=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_MEMOR=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\memory.obj" : $(SOURCE) $(DEP_CPP_MEMOR) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\misc.c
DEP_CPP_MISC_=\
	{$(INCLUDE)}"\sys\Stat.h"\
	".\mstruct.h"\
	".\mextern.h"\
	{$(INCLUDE)}"\sys\Types.h"\
	".\mtype.h"\
	
NODEP_CPP_MISC_=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\misc.obj" : $(SOURCE) $(DEP_CPP_MISC_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\object.c
DEP_CPP_OBJEC=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_OBJEC=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\object.obj" : $(SOURCE) $(DEP_CPP_OBJEC) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\player.c
DEP_CPP_PLAYE=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_PLAYE=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\player.obj" : $(SOURCE) $(DEP_CPP_PLAYE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\post.c
DEP_CPP_POST_=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_POST_=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\post.obj" : $(SOURCE) $(DEP_CPP_POST_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\room.c
DEP_CPP_ROOM_=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_ROOM_=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\room.obj" : $(SOURCE) $(DEP_CPP_ROOM_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\special1.c
DEP_CPP_SPECI=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_SPECI=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\special1.obj" : $(SOURCE) $(DEP_CPP_SPECI) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\strstr.c

"$(INTDIR)\strstr.obj" : $(SOURCE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\update.c
DEP_CPP_UPDAT=\
	".\mstruct.h"\
	".\mextern.h"\
	".\update.h"\
	".\mtype.h"\
	
NODEP_CPP_UPDAT=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\update.obj" : $(SOURCE) $(DEP_CPP_UPDAT) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\action.c
DEP_CPP_ACTIO=\
	".\mstruct.h"\
	".\mextern.h"\
	".\mtype.h"\
	
NODEP_CPP_ACTIO=\
	".\..\..\usr\local\include\dmalloc.h"\
	

"$(INTDIR)\action.obj" : $(SOURCE) $(DEP_CPP_ACTIO) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
