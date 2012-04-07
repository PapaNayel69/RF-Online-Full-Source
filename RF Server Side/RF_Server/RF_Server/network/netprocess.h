// NetProcess.h: interface for the CNetProcess class.
//CNetSocket과 CNetData를 이벤트 신호에 의해 연결
//Protocol단위로 존재
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETPROCESS_H__0ECEFE37_BCB3_4AB6_A636_400DC89E4C39__INCLUDED_)
#define AFX_NETPROCESS_H__0ECEFE37_BCB3_4AB6_A636_400DC89E4C39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NetSocket.h"
#include "NetUtil.h"
#include <mmsystem.h>

#define CLIENT FALSE
#define SERVER TRUE

//쓰레드 초기화 구조체
struct _thread_parameter
{
	BOOL	m_bStart;	
	void*	m_pParam;	//thread에 넘겨줄  parameter의  pointer
	int		m_nIndex;

	_thread_parameter()
	{
		m_bStart = FALSE;
		m_pParam = NULL;
		m_nIndex = -1;
	}

	void SetParameter(BOOL bStart, void* pParam, int nIndex)	//thread의 start
	{
		m_bStart = bStart;
		m_pParam = pParam;
		m_nIndex = nIndex;
	}

	void EndThread()	//thread의 exit	
	{
		m_bStart = FALSE;
	}
};

struct _NET_TYPE_PARAM : public _SOCK_TYPE_PARAM
{
	enum	{	check_send_socket_term = 10000 }; //10초..

	BYTE	m_byRecvThreadNum;			//Recv Thread 수 
	BOOL	m_bSendThread;				//Send Thread 사용할것인지
	BOOL	m_bSpeedHackCheck;			//클라이언트의 speed hack 사용체크할것인지
	BOOL	m_bSystemLogFile;			//파일에 로그기록 할것인지	
	BOOL	m_bRecvLogFile;				//받은패킷헤더를 파일에 기록할것인지
	BOOL	m_bSendLogFile;				//보낸패킷헤더를 파일에 기록할것인지	
	BOOL	m_bOddMsgWriteLog;			//잘못된 패킷을 파일에 기록할것인지
	BOOL	m_bOddMsgDisconnect;		//잘못된 패킷이 올경우 접속을 종료시컬것인지
//	BOOL	m_bRealClientCheck;			//진짜 클라이언트인지 체크할것인지..
	BYTE	m_byRecvSleepTime;			//Recv Thread의 루프를 몇회전마다 Sleep(1)걸어줄것인지
	BYTE	m_bySendSleepTime;			//Send Thread의 루프를 몇회전마다 Sleep(1)걸어줄것인지
	BOOL	m_bSvrToS;					//연결이 서비스 서버대 서비스 서버인지
	BOOL	m_bRealSockCheck;			//주기적인 소켓체크(send()..)를 할것인지
	DWORD   m_dwRealSendCheckTerm;		//소켓체크하는 Term
	BOOL	m_bKeyCheck;				//새로 접속하는 클라이언트의 serial, key, ip를 체크할것인지
	DWORD	m_dwKeyCheckTerm;			//키체크사용시 최대 대기가능한 Term
	DWORD	m_dwProcessMsgNumPerLoop;	//한루프당 처리할수있는 메시지수..
	char	m_szModuleName[128];		//연결모듈이름

	_NET_TYPE_PARAM(){

		m_bRealSockCheck = FALSE;
		m_bSendThread = FALSE;
		m_bSystemLogFile = FALSE;
		m_bRecvLogFile = FALSE;
		m_bSendLogFile = FALSE;
		m_bOddMsgWriteLog = FALSE;
		m_bOddMsgDisconnect = FALSE;
		m_bSpeedHackCheck = FALSE;
//		m_bRealClientCheck = FALSE;
		m_bKeyCheck = FALSE;
		m_bSvrToS = FALSE;
		m_szModuleName[0] = NULL;
		m_byRecvThreadNum = 2;
		m_byRecvSleepTime = 1;
		m_bySendSleepTime = 1;
		m_dwKeyCheckTerm = 60000; //60초
		m_dwProcessMsgNumPerLoop = 0xFFFFFFFF;
		m_dwRealSendCheckTerm = check_send_socket_term;
	}
};

