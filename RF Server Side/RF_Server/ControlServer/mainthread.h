#ifndef _mainthread_h_
#define _mainthread_h_

#include <process.h>
#include "NetworkEX.h"
#include "CharacterDB.h"
#include "AnalysisAvatorData.h"
#include "RFAccountDatabase.h"
#include "RFWorldDatabase.h"

struct _ADMIN_DATA		//유저데이터
{
	bool	m_bLogin;
	_CLID	m_idLocal;
	char	m_szAccountID[max_id_len+1];
	char	m_szPassword[max_pw_len+1];
	BYTE	m_byGrade;

	void InitData(DWORD dwIndex)
	{
		m_bLogin = false;
		m_idLocal.wIndex = (WORD)dwIndex;
		m_idLocal.dwSerial = 0xFFFFFFFF;
		m_byGrade = 0;
		m_szAccountID[0] = NULL;
		m_szPassword[0] = NULL;
	}

/*	void PreSetData(char* pszAccountID, char* pszPassword, DWORD dwLocalSerial)
	{
		strcpy(m_szAccountID, pszAccountID);
		strcpy(m_szPassword, pszPassword);
		m_idLocal.dwSerial = dwLocalSerial;
	}*/

	void SetAdminData( BYTE byGrade )
	{
		m_bLogin = true;
		m_byGrade = byGrade;
	}

	void ReleaseData()
	{
		m_bLogin = false;
		m_byGrade = 0;
		m_idLocal.dwSerial = 0xFFFFFFFF;
		m_szAccountID[0] = NULL;
		m_szPassword[0] = NULL;
	}
};

struct _WORLD_DATA
{
	bool	m_bOpen;
	bool	m_bService;
	DWORD	m_dwCode;

	char	m_szName[32];
	char	m_szDBName[32];			
	char	m_szDBMachine[32];	

	int		m_nUserNum;

	void InitWorld(DWORD dwIndex)
	{
		m_bOpen = false;
		m_bService = false;
		m_dwCode = dwIndex;
		
		m_szName[0] = NULL;
		m_szDBName[0] = NULL;			
		m_szDBMachine[0] = NULL;	

		m_nUserNum = 0;
	}

	void SetWorld(bool bOpen, bool bService, char* pszWorldName, char* pszDBName)
	{
		m_bOpen = bOpen;
		m_bService = bService;
		strcpy(m_szName, pszWorldName);
		strcpy(m_szDBName, pszDBName);
	}

	void UpdateWorldState(bool bOpen, bool bService, int nUserNum)
	{
		m_bOpen = bOpen;
		m_bService = bService;
		m_nUserNum = nUserNum;
	}
};

class CMainThread  
{
public:

	enum			{   connect_term_account = 5000	};	  
		
	CNetworkEX			m_Network;
	CRFAccountDatabase*	m_pAccountDB;
	CRFWorldDatabase*	m_pWorldDB;
	
	_ADMIN_DATA			m_AdminData[MAX_ADMIN_PER_GLOBAL];
	_WORLD_DATA			m_WorldSetData[MAX_WORLD_PER_GLOBAL];
	DWORD				m_dwMasterKey[KEY_NUM];

	WORD				m_wRegedWorldNum;
	char				m_szAccountDBMachine[128];			//add
	char				m_szAccountDBName[128];				//add
	bool				m_bMainThread;
	bool				m_bLoginAccount;
	
	CRecordData			m_tblItemData[item_tbl_num];
	CItemUpgradeTable	m_tblItemUpgrade;

	CNetTimer			m_tmrConnectAccount;
	CNetTimer			m_tmrConnectLogSpy;
	DWORD				m_dwAccountIP;
	DWORD				m_dwLogSpyIP;

	CLogFile			m_logMain;

private:


public:

	CMainThread();
	~CMainThread();

	bool InitMainThread();
	bool _DatabaseInit(char* szOdbcName);
	void MainLoop();
	char* GetProcessName();
	bool CommandProcess(char* pszCmd);

	bool CheckMasterKey(DWORD* pdwMasterKey);
	void SendErrMsgToClient(int n, const char *fmt, ...);

	void LoginAccountServer();
	bool db_Load_Avator(int nWorldCode, char* pszCharName, _AVATOR_DATA* OUT pData, char* OUT pAccount, char* OUT pszErrMsg);

private:

	bool _LoadIniFile();
	bool _ObjectInit();
	bool _NetworkInit();
	bool _ThreadInit();

	bool _db_Load_Base( char* pszCharName, _AVATOR_DATA* OUT poutCon, char* OUT pAccount, char* pszErrMsg);
	bool _db_Load_General( DWORD dwSerial, _AVATOR_DATA* OUT poutCon, char* pszErrMsg );
	bool _db_Load_Inven( DWORD dwSerial, int nBagNum, _INVEN_DB_BASE* OUT poutCon, char* pszErrMsg );

	void _ConnectingAccountServer();
	void _ConnectingLogSpy();
};

#endif
