#ifndef _CharacterDB_h_
#define _CharacterDB_h_

#include <windows.h>
#include <stdlib.h>
#include "Protocol.h"

#define MAX_ADMIN_PER_GLOBAL		10		//동접가능한 어드민계정수
#define MAX_WORLD_PER_GLOBAL		10		//최대 서비스 가능 월드수
#define MAX_LOGINER_PER_GLOBAL		5		//최대 로그인서버수
#define MAX_PATCHER_PER_GLOBAL		2		//최대 패치서버수
#define MAX_USER_PER_GLOBAL			10000	//최대 서비스가능 유저수
#define MAX_LOGIN_CLINET_NUM		1000	//로그인서버의 최대 유저수

#ifdef __SERVICE
#define MAX_PLAYER					3100	//존당 최대 플레이어수(서비스모드)
#endif

#ifndef __SERVICE
#define MAX_PLAYER					310		//존당 최대 플레이어수(개발모드)
#endif

#define MAX_MONSTER					10000			//존당 최대 몬스터수
#define MAX_NPC						50				//존당 최대 NPC수
#define MAX_ANIMUS					MAX_PLAYER/3+1	//존당 최대 Animus수
#define MAX_TOWER					MAX_PLAYER/3+1	//존당 최대 GuardTower수
#define MAX_ITEMBOX					MAX_PLAYER*2	//존당 최대 아이템상자수
#define MAX_BATTLEDUNGEON			100				//존당 최대 던젼게이트수	
#define MAX_PARKINGUNIT				MAX_PLAYER/3+1	//존당 최대 파킹유닛	
#define MAX_OBJ						MAX_PLAYER + MAX_MONSTER + MAX_ITEMBOX + MAX_NPC + MAX_BATTLEDUNGEON + MAX_ANIMUS + MAX_PARKINGUNIT + MAX_TOWER

#define MAX_LOAD_PER_ADMIN			10		//어드민당 최대 로드가능한 아바타정보수
#define MAX_CHAR_PER_WORLDUSER		3		//월드유저당 최대 생성가능한 캐릭터수
#define RACE_NUM					3

#define KEY_NUM						CHECK_KEY_NUM			//보안키수
#define NETMOVE_WAIT_TERM			120000		//서버이동시 대기시간(120초)
#define PUSH_CLOSE_WAIT_TERM		62000		//밀어내기종료 최대대기시간 (62초)
#define ACCOUNT_WORLD_PING_TERM		1000		//계정서버와 월드서버간의 핑간격(1초)

#define MAP_STD_RADIUS				3		//맵에서 플레이어위치에서 사방으로 더해지는 섹터수
#define	MAP_STD_SEC_SIZE			100		//한섹터의 한변의 길이

#define WORLD_RUN_MODE_SERVICE		0
#define WORLD_RUN_MODE_TEST			1

#define MAX_ECONOMY_HISTORY			12		//경제지표 최대 히스토리 저장수

//유저등급
#define USER_DEGREE_STD				0		//유저등급-일반
#define USER_DEGREE_GID				1		//유저등급-가이드
#define	USER_DEGREE_MGR				2		//유저등급-운영자
#define USER_DEGREE_DEV				3		//유저등급-개발자
#define USER_DEGREE_ULTRA			4		//유저등급-훌륭한개발자

//운영자등급
#define MGR_DEGREE_PRIVATE			0		//운영자등급-일반운영자
#define MGR_DEGREE_STAFF			1		//운영자등급-중간레벨
#define MGR_DEGREE_MASTER			2		//운영자등급-팀장급

#define equip_fix_num				8		//장착창 슬롯수 (tbl_code_upper ~ 	tbl_code_cloak)
#define embellish_fix_num			6		//장신구창 슬롯수 (tbl_code_ring, tbl_code_amulet, tbl_code_bullet)  
#define bag_storage_num				100		//최대 인벤 슬롯수
#define force_storage_num			88		//포스창 슬롯수
#define	animus_storage_num			4		//애니머스창 슬롯수	
#define unit_storage_num			4		//기갑장비 슬롯수
#define quest_storage_num			15		//수행중인 퀘스트 슬롯수
#define potion_belt_num				4		//포션창 슬롯수
#define sf_linker_num				8		//SF링크창 슬롯수
#define left_cutting_num			40		//가공임시창 최대 저장수
#define class_history_num			3		//과거 클래스 히스토리

#define max_level					50
#define max_animus_level			50
#define max_skill_level				7
#define max_class_grade				4		//클래스등급
#define default_per_part			16		//파트별로 기본갑옷의 수
#define base_fix_num				5		//디폴트케릭터 기본갑옷수
#define one_bag_store_num			20		//가방하나당 슬롯수
#define max_bag_num					5		//최대 가방수
#define max_unit_spare				8		//기갑장비 탄창 최대 여유분수
#define max_overlap_num				99		//중첩아이템의 최대 중첩수

#define no_part_wear				0xffff	//장착하지않은 파트
#define member_per_party			8		//파티당 최대 맴버수
#define MAX_EQUIP_EFFECT_KIND		4		//장착아이템의 최대 효과수
#define max_d_trade_item			15		//일대일거래시 1회당 거래가능한 아이템수
#define	CONT_SF_SIZE				5		//한명이 동시에 걸수있는 지속과 이득별 SF효과수
#define MAX_POTION_EFFECT_KIND		4		//포션아이템의 최대 효과수
#define STANDARD_MOVE_SPEED			15		//1초당 표준 이동거리 ( 오브젝트별로 값을 곱한 배수를 사용 )
#define STANDARD_VIEW_SIZE			340		//클라이언트의 뷰 반지름
#define MAX_TALIK_NUM				13		//업그레이드 탈릭수	
#define attack_able_height			50		//근접 공격가능 높이
#define attack_max_height			250		//원거리 공격가능 높이
#define EQUIP_MASTERY_CHECK_NUM		2		//아이템 장착시 최대 체크해야할 마스터리수
#define max_quest_act_num			3		//한 퀘스트당 요구행위수 
#define max_quest_per_type			5		//퀘스트타입별 최대 퀘스트수

#define max_skill_num				48		//스킬수
#define force_mastery_num			24		//포스 마스터리 수
#define skill_mastery_num			8		//스킬 마스터리수
#define mastery_level_num			4		//마스터리 레벨수
#define itembox_dist_lootable		100		//아템루팅가능거리
#define upgrade_jewel_num			4		//업그레이드하는데 필요한 최대 보석수
#define MAX_ITEM_LV					7		//업그레이드가능한 최대 아이템 레벨
#define DEFAULT_ITEM_LV				3		//상점구매 아이템은 최대 업그레이드 가능소켓수가 이걸루다가 고정됨..
#define trade_item_max_num			100		//한번에 상점에서 구입, 판매가능한 아이템수
#define DEFAULT_Y_POS				-65535	//케릭터 생성후 첫 필드에 나올때의 y좌표.. (이좌표를 첫접속으로 인식하고 마을로 자리를 잡아줌)

