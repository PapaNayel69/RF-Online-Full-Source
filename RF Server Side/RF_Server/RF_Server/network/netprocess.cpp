// NetProcess.cpp: implementation of the CNetProcess class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "NetProcess.h"
#include <process.h>
#include <mmsystem.h>
#include "NetUtil.h"
#include "NetWorking.h"

static _check_query sQry;
static BYTE sbyQryType[msg_header_num] = {check_msg, check_query};
static _check_answer sAns;
static BYTE sbyAnsType[msg_header_num] = {check_msg, check_answer};
static _ping_pong sPingPong;
static BYTE sbyPingPongType[msg_header_num] = {check_msg, ping_pong};

DWORD* CalcCodeKey(DWORD* pdwCode);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNetProcess::CNetProcess()
{
	m_bSetProcess = false;
	m_pRecvBuffer = NULL;
	m_pSendBuffer = NULL;
	m_csRecv = NULL;
	m_ndKeyCheck = NULL;
	m_dwKeyCheckBufferList = NULL;
//	m_ndRealClentCheck = NULL;
}

bool CNetProcess::SetProcess(int nIndex, _NET_TYPE_PARAM* pType, CNetWorking* pNetwork)
{
	int i;
	m_nIndex = nIndex;
	memcpy(&m_Type, pType, sizeof(_NET_TYPE_PARAM));
	m_pNetwork = pNetwork;
	m_dwLoopCurTime = timeGetTime();

	if(m_Type.m_wSocketMaxNum > sock_max)
	{
		char szMsg[128];
		sprintf(szMsg, "%d > %d Over Socket Num", m_Type.m_wSocketMaxNum, sock_max);
		::MessageBox(NULL, szMsg, "Net Error", MB_OK);
		return false;
	}
	if(m_Type.m_byRecvThreadNum > recv_thread_max)
	{
		char szMsg[128];
		sprintf(szMsg, "%d > %d Max RecvThread Num", m_Type.m_byRecvThreadNum, recv_thread_max);
		::MessageBox(NULL, szMsg, "Net Error", MB_OK);
		return false;
	}

	char szLogFileName[128];
	::CreateDirectory(".\\NetLog", NULL);

	wsprintf(szLogFileName, ".\\NetLog\\%s_%s_Send.log", pNetwork->m_szSystemName, m_Type.m_szModuleName);
	m_LogFile[send_log].SetWriteLogFile(szLogFileName, pType->m_bSendLogFile);
	
	wsprintf(szLogFileName, ".\\NetLog\\%s_%s_Recv.log", pNetwork->m_szSystemName, m_Type.m_szModuleName);
	m_LogFile[recv_log].SetWriteLogFile(szLogFileName, pType->m_bRecvLogFile);
	
	wsprintf(szLogFileName, ".\\NetLog\\%s_%s_System.log", pNetwork->m_szSystemName, m_Type.m_szModuleName);
	m_LogFile[system_log].SetWriteLogFile(szLogFileName, pType->m_bSystemLogFile);

	m_listAcceptEvent.SetList(m_Type.m_wSocketMaxNum);
	m_listCloseEvent.SetList(m_Type.m_wSocketMaxNum);
	m_listRecvEvent.SetList(m_Type.m_wSocketMaxNum*100);
	m_listAcceptReady.SetList(m_Type.m_wSocketMaxNum*10);
		
	m_nEventThreadNum = m_Type.m_wSocketMaxNum/event_per_therad;
	if(m_Type.m_wSocketMaxNum%event_per_therad != 0)
		m_nEventThreadNum += 1;

	m_NetSocket.SetSocket(&m_Type);
	
	m_pRecvBuffer = new _NET_BUFFER [m_Type.m_wSocketMaxNum];
	m_pSendBuffer = new _NET_BUFFER [m_Type.m_wSocketMaxNum];

	m_csRecv = new CNetCriticalSection [m_Type.m_wSocketMaxNum];
	
	if(pType->m_bKeyCheck)
	{
		if(m_ndKeyCheck)
			return false;

		m_nKeyCheckNodeNum = pType->m_wSocketMaxNum*2;
		m_ndKeyCheck = new _KEY_CHECK_NODE [m_nKeyCheckNodeNum];
		m_dwKeyCheckBufferList = new DWORD [m_nKeyCheckNodeNum];

		m_listKeyCheck.SetList(m_nKeyCheckNodeNum);
		m_listKeyCheck_Empty.SetList(m_nKeyCheckNodeNum);
		for(int i = 0; i < m_nKeyCheckNodeNum; i++)
			m_listKeyCheck_Empty.PushNode_Back(i);

		m_tmrListCheckerKeyCheck.BeginTimer(pType->m_dwKeyCheckTerm/2);
	}

/*	if(pType->m_bRealClientCheck && pType->m_bServer)
	{
		if(m_ndRealClentCheck)
			return false;

		m_nRealClientNodeNum = pType->m_wSocketMaxNum*2;
		m_ndRealClentCheck = new _REAL_CLIENT_CHECK_NODE [m_nRealClientNodeNum];

		m_listRealClient_Send.SetList(m_nRealClientNodeNum);
		m_listRealClient_Check.SetList(m_nRealClientNodeNum);
		m_listRealClient_Empty.SetList(m_nRealClientNodeNum);
		for(int i = 0; i < m_nRealClientNodeNum; i++)
			m_listRealClient_Empty.PushNode_Back(i);

		m_tmrListCheckerRealClient.BeginTimer(_REAL_CLIENT_CHECK_NODE::send_delay_time/5);
	}*/
	
	if(m_Type.m_bServer)
	{
		m_pmAcceptThread.SetParameter(TRUE, this, 0);
		::_beginthread(AcceptThread, 0, (void*)&m_pmAcceptThread);
	}

	for(i = 0; i < m_nEventThreadNum; i++)
	{
		m_pmEventThread[i].SetParameter(TRUE, this, i);
		::_beginthread(NetEventThread, 0, (void*)&m_pmEventThread[i]);
	}

	for(i = 0; i < m_Type.m_byRecvThreadNum; i++)
	{
		m_pmRecvThread[i].SetParameter(TRUE, this, i);
		::_beginthread(RecvThread, 0, (void*)&m_pmRecvThread[i]);
	}

	if(m_Type.m_bSendThread)
	{
		m_pmSendThread.SetParameter(TRUE, this, i);
		::_beginthread(SendThread, 0, (void*)&m_pmSendThread);
	}

	m_FC.Init(m_Type.m_wSocketMaxNum*10);

	m_nOddMsgNum = 0;

	m_bSetProcess = true;
	return true;
}

