#ifndef _pt_zone_client_h_
#define _pt_zone_client_h_

#include "rf_protocol.h"
#include "CharacterDB.h"

#pragma pack(push, 1)

//
//system..	1
//
#define enter_world_request_zone 1	// 사용자 인증을 요청	
struct _enter_world_request_zone {

	DWORD	dwAccountSerial;
	DWORD	dwMasterKey[CHECK_KEY_NUM];			
	bool	bFullMode;	//full mode true, window mode.. false

	int size()	{ return sizeof(*this);	}
};

#define enter_world_result_zone 2// 사용자 인증의 결과	
struct _enter_world_result_zone {

//유저등급..( CharacterDB.h )
//#define USER_DEGREE_STD			0		//유저등급-일반
//#define USER_DEGREE_GID			1		//유저등급-가이드
//#define USER_DEGREE_MGR			2		//유저등급-운영자
//#define USER_DEGREE_DEV			3		//유저등급-개발자

	BYTE	byResult;
	BYTE	byUserGrade; //계정등급..  2, 3등급 치트키 및 운영자 메시지, /이동 사용가능

	int size()	{ return sizeof(*this);	}
};

#define messenger_enter_key_inform_zocl	222
struct _messenger_enter_key_inform_zocl {	

	DWORD	dwAvatorSerial;			//add
	DWORD	dwKey[CHECK_KEY_NUM];
	DWORD	dwMessengerIP;

	int size()	{ return sizeof(*this);	}
};

#define movein_user_request_zone 3	// 사용자 인증을 요청
struct _movein_user_request_zone {

	DWORD	dwAccountSerial;	
	DWORD	dwZoneMasterKey;
	bool	bFullMode;	//full mode true, window mode.. false

	int size()	{ return sizeof(*this);	}
};

#define movein_user_result_zone 4// 사용자 인증의 결과
struct _movein_user_result_zone {

	BYTE	byResult;

	int size()	{ return sizeof(*this);}//-(2560-wInitDataSize);	}
};

#define moveout_user_request_zone 5
struct _moveout_user_request_zone {

	//#define ZONE_TYPE_GATE	0	..캐릭터선택을 다시할라믄..
	//#define ZONE_TYPE_MAP		1	..맵이동을 햇다면..
	BYTE	byNextZoneType;	

	int size() { return sizeof(*this); }
};

#define moveout_user_result_zone 6
struct _moveout_user_result_zone {

	BYTE	byRetCode;
	DWORD	dwIP;	
	WORD	wPort;
	DWORD	dwZoneMasterKey;

	int size()	{ return sizeof(*this);	}
};

#define reged_char_request_zone 7 //등록됀 캐릭터를 요구
struct _reged_char_request_zone {

	int size() { return 0; }
};

#define reged_char_result_zone 8 //등록됀 캐릭터를 요구
struct _reged_char_result_zone {

	BYTE	byRetCode;	
	BYTE	byCharNum;

	_REGED_AVATOR_DB RegedList[MAX_CHAR_PER_WORLDUSER];	

	_reged_char_result_zone(){
		byCharNum = 0;
	}

	int size(){ 
		return sizeof(*this)-sizeof(_REGED_AVATOR_DB)*(MAX_CHAR_PER_WORLDUSER-byCharNum);
	}
};

#define add_char_request_zone 9	// 캐릭터 등록
struct _add_char_request_zone {

	BYTE bySlotIndex;				
	char szCharName[max_name_len+1];
	BYTE byRaceSexCode;
	char szClassCode[class_code_len+1];	
	DWORD dwBaseShape;	
	
	int size()	{ return sizeof(*this);	}
};

#define add_char_result_zone 10	// 캐릭터 등록 확인
struct _add_char_result_zone {

	BYTE byRetCode;	
	BYTE byAddSlotIndex;

	int size()	{ return sizeof(*this);	}
};

#define del_char_request_zone 11	//삭제할 케릭터 선택
struct _del_char_request_zone {
	
	BYTE bySlotIndex;

	int size()	{ return sizeof(*this);	}
};

#define del_char_result_zone 12	//삭제할 케릭터 선택확인
struct _del_char_result_zone {

	BYTE byRetCode;	
	BYTE bySlotIndex;

	int size()	{ return sizeof(*this);	}
};

#define sel_char_request_zone 13	//플레이할 케릭터 선택
struct _sel_char_request_zone {
	
	BYTE bySlotIndex;

	int size()	{ return sizeof(*this);	}
};

#define sel_char_result_zone 14	//플레이할 케릭터 선택확인
struct _sel_char_result_zone {

	BYTE byRetCode;	
	BYTE bySlotIndex;
	DWORD	dwWorldSerial; 

	int size()	{ return sizeof(*this);	}
};

//SERVER NOTIFY CODE
#define server_notify_inform_zone 17
struct _server_notify_inform_zone {

	WORD	wMsgCode;
	DWORD	dwPushIP;


	int size()	{	return sizeof(*this);	}
};

//
//init_msg..	3
//

//존에 접속해서 데이터를 전부 다운로드받으면 byMapInMode = mapin_type_start
//포탈이동한후 위치보정을 완료하면 byMapInMode = mapin_type_move
//부활후 위치보정 완료하면 byMapInMode = mapin_type_revival
//기존 client_load_complete_clzo 와 _new_map_load_complete_clzo를 합쳤음..

#define new_pos_start_request_clzo 1
struct _new_pos_start_request_clzo {

	//#define mapin_type_start		0	//게임시작..
	//#define mapin_type_move		1	//포탈이동..
	//#define mapin_type_revival	2	//부활..
	//#define mapin_type_goto		3	//순간이동..
	//#define mapin_type_recall     4	//출동..
	//#define mapin_type_dungeon    5	//던젼..

	BYTE byMapInMode;
};

#define new_pos_start_result_zocl 2
struct _new_pos_start_result_zocl {

	BYTE byMapInMode;	//위 byMapInMode의 리바운드값.. 실패시 0xFF..
};

#define base_download_request_clzo 5
struct _base_download_request_clzo {	
};

//저장 아이템 시리얼 부여순서..
//1. Equip
//2. Embellish
//3. Belt
//4. Inven
//5. Force
//6. Animus
#define base_download_result_zocl 6
struct _base_download_result_zocl {

	BYTE byRetCode;	//0이면 성공..

	WORD wZoneIndex;	//현재 존상에서의 인덱스
	WORD wExpRate;	
	WORD wClassHistory[class_history_num];	//과거 class 기록
	WORD wHP;		
	WORD wSP;		
	WORD wFP;		
	WORD wMaxHP;			
	WORD wMaxSP;			
	WORD wMaxFP;			
	WORD wMapIndex;		//맵인덱스	
	short zPos[3];		//위치

	struct _list_equip{
		char sTableCode;	//table code	.. 빈 슬롯이면 -1
		WORD wItemIndex;	//item index
		DWORD dwUptInfo;//업그레이드정보..오른쪽부터 4비트씩 업그레이드정보.. F가 나올때까지업글레벨을 ++한다.
	}; 

	_list_equip EquipList[equip_fix_num];

	struct _list_embellish{
		char sTableCode;	//table code	.. 빈 슬롯이면 -1
		WORD wItemIndex;	//item index
		WORD wNum;		//화살일경우 남은갯수
		char sClientIndex;	
	};
	
	_list_embellish EmbellishList[embellish_fix_num];	

	struct _list_belt{
		short zItemIndex;	//item index 빈거면 -1 
		BYTE byNum;		//갯수
		char sClientIndex;	
	};
	
	_list_belt BeltList[potion_belt_num];

	struct _list_linker{
		BYTE byEffectCode;	
		WORD wItemIndex;	//item index
		char sClientIndex;	
	};

	_list_linker LinkerList[sf_linker_num];

	int size(){
		if(byRetCode != 0)
			return sizeof(byRetCode);
		return sizeof(*this);
	}
};

//_movein_user_result_zone후에 아바타데이터를 다운받는다.
//인벤토리..
#define inven_download_request_clzo 7
struct _inven_download_request_clzo {
};

#define inven_download_result_zocl 8
struct _inven_download_result_zocl {	

	BYTE byRetCode;	//0이면 성공..

	BYTE byBagNum;	
	BYTE bySlotNum;

	struct _list{
		BYTE byTableCode; 
		WORD wItemIndex;
		DWORD dwDurPoint;
		char sClientIndex;
		DWORD dwUptInfo;//업그레이드정보
	}; 	
	
	_list ItemSlotInfo[bag_storage_num];

	int size(){
		if(byRetCode != 0)
			return sizeof(byRetCode);
		return sizeof(*this)-sizeof(_list)*(bag_storage_num-bySlotNum);
	}
};

//각종 누적치..
#define cum_download_request_clzo 9
struct _cum_download_request_clzo { 
};

#define cum_download_result_zocl 10
struct _cum_download_result_zocl {

	BYTE byRetCode;	//0이면 성공..

	_STAT_DB_BASE Stat;	

	BYTE byLeftCuttingResNum;				
	WORD wleftResList[left_cutting_num];	//..하위바이트: res인덱스, 상위바이트: res갯수

	int size(){
		if(byRetCode != 0)
			return sizeof(byRetCode);
		return sizeof(*this)-sizeof(WORD)*(left_cutting_num-byLeftCuttingResNum);
	}
};

//포스인벤내용
#define force_download_request_clzo 11
struct _force_download_request_clzo {

};

#define force_download_result_zocl 12
struct _force_download_result_zocl {

	BYTE byRetCode;	//0이면 성공..

	BYTE bySlotNum;

	struct _list{
		WORD wItemIndex;
		DWORD dwCum;
	}; 	

	_list ItemSlotInfo[force_storage_num];

	int size(){
		if(byRetCode != 0)
			return sizeof(byRetCode);
		return sizeof(*this)-sizeof(_list)*(force_storage_num-bySlotNum);
	}
};

#define special_download_request_clzo 122
struct _special_download_request_clzo {

};

#define unit_download_result_zocl 123
struct _unit_download_result_zocl {	

	struct _list{
		BYTE					bySlotIndex;
		_UNIT_DB_BASE::_LIST	UnitData;
	};

	BYTE	byUnitNum;
	_list	UnitList[unit_storage_num];		
	
	_unit_download_result_zocl()	{
		byUnitNum = 0;
	}

	int size(){
		return sizeof(*this)-sizeof(_list)*(unit_storage_num-byUnitNum);
	}
};

#define animus_download_result_zocl 144
struct _animus_download_result_zocl {

	struct _list{
		char sItemIndex;	//item index 빈거면 -1 
		DWORD dwExp;		//아이템 수량 맴버변수사용..
		DWORD dwParam;		//	_animus_param 으로 형변환할것..(CharacterDB.h)		//아이템 업그레이드 맴버변수사용..
							//	struct _animus_param{
							//		WORD wHP;
							//		WORD wFP;		};
	};

	BYTE byAnimusNum;
	_list AnimusList[animus_storage_num];
	
	_animus_download_result_zocl()	{
		byAnimusNum = 0;
	}

	int size(){
		return sizeof(*this)-sizeof(_list)*(animus_storage_num-byAnimusNum);
	}
};

#define monster_create_zocl 13
struct _monster_create_zocl{

	WORD wRecIndex;
	WORD wIndex;
	DWORD dwSerial;
	short zPos[3];
	BYTE byInfo[4];

};

#define recallmon_create_zocl 113
struct _recallmon_create_zocl{	
	
	WORD wRecIndex;
	WORD wIndex;
	DWORD dwSerial;
	short zPos[3];
	BYTE byInfo[4];

};

#define npc_create_zocl 114
struct _npc_create_zocl {

	WORD wRecIndex;
	WORD wIndex;
	DWORD dwSerial;
	short zPos[3];
};

#define animus_create_zocl 115
struct _animus_create_zocl {

	WORD wIndex;
	WORD wRecIndex;
	DWORD dwSerial;
	short zPos[3];
	BYTE byLv;	
};

#define tower_create_zocl 116
struct _tower_create_zocl {	

	WORD wIndex;
	WORD wRecIndex;
	DWORD dwSerial;
	short zPos[3];	
};

#define itembox_create_zocl 14
struct _itembox_create_zocl{	

	BYTE byItemTableCode;
	WORD wItemRecIndex;
	BYTE byAmount;	//중첩아이템일경우 갯수..
	WORD wBoxIndex;	
	DWORD dwOwerSerial;	//1차로 줏을수잇는 권리자..
	_CHRID idDumber;	//버린사람.. 버린사람정보없으면 0xFF..
	BYTE byState;
	short zPos[3];
};