#define race_sex_kind_num			5	//종족, 성별구분수 RACE_NUM * 성별

#define sex_code_male				0	//남자
#define sex_code_female				1	//여자	

//종족
#define race_code_bellato			0	//벨라토
#define race_code_cora				1	//코라
#define race_code_accretia			2	//아크레시아

//클래스 계열
#define class_code_warrior			0	//워리어
#define class_code_ranger			1	//레인져
#define class_code_spirit			2	//스피리츄어리스트
#define class_code_special			3	//스페셜리스트

#define obj_kind_num				2	//오브젝트 종류수
#define obj_kind_char				0	//케릭터	(이동, 전투 가능한 오브젝트)	
#define obj_kind_item				1	//아이템	(이동, 전투가 불가능한 오브젝트)

#define obj_id_player				0	//플레이어 코드
#define obj_id_monster				1	//몬스트 코드
#define obj_id_npc					2	//NPC
#define obj_id_animus				3	//애니머스	
#define obj_id_tower				4	//가드타워
#define OBJ_CHAR_ID_NUM				5

#define obj_id_itembox				0	//아이템박스
#define obj_id_dungeongate			1	//던젼구멍
#define obj_id_parkingunit			2	//주차중인 기갑장비
#define OBJ_ITEM_ID_NUM				3

#define item_tbl_num				26	//아이템 테이블수	
#define	tbl_code_upper				0	//상의
#define	tbl_code_lower				1	//하의
#define	tbl_code_gauntlet			2	//장갑
#define	tbl_code_shoe				3	//신발
#define	tbl_code_helmet				4	//모자
#define	tbl_code_shield				5	//방패
#define	tbl_code_weapon				6	//무기
#define tbl_code_cloak				7	//망또
#define tbl_code_ring				8	//반지
#define tbl_code_amulet				9	//목걸이
#define tbl_code_bullet				10	//탄창
#define tbl_code_maketool			11	//아이템제작도구
#define tbl_code_bag				12	//가방
#define tbl_code_potion				13	//포션
#define tbl_code_face				14	//얼굴
#define tbl_code_fcitem				15	//포스리버
#define tbl_code_battery			16	//건전지	
#define tbl_code_ore				17	//원석
#define tbl_code_res				18	//보석
#define tbl_code_key				19	//기갑장비 인증키
#define tbl_code_booty				20	//전리품
#define tbl_code_map				21	//미니맵
#define tbl_code_town				22	//귀환아이템
#define tbl_code_bdungeon			23	//베틀던젼생성 아이템
#define tbl_code_animus				24	//소환아이템
#define tbl_code_tower				25	//가드타워

//아이템 업그레이드 파라메터 사용별 종류
#define item_kind_std				0	//업그레이드정보로 사용(일반)
#define item_kind_ani				1	//HP, FP정보로 사용 (애니머스)
#define item_kind_key				2	//다른칸의 인덱스로 사용 (유닛키)

//더미유닛 생성, 소멸 타입
#define unit_create_type_delivery	0	//출고
#define unit_create_type_leave		1	//하차

#define unit_destory_type_return	0	//입고
#define unit_destory_type_take		1	//승차

#define unit_trans_short_range		0	//유닛 근거리수송 
#define unit_trans_long_range		1	//유닛 원거리수송

//유닛프레임코드
#define unit_frame_code_goliah		0
#define unit_frame_code_catapult	1
#define unit_frame_code_engineer	2
#define unit_frame_code_nomade		3
#define UNIT_FRAME_CODE_NUM			4

//가갑장비 파트
#define unit_part_head				0
#define unit_part_upper				1
#define unit_part_lower				2
#define unit_part_arms				3
#define unit_part_shoulder			4
#define unit_part_back				5
#define UNIT_PART_NUM				6	

//기갑장비 탄창코드
#define unit_bullet_arms			0
#define unit_bullet_shoulder		1
#define UNIT_BULLET_NUM				2

#define sf_cont_code_damage			0
#define sf_cont_code_help			1
#define SF_CONT_CODE_NUM			2

//효과코드.. (클래스 스킬 추가)
#define effect_code_skill			0
#define effect_code_force			1
#define effect_code_class			2
#define EFFECT_CODE_NUM				3
 
//단축창 단축코드 (클래스 스킬 추가)
#define link_code_skill				effect_code_skill
#define link_code_force				effect_code_force
#define link_code_class				effect_code_class
#define link_code_animus			EFFECT_CODE_NUM + 0
#define LINK_CODE_NUM				4

//내성코드
#define TOL_CODE_NUM				4
#define TOL_CODE_FIRE				0		//불내성
#define TOL_CODE_WATER				1		//물내성
#define TOL_CODE_SOIL				2		//흙내성
#define TOL_CODE_WIND				3		//바람내성
#define TOL_CODE_NOTHING			-1		//내성없음

//파라메터 코드
#define param_code_hp				0
#define param_code_fp				1
#define param_code_sp				2
#define PARAM_CODE_NUM				3

//숙련도구분
#define mastery_code_weapon			0	//무기사용숙련도
#define mastery_code_defence		1	//방어숙련도
#define mastery_code_shield			2	//방패사용숙련도
#define mastery_code_skill			3	//스킬사용숙련도
#define mastery_code_force			4	//포스사용숙련도
#define mastery_code_make			5	//제작숙련도
#define mastery_code_special		6	//종족별특화숙련도
#define MASTERY_CODE_NUM			7

// 무기 계열
#define type_knife					0
#define type_sword					1		
#define type_axe					2
#define type_mace					3
#define type_spear					4
#define type_bow					5
#define type_firearm				6
#define type_luancher				7
#define type_throw					8
#define type_staff					9
#define type_mine					10
#define wp_type_num					11

// 공격거리코드 
#define attack_range_short			0
#define attack_range_long			1

// 일반공격 계열
#define	wp_class_close				attack_range_short		//근접
#define	wp_class_long				attack_range_long		//원거리
#define	WP_CLASS_NUM				2

//제작계열
#define mi_class_shield				0		//방패
#define mi_class_armor				1		//방어구
#define mi_class_bullet				2		//탄
#define MI_CLASS_NUM				3

// 포스 계열
#define fc_class_dark				0
#define fc_class_holy				1
#define fc_class_fire				2
#define fc_class_aqua				3
#define fc_class_terra				4
#define fc_class_wind				5
#define FC_CLASS_NUM				6

// 스킬 계열
#define sk_class_melee				attack_range_short
#define sk_class_bow				attack_range_long

//무기간격조정값
#define wp_adjust_knife				0.5f
#define wp_adjust_sword				0.25f
#define wp_adjust_axe				1.0f
#define wp_adjust_mace				0.0f
#define wp_adjust_spear				0.75f
#define wp_adjust_bow				0.25f
#define wp_adjust_firearm			0.75f
#define wp_adjust_launcher			0.5f
#define wp_adjust_throw				0.0f
#define wp_adjust_staff				0.5f
#define wp_adjust_bunch				0.5f   

