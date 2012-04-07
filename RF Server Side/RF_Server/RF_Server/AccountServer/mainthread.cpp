#include <process.h>
#include <math.h>  
#include "MainThread.h"
#include "rf_Protocol.h"
#include "pt_account_login.h"
#include "pt_account_world.h"
#include "pt_account_control.h"
#include "MyUtil.h"

#ifndef __SERVICE
char* pszProcessName = "AccountServer 개발모드";
#endif

#ifdef __SERVICE
char* pszProcessName = "AccountServer 서비스모드";
#endif

static char g_szSql[1280];
CMainThread g_Main;

extern void StringOutput(const char *fmt, ...);
extern void AddOBItemTypeInt( char* szItemName, int* pnItemValue );
extern void AddOBItemTypeChar( char* szItemName, char* pnItemValue );
extern void AddOBItemTypeBool( char* szItemName, bool* pnItemValue );

CMainThread::CMainThread()
{
	m_pAccountDB = NULL;
	::srand((unsigned)time(NULL));	
	m_bDQSThread = false;
}

CMainThread::~CMainThread()
{
	ReleaseMainThread();
}

bool CMainThread::InitMainThread()
{
	m_bOperControl = false;
	m_dwWorldSetNum = 0;
	m_dwSerialCounter = 0;
	m_nControlSocketIndex = -1;
	m_bExternalLoginOpen = false;

	m_MgrConnNum.Init();

	if(!_LoadData())	{		::StringOutput("CMainThread : _LoadIniFile() => failed");			return false;	}
	if(!_NetworkInit())	{		::StringOutput("CMainThread : _NetworkInit() => failed");			return false;	}
	if(!_DatabaseInit())	{		::StringOutput("CMainThread : _DatabaseInit() => failed");			return false;		}

	m_listDQSData.SetList(max_db_syn_data_num);
	m_listDQSDataComplete.SetList(max_db_syn_data_num);
	m_listDQSDataEmpty.SetList(max_db_syn_data_num);
	for(int i = 0; i < max_db_syn_data_num; i++)
		m_listDQSDataEmpty.PushNode_Back(i);

	m_bDQSThread = true;
	::_beginthread(DQSThread, 0, (void*)this);

	::StringOutput(">>Start..");	

	return true;
}

void CMainThread::ReleaseMainThread()
{
	m_bDQSThread = false;

	if (m_pAccountDB)
	{
		delete m_pAccountDB;
		m_pAccountDB = NULL;
	}

	m_bOperControl = false;
}

void CMainThread::MainLoop()
{
	m_Network.OnLoop();
	DQSCompleteProcess();
	CheckAccount();
	WorldInfInform();
	CheckWorldPing();
	UpdateWorldServerState();
	Sleep(1);
}

char* CMainThread::GetProcessName()
{
	return pszProcessName;
}

bool CMainThread::_LoadData()
{
	//계정데이터베이스의 주소 
	GetPrivateProfileString("Database", "AccountDB_Machine", "X", m_szAccountDBMachine, sizeof(m_szAccountDBMachine), ".\\Initialize\\AccountSystem.ini");
	
	//계정데이터베이스의 이름
	GetPrivateProfileString("Database", "AccountDB_Name", "X", m_szAccountDBName, sizeof(m_szAccountDBName), ".\\Initialize\\AccountSystem.ini");

	//등록됀 월드의 수
	m_dwWorldSetNum = GetPrivateProfileInt("World", "WorldSet_Num", 1, ".\\Initialize\\AccountSystem.ini");
	m_dwServiceSetNum = GetPrivateProfileInt("World", "ServiceSet_Num", 1, ".\\Initialize\\AccountSystem.ini");

	if(m_dwWorldSetNum > MAX_WORLD_PER_GLOBAL)
	{
		::StringOutput("등록된 월드수(%d > %d).. 초과", m_dwWorldSetNum, MAX_WORLD_PER_GLOBAL);
		return false;
	}
	if(m_dwServiceSetNum > m_dwWorldSetNum)
	{
		::StringOutput("서비스 월드수(%d > %d).. 초과", m_dwServiceSetNum, m_dwWorldSetNum);
		return false;
	}
	
	::StringOutput("등록월드수(%d)", m_dwWorldSetNum);
	::StringOutput("서비스월드수(%d)", m_dwServiceSetNum);

	for(DWORD i = 0; i < m_dwWorldSetNum; i++)
	{
		char szKey[128];
		char szWorldName[128], szWorldIP[32], szDBName[128];

		//월드의 이름
		sprintf(szKey, "Name%d", i);
		GetPrivateProfileString("World", szKey, "X", szWorldName, sizeof(szWorldName), ".\\Initialize\\AccountSystem.ini");

		//월드의 주소
		sprintf(szKey, "Address%d", i);
		GetPrivateProfileString("World", szKey, "X", szWorldIP, sizeof(szWorldIP), ".\\Initialize\\AccountSystem.ini");

		//DB의 이름
		sprintf(szKey, "DBName%d", i);
		GetPrivateProfileString("World", szKey, "X", szDBName, sizeof(szDBName), ".\\Initialize\\AccountSystem.ini");

		//월드세팅
		m_WorldSetData[i].InitWorld(i, szWorldName, inet_addr(szWorldIP), szDBName);

		::StringOutput("%d: %s월드 >> 세팅완료", i, szWorldName);
		::StringOutput(">>>DBName:%s", szDBName);
		::StringOutput(" ");

		AddOBItemTypeChar( m_WorldSetData[i].m_szName, m_WorldSetData[i].m_szDBName );
	}

	AddOBItemTypeChar( "계정DB", m_szAccountDBName );

	//계정데이타초기화
	for(i = 0; i < MAX_USER_PER_GLOBAL; i++)
		m_AccountData[i].InitData(i);

	//기타데이터초기화
	m_tmCheckAccount.BeginTimer(1000);
	m_tmInformWorldInf.BeginTimer(5000);
	m_tmUpdateWorldServerState.BeginTimer( 1000 * 60 * 1 );		//1분

	//계정서버 로그파일
	m_logMain.SetWriteLogFile(".\\NetLog\\AccountSystem_Exception.log", true, true);

	AddOBItemTypeInt( "둥록된 월드수", (int*)&m_dwWorldSetNum );
	AddOBItemTypeInt( "서비스 월드수", (int*)&m_dwServiceSetNum );
	AddOBItemTypeBool( "컨트롤서버 접속", &m_bOperControl );
	AddOBItemTypeBool( "외부접속", &m_bExternalLoginOpen );
	AddOBItemTypeInt( "시리얼카운터", (int*)&m_dwSerialCounter );

	for( i = 0; i < m_dwWorldSetNum; i++)
	{
		AddOBItemTypeInt( m_WorldSetData[i].m_szName, (int*)&m_WorldSetData[i].m_nLoginUserNum );
	}

	return true;
}

bool CMainThread::_NetworkInit()
{
	_NET_TYPE_PARAM typeParam[LINE_NUM];
	
	typeParam[LINE_LOGIN].m_bRealSockCheck = TRUE;
//	typeParam[LINE_LOGIN].m_bRealClientCheck = TRUE;
	typeParam[LINE_LOGIN].m_bSystemLogFile = TRUE;
	typeParam[LINE_LOGIN].m_bServer = TRUE;
	typeParam[LINE_LOGIN].m_byRecvThreadNum = 1;
	typeParam[LINE_LOGIN].m_byRecvSleepTime = 10;
	typeParam[LINE_LOGIN].m_wPort = PORT_ACCOUNT_LOGIN;
	typeParam[LINE_LOGIN].m_wSocketMaxNum = MAX_LOGINER_PER_GLOBAL;
	typeParam[LINE_LOGIN].m_bSvrToS = TRUE;
	typeParam[LINE_LOGIN].m_bSendThread = TRUE;
	typeParam[LINE_LOGIN].m_bOddMsgWriteLog = TRUE;
	sprintf(typeParam[LINE_LOGIN].m_szModuleName, "LoginLine");

	typeParam[LINE_WORLD].m_bRealSockCheck = TRUE;
//	typeParam[LINE_WORLD].m_bRealClientCheck = TRUE;
	typeParam[LINE_WORLD].m_bSystemLogFile = TRUE;
	typeParam[LINE_WORLD].m_bServer = TRUE;
	typeParam[LINE_WORLD].m_byRecvThreadNum = 1;
	typeParam[LINE_WORLD].m_byRecvSleepTime = 10;
	typeParam[LINE_WORLD].m_wPort = PORT_ACCOUNT_WORLD;
	typeParam[LINE_WORLD].m_wSocketMaxNum = MAX_WORLD_PER_GLOBAL;
	typeParam[LINE_WORLD].m_bSvrToS = TRUE;
	typeParam[LINE_WORLD].m_bSendThread = TRUE;
	typeParam[LINE_WORLD].m_bOddMsgWriteLog = TRUE;
	sprintf(typeParam[LINE_WORLD].m_szModuleName, "WorldLine");

	typeParam[LINE_CONTROL].m_bRealSockCheck = TRUE;
//	typeParam[LINE_CONTROL].m_bRealClientCheck = TRUE;
	typeParam[LINE_CONTROL].m_bSystemLogFile = TRUE;
	typeParam[LINE_CONTROL].m_bServer = TRUE;
	typeParam[LINE_CONTROL].m_byRecvThreadNum = 1;
	typeParam[LINE_CONTROL].m_byRecvSleepTime = 1;
	typeParam[LINE_CONTROL].m_wPort = PORT_ACCOUNT_CONTROL;
	typeParam[LINE_CONTROL].m_wSocketMaxNum = max_control_socket;
	typeParam[LINE_CONTROL].m_bSvrToS = TRUE;
	typeParam[LINE_CONTROL].m_bSendThread = TRUE;
	typeParam[LINE_CONTROL].m_bOddMsgWriteLog = TRUE;
	sprintf(typeParam[LINE_CONTROL].m_szModuleName, "ControlLine");

	m_Network.SetNetSystem(LINE_NUM, typeParam, "AccountServer");

	::StringOutput("Network 초기화");

	return true;
}

