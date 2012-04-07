#include "StdAfx.h"
#include <process.h>
#include <math.h>  
#include "MainThread.h"
#include "rf_Protocol.h"
#include "pt_account_control.h"
#include "pt_control_client.h"
#include "pt_logspy_control.h"

CMainThread g_Main;

extern void StringOutput(const char *fmt, ...);

CMainThread::CMainThread()
{
	::srand((unsigned)time(NULL));	
}

CMainThread::~CMainThread()
{
	if ( m_pAccountDB != NULL )
	{
		delete m_pAccountDB;
		m_pAccountDB = NULL;
	}

	if ( m_pWorldDB != NULL )
	{
		delete m_pWorldDB;
		m_pWorldDB = NULL;
	}
}

bool CMainThread::InitMainThread()
{
	if(!_LoadIniFile())
	{
		::StringOutput("CMainThread : _LoadIniFile() => failed");	
		return false;
	}
	else
		::StringOutput("CMainThread : _LoadIniFile() => success");
	
	if(!_ObjectInit())
	{
		::StringOutput("CMainThread : _ObjectInit() => failed");	
		return false;
	}
	else
		::StringOutput("CMainThread : _ObjectInit() => success");	

	if(!_NetworkInit())
	{
		::StringOutput("CMainThread : _NetworkInit() => failed");	
		return false;
	}
	else
		::StringOutput("CMainThread : _NetworkInit() => success");	

	::StringOutput("!!Start!!");	

	m_tmrConnectAccount.BeginTimer(connect_term_account);
	m_tmrConnectLogSpy.BeginTimer(connect_term_account-1000);

	return true;
}



void CMainThread::MainLoop()
{
	m_Network.OnLoop();
	_ConnectingAccountServer();
//	_ConnectingLogSpy();
	Sleep(1);
}

char* CMainThread::GetProcessName()
{
	static char* pName = "ControlServer";

	return pName;
}

bool CMainThread::_LoadIniFile()
{
 	char szIP[32];
	GetPrivateProfileString("Account", "AccountAddress", "X", szIP, sizeof(szIP), ".\\Initialize\\ControlSystem.ini");
	if ( strcmp(szIP, "X") == 0 )
		return false;
	m_dwAccountIP = inet_addr(szIP);
	GetPrivateProfileString("Account", "LogSpyAddress", "X", szIP, sizeof(szIP), ".\\Initialize\\ControlSystem.ini");
	if ( strcmp(szIP, "X") == 0 )
		return false;
	m_dwLogSpyIP = inet_addr(szIP);

	//Data File..
	if(!::WriteTableData(item_tbl_num, m_tblItemData, NULL))
		return false;

	if(!m_tblItemUpgrade.ReadRecord(".\\Script\\ItemUpgrade.dat", &m_tblItemData[tbl_code_res]))
		return false;

	return true;
}

bool CMainThread::_ObjectInit()
{
	for(DWORD i = 0; i < MAX_ADMIN_PER_GLOBAL; i++)
	{
		m_AdminData[i].InitData(i);
	}

	for(i = 0; i < MAX_WORLD_PER_GLOBAL; i++)
	{
		m_WorldSetData[i].InitWorld(i);
	}

	for(i = 0; i < KEY_NUM; i++)
	{
		DWORD dwR = ::rand();
		m_dwMasterKey[i] = (dwR<<16) + ::rand();
	}

	m_bLoginAccount = false;

	return true;
}

