// NetWorking.cpp: implementation of the CNetWorking class.
//
//////////////////////////////////////////////////////////////////////
#include "NetWorking.h"
#include <mmsystem.h>
#include <process.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNetWorking::CNetWorking()
{
	m_dwUseProcessNum = 0;

	WSADATA wsaData;
	if(WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
	{
		char szErr[128];
		wsprintf(szErr, "WSAStartup() Failure => %d", WSAGetLastError());
		MessageBox(NULL, "CNetWorking=>StartUp()", szErr, MB_OK);
		exit(0);	
	}

	else if(LOBYTE(wsaData.wVersion ) != 2 || HIBYTE(wsaData.wVersion) != 0) 
	{
		WSACleanup();
		char szErr[128];
		wsprintf(szErr, "wsaData.wVersion Failure");
		MessageBox(NULL, "CNetWorking=>StartUp()", szErr, MB_OK);
		exit(0);	
	}

	for(int i = 0; i < max_process; i++)
		m_pProcess[i] = &m_Process[i];
}

CNetWorking::~CNetWorking()
{
	for(DWORD i = 0; i < m_dwUseProcessNum; i++)
	{
		m_Process[i].Release();
	}

	WSACleanup();
}

void CNetWorking::SetNetSystem(DWORD dwUseProcessNum, _NET_TYPE_PARAM* pType, char* szSystemName)
{
	m_dwUseProcessNum = dwUseProcessNum;
	wsprintf(m_szSystemName, "%s", szSystemName);

	char szProcessLogPath[] = ".\\NetLog";
	::CreateDirectory(szProcessLogPath, NULL);

	char szLogFileName[128];
	wsprintf(szLogFileName, ".\\NetLog\\%s_Sys.log", m_szSystemName);
	m_LogFile.SetWriteLogFile(szLogFileName, true, true);

	for(DWORD i = 0; i < m_dwUseProcessNum; i++)
		m_Process[i].SetProcess(i, &pType[i], this);
}

void CNetWorking::ProcessLogFile(DWORD dwProID, bool bRecv, bool bSend, bool bSystem)
{
	m_Process[dwProID].LogFileOperSetting(bRecv, bSend, bSystem);
}

bool CNetWorking::Connect(DWORD dwProID, DWORD dwSocketIndex, DWORD dwIP, WORD wPort)
{
	sockaddr_in Addr;

	ZeroMemory(&Addr,sizeof(sockaddr_in));
	Addr.sin_family = AF_INET;
	Addr.sin_port = htons(wPort);
	Addr.sin_addr.S_un.S_addr = dwIP;

	int nRet = m_Process[dwProID].m_NetSocket.Connect(dwSocketIndex, &Addr);
	if(nRet != 0)
		return false;
	return true;
}

void CNetWorking::Close(DWORD dwProID, DWORD dwSocketIndex, bool bSlowClose)
{
	if(!bSlowClose)
		m_Process[dwProID].PushCloseNode(dwSocketIndex);
	else
	{
		_socket* pSocket = m_Process[dwProID].m_NetSocket.GetSocket(dwSocketIndex);
		if(!m_Process[dwProID].m_FC.PushNode(dwSocketIndex, pSocket->m_dwSerial))
			m_Process[dwProID].PushCloseNode(dwSocketIndex);
	}
}

_socket* CNetWorking::GetSocket(DWORD dwProID, DWORD dwSocketIndex)
{
	if(dwProID >= m_dwUseProcessNum)
		return NULL;

	return m_Process[dwProID].m_NetSocket.GetSocket(dwSocketIndex);
}

void CNetWorking::OnLoop()
{
	for(DWORD p = 0; p < m_dwUseProcessNum; p++)
		m_Process[p].OnLoop();

	UserLoop();
}
	
DWORD CNetWorking::GetCheckRecvTime(DWORD dwProID, DWORD dwSocketIndex)
{
	_socket* pSocket = GetSocket(dwProID, dwSocketIndex);

	if(!pSocket)
		return 0xFFFFFFFF;

	return pSocket->m_dwLastCheckRecvTime;
}