//상점종류
#define store_code_weapon			0	//무기상인
#define store_code_potion			1	//포션상인
#define store_code_shield			2	//갑옷상인
#define store_code_deco				3	//잡화상인
#define store_code_unit				4	//유닛
#define store_code_res				5	//자원
#define store_code_booty		    6	//전리품
#define store_code_used				7	//중고품
#define store_code_exchange			10	//환전소		
#define store_code_force			16	//포스상인
		
//돈단위						
#define money_unit_dalant			0
#define money_unit_gold				1
#define MONEY_UNIT_NUM				2

//채트타입
#define CHAT_TYPE_OPERATOR			0 //운영자..
#define CHAT_TYPE_CIRCLE			1 //주변전체에..
#define CHAT_TYPE_WHISPER			2 //일대일..
#define CHAT_TYPE_PARTY				3 //파티원들에게..
#define CHAT_TYPE_RACE				4 //동족에게..
#define CHAT_TYPE_MGRWSP			5 //운영자귓말

//탈릭종류 ( 아이템 업그레이드)
#define talik_type_fc				0	//모든공격력UP 
#define talik_type_hp				1	//가격하는_데미지의_데이터필드_퍼센트값만큼을_HP로_흡수 2
#define talik_type_fp				2	//가격하는_데미지의_데이터필드_퍼센트값만큼을_FP로_흡수 3
#define talik_type_stun				3	//가격당하는_대상을_stun시킴(데이터필드_퍼센트값만큼의_확률로_1초동안)  4
#define talik_type_dist				4	//사정거리UP 5
#define talik_type_def				5	//방어력UP 6
#define talik_type_contdam			6	//데이터필드_퍼센트값만큼의_지속피해보조시간_단축 7
#define talik_type_fire				7	//캐릭터의_불내성값에_데이터필드_값만큼의_불내성증가 8
#define talik_type_water			8	//캐릭터의_불내성값에_데이터필드_값만큼의_물내성증가 9
#define talik_type_soil				9	//캐릭터의_불내성값에_데이터필드_값만큼의_흙내성증가 10
#define talik_type_wind				10	//캐릭터의_불내성값에_데이터필드_값만큼의_바람내성증가 11
#define talik_type_rev				11	//데이터필드_퍼센트값만큼의_HP회복량_증가 12
#define talik_type_avd				12	//_데이터필드_퍼센트값만큼의_회피율_증가 13

#define talik_type_down				14	//가장_최근에_삽입한_탈릭_날려버림 14

//아이템박스 상태
#define itembox_state_normal		0 //지정자만이 먹을수잇는상태
#define itembox_state_open			1 //누구든지 먹을수 있는 상태
#define itembox_state_hurry			2 //소멸임박,깜빡임

//던젼게이트 상태
#define dungeongate_state_normal	0 //열린상태
#define dungeongate_state_hurry		1 //소멸임박,깜빡임

//맵 위치이동 코드
#define mapin_type_start			0	//게임시작
#define mapin_type_move				1	//포탈이동..
#define mapin_type_revival			2	//부활..
#define mapin_type_goto				3	//순간이동..
#define mapin_type_recall			4	//출동..
#define mapin_type_dungeon			5	//던젼..
#define mapin_type_num				mapin_type_dungeon+1

//퀘스트타입 코드
#define quest_type_story			0
#define	quest_type_auto				1
#define quest_type_task				2
#define QUEST_TYPE_NUM				3

//퀘스트실패코드
#define quest_fail_code_lv			0	//제한레벨에 걸림
#define quest_fail_code_timeout		1	//타임아웃..

//베틀던젼 종료 코드
#define dungeon_complete_code_timeout	0
#define dungeon_complete_code_mission	1

#define handle_both					100		//양손무기
#define ore_high_quality_grade		2		//3레벨 원석중 하이 퀄리티 코드

#define move_type_walk				0		//걷기모드
#define move_type_run				1		//뛰기모드
#define mode_type_demili			0		//평화모드
#define mode_type_mili				1		//공격모드

//몬스터 소멸 코드	
#define mon_destroy_code_die		0	//맞아죽음
#define mon_destroy_code_respawn	1	//소환

//소환수의 액션 타입
#define animus_act_type_action		0	//액쑌
#define animus_act_type_follow		1	//Follow me!
#define ANIMUS_ACT_TYPE_NUM			2	

//소환수의 소멸 타입
#define animus_des_type_back		0	//귀환
#define animus_des_type_die			1	//죽음
#define ANIMUS_DES_MODE_NUM			2

//소환수의 귀환 타입
#define animus_return_type_command		0	//쥔장명령
#define animus_return_type_ai			1	//애니머스 스스로
#define animus_return_type_rule			2	//룰적인 처리 (멥이동, 쥔장사망)
#define animus_return_type_path			3	//패스목적상.. 곧바로 다시 나옴..

//가드타워의 소멸 타입
#define tower_des_type_back			0	//해체
#define tower_des_type_destroy		1	//파괴
#define tower_des_type_disconnect	2	//설치자 게임종료
#define TOWER_DES_MODE_NUM			3

#define max_id_len					12	//계정사이즈
#define max_pw_len					12	//비번사이즈
#define max_name_len				16	//케릭터이름사이즈
#define max_world_name_len			32	//월드이름 사이즈
#define max_map_name_len			20	//맵이름 사이즈
#define class_code_len				4	//케릭터 클래스코드사이즈
#define max_message_size			255	//send 가능한 message문자열 최대 size 
#define max_error_code_size			64	//에러메시지사이즈

//파티타입
#define PARTY_LEVEL_BOSS			0
#define PRATY_LEVEL_MEMBER			1

//트레이드락상태
#define D_TRADE_STATE_UNLOCK		0
#define D_TRADE_STATE_LOCK			1

//포트..
#define PORT_SERVER_MAP				10001
#define PORT_ACCOUNT_LOGIN			27000
#define PORT_ACCOUNT_WORLD			29000
#define PORT_ACCOUNT_CONTROL		28000
#define	PORT_WORLD_CONTROL			17777
#define PORT_CONTROL_CLIENT			17888
#define	PORT_LOGIN_CLINET			27780
#define	PORT_WORLD_MESSENGER		27555
#define	PORT_MESSENGER_CLIENT		10002
#define PORT_LOGSPY_CONTROL			17988

//존타입
#define ZONE_TYPE_GATE				0
#define ZONE_TYPE_MAP				1

//맵타입
#define MAP_TYPE_STD				0
#define MAP_TYPE_DUNGEON			1

//로그인유저코드
#define LOGIN_CODE_USER				0
#define LOGIN_CODE_DEV				1

//보상코드
#define reward_case_std				0	//일반적인경우
#define reward_case_class			1	//전직
#define reward_case_quest			2	//퀘스트

