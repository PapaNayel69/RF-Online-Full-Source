#include "StdAfx.h"
#include <process.h>
#include <math.h>  
#include "MainThread.h"
#include "rf_Protocol.h"
#include "pt_account_login.h"
#include "pt_login_client.h"
#include "MyUtil.h"

CMainThread g_Main;

#ifndef __SERVICE
char* pszProcessName = "LoginServer 개발모드";
#endif

#ifdef __SERVICE
char* pszProcessName = "LoginServer 서비스모드";
#endif

_WORLDLIST_DELAY CMainThread::s_WorldListDelay;

static char	szDevSign[] = "!!!";
extern void StringOutput(const char *fmt, ...);
extern void AddOBItemTypeInt( char* szItemName, int* pnItemValue );
extern void AddOBItemTypeChar( char* szItemName, char* pnItemValue );
extern void AddOBItemTypeBool( char* szItemName, bool* pnItemValue );
static char g_szSql[1280];

#define max_cheat_word 10
#define max_cheat_word_size 32
static char s_szCheatWord[max_cheat_word][max_cheat_word_size];
static char* s_pszDstCheat[max_cheat_word] = {&s_szCheatWord[0][0], &s_szCheatWord[1][0], &s_szCheatWord[2][0], &s_szCheatWord[3][0], &s_szCheatWord[4][0], 
												&s_szCheatWord[5][0], &s_szCheatWord[6][0], &s_szCheatWord[7][0], &s_szCheatWord[8][0], &s_szCheatWord[9][0] };

CMainThread::CMainThread()
{
	m_pRFWebDB = NULL;
	m_pX2OnlineDB = NULL;
	m_bAccountLogin = false;
	m_dwWorldNum = 0;
	m_dwCheckAccountOldTick = timeGetTime();
	m_nFilterNum = 0;
	m_bExternalOpen = false;
	m_pFilterInfo = NULL;
	m_nAliveClient = 0;
}

CMainThread::~CMainThread()
{
	m_bDQSThread = false;

	if (m_pRFWebDB)
	{
		delete m_pRFWebDB;
		m_pRFWebDB = NULL;
	}

	if (m_pX2OnlineDB)
	{
		delete m_pX2OnlineDB;
		m_pX2OnlineDB = NULL;
	}

	
	if(m_pFilterInfo)
		delete [] m_pFilterInfo;

//	::CoUninitialize();
}

