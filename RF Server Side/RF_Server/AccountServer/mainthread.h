#ifndef _mainthread_h_
#define _mainthread_h_

#include <process.h>
#include "NetworkEX.h"
#include "RFAccountDatabase.h"
#include "CharacterDB.h"
#include "AccountDBQrySyn.h"
#include "MyClassUtil.h"
#include "ConnNumPHMgr.h"

struct _ACCOUNT_DATA
{
	bool	m_bLogin;					//로그인중인가..		
	bool	m_bWait;					//로그인서버에서 월드서버로 이동중인가..
	DWORD	m_dwIndex;					//해당구조체배열의 인덱스	
	BYTE	m_byUserCode;				//유저테이블코드..	LOGIN_CODE_USER이면 tbl_rfuser, LOGIN_CODE_DEV이면 tbl_rfdev..
	char	m_szAccountID[max_id_len+1];//계정
	BYTE	m_byUserDgr;				//유저등급
	BYTE	m_bySubDgr;					//유저서브등급
	bool    m_bChatLock;				//채팅금지.. add
	DWORD	m_dwClientIP;				//접속한 클라이언트의 ip주소
	DWORD	m_dwAccountSerial;			//계정시리얼
	DWORD	m_dwMasterKey[CHECK_KEY_NUM];		//로그인서버에서 월드서버로 로그인할때 인증받을 키값
	DWORD	m_dwWorldCode;				//현재 접속중인 월드코드(로그인 서버에 있으면... 0xFFFFFFFF)
	BYTE	m_byLoginServerIndex;		//로그인서버 인덱스
	_GLBID	m_gidGlobal;				//서버세트 전체의 ID
	_CLID	m_idLocal;					//로그인서버 또는 월드서버등 하위서버상의 로컬 ID

	BYTE	m_byLoginServerIndexPushWaiter;		//해당계정을 밀어내려는 유저의 로그인서버인덱스
	_CLID	m_idLocalPushWaiter;		//해당계정을 밀어내려는 유저의 로그인서버상의 로컬 ID
	DWORD	m_dwPushCloseWaitTime;		//해당계정을 밀어내려는 유저의 요청한 시간(Tick Count)
	DWORD	m_dwPusherIP;				//해당계정을 밀어내려는 유저의 클라이언트의 IP주소

	DWORD	m_dwWorldSelectTime;		//월드를 선택한 시간(허용된 시간내에 선택한 월드로 들어온지 체크할때 사용)
	char	m_szLoginTime[64];			//접속한 시간을 저장
	DWORD	m_dwLastDestroyTime;		//최근에 접속을 로그아웃한 시간(Tick count)(한번 사용한 계정메모리공간을 1분이내에는 사용하지 않기위해)

	char	m_szAvatorName[max_name_len+1];
	DWORD	m_dwAvatorSerial;

	//프로그램 시작할때 한번 호출해서 초기화함
	void InitData(DWORD dwIndex)
	{
		m_bLogin = false;
		m_bWait = false;
		m_dwIndex = dwIndex;
		m_byLoginServerIndex = 0xFF;
		m_byLoginServerIndexPushWaiter = 0xFF;
		m_idLocalPushWaiter.wIndex = 0xFFFF;
		m_idLocalPushWaiter.dwSerial = 0xFFFFFFFF;
		m_dwWorldCode = 0xFFFFFFFF;
		m_byUserDgr = USER_DEGREE_STD;
		m_bySubDgr = 0;
		m_dwLastDestroyTime = 0;
		memset( m_szAvatorName, 0, max_name_len+1 );
		m_dwAvatorSerial = 0xFFFF;
	}

	//로그인서버를 통해 처음 서버에 접속했을때 구조체세팅
	void SetData(BYTE byUserCode, char* pszAccountID, DWORD dwAccountSerial, BYTE byUserDgr, BYTE bySubDgr, DWORD dwClientIP, BYTE byLoginServerIndex, _CLID* pidLocal, DWORD dwGlobalSerial, char* pszLoginTime)
	{
		m_bLogin = true;
		m_bWait = false;
		m_byUserCode = byUserCode;
		strcpy(m_szAccountID, pszAccountID);
		m_dwAccountSerial = dwAccountSerial;
		m_byUserDgr = byUserDgr;
		m_bySubDgr = bySubDgr;
		m_dwClientIP = dwClientIP;
		m_byLoginServerIndex = byLoginServerIndex;
		memcpy(&m_idLocal, pidLocal, sizeof(_CLID));
		m_gidGlobal.dwSerial = dwGlobalSerial;
		m_gidGlobal.dwIndex = m_dwIndex;
		m_idLocalPushWaiter.wIndex = 0xFFFF;
		m_dwWorldSelectTime = 0;
		m_dwWorldCode = 0xFFFFFFFF;		
		m_bChatLock = false;
		strcpy(m_szLoginTime, pszLoginTime);
	}