//아바타상태비트플래그..
#define FLAG_MOVE_TYPE				0	//걷기, 뛰기 상태
#define FLAG_MODE_TYPE				1	//전투, 비전투상태
#define FLAG_STEALTH_STATE			2	//스텔스상태
#define FLAG_STUN_STATE				3	//스턴상태
#define FLAG_PARTY_STATE			4	//파티상태
#define FLAG_CORPSE_STATE			5	//시체상태
#define FLAG_LEADER_STATE			6	//파티리더	
#define FLAG_DTRADE_STATE			7	//직거래중
#define FLAG_DETECT_STATE			8	//detect모드
#define FLAG_OBSERVER_STATE			9	//옵저버모드
#define FLAG_PARTYLOCK_STATE		10	//파티잠금모드
#define FLAG_MINING_STATE			11	//채굴모드
#define FLAG_GROGGY_STATE			12	//그로기상태

//공용결과코드..
#define RET_CODE_SUCCESS			0	//모든월드쿼리에대한 성공
#define RET_CODE_ASYNC_ERR			1	//월드쿼리결과리턴전에 월드쿼리시 동기화 에러
#define RET_CODE_CLOSE_WORLD		2	//존에서 월드접속이 끊김
#define RET_CODE_BEFORE_LOGIN		3	//아직 로그인하지않았음
#define RET_CODE_FAULT_SERIAL		4	//월드시리얼이 다름
#define RET_CODE_ALREADY_LOGIN		5	//이미 로그인햇음
#define RET_CODE_WRONG_USERID		6	//계정이 틀림
#define RET_CODE_WRONG_PASSWD		7	//비번이 틀림
#define RET_CODE_USER_FULL			8	//월드에 접속자가 꽉참
#define RET_CODE_NONEXIST_WAIT		9	//존이동중인자의 리스트에 없음
#define RET_CODE_NONEXIST_MAP		10	//이동하고자하는 맵이없는것임
#define RET_CODE_ALREADY_REGED		11	//이미 등록아바타에대한 정보를 받았음
#define RET_CODE_BEFORE_REGED		12	//등록아바타에대한 정보를 받기전임
#define RET_CODE_SLOT_ERR			50	//아바타 슬롯인덱스에대한에러
#define RET_CODE_CLASS_ERR			13	//아바타 슬롯인덱스에대한에러
#define RET_CODE_EMPTY_SLOT			14	//빈 아바타 슬롯인덱스임
#define RET_CODE_NOT_REGED			15	//등록됀 아바타가 아님
#define RET_CODE_ALREADY_SEL		16	//이미 아바타를 선택햇음	
#define RET_CODE_ALREADY_EXIT		17	//이미 접속을 종료했음
#define RET_CODE_NONEXIST_ZONE		18	//해당 존에 존재하지 않음
#define RET_CODE_NONNEXT_ZONE		19	//이동할 존이 존재하지 않음
#define RET_CODE_ALREADY_MOVE_OUT	20	//이미 이동요청을 햇음
#define RET_CODE_WRONG_MASTERKEY	21	//마스터키가 틀림
#define RET_CODE_DB_OPEN_ERR		22  //데이터베이스가 열리지 않음
#define RET_CODE_REGED_ACCOUNT		23  //이미등록됀계정임
#define RET_CODE_DB_QUERY_ERR		24  //데이터베이스 쿼리 실패함
#define RET_CODE_REGED_AVATOR		25	//이미 등록됀 아바타명이 잇음
#define RET_CODE_FULL_LOGIN			26	//서비스가능 로그인수를 넘엇음
#define RET_CODE_BEFORE_NOT_EXIST	27	//강제로 끊을려고하는 먼저접속자가 이미 접속을 끊엇음
#define RET_CODE_ACCOUNT_NOT_SUIT	28	//계정이 유효한 문자조합이 아니다.
#define RET_CODE_NOT_OPEN_WORLD		29	//선택한 월드가 닫힌 월드임
#define RET_CODE_NOT_EQUAL_VERSION	30	//클라이언트프로그램의 버젼이 틀리다. 
#define RET_CODE_BEFORE_WORLD_LIST  31	//이미 월드리스트를 가져감
#define RET_CODE_NOT_WORLD_LIST		32	//아직 월드리스트를 안가져감
#define RET_CODE_NOT_WAIT_MODE		33	//서버이동 대기모드가 아님	
#define RET_CODE_NONE_AUTHOR_WORLD	34	//선택권한이 없는 월드임
#define RET_CODE_FAULT_ZONE_MOVE	35	//존이동 실패
#define RET_CODE_ALREADY_SELECT_AVATOR	36 //아바타 이미 선택했음
#define RET_CODE_NONEXIST_DB_RECORD		37	//데이터베이스에 없는 레코드
#define RET_CODE_NOTUSEABLE_AVATORNAME	38	//사용할수없는 아바타이름
#define RET_CODE_NOT_ADMIN			39	//관리자가 아님
#define RET_CODE_BLOCK_AVATOR		40	//블럭아바타
#define RET_CODE_LOBBY_CLOSE		41	//로비가 열리지않음
#define RET_CODE_HBKFILE_ERROR		42	//HBK파일이 지워지지않음
#define RET_CODE_NOT_AUTHORIZED		43	//권한없다
#define RET_CODE_DB_WAIT			44	//db결과 기다리는중
#define RET_CODE_LOGIN_DEV_MODE		45	//로그인서버가 개발모드임
#define RET_CODE_BLOCK_CHAT			46	//채팅금지계정
#define RET_CODE_ERR_CHAR_NAME		47	//특수문자가 사용된 케릭터이름
#define	RET_CODE_NO_SERVICE			48	//서비스 시간이 아닙니다
#define	RET_CODE_NO_SAVED_AVATOR	49	//이전에 세이브가 안된 케릭터임
#define RET_CODE_INTERNAL_ERROR		100	//포괄적인 내부에러
#define RET_CODE_NET_PROCESS_ERROR	200	//네트웤명령상의 절차적 에러
#define RET_CODE_PROGRAM_EXIT		0xFF	//내부사정으로 프로그램 종료함

#define DB_QUERY_ERR				RET_CODE_DB_QUERY_ERR 
#define DB_OPEN_ERR					RET_CODE_DB_OPEN_ERR 

#define __NSI						0xFF		//not search index 
#define __NO_TALIK					0x0F		//비어있는 업그레이드 탈릭 소켓	
#define __DF_UPT					0x0FFFFFFF	//item default upgrade..	맨 왼쪽 4비트 최대 소켓수, 오른쪽 4비트부터 차례로 탈릭소켓 F면 비어잇는거고 0~는 탈릭이 박힘

typedef DWORD	IP_ADDR;

struct _EQUIP_MASTERY_LIM
{
	int nMasteryCode;
	int nLimMastery;
};

