#ifndef _mainthread_h_
#define _mainthread_h_

#include <process.h>
#include "NetworkEX.h"
#include "CharacterDB.h"
#include "pt_account_login.h"
#include "RFWebDatabase.h"
#include "LoginDBQrySyn.h"
#include "MyClassUtil.h"

struct _WORLD_DATA
{
	bool	m_bOpen;			//접속가능상태인지
	char	m_szWorldName[max_world_name_len+1];	//월드이름
	DWORD	m_dwGateIP;			//클라이언트가 connect할 월드IP
	WORD	m_wGatePort;		//클라이언트가 connect할 월드PORT
	int		m_nUserNum;			//현재 접속중인 유저수

	_WORLD_DATA()
	{
		m_bOpen = false;
	}

	void SetWorld(char* szWorldName)
	{
		strcpy(m_szWorldName, szWorldName);		
	}

	void OpenWorld(DWORD dwGateIP, WORD wGatePort)
	{
		m_bOpen = true;
		m_dwGateIP = dwGateIP;
		m_wGatePort = wGatePort;
		m_nUserNum = 0;
	}

	void CloseWorld()
	{
		m_bOpen = false;
	}

	void SetUserNum(int nUserNum)
	{
		m_nUserNum = nUserNum;
	}
};

struct _CLIENT_DATA
{
	bool	m_bLogin;						//로그인중인지
	bool	m_bLoginCheck;					//x2o에 계정체크를 완료한 상태인지..
	BYTE	m_byLoginCode;					//tbl_rfuser 또는 tbl_rfdev 어느 테이블에있는 유저인지
	bool	m_bRegedWorld;					//월드리스트를 가져왔는지
	bool	m_bSelectWorld;					//월드를 선택했는지
	bool	m_bOverlapUser;					//로그인요청시 이미 다른사용자가 있었는지
	_CLID	m_idLocal;						//로그인서버의 식별자
	_GLBID	m_gidGlobal;					//계정서버의 식별자
	char	m_szAccountID[max_id_len+1];	//계정
	char	m_szAccountPW[max_pw_len+1];	//비번
	DWORD	m_dwAccountSerial;				//계정시리얼
	BYTE	m_byUserGrade;					//유저등급
	DWORD	m_dwSeletWorldCode;				//선택한 월드코드
	BYTE	m_byPlusKey;					//암호화 키값1
	WORD	m_wXORKey;						//암호화 키값2
							
	_CLIENT_DATA(){			
		Init(0xFFFF);			
	}						
							
	void Init(WORD wIndex)
	{	
		m_idLocal.wIndex = wIndex;
		m_idLocal.dwSerial = 0xFFFFFFFF;
		m_gidGlobal.dwSerial = 0xFFFFFFFF;
		m_bLogin = false;
		m_bRegedWorld = false;
		m_bSelectWorld = false;
		m_bOverlapUser = false;
		m_bLoginCheck = false;
		m_dwAccountSerial = 0xFFFFFFFF;
		m_dwSeletWorldCode = 0xFFFFFFFF;
		m_byUserGrade = USER_DEGREE_STD;
	}

	//클라이언트로부터 접속을 받음( call: CNetworkEX::AcceptClientCheck() )
	void Accept(DWORD dwSerial)				
	{
		m_idLocal.dwSerial = dwSerial;
	}

	//x2o계정 및 기타 접속가능한 클라이언트인지 인증을 받음( call: CMainThread::Certify_Account_Complete() )
	void PreLogin(char* pszID, char* pszPW, BYTE byLoginCode)	
	{
		m_byLoginCode = byLoginCode;
		strcpy(m_szAccountID, pszID);
		strcpy(m_szAccountPW, pszPW);
	}

	//계정서버로부터 로그인승인을 받음( call: CMainThread::pc_LoginAccountResult() )
	void Login(_GLBID* pgidGlobal, DWORD dwAccountSerial, BYTE byUserGrade)	
	{
		m_bLogin = true;
		m_bLoginCheck = true;
		memcpy(&m_gidGlobal, pgidGlobal, sizeof(_GLBID));
		m_bRegedWorld = false;
		m_bSelectWorld = false;
		m_bOverlapUser = false;
		m_dwAccountSerial = dwAccountSerial;
		m_byUserGrade = byUserGrade;
		m_dwSeletWorldCode = 0xFFFFFFFF;
	}