#define parkingunit_create_zocl 124
struct _parkingunit_create_zocl{

	WORD wObjIndex;
	DWORD dwObjSerial;
	BYTE byCreateType;	//#define unit_create_type_delivery	0	//출고	//#define unit_create_type_leave		1	//하차
	BYTE byFrame;
	BYTE byPart[UNIT_PART_NUM];
	DWORD dwOwerSerial;	
	short zPos[3];
	BYTE byTransDistCode;	//#define unit_trans_short_range		0	//유닛 근거리수송 #define unit_trans_long_range		1	//유닛 원거리수송
};	

#define player_destroy_zocl 15
struct _player_destroy_zocl{	//존서버종료//

	WORD wIndex;
	DWORD dwSerial;

	BYTE byState; //0: 게임종료, 1: 존이동
};

#define player_die_zocl 16
struct _player_die_zocl{	//시체로 죽음//

	WORD wIndex;
	DWORD dwSerial;
};

#define monster_destroy_zocl 17
struct _monster_destroy_zocl{

	WORD wIndex;
	DWORD dwSerial;	
	BYTE byDestroyCode;	//0.. die, 1.. respawn
};

#define npc_destroy_zocl 118
struct _npc_destroy_zocl{	

	WORD wIndex;
	DWORD dwSerial;	
};

#define animus_destroy_zocl 119
struct _animus_destroy_zocl {

	WORD wIndex;
	DWORD dwSerial;
	BYTE byDestroyCode;
};

#define tower_destroy_zocl 120
struct _tower_destroy_zocl {	

	WORD wIndex;
	DWORD dwSerial;

//#define tower_des_type_back			0	//회수
//#define tower_des_type_destroy		1	//파괴
//#define tower_des_type_disconnect	2	//설치자 게임종료

	BYTE byDestroyCode;	//맞아죽나..회수냐..
};

#define itembox_destroy_zocl 18
struct _itembox_destroy_zocl{

	WORD wIndex;
};

#define parkingunit_destroy_zocl 188
struct _parkingunit_destroy_zocl{

	WORD wObjIndex;
	DWORD dwObjSerial;
	BYTE byDestroyCode;//#define unit_destory_type_return	0	//입고#define unit_destory_type_take		1	//승차
};

#define other_shape_request_clzo 19
struct _other_shape_request_clzo	{

	WORD wIndex;	//대상캐릭터의 인덱스
	
	enum { SHAPE_ALL = 0, SHAPE_PART };

	BYTE byReqType;	//요구유형(0이면 첨보는 상대(other_shape_all_zocl). 1이면 다시만난 버젼이 다른 상대)..
};

#define other_shape_all_zocl 20
struct _other_shape_all_zocl	{	//_other_shape_request_clzo의 결과로 첨보는 상대일경우..	

	WORD wIndex;
	DWORD dwSerial;
	WORD wEquipVer;
		
	BYTE byRecIndex;
	BYTE byFaceIndex;	
	BYTE byHairIndex;
	WORD wModelPerPart[equip_fix_num];//상위 4비트.. 업그레이드레벨..  비착용 0xFFFF
	BYTE byUserGrade;	//유저등급 
	char szName[max_name_len+1];	

	BYTE byUnitFrameIndex;	//안타고있으면 0xFF..
	BYTE byUnitPartIndex[UNIT_PART_NUM];	//byUnitFrameIndex이 0xFF면 무시..

	_other_shape_all_zocl(){
		byUnitFrameIndex = 0xFF;
	}

	int size(){
		if(byUnitFrameIndex != 0xFF) return sizeof(*this);
		return sizeof(*this) - sizeof(byUnitPartIndex);
	}
};

#define other_shape_part_zocl 21
struct _other_shape_part_zocl	{		//_other_shape_request_clzo의 결과로 버젼이 바뀐 상대일경우..

	WORD wIndex;
	DWORD dwSerial;
	WORD wEquipVer;
		
	WORD wModelPerPart[equip_fix_num];	//상위 4비트.. 업그레이드레벨..  비착용 0xFFFF

	BYTE byUnitFrameIndex;	//안타고있으면 0xFF..
	BYTE byUnitPartIndex[UNIT_PART_NUM];	//byUnitFrameIndex이 0xFF면 무시..

	_other_shape_part_zocl(){
		byUnitFrameIndex = 0xFF;
	}

	int size(){
		if(byUnitFrameIndex != 0xFF) return sizeof(*this);
		return sizeof(*this) - sizeof(byUnitPartIndex);
	}
};

#define other_shape_failure_zocl 22
struct _other_shape_failure_zocl	{		//_other_shape_request_clzo의 결과로 에러리턴

	WORD wOtherIndex;

	enum { NOT_LIVE = 0, NOT_SERIAL };

	BYTE byErrCode;
};

#define other_shape_change_zocl 23
struct _other_shape_change_zocl	{	//주위 플레이어가 장착을 바꾼경우

	WORD wIndex;
	DWORD dwSerial;
	WORD wEquipVer;

	BYTE byPartIndex;
	WORD wItemIndex;	//상위 4비트.. 업그레이드레벨..  
};

#define other_unit_ride_change_zocl 233
struct _other_unit_ride_change_zocl	{	//주위 플레이어가 유닛 승하차를 바꾼경우

	WORD	wIndex;
	DWORD	dwSerial;
	WORD	wEquipVer;

	bool	bTake;				//승차는 true, 하차는 false
	DWORD	dwUnitSerial;		//모르는 시리얼의 경우.. 바깥 섹터에 있는 유닛.. 플레이어도 섹터밖으로 빼버리고.. 나중에 만나면 _other_shape_part_zocl로 새로받는다..
	short	zNewPos[3];			//승하차 뽀지숀..
};

//자신이 새로운곳이 뿅하고 나타날때.. 주위의 플레이어에게 뿌린다.
#define other_new_view_zocl 24
struct _other_new_view_zocl{	

	WORD wIndex;
	DWORD dwSerial;
	WORD wEquipVer;
	short zPos[3];
	BYTE byRaceCode;

	//byViewType----
	//#define mapin_type_start		0	//게임시작		
	//#define mapin_type_move		1	//포탈이동..
	//#define mapin_type_revival	2	//부활
	//#define mapin_type_goto		3	//순간이동
	//#define mapin_type_recall     4	//출동..

	BYTE byViewType;
	DWORD dwStateFlag;
	WORD wLastEffectCode;
};

#define player_revival_request_clzo 28
struct _player_revival_request_clzo{

};

#define player_revival_zocl 29
struct _player_revival_zocl{

	BYTE byRet;
	WORD wStartMapIndex;
	short zPos[3];
	BYTE byLevel;
	WORD wCurHP;
	WORD wCurFP;
	WORD wCurST;	
	BYTE byZoneCode;	//0이면 같은 존.. 1이면 다른존.. => move_out을 보낸다.
};

#define other_map_out_zocl 30
struct _other_map_out_zocl{

	BYTE byMapOutType;
	DWORD dwSerial;
	BYTE byNextMap;	//나가서 새로 들어갈맵..모르는경우는 0xFF
};

//부활..
#define player_resurrect_zocl 31
struct _player_resurrect_zocl{

	BYTE byRet;
	BYTE byLevel;
	WORD wCurHP;
	WORD wCurFP;
	WORD wCurST;
};

#define exit_save_data_request_clzo 32
struct _exit_save_data_request_clzo {		//바뀐것만으로..변경..

	WORD wDataSize;

//inven 저장수[1], { serial[2], client_index[1] }
///belt 저장수[1], { serial[2], client_index[1] }
///link 저장수[1], { effectcode[1], effectindex[1], client_index[1] }
///embell 저장수[1], { serial[2], client_index[1] }

	char sData[0x0FFF];

	int size(){
		return sizeof(*this)-sizeof(char)*(0x0FFF-wDataSize);
	}			
};

#define exit_save_data_result_zocl 33
struct _exit_save_data_result_zocl {
	
	BYTE byRetCode;	//0;//성공, 1;//시리얼틀림 2;//이미인덱싱했음

};

#define stat_inform_zocl	34
struct _stat_inform_zocl {

	BYTE byStatIndex;
	DWORD dwNewStat;
};

#define fcitem_inform_zocl	35
struct _fcitem_inform_zocl {

	WORD wItemSerial;
	DWORD dwNewStat;
};

//
//position_msg..	4
//

#define move_request_clzo 1
struct _move_request_clzo{

	BYTE byMoveType;
	BYTE byModeType;
	short zCur[3];	//xyz
	short zTar[2];	//xz
};

#define real_movpos_request_clzo 2
struct _real_movpos_request_clzo {

	short zCur[3];	//xyz
};

#define move_error_result_zocl 22
struct _move_error_result_zocl {

	BYTE byErrCode;
	short zCur[3];	//xyz..서버상의 위치..
};

#define player_move_zocl 4
struct _player_move_zocl{	

	DWORD dwSerial;
	WORD wIndex;
	WORD wEquipVer;	
	BYTE byRaceCode;
	WORD wLastEffectCode;	
	DWORD dwStateFlag;
	short zCur[3];//xyz
	short zTar[2];	//xz
	BYTE byAddSpeed;
};

#define monster_move_zocl 5
struct _monster_move_zocl{ 

	WORD wRecIndex;
	WORD wIndex;
	DWORD dwSerial;
	WORD wLastEffectCode;
	short zCur[3];
	short zTar[2];	//x,z
	WORD wHPRate;
	BYTE byInfo[4];
};

#define npc_move_zocl 106
struct _npc_move_zocl { 

	WORD wRecIndex;
	WORD wIndex;
	DWORD dwSerial;
	WORD wLastEffectCode;		
	short zCur[3];
	short zTar[2];	//x,z
};

#define animus_move_zocl 107
struct _animus_move_zocl {	

	WORD wRecIndex;
	WORD wIndex;
	DWORD dwSerial;
	short zCur[3];
	short zTar[2];
	WORD wHPRate;
	WORD wLastEffectCode;	
};

#define monster_chase_zocl 6
struct _monster_chase_zocl{

	WORD wRecIndex;
	WORD wIndex;
	WORD wDstIndex;
	BYTE byLastEffectCode;	
	short zCur[3];
	short zTar[2]; //x,z
	WORD wHPRate;
};

#define player_fixpositon_zocl 7
struct _player_fixpositon_zocl{

	WORD wIndex;
	DWORD dwSerial;
	WORD wEquipVer;	

	BYTE byRaceCode;
	short zCur[3];
	WORD wLastEffectCode; //최근에 걸린 효과코드..(왼쪽 4비트.. 효과코드(effect_code_skill, effect_code_force, effect_code_class).. 다음부터는 인덱스..
	DWORD dwStateFlag;
};

#define object_real_fixpositon_zocl 77
struct _object_real_fixpositon_zocl{	

	BYTE byObjKind;
	BYTE byObjID;
	WORD wIndex;
	DWORD dwSerial;
};

#define monster_fixpositon_zocl 8
struct _monster_fixpositon_zocl{

	WORD wRecIndex;
	WORD wIndex;
	DWORD dwSerial;
	WORD wLastEffectCode;		
	short zCur[3];
	WORD wHPRate;
	BYTE byInfo[4];	
};

#define npc_fixpositon_zocl 109
struct _npc_fixpositon_zocl {

	WORD wRecIndex;
	WORD wIndex;
	DWORD dwSerial;
	WORD wLastEffectCode; 
	short zCur[3];
};

#define animus_fixpositon_zocl 110
struct _animus_fixpositon_zocl {	

	WORD wRecIndex;
	WORD wIndex;
	DWORD dwSerial;
	short zCur[3];
	WORD wHPRate;
	BYTE byLv;	
	WORD wLastEffectCode;	
};

#define tower_fixpositon_zocl	111
struct _tower_fixpositon_zocl {	

	WORD wRecIndex;
	WORD wIndex;
	DWORD dwSerial;
	short zCur[3];
	WORD wCompLeftSec;	//다 짓기위해 남은시간..(0이면 다 지은 타워)
};

#define itembox_fixpositon_zocl 9	
struct _itembox_fixpositon_zocl{

	BYTE byItemTableCode;
	WORD wItemRecIndex;
	BYTE byAmount;	//중첩아이템일경우 수량
	WORD wItemBoxIndex;
	DWORD dwOwerSerial;	//누가 버린건 0xffff
	short zPos[3];
	BYTE byState;	//0: normal(지정자만이 먹을수잇는상태) 
					//1: open(누구든지 먹을수 있는 상태) 
					//2: hurry(소멸임박,깜빡임)
};

