#ifndef _pt_control_client_h_
#define _pt_control_client_h_

#include "CharacterDB.h"

#pragma pack(push, 1)

//어드민계정으로 컨트롤서버에 접속 
#define login_admin_request_clco 1
struct _login_admin_request_clco {

	char szID[max_id_len+1];
	char szPW[max_pw_len+1];

	int size() { return sizeof(*this);	}
};

#define login_admin_result_cocl 2
struct _login_admin_result_cocl {

	BYTE byRetCode;
	BYTE byGrade;

	WORD wRegedWorldNum;

	struct __list
	{
		bool	bOpen;
		char	szWorldName[max_world_name_len+1];
	};

	__list	WorldList[MAX_WORLD_PER_GLOBAL];

	_login_admin_result_cocl() {	wRegedWorldNum = 0;	}

	int size()	{	return sizeof(*this)-sizeof(__list)*(MAX_WORLD_PER_GLOBAL-wRegedWorldNum);	}
};

//캐릭터정보를 요구
#define load_avator_request_clco 3
struct _load_avator_request_clco {

	WORD wWorldCode;
	char szAvatorName[max_name_len+1];

	int size() { return sizeof(*this);	}
};

#define load_avator_result_cocl 4
struct _load_avator_result_cocl {

	char szRetMsg[32];
	char szID[max_id_len+1];
	WORD wWorldCode;
	bool bConnecting;

	_AVATOR_DATA Data;

	_load_avator_result_cocl() {	szRetMsg[0]	= NULL;	}

	int size() { return sizeof(*this);	}
};

#define world_open_inform_cocl 17 
struct _world_open_inform_cocl {

	WORD wWorldCode;
	bool bOpen;

	int size() { return sizeof(*this);	}
};


//WORLD OPER..	//단순히 월드를 열고닫는다.	
#define world_service_request_clco 50
struct _world_service_request_clco {		

	BYTE byWorldCode;
	bool bService;

	int size()	{	return sizeof(*this);	}
};

//WORLD EXIT..	//월드에 접속한 케릭터들을 로그아웃시킨다
#define world_exit_request_clco 51
struct _world_exit_request_clco {		

	BYTE byWorldCode;

	int size()	{	return sizeof(*this);	}
};

//WORLD MSG..	//월드에 접속한 모든 케릭터들을 메시지를 보낸다
#define world_msg_request_clco 52
struct _world_msg_request_clco {		

	BYTE byWorldCode;
	BYTE byMsgSize;
	char szMsg[255];

	_world_msg_request_clco() { byMsgSize = 0; }

	int size(){	return sizeof(*this)-(255-byMsgSize);	}
};

#define world_inf_request_clco 53
struct _world_inf_request_clco {			

	int size(){	return sizeof(*this); }
};

#define world_inf_result_cocl 54
struct _world_inf_result_cocl {			

	BYTE		byWorldNum;
	_WORLD_INFO	WorldInf[MAX_WORLD_PER_GLOBAL];	

	_world_inf_result_cocl() { byWorldNum = 0; }

	int size(){	return sizeof(*this)-sizeof(_WORLD_INFO)*(MAX_WORLD_PER_GLOBAL-byWorldNum);	}
};

#define user_inf_request_clco 55
struct _user_inf_request_clco {			

	char	szUserAccount[max_id_len+1];

	int size(){	return sizeof(*this); }
};

#define user_inf_result_cocl 56
struct _user_inf_result_cocl {			

	enum	{	max_avator = 100	};

	char	szUserAccount[max_id_len+1];
//	BYTE	byUserGrade;//등급.. //add
	char	szCreatTime[32];
	char	szCreateIP[16];
	char	szLastLoginTime[32];
	char	szLastLogoffTime[32];
	DWORD	dwTotalPlayMTime;
	char	szLastLoginIP[16];

	WORD	wBanPeriod;
	char	szBanStartDate[32];
	BYTE	byBanKind;

