#ifndef _AccountDBQrySyn_h_
#define _AccountDBQrySyn_h_

#include "CharacterDB.h"

#define max_db_syn_data_num		MAX_USER_PER_GLOBAL*2

struct _DB_QRY_SYN_DATA
{
	enum	{	max_data_size = 100 };
	enum	{	qry_case_join, qry_case_login, 
				log_case_usernum, log_case_push, log_case_logtime, upt_case_worldserver	};

	bool	m_bUse;
	bool	m_bLoad;

	BYTE	m_byLocalProcessCode;
	_CLID	m_idLocal;		
	BYTE	m_byQryCase;

	BYTE	m_byResult;
	char	m_sData[max_data_size];	
	
	_DB_QRY_SYN_DATA()	
	{
		m_bUse = false;
		m_bLoad = false;
	}
};

struct _qry_sheet_join
{
	char	in_szAccountID[max_id_len+1];
	char	in_szPassword[max_pw_len+1];
	DWORD	in_dwClientIP;

	_qry_sheet_join()	{	memset(this, 0, sizeof(*this));		}

	int size()
	{
		return sizeof(*this);
	}
};

struct _qry_sheet_login
{
	char	in_szAccountID[max_id_len+1];
	DWORD	in_dwClientIP;
	BYTE	in_byUserCode;
	char	in_szPassword[max_pw_len+1];//in_byUserCode가 LOGIN_CODE_DEV인경우..

	DWORD	out_dwAccountSerial;
	BYTE	out_byUserDgr;
	BYTE	out_bySubDgr;
	bool    out_bChatLock;
	char	out_szLogDate[64];

	_qry_sheet_login()	{	memset(this, 0, sizeof(*this));		}

	int size()
	{
		return sizeof(*this);
	}
};

struct _log_sheet_usernum
{
	int		in_nAveragePerHour;
	int		in_nMaxPerHour;

	_log_sheet_usernum()	{	memset(this, 0, sizeof(*this));		}

	int size()
	{
		return sizeof(*this);
	}
};

struct _log_sheet_push
{
	DWORD	in_dwAvatorSerial;
	DWORD	in_dwPushIP;
	DWORD	in_dwCloseIP;

	_log_sheet_push()	{	memset(this, 0, sizeof(*this));		}

	int size()
	{
		return sizeof(*this);
	}
};

struct _log_sheet_logtime
{
	DWORD in_dwAccountSerial;
	DWORD in_dwClientIP;
	BYTE  in_byUserGrade;
	char  in_szLoginTime[64];

	_log_sheet_logtime()	{	memset(this, 0, sizeof(*this));		}

	int size()
	{
		return sizeof(*this);
	}
};

struct _upt_case_worldserver
{
	BYTE bySerial;
	BYTE byState;

	_upt_case_worldserver()	{	memset(this, 0, sizeof(*this));		}

	int size()
	{
		return sizeof(*this);
	}
};

#endif