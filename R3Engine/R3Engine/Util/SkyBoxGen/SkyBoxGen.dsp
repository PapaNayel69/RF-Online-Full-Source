# Microsoft Developer Studio Project File - Name="SkyBoxGen" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=SkyBoxGen - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SkyBoxGen.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SkyBoxGen.mak" CFG="SkyBoxGen - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SkyBoxGen - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "SkyBoxGen - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/ODIN/R3Technology/Util/SkyBoxGen", WOOEAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SkyBoxGen - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "..\common" /I "..\..\d3d\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /c
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"..\..\R3Bin\SkyBoxGen.exe"

!ELSEIF  "$(CFG)" == "SkyBoxGen - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\common" /I "..\..\d3d\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"..\..\R3Bin\SkyBoxGen.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "SkyBoxGen - Win32 Release"
# Name "SkyBoxGen - Win32 Debug"
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

SOURCE=..\common\loadprescript.h
# End Source File
# Begin Source File

SOURCE=..\Common\Material.h
# End Source File
# Begin Source File

SOURCE=..\Common\MeshUtil.h
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

SOURCE=..\common\loadprescript.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\Material.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\MeshUtil.cpp
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
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\Script1.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\Main.cpp
# End Source File
# Begin Source File

SOURCE=.\RESOURCE.H
# End Source File
# Begin Source File

SOURCE=..\Common\D3dframe.lib
# End Source File
# End Target
# End Project
