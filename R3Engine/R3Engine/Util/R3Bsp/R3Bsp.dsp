# Microsoft Developer Studio Project File - Name="R3Bsp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=R3Bsp - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "R3Bsp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "R3Bsp.mak" CFG="R3Bsp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "R3Bsp - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "R3Bsp - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/ODIN/R3Technology/Util/R3Bsp", YLOEAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "R3Bsp - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\common" /I "..\..\d3d\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /c
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 d3dframe.lib opengl32.lib glut32.lib glu32.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"..\..\R3Bin\R3Bsp.exe"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "R3Bsp - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /GX /ZI /Od /I "..\common" /I "..\..\d3d\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /c
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib d3dframe.lib opengl32.lib glu32.lib glut32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"..\..\R3Bin\R3BspD.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "R3Bsp - Win32 Release"
# Name "R3Bsp - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\BSPRenderPart.h
# End Source File
# Begin Source File

SOURCE=..\common\d3dmath.h
# End Source File
# Begin Source File

SOURCE=.\Lightmap.h
# End Source File
# Begin Source File

SOURCE=.\R3Bsp.h
# End Source File
# Begin Source File

SOURCE=.\R3BspTool.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\BSPRenderPart.cpp
# End Source File
# Begin Source File

SOURCE=.\Lightmap.cpp
# End Source File
# Begin Source File

SOURCE=.\Main.cpp
# End Source File
# Begin Source File

SOURCE=.\R3Bsp.cpp
# End Source File
# Begin Source File

SOURCE=.\resource.rc
# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Group "Header(Common)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Common\BmpLib.H
# End Source File
# Begin Source File

SOURCE=..\Common\C_Vector.h
# End Source File
# Begin Source File

SOURCE=..\Common\DDSExport.h
# End Source File
# Begin Source File

SOURCE=..\common\entity.h
# End Source File
# Begin Source File

SOURCE=..\Common\JError.h
# End Source File
# Begin Source File

SOURCE=..\Common\Jmalloc.h
# End Source File
# Begin Source File

SOURCE=..\Common\Loadase.h
# End Source File
# Begin Source File

SOURCE=..\Common\LoadPreScript.h
# End Source File
# Begin Source File

SOURCE=..\Common\Material.h
# End Source File
# Begin Source File

SOURCE=..\Common\MeshUtil.h
# End Source File
# Begin Source File

SOURCE=..\Common\MoveCamera.h
# End Source File
# Begin Source File

SOURCE=..\Common\Patch.h
# End Source File
# Begin Source File

SOURCE=..\common\r3blockmat.h
# End Source File
# Begin Source File

SOURCE=..\Common\R3BspUtil.h
# End Source File
# Begin Source File

SOURCE=..\Common\Rad.h
# End Source File
# Begin Source File

SOURCE=..\Common\RevisitedRadix.h
# End Source File
# Begin Source File

SOURCE=..\Common\World.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\Common\Bmplib.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\DDSExport.cpp
# End Source File
# Begin Source File

SOURCE=..\common\entity.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\JError.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\Jmalloc.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\Loadase.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\LoadPreScript.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\Material.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\MeshUtil.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\MoveCamera.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\Patch.cpp
# End Source File
# Begin Source File

SOURCE=..\common\r3blockmat.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\R3BspUtil.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\Rad.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\RevisitedRadix.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\R3_32.ico
# End Source File
# Begin Source File

SOURCE=.\D3dframe.lib
# End Source File
# End Target
# End Project