	//로그아웃했을때 초기화
	void ReleaseData()
	{
		m_bLogin = false;
		m_bWait = false;
		m_byLoginServerIndex = 0xFF;
		m_byLoginServerIndexPushWaiter = 0xFF;
		m_idLocalPushWaiter.wIndex = 0xFFFF;
		m_idLocalPushWaiter.dwSerial = 0xFFFFFFFF;
		m_dwWorldCode = 0xcccccccc;
		m_dwLastDestroyTime = timeGetTime();
		memset( m_szAvatorName, 0, max_name_len+1 );
		m_dwAvatorSerial = 0xFFFF;
	}

	//밀어내려는 유저의 세팅
	void SetPushWaiter(BYTE byLoginServerIndexPushWaiter, _CLID* pidWaiter, DWORD dwPusherIP)
	{
		m_dwPusherIP = dwPusherIP;
		m_byLoginServerIndexPushWaiter = byLoginServerIndexPushWaiter;
		memcpy(&m_idLocalPushWaiter, pidWaiter, sizeof(_CLID));
		m_dwPushCloseWaitTime = timeGetTime();
	}

	//마스터키 생성
	void GenerateMasterKey()
	{
		for(int i = 0; i < CHECK_KEY_NUM; i++)
		{
			DWORD dwR1 = ::rand();
			m_dwMasterKey[i] = (dwR1<<16) + ::rand();
		}
	}

	//로그인서버에서 플레이할 월드를 선택
	void SelectWorld(DWORD dwWorldCode)
	{
		m_dwWorldSelectTime = timeGetTime();
		m_dwWorldCode = dwWorldCode;
		m_bWait = true;
	}

	//선택한 월드에 로그인
	void EnterWorld(DWORD dwWorldCode, _CLID* pidLocal)
	{
		memcpy(&m_idLocal, pidLocal, sizeof(_CLID));
		m_dwWorldCode = dwWorldCode;
		m_bWait = false;
	}

	void SetChatLock(bool bChatLock)
	{
		m_bChatLock = bChatLock;
	}
};

struct _WORLD_DATA
{
	bool	m_bOpen;			//서비스가능한상태로서 월드에 연결중인가
	bool	m_bService;			//현재 월드에 연결중이며 서버스상태인지
	BYTE	m_byRunMode;		//월드가 서비스모드인지..개발모드인지..(WORLD_RUN_MODE_SERVICE, WORLD_RUN_MODE_TEST)
	
	DWORD	m_dwCode;			//월드 리스트상의 인덱스
	char	m_szName[32];		//월드이름
	char	m_szDBName[32];		//월드DB 명		
	
	DWORD	m_dwSocketIndex;	//계정서버에 접속된 소켓 인덱스
	DWORD	m_dwWorldIP;
	DWORD	m_dwGateIP;			//유저가 월드서버에 접속가능한 IP
	WORD	m_wGatePort;		//유저가 월드서버에 접속가능한 Port
	int		m_nLoginUserNum;	//현재 해당월드에 접속중인 유저수
	
	DWORD	m_dwLastPingTime;	//주기적으로 월드서버에서 체크메시지를 받을때마다 Tick Count를 저장

	_WORLD_DATA() 
	{		
		m_bOpen = false; 
		m_bService = false;	
		m_nLoginUserNum = 0;
		m_szName[0] = NULL;
	}

	//프로그램 시작할때 한번 Ini파일등을 읽은후 호출해서 초기화함
	void InitWorld(DWORD dwWorldCode, char* pszWorldName, DWORD dwWorldIP, char* pdzDBName)
	{
		m_dwCode = dwWorldCode;
		strcpy(m_szName, pszWorldName);
		strcpy(m_szDBName, pdzDBName);
//		strcpy(m_szDBMachine, pszDBMachine);
		m_dwWorldIP = dwWorldIP;

		m_nLoginUserNum = 0;
		m_dwSocketIndex = 0xFFFFFFFF;	
	}