bool CMainThread::_DatabaseInit()
{
	//일반계정데이터베이스 초기화
	m_pAccountDB = new CRFAccountDatabase;

	if ( !m_pAccountDB->StartDataBase("odinAccount", "odinAccount", "s1qhdks*dbwj" ) )
	{
		::StringOutput("CMainThread : m_pAccountDB->StartDataBase = failed");
		return false;
	}

	return true;
}

//월드파트소켓의 인덱스로부터 월드구조체의 포인터를 얻어옴
_WORLD_DATA* CMainThread::GetWorldPtrFormSocketIndex(DWORD dwSocketIndex)
{
	for(DWORD i = 0; i < m_dwWorldSetNum; i++)
	{
		if(m_WorldSetData[i].m_dwSocketIndex == dwSocketIndex)
			return &m_WorldSetData[i];
	}

	return NULL;	
}

//대기시간안에 로그인서버에서 월드서버로 이동을 완료했는지 체크
void CMainThread::CheckAccount()
{
	if(!m_tmCheckAccount.CountingTimer())	//2초마다..
		return;

	//월드접속인수 카운트 초기화
	for(DWORD i = 0; i < MAX_WORLD_PER_GLOBAL; i++)
		m_WorldSetData[i].m_nLoginUserNum = 0;

	DWORD dwCurTime = timeGetTime();

	int nTotalLoginUserNum = 0;
	for(i = 0; i < MAX_USER_PER_GLOBAL; i++)
	{
		_ACCOUNT_DATA* p = &m_AccountData[i];

		if(!p->m_bLogin)
			continue;

		//월드접속인수 카운팅
		if(p->m_dwWorldCode != 0xFFFFFFFF)
		{
			if(p->m_dwWorldCode < m_dwServiceSetNum)//서비스모드인것만..
				nTotalLoginUserNum++;
			m_WorldSetData[p->m_dwWorldCode].m_nLoginUserNum++;
		}

		//월드로그인수를 초기화..
		if(p->m_bWait)
		{
			int nGap = dwCurTime - p->m_dwWorldSelectTime;	//event timer
			if(nGap > NETMOVE_WAIT_TERM)
			{
				_ReleaseAccount(p, "CheckWorldWaitAccount");
				continue;
			}
			else if(nGap < 0)
				p->m_dwWorldSelectTime = 0;
		}

		if(p->m_idLocalPushWaiter.wIndex != 0xFFFF)
		{
			int nGap = dwCurTime - p->m_dwPushCloseWaitTime;	//event timer
			if(nGap > PUSH_CLOSE_WAIT_TERM)
				_ReleaseAccount(p, "CheckClosePushWait");
			else if(nGap < 0)
				p->m_dwPushCloseWaitTime = 0;
		}
	}

#ifdef __SERVICE

	//서비스모드인 전체 월드의 동접수를 기록한다..
	_USER_NUM_SHEET* pSheet = m_MgrConnNum.Check(nTotalLoginUserNum);
	if(pSheet)
	{
		_log_sheet_usernum Sheet;

		Sheet.in_nAveragePerHour = pSheet->nAveragePerHour;
		Sheet.in_nMaxPerHour = pSheet->nMaxPerHour;

		PushDQSData(0xFF, NULL, _DB_QRY_SYN_DATA::log_case_usernum, (char*)&Sheet, Sheet.size());
	}

#endif
}

//계정이름으로 계정구조체의 포인터를 얻어옴
_ACCOUNT_DATA* CMainThread::GetPtrLoginAccount(DWORD dwAccountSerial, BYTE byUserCode)
{
	for(int i = 0; i < MAX_USER_PER_GLOBAL; i++)
	{
		_ACCOUNT_DATA* pAccount = &m_AccountData[i];

		if(!pAccount->m_bLogin)
			continue;

		if(pAccount->m_byUserCode != byUserCode)
			continue;

		if(pAccount->m_dwAccountSerial == dwAccountSerial)
			return pAccount;
	}

	return NULL;
}

_ACCOUNT_DATA* CMainThread::GetEmptyAccount()
{
	DWORD dwTime = timeGetTime();
	for(int i = 0; i < MAX_USER_PER_GLOBAL; i++)
	{
		_ACCOUNT_DATA* p = &m_AccountData[i];

		if(p->m_bLogin)
			continue;

		int nGap = dwTime - p->m_dwLastDestroyTime;	//event timer
		if(nGap > 60000)	
			return p;
		else if(nGap < 0)
			p->m_dwLastDestroyTime = dwTime;
	}

	return NULL;
}

//login server의 연결이 끊길경우에 접속중이던 유저를 release한다.
void CMainThread::CloseLoginServer(DWORD dwLoginServerIndex)
{
	for(DWORD i = 0; i < MAX_USER_PER_GLOBAL; i++)
	{
		_ACCOUNT_DATA*	pData = &m_AccountData[i];
		if(pData->m_bLogin && pData->m_dwWorldCode == 0xFFFFFFFF)
		{
			_ReleaseAccount(pData, "CloseLoginServer");
		}
	}

	m_LobbyData[dwLoginServerIndex].CloseLobby();

	//write log..
	//
	char szTime[128];	
	_strtime(szTime);
	::StringOutput("%s/ Close Login Server", szTime);
}

//world server의 연결이 끊길경우에 접속중이던 유저를 release한다.
void CMainThread::CloseWorldServer(DWORD dwWorldSocketIndex)
{
	_WORLD_DATA* pw = GetWorldPtrFormSocketIndex(dwWorldSocketIndex);
	if(!pw)	//	pw->OpenWorld();..이것이 호출돼기전에 끈긴경우..
	{
		m_logMain.Write("CMainThread::CloseWorldServer(%d).. GetWorldPtrFormSocketIndex(%d) == NULL", dwWorldSocketIndex, dwWorldSocketIndex);
		return;
	}

	//월드가 닫힘을 로그인서버에 알린다.
	//
	_inform_close_world_aclo Send;

	Send.dwWorldCode = pw->m_dwCode; 

	BYTE byType[msg_header_num] = {system_msg, inform_close_world_aclo};

	for(DWORD i = 0; i < MAX_LOGINER_PER_GLOBAL; i++)
	{
		if(m_LobbyData[i].m_bOpen)
			m_Network.m_pProcess[LINE_LOGIN]->LoadSendMsg(i, byType, (char*)&Send, Send.size());
	}


	//그쪽 월드에 접속해잇던 계정을 초기화한다.
	//
	for(i = 0; i < MAX_USER_PER_GLOBAL; i++)
	{
		_ACCOUNT_DATA*	pData = &m_AccountData[i];
		if(pData->m_bLogin && pData->m_dwWorldCode == pw->m_dwCode)
		{
			_ReleaseAccount(pData, "CloseWorldServer");
		}
	}

	pw->CloseWorld();

	//write log..
	//
	char szTime[128];
	_strtime(szTime);
	::StringOutput("%s/ Disconnect World Server : Code (%d), Name (%s)", szTime, pw->m_dwCode, pw->m_szName);
}

void CMainThread::LoginControlServer()
{
	_open_control_inform_acwr Send;

	Send.bControlOpen = true;

	_socket* pSocket = m_Network.GetSocket(LINE_CONTROL, m_nControlSocketIndex);
	if(!pSocket)
		return;

	Send.dwControlIP = pSocket->m_Addr.sin_addr.S_un.S_addr;
	Send.wControlPort = PORT_WORLD_CONTROL;
	memcpy(Send.dwMasterKey, m_dwControlMasterKey, sizeof(DWORD)*CHECK_KEY_NUM);

	BYTE byType[msg_header_num] = {system_msg, open_control_inform_acwr};
	for(DWORD i = 0; i < m_dwWorldSetNum; i++)
	{
		m_Network.m_pProcess[LINE_WORLD]->LoadSendMsg(i, byType, (char*)&Send, Send.size());
	}
}