#define parkingunit_fixpositon_zocl 99	
struct _parkingunit_fixpositon_zocl{

	WORD wObjIndex;
	DWORD dwObjSerial;
	BYTE byFrame;
	BYTE byPart[UNIT_PART_NUM];
	short zPos[3];
};

#define correct_position_zocl 10
struct _correct_position_zocl {

	BYTE byMapIndex;
	short sPos[3];
};

#define monster_stop_zocl 11
struct _monster_stop_zocl {

	WORD wIndex;
	short zCur[3];
};

#define player_stop_clzo 12
struct _player_stop_clzo {

	short zCur[3];
};

#define player_stop_zocl 13
struct _player_stop_zocl {

	DWORD dwSerial;
	short zCur[3];
};

#define player_real_move_zocl 14
struct _player_real_move_zocl {	

	WORD wIndex;
	DWORD dwSerial;
	WORD dwEquipVer;

	BYTE byRaceCode;
	short zCur[3];
	short zTar[2];	
	WORD wLastEffectCode; //최근에 걸린 효과코드..(왼쪽 1비트.. 효과코드(effect_code_skill, effect_code_force..).. 다음부터는 인덱스.. 
	DWORD dwStateFlag; 
	BYTE byAddSpeed;	//효과로인한 증가돼는 이동속도
};

#define monster_real_move_zocl 15
struct _monster_real_move_zocl {		

	WORD wRecIndex;
	WORD wIndex;
	DWORD dwSerial;
	WORD wLastEffectCode;		
	short zCur[3];
	short zTar[2];
	WORD wHPRate;
	BYTE byInfo[4];
};

#define npc_real_move_zocl 116
struct _npc_real_move_zocl {		

	WORD wRecIndex;
	WORD wIndex;
	DWORD dwSerial;
	WORD wLastEffectCode;
	short zCur[3];
	short zTar[2];
};

#define animus_real_move_zocl 117
struct _animus_real_move_zocl {	

	WORD wRecIndex;
	WORD wIndex;
	DWORD dwSerial;
	short zCur[3];
	short zTar[2];
	WORD wHPRate;
	BYTE byLv;	
	WORD wLastEffectCode;
};

#define state_inform_zocl 16	
struct _state_inform_zocl {		

	DWORD dwSerial;
	DWORD dwState;
};

#define goto_baseportal_request_clzo 17
struct _goto_baseportal_request_clzo {

	WORD wItemSerial;	//town item 시리얼(0xFFFF면 종족초기위치..발자버젼) 
};

#define goto_baseportal_result_zocl 18
struct _goto_baseportal_result_zocl {

	BYTE byRet;//0;//성공//1;//시체상태임 2;//포탈을못읽음 3;//좌표 못얻음 4;//다른종족맴
	BYTE byMapCode;	
	short zPos[3];
};

#define goto_avator_request_clzo 19
struct _goto_avator_request_clzo {		

	char szAvatorName[max_name_len+1];
};

#define goto_avator_result_zocl 20	
struct _goto_avator_result_zocl {	

	BYTE byRet;//0;//성공//1;//댜른존에 있음 2;//없음
	BYTE byMapCode;
	float fStartPos[3];
};

#define break_stop_result_zocl 21
struct _break_stop_result_zocl {	//..높이등의 문제땜에 중간에 멈춰야하는경우..

	BYTE byObjID;	
	DWORD dwObjSerial;
	short zCur[3];
};

#define move_type_change_request_clzo 222
struct _move_type_change_request_clzo {	

	BYTE byMoveType;	//	move_type_walk:0, move_type_run:1 
};

//
//fight_msg..	5
//
#define attack_gen_request_clzo 1
struct _attack_gen_request_clzo {

	BYTE byID;			
	WORD wIndex;
	BYTE byAttPart;		
	WORD wBulletSerial;	//	==> bullet과 무관한 공격이면 0xFF
};

#define attack_skill_request_clzo 5
struct _attack_skill_request_clzo {	

	BYTE byID;				
	WORD wIndex;
	BYTE byEffectCode;	//	effect_code_skill, effect_code_class ..		
	WORD wSkillIndex;
	WORD wBulletSerial;		//bullet 무관 : 0xFFFF
};

#define attack_force_request_clzo 9
struct _attack_force_request_clzo {

	BYTE byID;
	WORD wIndex;		//지역기라면 0xFFFF	
	short zAreaPos[2];	//x, z
	WORD wForceSerial;
};

#define attack_unit_request_clzo 240
struct _attack_unit_request_clzo {		

	BYTE byID;
	WORD wIndex;		//지역기라면 0xFFFF	

	//#define unit_bullet_arms			0
	//#define unit_bullet_shoulder		1
	BYTE byWeaponPart;	
};

#define attack_test_request_clzo 105
struct _attack_test_request_clzo{

	BYTE	byEffectCode;
	BYTE	byEffectIndex;
	WORD	wBulletSerial;		//bullet 무관 : 0xFFFF
	BYTE	byWeaponPart;		//유닛일경우.. 유닛탑승상태일경우반대시 세팅.. 위에꺼는 싸그리무시.
	short	zTar[2];

	_attack_test_request_clzo()	{
		byEffectCode = 0xFF;
		byEffectIndex = 0xFF;
		wBulletSerial = 0xFFFF;
		byWeaponPart = 0xFF;
	}
};

#define attack_error_result_zocl 255
struct _attack_error_result_zocl {	

	char sFailureCode;
};

#define attack_gen_result_zocl 254
struct _attack_gen_result_zocl {	

	_CHRID idAtter;	
	BYTE byAttackPart;
	BYTE byBulletIndex;
	bool bCritical;		//1이면 크리티컬공격

	BYTE byListNum;

	struct _dam_list{
		_CHRID idDster;	
		WORD wDamage;  //데미지		.. 0이면 미스, 0xFFFF면 반격기걸린상태를 침..
		WORD wLeftHP;
	};

	_dam_list DamList[32];
	
	int size(){
		return sizeof(*this)-sizeof(_dam_list)*(32-byListNum);
	}
};	

#define attack_skill_result_zocl 253
struct _attack_skill_result_zocl {	

	_CHRID idAtter;	
	BYTE byEffectCode;	//	effect_code_skill, effect_code_class ..		
	WORD wSkillIndex;	//	
	BYTE bySkillLv;	//add
	BYTE byAttackPart;
	BYTE byBulletIndex;
	bool bCritical;		//1이면 크리티컬공격

	BYTE byListNum;

	typedef _attack_gen_result_zocl::_dam_list _dam_list;

	_dam_list DamList[32];
	
	int size(){
		return sizeof(*this)-sizeof(_dam_list)*(32-byListNum);
	}
};	

#define attack_force_result_zocl 252
struct _attack_force_result_zocl {	

	_CHRID idAtter;	
	BYTE byForceIndex;
	BYTE byForceLv;		
	short zAreaPos[2];	//지역기일경우 타겟점
	BYTE byAttackPart;
	bool bCritical;		//1이면 크리티컬공격

	BYTE byListNum;

	typedef _attack_gen_result_zocl::_dam_list _dam_list;

	_dam_list DamList[32];
	
	int size(){
		return sizeof(*this)-sizeof(_dam_list)*(32-byListNum);
	}
};	

#define attack_unit_result_zocl 251
struct _attack_unit_result_zocl {	

	_CHRID idAtter;	
	BYTE byWeaponPart;
	WORD wWeaponIndex;
	BYTE byBulletIndex;			
	BYTE byAttackPart;
	bool bCritical;		//1이면 크리티컬공격

	BYTE byListNum;

	typedef _attack_gen_result_zocl::_dam_list _dam_list;

	_dam_list DamList[32];
	
	int size(){
		return sizeof(*this)-sizeof(_dam_list)*(32-byListNum);
	}
};	

#define attack_count_result_zocl 250
struct _attack_count_result_zocl {	

	_CHRID idAtter;	
	BYTE byAttackPart;
	bool bCritical;		//1이면 크리티컬공격

	BYTE byListNum;

	typedef _attack_gen_result_zocl::_dam_list _dam_list;

	_dam_list DamList[32];
	
	int size(){
		return sizeof(*this)-sizeof(_dam_list)*(32-byListNum);
	}
};

#define attack_test_result_zocl 104
struct _attack_test_result_zocl{

	_CHRID	idAtter;	
	BYTE	byBulletIndex;	// => 0xFF 무관..	
	BYTE	byEffectCode;	// 효과코드.. 0xFF면 일반공격
	BYTE	byEffectIndex;	//일반이면 무시..
	BYTE	byEffectLv;
	BYTE	byWeaponPart;	//unit의 경우..	
	short	zTar[2];
};			

#define attack_monster_inform_zocl 249
struct _attack_monster_inform_zocl{		
	
	_CHRID	idAtter;		
	BYTE	byAttackPart;	
	bool	bCritical;			//1이면 크리티컬공격

	_CHRID	idDst;		
	WORD	wDamage;
	WORD	wLeftHP;
};	

#define attack_animus_inform_zocl 248
struct _attack_animus_inform_zocl{		
	
	_CHRID	idAtter;		
	BYTE	byAttackPart;	
	bool	bCritical;			//1이면 크리티컬공격

	_CHRID	idDst;		
	WORD	wDamage;
	WORD	wLeftHP;
};	

#define attack_tower_inform_zocl 136
struct _attack_tower_inform_zocl{		
	
	_CHRID	idAtter;		
	BYTE	byAttackPart;	
	bool	bCritical;			//1이면 크리티컬공격

	_CHRID	idDst;		
	WORD	wDamage;
	WORD	wLeftHP;	
};	

#define monster_chase_change_zocl 13	//all
struct _monster_chase_change_zocl{

	WORD	wMonIndex;
	short	zDstIndex; //-1은 NULL
};

#define mode_change_request_clzo 14
struct _mode_change_request_clzo{

	BYTE	byModeCode;	//	평화모드:0, 전투모드:1 
};

#define mode_change_result_zocl 15
struct _mode_change_result_zocl{	

	BYTE	byModeCode;	//	평화모드:0, 전투모드:1 
};

#define breakdown_equip_item_zocl 16
struct _breakdown_equip_item_zocl{

	WORD	wPlayerIndex;	
	DWORD	dwPlayerSerial;
	WORD	dwEquipVer;

	BYTE	byPartIndex;
	WORD	wItemSerial;
};

#define shield_equip_dur_dec_zocl 17
struct _shield_equip_dur_dec_zocl{

	BYTE	byPartIndex;
	WORD	wSerial;
	WORD	wLeftDurPoint;
};

#define alter_weapon_bullet_inform_zocl 247	
struct _alter_weapon_bullet_inform_zocl {

	WORD	wItemSerial;
	WORD	wLeftNum;
};

#define alter_unit_bullet_inform_zocl 246	
struct _alter_unit_bullet_inform_zocl {

	BYTE	byPart;
	WORD	wLeftNum;
};

//
//item_msg	7
//

#define itembox_state_change_zocl 1
struct _itembox_state_change_zocl{

	WORD wItemBoxIndex;
	DWORD dwOwerSerial;	//누가 버린건 oxffff 
	BYTE byState;	//0: normal(지정자만이 먹을수잇는상태) 
					//1: open(누구든지 먹을수 있는 상태) 
					//2: hurry(소멸임박,깜빡임)
};

#define itembox_take_request_clzo 2	
struct _itembox_take_request_clzo{

	WORD wItemBoxIndex;
	WORD wAddSerial;	//포개지는 아이템일경우 포갤 시리얼.. 포갤곳이 없으면 0xFFFF
};

#define itembox_take_new_result_zocl 3		//_itembox_take_request_clzo의 wAddSerial이 0xFFFF인경우..
struct _itembox_take_new_result_zocl{		
	
	char sErrorCode;	//1;//빈공간없음 2;//기존아템없음 3;//만땅임 4;//없는박스 5;//권한제한 6;//거리제한
	BYTE byItemTableCode;
	WORD wItemIndex;
	DWORD dwCurDurPoint; 
	DWORD dwUptInfo;
	WORD wItemSerial;	

	int size(){
		if(sErrorCode != 0)
			return sizeof(sErrorCode);
		return sizeof(*this);
	}
};

#define itembox_take_add_result_zocl 4		//_itembox_take_request_clzo의 wAddSerial을 넣은경우..
struct _itembox_take_add_result_zocl{		
	
	char sErrorCode;	//1;//빈공간없음 2;//기존아템없음 3;//만땅임 4;//없는박스 5;//권한제한 6;//거리제한
	WORD wItemSerial;	
	BYTE byAmount;