bool CMainThread::_NetworkInit()
{
	_NET_TYPE_PARAM typeParam[LINE_NUM];
	
	typeParam[LINE_ACCOUNT].m_bRealSockCheck = TRUE;
//	typeParam[LINE_ACCOUNT].m_bRealClientCheck = TRUE;
	typeParam[LINE_ACCOUNT].m_bSystemLogFile = TRUE;
	typeParam[LINE_ACCOUNT].m_bServer = FALSE;
	typeParam[LINE_ACCOUNT].m_byRecvThreadNum = 1;
	typeParam[LINE_ACCOUNT].m_byRecvSleepTime = 1;
	typeParam[LINE_ACCOUNT].m_wSocketMaxNum = 1;
	typeParam[LINE_ACCOUNT].m_bSvrToS = TRUE;
	typeParam[LINE_ACCOUNT].m_bSendThread = TRUE;
	typeParam[LINE_ACCOUNT].m_bOddMsgWriteLog = TRUE;
	sprintf(typeParam[LINE_ACCOUNT].m_szModuleName, "ControlServer_AccountLine");

	typeParam[LINE_CLIENT].m_bRealSockCheck = TRUE;
//	typeParam[LINE_CLIENT].m_bRealClientCheck = TRUE;
	typeParam[LINE_CLIENT].m_bSystemLogFile = TRUE;
	typeParam[LINE_CLIENT].m_bServer = TRUE;
	typeParam[LINE_CLIENT].m_byRecvThreadNum = 1;
	typeParam[LINE_CLIENT].m_byRecvSleepTime = 1;
	typeParam[LINE_CLIENT].m_wPort = PORT_CONTROL_CLIENT;
	typeParam[LINE_CLIENT].m_wSocketMaxNum = MAX_ADMIN_PER_GLOBAL;
	typeParam[LINE_CLIENT].m_bSendThread = TRUE;
	typeParam[LINE_CLIENT].m_bOddMsgWriteLog = TRUE;
	sprintf(typeParam[LINE_CLIENT].m_szModuleName, "ControlServer_ClientLine");

	typeParam[LINE_LOGSPY].m_bRealSockCheck = TRUE;
//	typeParam[LINE_LOGSPY].m_bRealClientCheck = TRUE;
	typeParam[LINE_LOGSPY].m_bSystemLogFile = TRUE;
	typeParam[LINE_LOGSPY].m_bServer = FALSE;
	typeParam[LINE_LOGSPY].m_byRecvThreadNum = 1;
	typeParam[LINE_LOGSPY].m_byRecvSleepTime = 1;
	typeParam[LINE_LOGSPY].m_wSocketMaxNum = 1;
	typeParam[LINE_LOGSPY].m_bSendThread = TRUE;
	typeParam[LINE_LOGSPY].m_bOddMsgWriteLog = TRUE;
	sprintf(typeParam[LINE_LOGSPY].m_szModuleName, "ControlServer_LogspyLine");

	m_Network.SetNetSystem(LINE_NUM, typeParam, "ControlServer");
	return true;
}

bool CMainThread::_DatabaseInit(char* szOdbcName)
{
	m_pAccountDB = new CRFAccountDatabase();

	if ( !m_pAccountDB->StartDataBase(szOdbcName, "odinAccount", "tkfkd2du" ) )
	{
		::StringOutput("CMainThread : m_pAccountDB->StartDataBase = failed");
		return false;
	}
	else
		::StringOutput("CMainThread : m_pAccountDB->StartDataBase => success");	
	
	return true;
}

void CMainThread::_ConnectingAccountServer()
{
	if(!m_tmrConnectAccount.CountingTimer())
		return;	

	_socket* pSocket = m_Network.GetSocket(LINE_ACCOUNT, 0);

	if(pSocket->m_bAccept)
		return;

	if(!m_Network.Connect(LINE_ACCOUNT, 0, m_dwAccountIP, PORT_ACCOUNT_CONTROL))
	{
		::StringOutput("계정서버 연결실패");
	}
}

void CMainThread::_ConnectingLogSpy()
{
	if(!m_tmrConnectLogSpy.CountingTimer())
		return;	

	m_tmrConnectLogSpy.BeginTimer( connect_term_account );
	
	_socket* pSocket = m_Network.GetSocket(LINE_LOGSPY, 0);

	if(pSocket->m_bAccept)
		return;

//	if(!m_Network.Connect(LINE_LOGSPY, 0, m_dwLogSpyIP, PORT_LOGSPY_CONTROL))
	{
//		::StringOutput("LogSpy 연결실패");
	}

}

void CMainThread::LoginAccountServer()
{
	//계정서버에 로그인..
	_login_control_request_coac Send;

	memcpy(Send.dwMasterKey, m_dwMasterKey, sizeof(DWORD)*KEY_NUM);

	BYTE byType[msg_header_num] = {control_msg, login_control_request_coac};
	m_Network.m_pProcess[LINE_ACCOUNT]->LoadSendMsg(0, byType, (char*)&Send, Send.size());
}

bool CMainThread::CheckMasterKey(DWORD* pdwMasterKey)
{
	for(int i = 0; i < KEY_NUM; i++)
	{
		if(m_dwMasterKey[i] != pdwMasterKey[i])
			return false;
	}

	return true;
}

bool CMainThread::db_Load_Avator(int nWorldCode, char* pszCharName, _AVATOR_DATA* OUT pData, char* OUT pAccount, char* OUT pszErrMsg)
{
	_WORLD_DATA* pwd = &m_WorldSetData[nWorldCode];

	m_pWorldDB = new CRFWorldDatabase();

	if ( !m_pWorldDB->StartDataBase(pwd->m_szDBName, "odinWorld", "tkfkd2du" ) )
	{
		::StringOutput("CMainThread : m_pWorldDB->StartDataBase = failed");
		return false;
	}

	if(!_db_Load_Base(pszCharName, pData, pAccount, pszErrMsg))
		return false;

	if(!_db_Load_General(pData->dbAvator.m_dwRecordNum, pData, pszErrMsg))
		return false;

	if(!_db_Load_Inven(pData->dbAvator.m_dwRecordNum, pData->dbAvator.m_byBagNum, &pData->dbInven, pszErrMsg))
		return false;


	delete m_pWorldDB;
	m_pWorldDB = NULL;

	return true;
}