CNetProcess::~CNetProcess()
{
	if(m_bSetProcess)
		Release();
}

void CNetProcess::Release()
{
	if(!m_bSetProcess)
		return;

	int i;
	
	for(i = 0; i < (int)m_Type.m_wSocketMaxNum; i++)
	{
		_socket* pSocket = m_NetSocket.GetSocket(i);
		if(pSocket->m_bAccept)
			m_NetSocket.CloseSocket(i);
	}

	if(m_Type.m_bServer)
		m_pmAcceptThread.EndThread();

	for(i = 0; i < m_nEventThreadNum; i++)
	{
		m_pmEventThread[i].EndThread();
	}

	for(i = 0; i < m_Type.m_byRecvThreadNum; i++)
	{
		m_pmRecvThread[i].EndThread();
	}

	if(m_Type.m_bSendThread)
		m_pmSendThread.EndThread();

	Sleep(1000);

	if(m_ndKeyCheck)
		delete [] m_ndKeyCheck;

	if(m_dwKeyCheckBufferList)
		delete [] m_dwKeyCheckBufferList;

//	if(m_ndRealClentCheck)
//		delete [] m_ndRealClentCheck;

	if(m_csRecv)
		delete [] m_csRecv;

	m_NetSocket.Release();

	if(m_pRecvBuffer)
	{
		delete [] m_pRecvBuffer;
		m_pRecvBuffer = NULL;
	}
	if(m_pSendBuffer)
	{
		delete [] m_pSendBuffer;
		m_pSendBuffer = NULL;
	}

	m_bSetProcess = false;
}

void CNetProcess::CloseAll()
{
	for(DWORD i = 0; i < m_Type.m_wSocketMaxNum; i++)
		PushCloseNode(i);
}

void CNetProcess::OnLoop()
{
	m_dwLoopCurTime = timeGetTime();

	m_NetSocket.OnLoop();

	_Receipt();
	_CheckWaitKey();
//	_CheckRealClient();
	_ForceCloseLoop();
	_ResponSpeedHack();

	for(WORD i = 0; i < m_Type.m_wSocketMaxNum; i++)
	{
		if(!m_NetSocket.m_Socket[i].m_bAccept)
			continue;

		_PopRecvMsg(i);

		if(m_Type.m_bRealSockCheck)// && m_NetSocket.m_Socket[i].m_bCheckRealClient)
			_CheckSend(i);

		if(!m_Type.m_bSendThread)
			_SendLoop(i);
	}
}

bool CNetProcess::wt_AcceptClient(DWORD* pdwClientIndex /* = -1*/)
{
	if(m_Type.m_bServer)
	{
		*pdwClientIndex = (int)m_NetSocket.FindEmptySocket();
		if(*pdwClientIndex == 0xFFFFFFFF)
			return false;

		if(!m_NetSocket.Accept_Server(*pdwClientIndex))
			return false;
	}
	else
	{
		if(!m_NetSocket.Accept_Client(*pdwClientIndex))
			return false;
	}

	m_pRecvBuffer[*pdwClientIndex].Init();
	m_pSendBuffer[*pdwClientIndex].Init();

	return true;
}

bool CNetProcess::wt_CloseClient(DWORD dwClientIndex)
{
	if(!m_NetSocket.m_Socket[dwClientIndex].m_bAccept)
		return false;

	m_NetSocket.CloseSocket(dwClientIndex);

	m_pRecvBuffer[dwClientIndex].Init();
	m_pSendBuffer[dwClientIndex].Init();

	return true;
}

void CNetProcess::AcceptThread(void* pv)
{
	_thread_parameter* pParam = (_thread_parameter*)pv;	//parameter를 받는다.
	BOOL* pbStart = &pParam->m_bStart;
	CNetProcess* pProcess = (CNetProcess*)pParam->m_pParam; 
	CNetSocket* pNetwork = &pProcess->m_NetSocket;
	WSAEVENT* pEvent = &pProcess->m_NetSocket.m_AcceptEvent;

	while(*pbStart)
	{		
		int nEvent = WSAWaitForMultipleEvents(1, pEvent, FALSE, WSA_INFINITE, FALSE);

		if(nEvent == WSA_WAIT_FAILED || nEvent < WSA_WAIT_EVENT_0 || nEvent >= WSA_WAIT_EVENT_0+1)
		{
			::ResetEvent(pEvent);
			continue;
		}

		WSANETWORKEVENTS NetEvents;
		if(WSAEnumNetworkEvents(pNetwork->m_sAccept, pNetwork->m_AcceptEvent, &NetEvents) != SOCKET_ERROR)
		{
			if((NetEvents.lNetworkEvents&FD_ACCEPT) && !NetEvents.iErrorCode[FD_ACCEPT_BIT])
			{
				pProcess->m_listAcceptEvent.PushNode_Back(0);
			}
		}
		::ResetEvent(pEvent);
	}

	::_endthreadex(1);
	
}

