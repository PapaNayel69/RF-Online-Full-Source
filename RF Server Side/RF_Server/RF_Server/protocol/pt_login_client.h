#ifndef _pt_login_client_h_
#define _pt_login_client_h_

#pragma pack(push, 1)

//
//account_msg..
//

//JOIN ACCOUNT
#define join_account_request_cllo 1
struct _join_account_request_cllo {

	char szAccountID[max_id_len+1];
	char szPassword[max_pw_len+1];

	int size()	{	return sizeof(*this);	}
};

#define join_account_result_locl 2
struct _join_account_result_locl {

	BYTE	byRetCode;

	int size()	{	return sizeof(*this);	}
};

//LOGIN ACCOUNT
#define login_account_request_cllo 3
struct _login_account_request_cllo {

	char szAccountID[max_id_len+1];
	char szPassword[max_pw_len+1];

	int size()	{	return sizeof(*this);	}
};

#define login_account_result_locl 4
struct _login_account_result_locl {

	BYTE	byRetCode;
	DWORD	dwAccountSerial;	
	DWORD	dwPatchServerIP;
	WORD	wPatchServerPort;

	int size()	{	return sizeof(*this);	}
};

//REGED WORLD LIST
#define world_list_request_cllo 5
struct _world_list_request_cllo {

	DWORD	dwClientVersion;

	int size()	{	return sizeof(*this);	}
};

#define world_list_result_locl 6
struct _world_list_result_locl {	

	BYTE	byRetCode;

	//월드수[1] {	오픈[1], 이름크기[1], 이름[..(NULL포함)]	}..

	WORD	wDataSize;
	char	sListData[0x0FFF];

	int size()	{	
		return sizeof(*this)-sizeof(char)*(0x0FFF-wDataSize);	}		
};

#define world_user_inform_locl 66
struct _world_user_inform_locl {	

	BYTE	byServiceWorldNum;
	WORD	wUserNum[MAX_WORLD_PER_GLOBAL];

	int size()	{	return sizeof(*this)-sizeof(WORD)*(MAX_WORLD_PER_GLOBAL-byServiceWorldNum);	}
};

//SELECT WORLD	
#define select_world_request_cllo 7
struct _select_world_request_cllo {

	WORD	wWorldIndex;

	int size()	{	return sizeof(*this);	}
};

#define select_world_result_locl 8
struct _select_world_result_locl {

	BYTE	byRetCode;
	DWORD   dwWorldGateIP;
	WORD	wWorldGatePort;
	DWORD	dwWorldMasterKey[CHECK_KEY_NUM];

	int size()	{	return sizeof(*this);	}
};

//PUSH CLOSE
#define push_close_request_cllo 9
struct _push_close_request_cllo {

	char szAccountID[max_id_len+1];
	char szPassword[max_pw_len+1];
	DWORD	dwAccountSerial;

	int size()	{	return sizeof(*this);	}
};

#define push_close_result_locl 10
struct _push_close_result_locl {

	BYTE	byRetCode;

	int size()	{	return sizeof(*this);	}
};

//SERVER NOTIFY CODE
#define server_notify_inform_locl 11
struct _server_notify_inform_locl {

	WORD wMsgCode;

	int size()	{	return sizeof(*this);	}
};

#define	crypty_key_request_cllo 12
struct _crypty_key_request_cllo {

	int size()	{	return sizeof(*this);	}
};

#define	crypty_key_inform_locl 13
struct _crypty_key_inform_locl {

	BYTE byPlus;
	WORD wKey;

	int size()	{	return sizeof(*this);	}
};

#pragma pack(pop)

#endif