void CMainThread::CloseControlServer()
{
	m_bOperControl = false;

	_open_control_inform_acwr Send;

	Send.bControlOpen = false;

	BYTE byType[msg_header_num] = {system_msg, open_control_inform_acwr};
	for(DWORD i = 0; i < m_dwWorldSetNum; i++)
	{
		m_Network.m_pProcess[LINE_WORLD]->LoadSendMsg(i, byType, (char*)&Send, Send.size());
	}

	::StringOutput("Disconnecting ControlServer");
}

bool CMainThread::PushDQSData(BYTE byLocalProcessCode, _CLID* pidLocal, BYTE byQryCase, char* pQryData, int nSize)
{
	DWORD dwEmptyIndex;
	if(!m_listDQSDataEmpty.PopNode_Front(&dwEmptyIndex))
	{
		m_logMain.Write("%d.. m_listDQSDataEmpty.PopNode_Front() => failed ", byQryCase);
		return false;
	}

	_DB_QRY_SYN_DATA* pData = &m_DBQrySynData[dwEmptyIndex];

	pData->m_byLocalProcessCode = byLocalProcessCode;
	if(pidLocal)
		memcpy(&pData->m_idLocal, pidLocal, sizeof(_CLID));
	pData->m_byQryCase = byQryCase;
	if(pQryData)
		memcpy(pData->m_sData, pQryData, nSize);
	
	if(!m_listDQSData.PushNode_Back(dwEmptyIndex))
	{
		m_logMain.Write("%d : m_listDQSData.PushNode_Back() => failed ", byQryCase);
		return false;
	}

	pData->m_bUse = true;
	pData->m_bLoad = false;

	return true;
}

void CMainThread::DQSThread(void* pv)
{
	CMainThread* pDoc = (CMainThread*)pv;
	DWORD dwDataIndex;

	while(pDoc->m_bDQSThread)
	{
		pDoc->m_DBFrameRate.CalcSpeedPerFrame();

		while(pDoc->m_listDQSData.PopNode_Front(&dwDataIndex))
		{
			_DB_QRY_SYN_DATA* p = &pDoc->m_DBQrySynData[dwDataIndex];
			switch(p->m_byQryCase)
			{
			case _DB_QRY_SYN_DATA::qry_case_join:{				
				p->m_byResult = pDoc->db_join_dev((_qry_sheet_join*)p->m_sData);
			  }break;

			case _DB_QRY_SYN_DATA::qry_case_login:{
				p->m_byResult = pDoc->db_login_account((_qry_sheet_login*)p->m_sData);
			  }break;

			case _DB_QRY_SYN_DATA::log_case_usernum:{
				p->m_byResult = pDoc->db_log_usernum((_log_sheet_usernum*)p->m_sData);
			  }break;

			case _DB_QRY_SYN_DATA::log_case_push:{
				p->m_byResult = pDoc->db_log_pushclose((_log_sheet_push*)p->m_sData);
			  }break;

			case _DB_QRY_SYN_DATA::log_case_logtime:{
				p->m_byResult = pDoc->db_log_logtime((_log_sheet_logtime*)p->m_sData);
			  }break;
			
			case _DB_QRY_SYN_DATA::upt_case_worldserver:{
				p->m_byResult = pDoc->db_upt_worldServer((_upt_case_worldserver*)p->m_sData);
			  }break;
		
			}

			if(p->m_byResult == RET_CODE_DB_QUERY_ERR)
			{
				pDoc->m_logMain.Write("DQSThread.. DB fail.. case: %d", p->m_byQryCase);
			}
			
			p->m_bLoad = true;
			pDoc->m_listDQSDataComplete.PushNode_Back(dwDataIndex);
			Sleep(0);
		}
		Sleep(1);
	}

	::_endthreadex(0);
}

void CMainThread::DQSCompleteProcess()
{
	DWORD dwDataIndex;
	while(m_listDQSDataComplete.PopNode_Front(&dwDataIndex))
	{
		_DB_QRY_SYN_DATA* p = &m_DBQrySynData[dwDataIndex];
		switch(p->m_byQryCase)
		{
		case _DB_QRY_SYN_DATA::qry_case_join:{
			Join_Account_Complete(p->m_byLocalProcessCode, &p->m_idLocal, p->m_byResult);
		  }break;

		case _DB_QRY_SYN_DATA::qry_case_login:{
			Login_Account_Complete(p->m_byLocalProcessCode, &p->m_idLocal, p->m_byResult, (_qry_sheet_login*)p->m_sData);
		  }break;
		}

		p->m_bUse = false;
		p->m_bLoad = false;
		m_listDQSDataEmpty.PushNode_Back(dwDataIndex);
	}
}

void CMainThread::pc_JoinAccount(BYTE byLoginServerIndex, _CLID* pidLocal, char* pszAccountID, char* pszPassword, DWORD dwClientIP)
{//개발자계정을 따로 만든다..	
	//유효한문자의 조합인지 확인
	if(!g_IsSuitAccountID(&pszAccountID[3]))
	{
		Join_Account_Complete(byLoginServerIndex, pidLocal, RET_CODE_ACCOUNT_NOT_SUIT);
		return;
	}

	_qry_sheet_join Sheet;

	strcpy(Sheet.in_szAccountID, pszAccountID);
	strcpy(Sheet.in_szPassword, pszPassword);
	Sheet.in_dwClientIP = dwClientIP;

	if(!PushDQSData(byLoginServerIndex, pidLocal, _DB_QRY_SYN_DATA::qry_case_join, (char*)&Sheet, Sheet.size()))
	{
		Join_Account_Complete(byLoginServerIndex, pidLocal, RET_CODE_INTERNAL_ERROR);
		return;
	}
}

void CMainThread::Join_Account_Complete(BYTE byLoginServerIndex, _CLID* pidLocal, BYTE byRetCode)
{
	//결과를 전송한다.
	_join_account_result_aclo Send;

	Send.byRetCode = byRetCode;
	memcpy(&Send.idLocal, pidLocal, sizeof(_CLID));

	BYTE byType[msg_header_num] = {system_msg, join_account_result_aclo};
	m_Network.m_pProcess[LINE_LOGIN]->LoadSendMsg(byLoginServerIndex, byType, (char*)&Send, Send.size());	
}

void CMainThread::pc_LoginAccount(BYTE byLoginServerIndex, BYTE byUserCode, _CLID* pidLocal, char* pszAccountID, char* pszPassword, DWORD dwClientIP)
{
	_qry_sheet_login Sheet;

	strcpy(Sheet.in_szAccountID, pszAccountID);
	Sheet.in_dwClientIP = dwClientIP;
	Sheet.in_byUserCode = byUserCode;
	strcpy(Sheet.in_szPassword, pszPassword);

	if(!PushDQSData(byLoginServerIndex, pidLocal, _DB_QRY_SYN_DATA::qry_case_login, (char*)&Sheet, Sheet.size()))
	{
		Login_Account_Complete(byLoginServerIndex, pidLocal, RET_CODE_INTERNAL_ERROR, NULL);
		return;
	}
}

void CMainThread::Login_Account_Complete(BYTE byLoginServerIndex, _CLID* pidLocal, BYTE byRetCode, _qry_sheet_login* pData)
{
	_ACCOUNT_DATA* pLoginData = NULL;
	_ACCOUNT_DATA* pBeforeData = NULL;

	if ( byRetCode != RET_CODE_SUCCESS && byRetCode != RET_CODE_BLOCK_CHAT )
		goto RESULT;

	pLoginData = GetEmptyAccount();

	if(!pLoginData)
	{
		byRetCode = RET_CODE_FULL_LOGIN;
		goto RESULT;
	}

	if(pData->out_dwAccountSerial == 0)
	{
		m_logMain.Write("Login_Account_Complete.. account:%s.. serial == %d", pData->in_szAccountID, pData->out_dwAccountSerial);
		byRetCode = RET_CODE_INTERNAL_ERROR;
		goto RESULT;
	}

	//이미 접속해있는지 검사..
	if((pBeforeData = GetPtrLoginAccount(pData->out_dwAccountSerial, pData->in_byUserCode)) != NULL)
	{		
		//같은 ip인지 확인
		if(pData->in_dwClientIP == pBeforeData->m_dwClientIP)
		{
			//같은 ip면 바로 죽인다..
			ForceCloseAccount(pBeforeData, true, false, 0);
			_ReleaseAccount(pBeforeData, "pc_LoginAccount");
		}
		else
		{
			byRetCode = RET_CODE_ALREADY_LOGIN;
			goto RESULT;
		}
	}

RESULT:

	if(byRetCode == RET_CODE_SUCCESS || byRetCode == RET_CODE_BLOCK_CHAT)
	{
		pLoginData->SetData(pData->in_byUserCode, pData->in_szAccountID, pData->out_dwAccountSerial, pData->out_byUserDgr, pData->out_bySubDgr, pData->in_dwClientIP, byLoginServerIndex, pidLocal, m_dwSerialCounter++, pData->out_szLogDate);		

		if ( byRetCode == RET_CODE_BLOCK_CHAT )
			pLoginData->SetChatLock( true );

		_login_account_result_aclo Send;
		
//		Send.byRetCode = byRetCode;
		Send.byRetCode = RET_CODE_SUCCESS;
		memcpy(&Send.idLocal, pidLocal, sizeof(_CLID));
		Send.dwAccountSerial = pData->out_dwAccountSerial;
		Send.byUserGrade = pData->out_byUserDgr;
		memcpy(&Send.gidNewGlobal, &pLoginData->m_gidGlobal, sizeof(_GLBID));
		
		BYTE byType[msg_header_num] = {system_msg, login_account_result_aclo};
		m_Network.m_pProcess[LINE_LOGIN]->LoadSendMsg(byLoginServerIndex, byType, (char*)&Send, Send.size());
	}
	else
	{
		_login_account_result_aclo Send;
		
		Send.byRetCode = byRetCode;
		memcpy(&Send.idLocal, pidLocal, sizeof(_CLID));
		if(byRetCode = RET_CODE_ALREADY_LOGIN)
			Send.dwAccountSerial = pData->out_dwAccountSerial;
		
		BYTE byType[msg_header_num] = {system_msg, login_account_result_aclo};
		m_Network.m_pProcess[LINE_LOGIN]->LoadSendMsg(byLoginServerIndex, byType, (char*)&Send, Send.size());
	}
}