struct _KEY_CHECK_NODE
{
//	enum {	key_num = 4 };

	DWORD dwSerial;
	DWORD dwKey[CHECK_KEY_NUM];
	DWORD dwIP;
	DWORD dwWaitStartTime;
};

struct _REAL_CLIENT_CHECK_NODE
{
	enum	{	send_delay_time = 1000	};
	enum	{	check_recv_time = 3000	};	//dwAcceptTime으로부터..

	DWORD dwSocketIndex;
	DWORD dwSocketSerial;
	DWORD dwAcceptTime;
};

struct _FORCE_CLOSE
{
	struct __FD_NODE{
		DWORD	dwObjIndex;
		DWORD	dwObjSerial;
		DWORD   dwEventCreateTime;
	};

	enum				{	max_fd_node			= 0xFFFF	};
	enum				{	timer_fd_term		= 5*1000	};	//5초..

	DWORD					m_dwNodeNum;
	__FD_NODE*				m_pFDData;

	CNetTimer				m_tmFD;
	CNetIndexList			m_listFD;
	CNetIndexList			m_listFDEmpty;

	_FORCE_CLOSE()
	{
		m_dwNodeNum = 0;
		m_pFDData = NULL;
	}

	~_FORCE_CLOSE()
	{
		if(m_pFDData)
			delete [] m_pFDData;
	}

	bool Init(DWORD dwNodeNum)
	{
		if(dwNodeNum == 0 || dwNodeNum > max_fd_node)
			return false;

		m_dwNodeNum = dwNodeNum;
		m_pFDData = new __FD_NODE [dwNodeNum];

		m_tmFD.BeginTimer(timer_fd_term/2);
		m_listFD.SetList(dwNodeNum);
		m_listFDEmpty.SetList(dwNodeNum);
		for(DWORD i = 0; i < dwNodeNum; i++)
			m_listFDEmpty.PushNode_Back(i);

		return true;
	}

	bool PushNode(DWORD dwIndex, DWORD dwSerial)
	{
		DWORD dwEmptyIndex;
		if(m_listFDEmpty.PopNode_Front(&dwEmptyIndex))
		{
			__FD_NODE* pNode = &m_pFDData[dwEmptyIndex];
			pNode->dwEventCreateTime = timeGetTime();
			pNode->dwObjIndex = dwIndex;
			pNode->dwObjSerial = dwSerial;
			m_listFD.PushNode_Back(dwEmptyIndex);
			return true;
		}
		return false;
	}
};

#define msg_header_num 2

#pragma pack(push, 1)

struct _MSG_HEADER
{
	WORD m_wSize;
	BYTE m_byType[msg_header_num];	
};

#pragma pack(pop)

#define MAX_NET_BUFFER		10000
#define MAX_EXTRA_BUFFER	10000
#define MSG_HEADER_SIZE		sizeof(_MSG_HEADER)
#define MSG_LEN_HEADER_SIZE	sizeof(WORD)

struct _NET_BUFFER
{
	//Internal Process..
	int		m_dwPopRot;
	int		m_dwPopPnt;

	//PUSH
	int		m_dwPushRot;
	int		m_dwPushPnt;

	char	m_sMainBuffer[MAX_NET_BUFFER + MAX_EXTRA_BUFFER];//MAX_EXTRA_BUFFER의 여유분..
	char	m_sTempBuffer[MAX_EXTRA_BUFFER];

	CNetCriticalSection	m_csPush;
	CNetCriticalSection	m_csPop;

	_NET_BUFFER() 
	{	
		Init();	
	}