	int size(){
		if(sErrorCode != 0)
			return sizeof(sErrorCode);
		return sizeof(*this);
	}
};

#define throw_storage_request_clzo 5
struct _throw_storage_request_clzo{

	_STORAGE_POS_INDIV Item;
};

#define throw_storage_result_zocl 6
struct _throw_storage_result_zocl{	

	BYTE byErrCode;	//0;//성공, 1;//저장위치틀림 2;//없는아탬 3;//수량초과
};

#define use_potion_request_clzo 7
struct _use_potion_request_clzo{	
	
	_STORAGE_POS_INDIV Item;	
};

#define use_potion_result_zocl 8
struct _use_potion_result_zocl{	//one	

	BYTE byErrCode;	//0;//성공, //1;//저장위치틀림 2;//없는아탬 3;//포션아님 4;//수량초과 5;//회복잠김상태 6;//이미죽은상태
	WORD wPotionSerial;	
	WORD wHP;
	WORD wFP;
	WORD wSP;
	BYTE byLeftNum;		//add(4/19)
};

#define equip_part_request_clzo 9
struct _equip_part_request_clzo{

	_STORAGE_POS_INDIV Item;	
};

#define equip_part_result_zocl 10	
struct _equip_part_result_zocl{

	BYTE byErrCode;	//0;//성공, //1;//저장위치틀림 2;//없는아탬 3;//장착아이템아님 4;//사용할수없는종족 5;//방패들었음 6;//양손무기들었음
};

#define embellish_request_clzo 11	//반지, 목걸이, 총알..착용
struct _embellish_request_clzo{	

	_STORAGE_POS_INDIV Item;
	WORD wChangeSerial;	//바꾸는거라면 벗길 아이템의 시리얼.. 아니면 0xFFFF
};

#define embellish_result_zocl 12
struct _embellish_result_zocl{	

	BYTE byErrCode;	//0;//성공,
};

#define off_part_request_clzo 13
struct _off_part_request_clzo{	

	_STORAGE_POS_INDIV Item;
};

#define off_part_result_zocl 14
struct _off_part_result_zocl{

	BYTE byErrCode;	//0;//성공, 
};	

//제작 스킬사용전에 요거먼저 보내야한댜.
#define make_item_request_clzo 15
struct _make_item_request_clzo{		

	enum { material_num = 5	};

	_STORAGE_POS_INDIV ipMakeTool;	//제작툴이 있는 위치
	WORD wManualIndex;			//제작메뉴얼인덱스

	BYTE byMaterialNum;			//사용할 재료수
	_STORAGE_POS_INDIV ipMaterials[material_num];	//사용할 재료가 있는 위치

	_make_item_request_clzo(){
		byMaterialNum = 0;
	}

	int size(){
		return sizeof(*this)-sizeof(_STORAGE_POS_INDIV)*(material_num-byMaterialNum);
	}
};

//요거받은거 확인하고 제작스킬시전..
#define make_item_result_zocl 16
struct _make_item_result_zocl{	

	BYTE byErrCode;
};

#define upgrade_item_request_clzo 17
struct _upgrade_item_request_clzo {

	_STORAGE_POS_INDIV m_posUpgItem;	
	_STORAGE_POS_INDIV m_posTalik;
	_STORAGE_POS_INDIV m_posToolItem;

	BYTE byJewelNum;
	_STORAGE_POS_INDIV m_posUpgJewel[upgrade_jewel_num];

	_upgrade_item_request_clzo(){
		byJewelNum = 0;
	}

	int size(){
		return sizeof(*this)-sizeof(_STORAGE_POS_INDIV)*(upgrade_jewel_num-byJewelNum);
	}
};

#define upgrade_item_result_zocl 18
struct _upgrade_item_result_zocl {

	BYTE byErrCode;///1;//없는아이템 2;//업글만땅했음 3;//업글안돼는 아이템 
					//5;//더이상해당탈릭을추가할수없음 6;//이아이템에는해당탈릭을추가할수없음
					//100;//랜덤실패, 101;//기존탈릭파괴 102;//아이템파괴
};

#define downgrade_item_request_clzo 19
struct _downgrade_item_request_clzo {	

	_STORAGE_POS_INDIV m_posUpgItem;	
	_STORAGE_POS_INDIV m_posTalik;
	_STORAGE_POS_INDIV m_posToolItem;
};

#define downgrade_item_result_zocl 20
struct _downgrade_item_result_zocl {

	BYTE byErrCode;////1;//없는탈릭 2;//잘못됀탈릭 3;//없는툴 4;//없는아이템 5;//업글안돼는 아이템 6;//업글됀아템아님
};

#define add_bag_request_clzo 21	
struct _add_bag_request_clzo {	

	WORD wBagItemSerial;
};

#define add_bag_result_zocl 22
struct _add_bag_result_zocl {	

	BYTE byErrCode; //0;//성공, 1;//없는가방 2;//5개다쓰고있음
};

#define delete_storage_inform_zocl 23
struct _delete_storage_inform_zocl {

	BYTE byStorageCode;
	WORD wSerial;
};

#define storage_refresh_inform_zocl 24
struct _storage_refresh_inform_zocl {

	BYTE byStorageCode;
	BYTE byItemNum;
	WORD wSerial[100];

	_storage_refresh_inform_zocl() {	byItemNum = 0;	}

	int size() {	return sizeof(*this)-sizeof(WORD)*(100-byItemNum);	}
};

#define alter_item_dur_zocl 25	
struct _alter_item_dur_zocl {

	BYTE	byStorageCode;
	WORD	wItemSerial;
	DWORD	dwDur;
};

//
//map_msg..	8
//
#define move_potal_request_clzo 1
struct _move_potal_request_clzo{

	BYTE byPotalIndex;	
};

#define move_potal_result_zocl 2
struct _move_potal_result_zocl{

	BYTE byRet;	////1;//잘못됀포탈번호 2;//포탈과먼위치 3;//잘못됀맵코드 4;//잘못됀포탈코드 5;//연결좌표영역에러 
	BYTE byMapIndex;	//현재 맵과 다른경우 새로운 맵을 로드.. 로드후 로드완료 메시지를 보내야함
	float fStartPos[3];
	BYTE byZoneCode;	//0이면 같은 존.. 1이면 다른존.. => move_out을 보낸다.
};	

//
//confirm_msg..	9
//

//
//observer_msg.. 10
//
#define observer_move_request_clzo	1
struct _observer_move_request_clzo{

	float fMoveSpeed;
	short zCurPos[3];
	short zTarPos[3];
};

#define observer_move_result_zocl 2
struct _observer_move_result_zocl{

	float fMoveSpeed;
	short zCurPos[3];
	short zTarPos[3];
};


//
//chat_msg.. 2	
//

#define chat_operator_request_clzo 1//CHAT_TYPE_OPERATOR
struct _chat_operator_request_clzo {

	BYTE bySize;
	char sChatData[max_message_size];

	int size()
	{
		return sizeof(*this)-(max_message_size-bySize);
	}
};

#define chat_circle_request_clzo 2//CHAT_TYPE_OPERATOR
struct _chat_circle_request_clzo {

	BYTE bySize;
	char sChatData[max_message_size];

	int size()
	{
		return sizeof(*this)-(max_message_size-bySize);
	}
};

#define chat_whisper_request_clzo 3//CHAT_TYPE_WHISPER
struct _chat_whisper_request_clzo {

	WORD wDstIndex;

	BYTE bySize;
	char sChatData[max_message_size];

	int size()
	{
		return sizeof(*this)-(max_message_size-bySize);
	}
};

#define chat_far_request_clzo 4//CHAT_TYPE_WHISPER
struct _chat_far_request_clzo {

	char szName[max_name_len+1];
	
	BYTE bySize;
	char sChatData[max_message_size];

	int size()
	{
		return sizeof(*this)-(max_message_size-bySize);
	}
};

#define chat_far_failure_zocl 5//플레이어를 못찾앗다..
struct _chat_far_failure_zocl {
};

#define chat_party_request_clzo 6 //CHAT_TYPE_PARTY
struct _chat_party_request_clzo {

	BYTE bySize;
	char sChatData[max_message_size];

	int size()
	{
		return sizeof(*this)-(max_message_size-bySize);
	}
};

#define chat_race_request_clzo 16
struct _chat_race_request_clzo {

	BYTE bySize;
	char sChatData[max_message_size];

	int size()
	{
		return sizeof(*this)-(max_message_size-bySize);
	}
};

#define chat_cheat_request_clzo 7
struct _chat_cheat_request_clzo {

	BYTE bySize;
	char sChatData[max_message_size];

	int size()
	{
		return sizeof(*this)-(max_message_size-bySize);
	}
};

#define chat_manage_request_clzo 77
struct _chat_manage_request_clzo {

	BYTE bySize;
	char sChatData[max_message_size];

	int size()
	{
		return sizeof(*this)-(max_message_size-bySize);
	}
};

#define chat_mgr_whisper_request_clzo 66
struct _chat_mgr_whisper_request_clzo {	

	BYTE bySize;
	char sChatData[max_message_size];

	int size()
	{
		return sizeof(*this)-(max_message_size-bySize);
	}
};

#define chat_message_receipt_udp	8
struct _chat_message_receipt_udp {	

//#define CHAT_TYPE_OPERATOR		0 //운영자..
//#define CHAT_TYPE_CIRCLE		1 //주변전체에..
//#define CHAT_TYPE_WHISPER		2 //시리얼번호를 아는상대와 일대일..
//#define CHAT_TYPE_FAR_WHISPER	3 //시리얼번호를 모르는상대와 일대일..
//#define CHAT_TYPE_PARTY			4 //파티원들에게..
//#define CHAT_TYPE_RACE		5    //동족에게

	BYTE byMessageType;	
	DWORD dwSenderSerial;

	BYTE bySize;
	char sChatData[max_message_size];

	int size()
	{
		return sizeof(*this)-(max_message_size-bySize);
	}
};

#define announ_message_receipt_udp	18
struct _announ_message_receipt_udp {

//#define CHAT_TYPE_OPERATOR		0 //운영자..
//#define CHAT_TYPE_CIRCLE		1 //주변전체에..
//#define CHAT_TYPE_WHISPER		2 //시리얼번호를 아는상대와 일대일..
//#define CHAT_TYPE_FAR_WHISPER	3 //시리얼번호를 모르는상대와 일대일..
//#define CHAT_TYPE_PARTY			4 //파티원들에게..
//#define CHAT_TYPE_RACE		5    //동족에게
//#define CHAT_TYPE_MGRWSP		6 //운영자귓말

	BYTE byMessageType;	
	char szSenderName[max_name_len+1];	//NULL 포함..

	BYTE bySize;
	char sChatData[max_message_size];	//말하는사람 이름 포함

	int size()
	{
		return sizeof(*this)-(max_message_size-bySize);
	}
};

#define chat_lock_inform_zocl	19
struct _chat_lock_inform_zocl {	//add(4/19)

	bool	bLock;	//true면 락 설정..

	int size(){	return sizeof(*this);	}
};

//
//test_msg.. 0
//

#define socket_check_zocl 0
struct _socket_check_zocl {
};

//
//event_msg 11
//

#define level_up_zocl	1
struct _level_up_zocl{

	DWORD dwSerial;//플레이어의 시리얼..
	BYTE byLevel;
};	

#define recover_zocl	2
struct _recover_zocl{

	WORD wHP;
	WORD wFP;
	WORD wST;
};

#define max_hfsp_zocl	3
struct _max_hfsp_zocl{	

	WORD wMaxHP;
	WORD wMaxFP;
	WORD wMaxST;
};

#define exp_alter_zocl	4	
struct _exp_alter_zocl	{

	WORD wExpRate;	//만분율..
};	

#define change_class_command_zocl 5	
struct _change_class_command_zocl {		//전직 명령

	WORD	wCurClassIndex;

};

#define select_class_request_clzo 6	
struct _select_class_request_clzo {		//class 선택

	WORD	wSelClassIndex;
};

#define select_class_result_zocl 7	
struct _select_class_result_zocl {		//자신에게 전직결과

	BYTE	byRetCode;
};

#define change_class_other_zocl 8	
struct _change_class_other_zocl {		//주변에게 효과보여주기위함.

	DWORD	dwPlayerSerial;
	WORD	wClassIndex;
};

#define reward_add_item_zocl 9	
struct _reward_add_item_zocl {	//아이템 보상(전직, 퀘스트 보상)