void CMainThread::pc_PushClose(BYTE byLoginServerIndex, BYTE byUserCode, _CLID* pidLocal, DWORD dwAccountSerial, DWORD dwPusherIP)// char* pszAccountID, char* pszPassword, DWORD dwPusherIP)
{
	BYTE byRetCode = RET_CODE_SUCCESS;
	_ACCOUNT_DATA* pBeforeData = NULL;

	//접속중인지 확인
	//
	if((pBeforeData = GetPtrLoginAccount(dwAccountSerial, byUserCode)) == NULL)
	{
		byRetCode = RET_CODE_BEFORE_NOT_EXIST;
		goto RESULT;
	}

	//Close 대기중인지
	//
	if(pBeforeData->m_idLocalPushWaiter.wIndex != 0xFFFF)
	{
		byRetCode = RET_CODE_NET_PROCESS_ERROR;
		goto RESULT;
	}

RESULT:
	
	if(byRetCode == RET_CODE_SUCCESS)
	{
		//Push Waiter 기록
		//
		pBeforeData->SetPushWaiter(byLoginServerIndex, pidLocal, dwPusherIP);

		if(!pBeforeData->m_bWait)	//대기모드가 아니라면 접속중인 서버에 명령한다.
			ForceCloseAccount(pBeforeData, false, true, dwPusherIP);//Push Closer가 로그인서버에 있을때 강제종료로 명령한다
	}
	else	
	{
		_push_close_result_aclo Send;

		Send.byRetCode = byRetCode;
		memcpy(&Send.idLocal, pidLocal, sizeof(_CLID));

		BYTE byType[msg_header_num] = {system_msg, push_close_result_aclo};
		m_Network.m_pProcess[LINE_LOGIN]->LoadSendMsg(byLoginServerIndex, byType, (char*)&Send, Send.size());
	}
}

void CMainThread::pc_CloseAccount(_GLBID* pgidGlobal)
{
	BYTE byRetCode = RET_CODE_SUCCESS;
	_ACCOUNT_DATA* pAccountData = &m_AccountData[pgidGlobal->dwIndex];

	//로그인중인지 검사
	//
	if(!pAccountData->m_bLogin)
	{
//		m_logMain.Write("CMainThread::pc_CloseAccount() : 로그인중아님");
		byRetCode = RET_CODE_BEFORE_LOGIN;
		goto RESULT;
	}

	//시리얼검사..
	//
	if(pAccountData->m_gidGlobal.dwSerial != pgidGlobal->dwSerial)
	{
//		m_logMain.Write("CMainThread::pc_CloseAccount() : G_id 시리얼틀림");
		byRetCode = RET_CODE_FAULT_SERIAL;
		goto RESULT;
	}

RESULT:

	if(byRetCode == RET_CODE_SUCCESS)
	{
		_ReleaseAccount(pAccountData, "pc_CloseAccount");
	}
}

void CMainThread::pc_WorldListRequest(BYTE byLoginServerIndex)
{
	m_LobbyData[byLoginServerIndex].OpenLobby();

	//login server가 부팅시 등록된 월드리스트를 요구함
	//
	_world_list_result_aclo Send;

	Send.byServiceWorldNum = (BYTE)m_dwServiceSetNum;
	Send.byWorldNum = (BYTE)m_dwWorldSetNum;

	for(DWORD i = 0; i < m_dwWorldSetNum; i++)
	{
		_WORLD_DATA* pSet = &m_WorldSetData[i];

		Send.WorldList[i].bOpen = pSet->m_bService;
		strcpy(Send.WorldList[i].szWorldName, pSet->m_szName);
		Send.WorldList[i].dwGateIP = pSet->m_dwGateIP;
		Send.WorldList[i].wGatePort = pSet->m_wGatePort;
	}

	BYTE byType[msg_header_num] = {system_msg, world_list_result_aclo};
	m_Network.m_pProcess[LINE_LOGIN]->LoadSendMsg(byLoginServerIndex, byType, (char*)&Send, Send.size());

	//write log..
	//
	char szTime[128];
	_strtime(szTime);
	::StringOutput("%s/ Login Server Login", szTime);	

	//외부접속불가로 세팅
	m_bExternalLoginOpen = false;
}

void CMainThread::pc_SelectWorld(BYTE byLoginServerIndex, _GLBID* pgidGlobal, WORD wWorldCode, DWORD dwClientIP)
{
	BYTE byRetCode = RET_CODE_SUCCESS;
	_WORLD_DATA* pWorldSet = &m_WorldSetData[wWorldCode];
	_ACCOUNT_DATA* pUser = &m_AccountData[pgidGlobal->dwIndex];

	if(!pUser->m_bLogin)
	{
		byRetCode = RET_CODE_ALREADY_EXIT;
		goto RESULT;
	}

	//시리얼검사..
	//
	if(pUser->m_gidGlobal.dwSerial != pgidGlobal->dwSerial)
	{
		m_logMain.Write("CMainThread::pc_SelectWorld() : G_id 시리얼틀림");
		byRetCode = RET_CODE_FAULT_SERIAL;
		goto RESULT;
	}

	//열린월드인지..
	//
	if(!pWorldSet->m_bService)
	{
		m_logMain.Write("CMainThread::pc_SelectWorld() : 서비스중인 월드가아님[%d]", pWorldSet->m_dwCode);
		byRetCode = RET_CODE_NOT_OPEN_WORLD;
		goto RESULT;
	}

	//인원수체크
	if(pWorldSet->m_nLoginUserNum >= MAX_PLAYER)
	{
		byRetCode = RET_CODE_USER_FULL;
		goto RESULT;
	}

RESULT:

	if(byRetCode == RET_CODE_SUCCESS)
	{
		//set select world 
		//
		pUser->GenerateMasterKey();

		//inform account data to selected world first
		//
		_trans_account_inform_acwr Inform;

		Inform.dwAccountSerial = pUser->m_dwAccountSerial;
		strcpy(Inform.szAccountID, pUser->m_szAccountID);
		memcpy(Inform.dwKey, ::CalcCodeKey(pUser->m_dwMasterKey), sizeof(DWORD)*CHECK_KEY_NUM);//월드에는 암호를 알려주고..
		memcpy(&Inform.gidGlobal, &pUser->m_gidGlobal, sizeof(_GLBID));
		Inform.dwClientIP = dwClientIP;
		Inform.byUserDgr = pUser->m_byUserDgr;
		Inform.bySubDgr = pUser->m_bySubDgr;
		Inform.bChatLock = pUser->m_bChatLock;

		BYTE byType[msg_header_num] = {system_msg, trans_account_inform_acwr};
		m_Network.m_pProcess[LINE_WORLD]->LoadSendMsg(pWorldSet->m_dwSocketIndex, byType, (char*)&Inform, Inform.size());
	}
	else
	{	//실패시는 바로 결과를 보낸다..
		_select_world_result_aclo Send;

		Send.byRetCode = byRetCode;
		memcpy(&Send.idLocal, &pUser->m_idLocal, sizeof(_CLID));

		BYTE byType[msg_header_num] = {system_msg, select_world_result_aclo};
		m_Network.m_pProcess[LINE_LOGIN]->LoadSendMsg(byLoginServerIndex, byType, (char*)&Send, Send.size());
	}
}