	void Init() 
	{
		m_csPush.Lock();
		m_dwPushRot = m_dwPushPnt = 0;
		m_csPush.Unlock();

		m_csPop.Lock();
		m_dwPopRot = m_dwPopPnt =  0;
		m_csPop.Unlock();		
	}

	char* GetPushPos()
	{
		return &m_sMainBuffer[m_dwPushPnt];
	}

	char* GetPopPos()
	{
		return &m_sMainBuffer[m_dwPopPnt];
	}

/*	DWORD GetTotalPush()
	{
		m_csPush.Lock();
		DWORD dwTotal = m_dwPushRot * MAX_NET_BUFFER + m_dwPushPnt;
		m_csPush.Unlock();

		return dwTotal;		
	}

	DWORD GetTotalPop()
	{
		m_csPop.Lock();
		DWORD dwTotal = m_dwPopRot * MAX_NET_BUFFER + m_dwPopPnt;
		m_csPop.Unlock();

		return dwTotal;
	}*/

	int GetLeftLoadSize()
	{
		m_csPop.Lock();
		int  nGap = (m_dwPushRot - m_dwPopRot) * MAX_NET_BUFFER + (m_dwPushPnt - m_dwPopPnt);
		m_csPop.Unlock();
		return nGap;
	}

	void AddPushPos(DWORD dwAddSize)
	{
		m_csPush.Lock();

		DWORD dwBufPnt = m_dwPushPnt + dwAddSize;

		if(dwBufPnt >= MAX_NET_BUFFER)
		{
			m_dwPushPnt = dwBufPnt % MAX_NET_BUFFER;
			m_dwPushRot++;
		}
		else
			m_dwPushPnt = dwBufPnt;

		m_csPush.Unlock();
	}

	void AddPopPos(DWORD dwAddSize)
	{
		m_csPop.Lock();

		DWORD dwBufPnt = m_dwPopPnt + dwAddSize;

		if(dwBufPnt >= MAX_NET_BUFFER)
		{
			m_dwPopPnt = dwBufPnt % MAX_NET_BUFFER;
			m_dwPopRot++;
		}	
		else
			m_dwPopPnt = dwBufPnt;

		m_csPop.Unlock();
	}

	char* GetPopPoint(bool* OUT pbMiss)
	{
		//읽기가능한 버퍼사이즈
		int dwReadableSize = GetLeftLoadSize();//GetTotalPush() - GetTotalPop();
		DWORD dwBufPopPnt = m_dwPopPnt;

		//읽을 데이터가 아예없으면 그냥 리턴
		if(dwReadableSize == 0)
		{
			pbMiss = false;
			return NULL;
		}
		
		//MSG_HEADER_SIZE보다 작으면 실패
		if(dwReadableSize < MSG_HEADER_SIZE)
		{
			*pbMiss = true;
			return NULL;
		}

		//Msg 길이
		WORD wMsgSize;// = *(WORD*)&m_sMainBuffer[dwBufPopPnt];

		//만약 PopPnt 이후로 사이즈변수길이(word)가 일직선에있는경우
		if(dwBufPopPnt + MSG_LEN_HEADER_SIZE <= MAX_NET_BUFFER)
		{
			wMsgSize = *(WORD*)&m_sMainBuffer[dwBufPopPnt];
		}
		//만약 PopPnt 이후로 사이즈변수길이(word)가 일직선에있지않는경우
		else
		{
			((BYTE*)&wMsgSize)[0] = *(BYTE*)&m_sMainBuffer[dwBufPopPnt];
			((BYTE*)&wMsgSize)[1] = *(BYTE*)&m_sMainBuffer[(dwBufPopPnt + 1) % MAX_NET_BUFFER];
		}

		if(wMsgSize < MSG_HEADER_SIZE)
		{
			*pbMiss = false;
			return NULL;
		}

		//Msg길이가 dwReadableSize 보다 같거나 작은지..
		if(wMsgSize > dwReadableSize)
		{
			*pbMiss = true;
			return NULL;
		}

	//	return &m_sMainBuffer[dwBufPopPnt];


		//리턴값
		char* pRet = NULL;
		//만약 PopPnt 이후로 Msg사이즈만큼 일직선에있는경우
		if(dwBufPopPnt + wMsgSize <= MAX_NET_BUFFER)
		{
			pRet = &m_sMainBuffer[dwBufPopPnt];
		}
		//만약 PopPnt 이후로 Msg사이즈만큼 일직선에있지않은경우
		else
		{
			DWORD dw1stSize = MAX_NET_BUFFER - dwBufPopPnt;

			memcpy(&m_sTempBuffer[0], &m_sMainBuffer[dwBufPopPnt], dw1stSize);
			memcpy(&m_sTempBuffer[dw1stSize], &m_sMainBuffer[0], wMsgSize - dw1stSize);

			pRet = m_sTempBuffer;
		}

		return pRet;
	}