	BYTE	byTableCode;
	WORD	wItemIndex;
	DWORD	dwDur;
	DWORD	dwLv;
	WORD	wItemSerial;
};

//
//trade_msg 12
//

#define buy_store_request_clzo 3	
struct _buy_store_request_clzo{	

	BYTE byStoreIndex;	//상점 인덱스
	BYTE byBuyNum;	//품목수

	struct _list{
		BYTE byStorageCode;//인벤토리: 0, 장비: 1 장식: 2, 벨트:3, 포스:4
		DWORD dwGoodSerial;//일반은 인덱스
		BYTE byAmount;	
	};

	_list OfferList[trade_item_max_num];	//상위바이트:상품번호, 하위바이트:살갯수

	_buy_store_request_clzo(){
		byBuyNum = 0;
	}

	int size(){
		return sizeof(*this)-sizeof(_list)*(trade_item_max_num-byBuyNum);
	}
};

#define buy_store_success_zocl 4
struct _buy_store_success_zocl{

	DWORD dwLeftDalant;	//남은 달란트	
	DWORD dwLeftGold;	//남은 골드		
	DWORD dwConsumDanlant;	//소모 달란트	
	DWORD dwConsumGold;	//소모 골드		
	BYTE  byDiscountRate;//할인율(백분율)

	BYTE byBuyNum;	//품목수

	struct _list{
		WORD wSerial;
	};

	_list OfferList[trade_item_max_num];

	int size(){
		return sizeof(*this)-sizeof(_list)*(trade_item_max_num-byBuyNum);
	}
};

#define buy_store_failure_zocl 5
struct _buy_store_failure_zocl{	//위 실패시 선택한 품목의 갯수를 보낸다

	DWORD dwDalant;	//보유자금
	DWORD dwGold;
	
	BYTE byErrCodeLen;
	char szErrCode[max_error_code_size];

	_buy_store_failure_zocl(){
		byErrCodeLen = 0;	
	}

	int size(){
		return sizeof(*this)-(max_error_code_size-byErrCodeLen);
	}
};

#define sell_store_request_clzo 6
struct _sell_store_request_clzo{	

	WORD wStoreIndex;	//상점 인덱스
	BYTE bySellNum;	//품목수

	struct _list{
		BYTE byStorageCode;//인벤토리: 0, 장비: 1 장식: 2, 벨트:3, 포스:4
		WORD wSerial;
		BYTE byAmount;
	};

	_list Item[trade_item_max_num];	//상위워드:상품시리얼, 하위바이트:팔갯수

	int size(){
		return sizeof(*this)-sizeof(_list)*(trade_item_max_num-bySellNum);
	}
};

#define sell_store_result_zocl 7
struct _sell_store_result_zocl{	

	BYTE bSucc;	//true 면 성공
	DWORD dwLeftDalant;	//남은 달란트	
	DWORD dwLeftGold;	//남은 골드		
	DWORD dwProfitDanlant;	//번 달란트	
	DWORD dwProfitGold;	//번 골드
	BYTE  byDiscountRate;//할인율(백분율) 
	
	BYTE byErrCodeLen;
	char szErrCode[max_error_code_size];

	_sell_store_result_zocl(){
		byErrCodeLen = 0;
	}

	int size(){
		return sizeof(*this)-(max_error_code_size-byErrCodeLen);
	}
};

#define repair_request_clzo 8
struct _repair_request_clzo{

	BYTE byStoreIndex;	//상점 인덱스
	BYTE byItemNum;	//품목수

	struct _list{
		BYTE byPositon;	//인벤토리: 0, 장비: 1 장식: 2, 벨트:3, 포스:4 링크:5..
		WORD wSerial;
	};

	_list Item[trade_item_max_num];	//상위워드:상품시리얼, 하위바이트:팔갯수

	_repair_request_clzo(){
		byItemNum = 0;
	}

	int size(){
		return sizeof(*this)-sizeof(_list)*(trade_item_max_num-byItemNum);
	}
};

#define repair_result_zocl 9
struct _repair_result_zocl{	

	BYTE bSucc;	//true 면 성공
	DWORD dwLeftDalant;	//남은 달란트	
	DWORD dwLeftGold;	//남은 골드		
	DWORD dwConsumDanlant;	//소모 달란트	
	DWORD dwConsumGold;	//소모 골드	
	
	BYTE byErrCodeLen;
	char szErrCode[max_error_code_size];

	_repair_result_zocl(){
		byErrCodeLen = 0;
	}

	int size(){
		return sizeof(*this)-(max_error_code_size-byErrCodeLen);
	}
};

#define store_list_request_clzo 10
struct _store_lis_request_clzo{	
};

#define store_list_result_zocl 11
struct _store_list_result_zocl{	

	enum	{	max_store_per_map = 20 };

	BYTE byStoreNum;

	struct _store_pos{
		WORD wStoreIndex;
		float fPos[3];
	}; 

	_store_pos StorePos[max_store_per_map];

	int size(){
		return sizeof(*this)-sizeof(_store_pos)*(max_store_per_map-byStoreNum);
	}
};

#define exchange_dalant_for_gold_request_clzo 12
struct _exchange_dalant_for_gold_request_clzo {

	DWORD dwDalant;
};

#define exchange_gold_for_dalant_request_clzo 13
struct _exchange_gold_for_dalant_request_clzo {

	DWORD dwGold;
};

#define exchange_money_result_zocl 14		
struct _exchange_money_result_zocl {

	BYTE byErrCode;	//0;//성공, 1;//금액부족 
	DWORD dwLeftGold;		
	DWORD dwLeftDalant;
};

#define economy_history_inform_zocl 15	//	경제지표히스토리(지급환율) 접속시에 한번 보내줌
struct _economy_history_inform_zocl {	//upt(4/19).. 

	WORD wEconomyGuide[RACE_NUM][MAX_ECONOMY_HISTORY];	//0~11.. 0: 과거..11: 최근	//
};

#define economy_rate_inform_zocl 16	//	경제지표(지급환율) 접속시와 변경될때마다 3종족..
struct _economy_rate_inform_zocl {	

	bool	bStart;	//true면 첨 게임 시작시 받는것으로 wEconomyGuide는 적용하지 않는다.. false일경우만 아래꺼 적용해서 history에 추가..
	float	fPayExgRate;
	float	fTexRate;
	WORD	wMgrValue;	//관리값	//add(4/22)
	WORD	wEconomyGuide[RACE_NUM];	//upt(4/19)

	int size()	{	if(bStart) return sizeof(*this)-sizeof(wEconomyGuide); return sizeof(*this);	}
};

//
//ui_msg	13
//

#define sf_link_change_result_zocl 2
struct _sf_link_change_result_zocl{

	BYTE bySize;	//0이면 성공
	char szErrorCode[max_error_code_size];

	int size(){
		return sizeof(*this)-(max_error_code_size-bySize);
	}
};

#define force_inven_change_request_clzo 3
struct _force_inven_change_request_clzo{	

	_STORAGE_POS_INDIV Item;	
	WORD wReplaceSerial;	//빈곳이동은 0xffff
};

#define force_inven_change_result_zocl 4
struct _force_inven_change_result_zocl{	

	BYTE byErrCode;
};

#define animus_inven_change_request_clzo 113
struct _animus_inven_change_request_clzo{		

	_STORAGE_POS_INDIV Item;	
	WORD wReplaceSerial;	//빈곳이동은 0xffff
};

#define animus_inven_change_result_zocl 114
struct _animus_inven_change_result_zocl{		

	BYTE byErrCode;
};

#define res_separation_request_clzo 11	//inventory빈곳에 나눈다.
struct _res_separation_request_clzo{		

	WORD wStartSerial;
	BYTE byMoveAmount;
};

#define res_separation_result_zocl 12	
struct _res_separation_result_zocl{

	BYTE byErrCode;	//0: 성공, 1: 갯수모자름 , 2: 전부이동함, 3: 자원,원석이아님 4: 남은인벤토리없음, 5: 그런시리얼없음 6: 사용하지않는가방
	WORD wParentSerial;
	BYTE byParentAmount;
	WORD wChildSerial;
	BYTE byChildAmount;
};

#define res_division_request_clzo 13	//다른 inventory와 나눠서 병합시킨다.
struct _res_division_request_clzo{	

	WORD wStartSerial;	
	WORD wTarSerial;
	BYTE byMoveAmount;
};

#define res_division_result_zocl 14
struct _res_division_result_zocl{

	BYTE byErrCode;	//0: 성공, 1: 갯수모자름, 3: 자원,원석이아님, 4: 더많은양을이동, 5: 그런시리얼없음, 6: 같은종류의자원,원석아님. 7: 사용하지않는가방, 8: 더많이이동함 9: 이동할곳이넘침
	WORD wParentSerial;		//0xFFFF이면 슬롯삭제
	BYTE byParentAmount;	
	WORD wChildSerial;		//new serial
	BYTE byChildAmount;
};

#define potionsocket_change_request_clzo 15
struct _potionsocket_change_request_clzo{

	_STORAGE_POS_INDIV Item;
//	_STORAGE_POS_INDIV TarPos;//빈칸으로 이동은  _STORAGE_POS_INDIV::wItemSerial = 0xFFFF..
	BYTE byTargetStorageCode;	//upt(4/24).. 무조건 빈칸으로 이동..
};

#define potionsocket_change_result_zocl 16
struct _potionsocket_change_result_zocl{	

	BYTE byErrCode;	//0: 성공, 1;//사용가능가방아님 2;//존재하지않음 3;//포션아님 4;//옮길공간없음
};

#define potionsocket_separation_request_clzo 17	//inventory빈곳에 나눈다.
struct _potionsocket_separation_request_clzo{		

	struct _pos{
		BYTE byPositon;	//인벤토리: 0, 벨트: 1, 
	};

	_pos StartPos;	
	_pos TargetPos;
	WORD wStartSerial;
	BYTE byMoveAmount;

};

#define potionsocket_separation_result_zocl 18
struct _potionsocket_separation_result_zocl{

	char sErrorCode;	//0: 성공, -1: 갯수모자름 , -2: 전부이동함, -3: 포션이 아니다. -4: 남은 인벤토리가 없다, -5: 그런 시리얼은 없다
	WORD wParentSerial;
	BYTE byParentAmount;
	WORD wChildSerial;
	BYTE byChildAmount;
};

#define potionsocket_division_request_clzo 19	//다른 inventory와 나눠서 병합시킨다.
struct _potionsocket_division_request_clzo{	

	struct _pos{
		BYTE byPositon;	//인벤토리: 0, 벨트: 1, 
	};

	_pos StartPos;	
	_pos TargetPos;
	WORD wStartSerial;	
	WORD wTarSerial;
	BYTE byMoveAmount;
};

#define potionsocket_division_result_zocl 20	
struct _potionsocket_division_result_zocl{

	char sErrorCode;	//0: 성공, -1: 갯수 모자름, -3: 포션이 아니다, -4: 더 많은 양을 이동, -5: 그런 시리얼은 없다, -6: 같은 종류의 포션이 아님.
	WORD wSerial;	//실패면 byParentAmount 부터 0xffff로 옴
	BYTE byParentAmount;	//0이면 슬롯 삭제
	WORD wChildSerial;	
	BYTE byChildAmount;
};

//
//resource_msg	14
//

#define mine_start_request_clzo 1
struct _mine_start_request_clzo {

	BYTE byMineIndex;//광산 더미 인덱스 
};

#define mine_start_result_zocl 2
struct _mine_start_result_zocl {

	BYTE byErrCode;	//0: 성공, 2: 범위에러, 3: 채굴장비에러, 4: 현재 채굴모드임
	DWORD dwMineDelayTime;	//채굴시간	
};

#define mine_cancle_request_clzo 3	
struct _mine_cancle_request_clzo {

	WORD wBatterySerial;	//바떼리 시리얼
};

#define mine_cancle_result_zocl 4
struct _mine_cancle_result_zocl {

	BYTE byErrCode;	//0: 성공, 1: 바떼리에러, 3: 지연시간 에러 4:채굴장비에러
	WORD wBatteryLeftDur;	//남은 바떼리 용량
	WORD wEquipLeftDur;		//남은 채굴장비 내구성
};

#define mine_complete_request_clzo 5
struct _mine_complete_request_clzo {

	WORD wBatterySerial;	//바떼리 시리얼			
	BYTE byOreIndex;	//원석광물 인덱스
};

#define mine_complete_result_zocl 6
struct _mine_complete_result_zocl {

