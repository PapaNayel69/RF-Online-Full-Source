// Network.cpp: implementation of the CNetwork class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "NetSocket.h"
#include <mmsystem.h>
#include <wsipx.h>
#include "NetUtil.h"
#include <process.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNetSocket::CNetSocket()
{
	m_dwSerialCounter = 0;
	m_bSetSocket = false;
}

CNetSocket::~CNetSocket()
{
	if(m_bSetSocket)
		Release();
}

bool CNetSocket::SetSocket(_SOCK_TYPE_PARAM* pType)
{
	memcpy(&m_SockType, pType, sizeof(_SOCK_TYPE_PARAM));

	for(DWORD i = 0; i < m_SockType.m_wSocketMaxNum; i++)
		m_Event[i] = WSACreateEvent();

	if(pType->m_bAcceptIPCheck)
	{
		m_nIPCheckNodeNum = pType->m_wSocketMaxNum * 2;

		m_listIPCheck.SetList(m_nIPCheckNodeNum);
		m_listIPCheck_Empty.SetList(m_nIPCheckNodeNum);
		for(int i = 0; i < m_nIPCheckNodeNum; i++)
			m_listIPCheck_Empty.PushNode_Back(i);

		m_tmrListCheckerIPCheck.BeginTimer(pType->m_dwIPCheckTerm/2);
	}

	if(m_SockType.m_bServer)
	{
		if(!InitAcceptSocket())
			return false;
	}

	m_bSetSocket = true;
	return true;
}

void CNetSocket::Release()
{
	if(!m_bSetSocket)
		return;

	CloseAll();

	if(m_SockType.m_bServer)
		WSACloseEvent(m_AcceptEvent);

	for(DWORD i = 0; i < m_SockType.m_wSocketMaxNum; i++)
		WSACloseEvent(m_Event[i]);

	m_bSetSocket = false;
}

void CNetSocket::OnLoop()
{	
	if(m_tmrListCheckerIPCheck.CountingTimer())
	{
		DWORD dwCurTime = timeGetTime();
		DWORD dwNodeIndex;
		while(m_listIPCheck.CopyFront(&dwNodeIndex))
		{
			_IP_CHECK_NODE* pNode = &m_ndIPCheck[dwNodeIndex];

			if(dwCurTime - pNode->dwWaitStartTime > m_SockType.m_dwIPCheckTerm)
			{
				m_listIPCheck.PopNode_Front(&dwNodeIndex);
				m_listIPCheck_Empty.PushNode_Back(dwNodeIndex);
			}
			else
				break;
		}
	}
}
/*
bool CNetSocket::Accept_Server(DWORD dwSocketIndex)
{
	SOCKET		Socket;
	sockaddr_in	Addr;	
	int len = sizeof(sockaddr);

	Socket = accept(m_sAccept, (sockaddr*)&Addr, &len);
	if(Socket == INVALID_SOCKET)
		return false;

	//ip체크..
	if(m_SockType.m_bAcceptIPCheck)
	{
		int nFindIndex = -1;

		m_listIPCheck.m_csList.Lock();
		CNetIndexList::_index_node* pNode = m_listIPCheck.m_Head.m_pNext;
		while(pNode != &m_listIPCheck.m_Tail)
		{
			int nListIndex = pNode->m_dwIndex;
			_IP_CHECK_NODE* p = &m_ndIPCheck[nListIndex];

			if(p->dwIP == (DWORD)Addr.sin_addr.S_un.S_addr)
			{
				nFindIndex = nListIndex;
				break;
			}

			pNode = pNode->m_pNext;
		}
		m_listIPCheck.m_csList.Unlock();

		if(nFindIndex != -1)
		{		
			m_listIPCheck.FindNode(nFindIndex);
			m_listIPCheck_Empty.PushNode_Back(nFindIndex);
		}
		else
		{
			closesocket(Socket);
			return false;
		}
	}

	if(WSAEventSelect(Socket, m_Event[dwSocketIndex], FD_READ|FD_CLOSE) == SOCKET_ERROR)
	{
		closesocket(Socket);
		return false;
	}

	_socket* pSocket = &m_Socket[dwSocketIndex];

	pSocket->m_Socket = Socket;	
	memcpy(&pSocket->m_Addr, &Addr, sizeof(Addr));
	pSocket->m_bAccept = true;
	pSocket->m_bSendable = true;
	pSocket->m_dwConnectTime = timeGetTime();
	pSocket->m_dwLastCheckSendTime = pSocket->m_dwLastCheckRecvTime = timeGetTime();
	pSocket->m_dwSerial = m_dwSerialCounter++;

	m_TotalCount.m_dwAcceptNum++;
	
	//local ip??..
	//
	static BYTE byLocalIP[4] = {127, 0, 0, 1};
	if(pSocket->m_Addr.sin_addr.S_un.S_addr == *(DWORD*)byLocalIP)
		GetIPAddress(&pSocket->m_Addr);

	return true;	//success
}*/