	char* GetSendPoint(int* OUT pnSendSize, bool* OUT pMiss)
	{
		//읽기가능한 버퍼사이즈
		*pnSendSize = GetLeftLoadSize();//GetTotalPush() - GetTotalPop();		
		DWORD dwBufPopPnt = m_dwPopPnt;

		//읽을 데이터가 아예없으면 그냥 리턴
		if(*pnSendSize <= 0)
		{
			*pMiss = false;
			return NULL;
		}

		//만약 PopPnt 이후로 Msg사이즈만큼 일직선에있지않은경우
		if(dwBufPopPnt + *pnSendSize > MAX_NET_BUFFER)
		{
			DWORD dw1stSize = MAX_NET_BUFFER - dwBufPopPnt;
			if(dw1stSize > MAX_EXTRA_BUFFER)
			{
				*pMiss = true;
				return NULL;
			}

			memcpy(&m_sTempBuffer[0], &m_sMainBuffer[dwBufPopPnt], dw1stSize);
			memcpy(&m_sTempBuffer[dw1stSize], &m_sMainBuffer[0], *pnSendSize - dw1stSize);

			return m_sTempBuffer;
		}

		return &m_sMainBuffer[dwBufPopPnt];
	}
};

class CNetWorking;

class CNetProcess  
{
public:
	enum		{		send_log			= 0,	//log file  구분
						recv_log			= 1,
						system_log			= 2 };

	enum		{		net_event_default	= 0,
						net_event_connect	= 1,
						net_event_read		= 2,
						net_event_close		= 4	};	

	enum		{		event_per_therad	= 64	};
	enum		{		event_thread_max	= sock_max/event_per_therad};
	enum		{		recv_thread_max		= 8 };

	//구동할 CNetSocket과  CNetData
	CNetSocket			m_NetSocket; 
	_FORCE_CLOSE		m_FC;
	_NET_TYPE_PARAM		m_Type;

	int					m_nIndex;
	int					m_nOddMsgNum;	//잘못된 message수

private:

	bool				m_bSetProcess;
	int					m_nEventThreadNum;	//event를 대기하는 thread의 수 ==> m_nMaxSocket/64에 나머지가 0보다 크면 +1
	CNetWorking*		m_pNetwork;
	CLogFile			m_LogFile[3];

	CNetCriticalSection* m_csRecv;
	_NET_BUFFER*		m_pRecvBuffer;
	_NET_BUFFER*		m_pSendBuffer;

	_thread_parameter	m_pmAcceptThread;	//accept thread꺼
	_thread_parameter	m_pmEventThread[event_thread_max];	//event thread꺼
	_thread_parameter	m_pmRecvThread[recv_thread_max];
	_thread_parameter	m_pmSendThread;

	CNetIndexList		m_listAcceptEvent;
	CNetIndexList		m_listCloseEvent;
	CNetIndexList		m_listRecvEvent;
	CNetIndexList		m_listAcceptReady;

	CNetFrameRate		m_fpsSendThread;