void CMainThread::pc_OpenWorld(DWORD dwSocketIndex, char* pszWorldName, DWORD dwWorldServerIP, BYTE byRunMode)
{
	_WORLD_DATA* pWorldSet = NULL;
	bool bRet = true;
	char szErrCode[64] = {0,};

	char szWorldIP[16] = {0,};
	IN_ADDR inAddr;
	inAddr.S_un.S_addr = dwWorldServerIP;
	strcpy( szWorldIP, inet_ntoa( inAddr ) );
	m_logMain.Write( "WorldServer Start IP = %s", szWorldIP );

	//등록된 월드주소인지 확인..
	for(DWORD i = 0; i < m_dwWorldSetNum; i++)
	{
		_WORLD_DATA* pSet = &m_WorldSetData[i];
		if(pSet->m_bOpen)
			continue;

		_socket* pSocket = m_Network.GetSocket(LINE_WORLD, dwSocketIndex);
		if(!pSocket)
			break;

		if(!strcmp(pSet->m_szName, pszWorldName))// && pSet->m_dwWorldIP == dwWorldServerIP)
		{
			pWorldSet = pSet;
			break;
		}

	}
	if(pWorldSet)
	{
		pWorldSet->OpenWorld(dwSocketIndex, byRunMode);

		::StringOutput("%s World Open ", pWorldSet->m_szName);	
	}
	else
	{
		bRet = false;
		strcpy(szErrCode, "등록됀 월드가아님");

		m_logMain.Write("CMainThread::pc_OpenWorld() : 등록됀 월드가아님");
		m_Network.Close(LINE_WORLD, dwSocketIndex, false);
	}

	//to world
	if(bRet)
	{
		_open_world_success_acwr Ret;

		Ret.byWorldCode = (BYTE)pWorldSet->m_dwCode;
		strcpy(Ret.szDBName, pWorldSet->m_szDBName);
	//	Ret.dwDBIP = inet_addr(pWorldSet->m_szDBMachine);
	//	strcpy(Ret.szDBAccount, "sa");
	//	strcpy(Ret.szDBPassword, "tkfkd2du");

		Ret.bControlOpen = m_bOperControl;
		if(m_bOperControl)
		{
			_socket* pSocket = m_Network.GetSocket(LINE_CONTROL, m_nControlSocketIndex);
			if(pSocket)
			{
				Ret.dwControlIP = pSocket->m_Addr.sin_addr.S_un.S_addr;
				Ret.wControlPort = PORT_WORLD_CONTROL;
				memcpy(Ret.dwControlMasterKey, m_dwControlMasterKey, sizeof(DWORD)*CHECK_KEY_NUM);
			}
		}
		BYTE byType[msg_header_num] = {system_msg, open_world_success_acwr};
		m_Network.m_pProcess[LINE_WORLD]->LoadSendMsg(dwSocketIndex, byType, (char*)&Ret, Ret.size());
	}
	else
	{
		_open_world_failure_acwr Ret;

		strcpy(Ret.szErrCode, szErrCode);

		BYTE byType[msg_header_num] = {system_msg, open_world_failure_acwr};
		m_Network.m_pProcess[LINE_WORLD]->LoadSendMsg(dwSocketIndex, byType, (char*)&Ret, Ret.size());
	}
}

void CMainThread::pc_StartWorld(DWORD dwSocketIndex, DWORD dwGateIP, WORD wGatePort)
{
	_WORLD_DATA* pWorldSet = GetWorldPtrFormSocketIndex(dwSocketIndex);
	if(!pWorldSet)
	{
		m_logMain.Write("CMainThread::pc_StartWorld() : GetWorldPtrFormSocketIndex(%d) == NULL", dwSocketIndex);
		return;
	}

	if(!pWorldSet->m_bOpen)
	{
		m_logMain.Write("CMainThread::pc_StartWorld() : 아직 열리지 않았음");
		return;
	}

	if(pWorldSet->m_bService)
	{
		m_logMain.Write("CMainThread::pc_StartWorld(%s) : 이미 서비스 중임", pWorldSet->m_szName);
		return;
	}

	pWorldSet->ServiceStart(dwGateIP, wGatePort);

	//to login..
	_inform_open_world_aclo Send;

	Send.dwWorldCode = pWorldSet->m_dwCode;
	Send.dwGateIP = pWorldSet->m_dwGateIP;
	Send.wGatePort = pWorldSet->m_wGatePort;

	BYTE byType[msg_header_num] = {system_msg, inform_open_world_aclo};
	for(int i = 0; i < MAX_LOGINER_PER_GLOBAL; i++)
	{
		if(m_LobbyData[i].m_bOpen)
			m_Network.m_pProcess[LINE_LOGIN]->LoadSendMsg(i, byType, (char*)&Send, Send.size());
	}

	//write log..
	char szTime[128];	
	_strtime(szTime);
	::StringOutput("%s: Start World: Code (%d), Name (%s)", szTime, pWorldSet->m_dwCode, pWorldSet->m_szName);	
}

void CMainThread::pc_StopWorld(DWORD dwSocketIndex)
{
	//소켓은 유지됀상태에서 닫히기만함..
	_WORLD_DATA* pWorldSet = GetWorldPtrFormSocketIndex(dwSocketIndex);
	if(!pWorldSet)
	{
		m_logMain.Write("CMainThread::pc_CloseWorld() : GetWorldPtrFormSocketIndex(%d) == NULL", dwSocketIndex);
		return;
	}

	if(!pWorldSet->m_bOpen)
	{
		m_logMain.Write("CMainThread::pc_StartWorld() : 아직 열리지 않았음");		
		return;
	}

	if(!pWorldSet->m_bService)
	{
		m_logMain.Write("CMainThread::pc_StartWorld(%s) : 이미 서비스중지상태임", pWorldSet->m_szName);		
		return;
	}


	//서비스를 중지한다.
	pWorldSet->ServiceStop();

	//월드가 닫힘을 로그인서버에 알린다.
	_inform_close_world_aclo Send;

	Send.dwWorldCode = pWorldSet->m_dwCode; 

	BYTE byType[msg_header_num] = {system_msg, inform_close_world_aclo};
	for(int i = 0; i < MAX_LOGINER_PER_GLOBAL; i++)
	{
		if(m_LobbyData[i].m_bOpen)
			m_Network.m_pProcess[LINE_LOGIN]->LoadSendMsg(i, byType, (char*)&Send, Send.size());
	}

	//write log..
	char szTime[128];	
	_strtime(szTime);
	::StringOutput("%s: Stop World: Code (%d), Name (%s)", szTime, pWorldSet->m_dwCode, pWorldSet->m_szName);	
}

void CMainThread::pc_EnterWorldRequest(DWORD dwSocketIndex, _GLBID* pgidGlobal, _CLID* pidLocal)
{
	BYTE byRetCode = RET_CODE_SUCCESS;
	_WORLD_DATA* pWorldSet = GetWorldPtrFormSocketIndex(dwSocketIndex);
	_ACCOUNT_DATA* pUser = &m_AccountData[pgidGlobal->dwIndex];

	//시리얼검사
	//
	if(pUser->m_gidGlobal.dwSerial != pgidGlobal->dwSerial)
	{
		m_logMain.Write("CMainThread::pc_EnterWorldRequest() : G_id 시리얼틀림");
		byRetCode = RET_CODE_FAULT_SERIAL;
		goto RESULT;
	}

	//월드체크
	//
	if(!pWorldSet)
	{
		m_logMain.Write("CMainThread::pc_EnterWorldRequest() : GetWorldPtrFormSocketIndex(%d) == false", dwSocketIndex);
		byRetCode = RET_CODE_INTERNAL_ERROR;
		goto RESULT;
	}
	
	//계정로그인중인지 체크
	//
	if(!pUser->m_bLogin)
	{
		byRetCode = RET_CODE_BEFORE_LOGIN;
		goto RESULT;
	}

	//대기리스트에 있는지 확인
	//
	if(!pUser->m_bWait)
	{
		byRetCode = RET_CODE_NOT_WAIT_MODE;
		goto RESULT;
	}

	if(pUser->m_idLocalPushWaiter.wIndex != 0xFFFF)
	{
		byRetCode = RET_CODE_NOT_WAIT_MODE;
		goto RESULT;
	}

RESULT:

	if(byRetCode == RET_CODE_SUCCESS)
	{
		//!!enter world!!
		//
		pUser->EnterWorld(pWorldSet->m_dwCode, pidLocal);
	}
	else
	{
		//실패시는 월드에 반송해서 게임 종료케함
		//
		_enter_world_result_acwr Send;

		Send.byRetCode = byRetCode;
		memcpy(&Send.idLocal, pidLocal, sizeof(_CLID));

		BYTE byType[msg_header_num] = {system_msg, enter_world_result_acwr};
		m_Network.m_pProcess[LINE_WORLD]->LoadSendMsg(dwSocketIndex, byType, (char*)&Send, Send.size());
	}	
}