void CNetProcess::NetEventThread(void* pv)
{
	_thread_parameter* pParam = (_thread_parameter*)pv;
	int n = pParam->m_nIndex;
	BOOL* pbStart = &pParam->m_bStart;
	CNetProcess* pProcess = (CNetProcess*)pParam->m_pParam; 
	WSAEVENT* pEvent = pProcess->m_NetSocket.m_Event+(n*event_per_therad);
	CNetSocket* pNetwork = &pProcess->m_NetSocket;

	int nEventNum = event_per_therad;	//WSAWaitForMultipleEvents()이 처리할 event수

	//해당 thread가 마지막번째라면 event_per_therad개보다 작을수 있으므로
	if(n*event_per_therad+event_per_therad > (int)pProcess->m_Type.m_wSocketMaxNum)
		nEventNum = pProcess->m_Type.m_wSocketMaxNum%event_per_therad;	//나머지를 처리할 event수로 정한다.

	while(*pbStart)
	{
		int nEvent = WSAWaitForMultipleEvents(nEventNum, pEvent, FALSE, WSA_INFINITE, FALSE);

		if(nEvent == WSA_WAIT_FAILED || nEvent < WSA_WAIT_EVENT_0 || nEvent >= WSA_WAIT_EVENT_0+event_per_therad)
		{
			::ResetEvent(pEvent[nEvent]);
			continue;
		}

		int nIndex = nEvent+(n*event_per_therad); //발생한 event 번호와 해당 thread의 index를 이용하여 socket index를 얻는다.
	
		WSANETWORKEVENTS NetEvents;
		if(WSAEnumNetworkEvents(pNetwork->m_Socket[nIndex].m_Socket, pEvent[nEvent], &NetEvents) != SOCKET_ERROR)
		{
			if((NetEvents.lNetworkEvents&FD_CONNECT) && !NetEvents.iErrorCode[FD_CONNECT_BIT])
			{
				pProcess->m_listAcceptEvent.PushNode_Back(nIndex);			
			}
			else if((NetEvents.lNetworkEvents&FD_READ) && !NetEvents.iErrorCode[FD_READ_BIT])
			{
				pProcess->m_listRecvEvent.PushNode_Back(nIndex);			
			}
			else if((NetEvents.lNetworkEvents&FD_CLOSE) && !NetEvents.iErrorCode[FD_CLOSE_BIT])
			{
				pProcess->PushCloseNode(nIndex);
			}
		}
		::ResetEvent(pEvent[nEvent]);
	}

	::_endthreadex(22);

}

void CNetProcess::RecvThread(void* pv)
{
	_thread_parameter* pParam = (_thread_parameter*)pv;
	BOOL* pbStart = &pParam->m_bStart;
	CNetProcess* pProcess = (CNetProcess*)pParam->m_pParam; 
	CNetSocket* pNetSocket = &pProcess->m_NetSocket;
	DWORD dwSocketIndex;
	int nRecvSize;
	DWORD dwMaxLeftSize;
	int nNorLeftSize;
	_NET_BUFFER* pNB = NULL;
	_socket* pSocket = NULL;
	int nSleepTimer = 0;
	
	while(*pbStart)
	{
		while(pProcess->m_listRecvEvent.PopNode_Front(&dwSocketIndex))
		{	
			pProcess->m_csRecv[dwSocketIndex].Lock();

			pNB = &pProcess->m_pRecvBuffer[dwSocketIndex];
			pSocket = pNetSocket->GetSocket(dwSocketIndex);

			//먼저 버퍼를 읽음
			dwMaxLeftSize = MAX_NET_BUFFER + MAX_EXTRA_BUFFER - pNB->m_dwPushPnt;
			if(!pNetSocket->Recv(dwSocketIndex, pNB->GetPushPos(), dwMaxLeftSize, &nRecvSize))
			{
				pProcess->m_csRecv[dwSocketIndex].Unlock();

				if(nRecvSize != WSAEWOULDBLOCK)	//읽기 블럭인경우에 다음에 읽을수있도록 
				{
					pProcess->PushCloseNode(dwSocketIndex);
					if(pProcess->m_Type.m_bSvrToS)
						pProcess->m_LogFile[system_log].Write("recv(%d).. 오류.. code(%d)", dwSocketIndex, nRecvSize);
				}

				continue;
			}

			//dwBufferSize와 현재 GetTotalWrite() 합이 GetTotalRead()보다 크면 실패.. 소켓 클로즈
//			if(pNB->GetTotalPush() - pNB->GetTotalPop() + nRecvSize > MAX_NET_BUFFER)
			if(pNB->GetLeftLoadSize() + nRecvSize > MAX_NET_BUFFER)
			{
				pProcess->m_csRecv[dwSocketIndex].Unlock();

				pProcess->m_LogFile[system_log].Write("%d) RecvThread(%d).. 리스트공간미확보..)", pProcess->m_nIndex, dwSocketIndex);
				pProcess->PushCloseNode(dwSocketIndex);	//읽을수있는 공간 확보돼지않으면 강제종료 (메시지 처리부분의속도가 받는부분보다 느린경우)
				continue;
			}
			
			//MAX_NET_BUFFER의 범위를 벗어났는지..
			nNorLeftSize = MAX_NET_BUFFER - pNB->m_dwPushPnt;
			if(nNorLeftSize < nRecvSize)	//벗어나서 받은거라면 여분의 뒷부분을 잘라서 앞으로 꽂는다
			{
				memcpy(pNB->m_sMainBuffer, &pNB->m_sMainBuffer[MAX_NET_BUFFER], nRecvSize-nNorLeftSize);
			}

			pNB->AddPushPos(nRecvSize);

			pProcess->m_csRecv[dwSocketIndex].Unlock();
			Sleep(0);
		}
		if(++nSleepTimer > pProcess->m_Type.m_byRecvSleepTime)
		{
			Sleep(1);
			nSleepTimer = 0;
		}
	}

	_endthreadex(0);
}

