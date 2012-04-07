# Microsoft Developer Studio Project File - Name="EntityExporter" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=EntityExporter - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "EntityExporter.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "EntityExporter.mak" CFG="EntityExporter - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "EntityExporter - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "EntityExporter - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/ODIN/R3Technology/Util/EntityExporter", IPOEAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "EntityExporter - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "..\Common" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"..\..\R3bin\EntityExporter.exe"

!ELSEIF  "$(CFG)" == "EntityExporter - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\Common" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"..\..\R3bin\EntityExporter.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "EntityExporter - Win32 Release"
# Name "EntityExporter - Win32 Debug"
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Group "Common(headers)"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=..\common\bmplib.h
# End Source File
# Begin Source File

SOURCE=..\common\c_vector.h
# End Source File
# Begin Source File

SOURCE=..\common\d3dmath.h
# End Source File
# Begin Source File

SOURCE=..\common\ddsexport.h
# End Source File
# Begin Source File

SOURCE=..\common\entity.h
# End Source File
# Begin Source File

SOURCE=..\common\jerror.h
# End Source File
# Begin Source File

SOURCE=..\common\jmalloc.h
# End Source File
# Begin Source File

SOURCE=..\common\loadase.h
# End Source File
# Begin Source File

SOURCE=..\common\loadprescript.cpp
# End Source File
# Begin Source File

SOURCE=..\common\loadprescript.h
# End Source File
# Begin Source File

SOURCE=..\common\material.h
# End Source File
# Begin Source File

SOURCE=..\common\meshutil.h
# End Source File
# Begin Source File

SOURCE=..\common\patch.h
# End Source File
# Begin Source File

SOURCE=..\common\r3blockmat.h
# End Source File
# Begin Source File

SOURCE=..\common\r3bsputil.h
# End Source File
# Begin Source File

SOURCE=..\common\rad.h
# End Source File
# Begin Source File

SOURCE=..\common\world.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\common\bmplib.cpp
# End Source File
# Begin Source File

SOURCE=..\common\ddsexport.cpp
# End Source File
# Begin Source File

SOURCE=..\common\entity.cpp
# End Source File
# Begin Source File

SOURCE=..\common\jerror.cpp
# End Source File
# Begin Source File

SOURCE=..\common\jmalloc.cpp
# End Source File
# Begin Source File

SOURCE=..\common\loadase.cpp
# End Source File
# Begin Source File

SOURCE=..\common\material.cpp
# End Source File
# Begin Source File

SOURCE=..\common\meshutil.cpp
# End Source File
# Begin Source File

SOURCE=..\common\patch.cpp
# End Source File
# Begin Source File

SOURCE=..\common\r3blockmat.cpp
# End Source File
# Begin Source File

SOURCE=..\common\r3bsputil.cpp
# End Source File
# Begin Source File

SOURCE=..\common\rad.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\d3dframe.lib
# End Source File
# End Target
# End Project
