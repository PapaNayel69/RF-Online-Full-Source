# Microsoft Developer Studio Project File - Name="RF_Online" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=RF_Online - Win32 Developer Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "RF_Online.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "RF_Online.mak" CFG="RF_Online - Win32 Developer Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RF_Online - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "RF_Online - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "RF_Online - Win32 Developer Debug" (based on "Win32 (x86) Application")
!MESSAGE "RF_Online - Win32 Developer Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/RF_Online_Client_2002_09_11", LVBAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "RF_Online - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"..\..\Bin\RF_Online.bin"

!ELSEIF  "$(CFG)" == "RF_Online - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_NEW_UI_" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\..\Bin\RF_Online.bin" /pdbtype:sept

!ELSEIF  "$(CFG)" == "RF_Online - Win32 Developer Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "RF_Online___Win32_Developer_Debug"
# PROP BASE Intermediate_Dir "RF_Online___Win32_Developer_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "RF_Online___Win32_Developer_Debug"
# PROP Intermediate_Dir "RF_Online___Win32_Developer_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_DEVELOPER_" /D "_NEW_UI_" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\..\Bin\Project1121_Debug.exe" /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\..\Bin\Project1121_Debug_Dev.exe" /pdbtype:sept

!ELSEIF  "$(CFG)" == "RF_Online - Win32 Developer Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "RF_Online___Win32_Developer_Release"
# PROP BASE Intermediate_Dir "RF_Online___Win32_Developer_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "RF_Online___Win32_Developer_Release"
# PROP Intermediate_Dir "RF_Online___Win32_Developer_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_DEVELOPER_" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"..\..\Bin\Project1121_Dev.exe"

!ENDIF 

# Begin Target

# Name "RF_Online - Win32 Release"
# Name "RF_Online - Win32 Debug"
# Name "RF_Online - Win32 Developer Debug"
# Name "RF_Online - Win32 Developer Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\CLand.cpp
# End Source File
# Begin Source File

SOURCE=.\CObject.cpp
# End Source File
# Begin Source File

SOURCE=.\CResObject.cpp
# End Source File
# Begin Source File

SOURCE=.\MainApp.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CCamera.h
# End Source File
# Begin Source File

SOURCE=.\CLand.h
# End Source File
# Begin Source File

SOURCE=.\CObject.h
# End Source File
# Begin Source File

SOURCE=.\CResObject.h
# End Source File
# Begin Source File

SOURCE=.\MainApp.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Character"

# PROP Default_Filter ""
# Begin Group "Avatar"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Character\Avatar\CAvatar.cpp
# End Source File
# Begin Source File

SOURCE=.\Character\Avatar\CAvatar.h
# End Source File
# Begin Source File

SOURCE=.\Character\Avatar\CAvatar_behavior.cpp
# End Source File
# Begin Source File

SOURCE=.\Character\Avatar\CAvatar_combat.cpp
# End Source File
# Begin Source File

SOURCE=.\Character\Avatar\CAvatar_etc.cpp
# End Source File
# Begin Source File

SOURCE=.\Character\Avatar\CAvatar_Etc_UI.cpp
# End Source File
# Begin Source File

SOURCE=.\Character\Avatar\CAvatar_Quest.cpp
# End Source File
# Begin Source File

SOURCE=.\Character\Avatar\CAvatar_Quest.h
# End Source File
# Begin Source File

SOURCE=.\Character\Avatar\CAvatar_RelationInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Character\Avatar\CAvatar_RelationInfo.h
# End Source File
# Begin Source File

SOURCE=.\Character\Avatar\CAvatar_RelationInfo2.cpp
# End Source File
# Begin Source File

SOURCE=.\Character\Avatar\CAvatar_RelationInfo2.h
# End Source File
# Begin Source File

SOURCE=.\Character\Avatar\CAvatar_ui_process.cpp
# End Source File
# End Group
# Begin Group "Player"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Character\Player\CBasePlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\Character\Player\CBasePlayer.h
# End Source File
# Begin Source File

SOURCE=.\Character\Player\CPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\Character\Player\CPlayer.h
# End Source File
# Begin Source File