void CNetProcess::_PopRecvMsg(WORD wSocketIndex)
{
	bool bProcessSucc = true;
	bool bMiss;

	_socket* pSocket = m_NetSocket.GetSocket(wSocketIndex);
	_NET_BUFFER* pNB = &m_pRecvBuffer[wSocketIndex];
	DWORD dwAnalysisCount = 0;

	while(1)
	{
		bMiss = false;
		char* pMsg = pNB->GetPopPoint(&bMiss);

		if(!pMsg)
		{
			if(bMiss)	//메시지가 완전히 로딩이 안됀경우
			{
				if(pSocket->m_dwRecvPopMissTime == 0)//처음걸린경우
				{
					pSocket->m_dwRecvPopMissTime = timeGetTime();
				}
				else
				{
					if(timeGetTime() - pSocket->m_dwRecvPopMissTime > 5000)//5초이상
					{
						PushCloseNode(wSocketIndex);
						m_LogFile[system_log].Write("Socket(%d): _PopRecvMsg : bMiss = close", wSocketIndex);
					}					
				}
			}
			break;
		}
		else
		{
			if(pSocket->m_dwRecvPopMissTime != 0)
				pSocket->m_dwRecvPopMissTime = 0;
		}

		_MSG_HEADER* pHeader = (_MSG_HEADER*)pMsg;

	/*	if(pHeader->m_byType[0] == real_msg)
		{
			if(!pSocket->m_bCheckRealClient)//
				bProcessSucc = _RealClientPacketProcess(wSocketIndex, pHeader, &pMsg[MSG_HEADER_SIZE]);
			else
			{
				PushCloseNode(wSocketIndex);
				m_LogFile[system_log].Write("chk real.. 2nd ans: ip: %s", inet_ntoa(pSocket->m_Addr.sin_addr));
				break;
			}
		}
		else*/
		{
	//		if(pSocket->m_bCheckRealClient)	
			{//real_msg 이외에는 m_bCheckRealClient 언책된상태에서만 가능
				if(pHeader->m_byType[0] > 100)
				{
					bProcessSucc = _InternalPacketProcess(wSocketIndex, pHeader, &pMsg[MSG_HEADER_SIZE]);
				}
				else 
				{
					bProcessSucc = m_pNetwork->DataAnalysis(m_nIndex, wSocketIndex, pHeader, &pMsg[MSG_HEADER_SIZE]);
				}
			}
	/*		else
			{
				PushCloseNode(wSocketIndex);
				m_LogFile[system_log].Write("chk real.. pass ans: ip: %s", inet_ntoa(pSocket->m_Addr.sin_addr));
				break;
			}*/
		}

		if(!bProcessSucc)
		{
			if(m_Type.m_bOddMsgWriteLog)
			{
				m_nOddMsgNum++;
				m_LogFile[system_log].Write("id(%d)->OddMsg(%d, %d) : %dth ", pSocket->m_dwClientInfo, pHeader->m_byType[0], pHeader->m_byType[1], m_nOddMsgNum);
			}
			if(m_Type.m_bOddMsgDisconnect)
			{
				PushCloseNode(wSocketIndex);	//종료시킨다...
				pNB->Init();
				break;
			}
		}
		
		//읽은 포인트 증가
		pNB->AddPopPos(pHeader->m_wSize);

		//서버는 소켓당 한루프에 일정수이하로 처리한다
		if(++dwAnalysisCount > m_Type.m_dwProcessMsgNumPerLoop)
			break;
	}
}

void CNetProcess::SendThread(void* pv)
{
	_thread_parameter* pParam = (_thread_parameter*)pv;
	BOOL* pbStart = &pParam->m_bStart;
	CNetProcess* pProcess = (CNetProcess*)pParam->m_pParam; 
	CNetSocket* pNetSocket = &pProcess->m_NetSocket;
	DWORD i;
	int nSleepTimer = 0;
	
	while(*pbStart)
	{
		pProcess->m_fpsSendThread.CalcFrameRate();

		for(i = 0; i < pProcess->m_Type.m_wSocketMaxNum; i++)
		{
			if(!pNetSocket->GetSocket(i)->m_bAccept)
				continue;

			if(!pNetSocket->GetSocket(i)->m_bSendable)
			{
				if(timeGetTime() - pNetSocket->GetSocket(i)->m_dwSendBlockTime > 1000)//블럭수 1초이상 지났는지..
					pNetSocket->GetSocket(i)->m_bSendable = true;
				else
					continue;
			}

			pProcess->_SendLoop(i);

			if(i%100 ==0)
				Sleep(0);
		}
		if(++nSleepTimer > pProcess->m_Type.m_bySendSleepTime)
		{
			Sleep(1);
			nSleepTimer = 0;
		}
	}
	_endthreadex(0);
}

void CNetProcess::_SendLoop(DWORD n)
{
	_NET_BUFFER* pNB = &m_pSendBuffer[n];
	_socket* pSocket = m_NetSocket.GetSocket(n);
	
	int nSendSize;
	bool bMiss = false;
	char* pszSendMsg = pNB->GetSendPoint(&nSendSize, &bMiss);
	if(!pszSendMsg)
	{
		if(bMiss)
		{
			m_LogFile[system_log].Write("GetSendPoint(id:%d).. 오류..소켓닫기", pSocket->m_dwClientInfo);					
			PushCloseNode(n);
		}
		if(nSendSize < 0)
			m_LogFile[system_log].Write("_SendLoop: GetSendPoint(%d).. 오류..nSendSize(%d) < 0 .. 	PopRot: %d, PopPnt: %d, PushRot: %d, PushPnt: %d",  
			n, nSendSize, pNB->m_dwPopRot, pNB->m_dwPopPnt, pNB->m_dwPushRot, pNB->m_dwPushPnt);					
		return;
	}

	int nRet;
	if(!m_NetSocket.Send(n, pszSendMsg, nSendSize, &nRet))
	{
		if(nRet == WSAEWOULDBLOCK)
		{
			pSocket->m_bSendable = false;
			pSocket->m_dwSendBlockTime = timeGetTime();
		}
		else
		{
	//		if(nRet != WSAECONNRESET && nRet != WSAECONNABORTED && nRet != WSAESHUTDOWN)
	//			m_LogFile[system_log].Write("_SendLoop: send().. 오류.. code: %d, account: %d, size: %d", nRet, pSocket->m_dwClientInfo, nSendSize);					
			PushCloseNode(n);
		}
	}
	else
	{
		if(nRet > 0)
			pNB->AddPopPos(nRet);
		else
			m_LogFile[system_log].Write("_SendLoop: SendSize(%d).. 오류..", n, nRet);
	}
}

