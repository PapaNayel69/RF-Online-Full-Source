#ifndef _LoginDBQrySyn_h_
#define _LoginDBQrySyn_h_

#include "CharacterDB.h"

#define max_db_syn_data_num		MAX_LOGIN_CLINET_NUM*2

struct _DB_QRY_SYN_DATA
{
	enum	{	max_data_size = 100 };
	enum	{	qry_case_certify = 0	};
				

	bool	m_bUse;
	bool	m_bLoad;

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

struct _qry_sheet_certify
{
	BYTE	in_byLoginCode;
	char	in_szAccountID[max_id_len+1];
	char	in_szPassword[max_pw_len+1];
	DWORD	in_dwClientIP;

	_qry_sheet_certify()	{	memset(this, 0, sizeof(*this));		}

	int size()
	{
		return sizeof(*this);
	}
};


#endif