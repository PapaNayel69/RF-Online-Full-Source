#ifndef _pt_account_login_h_
#define _pt_account_login_h_

#include "CharacterDB.h"

#pragma pack(push, 1)

//JOIN ACCOUNT
#define join_account_request_loac 1
struct _join_account_request_loac {

	_CLID	idLocal;
	char	szAccountID[max_id_len+1];
	char	szPassword[max_pw_len+1];
	DWORD	dwClientIP;	

	int size()	{	return sizeof(*this);	}
};

#define join_account_result_aclo 2
struct _join_account_result_aclo {

	_CLID	idLocal;
	BYTE	byRetCode;

	int size()	{	return sizeof(*this);	}
};

//LOGIN ACCOUNT
#define login_account_request_loac 3
struct _login_account_request_loac {

	_CLID	idLocal;
	BYTE	byUserCode;	
	char	szAccountID[max_id_len+1];
	char	szPassword[max_pw_len+1];
	DWORD	dwClientIP;	

	int size()	{	return sizeof(*this);	}
};

#define login_account_result_aclo 4
struct _login_account_result_aclo {

	_CLID	idLocal;
	BYTE	byRetCode;
	_GLBID	gidNewGlobal;
	DWORD	dwAccountSerial;
	BYTE	byUserGrade;	//add(12/30)

	int size()	{	return sizeof(*this);	}
};

//SELECT WORLD	
#define select_world_request_loac 5
struct _select_world_request_loac {

	_GLBID	gidGlobal;
	WORD	wWorldIndex;
	DWORD	dwClientIP;	

	int size()	{	return sizeof(*this);	}
};

#define select_world_result_aclo 6
struct _select_world_result_aclo {

	_CLID	idLocal;
	BYTE	byRetCode;
	DWORD	dwWorldMasterKey[CHECK_KEY_NUM];

	int size()	{	return sizeof(*this);	}
};

//PUSH CLOSE
#define push_close_request_loac 7
struct _push_close_request_loac {

	_CLID	idLocal;
	BYTE	byUserCode;	
	DWORD	dwAccountSerial;
	DWORD	dwClientIP;	

	int size()	{	return sizeof(*this);	}
};

#define push_close_result_aclo 8
struct _push_close_result_aclo {

	_CLID	idLocal;
	BYTE	byRetCode;

	int size()	{	return sizeof(*this);	}
};

//CLOSE
#define logout_account_request_loac 9
struct _logout_account_request_loac {

	_GLBID	gidGlobal;

	int size()	{	return sizeof(*this);	}
};

//WORLD LIST
#define world_list_request_loac 10
struct _world_list_request_loac {

	int size()	{	return sizeof(*this);	}
};

#define world_list_result_aclo 11
struct _world_list_result_aclo {

	struct __list
	{
		bool	bOpen;
		char	szWorldName[max_world_name_len+1];
		DWORD	dwGateIP;
		WORD	wGatePort;
	};

	BYTE	byServiceWorldNum;
	BYTE	byWorldNum;
	__list	WorldList[MAX_WORLD_PER_GLOBAL];

	_world_list_result_aclo() {	byWorldNum = 0; }

	int size()	{	
		return sizeof(*this)-sizeof(__list)*(MAX_WORLD_PER_GLOBAL-byWorldNum);	}
};

#define	login_server_stat_request_aclo 12
struct	_login_server_stat_request_aclo {

	BYTE	byStat;
	WORD	wClientIndex;

	int size()	{	return sizeof(*this);	}
};

#define	login_server_stat_result_loac 13
struct	_login_server_stat_result_loac {

	BYTE	byRet;
	WORD	wClientIndex;

	int size()	{	return sizeof(*this);	}
};

//INFORM OPEN WORLD
#define inform_open_world_aclo 100
struct _inform_open_world_aclo {

	DWORD	dwWorldCode;
	DWORD   dwGateIP;
	WORD	wGatePort;

	int size()	{	return sizeof(*this);	}
};

//INFORM CLOSE WORLD
#define inform_close_world_aclo 101
struct _inform_close_world_aclo {

	DWORD	dwWorldCode;

	int size()	{	return sizeof(*this);	}
};

//INFORM USER NUM IN WORLD
#define inform_usernum_world_aclo 102
struct _inform_usernum_world_aclo {

	BYTE	byServiceWorldNum;
	WORD	wUserNum[MAX_WORLD_PER_GLOBAL];

	int size()	{	return sizeof(*this)-sizeof(WORD)*(MAX_WORLD_PER_GLOBAL-byServiceWorldNum);	}
};

//FORCE CLOSE
#define force_close_command_aclo 200
struct _force_close_command_aclo {

	_CLID	idLocal;	

	int size()	{	return sizeof(*this);	}
};

#pragma pack(pop)

#endif