SOURCE=.\Character\Player\CPlayer_Etc.cpp
# End Source File
# Begin Source File

SOURCE=.\Character\Player\CUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\Character\Player\CUnit.h
# End Source File
# End Group
# Begin Group "Npc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Character\Npc\CAnimus.cpp
# End Source File
# Begin Source File

SOURCE=.\Character\Npc\CAnimus.h
# End Source File
# Begin Source File

SOURCE=.\Character\Npc\CBaseNpc.cpp
# End Source File
# Begin Source File

SOURCE=.\Character\Npc\CBaseNpc.h
# End Source File
# Begin Source File

SOURCE=.\Character\Npc\CGuardTower.cpp
# End Source File
# Begin Source File

SOURCE=.\Character\Npc\CGuardTower.h
# End Source File
# Begin Source File

SOURCE=.\Character\Npc\CMonster.cpp
# End Source File
# Begin Source File

SOURCE=.\Character\Npc\CMonster.h
# End Source File
# Begin Source File

SOURCE=.\Character\Npc\CNpc.cpp
# End Source File
# Begin Source File

SOURCE=.\Character\Npc\CNpc.h
# End Source File
# End Group
# Begin Group "Base Character"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Character\Character\CCharacter.cpp
# End Source File
# Begin Source File

SOURCE=.\Character\Character\CCharacter.h
# End Source File
# Begin Source File

SOURCE=.\Character\Character\CCharacter_Etc.cpp
# End Source File
# Begin Source File

SOURCE=.\Character\Character\CCharacter_RelationInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Character\Character\CCharacter_RelationInfo.h
# End Source File
# Begin Source File

SOURCE=.\Character\Character\CCharacter_RelationInfo2.cpp
# End Source File
# Begin Source File

SOURCE=.\Character\Character\CCharacter_RelationInfo2.h
# End Source File
# End Group
# End Group
# Begin Group "Item"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Item\CItem.cpp
# End Source File
# Begin Source File

SOURCE=.\Item\CItem.h
# End Source File
# Begin Source File

SOURCE=.\item\CItemData.cpp
# End Source File
# Begin Source File

SOURCE=.\item\CItemData.h
# End Source File
# Begin Source File

SOURCE=.\Item\CItemMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Item\CItemMgr.h
# End Source File
# End Group
# Begin Group "Game Object"

# PROP Default_Filter ""
# Begin Group "Board"

# PROP Default_Filter ""
# Begin Group "Item Board"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GameObject\Board\ItemBoard\CBeltBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\GameObject\Board\ItemBoard\CBeltBoard.h
# End Source File
# Begin Source File

SOURCE=.\GameObject\Board\ItemBoard\CEquipBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\GameObject\Board\ItemBoard\CEquipBoard.h
# End Source File
# Begin Source File

SOURCE=.\GameObject\Board\ItemBoard\CInventoryBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\GameObject\Board\ItemBoard\CInventoryBoard.h
# End Source File
# Begin Source File

SOURCE=.\GameObject\Board\ItemBoard\CItemBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\GameObject\Board\ItemBoard\CItemBoard.h
# End Source File
# Begin Source File

SOURCE=.\gameobject\board\itemboard\CItemUpgradeBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobject\board\itemboard\CItemUpgradeBoard.h
# End Source File
# Begin Source File

SOURCE=.\gameobject\board\itemboard\CItemWorkBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobject\board\itemboard\CItemWorkBoard.h
# End Source File
# Begin Source File

SOURCE=.\GameObject\Board\ItemBoard\COreProcBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\GameObject\Board\ItemBoard\COreProcBoard.h
# End Source File
# End Group
# Begin Group "Skill Force Board"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GameObject\Board\SkillForceBoard\CAnimusBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\GameObject\Board\SkillForceBoard\CAnimusBoard.h
# End Source File
# Begin Source File

SOURCE=.\GameObject\Board\SkillForceBoard\CForceBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\GameObject\Board\SkillForceBoard\CForceBoard.h
# End Source File
# Begin Source File

SOURCE=.\GameObject\Board\SkillForceBoard\CSFBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\GameObject\Board\SkillForceBoard\CSFBoard.h
# End Source File
# Begin Source File