	//클라이언트와 접속을 끊음 받음( call: CNetworkEX::CloseClientCheck() )
	void Logoff()						
	{
		m_bLogin = false;
		m_bLoginCheck = false;
		m_idLocal.dwSerial = 0xFFFFFFFF;
		m_gidGlobal.dwSerial = 0xFFFFFFFF;
		m_dwAccountSerial = 0xFFFFFFFF;
	}

	//월드 리스트를 보냄( call: CMainThread::pc_WorldListRequest() )
	void Reged()
	{
		m_bRegedWorld = true;
	}

	//월드를 선택함( call: CMainThread::pc_SelectWorldRequest() )
	void SelectWorld(DWORD dwCode)
	{
		m_dwSeletWorldCode = dwCode;
	}

	//선택한 월드로 이동 ( call: CMainThread::pc_SelectWorldResult() )
	void EnterWorld()
	{
		m_bSelectWorld = true;
	}

	//선택한 월드로 이동 ( call: CMainThread::pc_LoginAccountResult() )
	void SetOverlap(bool bOverlap, DWORD dwAccountSerial)
	{
		m_bOverlapUser = bOverlap;
		m_dwAccountSerial = dwAccountSerial;
	}
};

//클라이언트에게 월드리스트를 지연시켜주기위함..
struct _WORLDLIST_DELAY : public _DELAY_PROCESS
{
	_WORLDLIST_DELAY() : _DELAY_PROCESS(MAX_LOGIN_CLINET_NUM, 3000) {}

	virtual void Process(DWORD dwIndex, DWORD dwSerial);
};

//외부접속 차단 예약..
struct _RESERVE_TIME
{
	bool	bReserve;
	DWORD	dwYear;
	DWORD	dwMonth;
	DWORD	dwDay;
	DWORD	dwHour;
	DWORD	dwMin;

	_RESERVE_TIME() 
	{
		bReserve = false;
		dwYear = dwMonth = dwDay = dwHour = dwMin = 0xFFFFFFFF;
	}
};

class CMainThread  
{
public:

	DWORD				m_dwWorldNum;							//등록된 월드수
	DWORD				m_dwServiceWorldNum;					//외부접속가능 월드수
	_WORLD_DATA			m_WorldData[MAX_WORLD_PER_GLOBAL];		//월드데이터

	_CLIENT_DATA		m_ClientData[MAX_LOGIN_CLINET_NUM];		//유저데이터

	DWORD				m_dwPatchIP[MAX_PATCHER_PER_GLOBAL];	//패치서버 ip
	DWORD				m_dwPatchServerNum;						//가동중인 패치서버수

	//비동기 DB 쿼리관련 맴버
	_DB_QRY_SYN_DATA	m_DBQrySynData[max_db_syn_data_num];	//쿼리관련 데이터 버퍼
	CNetIndexList		m_listDQSData;							//수행 대기 리스트
	CNetIndexList		m_listDQSDataComplete;					//수행 완료 대기리스트
	CNetIndexList		m_listDQSDataEmpty;						//빈 버퍼 리스트
	bool				m_bDQSThread;							//DQSThread().. on/off
	CFrameRate			m_DBFrameRate;							//DQSThread()의 초당회전수체크
	CLogFile			m_logDB_Thread;							//DB로그파일(스레드)

	CNetworkEX			m_Network;

	CRFWebDatabase*		m_pRFWebDB;								//클로즈테스터 WEB DB
	CRFWebDatabase*		m_pX2OnlineDB;							//x2online계정 DB

	bool				m_bAccountLogin;						//계정서버에 로그인됬는지
	bool				m_bExternalOpen;						//사내밖에서도 접속가능한 상태인지
	DWORD				m_dwCheckAccountOldTick;				//계정서버 접속을 체크한 최근 Tick Count..

	_RESERVE_TIME		m_ReserveExternalClose;					//외부접속 차단 예약..
	CNetTimer			m_tmReserveCheck;						//외부접속 차단 예약 타이머

	static _WORLDLIST_DELAY	s_WorldListDelay;					//클라이언트에게 월드리스트를 지연시켜주기

	//IP Address Filter..
	int					m_nFilterNum;							//IP필터수(사내아이피 목록)
	int					m_nAliveClient;							//현재 접속되어 있는 클라이언트수

