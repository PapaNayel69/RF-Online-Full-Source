#ifndef _pt_world_msg_h_
#define _pt_world_msg_h_

#include "rf_protocol.h"
#include "CharacterDB.h"

#pragma pack(push, 1)

//system_msg

////////////////////
//메신져서버가동..
#define open_msgr_request_mgwr 1	//MSG서버를 WORLD서버에 연결할때..
struct _open_msgr_request_mgwr {

	char	szWorldName[max_world_name_len+1];
	DWORD	dwLocalIP;

	int size()	{	return sizeof(*this);	}
	WORD head()	{	BYTE byH[] = {	system_msg,	open_msgr_request_mgwr 		};	return *(WORD*)byH; }
};

#define open_msgr_success_wrmg 2	//위에대한 성공메시지 (MSG서버운영에 필요한 데이터를 보냄)
struct _open_msgr_success_wrmg {	

	enum	{	max_map = 0xFF	};

	//맵정보
	struct __list{
		WORD	wLayerNum;
		DWORD	dwSectorNum;
		WORD	wSecNumW;	//가로의 섹터수..
	};

	WORD	wMapNum;
	__list	MapList[max_map];

	_open_msgr_success_wrmg()	{	wMapNum = 0;	}	
	int size()	{	if(wMapNum > max_map) wMapNum = 0;  return sizeof(*this) - sizeof(__list)*(max_map-wMapNum);	}
	WORD head()	{	BYTE byH[] = {	system_msg,	open_msgr_success_wrmg 		};	return *(WORD*)byH; }
};

#define open_msgr_failure_wrmg 3	//위에대한 실패메시지
struct _open_msgr_failure_wrmg {	

	char	szErrCode[64];
	
	int size()	{	return sizeof(*this);	}
	WORD head()	{	BYTE byH[] = {	system_msg,	open_msgr_failure_wrmg 		};	return *(WORD*)byH; }
};

//////////////////
//접속유저인증..
#define trans_user_inform_wrmg 11	//월드에서 케릭터를 선택할대 MSG서버에 등록할 캐릭터정보를 보냄
struct _trans_user_inform_wrmg {	//msgr는 현재접속자와 대기자중에 같은 dwAccountSerial이 있는지 보고.. 있다면 기존꺼는 exit, 추가꺼는 실패처리..

	_CLID	idWorld;
	DWORD	dwKey[CHECK_KEY_NUM];
	DWORD	dwClientIP;	
	DWORD	dwAccountSerial;
	DWORD	dwAvatorSerial;
	WORD	wAvatorIndex;
	char	szAccountID[max_id_len+1];	
	char	szAvatorName[max_name_len+1];	
	BYTE	byUserDgr;
	BYTE	bySubDgr;	
	BYTE	byRaceSexCode;

	int size()	{	return sizeof(*this);	}
	WORD head()	{	BYTE byH[] = {	system_msg,	trans_user_inform_wrmg 		};	return *(WORD*)byH; }
};

#define trans_user_report_mgwr 12	//위에대한 등록완료, 실패 답변
struct _trans_user_report_mgwr {

	_CLID	idWorld;
	BYTE	byRetCode;	//0.. success, 1~ failure

	int size()	{	return sizeof(*this);	}
	WORD head()	{	BYTE byH[] = {	system_msg,	trans_user_report_mgwr 		};	return *(WORD*)byH; }
};

//////////////////////
//유저서버출입 관련..
#define enter_user_report_mgwr 21	//MSG서버에서 trans_user_inform_wrmg를 통한 대기유저의 접속및 인증을 완료하면 WORLD서버에 알린다.
struct _enter_user_report_mgwr {	//만약 이 메시지를 받기전에 유저가 close된 상태에는 바로 close_user_command_wrmg로 나감을 알림..

	_CLID	idWorld;
	_CLID	idLocal;

	int size()	{	return sizeof(*this);	}
	WORD head()	{	BYTE byH[] = {	system_msg,	enter_user_report_mgwr 		};	return *(WORD*)byH; }
};

#define exit_user_report_mgwr 22	//MSG서버에서 유저가 close_user_command_wrmg없이 나간경우 WORLD서버에 알린다.
struct _exit_user_report_mgwr {

	_CLID	idWorld;

	int size()	{	return sizeof(*this);	}
	WORD head()	{	BYTE byH[] = {	system_msg,	exit_user_report_mgwr 		};	return *(WORD*)byH; }
};

#define close_user_command_wrmg 23	//WORLD서버에서 MSG서버에게 강제로 끊어야할 유저를 알려준다.
struct _close_user_command_wrmg {

	_CLID	idLocal;

	int size()	{	return sizeof(*this);	}
	WORD head()	{	BYTE byH[] = {	system_msg,	close_user_command_wrmg 		};	return *(WORD*)byH; }
};