SOURCE=.\GameObject\Board\SkillForceBoard\CSkillBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\GameObject\Board\SkillForceBoard\CSkillBoard.h
# End Source File
# End Group
# Begin Group "General"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\gameobject\board\general\CBoard_Etc.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobject\board\general\CBoard_Etc.h
# End Source File
# Begin Source File

SOURCE=.\GameObject\Board\General\CGameEventBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\GameObject\Board\General\CGameEventBoard.h
# End Source File
# Begin Source File

SOURCE=.\GameObject\Board\General\CHelpBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\GameObject\Board\General\CHelpBoard.h
# End Source File
# Begin Source File

SOURCE=.\GameObject\Board\General\CPartyBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\GameObject\Board\General\CPartyBoard.h
# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GameObject\Board\Common\CBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\GameObject\Board\Common\CBoard.h
# End Source File
# Begin Source File

SOURCE=.\GameObject\Board\Common\CBoardManager.cpp
# End Source File
# Begin Source File

SOURCE=.\GameObject\Board\Common\CBoardManager.h
# End Source File
# Begin Source File

SOURCE=.\GameObject\Board\Common\CLinkBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\GameObject\Board\Common\CLinkBoard.h
# End Source File
# End Group
# Begin Group "Shop Board"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GameObject\Board\ShopBoard\CPlayerTradeBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\GameObject\Board\ShopBoard\CPlayerTradeBoard.h
# End Source File
# Begin Source File

SOURCE=.\gameobject\board\shopboard\CShopTradeBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\gameobject\board\shopboard\CShopTradeBoard.h
# End Source File
# Begin Source File

SOURCE=.\GameObject\Board\ShopBoard\CUnitTuningBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\GameObject\Board\ShopBoard\CUnitTuningBoard.h
# End Source File
# End Group
# Begin Group "ETC Board"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GameObject\Board\EtcBoard\CClassChangeBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\GameObject\Board\EtcBoard\CClassChangeBoard.h
# End Source File
# End Group
# End Group
# Begin Group "Manager"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GameObject\Manager\CCharactermgr.cpp
# End Source File
# Begin Source File

SOURCE=.\GameObject\Manager\CCharactermgr.h
# End Source File
# Begin Source File

SOURCE=.\GameObject\Manager\CGroundItemManager.cpp
# End Source File
# Begin Source File

SOURCE=.\GameObject\Manager\CGroundItemManager.h
# End Source File
# Begin Source File

SOURCE=.\GameObject\Manager\CTargetMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\GameObject\Manager\CTargetMgr.h
# End Source File
# End Group
# End Group
# Begin Group "Game Progress"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GameProgress\CGameProgress.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProgress\CGameProgress.h
# End Source File
# Begin Source File

SOURCE=.\GameProgress\CGP_CharacterCreate.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProgress\CGP_CharacterCreate.h
# End Source File
# Begin Source File

SOURCE=.\GameProgress\CGP_LogIn.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProgress\CGP_LogIn.h
# End Source File
# Begin Source File

SOURCE=.\GameProgress\CGP_MainGame.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProgress\CGP_MainGame.h
# End Source File
# Begin Source File

SOURCE=.\GameProgress\CGP_MainGame_Etc.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProgress\CGP_MainGame_ProcessInput.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProgress\CGP_Title.cpp
# End Source File
# Begin Source File

SOURCE=.\GameProgress\CGP_Title.h
# End Source File
# Begin Source File

SOURCE=.\gameprogress\CInputProcessMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\gameprogress\CInputProcessMgr.h
# End Source File
# End Group
# Begin Group "Network"

# PROP Default_Filter ""
# Begin Group "Protocol"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Network\Protocol\AIMessage.h
# End Source File
# Begin Source File

SOURCE=.\Network\Protocol\CharacterDB.h
# End Source File
# Begin Source File

SOURCE=.\Network\Protocol\ErrorEventFromServer.h
# End Source File
# Begin Source File

SOURCE=.\Network\Protocol\Protocol.h
# End Source File
# Begin Source File