void CMainThread::pc_TransAccountReport(DWORD dwWorldSocketIndex, _GLBID* pgidGlobal, BYTE byRetCode)
{
	_ACCOUNT_DATA* pUser = &m_AccountData[pgidGlobal->dwIndex];

	if(!pUser->m_bLogin || pUser->m_byLoginServerIndex == 0xFF)
	{
		m_logMain.Write("CMainThread::pc_TransAccountReport() : 그사이에 로그오프");
		return;
	}

	//시리얼검사..
	//
	if(pUser->m_gidGlobal.dwSerial != pgidGlobal->dwSerial)
	{
		m_logMain.Write("CMainThread::pc_TransAccountReport() : G_id 시리얼틀림");
		return;
	}

	if(byRetCode == RET_CODE_SUCCESS)
	{
		_WORLD_DATA* pwd = GetWorldPtrFormSocketIndex(dwWorldSocketIndex);

		if(!pwd)
		{
			m_logMain.Write("CMainThread::pc_TransAccountReport().. GetWorldPtrFormSocketIndex(%d) == NULL", dwWorldSocketIndex);
			return;
		}

		pUser->SelectWorld(pwd->m_dwCode);
	}

	//send result to client
	//
	_select_world_result_aclo Send;

	Send.byRetCode = byRetCode;	
	memcpy(Send.dwWorldMasterKey, pUser->m_dwMasterKey, sizeof(DWORD)*CHECK_KEY_NUM);//클라이언트는 쌩을 알려줌..
	memcpy(&Send.idLocal, &pUser->m_idLocal, sizeof(_CLID));

	BYTE byType[msg_header_num] = {system_msg, select_world_result_aclo};
	m_Network.m_pProcess[LINE_LOGIN]->LoadSendMsg(pUser->m_byLoginServerIndex, byType, (char*)&Send, Send.size());
}

void CMainThread::pc_WorldServiceReport(DWORD dwWorldSocketIndex, bool bBefore, bool bAfter)
{
	_WORLD_DATA* pwd = GetWorldPtrFormSocketIndex(dwWorldSocketIndex);
	if(!pwd)
	{
		m_logMain.Write("CMainThread::pc_WorldOperReport().. GetWorldPtrFormSocketIndex(%d) == NULL", dwWorldSocketIndex);		
		return;
	}

	::StringOutput("%d..%s: WorldOperReport : before : %d -> after: %d", pwd->m_dwCode, pwd->m_szName, bBefore, bAfter);	
}

void CMainThread::pc_PingWorld(DWORD dwWorldSocketIndex)
{
	_WORLD_DATA* pwd = GetWorldPtrFormSocketIndex(dwWorldSocketIndex);
	if(!pwd)
		return;

	pwd->m_dwLastPingTime = timeGetTime();
}

void CMainThread::pc_LoginControlRequest(DWORD* pdwMasterKey)
{
	bool bSucc = true;

	if(m_bOperControl)	//이미 로그인중이면..
	{
		bSucc = false;
	}
	else
	{
		m_bOperControl = true;
		memcpy(m_dwControlMasterKey, pdwMasterKey, sizeof(DWORD)*CHECK_KEY_NUM);
	}
	
	_login_control_result_acco Send;

	Send.bRet = bSucc;
	Send.wRegedWorldNum = 0;

	if(bSucc)
	{
		strcpy(Send.szAccountDBName, m_szAccountDBName);
		strcpy(Send.szAccountDBMachine, m_szAccountDBMachine);
		for(DWORD i = 0; i < m_dwServiceSetNum; i++)
		{
			_WORLD_DATA* pwd = &m_WorldSetData[i];

//			if(pwd->m_bServiceMode)
			{
				strcpy(Send.WorldList[i].szWorldName, pwd->m_szName);
				strcpy(Send.WorldList[i].szDBName, pwd->m_szDBName);
//				strcpy(Send.WorldList[i].szDBMachine, pwd->m_szDBMachine);
			}
		}
		Send.wRegedWorldNum = (WORD)m_dwServiceSetNum;
	}

	BYTE byType[msg_header_num] = {control_msg, login_control_result_acco};
	m_Network.m_pProcess[LINE_CONTROL]->LoadSendMsg(m_nControlSocketIndex, byType, (char*)&Send, Send.size());

	if(bSucc)
	{
		LoginControlServer();
	}

	if(bSucc)
		::StringOutput("Control Server 접속 성공");
	else
		::StringOutput("Control Server 접속 실패");
}

void CMainThread::pc_WorldServiceRequest(DWORD dwWorldCode, bool bService, _CLID* pidLocal)
{
	_WORLD_DATA* pwd = &m_WorldSetData[dwWorldCode];

	if(!pwd->m_bOpen)
	{
		SendErrMsgToControl(pidLocal, "%s월드는 현재 작동가능한 월드가 아님", pwd->m_szName);		
		return;
	}

	if(pwd->m_bService != bService)
	{
		if(!pwd->m_bService)
		{
			pwd->ServiceReStart();

			//to login..
			_inform_open_world_aclo Send;

			Send.dwWorldCode = pwd->m_dwCode;
			Send.dwGateIP = pwd->m_dwGateIP;
			Send.wGatePort = pwd->m_wGatePort;

			BYTE byType[msg_header_num] = {system_msg, inform_open_world_aclo};
			for(int i = 0; i < MAX_LOGINER_PER_GLOBAL; i++)
			{
				if(m_LobbyData[i].m_bOpen)
					m_Network.m_pProcess[LINE_LOGIN]->LoadSendMsg(i, byType, (char*)&Send, Send.size());
			}

			//write log..
			char szTime[128];	
			_strtime(szTime);
			::StringOutput("%s: ReStart World: Code (%d), Name (%s)", szTime, pwd->m_dwCode, pwd->m_szName);			
		}
		else
		{
			//서비스를 중지한다.
			pwd->ServiceStop();

			//월드가 닫힘을 로그인서버에 알린다.
			_inform_close_world_aclo Send;

			Send.dwWorldCode = pwd->m_dwCode; 

			BYTE byType[msg_header_num] = {system_msg, inform_close_world_aclo};
			for(int i = 0; i < MAX_LOGINER_PER_GLOBAL; i++)
			{
				if(m_LobbyData[i].m_bOpen)
					m_Network.m_pProcess[LINE_LOGIN]->LoadSendMsg(i, byType, (char*)&Send, Send.size());
			}

			//write log..
			char szTime[128];	
			_strtime(szTime);
			::StringOutput("%s: Stop World: Code (%d), Name (%s)", szTime, pwd->m_dwCode, pwd->m_szName);
		}

		_world_service_inform_acwr Send;

		Send.bService = bService;

		BYTE byType[msg_header_num] = {control_msg, world_service_inform_acwr};
		m_Network.m_pProcess[LINE_WORLD]->LoadSendMsg(pwd->m_dwSocketIndex, byType, (char*)&Send, Send.size());
	}
	else
	{
		if(pwd->m_bService)
			SendErrMsgToControl(pidLocal, "%s월드는 이미 서비스중인 상태임", pwd->m_szName);
		else
			SendErrMsgToControl(pidLocal, "%s월드는 이미 서비스중이 아닌 상태임", pwd->m_szName);		
		return;
	}
}

void CMainThread::pc_WorldUserExitRequest(DWORD dwWorldCode, _CLID* pidLocal)
{
	_WORLD_DATA* pwd = &m_WorldSetData[dwWorldCode];

	if(!pwd->m_bOpen)
	{
		SendErrMsgToControl(pidLocal, "%s월드는 현재 작동가능한 월드가 아님", pwd->m_szName);		
		return;
	}

	if(pwd->m_bService)	//서비스중에는 하지못한다.
	{
		SendErrMsgToControl(pidLocal, "%s월드는 아직 서비스중인 상태임", pwd->m_szName);		
		return;
	}

	_world_exit_inform_acwr Send;

	BYTE byType[msg_header_num] = {control_msg, world_exit_inform_acwr};
	m_Network.m_pProcess[LINE_WORLD]->LoadSendMsg(pwd->m_dwSocketIndex, byType, (char*)&Send, Send.size());	
}

void CMainThread::pc_WorldMsgRequest(DWORD dwWorldCode, BYTE byMsgSize, char* pszMsg, _CLID* pidLocal)
{
	_WORLD_DATA* pwd = &m_WorldSetData[dwWorldCode];

	if(pwd->m_bOpen)
	{
		_world_msg_inform_acwr Send;

		Send.byMsgSize = byMsgSize;
		memcpy(Send.szMsg, pszMsg, byMsgSize);

		BYTE byType[msg_header_num] = {control_msg, world_msg_inform_acwr};
		m_Network.m_pProcess[LINE_WORLD]->LoadSendMsg(pwd->m_dwSocketIndex, byType, (char*)&Send, Send.size());
	}
	else
	{
		SendErrMsgToControl(pidLocal, "%s월드는 현재 작동가능한 월드가 아님", pwd->m_szName);		
		return;
	}	
}

void CMainThread::pc_UserPositionResult( WORD wClientIndex, char* szUserAccount, char* szWorldName, char* szAvatorName )
{
	_user_position_result_acco Send;
	Send.wClientIndex = wClientIndex;
	strcpy( Send.szAccountID, szUserAccount );
	strcpy( Send.szWorldName, szWorldName );
	strcpy( Send.szAvatorName, szAvatorName );

	BYTE byType[msg_header_num] = {control_msg, user_position_result_acco};
	m_Network.m_pProcess[LINE_CONTROL]->LoadSendMsg(m_nControlSocketIndex, byType, (char*)&Send, Send.size());
}