bool CNetSocket::Accept_Server(DWORD dwSocketIndex)
{
	_socket* pSocket = &m_Socket[dwSocketIndex];

	int len = sizeof(sockaddr);
	pSocket->m_Socket = accept(m_sAccept, (sockaddr*)&pSocket->m_Addr, &len);
	if(pSocket->m_Socket == INVALID_SOCKET)
		return false;

	//ip체크..
	if(m_SockType.m_bAcceptIPCheck)
	{
		bool bFind = false;
		int nListNum = m_listIPCheck.CopyIndexList(m_dwIPCheckBufferList, m_nIPCheckNodeNum);
		for(int i = 0; i < nListNum; i++)
		{
			_IP_CHECK_NODE* pNode = &m_ndIPCheck[m_dwIPCheckBufferList[i]];

			if(pNode->dwIP == (DWORD)pSocket->m_Addr.sin_addr.S_un.S_addr)
			{
				m_listIPCheck.FindNode(m_dwIPCheckBufferList[i]);
				m_listIPCheck_Empty.PushNode_Back(m_dwIPCheckBufferList[i]);
				bFind = true;
				break;
			}
		}

		if(!bFind)
		{		
			closesocket(pSocket->m_Socket);
			return false;
		}
	}

	if(WSAEventSelect(pSocket->m_Socket, m_Event[dwSocketIndex], FD_READ|FD_CLOSE) == SOCKET_ERROR)
		return false;

	pSocket->m_bAccept = true;
	pSocket->m_bSendable = true;
	pSocket->m_dwConnectTime = timeGetTime();
	pSocket->m_dwLastCheckSendTime = pSocket->m_dwLastCheckRecvTime = timeGetTime();
	pSocket->m_dwSerial = m_dwSerialCounter++;

	m_TotalCount.m_dwAcceptNum++;
	
	//local ip??..
	//
	static BYTE byLocalIP[4] = {127, 0, 0, 1};
	if(pSocket->m_Addr.sin_addr.S_un.S_addr == *(DWORD*)byLocalIP)
		GetIPAddress(&pSocket->m_Addr);

	return true;	//success
}


bool CNetSocket::Accept_Client(DWORD dwSocketIndex)
{
	_socket* pSocket = &m_Socket[dwSocketIndex];

	if(pSocket->m_bAccept)
		return false;	//use socket 

	pSocket->m_bAccept = true;
	pSocket->m_bSendable = true;
	pSocket->m_dwConnectTime = timeGetTime();
	pSocket->m_dwLastCheckSendTime = pSocket->m_dwLastCheckRecvTime = timeGetTime();
	pSocket->m_dwSerial = m_dwSerialCounter++;

	m_TotalCount.m_dwAcceptNum++;

	return true;	//success
}

int CNetSocket::Connect(DWORD n, sockaddr_in* pAddr)
{
	if(m_Socket[n].m_bAccept)
		return -1;	

	int nSize;

	if(m_SockType.m_byProtocolID == protocol_tcp)
	{
		m_Socket[n].m_Socket = socket(AF_INET, SOCK_STREAM, 0);
		nSize = sizeof(sockaddr_in);
	}
	
	else if(m_SockType.m_byProtocolID == protocol_ipx)	
	{
		m_Socket[n].m_Socket = socket(AF_IPX, SOCK_SEQPACKET, NSPROTO_SPX);
		nSize = sizeof(sockaddr_ipx);
	}

	if(connect(m_Socket[n].m_Socket, (sockaddr*)pAddr, nSize) == SOCKET_ERROR) 
	{
		closesocket(m_Socket[n].m_Socket);
		return WSAGetLastError();	//connect() error
	}

	if(WSAEventSelect(m_Socket[n].m_Socket, m_Event[n], FD_CONNECT|FD_READ|FD_CLOSE) == SOCKET_ERROR)
	{
		return -3;	//event select error
	}

	return 0;	//success
}

bool CNetSocket::Recv(DWORD n, char* pBuf, int nBufMaxSize, int* OUT pnRet)
{
	*pnRet = recv(m_Socket[n].m_Socket, pBuf, nBufMaxSize, 0);
	if(*pnRet == SOCKET_ERROR)
	{
		*pnRet = WSAGetLastError();
		if(*pnRet != WSAEWOULDBLOCK)
		{
			m_TotalCount.m_dwTotalRecvErrNum++;
		}
		return false;
	}

//	m_TotalCount.m_dTotalRecvBYTESize += *pnRet;
//	m_Socket[n].m_dwTotalRecvMsg += *pnRet;

	return true;
}

bool CNetSocket::Send(DWORD n, char* pBuf, int nSize, int* OUT pnRet)
{
	*pnRet = send(m_Socket[n].m_Socket, pBuf, nSize, 0);
	if(*pnRet == SOCKET_ERROR)
	{
		*pnRet = WSAGetLastError();
		if(*pnRet == WSAEWOULDBLOCK)
		{
			m_TotalCount.m_dwTotalSendBlockNum++;
			m_Socket[n].m_dwTotalRecvBlock++;
		}
		else
		{
			m_TotalCount.m_dwTotalSendErrNum++;	
		}		
		return false;
	}

//	m_TotalCount.m_dTotalSendBYTESize += *pnRet;
//	m_Socket[n].m_dwTotalSendMsg += *pnRet;

	return true;
}

