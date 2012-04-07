#ifndef _pt_account_control_h_
#define _pt_account_control_h_

#include "CharacterDB.h"

#pragma pack(push, 1)

#define login_control_request_coac 1
struct _login_control_request_coac {

	DWORD dwMasterKey[CHECK_KEY_NUM];

	int size() {	return sizeof(*this);	}
};

#define login_control_result_acco 2
struct _login_control_result_acco {

	bool bRet;
	WORD wRegedWorldNum;

	char	szAccountDBName[32];	//add
	char	szAccountDBMachine[32];	//add

	struct __list
	{			
		char	szWorldName[max_world_name_len+1];
		char	szDBName[32];	//add
//		char	szDBMachine[32];	//del.
	};

	__list	WorldList[MAX_WORLD_PER_GLOBAL];

	_login_control_result_acco() { wRegedWorldNum = 0; }

	int size() {	return sizeof(*this)-sizeof(__list)*(MAX_WORLD_PER_GLOBAL-wRegedWorldNum);	}

};

//WORLD OPER..	//단순히 월드를 열고닫는다.	
#define world_service_request_coac 5
struct _world_service_request_coac {

	_CLID	idClient;
	BYTE byWorldCode;
	bool bService;

	int size()	{	return sizeof(*this);	}
};

//WORLD EXIT..	//월드에 접속한 케릭터들을 로그아웃시킨다.
#define world_exit_request_coac 6
struct _world_exit_request_coac {

	_CLID	idClient;
	BYTE byWorldCode;

	int size()	{	return sizeof(*this);	}
};

//WORLD MSG..	//월드에 접속한 모든 케릭터들을 메시지를 보낸다
#define world_msg_request_coac 7
struct _world_msg_request_coac {

	_CLID	idClient;
	BYTE byWorldCode;
	BYTE byMsgSize;
	char szMsg[255];

	_world_msg_request_coac() { byMsgSize = 0; }

	int size(){	return sizeof(*this)-(255-byMsgSize);	}
};

#define	user_position_request_coac 8
struct	_user_position_request_coac {

	WORD	wClientIndex;
	char	szAccountID[max_name_len+1];

	int size()	{	return sizeof(*this);	}
};

#define	user_position_result_acco 9
struct	_user_position_result_acco {
	
	WORD	wClientIndex;
	char	szAccountID[max_name_len+1];
	char	szWorldName[max_world_name_len+1];
	char	szAvatorName[max_name_len+1];

	int size()	{	return sizeof(*this);	}
};

#define user_block_command_request_coac 10
struct	_user_block_command_request_coac {

	BYTE	byBlockKind;					// 블럭종류 ( 0: 계정블럭, 1:채팅금지 );
	WORD	wBlockHour;
	char	szAccountID[max_name_len+1];
	int size()	{	return sizeof(*this);	}
};

#define	login_server_stat_request_coac 11
struct	_login_server_stat_request_coac {

	BYTE	byStat;
	WORD	wClientIndex;

	int size()	{	return sizeof(*this);	}
};

#define	login_server_stat_result_acco 12
struct	_login_server_stat_result_acco {

	BYTE	byRet;
	WORD	wClientIndex;

	int size()	{	return sizeof(*this);	}
};

#define world_inf_inform_acco 100
struct _world_inf_inform_acco {

	BYTE		byWorldNum;
	_WORLD_INFO	WorldInf[MAX_WORLD_PER_GLOBAL];	

	_world_inf_inform_acco() { byWorldNum = 0; }

	int size(){	return sizeof(*this)-sizeof(_WORLD_INFO)*(MAX_WORLD_PER_GLOBAL-byWorldNum);	}
};

#define error_msg_inform_acco 101
struct _error_msg_inform_acco {

	_CLID	idClient;
	char	szMessage[64+1];

	_error_msg_inform_acco() { szMessage[0] = NULL; }

	int size(){	return sizeof(*this)-(64-strlen(szMessage));	}
};

#pragma pack(pop)

#endif