void CMainThread::pc_UserSelectAvatorReport( _GLBID gidGlobal, char* szCharName, DWORD dwAvatorSerial )
{
	for(int i = 0; i < MAX_USER_PER_GLOBAL; i++)
	{
		if ( m_AccountData[i].m_bLogin )
		{
			if ( m_AccountData[i].m_gidGlobal.dwIndex == gidGlobal.dwIndex &&
				m_AccountData[i].m_gidGlobal.dwSerial == gidGlobal.dwSerial )
			break;
		}
	}

	if ( i >= MAX_USER_PER_GLOBAL )
		return;

	memcpy( m_AccountData[i].m_szAvatorName, szCharName, sizeof(m_AccountData[i].m_szAvatorName) );
	m_AccountData[i].m_dwAvatorSerial = dwAvatorSerial;
}

void CMainThread::pc_UserEnterLobby( _GLBID gidGlobal )
{
	for(int i = 0; i < MAX_USER_PER_GLOBAL; i++)
	{
		if ( m_AccountData[i].m_bLogin )
		{
			if ( m_AccountData[i].m_gidGlobal.dwIndex == gidGlobal.dwIndex &&
				m_AccountData[i].m_gidGlobal.dwSerial == gidGlobal.dwSerial )
			break;
		}
	}

	if ( i >= MAX_USER_PER_GLOBAL )
		return;

	memset( m_AccountData[i].m_szAvatorName, 0, sizeof(m_AccountData[i].m_szAvatorName) );
	m_AccountData[i].m_dwAvatorSerial = 0xFFFF;
}

void CMainThread::pc_UserBlockRequest( char* szUserAccount, BYTE byBlockKind, WORD wBlockHour )
{
	for(int i = 0; i < MAX_USER_PER_GLOBAL; i++)
	{
		if ( m_AccountData[i].m_bLogin )
		{
			if ( strcmp(m_AccountData[i].m_szAccountID, szUserAccount) == 0 )
			{
				if ( m_AccountData[i].m_dwWorldCode == 0xFFFFFFFF )
				{
//					strcpy( szWorldName, "로그인서버");
				}
				else if ( m_AccountData[i].m_dwWorldCode < m_dwWorldSetNum )
				{
					BYTE byWorldSocketIndex = (BYTE)m_WorldSetData[m_AccountData[i].m_dwWorldCode].m_dwSocketIndex;	
					if(byWorldSocketIndex == 0xFF)
						return;

					if ( byBlockKind == 0 )				// 계정블럭
					{
						_force_close_command_acwr Send;
						Send.idLocal.wIndex = m_AccountData[i].m_idLocal.wIndex;
						Send.idLocal.dwSerial = m_AccountData[i].m_idLocal.dwSerial;
						Send.bDirectly = false;
						Send.byKickType = FORCE_CLOSE_TYPE_KICK;
						Send.dwPushIP = 0xFFFF;
						BYTE byType[msg_header_num] = {system_msg, force_close_command_acwr};
						m_Network.m_pProcess[LINE_WORLD]->LoadSendMsg(byWorldSocketIndex, byType, (char*)&Send, Send.size());
					}
					else if ( byBlockKind == 1 )		// 채팅금지
					{
						_chat_lock_command_acwr Send;
						Send.idLocal.wIndex = m_AccountData[i].m_idLocal.wIndex;
						Send.idLocal.dwSerial = m_AccountData[i].m_idLocal.dwSerial;
						Send.wBlockTimeH = wBlockHour;
						BYTE byType[msg_header_num] = {control_msg, chat_lock_command_acwr};
						m_Network.m_pProcess[LINE_WORLD]->LoadSendMsg(byWorldSocketIndex, byType, (char*)&Send, Send.size());
					}
				}
				break;				// 찾았으면 종료
			}
		}
	}
}

//계정구분리스트에서 계정을 삭제하고 푸쉬클로즈대상자에게 메시지를 보낸다
bool CMainThread::_ReleaseAccount(_ACCOUNT_DATA* pAccount, char* pszCallFunc)
{
	if(!pAccount->m_bLogin)
		return false;

	//만약 밀려나가기 중이였다면..
	if(pAccount->m_idLocalPushWaiter.wIndex != 0xFFFF)
	{
		//m_idLocalPushWaiter가 있으면 
		_push_close_result_aclo Send;

		Send.byRetCode = RET_CODE_SUCCESS;
		memcpy(&Send.idLocal, &pAccount->m_idLocalPushWaiter, sizeof(_CLID));

		BYTE byType[msg_header_num] = {system_msg, push_close_result_aclo};
		m_Network.m_pProcess[LINE_LOGIN]->LoadSendMsg(pAccount->m_byLoginServerIndexPushWaiter, byType, (char*)&Send, Send.size());

		//밀려난기록 남기기..
		if(pAccount->m_byUserCode == LOGIN_CODE_USER)
		{
			_log_sheet_push Sheet;

			Sheet.in_dwAvatorSerial = pAccount->m_dwAccountSerial;
			Sheet.in_dwCloseIP = pAccount->m_dwClientIP;
			Sheet.in_dwPushIP = pAccount->m_dwPusherIP;

			PushDQSData(0xFF, NULL, _DB_QRY_SYN_DATA::log_case_push, (char*)&Sheet, Sheet.size());
		}
	}

#ifdef __SERVICE
	if(pAccount->m_szLoginTime[0] != 0)
	{
		_log_sheet_logtime Sheet;

		Sheet.in_dwAccountSerial = pAccount->m_dwAccountSerial;
		Sheet.in_dwClientIP = pAccount->m_dwClientIP;
		Sheet.in_byUserGrade = pAccount->m_byUserDgr;
		strcpy(Sheet.in_szLoginTime, pAccount->m_szLoginTime);

		PushDQSData(0xFF, NULL, _DB_QRY_SYN_DATA::log_case_logtime, (char*)&Sheet, Sheet.size());
	}
#endif

	pAccount->ReleaseData();

	return true;
}

DWORD CMainThread::cmd_LoginNum()
{
	DWORD dwTotalNum = 0;
	for(int i = 0; i < MAX_USER_PER_GLOBAL; i++)
	{
		if(m_AccountData[i].m_bLogin)
			dwTotalNum++;	
	}

	return dwTotalNum;
}

void CMainThread::WorldInfInform()
{
	if(!m_bOperControl)
		return;

	if(m_tmInformWorldInf.CountingTimer())
	{
		_world_inf_inform_acco			ControlSend;
		_inform_usernum_world_aclo		LoginSend;

		ControlSend.byWorldNum = (BYTE)m_dwServiceSetNum;
		LoginSend.byServiceWorldNum = (BYTE)m_dwServiceSetNum;
		
		for(DWORD i = 0; i < m_dwServiceSetNum; i++)
		{
			_WORLD_DATA* pwd = &m_WorldSetData[i];

			ControlSend.WorldInf[i].byWorldCode = (BYTE)i;
			ControlSend.WorldInf[i].bOpen = pwd->m_bOpen;
			ControlSend.WorldInf[i].bService = pwd->m_bService;
			if(pwd->m_bOpen)
			{
				ControlSend.WorldInf[i].nUserNum = pwd->m_nLoginUserNum;
				LoginSend.wUserNum[i] = pwd->m_nLoginUserNum;
			}
			else
			{
				ControlSend.WorldInf[i].nUserNum = 0;
				LoginSend.wUserNum[i] = 0;
			}
		}
		
		BYTE byControlType[msg_header_num] = {control_msg, world_inf_inform_acco};
		m_Network.m_pProcess[LINE_CONTROL]->LoadSendMsg(m_nControlSocketIndex, byControlType, (char*)&ControlSend, ControlSend.size());	

		BYTE byLoginType[msg_header_num] = {system_msg, inform_usernum_world_aclo};
		for(i = 0; i < MAX_LOGINER_PER_GLOBAL; i++)
		{
			if(m_LobbyData[i].m_bOpen)
				m_Network.m_pProcess[LINE_LOGIN]->LoadSendMsg(i, byLoginType, (char*)&LoginSend, LoginSend.size());	
		}
	}
}

void CMainThread::CheckWorldPing()
{
	DWORD dwCurTime = timeGetTime();

	for(DWORD i = 0; i < m_dwWorldSetNum; i++)
	{
		_WORLD_DATA* pwd = &m_WorldSetData[i];

		if(pwd->m_bOpen)
		{
			if(dwCurTime - pwd->m_dwLastPingTime > ACCOUNT_WORLD_PING_TERM*10)//10초동안 안올때까지..
			{//핑을 받은지 지연된경우..
		//		::StringOutput("%s World Ping Miss!!", pwd->m_szName);

				//todo..
			}
		}
	}		
}