//강제종료명령의 타입
#define FORCE_CLOSE_TYPE_KICK		0		//특정 유저에다한 강제종료
#define FORCE_CLOSE_TYPE_PUSH		1		//같은계정의 접속으로인한 강제종료
#define FORCE_CLOSE_TYPE_SERVICE	2		//서비스운영에 의한 전체 강제종료
#define FORCE_CLOSE_TYPE_ERROR		3		//프로그램 오류에 의한 강제종료

class CRecordData;

//아이템레코드코드로 아이템데이블인덱스를 얻어옴
int		GetItemTableCode(char* psItemCode);
//아이템데이블을 배열로 통채로 얻어옴
bool	WriteTableData(int nItemNum, CRecordData* pItemData, char* szErrCode = NULL);
//아이템 디폴트 내구성을 얻어옴
int		GetItemDurPoint(int nTableCode, int nIndex);
//아이템 표준가격을 얻어옴
int		GetItemStdPrice(int nTableCode, int nItemIndex, int nRace, BYTE* pbyMoneyKind);
//아이템사용 제한레벨을 얻어옴
int		GetItemEquipLevel(int nTableCode, int nItemIndex);
//아이템사용 제한마스터리를 얻어옴
_EQUIP_MASTERY_LIM*	GetItemEquipMastery(int nTableCode, int nItemIndex, int* OUT pnLimNum);
//아이템사용 제한종족을 얻어옴
char*	GetItemEquipCivil(int nTableCode, int nItemIndex);
//아이템 한글이름을 얻어옴
char*	GetItemKorName(int nTableCode, int nItemIndex);
//아이템의 디폴트 업그레이드 소켓수 얻어옴
BYTE	GetDefItemUpgSocketNum(int nTableCode, int nItemIndex);
//무기의 클래스를 얻어옴
BYTE	GetWeaponClass(int nItemIndex);
//아아템 파라메터사용에 의한 아이템 분류
BYTE	GetItemKindCode(int nTableCode);

//아이템사용 제한종족인지..
BOOL	IsItemEquipCivil(int nTableCode, int nItemIndex, BYTE bySexRace);
//중첩아이템인지..
BOOL	IsOverLapItem(int nTableCode);
//히스토리기능에서 보호되는 아이템인지
BOOL	IsProtectItem(int nTableCode);//history log로 보호되는 아이템
//수기가능한 아이템인지..
BOOL	IsRepairableItem(int nTableCode, int nItemIndex);
//마모되는 아이템인지..
BOOL	IsAbrItem(int nTableCode, int nItemIndex);
//버릴수있는 아이템인지
BOOL	IsGroundableItem(int nTableCode, int nItemIndex);
//실제 게임에서 사용가능한 아이템인지..
BOOL    IsExistItem(int nTableCode, int nItemIndex);
//내성관련 탈릭인가
BOOL	IsTalikAboutTol(int nTalikIndex);
//아이템 종류에 맞는 스토리지 코드인가
BOOL	IsStorageCodeWithItemKind(int nTableCode, int nStorageCode);

//아이템 업그레이드 레벨수
BYTE	GetItemUpgedLv(DWORD dwLvBit);
//아이템 업그레이드가능한 소켓수
BYTE	GetItemUpgLimSocket(DWORD dwLvBit);
//해당 소켓의 탈릭인덱스 얻기
BYTE	GetTalikFromSocket(DWORD dwLvBit, BYTE bySocketIndex);
//업그레이드후의 비트
DWORD	GetBitAfterUpgrade(DWORD dwCurBit, DWORD dwTalikCode, BYTE byCurLv);
//다운그레이드후의 비트
DWORD	GetBitAfterDowngrade(DWORD dwCurBit, BYTE byCurLv);
//제한소켓설정후 비트
DWORD	GetBitAfterSetLimSocket(BYTE byLimSocketNum);

//아이템 업그레이드정보를 문자로 보여준다
char*	DisplayItemUpgInfo(int nTableCode, DWORD dwLvBit);
//해당탈릭을 특정 아이템에 박을수 있는가
bool	IsAddAbleTalikToItem(BYTE byItemTableCode, WORD wItemIndex, DWORD dwItemCurLv, WORD wTalikIndex, int* pnTalikLim);

#pragma pack(push, 1)

//포션창의 키
struct _POTIONKEY
{
	BYTE bySlotIndex;	//클라이언트 슬롯인덱스
	BYTE byAmount;		//수량
	WORD wItemIndex;	//포션인덱스

	bool IsFilled()		{		if(*(DWORD*)this == 0xFFFFFFFF) return false;	return true;	}	//채워진 슬롯인지
	void SetRelease()	{		*(DWORD*)this = 0xFFFFFFFF;	}										//비운다
	int CovDBKey()	{		return *(int*)this;	}													//DB에 들어가는 정수형으로 변환
	void  LoadDBKey(int pl_nKey)	{	*(DWORD*)this = (DWORD)pl_nKey;	}							//DB에서 받은값을 집어넣음
};

//인벤창의 키
struct _INVENKEY
{
	BYTE bySlotIndex;	//클라이언트 슬롯인덱스
	BYTE byTableCode;	//아이템종류코드
	WORD wItemIndex;	//아이템인덱스	

	bool IsFilled()		{		if(*(DWORD*)this == 0xFFFFFFFF) return false;	return true;	}
	void SetRelease()	{		*(DWORD*)this = 0xFFFFFFFF;	}
	int CovDBKey()	{		return *(int*)this;	}
	void  LoadDBKey(int pl_nKey)	{	*(DWORD*)this = (DWORD)pl_nKey;	}
};

//장착창의 키
struct _EQUIPKEY
{
	short zItemIndex;	//아이템인덱스 ( 장착은 순서대로 들어가므로 tablecode나 slotindex는 필요없다)
	
	bool IsFilled()		{		if(zItemIndex == -1) return false;	return true;	}
	void SetRelease()	{		zItemIndex = -1;	}
	short CovDBKey()	{		return zItemIndex;	}
	void  LoadDBKey(short pl_zKey)	{	zItemIndex = pl_zKey;	}
};

//장신구창의 키
struct _EMBELLKEY
{
	BYTE bySlotIndex;	//클라이언트 슬롯인덱스
	BYTE byTableCode;	//아이템종류코드
	WORD wItemIndex;	//아이템인덱스	

	bool IsFilled()		{		if(*(DWORD*)this == 0xFFFFFFFF) return false;	return true;	}
	void SetRelease()	{		*(DWORD*)this = 0xFFFFFFFF;	}
	int CovDBKey()	{		return *(int*)this;	}
	void  LoadDBKey(int pl_nKey)	{	*(DWORD*)this = (DWORD)pl_nKey;	}
};

//포스창의 키
struct _FORCEKEY
{
	DWORD dwKey;		//왼쪽 8비트는 포스리버 인덱스, 나머지부터는 포스성공누적수	