	BYTE byErrCode;//0: 성공, 1: 지연시간에러, 2: 원석가방에러, 3: 담을공간없음, 4:원석종류에러,5: 베터리 에러, 6;채굴장비 에러
	WORD wEquipLeftDur;		//남은 채굴장비 내구성
	WORD wBatteryLeftDur;	//남은 바떼리 용량

	BYTE byOreIndex;
	WORD wOreSerial;
	BYTE byOreDur;	//1이면 위 시리얼은 새로운 시리얼 1이상이면 기존 시리얼
	
};

#define mine_start_result_other_zocl 22
struct _mine_start_result_other_zocl {

	DWORD	dwSerial;
	WORD	wIndex;
};

#define mine_end_result_other_zocl 33
struct _mine_end_result_other_zocl {

	DWORD	dwSerial;
	WORD	wIndex;
};

#define ore_cutting_request_clzo 7	
struct _ore_cutting_request_clzo {

	BYTE byCuttingHouse;
	WORD wOreSerial;
	BYTE byProcessNum;
};

#define ore_cutting_result_zocl 8
struct _ore_cutting_result_zocl {

	BYTE byErrCode;	//0:성공 1:원석 가방, 시리얼실패 2:수량실패 3:소지금부족 4:가공임시공간 안비움 
	BYTE byLeftNum;
	DWORD dwLeftDalant;
	DWORD dwConsumDalant;
	
	BYTE byCuttingNum;

	struct _list {
		WORD wResIndex;		//자원인댁스
		BYTE byAddAmount;	//추가되는 양
	};

	_list ResList[100];

	_ore_cutting_result_zocl(){
		byCuttingNum = 0;
	}

	int size(){
		return sizeof(*this)-sizeof(_list)*(100-byCuttingNum);
	}
};

#define ore_into_bag_request_clzo 9	
struct _ore_into_bag_request_clzo {

	WORD wResIndex;
	WORD wSerial; //빈곳에 넣은거라면 0xFFFF
	BYTE byAddAmount;//추가되는 양
};

#define ore_into_bag_result_zocl 10
struct _ore_into_bag_result_zocl {

	BYTE byErrCode;	//0:성공 1:가공모드아님 2:자원없거나부족 3:사용가방아님 4:가방공간부족 5:가방에없는슬롯 6:슬롯당용량초과, 7:가방과내용이다름
	WORD wNewSerial; //새로운 시리얼.. 기존꺼에다 쌓은경우는 기존의시리얼
};

#define cutting_complete_request_clzo 11
struct _cutting_complete_request_clzo {

	BYTE byNpcRace;	//npc종족.. 0:벨리토, 1:코라 2:아크레시아 
};

#define cutting_complete_result_zocl 12	
struct _cutting_complete_result_zocl {

	DWORD dwLeftGold;
};

//
//party_msg		16
//

//특정대상을 파티원으로 모집요청
//조건사항.. 자신은 무소속또는 보스..상대는 무소속인상태에서만 요청가능
#define party_join_invitation_clzo 1
struct _party_join_invitation_clzo {

	WORD wDstIndex;
};

//특정인이 자신에게 파티에 모집요청함
#define party_join_invitation_question_zocl 2
struct _party_join_invitation_question_zocl {
	
	_CLID idBoss;
};

//특정인이 자신에게 파티에 모집요청의 답변
#define party_join_invitation_answer_clzo 3
struct _party_join_invitation_answer_clzo {

	_CLID idBoss;
};

//특정파티장에게 자신을 파티원으로 지원요청
//조건사항.. 자신은 무소속.. 상대는 보스인상태에서만 요청가능
#define party_join_application_clzo 4
struct _party_join_application_clzo {

	WORD wBossIndex;
};

//특정인이 자신에게 파티원으로 지원함
#define party_join_application_question_zocl 5
struct _party_join_application_question_zocl {

	_CLID idApplicant;
};

//특정인이 자신에게 파티원으로 지원요청의 답변
#define party_join_application_answer_clzo 6
struct _party_join_application_answer_clzo {
	
	_CLID idApplicant;
};

//모집요청승락시 자신에게 조직원리스트를 보냄..
#define party_join_joiner_result_zocl 7
struct _party_join_joiner_result_zocl {

	struct _list{
		DWORD dwSerial;
		char szAvatorName[max_name_len+1];
	};

	BYTE byListNum;
	_list List[member_per_party];

	_party_join_joiner_result_zocl(){ byListNum = 0; }

	int size(){ return sizeof(*this)-sizeof(_list)*(member_per_party-byListNum); }
};

//모집요청승락시 기존조직원들에게 요청승락을 알림..
#define party_join_member_result_zocl 8
struct _party_join_member_result_zocl {

	DWORD dwJoinerSerial;
	char szJoinerName[max_name_len+1];
};


//스스로 조직에서 탈퇴함을 요청
#define party_leave_self_request_clzo 9
struct _party_leave_self_request_clzo {

};

//스스로 조직에서 탈퇴함을 자신포함조직원에게 알림..
//보스의 탈퇴의경우 다음끗발자가 보스로 승계
#define party_leave_self_result_zocl 10
struct _party_leave_self_result_zocl {

	DWORD dwExiterSerial;		//0xFF이면 실패메시지..자신에게만
	bool bWorldExit;	

};

//조직에서 강퇴시킴을 요청
#define party_leave_compulsion_request_clzo 11
struct _party_leave_compulsion_request_clzo {

	DWORD dwExiterSerial;
};

//조직에서 강퇴됨을 다른조직원에게 알림..
#define party_leave_compulsion_result_zocl 12
struct _party_leave_compulsion_result_zocl {

	DWORD dwExiterSerial; //0xFF이면  실패메시지..자신에게만
};

//조직을 해체요청..보스만 가능
#define party_disjoint_request_clzo 13
struct _party_disjoint_request_clzo {

};

//조직이 해체됬음을 조직원에게 알림..
#define party_disjoint_result_zocl 14
struct _party_disjoint_result_zocl {
	
	BYTE bySuccess;	//(bool) false면 자신에게만 옴.. true면 전 파티원에게..
};

//보스승계요청
#define party_succession_request_clzo 15
struct _party_succession_request_clzo {

	DWORD dwSuccessorSerial;
};

//보스승계 알림
#define party_succession_result_zocl 16
struct _party_succession_result_zocl {

	DWORD dwSuccessorSerial;	//0xff이면 실패.. 자기한테만..
};

//파티멤버 잠금요청
#define party_lock_request_clzo 115
struct _party_lock_request_clzo {

	bool bLock;	//true..잠금.. false.. 품..	
};

//파티멤버 잠금요청 알림
#define party_lock_result_zocl 116
struct _party_lock_result_zocl {	

	BYTE byLock;	//1..잠금.. 0.. 품..	0xFF : 에러..
};

#define party_member_info_upd 17
struct _party_member_info_upd {

	DWORD dwMemSerial; //맴버의 시리얼.
	WORD wHP;
	WORD wFP;
	WORD wSP;	
	WORD wMaxHP;	
	WORD wMaxFP;	
	WORD wMaxSP;
	BYTE byLv;	
	BYTE byMapCode;
	short zPos[2]; //x, z

	BYTE byContEffectNum;

	struct _EFFECT {
		WORD wEffectCode;	//맨왼쪽비트... effect_code.. 그다음부턴 SFIndex..	
		BYTE byEffectLv;	
		WORD wLeftSec;//작은것일수록 최근에 걸린거..	//upt(4/24)
	};	

	_EFFECT Effect[CONT_SF_SIZE*2];
	
	int size(){ return sizeof(*this)-sizeof(_EFFECT)*(CONT_SF_SIZE*2-byContEffectNum); }
};

#define party_member_hp_upd 18
struct _party_member_hp_upd {	

	DWORD dwMemSerial; //맴버의 시리얼.
	WORD wHP;
};

#define party_member_fp_upd 19
struct _party_member_fp_upd {	

	DWORD dwMemSerial; //맴버의 시리얼.
	WORD wFP;
};

#define party_member_sp_upd 118
struct _party_member_sp_upd {	

	DWORD dwMemSerial; //맴버의 시리얼.
	WORD wSP;
};

#define party_member_lv_upd 119	
struct _party_member_lv_upd {	

	DWORD dwMemSerial; //맴버의 시리얼.
	BYTE byLv;
};

#define party_member_pos_upd 120	
struct _party_member_pos_upd {	

	DWORD dwMemSerial; //맴버의 시리얼.
	BYTE byMapCode;
	short zPos[2];
};

#define party_member_max_hfsp_upd 121
struct _party_member_max_hfsp_upd {

	DWORD dwMemSerial; //맴버의 시리얼.
	WORD wMaxHP;
	WORD wMaxFP;
	WORD wMaxSP;
};

#define party_member_effect_upd 20
struct _party_member_effect_upd {

	DWORD dwMemSerial; //맴버의 시리얼.
	BYTE byAlterCode;	//0: 삽입.. 1: 제거..
	WORD wEffectCode;	//맨왼쪽비트... effect_code.. 그다음부턴 SFIndex..
	BYTE byEffectLv;
};

#define party_exp_division_zocl 21
struct _party_exp_division_zocl {	//del..

	WORD wExpRate; 
};

//
//cheat_msg		15
//

#define cheat_force_all_use_zocl 1
struct _cheat_force_all_use_zocl {

	WORD wStartSerial;
};

//
//effect_msg		17
//

//포스요청..일시보조, 지속..
#define force_request_clzo 1
struct _force_request_clzo {

	WORD wForceSerial;
	_CHRID idDst;//시전대상..	
};

//포스를 자신에게 알림
#define force_result_one_zocl 2
struct _force_result_one_zocl {

	BYTE byErrCode;//0:성공, //1:없는포스아이템, //2:FP부족 //3:없는대상 4;//일시보조,지속아님 5;//사용할수없는포스 6;//지속피해보조포스사용제한 7;//지속이득보조포스사용제한 8;//시전실패 100;//플레이어 상황상 사용못하는 포스
//	WORD wLeftFP;	//del(4/27)
	DWORD dwItemCum; //포스아이템성공누적치..
};

//포스의 결과를 주변에 알림
#define force_result_other_zocl 3
struct _force_result_other_zocl {
	
	BYTE byRetCode;	//0이면 성공..	
	BYTE byForceIndex;
	BYTE byForceLv;
	_CHRID idPerformer;	//시전자
	_CHRID idDster;		//시전대상..
};

//스킬요청..일시보조, 지속..
#define skill_request_clzo 4	
struct _skill_request_clzo {
	
	BYTE bySkillIndex;
	_CHRID idDst;//시전대상..	
};

//스킬를 자신에게 알림
#define skill_result_one_zocl 5
struct _skill_result_one_zocl {

	BYTE byErrCode;//0:성공, //1:할수없는스킬 //2:FP부족 //3:없는대상 4;//일시보조,지속아님 5;//사용할수없는스킬 6;//지속이득보조스킬사용제한 7;//시전실패, 100;//플레이어 상황상 사용못하는 스킬
//	WORD wLeftFP;	//del(4/27)
};

//스킬의 결과를 주변에 알림
#define skill_result_other_zocl 6
struct _skill_result_other_zocl {
	
	BYTE byRetCode;	//0이면 성공..
	_CHRID idPerformer;	//시전자
	_CHRID idDster;		//시전대상..
	BYTE bySkillIndex;
	BYTE bySkillLv;
};

//클래스스킬요청..일시보조, 지속..
#define class_skill_request_clzo 44	
struct _class_skill_request_clzo {
	
	WORD wSkillIndex;
	_CHRID idDst;//시전대상..	
};

//스킬를 자신에게 알림
#define class_skill_result_one_zocl 45
struct _class_skill_result_one_zocl {

	BYTE byErrCode;//0:성공, //1:할수없는스킬 //2:FP부족 //3:없는대상 4;//일시보조,지속아님 5;//사용할수없는스킬 6;//지속이득보조스킬사용제한 7;//시전실패, 100;//플레이어 상황상 사용못하는 스킬
//	WORD wLeftFP;	//del(4/27)
};

//스킬의 결과를 주변에 알림
#define class_skill_result_other_zocl 46
struct _class_skill_result_other_zocl {
	
	BYTE byRetCode;	//0이면 성공..
	_CHRID idPerformer;	//시전자
	_CHRID idDster;		//시전대상..
	WORD wSkillIndex;
};

#define effect_add_inform_zocl 7
struct _effect_add_inform_zocl {

	BYTE byLv;	
	WORD wEffectCode;	//맨왼쪽비트... effect_code.. 그다음부턴 SFIndex..
};

#define effect_remove_inform_zocl 8
struct _effect_remove_inform_zocl {