void CMainThread::ForceCloseAccount(_ACCOUNT_DATA* pAccount, bool bDirectly, bool bPushClose, DWORD dwPushIP)
{
	if(pAccount->m_dwWorldCode == 0xFFFFFFFF)
	{
		if(pAccount->m_byLoginServerIndex == 0xFF)
			return;

		//Push Closer가 로그인서버에 있을때 로그인서버에 강제종료로 명령한다
		//
		_force_close_command_aclo Send;

		memcpy(&Send.idLocal, &pAccount->m_idLocal, sizeof(_CLID));

		BYTE byType[msg_header_num] = {system_msg, force_close_command_aclo};
		m_Network.m_pProcess[LINE_LOGIN]->LoadSendMsg(pAccount->m_byLoginServerIndex, byType, (char*)&Send, Send.size());
	}
	else
	{
		//Push Closer가 월드서버에 있을때 월드서버에 강제종료로 명령한다
		//
		BYTE byWorldSocketIndex = (BYTE)m_WorldSetData[pAccount->m_dwWorldCode].m_dwSocketIndex;	
		if(byWorldSocketIndex == 0xFF)
			return;

		_force_close_command_acwr Send;

		memcpy(&Send.idLocal, &pAccount->m_idLocal, sizeof(_CLID));
		Send.bDirectly = bDirectly;
		Send.byKickType = FORCE_CLOSE_TYPE_KICK;
		Send.dwPushIP = dwPushIP;

		BYTE byType[msg_header_num] = {system_msg, force_close_command_acwr};
		m_Network.m_pProcess[LINE_WORLD]->LoadSendMsg(byWorldSocketIndex, byType, (char*)&Send, Send.size());
	}
}

void CMainThread::SendErrMsgToControl(_CLID* pidLocal, const char *fmt, ...)
{
	va_list arg_ptr;
	char str[64+1] = {0,};

	va_start(arg_ptr, fmt);
	vsprintf(str, fmt, arg_ptr);
	va_end(arg_ptr);

	_error_msg_inform_acco Send;

	memcpy(&Send.idClient, pidLocal, sizeof(_CLID));
	strcpy(Send.szMessage, str);

	BYTE byType[msg_header_num] = {control_msg, error_msg_inform_acco};
	m_Network.m_pProcess[LINE_CONTROL]->LoadSendMsg(m_nControlSocketIndex, byType, (char*)&Send, Send.size());
}

bool CMainThread::CommandProcess(char* pszCmd)
{
	BYTE bySize = strlen(pszCmd)+1;

	if(!strcmp("/login num", pszCmd))
	{	
		DWORD dwNum = cmd_LoginNum();
		StringOutput(">>Present %d members on the login_list", dwNum);
		return true;
	}

	else if(!strcmp("/num per world", pszCmd))	//월드당 인원수
	{
/*		cmd_CalcNumPerWorld();

		for(DWORD i = 0; i < m_dwWorldSetNum; i++)
		{
			_WORLD_DATA* pWorld = &m_WorldSetData[i];

			StringOutput("%s: %d\n", pWorld->m_szName, m_nNumPerWorld[i]);
		}
*/
		return true;
	}

	else if(!strncmp("/worldstart ", pszCmd, 12))	//월드 열기
	{		
		char szWorldCode[5];
		if(bySize-12 >= 5)
		{
			return false;
		}

		strncpy(szWorldCode, &pszCmd[12], bySize-12);
		szWorldCode[bySize-12] = NULL;

		int nWorldCode = atoi(szWorldCode);
		if(nWorldCode >= (int)m_dwWorldSetNum)
		{
			::StringOutput("월드코드 초과(0 ~ %d)", m_dwWorldSetNum-1);
			return true;
		}

	//	pc_WorldServiceRequest(nWorldCode, true);

		return true;
	}

	else if(!strncmp("/worldstop ", pszCmd, 11))	//월드 닫기
	{		
		char szWorldCode[5];
		if(bySize-11 >= 5)
		{
			return false;
		}

		strncpy(szWorldCode, &pszCmd[11], bySize-11);
		szWorldCode[bySize-11] = NULL;

		int nWorldCode = atoi(szWorldCode);

		if(nWorldCode >= (int)m_dwWorldSetNum)
		{
			::StringOutput("월드코드 초과(0 ~ %d)", m_dwWorldSetNum-1);
			return true;
		}

//		pc_WorldServiceRequest(nWorldCode, false);

		return true;
	}

	else if(!strncmp("/userexit ", pszCmd, 10))	//월드상의 유저 전부내쫒고 존닫기
	{		
		char szWorldCode[5];
		if(bySize-10 >= 5)
		{
			return false;
		}

		strncpy(szWorldCode, &pszCmd[10], bySize-10);
		szWorldCode[bySize-10] = NULL;

		int nWorldCode = atoi(szWorldCode);

		if(nWorldCode >= (int)m_dwWorldSetNum)
		{
			::StringOutput("월드코드 초과(0 ~ %d)", m_dwWorldSetNum-1);
			return true;
		}

		_WORLD_DATA* pwd = &m_WorldSetData[nWorldCode];

		if(pwd->m_bOpen)
		{
			_world_exit_inform_acwr Send;

			BYTE byType[msg_header_num] = {control_msg, world_exit_inform_acwr};
			m_Network.m_pProcess[LINE_WORLD]->LoadSendMsg(pwd->m_dwSocketIndex, byType, (char*)&Send, Send.size());
		}
		else
		{
			::StringOutput("닫힌 월드: %d", nWorldCode);
		}
		return true;
	}

	else if(!strncmp("/msg ", pszCmd, 5))	//월드상의 모든존에 접속가능하게..
	{		
		char szWorldCode[5];

		int nCnt = 0;
		while(pszCmd[5+nCnt] != ' ')
		{
			szWorldCode[nCnt] = pszCmd[5+nCnt];
			nCnt++;
			if(nCnt > 4)
			{
				::StringOutput("월드코드 초과(0 ~ %d)", m_dwWorldSetNum-1);
				return true;
			}
		}

		szWorldCode[nCnt] = NULL;

		int nWorldCode = atoi(szWorldCode);

		char szMsg[256];

		strcpy(szMsg, &pszCmd[5+nCnt+1]);
		BYTE byMsgSize = bySize - (5+nCnt+1);//NULL포함..
		if(byMsgSize > 255)
		{
			::StringOutput("메시지사이즈 초과(%d > 255)", byMsgSize);
			return true;
		}

//		pc_WorldMsgRequest(nWorldCode, byMsgSize, szMsg);

		return true;
	}	
	return false;
}

bool CMainThread::SearchUserPosition( char* IN szUserAccount, char* OUT szWorldName, char* OUT szAvatorName )
{
	for(int i = 0; i < MAX_USER_PER_GLOBAL; i++)
	{
		if ( m_AccountData[i].m_bLogin )
		{
			if ( strcmp(m_AccountData[i].m_szAccountID, szUserAccount) == 0 )
			{
				if ( m_AccountData[i].m_dwWorldCode == 0xFFFFFFFF )
					strcpy( szWorldName, "로그인서버");
				else if ( m_AccountData[i].m_dwWorldCode < m_dwWorldSetNum )
					strcpy( szWorldName, m_WorldSetData[m_AccountData[i].m_dwWorldCode].m_szName );
				else
					strcpy( szWorldName, "???" );

				strcpy( szAvatorName, m_AccountData[i].m_szAvatorName );

				return true;
			}
		}
	}
	return false;
}

void CMainThread::UpdateWorldServerState()
{
	if (m_tmUpdateWorldServerState.CountingTimer())
	{
		for(DWORD i = 0; i < m_dwServiceSetNum; i++)
		{
			_WORLD_DATA* pwd = &m_WorldSetData[i];
			if ( pwd == NULL )
				continue;

			_upt_case_worldserver Sheet;
			Sheet.bySerial = (BYTE)pwd->m_dwCode;

			if ( m_bExternalLoginOpen )
			{
				if ( pwd->m_bOpen == false )
				{
					Sheet.byState = 3;
				}
				else
				{
					if ( pwd->m_nLoginUserNum >= 1000 )
						Sheet.byState = 2;
					else
						Sheet.byState = 1;
				}
			}
			else
			{
				Sheet.byState = 3;
			}

			PushDQSData(0xFF, NULL, _DB_QRY_SYN_DATA::upt_case_worldserver, (char*)&Sheet, Sheet.size());

		}
	}
}

bool g_IsSuitAccountID(char* pszAccountID)
{
	//65~90	:	41~5A
	//97~122 :	61~7A
	static const bool bSuitTable[256] = {
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
		0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
		0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0		
	};

	if(strlen(pszAccountID) > max_id_len)
		return false;

	if(strlen(pszAccountID) < 2)	//최소한 두자이상
		return false;

	int nOffset = 0;
	while(pszAccountID[nOffset] != NULL)
	{
		if(!bSuitTable[(BYTE)pszAccountID[nOffset]])
			return false;
		nOffset++;
	}

	return true;
}

/*
WORD g_GetIDSearchIndex(char* pszAccountID)
{
	static const BYTE bSuitTable[256] = {
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
		0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
		0xFF,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,	
		0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,	
		0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x30,0x31,0x32,0x33,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,	
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF	
	};

	static const BYTE byAlphabetNum = 26+26+10;

	if(!::g_IsSuitAccountID(pszAccountID))
		return 0xFFFF;

	BYTE byFirst = bSuitTable[(BYTE)pszAccountID[0]];
	if(byFirst == 0xFF)
		return 0xFFFF;

	BYTE bySecond = bSuitTable[(BYTE)pszAccountID[1]];
	if(bySecond == 0xFF)
		return 0xFFFF;

	return byFirst*byAlphabetNum+bySecond;
}
*/