	void SetKey(BYTE pl_byItemIndex, DWORD pl_dwStat)		//포스를 채운다
	{
		DWORD dwIndex = (DWORD)pl_byItemIndex;
		dwKey = ((DWORD)dwIndex<<24) | (pl_dwStat&0x00FFFFFF);
	}
	void SetStat(DWORD pl_dwStat)							//누적수를 올린다.
	{
		dwKey &= 0xFF000000;
		dwKey |= (pl_dwStat & 0x00FFFFFF);
	}

	BYTE GetIndex()	{		return (BYTE)(dwKey>>24);	}		//포스인덱스
	DWORD GetStat()	{		return (DWORD)(dwKey & 0x00FFFFFF);	}	//포스누적수

	bool IsFilled()		{		if(dwKey == 0xFFFFFFFF) return false;	return true;	}
	void SetRelease()	{		dwKey = 0xFFFFFFFF;	}
	int CovDBKey()	{		return (int)dwKey;	}
	void  LoadDBKey(int pl_nKey)	{	dwKey = (DWORD)pl_nKey;	}
};

//애니머스창의 키
struct _ANIMUSKEY
{
	BYTE byItemIndex;		//애니머스아이템 인덱스

	bool IsFilled()		{		if(*(BYTE*)this == 0xFF) return false;	return true;	}
	void SetRelease()	{		*(BYTE*)this = 0xFF;	}
	BYTE CovDBKey()	{		return *(BYTE*)this;	}
	void  LoadDBKey(BYTE pl_byKey)	{	*(BYTE*)this = pl_byKey;	}
};

//효과 링크키
struct _LINKKEY
{
	WORD wEffectCode;	//왼쪽 4비트는 코드..나머지 인덱스..

	bool IsFilled()		{		if(*(WORD*)this == 0xFFFF) return false;	return true;	}
	void SetRelease()	{		*(WORD*)this = 0xFFFF;	}
	short CovDBKey()		{		return *(short*)this;	}
	void  LoadDBKey(short pl_zKey)	{	*(WORD*)this = (WORD)pl_zKey;	}
	void SetData(WORD wCode, WORD wIndex)	{	wEffectCode = ((wCode << 12) | wIndex);	}
	WORD GetCode()	{		return wEffectCode >> 12;	}		//링크코드
	WORD GetIndex()	{		return (wEffectCode & 0x0FFF);	}	//링크인덱스
};

struct _REGED_AVATOR_DB					//등록아바타를 클라이언트로 보낼때담는하는 구조체
{
	char		m_szAvatorName[max_name_len+1];		//Base [Name]						
	DWORD		m_dwRecordNum;						//Base [Serial] :	아바타 시리얼			
	BYTE		m_byRaceSexCode;					//Base [Race]	:	종족 & 성별코드		
	BYTE		m_bySlotIndex;						//Base [Slot]	:	아바타 선택창 인덱스	
	char		m_szClassCode[class_code_len+1];	//Base [Class]	:	클래스코드			
	BYTE		m_byLevel;							//Base [Lv]		:	레벨					
	DWORD		m_dwDalant;							//Base [Dalant]	:	달란트				
	DWORD		m_dwGold;							//Base [Gold]	:	골드					
	DWORD		m_dwBaseShape;						//Base [BaseShape]	: 기본생김..(0x00654321	1:upper, 2:lower, 3:gauntlet, 4:shoe, 5:helmet, 6:face)
	_EQUIPKEY	m_EquipKey[equip_fix_num];			//Base [EK%d]	:	장착키				
	BYTE		m_byEquipLv[equip_fix_num];			//장착레벨				//Base [EU%d]를 분석후 레벨을 적음
	DWORD		m_dwLastConnTime;					//Base [LastConnTime]	:	마지막 로그오프한 시간.. 

	_REGED_AVATOR_DB(){	Init();	}

	void Init(){
		memset(this, 0, sizeof(*this));
		m_bySlotIndex = 0xFF;	
		m_dwRecordNum = 0xFFFFFFFF;
		for(int i = 0; i < equip_fix_num; i++)
			m_EquipKey[i].SetRelease();
	}
};

struct _REGED : public _REGED_AVATOR_DB		//업그레이드 정보는 선택하기전까지은 안가므로 하위 클래스로 넣음..
{
	DWORD   m_dwFixEquipLv[equip_fix_num];	//Base [EU%d]	:	장착한 아이템의 업그레이드정보	//

	_REGED(){		init();	}

	void init()
	{
		_REGED_AVATOR_DB::Init();
		for(int i = 0; i < equip_fix_num; i++) 
			m_dwFixEquipLv[i] = __DF_UPT;
	}

	void UpdateEquipLv(){
		for(int v = 0; v < equip_fix_num; v++)
			_REGED_AVATOR_DB::m_byEquipLv[v] = ::GetItemUpgedLv(m_dwFixEquipLv[v]);	//업그레이드변수를 분석해서 상위클래스의 m_byEquipLv를 세팅함
	}
};

struct _AVATOR_DB_BASE : public _REGED		//케릭터 기본정보
{
	DWORD	m_dwHP;					//General [HP]	:	HP					
	DWORD	m_dwFP;					//General [FP]	:	FP					
	DWORD	m_dwSP;					//General [SP]	:	SP					
	double	m_dExp;					//General [Exp]	:	경험치				
	BYTE	m_byBagNum;				//General [BagNum]	:	사용중인 가방갯수		
	BYTE	m_byMapCode;			//General [Map]	:	시작할 맵코드			
	short	m_zClassHistory[class_history_num];		//General [Class0, Class1, Class2]
	float	m_fStartPos[3];			//General [X], [Y], [Z]	:	시작할 좌표			
	DWORD	m_dwTotalPlayMin;		//General	[TotalPlayMin]	:	총 플레이한 시간		
	DWORD	m_dwStartPlayTime;		//사용안함.. 플레이시작시간		

	_AVATOR_DB_BASE(){ Init(); }

	void Init(){
		_REGED::Init();
		for(int i = 0; i < class_history_num; i++)
			m_zClassHistory[i] = -1;
	};
};

struct _BELT_DB_BASE					//벨트 기본정보 (포션, 링크)
{
	struct _POTION_LIST{	//포션리스트	

		_POTIONKEY	Key;		//General [PK%d]

		_POTION_LIST(){ Init(); }

		void Init(){
			Key.SetRelease();
		};
	};

	struct _LINK_LIST{		//링크리스트

		_LINKKEY	Key;		//General [LK%d]

		_LINK_LIST(){ Init(); }

		void Init(){
			Key.SetRelease();
		};
	};

	_POTION_LIST	m_PotionList[potion_belt_num];
	_LINK_LIST		m_LinkList[sf_linker_num];

	_BELT_DB_BASE() {Init(); }

	void Init(){
		for(int i = 0; i < potion_belt_num; i++)
			m_PotionList[i].Init();
		for(i = 0; i < sf_linker_num; i++)
			m_LinkList[i].Init();
	};
};

struct _EQUIP_DB_BASE			//장착정보 (Equip는 케릭터 기본정보로 빠짐)
{ 
	struct _EMBELLISH_LIST{