	WORD wEffectCode;	
};

#define set_fp_inform_zocl 9	
struct _set_fp_inform_zocl {

	WORD wFP;
};

#define set_hp_inform_zocl 10
struct _set_hp_inform_zocl {

	WORD wHP;
};

#define set_sp_inform_zocl 11
struct _set_sp_inform_zocl {

	WORD wSP;
};

#define insert_item_inform_zocl 12
struct _insert_item_inform_zocl {
						
	BYTE byStorageCode;	//인벤토리:0, 장비:1, 장식: 2, 벨트:3, 포스:4, 링크:5
	BYTE byTableCode;	
	WORD wItemIndex;	
	DWORD dwDurPoint;	//중첩아이템은 수량.. 내구아이템은 상위워드(한계내구성) 하위워드(현재내구성)
	WORD wSerial;		
};

#define stun_inform_zocl 13
struct _stun_inform_zocl {

	_CHRID idStun;	
};

#define last_effect_change_inform_zocl 14
struct _last_effect_change_inform_zocl {

	BYTE  byObjID;	//obj_id_player.. obj_id_monter..	
	DWORD dwSerial;
	WORD wLastContEffect;	
};

#define make_tower_request_clzo 15
struct _make_tower_request_clzo {	

	enum	{	max_material = 30	};	

	WORD	wSkillIndex;	
	short	zPos[3];
	WORD	wTowerItemSerial;
	
	struct __material{
		WORD wItemSerial;
		BYTE byMaterSlotIndex;	//가드타워데이터 필드중 재료정의 필드의 순서 0 ~ 2
		BYTE byAmount;
	};

	BYTE	byMaterialNum;
	__material Material[max_material];

	_make_tower_request_clzo()	{	byMaterialNum = 0;	}
	int size()	{ if(byMaterialNum > max_material) byMaterialNum = 0;	return sizeof(*this) - sizeof(__material) * (max_material - byMaterialNum);	}
};

#define make_tower_result_zocl 16
struct _make_tower_result_zocl {	

	BYTE	byErrCode;
	DWORD	dwTowerObjSerial;
	WORD	wLeftFP;	//add(4/25)
};

#define back_tower_request_clzo 17	//타워해체
struct _back_tower_request_clzo {	

	DWORD	dwTowerObjSerial;
};

#define back_tower_result_zocl 18	
struct _back_tower_result_zocl {	

	BYTE	byErrCode;
	WORD	wItemSerial;
	WORD	wLeftHP;
};

#define alter_tower_hp_zocl 19
struct _alter_tower_hp_zocl {	

	WORD	wItemSerial;
	WORD	wLeftHP;	//0이면 파괴..
};

#define tower_complete_inform_zocl 20	//가드타워완성..
struct _tower_complete_inform_zocl {		//add(4/24)

	DWORD	dwTowerObjSerial;
};

#define alter_cont_effect_time_zocl 21	//걸려있는 지속효과의 남은 시간이 바뀜
struct _alter_cont_effect_time_zocl {		//add(4/24)

	struct __list
	{
		WORD	wEffectCode;
		WORD	wLeftSec;
	};

	BYTE	byEffectNum;
	__list	List[CONT_SF_SIZE];

	_alter_cont_effect_time_zocl()	{	byEffectNum = 0;	}
	int size()	{	if(byEffectNum > CONT_SF_SIZE)	byEffectNum = 0; return sizeof(*this)-sizeof(__list)*(CONT_SF_SIZE-byEffectNum);	}
};

//
//dtrade_msg 18... 
//

//ASK
#define d_trade_ask_request_clzo 1
struct _d_trade_ask_request_clzo {	//wDstIndex에게 거래를 요청

	WORD wDstIndex;	
};

#define d_trade_ask_result_clzo 2
struct _d_trade_ask_result_clzo {	//d_trade_ask_request_clzo의 결과로 자신한테옴

	BYTE byErrCode;//0;성공//1;//자신이이미트레이드중 2;//거리가멈 3;//유효한대상아님 4;//상대가트레이드중
};

#define d_trade_ask_inform_zocl 3
struct _d_trade_ask_inform_zocl {	//idAsker가 자신한테 거래를 제의함

	_CLID idAsker;
};

//ANSWER
#define d_trade_answer_request_clzo 4
struct _d_trade_answer_request_clzo {	//제의를 받아들임

	_CLID idAsker;
};

#define d_trade_answer_result_zocl 5	//실패시에만..
struct _d_trade_answer_result_zocl {	//제의 받아들임의 실패	

	BYTE byErrCode;	//1;//유효한ask가아님 2;//asker가이미트레이드모드 3;//answer가이미트레이드모드 4;//asker가자신을요청중아님 5;//거리가 멈 6;//유효한 대상이 아님
};

//START
#define d_trade_start_inform_zocl 6
struct _d_trade_start_inform_zocl {	//거래대상자 각각의 인벤토리 빈칸수..빈칸수만큼 거래가능

	_CLID idAsker;
	BYTE byAskerEmptyNum;	
	_CLID idAnswer;
	BYTE byAnswerEmptyNum;	
};

//CANCLE
#define d_trade_cancle_request_clzo 7
struct _d_trade_cancle_request_clzo {	//거래취소

};

#define d_trade_cancle_result_zocl 8
struct _d_trade_cancle_result_zocl {	//취소자에게
	
	BYTE byErrCode; //0이면 성공..//1;//트레이드아님 2;//락걸린상태 
};

#define d_trade_cancle_inform_zocl 9
struct _d_trade_cancle_inform_zocl {	//취소당한 사람한테	

};

//LOCK
#define d_trade_lock_request_clzo 10
struct _d_trade_lock_request_clzo {		//잠금

};

#define d_trade_lock_result_zocl 11
struct _d_trade_lock_result_zocl {		//잠근자에게
	
	BYTE	byErrCode; //0이면 성공..1;//트레이드모드아님 2;//락걸린상태
};

#define d_trade_lock_inform_zocl 12
struct _d_trade_lock_inform_zocl {		//대상에게

};	

//ADD
#define d_trade_add_request_clzo 13
struct _d_trade_add_request_clzo {		//거래목록추가

	BYTE byStorageCode;
	DWORD dwSerial;
	BYTE bySlotIndex;
	BYTE byAmount;	
};

#define d_trade_add_result_zocl 14
struct _d_trade_add_result_zocl {
	
	BYTE byErrCode; //0이면 성공//1;//트레이드모드아님 2;//락상태임 3;//유효한슬롯이 아님 4;//채워진슬롯 5;//없는아탬 6;//잘못됀양
};

#define d_trade_add_inform_zocl 15
struct _d_trade_add_inform_zocl {

	BYTE bySlotIndex;
	BYTE byTableCode;
	WORD wItemIndex;
	DWORD dwDurPoint;
	DWORD dwUdtInfo;
	BYTE byAmount;
	BYTE byEmptyInvenNum;
};

#define d_trade_del_request_clzo 16
struct _d_trade_del_request_clzo {		//거래목록에서 삭제

	BYTE bySlotIndex;
};

#define d_trade_del_result_zocl 17
struct _d_trade_del_result_zocl {
	
	BYTE byErrCode; //0이면 성공.//0이면 성공//1;//트레이드모드아님 2;//락상태임 3;//유효한슬롯이 아님 4;//빈슬롯
};

#define d_trade_del_inform_zocl 18
struct _d_trade_del_inform_zocl {

	BYTE bySlotIndex;
	BYTE byEmptyInvenNum;
};

#define d_trade_bet_request_clzo 19
struct _d_trade_bet_request_clzo {		//금액 배팅

	//#define money_unit_dalant	0
	//#define money_unit_gold	1

	BYTE byMoneyUnit;
	DWORD dwBetAmount;
};

#define d_trade_bet_result_zocl 20
struct _d_trade_bet_result_zocl {
	
	BYTE byErrCode; //0이면 성공.//1;//달란트부족 2;//골드부족 3;//단위에러 4;//락상태임
};

#define d_trade_bet_inform_zocl 21
struct _d_trade_bet_inform_zocl {

	BYTE byMoneyUnit;
	DWORD dwBetAmount;
};

#define d_trade_ok_request_clzo 22
struct _d_trade_ok_request_clzo {	//ok버튼

};

#define d_trade_ok_result_zocl 23
struct _d_trade_ok_result_zocl {
	
	BYTE byErrCode; //0이면 성공..	100이면 상대에 넣을 공간이 부족
};

#define d_trade_ok_inform_zocl 24
struct _d_trade_ok_inform_zocl {

};	

#define d_trade_accomplish_inform_zocl 25
struct _d_trade_accomplish_inform_zocl {	//거래 성사
	
	DWORD	dwDalant;
	DWORD	dwGold;
	WORD	wStartSerial;
	bool	bSucc;	//성공여부.. false면 거래의 모든과정을 초기화함
};

#define d_trade_close_inform_zocl 26	
struct _d_trade_close_inform_zocl {	//거래 서버직권 취소	

	BYTE byCloseCode;	
};

#define d_trade_unit_info_inform_zocl 27	//유닛을 올렸을때 상대에게 유닛의 정보를 알린다.
struct _d_trade_unit_info_inform_zocl {		

	BYTE	byTradeSlotIndex;	//트레이드창 인덱스
	BYTE	byFrame;	
	WORD	wGauge;		
	BYTE	byPart[UNIT_PART_NUM];
	DWORD	dwBullet[UNIT_BULLET_NUM];
	DWORD	dwSpare[max_unit_spare];
	int		nDebtFee;	//채무 보관료, 견인료
};

#define d_trade_unit_add_inform_zocl 28	//거래가 성사된후 유닛이 폼함되어있으면 유닛데이터를 보낸다
struct _d_trade_unit_add_inform_zocl {		

	WORD	wUnitKeySerial;
	BYTE	bySlotIndex;
	BYTE	byFrame;	//0xFF면 빈 슬롯
	WORD	wGauge;		//hp
	BYTE	byPart[UNIT_PART_NUM];
	DWORD	dwBullet[UNIT_BULLET_NUM];
	DWORD	dwSpare[max_unit_spare];
	int		nKeepingFee;	//채무 보관료
	int		nPullingFee;	//채무 견인료 
};

//dungeon_msg.. 19

#define battledungeon_open_request_clzo 1
struct _battledungeon_open_request_clzo	{

	DWORD dwItemSerial;
};

#define battledungeon_open_result_zocl 2
struct _battledungeon_open_result_zocl	{

	BYTE byRetCode;	
};

#define dungeongate_create_zocl 3
struct _dungeongate_create_zocl	{

	WORD wGateIndex;
	DWORD dwGateSerial;
	DWORD dwOpenerSerial;
	WORD wItemRecIndex;
	BYTE byState;
	short zPos[3];
};

#define dungeongate_destroy_zocl 4
struct _dungeongate_destroy_zocl {
		
	WORD wGateIndex;
	DWORD dwGateSerial;
};

#define dungeongate_fixpositon_zocl 5
struct _dungeongate_fixpositon_zocl {

	WORD wGateIndex;
	DWORD dwGateSerial;
	DWORD dwOpenerSerial;
	WORD wItemRecIndex;
	BYTE byState;
	short zPos[3];
};

#define dungeongate_state_change_zocl 6
struct _dungeongate_state_change_zocl {

	BYTE byState;
	WORD wGateIndex;
	DWORD dwGateSerial;
};

#define goto_dungeon_request_clzo 7
struct _goto_dungeon_request_clzo {	

	WORD	wGateIndex;
	DWORD	dwGateSerial;
};

#define goto_dungeon_result_zocl 8
struct _goto_dungeon_result_zocl {	//	==> new_pos_start_request_clzo로 연결..

	BYTE	byRetCode;	
	BYTE	byMapCode;
	short	zPos[3];
	WORD	wLeftSec;	//남은초 
	WORD	wTotalKillNum;	
	WORD	wMissionKillNum;
};

#define close_dungeon_inform_zocl 9
struct _close_dungeon_inform_zocl {

	BYTE byCompleteCode;//	CharacterDB.h.. dungeon_complete_code_timeout 0 (던젼이 닫히는 이유..)
};

#define outof_dungeon_request_clzo 10
struct _outof_dungeon_request_clzo {

};

#define outof_dungeon_result_zocl 11
struct _outof_dungeon_result_zocl {		//==> new_pos_start_request_clzo로 연결..

	BYTE	byRetCode;	
	BYTE	byMapCode;
	short	zPos[3];
};

#define killmon_dungeon_inform_zocl 12
struct _killmon_dungeon_inform_zocl {	