bool CMainThread::InitMainThread()
{
	if(!_LoadIniFile())
	{
		::StringOutput("CMainThread : _LoadIniFile() => failed");	
	//	return false;
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

	if(!_DatabaseInit())
	{
		::StringOutput("CMainThread : _DatabaseInit() => failed");	
		return false;
	}
	else
		::StringOutput("CMainThread : _NetworkInit() => success");
	
	m_listDQSData.SetList(max_db_syn_data_num);
	m_listDQSDataComplete.SetList(max_db_syn_data_num);
	m_listDQSDataEmpty.SetList(max_db_syn_data_num);
	for(int i = 0; i < max_db_syn_data_num; i++)
		m_listDQSDataEmpty.PushNode_Back(i);

	m_bDQSThread = true;
	::_beginthread(DQSThread, 0, (void*)this);

	::StringOutput("!!Start!!");	

	if(m_bExternalOpen)
		::StringOutput("외부 접속 가능상태");	
	else
		::StringOutput("외부 접속 불가상태");	

	AddOBItemTypeInt( "Client수", &m_nAliveClient );
	AddOBItemTypeInt( "둥록된 월드수", (int*)&m_dwWorldNum );
	AddOBItemTypeInt( "서비스 월드수", (int*)&m_dwServiceWorldNum );
	AddOBItemTypeBool( "외부접속", &m_bExternalOpen );
	AddOBItemTypeInt( "필터링IP항목수", (int*)&m_nFilterNum );

	return true;
}

void CMainThread::MainLoop()
{
	m_Network.OnLoop();
	DQSCompleteProcess();
	CheckAccountLineState();
	s_WorldListDelay.CheckOnLoop();
	_CheckReserve();
	Sleep(1);
}

char* CMainThread::GetProcessName()
{
	return pszProcessName;
}

bool CMainThread::CommandProcess(char* pszCmd)
{
	int nWordNum = ::ParsingCommand(pszCmd, max_cheat_word, s_pszDstCheat, max_cheat_word_size);
	if(nWordNum <= 0)
		return false;
	
	if(!strcmp("/open", s_pszDstCheat[0]))
	{
		::StringOutput("외부 접속 가능상태로 변경");	
		m_bExternalOpen = true;
		return true;
	}
	else if(!strcmp("/close", s_pszDstCheat[0]))
	{
		::StringOutput("외부 접속 불가상태로 변경");	
		m_bExternalOpen = false;
		return true;
	}
	else if(!strcmp("/patchup", s_pszDstCheat[0]))
	{
		DWORD dwBufNum = m_dwPatchServerNum + 1;
		if(dwBufNum > MAX_PATCHER_PER_GLOBAL)
			dwBufNum = MAX_PATCHER_PER_GLOBAL;		

		::StringOutput("패치서버 증가(%d -> %d)", m_dwPatchServerNum, dwBufNum);	
		m_dwPatchServerNum = dwBufNum;
		return true;
	}
	else if(!strcmp("/patchdown", s_pszDstCheat[0]))
	{
		DWORD dwBufNum;
		if(m_dwPatchServerNum <= 1)
			dwBufNum = 1;
		else
			dwBufNum = m_dwPatchServerNum - 1;	

		::StringOutput("패치서버 감소(%d -> %d)", m_dwPatchServerNum, dwBufNum);	
		m_dwPatchServerNum = dwBufNum;
		return true;
	}
	else if(!strcmp("/예약", s_pszDstCheat[0]))
	{
		if(nWordNum > 1)
		{
			if(!strcmp("외부차단", s_pszDstCheat[1]) && nWordNum == 7)
			{
				if(_SetReserveExternalClose(atoi(s_pszDstCheat[2]), atoi(s_pszDstCheat[3]), atoi(s_pszDstCheat[4]), atoi(s_pszDstCheat[5]), atoi(s_pszDstCheat[6])))
					::StringOutput("예약접수");
				else
					::StringOutput("예약시간오류");
				return true;
			}
		}
	}
	else if(!strcmp("/loginstat", s_pszDstCheat[0]))
	{
		if ( m_bExternalOpen )
			::StringOutput("외부접속 가능 상태");
		else
			::StringOutput("외부접속 불가 상태");
		return true;
	}
	else if(!strcmp("/clientcount", s_pszDstCheat[0]))
	{
		DWORD dwClientCount = CountAliveConnection();
		::StringOutput( "Connection수 : %d", dwClientCount );
		return true;
	}

	return false;
}

bool CMainThread::_LoadIniFile()
{
	//load patch server ip address form ini file
	//
	//char szIP[32];
	//GetPrivateProfileString("Patch", "PatchAddress", "X", szIP, sizeof(szIP), ".\\Initialize\\LoginSystem.ini");
	//m_dwPatchIP = inet_addr(szIP);
	//if(!strcmp(szIP, "X"))
	//	return false;

	AddOBItemTypeInt( "패치서버수", (int*)&m_dwPatchServerNum );

	char szIP[32] = {0,};

	GetPrivateProfileString("Patch", "PatchAddress1", "X", szIP, sizeof(szIP), ".\\Initialize\\LoginSystem.ini");
	if ( strcmp(szIP, "X") == 0 )
		return false;

	m_dwPatchIP[0] = inet_addr(szIP);
	AddOBItemTypeChar( "PatchAddress1", szIP );

	GetPrivateProfileString("Patch", "PatchAddress2", "X", szIP, sizeof(szIP), ".\\Initialize\\LoginSystem.ini");
	if ( strcmp(szIP, "X") == 0 )
		return false;

	m_dwPatchIP[1] = inet_addr(szIP);
	AddOBItemTypeChar( "PatchAddress2", szIP );

	//ip filter setting
	//
	m_nFilterNum = 	GetPrivateProfileInt("IP Adderss Filter", "FilterNum", -1, ".\\Initialize\\LoginSystem.ini");
	if(m_nFilterNum == -1)
		return false;
	if(m_nFilterNum > 0)
	{
		m_pFilterInfo = new __FILTER_INFO [m_nFilterNum];

		for(int i = 0; i < m_nFilterNum; i++)
		{
			char szKey[128], szVal[128];
			sprintf(szKey, "Filter%d", i);
			GetPrivateProfileString("IP Adderss Filter", szKey, "X", szVal, sizeof(szVal), ".\\Initialize\\LoginSystem.ini");
			*((DWORD*)m_pFilterInfo[i].byIPAddressPoint) = inet_addr(szVal);

			sprintf(szKey, "FilterRange%d", i);
			m_pFilterInfo[i].byIPAddressRange = GetPrivateProfileInt("IP Adderss Filter", szKey, 0, ".\\Initialize\\LoginSystem.ini");
		}
	}

	return true;
}

bool CMainThread::_ObjectInit()
{
	m_dwWorldNum = 0;
	m_dwPatchServerNum = 2;
	
	for(int i = 0; i < MAX_LOGIN_CLINET_NUM; i++)
		m_ClientData[i].Init((WORD)i);

	m_tmReserveCheck.BeginTimer(30000);	//30초..

	return true;
}

bool CMainThread::_NetworkInit()
{
	_NET_TYPE_PARAM typeParam[LINE_NUM];
	
	typeParam[LINE_ACCOUNT].m_bRealSockCheck =	TRUE;
	typeParam[LINE_ACCOUNT].m_bSystemLogFile = TRUE;
	typeParam[LINE_ACCOUNT].m_bServer = FALSE;
	typeParam[LINE_ACCOUNT].m_bSendThread = TRUE;
	typeParam[LINE_ACCOUNT].m_byRecvThreadNum = 1;
	typeParam[LINE_ACCOUNT].m_byRecvSleepTime = 1;
	typeParam[LINE_ACCOUNT].m_wSocketMaxNum = 1;
	typeParam[LINE_ACCOUNT].m_bSvrToS = TRUE;
	typeParam[LINE_ACCOUNT].m_bOddMsgWriteLog = TRUE;
	sprintf(typeParam[LINE_ACCOUNT].m_szModuleName, "AccountLine");

	typeParam[LINE_CLIENT].m_bRealSockCheck = TRUE;
	typeParam[LINE_CLIENT].m_bSpeedHackCheck = TRUE;
	typeParam[LINE_CLIENT].m_bSystemLogFile = TRUE;
	typeParam[LINE_CLIENT].m_bServer = TRUE;
	typeParam[LINE_CLIENT].m_byRecvThreadNum = 2;
	typeParam[LINE_CLIENT].m_byRecvSleepTime = 1;
	typeParam[LINE_CLIENT].m_wPort = PORT_LOGIN_CLINET;
	typeParam[LINE_CLIENT].m_wSocketMaxNum = MAX_LOGIN_CLINET_NUM;
	typeParam[LINE_CLIENT].m_bSendThread = TRUE;
	typeParam[LINE_CLIENT].m_bOddMsgWriteLog = TRUE;
	typeParam[LINE_CLIENT].m_bOddMsgDisconnect = TRUE;
	typeParam[LINE_CLIENT].m_dwProcessMsgNumPerLoop = 1;
//	typeParam[LINE_CLIENT].m_dwSendBufferSize = 1000;
//	typeParam[LINE_CLIENT].m_dwRecvBufferSize = 1000;
	sprintf(typeParam[LINE_CLIENT].m_szModuleName, "ClientLine");

	m_Network.SetNetSystem(LINE_NUM, typeParam, "LoginServer");
	return true;
}

bool CMainThread::_DatabaseInit()
{
#ifdef __SERVICE

	//X2Online DB 접속
	m_pX2OnlineDB = new CRFWebDatabase;
	if ( !m_pX2OnlineDB->StartDataBase("x2online", "loginChk", "lo2%xqt" ) )
	{
		::StringOutput("CMainThread : m_pX2OnlineDB->StartDataBase = failed");
		return false;
	}

#endif 

	//클로즈테스터 DB 접속
	m_pRFWebDB = new CRFWebDatabase;
	if ( !m_pRFWebDB->StartDataBase("rfweb", "rfadmin", "rf#9k7s" ) )
	{
		::StringOutput("CMainThread : m_pRFWebDB->StartDataBase = failed");
		return false;
	}

	return true;
}

void CMainThread::_CheckReserve()
{
	if(m_tmReserveCheck.CountingTimer())
	{
		_CheckReserveExternalClose();
	}
}

bool CMainThread::PushDQSData(_CLID* pidLocal, BYTE byQryCase, char* pQryData, int nSize)
{
	DWORD dwEmptyIndex;
	if(!m_listDQSDataEmpty.PopNode_Front(&dwEmptyIndex))
		return false;

	_DB_QRY_SYN_DATA* pData = &m_DBQrySynData[dwEmptyIndex];

	if(pidLocal)
		memcpy(&pData->m_idLocal, pidLocal, sizeof(_CLID));
	pData->m_byQryCase = byQryCase;
	if(pQryData)
		memcpy(pData->m_sData, pQryData, nSize);
	
	if(!m_listDQSData.PushNode_Back(dwEmptyIndex))
		return false;

	pData->m_bUse = true;
	pData->m_bLoad = false;

	return true;
}

void CMainThread::DQSThread(void* pv)
{
	CMainThread* pDoc = (CMainThread*)pv;
	DWORD dwDataIndex;

	if(!pDoc->m_logDB_Thread.m_bInit)
		pDoc->m_logDB_Thread.SetWriteLogFile(".\\NetLog\\LoginServer_DB.log", true, true);

	while(pDoc->m_bDQSThread)
	{
		pDoc->m_DBFrameRate.CalcSpeedPerFrame();

		while(pDoc->m_listDQSData.PopNode_Front(&dwDataIndex))
		{
			_DB_QRY_SYN_DATA* p = &pDoc->m_DBQrySynData[dwDataIndex];
			switch(p->m_byQryCase)
			{
			case _DB_QRY_SYN_DATA::qry_case_certify:{				
				p->m_byResult = pDoc->db_certify_account((_qry_sheet_certify*)p->m_sData);
			  }break;
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
		case _DB_QRY_SYN_DATA::qry_case_certify:{
			Certify_Account_Complete(&p->m_idLocal, p->m_byResult, (_qry_sheet_certify*)p->m_sData);
		  }break;
		}

		p->m_bUse = false;
		p->m_bLoad = false;
		m_listDQSDataEmpty.PushNode_Back(dwDataIndex);
	}
}

void CMainThread::pc_WorldListResult(DWORD dwWorldNum, _world_list_result_aclo::__list* pWorldList, BYTE byServiceWorldNum)
{
	m_dwWorldNum = dwWorldNum;
	m_dwServiceWorldNum = byServiceWorldNum;

	//world setting..
	//
	for(DWORD i = 0; i < dwWorldNum; i++)
	{
		m_WorldData[i].SetWorld(pWorldList[i].szWorldName);
		AddOBItemTypeBool( m_WorldData[i].m_szWorldName, &m_WorldData[i].m_bOpen );
	}

	//open world..
	//
	for(i = 0; i < dwWorldNum; i++)
	{
		if(pWorldList[i].bOpen)
			m_WorldData[i].OpenWorld(pWorldList[i].dwGateIP, pWorldList[i].wGatePort);
	}

	//log..
	//
	::StringOutput("Account Server Login..");
	::StringOutput("Registrated World Number (%d)..", dwWorldNum);
	for(i = 0; i < dwWorldNum; i++)
		::StringOutput("World Code (%d), Name (%s), Open (%d) ..", i, m_WorldData[i].m_szWorldName, m_WorldData[i].m_bOpen);

	
}

void CMainThread::pc_InformOpenWorld(DWORD dwWorldCode, DWORD dwGateIP, WORD wGatePort)
{
	m_WorldData[dwWorldCode].OpenWorld(dwGateIP, wGatePort);

	//log..
	//
	::StringOutput("World Open: Code (%d), Name (%s)..", dwWorldCode, m_WorldData[dwWorldCode].m_szWorldName);
}

void CMainThread::pc_InformCloseWorld(DWORD dwWorldCode)
{
	m_WorldData[dwWorldCode].CloseWorld();

	//log..
	//
	::StringOutput("World Close: Code (%d), Name (%s)..", dwWorldCode, m_WorldData[dwWorldCode].m_szWorldName);
}

void CMainThread::pc_InformUserNumWorld(BYTE byServiceWorldNum, WORD* pwUserNum)
{
	for(DWORD i = 0; i < byServiceWorldNum; i++)
	{
		m_WorldData[i].SetUserNum((int)pwUserNum[i]);
	}
}

void CMainThread::pc_JoinAccountResult(BYTE byRetCode, _CLID* pidLocal)
{
	_CLIENT_DATA* pObj = &m_ClientData[pidLocal->wIndex];

	//compare local serial 
	//
	if(pObj->m_idLocal.dwSerial != pidLocal->dwSerial)
		return;

	//send to client result
	//
	_join_account_result_locl Send;

	Send.byRetCode = byRetCode;

	BYTE byType[msg_header_num] = {account_msg, join_account_result_locl};
	m_Network.m_pProcess[LINE_CLIENT]->LoadSendMsg(pObj->m_idLocal.wIndex, byType, (char*)&Send, Send.size());
}

void CMainThread::pc_LoginAccountResult(BYTE byRetCode, DWORD dwAccountSerial, BYTE byUserGrade, _CLID* pidLocal, _GLBID* pgidNewGlobal)
{
	static int nSelPatch = 0;

	_CLIENT_DATA* pObj = &m_ClientData[pidLocal->wIndex];

	//compare local serial 
	//
	if(pObj->m_idLocal.dwSerial != pidLocal->dwSerial)
	{
		//그사이에 나간경우라면 그 사실을 알린다..
		_logout_account_request_loac Send;

		memcpy(&Send.gidGlobal, pgidNewGlobal, sizeof(_GLBID));

		BYTE byType[msg_header_num] = {system_msg, logout_account_request_loac};
		m_Network.m_pProcess[LINE_ACCOUNT]->LoadSendMsg(0, byType, (char*)&Send, Send.size());

		return;
	}

	if(byRetCode == RET_CODE_SUCCESS)
	{
		//set client object data
		//
		pObj->Login(pgidNewGlobal, dwAccountSerial, byUserGrade);
	}
	else if(byRetCode == RET_CODE_ALREADY_LOGIN)
	{
		//set client overlapped state
		//
		pObj->SetOverlap(true, dwAccountSerial);
	}

	//send to client result
	//
	_login_account_result_locl Send;

	Send.byRetCode = byRetCode;
	if(byRetCode == RET_CODE_SUCCESS)
	{
		Send.dwAccountSerial = dwAccountSerial;
		Send.dwPatchServerIP = m_dwPatchIP[(nSelPatch++)%m_dwPatchServerNum];
		Send.wPatchServerPort = 80;
	}

	BYTE byType[msg_header_num] = {account_msg, login_account_result_locl};
	m_Network.m_pProcess[LINE_CLIENT]->LoadSendMsg(pObj->m_idLocal.wIndex, byType, (char*)&Send, Send.size());
}

void CMainThread::pc_SelectWorldResult(BYTE byRetCode, _CLID* pidLocal, DWORD* pdwWorldMasterKey)
{
	_CLIENT_DATA* pObj = &m_ClientData[pidLocal->wIndex];

	//compare local serial 
	//
//	::StringOutput("select: i: %d, s: %d", pidLocal->wIndex, pidLocal->dwSerial);

	if(pObj->m_idLocal.dwSerial != pidLocal->dwSerial)
		return;

	if(byRetCode == RET_CODE_SUCCESS)
	{
		//client object select world
		//
		if(pObj->m_dwSeletWorldCode == 0xFFFFFFFF)
			return;

		pObj->EnterWorld();

		//push close node
		//
		m_Network.Close(LINE_CLIENT, pObj->m_idLocal.wIndex, true);
	}

	//send to client result
	//
	_select_world_result_locl Send;

	Send.byRetCode = byRetCode;
	if(byRetCode == RET_CODE_SUCCESS)
	{
		_WORLD_DATA* pWorld = &m_WorldData[pObj->m_dwSeletWorldCode];
		Send.dwWorldGateIP = pWorld->m_dwGateIP;
		Send.wWorldGatePort = pWorld->m_wGatePort;
		memcpy(Send.dwWorldMasterKey, pdwWorldMasterKey, sizeof(DWORD)*CHECK_KEY_NUM);
	}

	BYTE byType[msg_header_num] = {account_msg, select_world_result_locl};
	m_Network.m_pProcess[LINE_CLIENT]->LoadSendMsg(pObj->m_idLocal.wIndex, byType, (char*)&Send, Send.size());
}

void CMainThread::pc_PushCloseResult(BYTE byRetCode, _CLID* pidLocal)
{
	_CLIENT_DATA* pObj = &m_ClientData[pidLocal->wIndex];

	//compare local serial 
	//
	if(pObj->m_idLocal.dwSerial != pidLocal->dwSerial)
		return;

	_push_close_result_locl Send;

	Send.byRetCode = byRetCode;

	BYTE byType[msg_header_num] = {account_msg, push_close_result_locl};
	m_Network.m_pProcess[LINE_CLIENT]->LoadSendMsg(pObj->m_idLocal.wIndex, byType, (char*)&Send, Send.size());
}

void CMainThread::pc_ForceCloseCommand(_CLID* pidLocal)
{
	_CLIENT_DATA* pObj = &m_ClientData[pidLocal->wIndex];

	//compare local serial 
	//
	if(pObj->m_idLocal.dwSerial != pidLocal->dwSerial)
		return;

	//유저에게 미리 공지함
	//
	_server_notify_inform_locl Send;

	Send.wMsgCode = FORCE_CLOSE_TYPE_PUSH;

	BYTE byType[msg_header_num] = {account_msg, server_notify_inform_locl};
	m_Network.m_pProcess[LINE_CLIENT]->LoadSendMsg(pObj->m_idLocal.wIndex, byType, (char*)&Send, Send.size());

	//강제 close 노드에푸쉬..남은노드없으면 그냥 클로즈..
	//
	m_Network.Close(LINE_CLIENT, pidLocal->wIndex, true);
}

void CMainThread::pc_JoinAccountRequest(DWORD dwClientIndex, char* pszAccountID, char* pszPassword)
{
//	char			szID[max_id_len+1];

	//User인지..dev인지..확인하자..
	//먼저 사인보다 커야한다..
	if(strlen(pszAccountID) > strlen(szDevSign))
	{
		if(strncmp(szDevSign, pszAccountID, strlen(szDevSign)))
			return;
	}
	else
		return;

//	strcpy(szID, &pszAccountID[strlen(szDevSign)]);
	
	//query to account server
	//
	_join_account_request_loac Send;

	strcpy(Send.szAccountID, pszAccountID);
	strcpy(Send.szPassword, pszPassword);
	memcpy(&Send.idLocal, &m_ClientData[dwClientIndex].m_idLocal, sizeof(_CLID));
	Send.dwClientIP = m_Network.GetSocket(LINE_CLIENT, dwClientIndex)->m_Addr.sin_addr.s_addr;

	BYTE byType[msg_header_num] = {system_msg, join_account_request_loac};
	m_Network.m_pProcess[LINE_ACCOUNT]->LoadSendMsg(0, byType, (char*)&Send, Send.size());
}

void CMainThread::pc_LoginAccountRequest(DWORD dwClientIndex, char* pszAccountID, char* pszPassword)
{
	_CLIENT_DATA*	pObj = &m_ClientData[dwClientIndex];
	BYTE			byLoginCode = LOGIN_CODE_USER;
	_socket*		pSocket = m_Network.GetSocket(LINE_CLIENT, dwClientIndex);

	if(pObj->m_idLocal.dwSerial == 0xFFFFFFFF)
		return;

	if(pObj->m_bLoginCheck)
		return;

	//User인지..dev인지..확인하자..
	if(strlen(pszAccountID) > strlen(szDevSign))
	{
		if(!strncmp(szDevSign, pszAccountID, strlen(szDevSign)))
			byLoginCode = LOGIN_CODE_DEV;
	}

	_qry_sheet_certify Sheet;

	Sheet.in_byLoginCode = byLoginCode;
	Sheet.in_dwClientIP = *(DWORD*)&pSocket->m_Addr.sin_addr;
	strcpy(Sheet.in_szAccountID, pszAccountID);
	strcpy(Sheet.in_szPassword, pszPassword);

	if(!PushDQSData(&pObj->m_idLocal, _DB_QRY_SYN_DATA::qry_case_certify, (char*)&Sheet, Sheet.size()))
	{
		Certify_Account_Complete(&pObj->m_idLocal, RET_CODE_INTERNAL_ERROR, &Sheet);
		return;
	}
}

void CMainThread::Certify_Account_Complete(_CLID* pidLocal, BYTE byRetCode, _qry_sheet_certify* pData)
{
	_CLIENT_DATA*	pObj = &m_ClientData[pidLocal->wIndex];

	if(pObj->m_idLocal.dwSerial != pidLocal->dwSerial)
		return;

	if(byRetCode == RET_CODE_SUCCESS)
	{
		//set prelogin
		//
		pObj->PreLogin(pData->in_szAccountID, pData->in_szPassword, pData->in_byLoginCode);

		//query to account server
		//
		_login_account_request_loac Send;

		Send.byUserCode = pData->in_byLoginCode; 
		strcpy(Send.szAccountID, pData->in_szAccountID);
		strcpy(Send.szPassword, pData->in_szPassword);
		Send.dwClientIP = pData->in_dwClientIP;
		memcpy(&Send.idLocal, pidLocal, sizeof(_CLID));

		BYTE byType[msg_header_num] = {system_msg, login_account_request_loac};
		m_Network.m_pProcess[LINE_ACCOUNT]->LoadSendMsg(0, byType, (char*)&Send, Send.size());
	}
	else if(byRetCode == RET_CODE_BLOCK_CHAT)
	{
		pObj->PreLogin(pData->in_szAccountID, pData->in_szPassword, pData->in_byLoginCode);

		_login_account_request_loac Send;

		Send.byUserCode = pData->in_byLoginCode; 
		strcpy(Send.szAccountID, pData->in_szAccountID);
		strcpy(Send.szPassword, pData->in_szPassword);
		Send.dwClientIP = pData->in_dwClientIP;
		memcpy(&Send.idLocal, pidLocal, sizeof(_CLID));
		// 채금추가

		BYTE byType[msg_header_num] = {system_msg, login_account_request_loac};
		m_Network.m_pProcess[LINE_ACCOUNT]->LoadSendMsg(0, byType, (char*)&Send, Send.size());
	}
	else
	{
		_login_account_result_locl Send;

		Send.byRetCode = byRetCode;

		BYTE byType[msg_header_num] = {account_msg, login_account_result_locl};
		m_Network.m_pProcess[LINE_CLIENT]->LoadSendMsg(pidLocal->wIndex, byType, (char*)&Send, Send.size());
	}
}

void CMainThread::pc_WorldListRequest(DWORD dwClientIndex, DWORD dwClientVersion)
{
	BYTE byRetCode = RET_CODE_SUCCESS;

	_CLIENT_DATA* pObj = &m_ClientData[dwClientIndex];

	if(pObj->m_idLocal.dwSerial == 0xFFFFFFFF)
		return;

	//be loged in account server
	//
	if(!pObj->m_bLogin)
	{
		byRetCode = RET_CODE_BEFORE_LOGIN;
		goto RESULT;
	}
	
	//compare client version
	//
//	if(m_dwClientVersion != dwClientVersion)
//	{
//		byRetCode = RET_CODE_NOT_EQUAL_VERSION;
//		goto RESULT;
//	}

	//examine to recive world list already
	//
	if(pObj->m_bRegedWorld)
	{
		byRetCode = RET_CODE_BEFORE_WORLD_LIST;
		goto RESULT;
	}

RESULT:

	pObj->Reged();

	if(byRetCode != RET_CODE_SUCCESS)
	{
		_world_list_result_locl Send;

		Send.byRetCode = byRetCode;
		Send.wDataSize = 0;

		BYTE byType[msg_header_num] = {account_msg, world_list_result_locl};
		m_Network.m_pProcess[LINE_CLIENT]->LoadSendMsg(dwClientIndex, byType, (char*)&Send, Send.size());
	}
	else
	{
		s_WorldListDelay.Push(pObj->m_idLocal.wIndex, pObj->m_idLocal.dwSerial);
	}
}

void CMainThread::pc_SelectWorldRequest(DWORD dwClientIndex, WORD wWorldIndex)
{
	BYTE byRetCode = RET_CODE_SUCCESS;

	_CLIENT_DATA* pObj = &m_ClientData[dwClientIndex];

	//be loged in account server
	//
	if(pObj->m_idLocal.dwSerial == 0xFFFFFFFF)
		return;

	if(!pObj->m_bLogin)
	{
		byRetCode = RET_CODE_BEFORE_LOGIN;
		goto RESULT;
	}

	//examine to recive world list already
	//
	if(!pObj->m_bRegedWorld)
	{
		byRetCode = RET_CODE_NOT_WORLD_LIST;
		goto RESULT;
	}

	//일반이 개발월드를 선택했는지..
	//
	if(pObj->m_byUserGrade == USER_DEGREE_STD)	//다른 등급중에 개발월드를 못들어가지만
	{											//월드리스트 내려줄때 선별해서 내려줌으로 여기에서는 일반만 막도록한다.
		if(wWorldIndex >= m_dwServiceWorldNum)	
		{
			byRetCode = RET_CODE_NONE_AUTHOR_WORLD;
			goto RESULT;
		}
	}

	//examine to open selected world
	//
	if(!m_WorldData[wWorldIndex].m_bOpen)
	{
		byRetCode = RET_CODE_NOT_OPEN_WORLD;
		goto RESULT;
	}

RESULT:

	if(byRetCode != RET_CODE_SUCCESS)
	{
		//send result
		//
		_select_world_result_locl Send;

		Send.byRetCode = byRetCode;

		BYTE byType[msg_header_num] = {account_msg, select_world_result_locl};
		m_Network.m_pProcess[LINE_CLIENT]->LoadSendMsg(dwClientIndex, byType, (char*)&Send, Send.size());
	}
	else
	{
		//write selected world code on buffer
		//
		pObj->SelectWorld(wWorldIndex);

		//send query
		//

		_socket* pSocket = m_Network.GetSocket(LINE_CLIENT, dwClientIndex);
		_select_world_request_loac Send;

		memcpy(&Send.gidGlobal, &pObj->m_gidGlobal, sizeof(_GLBID));
		Send.wWorldIndex = wWorldIndex;
		Send.dwClientIP = (DWORD)pSocket->m_Addr.sin_addr.S_un.S_addr;

		BYTE byType[msg_header_num] = {system_msg, select_world_request_loac};
		m_Network.m_pProcess[LINE_ACCOUNT]->LoadSendMsg(0, byType, (char*)&Send, Send.size());
	}
}

void CMainThread::CheckAccountLineState()
{
	_socket* pSocket = m_Network.GetSocket(LINE_ACCOUNT, 0);		
	if(pSocket->m_bAccept)
		return;

	DWORD dwNewTick = timeGetTime();
	int nGap = dwNewTick-m_dwCheckAccountOldTick;
	if(nGap > 5000)
	{
		m_dwCheckAccountOldTick = dwNewTick;

 		char szIP[32];
		GetPrivateProfileString("Account", "AccountAddress", "127.0.0.1", szIP, sizeof(szIP), ".\\Initialize\\LoginSystem.ini");
		m_Network.Connect(LINE_ACCOUNT, 0, inet_addr(szIP), PORT_ACCOUNT_LOGIN);
	}
	else if(nGap < 0)
		m_dwCheckAccountOldTick = dwNewTick;
}

void CMainThread::LoginAccount()
{
	_world_list_request_loac Send;

	BYTE byType[msg_header_num] = {system_msg, world_list_request_loac};
	m_Network.m_pProcess[LINE_ACCOUNT]->LoadSendMsg(0, byType, (char*)&Send, Send.size());
}

void CMainThread::pc_PushCloseRequest(DWORD dwClientIndex, char* pszAccountID, char* pszPassword)
{
	BYTE byRetCode = RET_CODE_SUCCESS;

	_CLIENT_DATA* pObj = &m_ClientData[dwClientIndex];

	if(!pObj->m_bOverlapUser)
	{
		byRetCode = RET_CODE_NET_PROCESS_ERROR;
		goto RESULT;
	}

RESULT:

	if(byRetCode == RET_CODE_SUCCESS)
	{

		_push_close_request_loac Query;

		Query.dwAccountSerial = pObj->m_dwAccountSerial;
		Query.dwClientIP = m_Network.GetSocket(LINE_CLIENT, dwClientIndex)->m_Addr.sin_addr.s_addr;
		memcpy(&Query.idLocal, &pObj->m_idLocal, sizeof(_CLID));
		Query.byUserCode = pObj->m_byLoginCode;

		BYTE byType[msg_header_num] = {system_msg, push_close_request_loac};
		m_Network.m_pProcess[LINE_ACCOUNT]->LoadSendMsg(0, byType, (char*)&Query, Query.size());

		pObj->SetOverlap(false, 0xFFFFFFFF);
	}
}

bool CMainThread::_SetReserveExternalClose(int nYear, int nMonth, int nDay, int nHour, int nMin)
{
	int nLastYear = ::GetCurrentYear();
	int nLastMonth = ::GetCurrentMonth();
	int nLastDay = ::GetCurrentDay();
	int nLastHour = ::GetCurrentHour();
	int nLastMin = ::GetCurrentMin();

	if(nLastYear > nYear)
		return false;
	if(nLastMonth > nMonth && !(nLastYear < nYear))
		return false;
	if(nLastDay > nDay && !(nLastYear < nYear || nLastMonth < nMonth))
		return false;
	if(nLastHour > nHour && !(nLastYear < nYear || nLastMonth < nMonth || nLastDay < nDay))
		return false;
	if(nLastMin > nMin && !(nLastYear < nYear || nLastMonth < nMonth || nLastDay < nDay || nLastHour < nHour))
		return false;

	m_ReserveExternalClose.dwYear = nYear;
	m_ReserveExternalClose.dwMonth = nMonth;
	m_ReserveExternalClose.dwDay = nDay;
	m_ReserveExternalClose.dwHour = nHour;
	m_ReserveExternalClose.dwMin = nMin;

	m_ReserveExternalClose.bReserve = true;

	return true;
}

void CMainThread::_CheckReserveExternalClose()
{
	if(!m_ReserveExternalClose.bReserve)
		return;

	if(::GetCurrentYear() < (int)m_ReserveExternalClose.dwYear)
		return;
	if(::GetCurrentMonth() < (int)m_ReserveExternalClose.dwMonth)
		return;
	if(::GetCurrentDay() < (int)m_ReserveExternalClose.dwDay)
		return;
	if(::GetCurrentHour() < (int)m_ReserveExternalClose.dwHour)
		return;
	if(::GetCurrentMin() < (int)m_ReserveExternalClose.dwMin)
		return;

	m_ReserveExternalClose.bReserve = false;

	//실행..
	char szDate[32], szTime[32];
	_strdate(szDate);
	_strtime(szTime);

	::StringOutput("예약변경: 외부접속 불가상태 (%s : %s)", szDate, szTime);	
	m_bExternalOpen = false;
}

bool CMainThread::IsPassIPFilter(DWORD dwAcceptIP)
{
	if(m_nFilterNum == 0)
		return false;

	for(int i = 0; i < m_nFilterNum; i++)
	{
		DWORD dwIP = *((DWORD*)m_pFilterInfo[i].byIPAddressPoint);
		if(m_pFilterInfo[i].byIPAddressRange == 255
			&& m_pFilterInfo[i].byIPAddressPoint[3] == 0)	//클래스전체를 허용하는거라면..
		{
			DWORD dwBufClientIP = dwAcceptIP;
			DWORD dwBufFilterIP = dwIP;

			((BYTE*)&dwBufClientIP)[3] = ((BYTE*)&dwBufFilterIP)[3] = 0;			 
			if(dwBufFilterIP == dwBufClientIP)
				return true;
		}
		else
		{
			for(int a = m_pFilterInfo[i].byIPAddressPoint[3]; a <= m_pFilterInfo[i].byIPAddressRange; a++)
			{
				((BYTE*)&dwIP)[3] = (BYTE)a;
				if(dwIP == dwAcceptIP)
					return true;
			}
		}
	}
	return false;
}

DWORD CMainThread::CountAliveConnection()
{
	DWORD dwClientCount = 0;

	for ( int i=0; i<MAX_LOGIN_CLINET_NUM; i++)
	{
		if ( m_ClientData[i].m_idLocal.dwSerial != 0xFFFFFFFF )
			dwClientCount++;
	}
	
	return dwClientCount;
}

void _WORLDLIST_DELAY::Process(DWORD dwIndex, DWORD dwSerial)
{
	_CLIENT_DATA* pObj = &g_Main.m_ClientData[dwIndex];
	if(pObj->m_idLocal.dwSerial != dwSerial)
		return;

	_world_list_result_locl Send;

	Send.byRetCode = RET_CODE_SUCCESS;

	//일단계정은 서비스모드인것만 알리고, 개발등급이상은 전체를 다보낸다.
	DWORD dwWorldNumBuf = g_Main.m_dwServiceWorldNum;
	if(pObj->m_byUserGrade == USER_DEGREE_DEV || 
		pObj->m_byUserGrade == USER_DEGREE_ULTRA ||
		pObj->m_byUserGrade == USER_DEGREE_MGR)
		dwWorldNumBuf = g_Main.m_dwWorldNum;

	char* p = Send.sListData;
	WORD offset = 0;

	memcpy(&p[offset], &dwWorldNumBuf, sizeof(BYTE));
	offset += sizeof(BYTE);

	for(DWORD i = 0; i < dwWorldNumBuf; i++)
	{
		memcpy(&p[offset], &g_Main.m_WorldData[i].m_bOpen, sizeof(BYTE));
		offset += sizeof(BYTE);

		BYTE byLen = strlen(g_Main.m_WorldData[i].m_szWorldName)+1;
		memcpy(&p[offset], &byLen, sizeof(BYTE));
		offset += sizeof(BYTE);

		memcpy(&p[offset], g_Main.m_WorldData[i].m_szWorldName, byLen);
		offset += byLen;
	}
	Send.wDataSize = offset;

	BYTE bySendType[msg_header_num] = {account_msg, world_list_result_locl};
	g_Main.m_Network.m_pProcess[LINE_CLIENT]->LoadSendMsg(dwIndex, bySendType, (char*)&Send, Send.size());

	//접속유저수를 알려준다
	_world_user_inform_locl Inform;

	Inform.byServiceWorldNum = (BYTE)dwWorldNumBuf;
	for(i = 0; i < dwWorldNumBuf; i++)
	{
		Inform.wUserNum[i] = (WORD)g_Main.m_WorldData[i].m_nUserNum;			
	}
	BYTE byInformType[msg_header_num] = {account_msg, world_user_inform_locl};
	g_Main.m_Network.m_pProcess[LINE_CLIENT]->LoadSendMsg(dwIndex, byInformType, (char*)&Inform, Inform.size());
}

