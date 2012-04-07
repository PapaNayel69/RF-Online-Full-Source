// NetWorking.h: interface for the CNetWorking class.
//각각의 프로토에 해당하는 CNetProcess을 구동시킨다.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETWORKING_H__00BF4649_3825_42B8_BFC7_C2341FAA0D2F__INCLUDED_)
#define AFX_NETWORKING_H__00BF4649_3825_42B8_BFC7_C2341FAA0D2F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NetProcess.h"

#define max_process 3

class CNetWorking  
{
public:
	char m_szSystemName[128];

	DWORD			m_dwUseProcessNum;
	CLogFile		m_LogFile;
	CNetProcess		m_Process[max_process];
	CNetProcess*	m_pProcess[max_process];

public:
	CNetWorking();
	virtual ~CNetWorking();

	void SetNetSystem(DWORD dwUseProcessNum, _NET_TYPE_PARAM* pType, char* szSystemName);
	void OnLoop();

	virtual void UserLoop(){};

	void ProcessLogFile(DWORD dwProID, bool bRecv, bool bSend, bool bSystem);
	bool Connect(DWORD dwProID, DWORD dwSocketIndex, DWORD dwIP, WORD wPort);
	void Close(DWORD dwProID, DWORD dwSocketIndex, bool bSlowClose = false);
	_socket* GetSocket(DWORD dwProID, DWORD dwSocketIndex);
	DWORD GetCheckRecvTime(DWORD dwProID, DWORD dwSocketIndex);

	virtual bool DataAnalysis(DWORD	dwProID, DWORD dwClientIndex, _MSG_HEADER* pMsgHeader, char* pMsg) {return true;}
	virtual void AcceptClientCheck(DWORD dwProID, DWORD dwIndex, DWORD dwSerial) {}
	virtual void CloseClientCheck(DWORD	dwProID, DWORD dwIndex, DWORD dwSerial) {}
};

#endif // !defined(AFX_NETWORKING_H__00BF4649_3825_42B8_BFC7_C2341FAA0D2F__INCLUDED_)