BOOL CNetProcess::LoadSendMsg(DWORD dwClientIndex, BYTE* pbyType, char* szMsg, WORD nLen)
{
	_socket* pSocket = m_NetSocket.GetSocket(dwClientIndex);
	if(!pSocket)
		return FALSE;

	if(!pSocket->m_bAccept)
		return FALSE;

/*	if(!pSocket->m_bCheckRealClient)
	{
		if(pbyType[0] != real_msg)
			return FALSE;
	}*/

	if(nLen > MAX_NET_BUFFER - MSG_HEADER_SIZE)
	{
		m_LogFile[system_log].Write("LoadSendMsg(id:%d).. 초과패킷사이즈.. 헤더: %d-%d , 사이즈: %d", pSocket->m_dwClientInfo, pbyType[0], pbyType[1], nLen);
		return FALSE;
	}

	_NET_BUFFER* pNB = &m_pSendBuffer[dwClientIndex];

	_MSG_HEADER Header;

	*(WORD*)Header.m_byType = *(WORD*)pbyType;
	Header.m_wSize			= nLen + MSG_HEADER_SIZE;

//	if(pNB->GetTotalPush() - pNB->GetTotalPop() + Header.m_wSize > MAX_NET_BUFFER)
	if(pNB->GetLeftLoadSize() + Header.m_wSize > MAX_NET_BUFFER)
	{
		if(!m_Type.m_bSvrToS)
			PushCloseNode(dwClientIndex);
	//	m_LogFile[system_log].Write("LoadSendMsg(id:%d).. 리스트공간미확보.. 추가사이즈: %d", pSocket->m_dwClientInfo, Header.m_wSize);
		return FALSE;
	}

	memcpy(&pNB->m_sMainBuffer[pNB->m_dwPushPnt], &Header, MSG_HEADER_SIZE);		//헤더카피
	if(nLen > 0)
		memcpy(&pNB->m_sMainBuffer[pNB->m_dwPushPnt + MSG_HEADER_SIZE], szMsg, nLen);	//내용카피
	
	//MAX_NET_BUFFER의 범위를 벗어났는지..
	int nNorLeftSize = MAX_NET_BUFFER - pNB->m_dwPushPnt;
	if(nNorLeftSize < Header.m_wSize)	//벗어나서 받은거라면 여분의 뒷부분을 잘라서 앞으로 꽂는다
	{
		memcpy(pNB->m_sMainBuffer, &pNB->m_sMainBuffer[MAX_NET_BUFFER], Header.m_wSize - nNorLeftSize);
	}

	pNB->AddPushPos(Header.m_wSize);
	return TRUE;
}

BOOL CNetProcess::LoadSendMsg(DWORD dwClientIndex, WORD wType, char* szMsg, WORD nLen)
{
	return CNetProcess::LoadSendMsg(dwClientIndex, (BYTE*)&wType, szMsg, nLen);
}

bool CNetProcess::StartSpeedHackCheck(DWORD dwClientIndex, DWORD dwClientInfo)
{
	if(!m_Type.m_bSpeedHackCheck)
		return false;

	_socket* pSocket = m_NetSocket.GetSocket(dwClientIndex);

	if(!pSocket)
		return false;

	pSocket->m_dwClientInfo = dwClientInfo;
	pSocket->m_dwSendSpeedHackTime = timeGetTime();

	_check_speed_hack_ask Send;

	BYTE byType[msg_header_num] = {secure_msg, check_speed_hack_ask};
	if(!LoadSendMsg(dwClientIndex, byType, (char*)&Send, sizeof(Send)))
		return false;

	return true;
}
/*
bool CNetProcess::PushCheckRealClient(DWORD dwSocketIndex, DWORD dwSocketSerial)
{//일정시간후에 _SendMsg_RealClientCheck 를 실행시키기위해..
	if(!m_Type.m_bRealClientCheck)
		return false;

	DWORD dwEmptyIndex;
	if(!m_listRealClient_Empty.PopNode_Front(&dwEmptyIndex))
		return false;

	m_ndRealClentCheck[dwEmptyIndex].dwSocketIndex = dwSocketIndex;
	m_ndRealClentCheck[dwEmptyIndex].dwSocketSerial = dwSocketSerial;
	m_ndRealClentCheck[dwEmptyIndex].dwAcceptTime = m_dwLoopCurTime;

	return m_listRealClient_Send.PushNode_Back(dwEmptyIndex);
}*/

void CNetProcess::_CheckSend(WORD wSocketIndex)
{
	_socket* pSocket = &m_NetSocket.m_Socket[wSocketIndex];
/*
	if((int)(m_dwLoopCurTime - pSocket->m_dwLastAnswerTime) > (int)pSocket->m_dwBoundResponTerm)
	{
		PushCloseNode(wSocketIndex);	//BOUNDING TIME OUT..
		m_LogFile[system_log].Write("Socket(%d) : Bound Time Out ", wSocketIndex);
		return;
	}
*/
	if((int)(m_dwLoopCurTime - pSocket->m_dwLastCheckSendTime) > (int)m_Type.m_dwRealSendCheckTerm)	
	{
		LoadSendMsg(wSocketIndex, sbyQryType, (char*)&sQry, sQry.size());
		pSocket->m_dwLastCheckSendTime = m_dwLoopCurTime;
	}
}

