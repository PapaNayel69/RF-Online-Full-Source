#ifndef _pt_msg_client_h_
#define _pt_msg_client_h_

#include "rf_protocol.h"
#include "CharacterDB.h"

#pragma pack(push, 1)

//
//system_msg.. 	
//

#define enter_msgr_request_clmg		1// 메신져서버로 인증요청
struct _enter_msgr_request_clmg	{		//add(

	DWORD	dwAccountSerial;
	DWORD	dwAvatorSerial;
	DWORD	dwMasterKey[CHECK_KEY_NUM];			

	int size()	{ return sizeof(*this);	}
	WORD head()	{	BYTE byH[] = {	system_msg,	enter_msgr_request_clmg 		};	return *(WORD*)byH; }
};

//
//init_msg..	
//

#define other_shape_request_clmg 1		//주위사람의 생김새를 요청.. SHAPE_ALL이면 첨본사람, SHAPE_PART면 중간에 다시 만났을때 모양새가 바뀐사람
struct _other_shape_request_clmg	{

	enum { SHAPE_ALL = 0, SHAPE_PART };

	WORD	wAvatorIndex;	//대상캐릭터의 인덱스
	BYTE	byReqType;	//요구유형(0이면 첨보는 상대(other_shape_all_zocl). 1이면 다시만난 버젼이 다른 상대)..

	int size()	{ return sizeof(*this);	}
	WORD head()	{	BYTE byH[] = {	init_msg,	other_shape_request_clmg 		};	return *(WORD*)byH; }
};

#define other_shape_all_mgcl	 2	//_other_shape_request_clmg의 결과로 첨보는 상대일경우 전부를 알려줌	
struct _other_shape_all_mgcl	{	

	WORD	wAvatorIndex;
	DWORD	dwAvatorSerial;
	WORD	wEquipVer;
		
	BYTE	byUserGrade;	//유저등급 
	char	szName[max_name_len];
	BYTE	byRecIndex;	//RaceSexCode..
	BYTE	byFaceIndex;	
	BYTE	byHairIndex;

	WORD	wModelPerPart[equip_fix_num];//상위 4비트.. 업그레이드레벨..  비착용 0xFFFF

	BYTE	byUnitFrameIndex;	//안타고있으면 0xFF..
	BYTE	byUnitPartIndex[UNIT_PART_NUM];	//byUnitFrameIndex이 0xFF면 무시..

	_other_shape_all_mgcl()	{	byUnitFrameIndex = 0xFF;	}
	int size(){		if(byUnitFrameIndex != 0xFF)	return sizeof(*this);	return sizeof(*this) - sizeof(byUnitPartIndex);	}
	WORD head()	{	BYTE byH[] = {	init_msg,	other_shape_all_mgcl 		};	return *(WORD*)byH; }
};

#define other_shape_part_mgcl 3		//_other_shape_request_clmg의 결과로 버젼이 바뀐 상대일경우 일부만 알려줌
struct _other_shape_part_mgcl	{		

	WORD	wAvatorIndex;
	DWORD	dwAvatorSerial;
	WORD	wEquipVer;
		
	WORD	wModelPerPart[equip_fix_num];	//상위 4비트.. 업그레이드레벨..  비착용 0xFFFF

	BYTE	byUnitFrameIndex;	//안타고있으면 0xFF..
	BYTE	byUnitPartIndex[UNIT_PART_NUM];	//byUnitFrameIndex이 0xFF면 무시..

	_other_shape_part_mgcl(){	byUnitFrameIndex = 0xFF;	}
	int size(){	if(byUnitFrameIndex != 0xFF)	return sizeof(*this);	return sizeof(*this) - sizeof(byUnitPartIndex);	}
	WORD head()	{	BYTE byH[] = {	init_msg,	other_shape_part_mgcl 		};	return *(WORD*)byH; }
};

#define other_shape_failure_mgcl 4		//_other_shape_request_clmg의 결과로 에러리턴
struct _other_shape_failure_mgcl	{		

	enum { NOT_LIVE = 0, NOT_SERIAL };

	WORD	wAvatorIndex;
	BYTE	byErrCode;

	int size()	{ return sizeof(*this);	}
	WORD head()	{	BYTE byH[] = {	init_msg,	other_shape_failure_mgcl 		};	return *(WORD*)byH; }
};

//
//chat_msg.. 
//

#define chat_request_clmg 1			//채팅시도
struct _chat_request_clmg {			

//#define CHAT_TYPE_OPERATOR		0 //운영자..
//#define CHAT_TYPE_CIRCLE			1 //주변전체에..
//#define CHAT_TYPE_PARTY			3 //파티원들에게..
//#define CHAT_TYPE_RACE			4 //동족에게..
//#define CHAT_TYPE_MGRWSP			5 //운영자귓말

	BYTE	byChatType;
	BYTE	bySize;
	char	szChatData[max_message_size];		//송화자의 이름까지 넣어서 보낸댜.. ex)	케릭터이름 : 내용~

	_chat_request_clmg()	{	bySize = 0;	}	
	int size(){	if(bySize > max_message_size) bySize = 0; return sizeof(*this)-(max_message_size-bySize);	}
	WORD head()	{	BYTE byH[] = {	chat_msg,	chat_request_clmg 		};	return *(WORD*)byH; }
};

#define chat_whisper_request_clmg 2		//일대일대화시도
struct _chat_whisper_request_clmg {

//#define CHAT_TYPE_WHISPER			2 //일대일..

	char	szDstName[max_name_len+1];		//수신자이름
	BYTE	bySize;
	char	szChatData[max_message_size];	//송화자의 이름까지 넣어서 보낸댜.. ex)	케릭터이름 : 내용~

	_chat_whisper_request_clmg()	{	bySize = 0;	}	
	int size(){	if(bySize > max_message_size) bySize = 0; return sizeof(*this)-(max_message_size-bySize);	}
	WORD head()	{	BYTE byH[] = {	chat_msg,	chat_whisper_request_clmg 		};	return *(WORD*)byH; }
};

#define chat_broadcast_mgcl	3		//모든채팅을 클라이언트에게 뿌림
struct _chat_broadcast_mgcl {	

	BYTE	byChatType;	
	BYTE	bySize;
	char	szChatData[max_message_size];	//송화자의 이름까지 포함.. 그대로 창에 뿌리면됌 ex)	케릭터이름 : 내용~

	_chat_broadcast_mgcl()	{	bySize = 0;	}	
	int size(){	if(bySize > max_message_size) bySize = 0; return sizeof(*this)-(max_message_size-bySize);	}
	WORD head()	{	BYTE byH[] = {	chat_msg,	chat_broadcast_mgcl 		};	return *(WORD*)byH; }
};

#pragma pack(pop)

#endif
