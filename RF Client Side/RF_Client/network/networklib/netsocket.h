// NetSokcet.h: interface for the CNetwork class.
//EventAsync방식의 socket api 클래스
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETWORK_H__C1F75719_8F42_474C_A115_0275A62A3E95__INCLUDED_)
#define AFX_NETWORK_H__C1F75719_8F42_474C_A115_0275A62A3E95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NetUtil.h"
#include "../Protocol/Protocol.h"

#define	sock_max	3840

#define protocol_tcp 0
#define protocol_ipx 1

#define check_bound_socket_term 200000//	20초..

struct _socket
{
	SOCKET		m_Socket;
	int			m_nIndex;
	DWORD		m_dwSerial;
	sockaddr_in m_Addr;
	DWORD		m_dwClientInfo;

	bool		m_bAccept;
	bool		m_bSendable;
//	bool		m_bCheckRealClient;
	DWORD		m_dwConnectTime;
	DWORD		m_dwLastCloseTime;
	DWORD		m_dwLastCheckSendTime;
	DWORD		m_dwLastCheckRecvTime;	//마지막으로 쿼리에 응답한시간
	DWORD		m_dwRealClientKeyValue[CHECK_KEY_NUM];
	
	DWORD		m_dwTotalRecvMsg;
	DWORD		m_dwTotalSendMsg;
	DWORD		m_dwTotalRecvBlock;

	DWORD		m_dwBoundResponTerm;
	DWORD		m_dwSendBlockTime;
	DWORD		m_dwRecvPopMissTime;
	DWORD		m_dwSendSpeedHackTime;
	DWORD		m_dwResponSpeedHackTime;
	BYTE		m_bySpeedHackContCount; //연속 스핵에 적발된 카운트

	_socket()
	{
		InitParam();	
	}

	~_socket()
	{
	}

	void InitParam()
	{
		m_nIndex = -1;
		m_dwTotalRecvMsg = 0;
		m_dwTotalSendMsg = 0;
		m_dwTotalRecvBlock = 0;
		m_dwLastCloseTime = 0;
		m_dwRecvPopMissTime = 0;
		m_dwSendSpeedHackTime = 0;
		m_dwResponSpeedHackTime = 0;
		m_bySpeedHackContCount = 0;
		m_dwClientInfo = 0xFFFFFFFF;
		m_dwBoundResponTerm = check_bound_socket_term;
		m_bAccept = false;
		m_bSendable = false;
//		m_bCheckRealClient = false;
		m_Socket = INVALID_SOCKET;
	}
};

struct _SOCK_TYPE_PARAM
{
	BYTE	m_byProtocolID;
	BOOL	m_bServer;
	WORD	m_wPort;
	WORD	m_wSocketMaxNum;
	BOOL	m_bAcceptIPCheck;
	DWORD	m_dwIPCheckTerm;
	DWORD	m_dwSocketRecycleTerm;

	_SOCK_TYPE_PARAM(){
	
		m_bServer = FALSE;		
		m_bAcceptIPCheck = FALSE;
		m_byProtocolID = protocol_tcp;
		m_wSocketMaxNum = 1;
		m_dwIPCheckTerm = 60000;//기본 60초
		m_dwSocketRecycleTerm = 5000;//기본 5초..
	}
};

struct _total_count
{
	DWORD	m_dwAcceptNum;
	DWORD	m_dwSendAbleNum;	
	double	m_dTotalRecvBYTESize;
	double	m_dTotalSendBYTESize;
	DWORD	m_dwTotalRecvErrNum;	
	DWORD	m_dwTotalSendErrNum;	
	DWORD	m_dwTotalSendBlockNum;
	DWORD	m_dwTotalRecvBlockNum;
	DWORD	m_dwBufferClean;
	DWORD	m_dwFdWriteCnt;

	_total_count(){
		memset(this, 0, sizeof(*this));
	}

	double GetTotalRecvSize(){
		return m_dTotalRecvBYTESize;
	}

	double GetTotalSendSize(){
		return m_dTotalSendBYTESize;
	}
};

struct _IP_CHECK_NODE
{
	DWORD	dwIP;
	DWORD	dwWaitStartTime;
};

class CNetSocket  
{
public:

	SOCKET				m_sAccept;			
	WSAEVENT			m_AcceptEvent;

	_socket				m_Socket[sock_max];
	WSAEVENT			m_Event[sock_max];

	_SOCK_TYPE_PARAM	m_SockType;
	_total_count		m_TotalCount;

private:

	bool				m_bSetSocket;
	DWORD				m_dwSerialCounter;
	int					m_nIPCheckNodeNum;
	_IP_CHECK_NODE		m_ndIPCheck[sock_max*2];
	DWORD				m_dwIPCheckBufferList[sock_max*2];
	CNetIndexList		m_listIPCheck;
	CNetIndexList		m_listIPCheck_Empty;
	CNetTimer			m_tmrListCheckerIPCheck;

public:
	CNetSocket();
	virtual ~CNetSocket();

	bool SetSocket(_SOCK_TYPE_PARAM* pType);
	void Release();
	void OnLoop();
	
	bool Accept_Server(DWORD dwSocketIndex);
	bool Accept_Client(DWORD dwSocketIndex);//Client Only
	int Connect(DWORD n, sockaddr_in* pAddr);//Server와의 연결을 시도 ==> Client Only
	bool Recv(DWORD n, char* pBuf, int nBufMaxSize, int* OUT pnRet);
	bool Send(DWORD n, char* pBuf, int nSize, int* OUT pnRet);
	bool CloseSocket(DWORD n);
	void EmptySocketBuffer(DWORD n);

	bool PushIPCheckList(DWORD dwIP);

	_socket* GetSocket(DWORD dwIndex) { return &m_Socket[dwIndex]; }
	_SOCK_TYPE_PARAM* GetSocketType();
	_total_count* GetTotalCount();
	unsigned long GetSocketIPAddress(DWORD dwIndex);
	DWORD FindEmptySocket();

private:
	bool InitAcceptSocket();
	void CloseAll();
};


#endif // !defined(AFX_NETWORK_H__C1F75719_8F42_474C_A115_0275A62A3E95__INCLUDED_)