bool CMainThread::_db_Load_Base( char* pszCharName, _AVATOR_DATA* poutCon, char* pAccount, char* pszErrMsg )
{

	_worlddb_character_base_info dbCharacterInfo;
	memset( &dbCharacterInfo, 0, sizeof(_worlddb_character_base_info) );

	BYTE byDBRet = m_pWorldDB->Select_CharacterBaseInfoByName( pszCharName, &dbCharacterInfo );
	
	if ( byDBRet == db_result_sql_error )
	{
		sprintf( pszErrMsg, "DB에러 입니다" );
		return false;
	}
	else if ( byDBRet == db_result_no_data )
	{
		sprintf( pszErrMsg, "%s는(은) 없는 캐릭터 입니다", pszCharName );
		return false;
	}


	memcpy(poutCon->dbAvator.m_szAvatorName, pszCharName, max_name_len+1 );
	poutCon->dbAvator.m_dwRecordNum = dbCharacterInfo.dwSerial;
	poutCon->dbAvator.m_byRaceSexCode = dbCharacterInfo.byRace;
	memcpy(poutCon->dbAvator.m_szClassCode, dbCharacterInfo.szClassCode, class_code_len+1);	
	poutCon->dbAvator.m_bySlotIndex = dbCharacterInfo.bySlotIndex;	
	poutCon->dbAvator.m_byLevel = dbCharacterInfo.byLevel;
	poutCon->dbAvator.m_dwDalant = dbCharacterInfo.dwDalant;
	poutCon->dbAvator.m_dwGold = dbCharacterInfo.dwGold;
	poutCon->dbAvator.m_dwBaseShape = dbCharacterInfo.dwBaseShape;
	poutCon->dbAvator.m_dwLastConnTime = dbCharacterInfo.dwLastConnTime;
	memcpy( pAccount, dbCharacterInfo.szAccount, max_name_len+1 );	

	for( int i = 0; i < equip_fix_num; i++)
	{
		poutCon->dbAvator.m_EquipKey[i].LoadDBKey(dbCharacterInfo.shEKArray[i]);
		poutCon->dbAvator.m_dwFixEquipLv[i] = dbCharacterInfo.dwEUArray[i];
	}

	return true;
}