	//월드서버가 계정서버에 로그인함
	void OpenWorld(DWORD dwSocketIndex, BYTE byRunMode)
	{
		m_byRunMode = byRunMode;
		m_dwSocketIndex = dwSocketIndex;
		m_nLoginUserNum = 0;

		m_bOpen = true;
		m_bService = false;
	}

	//월드서버가 계정서버에 로그아웃함
	void CloseWorld()
	{
		m_bOpen = false;
		m_bService = false;
		m_dwSocketIndex = 0xFFFFFFFF;	
		m_nLoginUserNum = 0;
	}

	//월드서버의 서비스시작
	void ServiceStart(DWORD dwGateIP, WORD wGatePort)
	{
		m_dwGateIP = dwGateIP;
		m_wGatePort = wGatePort;
	
		m_bService = true;
		m_dwLastPingTime = timeGetTime();
	}

	//월드서버의 서비스 재시작
	void ServiceReStart()
	{
		m_bService = true;
		m_dwLastPingTime = timeGetTime();
	}

	//월드서버의 서비스중지
	void ServiceStop()
	{
		m_bService = false;
	}

};

struct _LOBBY_DATA
{
	bool	m_bOpen;		//서비스연결 가능상태인지..

	_LOBBY_DATA()
	{
		m_bOpen = false;
	}

	void OpenLobby()
	{
		m_bOpen = true;
	}

	void CloseLobby()
	{
		m_bOpen = false;
	}
};

struct __cnt_per_h		//동접수를 계산하기위한 구조체
{
	int		nUserCumPerMin;		//한시간동안의 매분당 누적 유저수
	DWORD	dwCounting;			//한시간동안 누적카운팅.. 
	int		nMaxUserNum;		//한시간동안 최대유저수	

	__cnt_per_h() {	Init();	}

	void Init()		
	{	
		memset(this, 0, sizeof(*this));	
	}

	int GetAverageUserNumPerHour()	
	{
		if(dwCounting == 0)
			return 0;

		return nUserCumPerMin/dwCounting;	//시간당 평균유저수 = 한시간동안의 매분당 누적 유저수/한시간동안 누적카운팅
	}
};

class CMainThread  
{
public:

	enum	{	max_control_socket = 10	};
	
	CNetworkEX			m_Network;

	CRFAccountDatabase*	m_pAccountDB;

	_ACCOUNT_DATA		m_AccountData[MAX_USER_PER_GLOBAL];		//계정데이터
	_WORLD_DATA			m_WorldSetData[MAX_WORLD_PER_GLOBAL];	//월드데이터
	_LOBBY_DATA			m_LobbyData[MAX_LOGINER_PER_GLOBAL];	//로그인서버데이터	
	
	//비동기 DB 쿼리관련 맴버
	_DB_QRY_SYN_DATA	m_DBQrySynData[max_db_syn_data_num];	//쿼리관련 데이터 버퍼
	CNetIndexList		m_listDQSData;							//수행 대기 리스트
	CNetIndexList		m_listDQSDataComplete;					//수행 완료 대기리스트
	CNetIndexList		m_listDQSDataEmpty;						//빈 버퍼 리스트
	bool				m_bDQSThread;							//DQSThread().. on/off
	CFrameRate			m_DBFrameRate;							//DQSThread()의 초당회전수체크
	
	bool				m_bOperControl;							//컨트롤서버 로그인 여부		
	bool				m_bExternalLoginOpen;					//외부접속허용상태인지

	DWORD				m_dwWorldSetNum;						//등록된 월드수
	DWORD				m_dwServiceSetNum;						//등록된월드중 외부서비스 월드수
	char				m_szAccountDBMachine[128];				//계정DB IP
	char				m_szAccountDBName[128];					//계정DB이름	

	CNetTimer			m_tmCheckAccount;						//계정체크 타이밍(1초마다)
	CNetTimer			m_tmInformWorldInf;						//월드정보 알림 타이밍(5초마다)
	CNetTimer			m_tmUpdateWorldServerState;				//월드서버상태 업데이트 타이밍(1분마다)
//	CNetTimer			m_tmCountingLoginNum;					//동접수 체크 타이밍(1분마다)

	CLogFile			m_logMain;								//에러로그파일
	DWORD				m_dwSerialCounter;						//Golbal ID Serial를 매기기위한 카운팅..

	int					m_nControlSocketIndex;

private:

	DWORD				m_dwControlMasterKey[CHECK_KEY_NUM];

	//시간별 동접수를 계산하기위한 맴버
	CConnNumPHMgr		m_MgrConnNum;

public:

	CMainThread();
	~CMainThread();