	WORD	wAvatorNum;

	struct __avator_list
	{
		WORD	wWorldCode;
		BYTE	bySlotIndex;
		char	szAvatorName[max_name_len+1];
		BYTE	byRaceCode;
		BYTE	byLv;	
		DWORD	dwSerial;
		BYTE	byDck;
	};

	__avator_list	AvatorData[max_avator];	

	_user_inf_result_cocl() {	wAvatorNum = 0;	}
		
	int size() { return sizeof(*this)-sizeof(__avator_list)*(max_avator-wAvatorNum);}
};

#define push_item_request_clco 57
struct _push_item_request_clco {

	char	szAvatorName[max_name_len+1];
	char	szItemCode[10];
	DWORD	dwDur;
	BYTE	byLv;
	BYTE	byTalikIndex[MAX_ITEM_LV];
};

#define push_item_result_cocl 58
struct _push_item_result_cocl {

	DWORD	dwAvatorSerial;
	char	szItemCode[10];
	DWORD	dwDur;
	DWORD	dwLv;
};

// 동시접속자 정보를 요청
#define user_count_request_clco 59
struct _user_count_request_clco {

	char	szStartDate[16];
	char	szEndDate[16];

	int size(){	return sizeof(*this); }
};

#define max_usercount_rowcount	256

// 동시접속자 정보를 줌
#define user_count_result_cocl 60
struct _user_count_result_cocl {

	WORD				wRowCount;

	struct __user_count_info 
	{
		WORD	wYear;
		BYTE	byMonth;
		BYTE	byDay;
		BYTE	byHour;
		DWORD	dwAvgCount;
		DWORD	dwMaxCount;
	};

	__user_count_info	UserCount[max_usercount_rowcount];

	int size() { return sizeof(*this)-sizeof(__user_count_info)*(max_usercount_rowcount-wRowCount);}
};

#define history_filecount_request_clco 61
struct _history_filecount_request_clco {

	char	szCharacterName[max_name_len+1];
	WORD	wWorldCode;
	WORD	wYear;
	BYTE	byMonth;
	BYTE	byDay;
	
	int size() { return sizeof(*this);	}
};

#define max_history_file_count 64

#define history_filecount_result_cocl 62
struct _history_filecount_result_cocl {

	DWORD	dwSerial;
	WORD	wFileCount;

	struct __history_file_info
	{
		WORD	wYear;
		BYTE	byMonth;
		BYTE	byDay;
		BYTE	byHour;
		char	szFileName[16];
	};

	__history_file_info HistoryFile[max_history_file_count];

	int size() { return sizeof(*this)-sizeof(__history_file_info)*(max_history_file_count-wFileCount);}
};

#define history_fileget_request_clco 63
struct _history_fileget_request_clco {

	WORD	wWorldCode;
	WORD	wYear;
	BYTE	byMonth;
	BYTE	byDay;
	BYTE	byHour;
	char	szFileName[16];

	int size() { return sizeof(*this);	}
};

#define max_hitoryfile_length 512

#define history_fileget_result_cocl 64
struct _history_fileget_result_cocl {

	DWORD	dwFileBodyLength;
	char	szFileBody[max_hitoryfile_length];
	
	int size() { return sizeof(*this)-sizeof(char)*(max_hitoryfile_length-dwFileBodyLength);}
};

#define history_filemore_request_clco 65
struct _history_filemore_request_clco {

	WORD	wWorldCode;

	int size() { return sizeof(*this);	}
};

#define character_rebirth_request_clco 66
struct _character_rebirth_request_clco {

	WORD	wWorldCode;
	DWORD	dwCharacterSerial;
	char	szCharacterName[16];

	int size() { return sizeof(*this);	}
};

#define character_rebirth_result_cocl 67
struct _character_rebirth_result_cocl {

	BYTE	byRet;

	int size() { return sizeof(*this);	}
};