	int					m_nKeyCheckNodeNum;
	_KEY_CHECK_NODE*	m_ndKeyCheck;
	DWORD*				m_dwKeyCheckBufferList;
	CNetIndexList		m_listKeyCheck;
	CNetIndexList		m_listKeyCheck_Empty;
	CNetTimer			m_tmrListCheckerKeyCheck;

	DWORD				m_dwLoopCurTime;

public:

	CNetProcess();
	virtual ~CNetProcess();
	
	bool SetProcess(int nIndex, _NET_TYPE_PARAM* pType, CNetWorking* pNetwork);
	void Release();
	void CloseAll();	

	void OnLoop();

	//대기리스트 체크
	bool PushKeyCheckList(DWORD dwSerial, DWORD dwIP, DWORD* pdwKey, int nUseKeyNum);
	bool FindKeyFromWaitList(DWORD dwSocketIndex, DWORD dwSerial, DWORD* pdwKey, int nUseKeyNum);
	BOOL LoadSendMsg(DWORD dwClientIndex, BYTE* pbyType, char* szMsg, WORD nLen);
	BOOL LoadSendMsg(DWORD dwClientIndex, WORD wType, char* szMsg, WORD nLen);
	bool StartSpeedHackCheck(DWORD dwClientIndex, DWORD dwClientInfo);

	DWORD GetSendThreadFrame();
	void PushCloseNode(int nIndex);
	void LogFileOperSetting(bool bRecv, bool bSend, bool bSystem);
	void IOLogFileOperSetting(bool bOper);

private:

	static void AcceptThread(void* pv);
	static void NetEventThread(LPVOID pParam);
	static void RecvThread(void* pv);
	static void SendThread(void* pv);

	bool wt_AcceptClient(DWORD* pdwClientIndex);
	bool wt_CloseClient(DWORD dwClientIndex);

	void _Receipt();
	void _PopRecvMsg(WORD wSocketIndex);
	void _CheckSend(WORD wSocketIndex);
	void _SendLoop(DWORD dwSocketIndex);

	bool _InternalPacketProcess(DWORD dwSocketIndex, _MSG_HEADER* pMsgHeader, char* pMsg);

	void _CheckWaitKey();
	void _ForceCloseLoop();
	void _ResponSpeedHack();
};

#define check_msg		101
#define secure_msg		102	//보안관련..
#define real_msg		222	//클라이언트체크

#pragma pack(push, 1)

//check_msg
#define check_query 1
struct _check_query {

	char sDum;

	int size(){	return sizeof(*this); }
};

#define check_answer 2
struct _check_answer {

	char sDum;

	int size(){	return sizeof(*this); }
};

//check_msg
#define ping_pong	3
struct _ping_pong {

	char sDum[111];

	int size(){	return sizeof(*this); }
};

//secure_msg
#define respon_free_request 1
struct _respon_free_request {

	DWORD dwDeveloperKey[4];//임시로 {0, 1, 2, 3} 로..

	int size(){	return sizeof(*this); }
};

#define check_speed_hack_ask 2
struct _check_speed_hack_ask {
	enum	{	ans_term	= 5000	};	//5초이상 딜레이

	char sDum;

	int size(){	return sizeof(*this); }
};

#define check_speed_hack_ans 3
struct _check_speed_hack_ans {

	char sDum;

	int size(){	return sizeof(*this); }
};

//real_msg
#define real_client_ask 1
struct _real_client_ask {

	DWORD dwKey[CHECK_KEY_NUM];
};

#define real_client_ans 2
struct _real_client_ans {

	DWORD dwResponKey[CHECK_KEY_NUM];
};

#define real_client_complete 3
struct _real_client_complete {
	
};

#pragma pack(pop)

DWORD* CalcCodeKey(DWORD* pdwCode);

#endif // !defined(AFX_NETPROCESS_H__0ECEFE37_BCB3_4AB6_A636_400DC89E4C39__INCLUDED_)
