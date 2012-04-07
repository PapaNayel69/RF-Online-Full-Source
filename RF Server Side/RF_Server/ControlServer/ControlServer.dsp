# Microsoft Developer Studio Project File - Name="ControlServer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ControlServer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ControlServer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ControlServer.mak" CFG="ControlServer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ControlServer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ControlServer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/RF_Server/ControlServer", BVAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ControlServer - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\RF_Build\release\ControlServer"
# PROP Intermediate_Dir "..\..\RF_Build\release\ControlServer"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /O2 /I ".\\" /I "..\Protocol" /I "..\DataBase" /I "..\Network" /I "..\Common" /I "..\Table\Common" /I "..\Table" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Ws2_32.lib winmm.lib /nologo /subsystem:windows /machine:I386 /out:"..\..\RF_Bin\ControlServerR.exe"

!ELSEIF  "$(CFG)" == "ControlServer - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\RF_Build\debug\ControlServer"
# PROP Intermediate_Dir "..\..\RF_Build\debug\ControlServer"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\Protocol" /I "..\DataBase" /I "..\Network" /I "..\Common" /I "..\Table\Common" /I "..\Table" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Ws2_32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\..\RF_Bin\ControlServerD.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ControlServer - Win32 Release"
# Name "ControlServer - Win32 Debug"
# Begin Group "Main"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\mainthread.cpp
# End Source File
# Begin Source File

SOURCE=.\mainthread.h
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

SOURCE=..\protocol\pt_control_client.h
# End Source File
# Begin Source File

SOURCE=..\protocol\pt_logspy_control.h
# End Source File
# Begin Source File

SOURCE=..\protocol\rf_protocol.h
# End Source File
# End Group
# Begin Group "Database"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\database\characterdb.cpp
# End Source File
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
# Begin Source File

SOURCE=..\database\rfworlddatabase.cpp
# End Source File
# Begin Source File

SOURCE=..\database\rfworlddatabase.h
# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
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
# Begin Group "Table"

# PROP Default_Filter ""
# Begin Group "Re"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\table\itemloottable.h
# End Source File
# Begin Source File

SOURCE=..\table\itemmakedatatable.h
# End Source File
# Begin Source File

SOURCE=..\table\itemupgradetable.h
# End Source File
# Begin Source File

SOURCE=..\table\mapdatatable.h
# End Source File
# Begin Source File

SOURCE=..\table\orecuttingtable.h
# End Source File
# Begin Source File

SOURCE=..\table\weaponbulletlinktable.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\table\common\amuletitemfld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\AnimusFld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\AnimusItemFld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\basefld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\batteryitemfld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\BattleDungeonItemFld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\bootyitemfld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\bulletitemfld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\CharaterClassFld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\DfnEquipItemFld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\etcitemfld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\expfld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\faceitemfld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\forcefld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\forceitemfld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\itemlootfld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\itemmakedatafld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\ItemUpgradeFld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\maketoolitemfld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\mapfld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\MapItemFld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\monblockfld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\MonsterCharacterFld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\NPCharacterFld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\orecuttingfld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\oreitemfld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\playercharacterfld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\portalfld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\potionitemfld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\ResourceItemFld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\ringitemfld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\skillfld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\storelistfld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\TOWNItemFld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\UnitBulletFld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\UnitFrameFld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\UnitKeyItemFld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\UnitPartFld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\weaponbulletlinkfld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\weaponitemfld.h
# End Source File
# End Group
# End Target
# End Project