bool CNetProcess::PushKeyCheckList(DWORD dwSerial, DWORD dwIP, DWORD* pdwKey, int nUseKeyNum)
{
	if(!m_Type.m_bKeyCheck)
		return false;

	if(nUseKeyNum > CHECK_KEY_NUM)
		return false;

	DWORD dwEmptyIndex;
	if(!m_listKeyCheck_Empty.PopNode_Front(&dwEmptyIndex))
		return false;

	m_ndKeyCheck[dwEmptyIndex].dwSerial = dwSerial;
	memcpy(m_ndKeyCheck[dwEmptyIndex].dwKey, pdwKey, sizeof(DWORD)*nUseKeyNum);
	m_ndKeyCheck[dwEmptyIndex].dwIP = dwIP;
	m_ndKeyCheck[dwEmptyIndex].dwWaitStartTime = m_dwLoopCurTime;

	return m_listKeyCheck.PushNode_Back(dwEmptyIndex);
}

bool CNetProcess::FindKeyFromWaitList(DWORD dwSocketIndex, DWORD dwSerial, DWORD* pdwKey, int nUseKeyNum)
{
	if(!m_Type.m_bKeyCheck)
		return false;

	if(nUseKeyNum > CHECK_KEY_NUM)
		return false;

	_socket* pSocket = m_NetSocket.GetSocket(dwSocketIndex);
	if(!pSocket)
		return false;

	DWORD dwIP = (DWORD)pSocket->m_Addr.sin_addr.S_un.S_addr;

	int nFindIndex = -1;
	int nListNum = m_listKeyCheck.CopyIndexList(m_dwKeyCheckBufferList, m_nKeyCheckNodeNum);
	for(int i = 0; i < nListNum; i++)
	{
		_KEY_CHECK_NODE* pNode = &m_ndKeyCheck[m_dwKeyCheckBufferList[i]];

		bool bEqual = true;
		if(pNode->dwIP == dwIP && pNode->dwSerial == dwSerial)
		{
			for(int k = 0; k < nUseKeyNum; k++)
			{
				if(pNode->dwKey[k] != pdwKey[k])
				{
					bEqual = false;
					break;
				}
			}
			if(bEqual)
			{
				nFindIndex = i;
				break;
			}
		}
	}

	if(nFindIndex != -1)
	{
		m_listKeyCheck.FindNode(m_dwKeyCheckBufferList[nFindIndex]);
		m_listKeyCheck_Empty.PushNode_Back(m_dwKeyCheckBufferList[nFindIndex]);
		return true;
	}

	return false;
}

DWORD CNetProcess::GetSendThreadFrame()
{
	return m_fpsSendThread.m_dwFrames;
}

void CNetProcess::PushCloseNode(int nIndex)
{
	_socket* p = m_NetSocket.GetSocket(nIndex);
	if(!p->m_bAccept)
		return;

	if(m_listCloseEvent.IsInList(nIndex))
		return;

	m_listCloseEvent.PushNode_Back(nIndex);
}

void CNetProcess::LogFileOperSetting(bool bRecv, bool bSend, bool bSystem)
{
	m_LogFile[recv_log].SetWriteAble(bRecv);
	m_LogFile[send_log].SetWriteAble(bSend);
	m_LogFile[system_log].SetWriteAble(bSystem);
}

void CNetProcess::IOLogFileOperSetting(bool bOper)
{
	m_LogFile[send_log].SetWriteAble(bOper);
	m_LogFile[recv_log].SetWriteAble(bOper);
}

void CNetProcess::_Receipt()
{
	DWORD dwIndex;

	while(m_listAcceptEvent.PopNode_Front(&dwIndex))
	{
		if(wt_AcceptClient(&dwIndex))
		{
			if(m_Type.m_bServer)
			{
				_socket* pSocket = m_NetSocket.GetSocket(dwIndex);

/*				if(m_Type.m_bRealClientCheck)
				{	//real check 옵션인경우.. real check 된후에 
					//pSocket->m_bCheckRealClient = true로 언책함..
					PushCheckRealClient(dwIndex, pSocket->m_dwSerial);
				}
				else*/
				{	//real check 옵션이 아닌경우.. 
					//즉시 pSocket->m_bCheckRealClient = true로 언책함..					
//					pSocket->m_bCheckRealClient = true;
					m_pNetwork->AcceptClientCheck(m_nIndex, dwIndex, pSocket->m_dwSerial);
				}
			}
			else
			{
//				if(!m_Type.m_bRealClientCheck)
				{
					_socket* pSocket = m_NetSocket.GetSocket(dwIndex);

		//			pSocket->m_bCheckRealClient = true;
					m_listAcceptReady.PushNode_Back(dwIndex);
				}
			}
		}
	}
	while(m_listCloseEvent.PopNode_Front(&dwIndex))
	{
		if(wt_CloseClient(dwIndex))
		{
			_socket* pSocket = m_NetSocket.GetSocket(dwIndex);
			m_pNetwork->CloseClientCheck(m_nIndex, dwIndex, pSocket->m_dwSerial);
		}
	}

	DWORD dwCurTime = timeGetTime();
	while(m_listAcceptReady.CopyFront(&dwIndex))
	{
		_socket* pSocket = m_NetSocket.GetSocket(dwIndex);

		if(dwCurTime - pSocket->m_dwConnectTime < 1000)		//1초후에 사용가능토록..
			break;
		
		if(pSocket->m_bAccept)
			m_pNetwork->AcceptClientCheck(m_nIndex, dwIndex, pSocket->m_dwSerial);

		m_listAcceptReady.PopNode_Front(&dwIndex);
	}
}