	//자동호출 맴버함수..
	bool InitMainThread();
	void ReleaseMainThread();
	char* GetProcessName();
	void MainLoop();					//메인루프
	bool CommandProcess(char* pszCmd);	//입력을 직접받은명령을 수행

	//로그인서버와의 연결이 끊어짐
	void CloseLoginServer(DWORD dwLoginServerIndex);	
	//월드서버와의 연결이 끊어짐
	void CloseWorldServer(DWORD dwWorldSocketIndex);
	//컨트롤서버의 Close를 모든 월드에 알림
	void CloseControlServer();
	//컨트롤서버의 Connect를 모든 월드에 알림
	void LoginControlServer();

	//새로운 RF개발자유저를 추가(x2online계정과는 무관) <- login_s
	void pc_JoinAccount(BYTE byLoginServerIndex, _CLID* pidLocal, char* pszAccountID, char* pszPassword, DWORD dwClientIP);//join_account_request_loac
	//유저의 로그인 요청	<- login_s
	void pc_LoginAccount(BYTE byLoginServerIndex, BYTE byUserCode, _CLID* pidLocal, char* pszAccountID, char* pszPassword, DWORD dwClientIP);//login_account_request_loac
	//먼저 접속한 계정을 밀어내기 요청 <- login_s
 	void pc_PushClose(BYTE byLoginServerIndex, BYTE byUserCode, _CLID* pidLocal, DWORD dwAccountSerial, DWORD dwPusherIP);//push_close_request_loac
	//유저의 로그아웃을 요청 <- login_s, world_s
	void pc_CloseAccount(_GLBID* pgidGlobal);//logout_account_request_loac, logout_account_request_wrac
	//로그인서버의 초기가동시 등록된 월드리스트를 요청함 <-loing_s
	void pc_WorldListRequest(BYTE byLoginServerIndex);//world_list_request_loac
	//유저의 월드선택을 요청 <- login_s
	void pc_SelectWorld(BYTE byLoginServerIndex, _GLBID* pgidGlobal, WORD wWorldCode, DWORD dwClientIP);//select_world_request_loac
	//월드서버의 계정서버로의 로그인을 요청 <- world_s
	void pc_OpenWorld(DWORD dwSocketIndex, char* pszWorldName, DWORD dwWorldServerIP, BYTE byRunMode);//open_world_request_wrac
	//계정서버로 로그인된 월드서버의 서비스시작 요청 (이것이 호출완료된 월드만이 서비스가능) <- world_s
	void pc_StartWorld(DWORD dwSocketIndex, DWORD dwGateIP, WORD wGatePort);//start_world_request_wrac
	//계정서버로 로그인된 월드서버의 서비스중지 요청 (이것이 호출완료된 월드만이 서비스가능) <- world_s
	void pc_StopWorld(DWORD dwSocketIndex);//stop_world_request_wrac
	//월드를 선택한 유저가 선택한 월드를 접속했을때 인증요청 <- world_s
	void pc_EnterWorldRequest(DWORD dwSocketIndex, _GLBID* pgidGlobal, _CLID* pidLocal);//enter_world_request_wrac
	//선택한 월드에 다음 접속할 유저의 정보(ip, serial, key)를 알려주고 받는 응답 <- world_s
	void pc_TransAccountReport(DWORD dwWorldSocketIndex, _GLBID* pgidGlobal, BYTE byRetCode);//trans_account_report_wrac
	//월드서버로부터 직접 서비스 시작또는 중지를 보고받음 <- world_s
	void pc_WorldServiceReport(DWORD dwWorldSocketIndex, bool bBefore, bool bAfter);//world_service_report_wrac
	//주기적으로 월드서버로부터 받는 체크(월드의 정상적인 가동..block, down상태등을 파악) <-world_s
	void pc_PingWorld(DWORD dwWorldSocketIndex);//world_account_ping_wrac
	//컨트롤서버의 로그인요청 <- control_s
	void pc_LoginControlRequest(DWORD* pdwMasterKey);//login_control_request_coac
	//컨트롤서버로부터 월드의 서비스 시작또는 중지를 요청받음 <- control_s
	void pc_WorldServiceRequest(DWORD dwWorldCode, bool bService, _CLID* pidLocal);//world_service_request_coac
	//컨트롤서버로부터 특정월드에 접속한 모든유저의 강제 로그아웃을 요청받음 <- control_s
	void pc_WorldUserExitRequest(DWORD dwWorldCode, _CLID* pidLocal);//world_exit_request_coac
	//컨트롤서버로부터 특정월드에 접속한 모든유저에게 운영메시지를 보낼것을 요청받음 <- control_s
	void pc_WorldMsgRequest(DWORD dwWorldCode, BYTE byMsgSize, char* pszMsg, _CLID* pidLocal);//world_msg_request_coac
	//유저의 위치에 대한 응답
	void pc_UserPositionResult( WORD wClientIndex, char* szUserAccount, char* szWorldName, char* szAvatorName );
	//캐릭터를 선택했음
	void pc_UserSelectAvatorReport( _GLBID gidGlobal, char* szCharName, DWORD dwAvatorSerial );
	//로비로 나갔음(캐릭터 선택화면)
	void pc_UserEnterLobby( _GLBID gidGlobal );
	//블럭 요청
	void pc_UserBlockRequest( char* szUserAccount, BYTE byBlockKind, WORD wBlockHour );
	