bool CMainThread::_db_Load_General( DWORD dwSerial, _AVATOR_DATA* poutCon, char* pszErrMsg )
{
	char szBuf[128];
	memset( szBuf, 0, sizeof(szBuf) );

	_worlddb_character_general_info dbCharacterInfo;
	memset( &dbCharacterInfo, 0, sizeof(_worlddb_character_general_info) );
	
	BYTE byDBRet = m_pWorldDB->Select_CharacterGeneralInfo( dwSerial, &dbCharacterInfo );

	if ( byDBRet == db_result_sql_error )
	{
		sprintf( pszErrMsg, "DB에러 입니다" );
		return false;
	}
	else if ( byDBRet == db_result_no_data )
	{
		sprintf( pszErrMsg, "없는 캐릭터 입니다(Code2)" );
		return false;
	}

	poutCon->dbAvator.m_dwHP = dbCharacterInfo.dwHP;
	poutCon->dbAvator.m_dwFP = dbCharacterInfo.dwFP;
	poutCon->dbAvator.m_dwSP = dbCharacterInfo.dwSP;
	poutCon->dbAvator.m_dExp = dbCharacterInfo.dExp;			
	poutCon->dbAvator.m_byBagNum = dbCharacterInfo.byBagNum;
	poutCon->dbAvator.m_byMapCode = dbCharacterInfo.byMapCode;
	poutCon->dbAvator.m_fStartPos[0] = dbCharacterInfo.fStartPos[0];
	poutCon->dbAvator.m_fStartPos[1] = dbCharacterInfo.fStartPos[1];
	poutCon->dbAvator.m_fStartPos[2] = dbCharacterInfo.fStartPos[2];
	poutCon->dbAvator.m_dwTotalPlayMin = dbCharacterInfo.dwTotalPlayMin;

	char szLeftResData[160] = {0,};
	memcpy(szLeftResData, dbCharacterInfo.szLeftResList, 160);

	poutCon->dbCutting.Init();
	if(szLeftResData != "*")
	{
		int nSize = strlen(szLeftResData);

		if(nSize%5 == 0)	//5자딱 떨어져야한다..
		{
			poutCon->dbCutting.m_byLeftNum = nSize/5;	//res의 종류수

			int nOffSet = 0;
			for(int i = 0; i < poutCon->dbCutting.m_byLeftNum; i++)
			{
				memcpy(szBuf, &szLeftResData[nOffSet], 2);
				szBuf[2] = NULL;
				nOffSet += 2;

				poutCon->dbCutting.m_List[i].byResIndex = atoi(szBuf);

				memcpy(szBuf, &szLeftResData[nOffSet], 3);
				szBuf[3] = NULL;
				nOffSet += 3;

				poutCon->dbCutting.m_List[i].byAmt = atoi(szBuf);
			}
		}
	}

	for(int i = 0; i < potion_belt_num; i++)
	{
		sprintf(szBuf, "PK%d", i);
		poutCon->dbBelt.m_PotionList[i].Key.LoadDBKey(dbCharacterInfo.lPK[i]);
	}
	for(i = 0; i < sf_linker_num; i++)
	{
		sprintf(szBuf, "LK%d", i);
		poutCon->dbBelt.m_LinkList[i].Key.LoadDBKey(dbCharacterInfo.shLK[i]);
	}
	for(i = 0; i < embellish_fix_num; i++)
	{
		sprintf(szBuf, "EK%d", i);
		poutCon->dbEquip.m_EmbellishList[i].Key.LoadDBKey(dbCharacterInfo.lEK[i]);

		sprintf(szBuf, "ED%d", i);
		poutCon->dbEquip.m_EmbellishList[i].wAmount = dbCharacterInfo.shED[i];
	}
	for(i = 0; i < force_storage_num; i++)
	{
		sprintf(szBuf, "F%d", i);
		poutCon->dbForce.m_List[i].Key.LoadDBKey(dbCharacterInfo.lF[i]);
	}
	for(i = 0; i < WP_CLASS_NUM; i++)
	{
		sprintf(szBuf, "WM%d", i);
		poutCon->dbStat.m_dwDamWpCnt[i] = dbCharacterInfo.dwWM[i];
	}
	for(i = 0; i < force_mastery_num; i++)
	{
		sprintf(szBuf, "FM%d", i);
		poutCon->dbStat.m_dwForceCum[i] = dbCharacterInfo.dwFM[i];
	}
	for(i = 0; i < max_skill_num; i++)
	{
		sprintf(szBuf, "SM%d", i);
		poutCon->dbStat.m_dwSkillCum[i] = dbCharacterInfo.dwSM[i];
	}
	for(i = 0; i < MI_CLASS_NUM; i++)
	{
		sprintf(szBuf, "MI%d", i);
		poutCon->dbStat.m_dwMakeCum[i] = dbCharacterInfo.dwMI[i];
	}

	poutCon->dbStat.m_dwSpecialCum = dbCharacterInfo.dwSR;
	poutCon->dbStat.m_dwDefenceCnt = dbCharacterInfo.dwDM;
	poutCon->dbStat.m_dwShieldCnt = dbCharacterInfo.dwPM;

	for(i = 0; i < class_history_num; i++)
	{
		poutCon->dbAvator.m_wClassHistory[i] = dbCharacterInfo.wClassHistory[i];
	}

	return true;
}

bool CMainThread::_db_Load_Inven( DWORD dwSerial, int nBagNum, _INVEN_DB_BASE* poutCon, char* pszErrMsg )
{
	_worlddb_inven_info dbInvenInfo;
	memset( &dbInvenInfo, 0, sizeof(_worlddb_inven_info) );
	
	BYTE byDBRet = m_pWorldDB->Select_Inven( dwSerial, nBagNum, &dbInvenInfo );
	
	if ( byDBRet == db_result_sql_error )
	{
		sprintf( pszErrMsg, "DB에러 입니다" );
		return false;
	}
	else if ( byDBRet == db_result_no_data )
	{
		sprintf( pszErrMsg, "없는 캐릭터 입니다(Code3)" );
		return false;
	}

	for(int i = 0; i < bag_storage_num; i++)
	{
		if(one_bag_store_num*nBagNum > i)
		{
			poutCon->m_List[i].Key.LoadDBKey(dbInvenInfo.lK[i]);
			poutCon->m_List[i].dwDur = dbInvenInfo.dwD[i];
			poutCon->m_List[i].dwUpt = dbInvenInfo.dwU[i];
		}
		else
		{
			poutCon->m_List[i].Key.SetRelease();
		}
	}

	return true;
}

void CMainThread::SendErrMsgToClient(int n, const char *fmt, ...)
{
	va_list arg_ptr;
	char str[64+1] = {0,};

	va_start(arg_ptr, fmt);
	vsprintf(str, fmt, arg_ptr);
	va_end(arg_ptr);

	_error_msg_inform_cocl Send;

	strcpy(Send.szMessage, str);

	BYTE byType[msg_header_num] = {control_msg, error_msg_inform_cocl};
	m_Network.m_pProcess[LINE_CLIENT]->LoadSendMsg(n, byType, (char*)&Send, Send.size());
}

bool CMainThread::CommandProcess(char* pszCmd)
{

	return true;
}