bool CNetProcess::_InternalPacketProcess(DWORD dwSocketIndex, _MSG_HEADER* pMsgHeader, char* pMsg)
{
	if(pMsgHeader->m_byType[0] == check_msg)
	{
		if(pMsgHeader->m_byType[1] == check_query)
		{
	//		LoadSendMsg(dwSocketIndex, sbyAnsType, (char*)&sAns, sAns.size());
			return true;
		}
		else if(pMsgHeader->m_byType[1] == check_answer)
		{
			_socket* pSocket = m_NetSocket.GetSocket(dwSocketIndex);
			pSocket->m_dwLastCheckRecvTime = m_dwLoopCurTime;
			return true;
		}
		else if(pMsgHeader->m_byType[1] == ping_pong)
		{
		//	_socket* pSocket = m_NetSocket.GetSocket(dwSocketIndex);
		//	pSocket->m_dwPingPongCount++;

		//	LoadSendMsg(dwSocketIndex, sbyPingPongType, (char*)&sPingPong, sPingPong.size());
		//	return true;
		}
		
		return false;
	}

	if(pMsgHeader->m_byType[0] == secure_msg)
	{
		if(pMsgHeader->m_byType[1] == respon_free_request)
		{
			return true;
/*
			_respon_free_request* pRecv = (_respon_free_request*)pMsg;

			for(DWORD i = 0; i < 4; i++)
			{
				if(pRecv->dwDeveloperKey[i] != i)
				{
					m_NetSocket.GetSocket(dwSocketIndex)->m_dwBoundResponTerm = 0x70000000;
					break;
				}
			}
			return true;
*/
		}
		if(pMsgHeader->m_byType[1] == check_speed_hack_ask)
		{
			_socket* pSocket = m_NetSocket.GetSocket(dwSocketIndex);
			pSocket->m_dwResponSpeedHackTime = timeGetTime();

			return true;
		}
		if(pMsgHeader->m_byType[1] == check_speed_hack_ans)
		{
			_socket* pSocket = m_NetSocket.GetSocket(dwSocketIndex);

			if(pSocket)
			{
				DWORD dwTime = timeGetTime() - pSocket->m_dwSendSpeedHackTime;
				if(dwTime < _check_speed_hack_ask::ans_term-100)//정해진시간보다 빨리옴.
				{		
					pSocket->m_bySpeedHackContCount++;
					if(pSocket->m_bySpeedHackContCount > 1)	//1번까지는 봐준다..
					{
						m_LogFile[system_log].Write("speedhack.. %d (time:%d)", pSocket->m_dwClientInfo, dwTime);
						PushCloseNode(dwSocketIndex);
					}
					else
					{
						_check_speed_hack_ask Send;

						BYTE byType[msg_header_num] = {secure_msg, check_speed_hack_ask};
						if(LoadSendMsg(dwSocketIndex, byType, (char*)&Send, sizeof(Send)))
							pSocket->m_dwSendSpeedHackTime = timeGetTime();
					}
				}
				else
				{
					pSocket->m_bySpeedHackContCount = 0;

					_check_speed_hack_ask Send;

					BYTE byType[msg_header_num] = {secure_msg, check_speed_hack_ask};
					if(LoadSendMsg(dwSocketIndex, byType, (char*)&Send, sizeof(Send)))
						pSocket->m_dwSendSpeedHackTime = timeGetTime();
				}
			}
			return true;
		}

		return false;
	}	

	return false;
}
/*
bool CNetProcess::_RealClientPacketProcess(DWORD dwSocketIndex, _MSG_HEADER* pMsgHeader, char* pMsg)
{
	if(pMsgHeader->m_byType[0] == real_msg)
	{
		if(pMsgHeader->m_byType[1] == real_client_ask)
		{//클라이언트..
			//암호화해서 서버로 보낸다..
			if(!m_Type.m_bServer)
			{
				_real_client_ask* pRecv = (_real_client_ask*)pMsg;

				_real_client_ans Send;

				memcpy(Send.dwResponKey, ::CalcCodeKey(pRecv->dwKey), sizeof(DWORD)*CHECK_KEY_NUM);
				
				BYTE byType[msg_header_num] = {real_msg, real_client_ans};
				LoadSendMsg(dwSocketIndex, byType, (char*)&Send, sizeof(Send));
			}
			else
				return false;

			return true;
		}
		else if(pMsgHeader->m_byType[1] == real_client_ans)
		{//서버..
			//암호가 맞는건지 확인..
			if(m_Type.m_bServer)
			{
				_real_client_ans* pRecv = (_real_client_ans*)pMsg;

				_socket* pSocket = m_NetSocket.GetSocket(dwSocketIndex);

				for(int i = 0; i < CHECK_KEY_NUM; i++)
				{
					if(pSocket->m_dwRealClientKeyValue[i] != pRecv->dwResponKey[i])
						return false;
				}

				pSocket->m_bCheckRealClient = true;

				//맞는거라면..	
				_real_client_complete Send;

				BYTE byType[msg_header_num] = {real_msg, real_client_complete}; 
				LoadSendMsg(dwSocketIndex, byType, (char*)&Send, sizeof(Send));		

				m_pNetwork->AcceptClientCheck(m_nIndex, dwSocketIndex, pSocket->m_dwSerial);
			}
			else
				return false;
				

			return true;
		}
		else if(pMsgHeader->m_byType[1] == real_client_complete)
		{//클라이언트
			if(!m_Type.m_bServer)
			{
				_socket* pSocket = m_NetSocket.GetSocket(dwSocketIndex);

				pSocket->m_bCheckRealClient = true;
				m_pNetwork->AcceptClientCheck(m_nIndex, dwSocketIndex, pSocket->m_dwSerial);
			}
			else
				return false;

			return true;
		}
		
		return false;
	}

	return false;
}*/