SOURCE=.\Network\Protocol\pt_login_client.h
# End Source File
# Begin Source File

SOURCE=.\Network\Protocol\pt_zone_client.h
# End Source File
# End Group
# Begin Group "Network Lib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Network\NetworkLib\NetProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\NetworkLib\NetProcess.h
# End Source File
# Begin Source File

SOURCE=.\Network\NetworkLib\NetSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\NetworkLib\NetSocket.h
# End Source File
# Begin Source File

SOURCE=.\Network\NetworkLib\NetUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\NetworkLib\NetUtil.h
# End Source File
# Begin Source File

SOURCE=.\Network\NetworkLib\NetWorking.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\NetworkLib\NetWorking.h
# End Source File
# End Group
# Begin Group "Network Messsage Processor"

# PROP Default_Filter ""
# Begin Group "Party"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Network\NetMsgProcessor\Party\CNetMsgProcessor_Party.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\NetMsgProcessor\Party\CNetMsgProcessor_Party.h
# End Source File
# Begin Source File

SOURCE=.\Network\NetMsgProcessor\Party\CNetMsgProcessor_Party_Dev.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\NetMsgProcessor\Party\CNetMsgProcessor_Party_Dev.h
# End Source File
# End Group
# Begin Group "Map"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Network\NetMsgProcessor\Map\CNetMsgProcessor_Map.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\NetMsgProcessor\Map\CNetMsgProcessor_Map.h
# End Source File
# End Group
# Begin Group "Dungeon"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Network\NetMsgProcessor\Dungeon\CNetMsgProcessor_Dungeon.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\NetMsgProcessor\Dungeon\CNetMsgProcessor_Dungeon.h
# End Source File
# End Group
# Begin Group "Item NMP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Network\NetMsgProcessor\Item\CNetMsgProcessor_Item.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\NetMsgProcessor\Item\CNetMsgProcessor_Item.h
# End Source File
# Begin Source File

SOURCE=.\Network\NetMsgProcessor\Item\CNetMsgProcessor_ItemArrangement.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\NetMsgProcessor\Item\CNetMsgProcessor_ItemArrangement.h
# End Source File
# End Group
# Begin Group "Summon"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Network\NetMsgProcessor\Summon\CNetMsgProcessor_Summon.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\NetMsgProcessor\Summon\CNetMsgProcessor_Summon.h
# End Source File
# End Group
# Begin Group "Unit"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Network\NetMsgProcessor\Unit\CNetMsgProcessor_Unit.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\NetMsgProcessor\Unit\CNetMsgProcessor_Unit.h
# End Source File
# End Group
# Begin Group "Item Trade"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\network\netmsgprocessor\itemtrade\CNetMsgProcessor_ItemTrade.cpp
# End Source File
# Begin Source File

SOURCE=.\network\netmsgprocessor\itemtrade\CNetMsgProcessor_ItemTrade.h
# End Source File
# Begin Source File

SOURCE=.\network\netmsgprocessor\itemtrade\CNetMsgProcessor_ItemTradePlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\network\netmsgprocessor\itemtrade\CNetMsgProcessor_ItemTradePlayer.h
# End Source File
# End Group
# Begin Group "Combat"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Network\NetMsgProcessor\Combat\CNetMsgProcessor_Combat.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\NetMsgProcessor\Combat\CNetMsgProcessor_Combat.h
# End Source File
# End Group
# Begin Group "Resource"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\network\netmsgprocessor\Resource\CNetMsgProcessor_Resource.cpp
# End Source File
# Begin Source File

SOURCE=.\network\netmsgprocessor\Resource\CNetMsgProcessor_Resource.h
# End Source File
# End Group
# Begin Group "Event"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Network\NetMsgProcessor\Event\CNetMsgProcessor_Event.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\NetMsgProcessor\Event\CNetMsgProcessor_Event.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Network\NetMsgProcessor\CNetMsgProcessor_Base.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\NetMsgProcessor\CNetMsgProcessor_Base.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Network\CNetworkMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\CNetworkMgr.h
# End Source File
# Begin Source File