void CNetSocket::EmptySocketBuffer(DWORD n)
{
	char szBuf[128];
	while(1)
	{
		int nRet = recv(m_Socket[n].m_Socket, szBuf, 128, 0);
	
		if(nRet == 0 || nRet == SOCKET_ERROR)
			break;
	}
}

bool CNetSocket::CloseSocket(DWORD n)
{
	shutdown(m_Socket[n].m_Socket, SD_SEND);

	EmptySocketBuffer(n);

	m_TotalCount.m_dwAcceptNum--;

	closesocket(m_Socket[n].m_Socket);
	m_Socket[n].InitParam();
	m_Socket[n].m_dwLastCloseTime = timeGetTime();
	
	return true;
}

bool CNetSocket::PushIPCheckList(DWORD dwIP)
{
	if(!m_SockType.m_bAcceptIPCheck)
		return false;

	DWORD dwEmptyIndex;
	if(!m_listIPCheck_Empty.PopNode_Front(&dwEmptyIndex))
		return false;

	m_ndIPCheck[dwEmptyIndex].dwIP = dwIP;
	m_ndIPCheck[dwEmptyIndex].dwWaitStartTime = timeGetTime();

	bool bSucc = m_listIPCheck.PushNode_Back(dwEmptyIndex);

	if(!bSucc)
		return false;

	return true;
}

bool CNetSocket::InitAcceptSocket()
{
//socket()
	if(m_SockType.m_byProtocolID == protocol_tcp)
		m_sAccept = socket(AF_INET, SOCK_STREAM, 0); 
	
	else if(m_SockType.m_byProtocolID == protocol_ipx)
		m_sAccept = socket(AF_IPX, SOCK_SEQPACKET, NSPROTO_SPX);

	if(m_sAccept == INVALID_SOCKET) 
	{
		char szErr[128];
		wsprintf(szErr, "socket() Failure => %d", WSAGetLastError());
		MessageBox(NULL, "CNetSocket=>InitAcceptSocket", szErr, MB_OK);
		return false;
	}

//bind()
	int nSize;
	
	if(m_SockType.m_byProtocolID == protocol_tcp)
		nSize = sizeof(sockaddr_in);
	
	else if(m_SockType.m_byProtocolID == protocol_ipx)
		nSize = sizeof(sockaddr_ipx);

	sockaddr_in Addr;
	ZeroMemory(&Addr, sizeof(sockaddr_in));
	Addr.sin_family = AF_INET;
	Addr.sin_port = htons(m_SockType.m_wPort);
	Addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(m_sAccept, (sockaddr*)&Addr, nSize) == SOCKET_ERROR)
	{
		char szErr[128];
		wsprintf(szErr, "bind() Failure => %d", WSAGetLastError());
		MessageBox(NULL, "CNetSocket=>InitAcceptSocket", szErr, MB_OK);
		return false;
	}


//listen()
	if(listen(m_sAccept, 5) == SOCKET_ERROR) 
	{
		char szErr[128];
		wsprintf(szErr, "listen() Failure => %d", WSAGetLastError());
		MessageBox(NULL, "CNetSocket=>InitAcceptSocket", szErr, MB_OK);
		return false;
	}

//Accept Socket Set EventHandle & NonBlocking Mode
	m_AcceptEvent = WSACreateEvent();
	if(WSAEventSelect(m_sAccept, m_AcceptEvent, FD_ACCEPT) == SOCKET_ERROR)
	{
		char szErr[128];
		wsprintf(szErr, "WSAEventSelect() Failure => %d", WSAGetLastError());
		MessageBox(NULL, "CNetSocket=>InitAcceptSocket", szErr, MB_OK);
		return false;
	}

	return true;	
}

void CNetSocket::CloseAll()
{
	for(DWORD i = 0; i < m_SockType.m_wSocketMaxNum; i++)
	{
		if(m_Socket[i].m_bAccept)
		{
			CloseSocket(i);
			Sleep(1);
		}
	}

	if(m_SockType.m_bServer)
		closesocket(m_sAccept);
}

_SOCK_TYPE_PARAM* CNetSocket::GetSocketType()
{
	return &m_SockType;
}

_total_count* CNetSocket::GetTotalCount()
{
	return &m_TotalCount;
}

unsigned long CNetSocket::GetSocketIPAddress(DWORD dwIndex)
{
	return m_Socket[dwIndex].m_Addr.sin_addr.S_un.S_addr; 
}

DWORD CNetSocket::FindEmptySocket()
{
	for(DWORD i = 0; i < m_SockType.m_wSocketMaxNum; i++)
	{
		if(!m_Socket[i].m_bAccept)	//accept => false
		{
			if(timeGetTime()-m_Socket[i].m_dwLastCloseTime > m_SockType.m_dwSocketRecycleTerm)//끊은지 1분이상된것을 사용한다.
				return i;	//success
		}
	}

	return 0xFFFFFFFF;	//false
}