		_EMBELLKEY	Key;			//General [EK%d]											
		WORD		wAmount;		//General [ED%d]	:	탄환일경우 수량	

		_EMBELLISH_LIST(){ Init(); }

		void Init(){
			Key.SetRelease();
			wAmount = 0;
		};
	};

	_EMBELLISH_LIST		m_EmbellishList[embellish_fix_num];

	_EQUIP_DB_BASE(){ Init(); }

	void Init(){
		for(int i = 0; i < embellish_fix_num; i++)
			m_EmbellishList[i].Init();
	};
};

struct _INVEN_DB_BASE			//인벤토리 정보
{
	struct _LIST{
		
		_INVENKEY	Key;		//Inven [K%d]	:	키가 IsFilled() == false면 이하 무시..	
		DWORD		dwDur;		//Inven [D%d]	:	현재내구도								
		DWORD		dwUpt;		//Inven [U%d]	:	업그레이드.. (4비트씩 0번비트부터.. 보석레코드인덱스.. 빈칸이면 F)

		_LIST(){ Init(); }

		void Init(){
			Key.SetRelease();
			dwDur = 0;
			dwUpt = __DF_UPT;
		};
	};

	_LIST m_List[bag_storage_num];

	_INVEN_DB_BASE(){ Init(); }

	void Init(){
		for(int i = 0; i < bag_storage_num; i++)
			m_List[i].Init();
	};
};

struct _FORCE_DB_BASE			//포스창 정보
{
	struct _LIST{
		
		_FORCEKEY	Key;			//General [F%d]

		_LIST(){ Init(); }

		void Init(){
			Key.SetRelease();
		};
	};

	_LIST m_List[force_storage_num];

	_FORCE_DB_BASE(){ Init(); }

	void Init(){
		for(int i = 0; i < force_storage_num; i++)
			m_List[i].Init();
	};
};

struct _ANIMUS_DB_BASE			//애니머스창 정보
{
	struct _LIST{
		
		_ANIMUSKEY	Key;		//General [AK%d]	:	키			
		DWORD		dwExp;		//General [AD%d]	:	경험치		
		DWORD		dwParam;	//General [AP%d]	:	파라메터..	

		_LIST(){ Init(); }

		void Init(){
			Key.SetRelease();
			dwExp = 0;
			dwParam = 0;
		};
	};

	_LIST m_List[animus_storage_num];

	_ANIMUS_DB_BASE(){ Init(); }

	void Init(){
		for(int i = 0; i < animus_storage_num; i++)
			m_List[i].Init();
	};
};

struct _UNIT_DB_BASE						//유닛정보
{
	struct _LIST{
		BYTE	bySlotIndex;				//유닛슬롯
		BYTE	byFrame;					//Unit [F_%d]	:	프레임인덱스, 0xFF면 빈 슬롯
		DWORD	dwGauge;					//Unit [Gg_%d]	:	hp
		BYTE	byPart[UNIT_PART_NUM];		//Unit [H_%d], [U_%d], [L_%d], [A_%d], [S_%d], [B_%d]	:	파트정보
		DWORD	dwBullet[UNIT_BULLET_NUM];	//Unit [AB_%d], [SB_%d]:	탄창정보 (_unit_bullet_param로 변환) 0xFFFFFFFF면 빈탄창
		DWORD	dwSpare[max_unit_spare];	//Unit [Sp%d_%d]:	여분탄창정보 (_unit_bullet_param로 변환) 0xFFFFFFFF면 빈탄창
//		int		nKeepingFee;				//Unit [KF_%d]	:	채무 보관료, 견인료
		int		nPullingFee;				//Unit [PF_%d]	:	채무 견인료 
		DWORD	dwCutTime;					//Unit [Cut_%d]	:	유닛띄운상태에서 비정상 종료시 그 시간 (나중에 5분안에 재접시 그자리에 다시 유닛을 세워준다)
		WORD	wBooster;					//부스터수치

		_LIST(){ Init(); }

		void Init(BYTE byIndex = 0xFF){
			if(byIndex != 0xFF)
				bySlotIndex = byIndex;
			byFrame = 0xFF;
			memset(byPart, 0xFF, sizeof(byPart));
			memset(dwBullet, 0xFFFFFFFF, sizeof(dwBullet));
			memset(dwSpare, 0xFFFFFFFF, sizeof(dwSpare));
			dwGauge = 0;
//			nKeepingFee = 0;
			nPullingFee = 0;
			dwCutTime = 0;
		};

		void PushUnit(_LIST* pData){
			BYTE byBufferSlotIndex = bySlotIndex;
			memcpy(this, pData, sizeof(_LIST));
			bySlotIndex = byBufferSlotIndex;			
		};

		void DelUnit(){
			byFrame = 0xFF;
		}
	};

	_LIST m_List[unit_storage_num];

	_UNIT_DB_BASE(){ Init(); }

	void Init(){
		for(int i = 0; i < unit_storage_num; i++)
			m_List[i].Init(i);
	};
};

struct _CUTTING_DB_BASE		//가공창에 남아있는 보석류 정보
{
	struct _LIST{				//General [LeftResList] .. 문자열로 byResIndex, byAmt가 각각 2, 3문자씩 이어져있음
						
		BYTE byResIndex;		//보석인덱스 
		BYTE byAmt;				//수량

		_LIST(){ Init(); }

		void Init(){
			byResIndex = 0xFF;
			byAmt = 0;
		};	
	};

	BYTE	m_byLeftNum;

	_LIST	m_List[left_cutting_num];

	_CUTTING_DB_BASE(){ Init(); }

	void Init(){
		m_byLeftNum = 0;
		for(int i = 0; i < left_cutting_num; i++)
			m_List[i].Init();
	};
};

struct _QUEST_DB_BASE		//가공창에 남아있는 보석류 정보
{
	struct _LIST{			//미구현..

		BYTE byQuestType;	
		WORD wIndex;
		WORD wNum[max_quest_act_num];
		WORD wLeftMin;

		_LIST(){ Init(); }

		void Init(){
			byQuestType = 0xFF;
			wIndex = 0xFFFF;
			for(int i = 0; i < max_quest_act_num; i++)	
				wNum[i] = 0xFFFF;	//나중에 세팅할때 사용하는 맴버는 0으로 돌린다..
			wLeftMin = 0xFFFF;
		};	
	};

	_LIST m_List[quest_storage_num];

	_QUEST_DB_BASE(){ Init(); }

	void Init(){
		for(int i = 0; i < quest_storage_num; i++)
			m_List[i].Init();
	};
};

struct _STAT_DB_BASE		//마스터리등 각종 누적치들
{
	enum {  DATAPOS_WPCNT = 0,
			DATAPOS_DEF = DATAPOS_WPCNT+WP_CLASS_NUM,
			DATAPOS_SHLD = DATAPOS_DEF+1,
			DATAPOS_SKILL = DATAPOS_SHLD+1,
			DATAPOS_FORCE = DATAPOS_SKILL+max_skill_num,
			DATAPOS_MAKE = DATAPOS_FORCE+force_mastery_num,
			DATAPOS_SPECIAL = DATAPOS_MAKE+MI_CLASS_NUM	};	