SOURCE=.\Network\CNetworkMgr_ChatMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\CNetworkMgr_EffectMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\CNetworkMgr_EventMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\CNetworkMgr_FightMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\CNetworkMgr_InitMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\CNetworkMgr_MasterServerMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\CNetworkMgr_ObserverMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\CNetworkMgr_PositionMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\Network\CNetworkMgr_SystemMsg.cpp
# End Source File
# End Group
# Begin Group "Global Data"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Define.h
# End Source File
# Begin Source File

SOURCE=.\DefineMessage.h
# End Source File
# Begin Source File

SOURCE=.\Global.cpp
# End Source File
# Begin Source File

SOURCE=.\Global.h
# End Source File
# End Group
# Begin Group "ETC"

# PROP Default_Filter ""
# Begin Group "Input"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Input\IMEEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\Input\IMEEdit.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\BaseHeader.h
# End Source File
# Begin Source File

SOURCE=.\CList.h
# End Source File
# Begin Source File

SOURCE=.\CPath.cpp
# End Source File
# Begin Source File

SOURCE=.\CPath.h
# End Source File
# Begin Source File

SOURCE=.\DataFile.cpp
# End Source File
# Begin Source File

SOURCE=.\DataFile.h
# End Source File
# End Group
# Begin Group "Data Table"

# PROP Default_Filter ""
# Begin Group "ServerTable"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DataTable\ServerTable\BaseFld.h
# End Source File
# Begin Source File

SOURCE=.\DataTable\ServerTable\ExpFld.h
# End Source File
# Begin Source File

SOURCE=.\DataTable\ServerTable\PlayerCharacterFld.h
# End Source File
# Begin Source File

SOURCE=.\DataTable\ServerTable\WeaponBulletLinkFld.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\DataTable\CCharacterDataMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\DataTable\CCharacterDataMgr.h
# End Source File
# Begin Source File

SOURCE=.\DataTable\CItemDataMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\DataTable\CItemDataMgr.h
# End Source File
# Begin Source File

SOURCE=.\datatable\CMessageMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\datatable\CMessageMgr.h
# End Source File
# Begin Source File

SOURCE=.\datatable\CResourceDataMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\datatable\CResourceDataMgr.h
# End Source File
# End Group
# Begin Group "User Interface"

# PROP Default_Filter ""
# Begin Group "GUI Controller"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\UserInterface\GUIController\GUIButton.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GUIController\GUIButton.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GUIController\GUIContainer.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GUIController\GUIContainer.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GUIController\GUIDialogBox.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GUIController\GUIDialogBox.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GUIController\GUIInput.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GUIController\GUIInput.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GUIController\GUIItem.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GUIController\GUIItem.h
# End Source File
# Begin Source File

SOURCE=.\userinterface\guicontroller\GUIListBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\guicontroller\GUIListBoard.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GUIController\GUIMessagebox.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GUIController\GUIMessagebox.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GUIController\GUIMouse.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GUIController\GUIMouse.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GUIController\GUINumInputDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GUIController\GUINumInputDlg.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GUIController\GUIObject.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GUIController\GUIObject.h
# End Source File
# Begin Source File

SOURCE=.\userinterface\guicontroller\GUIObjectEvent.h
# End Source File
# Begin Source File

SOURCE=.\userinterface\guicontroller\GUIObjectEventProc.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\guicontroller\GUIRadioButton.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\guicontroller\GUIRadioButton.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GUIController\GUIScreen.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GUIController\GUIScreen.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GUIController\GUIScrollbar.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GUIController\GUIScrollbar.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GUIController\GUISelectItem.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GUIController\GUISelectItem.h
# End Source File
# Begin Source File

SOURCE=.\userinterface\guicontroller\GUIText.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\guicontroller\GUIText.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GUIController\GUITextBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GUIController\GUITextBoard.h
# End Source File
# Begin Source File

SOURCE=.\userinterface\guicontroller\GUITooltip.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\guicontroller\GUITooltip.h
# End Source File
# Begin Source File

SOURCE=.\userinterface\guicontroller\GUITree.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\guicontroller\GUITree.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GUIController\GUIWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GUIController\GUIWindow.h
# End Source File
# End Group
# Begin Group "Game Window"