#define user_ban_request_clco 68
struct _user_ban_request_clco {

	char	szAccountID[16];
	BYTE	byBanKind;
	DWORD	dwBanPeriod;
	char	szBanReason[32];

	int size() { return sizeof(*this);	}
};

#define	user_ban_result_fail	0
#define	user_ban_result_success	1

#define user_ban_result_cocl 69
struct _user_ban_result_cocl {

	BYTE	byRet;

	int size() { return sizeof(*this);	}
};

#define item_charge_info_request_clco 70
struct _item_charge_info_request_clco {

	WORD	wWorldCode;
	char	szCharacterName[16];

	int size() { return sizeof(*this); };
};

#define item_charge_info_result_cocl 71
struct _item_charge_info_result_cocl {

	struct __item_charge_info
	{
		DWORD	dwItemCode_K;
		DWORD	dwItemCode_D;
		DWORD	dwItemCode_U;
		char	szDate[32];
	};

	BYTE	byRet;
	BYTE	byLevel;
	BYTE	byRace;
	DWORD	dwDalant;
	DWORD	dwGold;
	DWORD	dwAvatorSerial;
	BYTE	byWaitItemCount;
	BYTE	byTakeItemCount;

	__item_charge_info	itemList[64];

	int size() { return sizeof(*this)-sizeof(__item_charge_info)*(64-byWaitItemCount-byTakeItemCount);}
//	int size() { return sizeof(*this);}

};

#define	avator_give_item_request_clco 72
struct	_avator_give_item_request_clco {

	WORD	wWorldCode;
	DWORD	dwAvatorSerial;

	DWORD	dwKey;		//Inven [K%d]	:	키가 IsFilled() == false면 이하 무시..	
	DWORD	dwDur;		//Inven [D%d]	:	현재내구도(수량)							
	DWORD	dwUpt;		//Inven [U%d]	:	업그레이드.. (4비트씩 0번비트부터.. 보석

	int size() { return sizeof(*this); };
};

#define	avator_give_item_result_cocl 73
struct	_avator_give_item_result_cocl {

	BYTE	byRet;
	char	szErrorMsg[256];

	int size() { return sizeof(*this); };
};

#define	user_position_request_clco 74
struct	_user_position_request_clco {

	char	szAccountID[max_name_len+1];

	int size()	{	return sizeof(*this);	}
};

#define	user_position_result_cocl 75
struct	_user_position_result_cocl {

	char	szAccountID[max_name_len+1];
	char	szWorldName[max_world_name_len+1];
	char	szAvatorName[max_name_len+1];

	int size()	{	return sizeof(*this);	}
};

#define user_ban_history_request_clco 76
struct _user_ban_history_request_clco {

	char	szAccountID[max_name_len+1];

	int size()	{	return sizeof(*this);	}
};

#define	max_ban_history_count	32

#define user_ban_history_result_cocl 77
struct _user_ban_history_result_cocl {

	struct	__ban_history_info
	{
		BYTE	byKind;
		WORD	wPeriod;
		char	szDate[32];
		char	szReason[32];
	};

	BYTE	byBanHistoryCount;

	__ban_history_info	banHistoryData[max_ban_history_count];

	int size()	{	return sizeof(*this);	}
};

#define	login_server_stat_request_clco 78
struct	_login_server_stat_request_clco {

	BYTE	byStat;

	int size()	{	return sizeof(*this);	}
};

#define	login_server_stat_result_cocl 79
struct	_login_server_stat_result_cocl {

	BYTE	byRet;

	int size()	{	return sizeof(*this);	}
};

#define error_msg_inform_cocl 100
struct _error_msg_inform_cocl {

	char	szMessage[64+1];

	_error_msg_inform_cocl() { szMessage[0] = NULL; }

	int size(){	return sizeof(*this)-(64-strlen(szMessage));	}
};

#pragma pack(pop)

#endif