/////////////////////////
//유저데이터동기화 관련..
#define start_shape_inform_wrmg	51	//처음시작할때 시작모양새를 알림
struct _start_shape_inform_wrmg {	//add(4/7)

	_CLID	idLocal;
	BYTE	byDftPart_Face;
	BYTE	byDftPart[base_fix_num];
	WORD	wEquipPart[equip_fix_num];//상위 4비트.. 업그레이드레벨..  비착용 0xFFFF
	WORD	wVisualVer;

	int size()	{	return sizeof(*this);	}
	WORD head()	{	BYTE byH[] = {	system_msg,	start_shape_inform_wrmg 		};	return *(WORD*)byH; }
};

#define update_pos_inform_wrmg	31	//위치가 바뀐 유저의 정보를 일정간격으로 한번에 묶어서 보낸다.
struct _update_pos_inform_wrmg {

	struct __list	{
		_CLID	idLocal;
		BYTE	byMapCode;
		BYTE	byLayerIndex;
		DWORD	wSectorIndex;
	};

	WORD	wUpdateNum;
	__list	UpdateList[MAX_PLAYER];

	_update_pos_inform_wrmg()	{	wUpdateNum = 0;	}	
	int size()	{	if(wUpdateNum > MAX_PLAYER)	wUpdateNum = 0;		return sizeof(*this)-sizeof(__list)*(MAX_PLAYER-wUpdateNum);	}
	WORD head()	{	BYTE byH[] = {	system_msg,	update_pos_inform_wrmg 		};	return *(WORD*)byH; }
};

#define update_equip_inform_wrmg 32	//모양새가 바뀐 유저의 정보를 실시간으로 보낸다.
struct _update_equip_inform_wrmg {

	_CLID	idLocal;
	WORD	wVisualVer;
	BYTE	byPartCode;
	WORD	wEquipPart;

	int size()	{	return sizeof(*this);	}
	WORD head()	{	BYTE byH[] = {	system_msg,	update_equip_inform_wrmg 		};	return *(WORD*)byH; }
};

#define riding_unit_inform_wrmg 33	//유닛을 탑승한 유저의 정보를 실시간으로 보낸다.
struct _riding_unit_inform_wrmg {

	_CLID	idLocal;
	WORD	wVisualVer;
	BYTE	byFrameCode;
	BYTE	byPartCode[UNIT_PART_NUM];

	int size()	{	return sizeof(*this);	}
	WORD head()	{	BYTE byH[] = {	system_msg,	riding_unit_inform_wrmg 		};	return *(WORD*)byH; }
};

#define leave_unit_inform_wrmg 34	//유닛을 하차한 유저의 정보를 실시간으로 보낸다.
struct _leave_unit_inform_wrmg {

	_CLID	idLocal;
	WORD	wVisualVer;

	int size()	{	return sizeof(*this);	}
	WORD head()	{	BYTE byH[] = {	system_msg,	leave_unit_inform_wrmg 		};	return *(WORD*)byH; }
};

#define join_party_inform_wrmg 35	//파티가입
struct _join_party_inform_wrmg {

	_CLID	idLocal;

	BYTE	byMemberNum;
	WORD	wAvatorIndex[member_per_party];
	
	_join_party_inform_wrmg()	{	byMemberNum = 0;	}
	int size()	{	if(byMemberNum > member_per_party)	byMemberNum = 0;	return sizeof(*this)-sizeof(WORD)*(member_per_party-byMemberNum);	}
	WORD head()	{	BYTE byH[] = {	system_msg,	join_party_inform_wrmg 		};	return *(WORD*)byH; }
};

#define add_party_inform_wrmg 36	//파티원추가
struct _add_party_inform_wrmg {

	_CLID	idLocal;

	WORD	wAvatorIndex;
	
	int size()	{	return sizeof(*this);	}
	WORD head()	{	BYTE byH[] = {	system_msg,	add_party_inform_wrmg 		};	return *(WORD*)byH; }
};

#define drop_party_inform_wrmg 37	//파티원감소
struct _drop_party_inform_wrmg {

	_CLID	idLocal;

	WORD	wAvatorIndex;
	
	int size()	{	return sizeof(*this);	}
	WORD head()	{	BYTE byH[] = {	system_msg,	drop_party_inform_wrmg 		};	return *(WORD*)byH; }
};

#define disjoint_party_inform_wrmg 38	//파티해체
struct _disjoint_party_inform_wrmg {

	_CLID	idLocal;

	int size()	{	return sizeof(*this);	}
	WORD head()	{	BYTE byH[] = {	system_msg,	disjoint_party_inform_wrmg 		};	return *(WORD*)byH; }
};

#pragma pack(pop)

#endif