# PROP Default_Filter ""
# Begin Group "Mining"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\userinterface\gamewindow\mining\GUIMiningWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\mining\GUIMiningWindow.h
# End Source File
# End Group
# Begin Group "Npc UI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\userinterface\gamewindow\npc\GUIClassChangeWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\npc\GUIClassChangeWindow.h
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\npc\GUIExchangeWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\npc\GUIExchangeWindow.h
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\npc\GUINpcSelectWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\npc\GUINpcSelectWindow.h
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\npc\GUIOreProcWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\npc\GUIOreProcWindow.h
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\Npc\GUIShopWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\npc\guishopwindow.h
# End Source File
# End Group
# Begin Group "Recall"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\userinterface\gamewindow\Recall\GUIMobControl.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\Recall\GUIMobControl.h
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\Recall\GUIRecallWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\Recall\GUIRecallWindow.h
# End Source File
# End Group
# Begin Group "ItemProc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\UserInterface\GameWindow\ItemProc\GUIItemMakeWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\ItemProc\GUIItemMakeWindow.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GameWindow\ItemProc\GUIItemUpgradeWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\ItemProc\GUIItemUpgradeWindow.h
# End Source File
# End Group
# Begin Group "SkillForce"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\UserInterface\GameWindow\SkillForce\GUIForceWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\SkillForce\GUIForceWindow.h
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\SkillForce\GUISkillForceQueue.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\SkillForce\GUISkillForceQueue.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GameWindow\SkillForce\GUISkillWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\SkillForce\GUISkillWindow.h
# End Source File
# End Group
# Begin Group "Group"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\userinterface\gamewindow\Group\GUIPartyWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\Group\GUIPartyWindow.h
# End Source File
# End Group
# Begin Group "EtcWin"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\userinterface\gamewindow\etcwin\GUIEventWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\etcwin\GUIEventWindow.h
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\etcwin\GUIHelpWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\etcwin\GUIHelpWindow.h
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\etcwin\GUIMapLoadingWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\etcwin\GUIMapLoadingWindow.h
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\EtcWin\GUIMapMoveWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\EtcWin\GUIMapMoveWindow.h
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\etcwin\GUIMiniMapWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\etcwin\GUIMiniMapWindow.h
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\etcwin\GUIOptionWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\etcwin\GUIOptionWindow.h
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\etcwin\GUIRadarWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\etcwin\GUIRadarWindow.h
# End Source File
# End Group
# Begin Group "Player UI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\userinterface\gamewindow\player\GUITradeWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\player\GUITradeWindow.h
# End Source File
# End Group
# Begin Group "Dungeon UI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\userinterface\gamewindow\dungeon\GUIDungeon.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\dungeon\GUIDungeon.h
# End Source File
# End Group
# Begin Group "Unit UI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\userinterface\gamewindow\Unit\GUIUnitStoreWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\Unit\GUIUnitStoreWindow.h
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\Unit\GUIUnitTuningWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\Unit\GUIUnitTuningWindow.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\userinterface\gamewindow\GUIAttackPartWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\GUIAttackPartWindow.h
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\GUICharInfoWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\GUICharInfoWindow.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GameWindow\GUIChatWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GameWindow\GUIChatWindow.h
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\GUIForceBeltWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\GUIForceBeltWindow.h
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\GUIGaugeExpWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\GUIGaugeExpWindow.h
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\GUIHotKeyBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\GUIHotKeyBoard.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GameWindow\GUIInventoryWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\GameWindow\GUIInventoryWindow.h
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\GUIItemBeltWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\GUIItemBeltWindow.h
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\GUISelectedCharMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\GUISelectedCharMenu.h
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\GUISelectedCharWin.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\gamewindow\GUISelectedCharWin.h
# End Source File
# End Group
# Begin Group "LoginWindow"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\UserInterface\LogInWindow\GUICreateCharacter.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\LogInWindow\GUICreateCharacter.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\LogInWindow\GUILoadingWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\LogInWindow\GUILoadingWindow.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\LogInWindow\GUILoginWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\LogInWindow\GUILoginWindow.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\LogInWindow\GUISelectCharacter.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\LogInWindow\GUISelectCharacter.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\LogInWindow\GUISelectRace.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\LogInWindow\GUISelectRace.h
# End Source File
# End Group
# Begin Group "Global"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\UserInterface\ConsoleWin.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\ConsoleWin.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\InputMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\InputMgr.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\MouseDev.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\MouseDev.h
# End Source File
# Begin Source File

