#ifndef _pt_account_world_h_
#define _pt_account_world_h_

#include "rf_protocol.h"
#include "CharacterDB.h"

#pragma pack(push, 1)

//OPEN
#define open_world_request_wrac 1
struct _open_world_request_wrac {

	char	szWorldName[max_world_name_len+1];
	BYTE	byRunMode;	
	DWORD	dwWorldServerIP;	

	int size()	{	return sizeof(*this);	}
};

#define open_world_success_acwr 2
struct _open_world_success_acwr {	

	BYTE	byWorldCode;
	bool	bControlOpen;
	DWORD	dwControlIP;	
	WORD	wControlPort;
	DWORD	dwControlMasterKey[CHECK_KEY_NUM];
	char	szDBName[32];	
	
	int size()	{	return sizeof(*this);	}
};

#define open_world_failure_acwr 22
struct _open_world_failure_acwr {	

	char	szErrCode[64];
	
	int size()	{	return sizeof(*this);	}
};

//START
#define start_world_request_wrac 3
struct _start_world_request_wrac {

	DWORD	dwGateIP;
	WORD	wGatePort;

	int size()	{	return sizeof(*this);	}
};

//STOP	
#define stop_world_request_wrac 4
struct _stop_world_request_wrac {

	int size()	{	return sizeof(*this);	}
};

//LOG OUT
#define logout_account_request_wrac 5
struct _logout_account_request_wrac {

	_GLBID	gidGlobal;

	int size()	{	return sizeof(*this);	}
};

#define open_control_inform_acwr 6
struct _open_control_inform_acwr {

	bool	bControlOpen;
	DWORD	dwControlIP;	
	WORD	wControlPort;
	DWORD	dwMasterKey[CHECK_KEY_NUM];

	int size()	{	return sizeof(*this);	}
};

//FORCE CLOSE
#define force_close_command_acwr 7
struct _force_close_command_acwr {

	_CLID	idLocal;	
	bool	bDirectly;
	BYTE    byKickType;		//0: kick, 1: push, 2: system 	//upt(4/20)
	DWORD	dwPushIP;		//0xFFFF

	int size()	{	return sizeof(*this);	}
};

//계정학인
#define trans_account_inform_acwr 8
struct _trans_account_inform_acwr {

	_GLBID	gidGlobal;
	DWORD	dwKey[CHECK_KEY_NUM];
	DWORD	dwClientIP;	
	DWORD	dwAccountSerial;
	char	szAccountID[max_id_len+1];	
	BYTE	byUserDgr;
	BYTE	bySubDgr;
	bool	bChatLock;	//add(4/19)

	int size()	{	return sizeof(*this);	}
};

//계정학인
#define trans_account_report_wrac 9
struct _trans_account_report_wrac {

	_GLBID	gidGlobal;
	BYTE	byRetCode;

	int size()	{	return sizeof(*this);	}
};

//ENTER	
#define enter_world_request_wrac 10
struct _enter_world_request_wrac {

	_GLBID	gidGlobal;
	_CLID	idLocal;

	int size()	{	return sizeof(*this);	}
};

#define enter_world_result_acwr 11
struct _enter_world_result_acwr {

	_CLID	idLocal;
	BYTE	byRetCode;

	int size()	{	return sizeof(*this);	}
};

//SELECT CHAR
#define select_avator_report_wrac 12
struct _select_avator_report_wrac {	

	_GLBID	gidGlobal;
	char	szCharName[max_name_len+1];
	DWORD	dwAvatorSerial;

	int size()	{	return sizeof(*this);	}
};

#define enter_lobby_report_wrac	13
struct _enter_lobby_report_wrac {	

	_GLBID	gidGlobal;

	int size()	{	return sizeof(*this);	}
};


//control_msg..

//WORLD OPER..	//단순히 월드를 열고닫는다.	
#define world_service_inform_acwr 1
struct _world_service_inform_acwr {

	bool bService;

	int size()	{	return sizeof(*this);	}
};

#define world_service_report_wrac 2
struct _world_service_report_wrac {

	bool bBefore;
	bool bAfter;

	int size()	{	return sizeof(*this);	}
};

//WORLD EXIT..	//월드에 접속한 케릭터들을 로그아웃시킨다.
#define world_exit_inform_acwr 3
struct _world_exit_inform_acwr {

	int size()	{	return sizeof(*this);	}
};

#define world_exit_report_wrac 4
struct _world_exit_report_wrac {

	int size()	{	return sizeof(*this);	}
};

//WORLD MSG..	//월드에 접속한 모든 케릭터들을 메시지를 보낸다
#define world_msg_inform_acwr 5
struct _world_msg_inform_acwr {

	BYTE byMsgSize;
	char szMsg[255];

	_world_msg_inform_acwr() { byMsgSize = 0; }

	int size(){	return sizeof(*this)-(255-byMsgSize);	}
};

#define world_msg_report_wrac 6
struct _world_msg_report_wrac {

	int size()	{	return sizeof(*this);	}
};

#define chat_lock_command_acwr	7	//채팅블럭
struct _chat_lock_command_acwr {	//add(4/19)

	_CLID	idLocal;
	WORD	wBlockTimeH;

	int size()	{	return sizeof(*this);	}
};

//sync_msg
#define world_account_ping_wrac 1
struct _world_account_ping_wrac {

	int size()	{	return sizeof(*this);	}
};

#pragma pack(pop)

#endif