	//현재 접속자수를 계산
	DWORD cmd_LoginNum();
	//유저의 강제 Logout
	void ForceCloseAccount(_ACCOUNT_DATA* pAccount, bool bDirectly, bool bPushClose, DWORD dwPushIP);
	// 컨트롤서버의 요청에대한 실패메시지
	void SendErrMsgToControl(_CLID* pidLocal, const char *fmt, ...);

	//비동기식 DB Query를 의한 맴버함수..
	//Query에 필요한 데이터를 삽입함
	bool PushDQSData(BYTE byLocalProcessCode, _CLID* pidLocal, BYTE byQryCase, char* pQryData, int nSize);
	//PushDQSData에 의해 추가된 쿼리를 thread로 수행
	static void DQSThread(void* pv);
	//DQSThread에 의해 쿼리가 수행된 것에대한 완료처리
	void DQSCompleteProcess();

	//데이터베이스 접근 맴버함수..
	//개발자 계정을 만든다(!%* 계정)
	BYTE db_join_dev(_qry_sheet_join* pData);
	//계정로그인 인증
	BYTE db_login_account(_qry_sheet_login* pData);
	//시간대별 동시접속자수를 기록
	BYTE db_log_usernum(_log_sheet_usernum* pData);
	//게임중인 계정을 다른IP에서 밀어낼때 기록
	BYTE db_log_pushclose(_log_sheet_push* pData);
	//유저의 게임에 접속한 기록
	BYTE db_log_logtime(_log_sheet_logtime* pData);
	//월드서버의 상태기록
	BYTE db_upt_worldServer(_upt_case_worldserver* pData);

	//DB Query 수행 완료 맴버함수..
	//개발자 계정 생성 완료
	void Join_Account_Complete(BYTE byLoginServerIndex, _CLID* pidLocal, BYTE byRetCode);
	//계정인증 완료
	void Login_Account_Complete(BYTE byLoginServerIndex, _CLID* pidLocal, BYTE byRetCode, _qry_sheet_login* pData);

	bool SearchUserPosition( char* IN szUserAccount, char* OUT szWorldName, char* OUT szAvatorName );


private:

	//주기적으로 로그인서버와 컨트롤서버에 각 월드의현재상태정보를 알림
	void WorldInfInform();
	//월드서버로부터 주기적으로 체크메시지를 받은것에대한 처리
	void CheckWorldPing();

	//가동초기에 각종데이터를 로드 및 초기화
	bool _LoadData();//	".\\Initialize\\AccountSystem.ini"
	//네트워크모듈 초기화
	bool _NetworkInit();
	//데이터베이스 연결 초기화
	bool _DatabaseInit();

	//대기시간안에 로그인서버에서 월드서버로 이동을 완료했는지 체크
	void CheckAccount();
	//월드파트소켓의 인덱스로부터 월드구조체의 포인터를 얻어옴
	_WORLD_DATA* GetWorldPtrFormSocketIndex(DWORD dwSocketIndex);
	//계정이름으로 계정구조체의 포인터를 얻어옴
	_ACCOUNT_DATA* GetPtrLoginAccount(DWORD dwAccountSerial, BYTE byUserCode);
	//빈계정을 얻어옴	
	_ACCOUNT_DATA* GetEmptyAccount();
	//접속계정정보를 지운다
	bool _ReleaseAccount(_ACCOUNT_DATA* pAccount, char* pszCallFunc);

	void UpdateWorldServerState();
};

//계정 스트링이 유효한 문자로 조합된건지 학인 (숫자, 영문대소문자)
bool g_IsSuitAccountID(char* pszAccountID);


#endif