SOURCE=.\userinterface\NDQueue.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\Singleton.h
# End Source File
# Begin Source File

SOURCE=.\UserInterface\SpriteMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\UserInterface\SpriteMgr.h
# End Source File
# Begin Source File

SOURCE=.\userinterface\UIGlobal.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\UIGlobal.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\userinterface\GUIDebuggingWin.cpp
# End Source File
# Begin Source File

SOURCE=.\userinterface\GUIDebuggingWin.h
# End Source File
# End Group
# Begin Group "AddCodeByJang"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AddCodebyJang\CMagicScript.cpp
# End Source File
# Begin Source File

SOURCE=.\AddCodebyJang\CMagicScript.h
# End Source File
# Begin Source File

SOURCE=.\AddCodebyJang\CSwordTrace.cpp
# End Source File
# Begin Source File

SOURCE=.\AddCodebyJang\CSwordTrace.h
# End Source File
# Begin Source File

SOURCE=.\addcodebyjang\DummyDefine.h
# End Source File
# Begin Source File

SOURCE=.\AddCodebyJang\EffectCore.cpp
# End Source File
# Begin Source File

SOURCE=.\AddCodebyJang\EffectCore.h
# End Source File
# Begin Source File

SOURCE=.\AddCodebyJang\EffectEntityList.cpp
# End Source File
# Begin Source File

SOURCE=.\AddCodebyJang\EffectEntityList.h
# End Source File
# Begin Source File

SOURCE=.\addcodebyjang\EffectUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\addcodebyjang\EffectUtil.h
# End Source File
# Begin Source File

SOURCE=.\addcodebyjang\effectutilFont.cpp
# End Source File
# Begin Source File

SOURCE=.\addcodebyjang\effectutilFont.h
# End Source File
# Begin Source File

SOURCE=.\AddCodebyJang\ForEffect.cpp
# End Source File
# Begin Source File

SOURCE=.\AddCodebyJang\ForEffect.h
# End Source File
# Begin Source File

SOURCE=.\addcodebyjang\Itemspecialeffect.cpp
# End Source File
# Begin Source File

SOURCE=.\addcodebyjang\Itemspecialeffect.h
# End Source File
# Begin Source File

SOURCE=.\AddCodebyJang\MonsterSpecialEffect.cpp
# End Source File
# Begin Source File

SOURCE=.\AddCodebyJang\MonsterSpecialEffect.h
# End Source File
# Begin Source File

SOURCE=.\addcodebyjang\MoveEffectManager.cpp
# End Source File
# Begin Source File

SOURCE=.\addcodebyjang\MoveEffectManager.h
# End Source File
# Begin Source File

SOURCE=.\addcodebyjang\playerspecialeffect.cpp
# End Source File
# Begin Source File

SOURCE=.\addcodebyjang\playerspecialeffect.h
# End Source File
# Begin Source File

SOURCE=.\addcodebyjang\vieweffectcode.cpp
# End Source File
# Begin Source File

SOURCE=.\addcodebyjang\vieweffectcode.h
# End Source File
# End Group
# Begin Group "AddCodeByYun"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\addcodebyYun\ParticleList.cpp
# End Source File
# Begin Source File

SOURCE=.\addcodebyYun\ParticleList.h
# End Source File
# Begin Source File

SOURCE=.\addcodebyyun\Protect.cpp
# End Source File
# Begin Source File

SOURCE=.\addcodebyyun\Protect.h
# End Source File
# Begin Source File

SOURCE=.\addcodebyyun\ShadowList.cpp
# End Source File
# Begin Source File

SOURCE=.\addcodebyyun\ShadowList.h
# End Source File
# End Group
# End Target
# End Project