	DWORD	m_dwDamWpCnt[WP_CLASS_NUM];			//General [WM%d]	:	클래스별 무기사용수
	DWORD	m_dwDefenceCnt;						//General [DM]		:	방어수
	DWORD	m_dwShieldCnt;						//General [PM]		:	방패사용성공횟수	
	DWORD	m_dwSkillCum[max_skill_num];		//General [SM%d]	:	스킬별 사용횟수
	DWORD	m_dwForceCum[force_mastery_num];	//General [FM%d]	:	포스마스터리별 사용횟수
	DWORD	m_dwMakeCum[MI_CLASS_NUM];			//General [MI%d]	:	제작클래스별 성공횟수
	DWORD	m_dwSpecialCum;						//General [SR]		:	종종별 특화기술 횟수 (벨: 유닛, 코: 소환, 아: 런쳐)

	_STAT_DB_BASE(){ Init(); }

	void Init(){
		memset(this, 0, sizeof(*this));
	};

	static int GetStatIndex(BYTE byMasteryClass, BYTE byIndex)	//(DWORD*)&_STAT_DB_BASE 했을때 인덱스가져오기
	{
		int nStatIndex = -1;
		switch(byMasteryClass)
		{
		case mastery_code_weapon:			nStatIndex = _STAT_DB_BASE::DATAPOS_WPCNT + byIndex;	break;
		case mastery_code_defence:			nStatIndex = _STAT_DB_BASE::DATAPOS_DEF;				break;
		case mastery_code_shield:			nStatIndex = _STAT_DB_BASE::DATAPOS_SHLD;				break;
		case mastery_code_skill:			nStatIndex = _STAT_DB_BASE::DATAPOS_SKILL + byIndex;	break;
		case mastery_code_force:			nStatIndex = _STAT_DB_BASE::DATAPOS_FORCE + byIndex;	break;
		case mastery_code_make:				nStatIndex = _STAT_DB_BASE::DATAPOS_MAKE + byIndex;		break;
		case mastery_code_special:			nStatIndex = _STAT_DB_BASE::DATAPOS_SPECIAL;			break;
		}
		return nStatIndex;
	}

	static bool IsRangePerMastery(BYTE byMasteryClass, BYTE byIndex)//마스터리별 인덱스 범위체크
	{		
		switch(byMasteryClass)
		{
		case mastery_code_weapon:	if(byIndex < WP_CLASS_NUM)		return true;
		case mastery_code_defence:	if(byIndex == 0)				return true;
		case mastery_code_shield:	if(byIndex == 0)				return true;
		case mastery_code_skill:	if(byIndex < max_skill_num)		return true;
		case mastery_code_force:	if(byIndex < force_mastery_num) return true;
		case mastery_code_make:		if(byIndex < MI_CLASS_NUM)		return true;
		case mastery_code_special:	if(byIndex == 0)				return true;
		}
		return false;		
	}
};

struct _AVATOR_DATA		//위의 총체적인 구조체
{
	_AVATOR_DB_BASE  dbAvator;	
	_BELT_DB_BASE    dbBelt;	
	_EQUIP_DB_BASE   dbEquip;	
	_FORCE_DB_BASE   dbForce;	
	_STAT_DB_BASE	 dbStat;	
	_INVEN_DB_BASE   dbInven;	
	_CUTTING_DB_BASE dbCutting;	
	_QUEST_DB_BASE	 dbQuest;
	
	_UNIT_DB_BASE	 dbUnit;	
	_ANIMUS_DB_BASE  dbAnimus;		

	_AVATOR_DATA(){ InitData(); }

	void InitData(){

		dbAvator.Init();
		dbBelt.Init();
		dbEquip.Init();
		dbForce.Init();
		dbAnimus.Init();
		dbStat.Init();
		dbInven.Init();
		dbCutting.Init();
	}	
};

struct _STORAGE_POS	//저정장소정보
{		
	enum { INVEN = 0, EQUIP, EMBELLISH, BELT, FORCE, ANIMUS, LINKER };
	//인벤토리: 0, 장비: 1 장식: 2, 벨트:3, 포스:4, 소환:5, 링크:6
	enum { POS_NUM = LINKER+1 };
	enum { STORAGE_NUM = ANIMUS+1 };
	
	BYTE byStorageCode;
};

struct _STORAGE_POS_INDIV : public _STORAGE_POS	//저장물품의 개별정보
{
	WORD wItemSerial;
	BYTE byNum;
};

struct _GLBID{		//글로벌 아이디

	DWORD	dwIndex;
	DWORD	dwSerial;
};

struct _CLID{		//로컬 아이디	

	WORD	wIndex;
	DWORD	dwSerial;
};

struct _CHRID{		//월드에서 케릭터 구분자..

	BYTE byID;
	WORD wIndex;	//del(2/25) .. 둘중의 하나 지우기..
	DWORD dwSerial;	//del(2/25)
};

struct _COUNT_LV	
{
	DWORD	dwCnt;
	int		nLv;
};

struct _ITEM_EFFECT {	//아이템효과

	int nEffectCode;		//효과코드
	float fEffectValue;		//효과치
};

struct _WORLD_INFO			//월드정보
{	
	BYTE byWorldCode;		//월드코드
	bool bOpen;				//계정에 연결
	bool bService;			//서비스가능
	int	 nUserNum;			//해당 월드의  접속자수
};

struct _animus_param		//애미머스 아이템의 업그레이드정보를 형변환하여 접근하는 구조체
{
	WORD wHP;				//hp
	WORD wFP;				//fp
};

struct _unit_bullet_param	//유닛의 탄장정보를 형변환하여 접근하는 구조체
{
	WORD wBulletIndex;		//탄 인덱스
	WORD wLeftNum;			//남은탄수

	_unit_bullet_param(){
		*(DWORD*)this = 0xFFFFFFFF;
	}

	int IsFill(){
		if(*(DWORD*)this == 0xFFFFFFFF)
			return false;
		return true;
	}
};

struct _tuning_data			//유닛 튜닝시 바뀐부분에대한 구조체
{
	BYTE byPartCode;		//파트코드	unit_part_head, unit_part_upper, unit_part_lower, unit_part_arms, unit_part_shoulder, unit_part_back		
	BYTE byPartIndex;		//파트별데이터 인덱스
};


struct _ENTER_WORLD_TEMP_DATA
{
	DWORD				m_dwWorldIP;
	WORD				m_wWorldPort;
	char				m_szID[max_id_len+1];
	DWORD				m_dwAccountSerial;
	DWORD				m_dwWorldMasterKey[KEY_NUM];
};

#pragma pack(pop)


#endif