	struct	__FILTER_INFO
	{
		BYTE	byIPAddressPoint[4];
		BYTE	byIPAddressRange;
	};
	
	__FILTER_INFO*			m_pFilterInfo;

public:

	CMainThread();
	~CMainThread();

	bool InitMainThread();
	void MainLoop();
	char* GetProcessName();
	bool CommandProcess(char* pszCmd);

	void CheckAccountLineState();
	void LoginAccount();

	//계정서버의 월드리스트 요청에대한 결과 <- account_s
	void pc_WorldListResult(DWORD dwWorldNum, _world_list_result_aclo::__list* pWorldList, BYTE byServiceWorldNum);//_world_list_result_aclo
	//월드접속가능상태 알림 <- account_s
	void pc_InformOpenWorld(DWORD dwWorldCode, DWORD dwGateIP, WORD wGatePort);	//_inform_open_world_aclo
	//월드접속불가능상태 알림 <- account_s
	void pc_InformCloseWorld(DWORD dwWorldCode);//_inform_close_world_aclo
	//월드당 접속유저수 <- account_s
	void pc_InformUserNumWorld(BYTE byServiceWorldNum, WORD* pwUserNum);//_inform_usernum_world_aclo
	//개발자계정 추가요청에대한 결과 <- account_s
	void pc_JoinAccountResult(BYTE byRetCode, _CLID* pidLocal);//_join_account_result_aclo
	//로그인요청에대한 결과 <- account_s
	void pc_LoginAccountResult(BYTE byRetCode, DWORD dwAccountSerial, BYTE byUserGrade, _CLID* pidLocal, _GLBID* pgidNewGlobal);//_login_account_result_aclo
	//월드선택에대한결과 <- account_s
	void pc_SelectWorldResult(BYTE byRetCode, _CLID* pidLocal, DWORD* pdwWorldMasterKey);//_select_world_result_aclo
	//같은ㄱ계정의 기존사용자 밀어내기 요청에대한결과 <- account_s
	void pc_PushCloseResult(BYTE byRetCode, _CLID* pidLocal);//_push_close_result_aclo
	//특정 유저의 강제종료 명령받음 <- account_s
	void pc_ForceCloseCommand(_CLID* pidLocal);//_force_close_command_aclo

	//개발자계정 추가 요청 <- user
	void pc_JoinAccountRequest(DWORD dwClientIndex, char* pszAccountID, char* pszPassword);//_join_account_request_cllo
	//로그인 요청 <- user
	void pc_LoginAccountRequest(DWORD dwClientIndex, char* pszAccountID, char* pszPassword);//_login_account_request_cllo
	//월드리스트 요청 <- user
	void pc_WorldListRequest(DWORD dwClientIndex, DWORD dwClientVersion);//_world_list_request_cllo
	//월드선택 요청 <- user
	void pc_SelectWorldRequest(DWORD dwClientIndex, WORD wWorldIndex);//_select_world_request_cllo
	//같은계정 기존사용자 밀어내기 요청 <- user
	void pc_PushCloseRequest(DWORD dwClientIndex, char* pszAccountID, char* pszPassword);//_push_close_request_cllo

	//비동기식 DB Query를 의한 맴버함수..
	//Query에 필요한 데이터를 삽입함
	bool PushDQSData(_CLID* pidLocal, BYTE byQryCase, char* pQryData, int nSize);
	//PushDQSData에 의해 추가된 쿼리를 thread로 수행
	static void DQSThread(void* pv);
	//DQSThread에 의해 쿼리가 수행된 것에대한 완료처리
	void DQSCompleteProcess();

	//데이터베이스 접근 맴버함수..
	//계정인증 체크
	BYTE db_certify_account(_qry_sheet_certify* pData);

	//DB Query 수행 완료 맴버함수..
	//계정인증 완료
	void Certify_Account_Complete(_CLID* pidLocal, BYTE byRetCode, _qry_sheet_certify* pData);

	//사내 아이피인지
	bool IsPassIPFilter(DWORD dwAcceptIP);

private:

	bool _LoadIniFile();
	bool _ObjectInit();
	bool _NetworkInit();
	bool _DatabaseInit();

	//외부접속차단 예약 체크
	void _CheckReserve();
	bool _SetReserveExternalClose(int nYear, int nMonth, int nDay, int nHour, int nMin);
	void _CheckReserveExternalClose();

	DWORD CountAliveConnection();
};

#endif