	WORD	wTotalKillNum;
	WORD	wMissionKillNum;
};

//unit_msg.. 23

#define unit_frame_buy_request_clzo 1
struct _unit_frame_buy_request_clzo {	

	BYTE byFrameCode;
};

#define unit_frame_buy_result_zocl 2
struct _unit_frame_buy_result_zocl {	

	BYTE    byRetCode;
	BYTE	byFrameCode;
	BYTE	byAddUnitSlot;
	BYTE	byKeyIndex;
	WORD	wKeySerial;

	DWORD	dwLeftMoney[MONEY_UNIT_NUM];
	DWORD	dwConsumMoney[MONEY_UNIT_NUM];
};

#define unit_sell_request_clzo 11
struct _unit_sell_request_clzo {	

	BYTE bySlotIndex;
};

#define unit_sell_result_zocl 22
struct _unit_sell_result_zocl {	

	BYTE    byRetCode;
	BYTE	bySlotIndex;
	WORD	wKeySerial;

	DWORD	dwNonPayDalant;
	DWORD	dwAddMoney[MONEY_UNIT_NUM];
	DWORD	dwLeftMoney[MONEY_UNIT_NUM];
};

#define unit_part_tuning_request_clzo 3
struct _unit_part_tuning_request_clzo {	

	BYTE	bySlotIndex;

	BYTE    byTuningNum;
	_tuning_data	TuningList[UNIT_PART_NUM];

	_unit_part_tuning_request_clzo(){
		byTuningNum = 0;
	}

	int size(){
		return sizeof(*this)-sizeof(_tuning_data)*(UNIT_PART_NUM-byTuningNum);
	}
};

#define unit_part_tuning_result_zocl 4
struct _unit_part_tuning_result_zocl {	

	BYTE    byRetCode;
	BYTE	bySlotIndex;
	BYTE	byPart[UNIT_PART_NUM];
	DWORD   dwBullet[UNIT_BULLET_NUM];	//add(4/24)

	int		nCost[MONEY_UNIT_NUM];	
	DWORD	dwLeftMoney[MONEY_UNIT_NUM];
};

#define unit_frame_repair_request_clzo 5
struct _unit_frame_repair_request_clzo {	

	BYTE	bySlotIndex;
};

#define unit_frame_repair_result_zocl 6
struct _unit_frame_repair_result_zocl {	

	BYTE    byRetCode;
	BYTE	bySlotIndex;
	WORD	wNewGauge;

	DWORD	dwConsumDalant;	//수리비
	DWORD	dwLeftDalant;	//남은 달란트
};

#define unit_bullet_fill_request_clzo 7
struct _unit_bullet_fill_request_clzo {	

	BYTE	bySlotIndex;
	
//	#define unit_bullet_arms			0
//	#define unit_bullet_shoulder		1
//	#define UNIT_BULLET_NUM				2	

	BYTE	byBulletIndex[UNIT_BULLET_NUM];	//	.. 채우지않은 파트는 0xFF
};

#define unit_bullet_fill_result_zocl 8
struct _unit_bullet_fill_result_zocl {	

	BYTE    byRetCode;
	BYTE	bySlotIndex;
	bool	byBulletIndex[UNIT_BULLET_NUM];	//채우지않은 파트는 0xFF

	DWORD	dwComsumMoney[MONEY_UNIT_NUM];
	DWORD	dwLeftMoney[MONEY_UNIT_NUM];
};

#define unit_pack_fill_request_clzo 111
struct _unit_pack_fill_request_clzo {	

	BYTE	bySlotIndex;

	BYTE	byFillNum;

	struct __list{					
		BYTE	bySpareIndex;
		BYTE	byBulletIndex;
	};

	__list	List[max_unit_spare];

	_unit_pack_fill_request_clzo()	{	byFillNum = 0;	}

	int size()	{	return sizeof(*this) - sizeof(__list)*(max_unit_spare - byFillNum);	}
};

#define unit_pack_fill_result_zocl 12
struct _unit_pack_fill_result_zocl {

	BYTE    byRetCode;
	BYTE	bySlotIndex;

	DWORD	dwComsumMoney[MONEY_UNIT_NUM];
	DWORD	dwLeftMoney[MONEY_UNIT_NUM];

	BYTE	byFillNum;

	typedef _unit_pack_fill_request_clzo::__list __list;

	__list	List[max_unit_spare];

	_unit_pack_fill_result_zocl()	{	byFillNum = 0;	}

	int size()	{	return sizeof(*this) - sizeof(__list)*(max_unit_spare - byFillNum);	}
};

#define unit_delivery_request_clzo 13
struct _unit_delivery_request_clzo {	

	BYTE	bySlotIndex;
	BYTE	byStoreIndex;	//0xFF면 원거리수송
	short	zUnitNewPos[3];	//현재위치에서 20이니..
};

#define unit_delivery_result_zocl 14
struct _unit_delivery_result_zocl {	

	BYTE    byRetCode;
	BYTE	bySlotIndex;
	BYTE	byTransDistCode;	//#define unit_trans_short_range		0	//유닛 근거리수송  #define unit_trans_long_range		1	//유닛 원거리수송
	DWORD	dwParkingUnitSerial;

	DWORD	dwPayDalant;
	DWORD	dwLeftDalant;
};

#define unit_return_request_clzo 15
struct _unit_return_request_clzo {	

};

#define unit_return_result_zocl 16
struct _unit_return_result_zocl {	

	BYTE    byRetCode;
	DWORD	dwPayDalant;
	DWORD   dwNewDalant;
};

#define unit_take_request_clzo 17
struct _unit_take_request_clzo {	

};

#define unit_take_result_zocl 18
struct _unit_take_result_zocl {	

	BYTE    byRetCode;
};

#define unit_leave_request_clzo 19
struct _unit_leave_request_clzo {	

	short	zPlayerNewPos[3];	//현재위치에서 20이니..
};

#define unit_leave_result_zocl 20
struct _unit_leave_result_zocl {	

	BYTE    byRetCode;
};

#define unit_alter_fee_inform_zocl 30
struct _unit_alter_fee_inform_zocl {		

	BYTE	bySlotIndex;
//	DWORD	dwKeepingFee;	del(4/22)
	DWORD	dwPullingFee;
};

#define parkingunit_change_owner_zocl 31
struct _parkingunit_change_owner_zocl {		

	WORD	wObjIndex;
	DWORD	dwObjSerial;
	DWORD	dwOldOwnerSerial;
	DWORD	dwNewOwnerSerial;
	BYTE	byNewOwnerUnitSlotIndex;
};

#define unit_bullet_replace_request_clzo 32		//백팩에 있는것을 탄창에 옮긴다..
struct _unit_bullet_replace_request_clzo {	

	BYTE	bySlotIndex;
	BYTE	byPackIndex;
	BYTE	byBulletPart;	
};

#define unit_bullet_replace_result_zocl 33
struct _unit_bullet_replace_result_zocl {	

	BYTE    byRetCode;
};

#define unit_booster_charge_inform_zocl 34	//부스터 변경
struct _unit_booster_charge_inform_zocl {	

	BYTE	bySlotIndex;
	WORD	wBoosterGauge;
};

#define unit_destroy_inform_zocl 35	//유닛파괴
struct _unit_destroy_inform_zocl {		//add(4/24)
	
	BYTE	bySlotIndex;
};

#define unit_force_return_inform_zocl 36	//유닛강제 견인
struct _unit_force_return_inform_zocl {		//add(4/26)
	
	BYTE	bySlotIndex;
	DWORD	dwDebt;
};

//recall_msg.. 22

#define animus_recall_request_clzo 1	//소환
struct _animus_recall_request_clzo {

	DWORD dwAnimusItemSerial;	//애니머스창에있는 아이템의 시리얼..
	WORD  wAnimusClientHP;		//클라이언트상의 회복된 HP
	WORD  wAnimusClientFP;		//클라이언트상의 회복된 FP
};

#define animus_recall_result_zocl 2
struct _animus_recall_result_zocl {	

	BYTE byResultCode;
	DWORD dwAnimusSerial;		//애니머스 케릭터의 시리얼
	WORD  wAnimusHP;			//애니머스 HP
	WORD  wAnimusFP;			//애니머스 FP
	WORD  wAnimusExp;			//애니머스 Exp
	WORD  wLeftFP;				//자신의 FP
};

#define animus_return_request_clzo 3	//귀환
struct _animus_return_request_clzo {
};

#define animus_return_result_zocl 4	//_animus_return_request_clzo의 결과 또는 서버상의 강제귀환의 경우..
struct _animus_return_result_zocl {	

	BYTE  byResultCode;
	WORD  wAnimusItemSerial;
};

#define animus_command_request_clzo 5	//애니머스에 명령.. 
struct _animus_command_request_clzo {

	//#define animus_act_type_action	0	//액쑌
	//#define animus_act_type_follow	1	//Follow me!

	BYTE byCommandCode;
};

#define animus_command_inform_zocl 6	//애니머스의 상태변경.. 내부적으로 모드가 바뀔적마다..
struct _animus_command_inform_zocl {

	BYTE  byStateCode;
};

#define animus_target_request_clzo 7	//목표물
struct _animus_target_request_clzo {

	BYTE	byObjectID;
	WORD	wObjectIndex;
	DWORD	dwObjectSerial;
};

#define animus_target_result_zocl 8
struct _animus_target_result_zocl {	

	BYTE  byResultCode;
};

#define animus_hp_inform_zocl 9
struct _animus_hp_inform_zocl {	

	WORD wAnimusItemSerial;
	WORD wLeftHP;
};

#define animus_fp_inform_zocl 10
struct _animus_fp_inform_zocl {	

	WORD wAnimusItemSerial;
	WORD wLeftFP;
};

#define animus_exp_inform_zocl 11		//애니머스의 경험치.. 케릭터 스킬은 따로 옴..
struct _animus_exp_inform_zocl {

	WORD wAnimusItemSerial;
	DWORD dwExp;
};

#define animus_lvup_inform_zocl 12		//애니머스의 레벨업.. (주변..)
struct _animus_lvup_inform_zocl {

	WORD wIndex;
	DWORD dwSerial;
	BYTE byLv;
};

#define animus_recall_wait_time_free_inform_zocl 13		//애니머스 리콜 대기시간 프리..
struct _animus_recall_wait_time_free_inform_zocl {

	bool bFree;	//true 면 대기시간없음.. falae면 원래대로..
};

//sync_msg.. 19

#define object_server_pos_request_clzo 1
struct _object_server_pos_request_clzo {

	BYTE byObjKind;
	BYTE byObjID;
	WORD wObjIndex;
	DWORD dwObjSerial;
};

#define object_server_pos_result_zocl 2
struct _object_server_pos_result_zocl {		

	BYTE byErrCode; //0: 성공..1: 배열범위초과, 2: 죽은오브젝트, 3: 시리얼이 틀림

	BYTE byObjKind;
	BYTE byObjID;
	WORD wObjIndex;
	DWORD dwObjSerial;

	float	fServerPos[3];
};

#define adjust_amount_inform_zocl 3
struct _adjust_amount_inform_zocl {

	BYTE byStorageCode;
	WORD wSerial;
	DWORD dwNewDur;
};

//
//quest_msg
//

#define insert_new_quest_inform_zocl 1
struct _insert_new_quest_inform_zocl {	//add(3/23)

	BYTE	bySlotIndex;
	DWORD	dwEventIndex;
	BYTE	byEventNodeIndex;
	DWORD	dwQuestIndex;
};

#define select_waited_quest_command_zocl 2
struct _select_waited_quest_command_zocl {	//add(3/23)

	DWORD	dwEventIndex;
	BYTE	byEventNodeIndex;
};

#define insert_quest_failure_zocl 3
struct _insert_quest_failure_zocl {	//add(3/23)

	DWORD	dwEventIndex;
	BYTE	byEventNodeIndex;
};

#define quest_complete_inform_zocl 4
struct _quest_complete_inform_zocl {	//add(3/23)

	BYTE	byQuestDBSlot;
};

#define quest_process_inform_zocl 5
struct _quest_process_inform_zocl {	//add(3/23)

	BYTE	byQuestDBSlot;
	BYTE	byActIndex;
	WORD	wCount;	//0xFFFF면 해당 요구액숀 완료
};

#define quest_fail_inform_zocl 5
struct _quest_fail_inform_zocl {	//add(3/25)

//#define quest_fail_code_lv			0	
//#define quest_fail_code_timeout		1	

	BYTE	byFailCode;	
	BYTE	byQuestDBSlot;
};


#pragma pack(pop)

#endif