void CNetProcess::_CheckWaitKey()
{
	if(!m_Type.m_bKeyCheck)
		return;

	if(m_tmrListCheckerKeyCheck.CountingTimer())
	{
		DWORD dwNodeIndex;
		while(m_listKeyCheck.CopyFront(&dwNodeIndex))
		{
			_KEY_CHECK_NODE* pNode = &m_ndKeyCheck[dwNodeIndex];

			if((int)(m_dwLoopCurTime - pNode->dwWaitStartTime) > (int)m_Type.m_dwKeyCheckTerm)
			{
				m_listKeyCheck.PopNode_Front(&dwNodeIndex);
				m_listKeyCheck_Empty.PushNode_Back(dwNodeIndex);
			}
			else
				break;
		}
	}
}
/*
void CNetProcess::_CheckRealClient()
{
	if(!m_Type.m_bServer || !m_Type.m_bRealClientCheck)
		return;

	if(m_tmrListCheckerRealClient.CountingTimer())
	{
		DWORD dwNodeIndex;
		while(m_listRealClient_Send.CopyFront(&dwNodeIndex))
		{
			_REAL_CLIENT_CHECK_NODE* p = &m_ndRealClentCheck[dwNodeIndex];

			if((int)(m_dwLoopCurTime - p->dwAcceptTime) > _REAL_CLIENT_CHECK_NODE::send_delay_time)
			{
				_socket* pSocket = m_NetSocket.GetSocket(p->dwSocketIndex);
				if(pSocket->m_bAccept && pSocket->m_dwSerial == p->dwSocketSerial)
				{
					_SendMsg_RealClientCheck(p->dwSocketIndex);	//1초가 지난 소
					m_listRealClient_Check.PushNode_Back(dwNodeIndex);
				}
				else
				{
					m_listRealClient_Empty.PushNode_Back(dwNodeIndex);
				}

				m_listRealClient_Send.PopNode_Front(&dwNodeIndex);				
			}
			else
				break;
		}

		while(m_listRealClient_Check.CopyFront(&dwNodeIndex))
		{
			_REAL_CLIENT_CHECK_NODE* p = &m_ndRealClentCheck[dwNodeIndex];

			if((int)(m_dwLoopCurTime - p->dwAcceptTime) > _REAL_CLIENT_CHECK_NODE::check_recv_time)
			{
				_socket* pSocket = m_NetSocket.GetSocket(p->dwSocketIndex);
				if(pSocket->m_bAccept && pSocket->m_dwSerial == p->dwSocketSerial)
				{
					if(!pSocket->m_bCheckRealClient)	//3초가 넘었는데 체크가 안됀 소켓..
					{
						PushCloseNode(p->dwSocketIndex);
						m_LogFile[system_log].Write("chk real.. no ans: ip: %s", inet_ntoa(pSocket->m_Addr.sin_addr));
					}
				}
				m_listRealClient_Check.PopNode_Front(&dwNodeIndex);				
				m_listRealClient_Empty.PushNode_Back(dwNodeIndex);
			}
			else
				break;
		}
	}
}*/

void CNetProcess::_ForceCloseLoop()
{
	if(m_FC.m_tmFD.CountingTimer())
	{
		DWORD dwIndex;
		while(1)
		{
			if(m_FC.m_listFD.CopyFront(&dwIndex))
			{
				_FORCE_CLOSE::__FD_NODE* pNode = &m_FC.m_pFDData[dwIndex];
				if((int)(m_dwLoopCurTime - pNode->dwEventCreateTime) > (int)_FORCE_CLOSE::timer_fd_term)
				{
					_socket* pSocket = m_NetSocket.GetSocket(pNode->dwObjIndex);
					if(pNode->dwObjSerial == pSocket->m_dwSerial)
					{
						PushCloseNode(pNode->dwObjIndex);
					}
					m_FC.m_listFD.PopNode_Front(&dwIndex);
					m_FC.m_listFDEmpty.PushNode_Back(dwIndex);
				}
				else
					break;					
			}
			else 
				break;
		}
	}
}

void CNetProcess::_ResponSpeedHack()
{
	if(m_Type.m_bSpeedHackCheck && !m_Type.m_bServer) 
	{
		DWORD dwCurTime = timeGetTime();

		for(int i = 0; i < m_Type.m_wSocketMaxNum; i++)
		{
			_socket* pSocket = m_NetSocket.GetSocket(i);

			if(pSocket->m_dwResponSpeedHackTime != 0)
			{
				if(dwCurTime - pSocket->m_dwResponSpeedHackTime > _check_speed_hack_ask::ans_term)
				{
					_check_speed_hack_ans Send;

					BYTE byType[msg_header_num] = {secure_msg, check_speed_hack_ans};
					LoadSendMsg(i, byType, (char*)&Send, sizeof(Send));

					pSocket->m_dwResponSpeedHackTime = 0;
				}
			}
		}
	}
}
/*
void CNetProcess::_SendMsg_RealClientCheck(DWORD dwSocketIndex)
{
	_socket* pSocket = m_NetSocket.GetSocket(dwSocketIndex);

	DWORD dwKey[CHECK_KEY_NUM];
	for(int i = 0; i < CHECK_KEY_NUM; i++)
	{
		DWORD dwR = rand();
		dwKey[i] = (dwR<<16) + rand();
	}

	memcpy(pSocket->m_dwRealClientKeyValue, ::CalcCodeKey(dwKey), sizeof(DWORD)*CHECK_KEY_NUM);

	_real_client_ask Send;

	memcpy(Send.dwKey, dwKey, sizeof(DWORD)*CHECK_KEY_NUM);
	
	BYTE byType[msg_header_num] = {real_msg, real_client_ask};
	LoadSendMsg(dwSocketIndex, byType, (char*)&Send, sizeof(Send));
}*/

static DWORD s_dwKey = 0x12AB34CC;
static DWORD s_KeyBuffer[CHECK_KEY_NUM];
DWORD* CalcCodeKey(DWORD* pdwCode)
{
	DWORD dw0 = pdwCode[0] + pdwCode[2];
	DWORD dw1 = pdwCode[1] + pdwCode[3];
	DWORD dw2 = dw0 - dw1;

	for(int i = 0; i < CHECK_KEY_NUM; i++)
		s_KeyBuffer[i] = pdwCode[i]^(s_dwKey*dw2);

	return s_KeyBuffer;
}

