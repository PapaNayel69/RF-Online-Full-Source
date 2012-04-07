# Microsoft Developer Studio Project File - Name="AccountServer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AccountServer - Win32 Service Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AccountServer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AccountServer.mak" CFG="AccountServer - Win32 Service Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AccountServer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AccountServer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "AccountServer - Win32 Service Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/RF_Server/AccountServer", BKAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AccountServer - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\RF_Build\release\AccountServer"
# PROP Intermediate_Dir "..\..\RF_Build\release\AccountServer"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /O2 /I ".\\" /I "..\Protocol" /I "..\DataBase" /I "..\Network" /I "..\Common" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Ws2_32.lib winmm.lib /nologo /subsystem:windows /machine:I386 /out:"..\..\RF_BinAccountServerR.exe"

!ELSEIF  "$(CFG)" == "AccountServer - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\RF_Build\debug\AccountServer"
# PROP Intermediate_Dir "..\..\RF_Build\debug\AccountServer"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\Protocol" /I "..\DataBase" /I "..\Network" /I "..\Common" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Ws2_32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\..\RF_Bin\AccountServerD.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "AccountServer - Win32 Service Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AccountServer___Win32_Service_Debug"
# PROP BASE Intermediate_Dir "AccountServer___Win32_Service_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\RF_Build\debug\AccountServerSD"
# PROP Intermediate_Dir "..\..\RF_Build\debug\AccountServerSD"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\Protocol" /I "..\DataBase" /I "..\Network" /I "..\Common" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\Protocol" /I "..\DataBase" /I "..\Network" /I "..\Common" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "__SERVICE" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Ws2_32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\..\RF_Bin\AccountServerD.exe" /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Ws2_32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\..\RF_Bin\AccountServerSD.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "AccountServer - Win32 Release"
# Name "AccountServer - Win32 Debug"
# Name "AccountServer - Win32 Service Debug"
# Begin Group "Main"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AccountDBQrySyn.h
# End Source File
# Begin Source File

SOURCE=.\mainthread.cpp
# End Source File
# Begin Source File

SOURCE=.\MainThread.h
# End Source File
# Begin Source File

SOURCE=.\MainThread_db.cpp
# End Source File
# Begin Source File

SOURCE=.\networkex.cpp
# End Source File
# Begin Source File

SOURCE=.\networkex.h
# End Source File
# End Group
# Begin Group "Network"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\network\netprocess.cpp
# End Source File
# Begin Source File

SOURCE=..\network\netprocess.h
# End Source File
# Begin Source File

SOURCE=..\network\netsocket.cpp
# End Source File
# Begin Source File

SOURCE=..\network\netsocket.h
# End Source File
# Begin Source File

SOURCE=..\network\netutil.cpp
# End Source File
# Begin Source File

SOURCE=..\network\netutil.h
# End Source File
# Begin Source File

SOURCE=..\network\networking.cpp
# End Source File
# Begin Source File

SOURCE=..\network\networking.h
# End Source File
# End Group
# Begin Group "Protocol"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\protocol\pt_account_control.h
# End Source File
# Begin Source File

SOURCE=..\protocol\pt_account_login.h
# End Source File
# Begin Source File

SOURCE=..\protocol\pt_account_world.h
# End Source File
# Begin Source File

SOURCE=..\protocol\rf_protocol.h
# End Source File
# End Group
# Begin Group "Database"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\database\characterdb.h
# End Source File
# Begin Source File

SOURCE=..\database\rfaccountdatabase.cpp
# End Source File
# Begin Source File

SOURCE=..\database\rfaccountdatabase.h
# End Source File
# Begin Source File

SOURCE=..\database\rfnewdatabase.cpp
# End Source File
# Begin Source File

SOURCE=..\database\rfnewdatabase.h
# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\ConnNumPHMgr.h
# End Source File
# Begin Source File

SOURCE=..\common\myclassutil.cpp
# End Source File
# Begin Source File

SOURCE=..\common\myclassutil.h
# End Source File
# Begin Source File

SOURCE=..\common\myutil.cpp
# End Source File
# Begin Source File

SOURCE=..\common\myutil.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=..\common\WinMain.cpp
# End Source File
# End Group
# End Target
# End Project
