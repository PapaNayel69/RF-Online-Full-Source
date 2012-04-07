////////////////////////////////////////////////////////////////////////////////
//
// Define & Macro
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __DEFINE_H__
#define __DEFINE_H__

#include "./Network/Protocol/CharacterDB.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define _NEW( x, type )					{ if( !x )	{ x = new type; } }
#define _NEW_ARRAY( x, type, num )		{ if( !x )	{ x = new type[num]; } }
#define _DELETE( x )					{ if( x )	{ delete x; x = NULL; } }
#define _DELETE_ARRAY( x )				{ if( x )	{ delete [] x; x = NULL; } }

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

enum	// Language Type
{
	LT_KOREAN, LT_ENGLISH, LT_CHINESE, LT_JAPANESE, MAX_LANGUAGE_TYPE
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////          CObject           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

//#define ID_INVALID				0xFFFFFFFF
#define ID_INVALID					( -1L )

//------------------------------------------------------------------------------
// Object Type ID ( OTI_ )
#define OTI_CHAR					0x1
#define OTI_ITEM					0x2
#define OTI_UI						0x4

//------------------------------------------------------------------------------
// Object Status ID ( OSI_ )
#define OSI_CREATE					0x00000001
#define OSI_DESTROY					0x00000003

//------------------------------------------------------------------------------

#define MAX_VISUAL_FIELD			( 350.0f )//( 400.0f )
#define MIN_VISUAL_FIELD			( 200.0f )//( 250.0f )

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         CCharacter         //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// 캐릭터 타입에 대한 매크로 ( CTI = Character Type ID )
#define CTI_INVALID					0xFFFFFFFF

#define MAX_CHAR_TYPE				( 6 )			// CTI_UNIT + 1
#define CTI_AVATAR					0xF				// 아바타를 제외한 나머지 값들은 서버측과 맞춘것임
#define CTI_PLAYER					obj_id_player	// 0				
#define CTI_MONSTER					obj_id_monster	// 1
#define CTI_NPC						obj_id_npc		// 2
#define CTI_ANIMUS					obj_id_animus	// 3
#define CTI_GUARD_TOWER				obj_id_tower	// 4
#define CTI_UNIT					0x5
#define CTI_OBSERVER				0x6
#define CTI_DUMMY					0x7

//------------------------------------------------------------------------------
// 캐릭터 클래스 ( PCT = Player Class Type )
#define	PCT_WARRIOR					( 0 )
#define	PCT_RANGER					( 1 )
#define	PCT_SPIRITUALIST			( 2 )
#define	PCT_SPECIALIST				( 3 )
#define	MAX_CLASS_TYPE				PCT_SPECIALIST

//------------------------------------------------------------------------------
// 캐릭터 액션에 대한 매크로 ( CAI = Character Action ID )
#define CAI_INVALID					0xFFFFFFFF

#define CAI_MOVE					0x1
#define CAI_MOVE_STOP				0x10000000
#define CAI_MOVE_MOVING				0x10000001
#define CAI_MOVE_IDLE				0x10000002
#define CAI_COMBAT					0x2
#define CAI_COMBAT_READY			0x20000000
#define CAI_COMBAT_ATTACK			0x20000001
#define CAI_COMBAT_ATTACK_ERROR		0x20000002		// 공격 에러 ( 서버에서온 결과값이 에러임 )
#define CAI_COMBAT_DEFENCE			0x20000003
#define CAI_FORCE					0x3
#define CAI_FORCE_READY				0x30000000
#define CAI_FORCE_CAST				0x30000001
#define CAI_FORCE_INVOKE			0x30000002
#define CAI_SKILL					0x4
#define CAI_SKILL_READY				0x40000000
#define CAI_SKILL_CAST				0x40000001
#define CAI_SKILL_INVOKE			0x40000002
#define CAI_DEATH					0x5
#define CAI_DEATH_READY				0x50000000
#define CAI_DEATH_DIE				0x50000001
#define CAI_DAMAGED					0x6				// 데미지를 받았음
#define CAI_DAMAGED_NORMAL			0x60000000		// 일반 형태의 데미지
#define CAI_DAMAGED_CRITICAL		0x60000001		// 크리티컬 데미지
#define CAI_DAMAGED_BY_SKILL		0x60000002		// 스킬 데미지
#define CAI_DAMAGED_BY_FORCE		0x60000003		// 포스 데미지
#define CAI_DAMAGED_MISS			0x60000004		// 데미지 ( 미스 )
#define CAI_ITEM					0x7
//#define CAI_ITEM_PICKUP			0x70000000
#define CAI_ITEM_USE				0x70000001
#define CAI_ITEM_USE_ITEM			0x70000002		// 이 코드는 애니메이션에서는 안 쓰이고, 이펙트를 설정할때만 쓰인다.
#define CAI_ITEM_USE_POTION			0x70000003
#define CAI_ITEM_USE_MINING			0x70000004
#define CAI_ITEM_USE_RETURN			0x70000005
#define CAI_ITEM_USE_DUNGEON		0x70000006
#define CAI_ITEM_USE_STOP			0x70000007
#define CAI_ITEM_CHANGE_WEAPON		0x70000008
#define CAI_CREATE					0x8
#define CAI_CREATE_CREATE			0x80000000
#define CAI_GESTURE					0x9
#define CAI_GESTURE_LEVEL_UP		0x90000000
#define CAI_GESTURE_CLASS_CHANGE	0x90000001
#define CAI_GESTURE_STANDBY			0x90000002
#define CAI_GESTURE_MOUNT			0x90000003
#define CAI_GESTURE_DISMOUNT		0x90000004
#define CAI_GESTURE_SUMMON			0x90000005		// 소환자만 사용 ( 소환수가 생성될 때는 CAI_CREATE_SUMMON을 사용 )
#define CAI_GESTURE_REPATRIATE		0x90000006		// 소환자만 사용 ( 소환수가 사라질 때는 CAI_DEATH_SUMMON을 사용 )
#define	CAI_GESTURE_MOVEMAP_ENTER	0x90000007
#define	CAI_GESTURE_MOVEMAP_EXIT	0x90000008
#define	CAI_GESTURE_DUNGEON_ENTER	0x90000009
#define	CAI_GESTURE_DUNGEON_EXIT	0x9000000A
#define	CAI_GESTURE_GAME_LOGOUT		0x9000000B
#define CAI_INTERNALSTATE			0xA
#define CAI_INTERNALSTATE_DEATH		0xA0000000
#define CAI_INSTANT_ACTION			0xB
#define CAI_INSTANT_ACTION_SKILL	0xB0000001
#define CAI_INSTANT_ACTION_FORCE	0xB0000002

#define CAI_NEXTACTION				0xF0000000		// 넥스트 액션은 오직 다음에 행할 상태의 저장에 쓰이며 실제 캐릭터의 액션 설정등에는 전혀 영향을 미치지 않는다.
#define CAI_NEXTACTION_SELECT_CHAR	0xF0000001

//------------------------------------------------------------------------------
// 캐릭터의 움직임 모드
#define CMM_MOVE_STOP					3
#define CMM_MOVE_WALK					move_type_walk		// 0	// CMI_MOVE_WALK와 CMI_MOVE_RUN의
#define CMM_MOVE_RUN					move_type_run		// 1	// 값은 서버와 같아야 한다.
#define CMM_MOVE_FLY					2							// CMM_MOVE_FLY는 나중을 위해서..

//------------------------------------------------------------------------------
// 캐릭터 상태에 대한 매크로 ( CSI = Character Status ID )
#define CSI_INVALID						

#define CSI_PEACE						mode_type_demili	// 0	// CSI_PEACE와 CSI_COMBAT값은 서버와 같아야 된다.
#define CSI_COMBAT						mode_type_mili		// 1

//------------------------------------------------------------------------------
// 캐릭터의 공격 형태 ( CAT_ = Character Attack Type )
#define CAT_NONE						0
#define CAT_NORMAL						3
#define CAT_SKILL						effect_code_skill	// 0
#define CAT_FORCE						effect_code_force	// 1
#define CAT_CLASS_SKILL					effect_code_class	// 2

//------------------------------------------------------------------------------
// 캐릭터의 공격의 사정거리 형태 형태 ( ADT_ = Attack Distance Type )
#define ADT_NONE						0
#define ADT_NEAR						1
#define ADT_FAR							2

//------------------------------------------------------------------------------
// 캐릭터의 텍스트 컬러
#define CHAR_AVATAR_NAME_COLOR			0xFFFFFFFF
#define CHAR_PLAYER_NAME_COLOR			0xFFEDC650
#define CHAR_GM_NAME_COLOR				0xFFFFA0FF
#define CHAR_DEV_NAME_COLOR				0xFF80FFFF
#define CHAR_NPC_NAME_COLOR				0xFF08FF08
#define CHAR_MONSTER_NAME_COLOR			0xFFCC5050
#define CHAR_SUMMON_MONSTER_NAME_COLOR	0xFF5050CC

#define CHAR_DAMAGE_NUMBER_COLOR		0xFFFF0000

//------------------------------------------------------------------------------
// Internal State Damage Type
#define ISDT_NORMAL						0x0
#define ISDT_FORCE						0x1
#define ISDT_SKILL						0x2

//------------------------------------------------------------------------------
// Character Creation Type ( 서버와 맞추어야 함 )
#define CCT_NORMAL						0x0
#define CCT_SUMMON						0x1

//------------------------------------------------------------------------------
// Summon Creature Order Type ( 소환된 크리쳐들에게 내리는 명령 타입 - 서버와 맞추어야 함 )
#define	SCOT_FOLLOW						0x0
#define	SCOT_COMBAT						0x1

//------------------------------------------------------------------------------
// 기타
#define CHAR_SPEED_DEFAULT				( 15.0f )

#define IDLE_TIME1						( 60000 )
#define IDLE_TIME2						( 5000 )

/*
#define CHAR_SPEED_WALK					( CHAR_SPEED_DEFAULT * 1.3f )
#define CHAR_SPEED_RUN					( CHAR_SPEED_DEFAULT * 2.8f )
#define CHAR_SPEED_FLY					( CHAR_SPEED_DEFAULT * 5.0f )

#define MONSTER_SPEED_WALK				( CHAR_SPEED_DEFAULT * 1.5f )
#define MONSTER_SPEED_RUN				( CHAR_SPEED_DEFAULT * 1.5f )
#define MONSTER_SPEED_FLY				( CHAR_SPEED_DEFAULT * 1.5f )*/

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////       Animation ID         //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Player Animation ID ( PAI_ )
#define PAI_WALK					0x00
#define PAI_RUN						0x01
#define PAI_DAMAGE_NORMAL			0x02
#define PAI_IDLE					0x03
#define PAI_USEITEM					0x04
#define PAI_ATTACK					0x05
#define PAI_STOP					0x06
#define PAI_PICKUP					0x07
#define PAI_DEATH_READY				0x08
#define PAI_DEATH_DIE				0x09
#define PAI_DAMAGE_CRITICAL			0x0A
#define PAI_UNIT_MOUNT				0x0B
#define PAI_UNIT_STANDBY			0x0C
#define PAI_UNIT_DISMOUNT			0x0D
#define PAI_UNIT_BOOSTER			0x0E

#define PAI_CREATE_CREATE			PAI_DEATH_DIE

//------------------------------------------------------------------------------
// Monster Animation ID ( MAI_ )
#define MAI_PEACE_STOP				0x00
#define MAI_PEACE_WALK				0x01
#define MAI_PEACE_IDLE				0x02
#define MAI_PEACE_DAMAGE			0x03
#define MAI_COMBAT_STOP				0x04
#define MAI_COMBAT_WALK				0x05
#define MAI_COMBAT_IDLE				0x06
#define MAI_COMBAT_DAMAGE			0x07
#define MAI_COMBAT_POWERUP			0x08
#define MAI_COMBAT_ATTACK			0x09
#define MAI_DEATH_READY				0x0A
#define MAI_DEATH_DIE				0x0B
#define MAI_FORCE_ATTACK			0x0C
#define MAI_FORCE_SUPPORT			0x0D
#define MAI_PEACE_CRITICAL			0x0E
#define MAI_COMBAT_CRITICAL			0x0F

#define	MAI_CREATE_CREATE			MAI_DEATH_DIE

//------------------------------------------------------------------------------
// Npc Animation ID ( NAI_ )
#define NAI_PEACE_STOP				0x00
#define NAI_PEACE_WALK				0x01
#define NAI_PEACE_IDLE				0x02
#define NAI_PEACE_DAMAGE			0x03
#define NAI_COMBAT_STOP				0x04
#define NAI_COMBAT_WALK				0x05
#define NAI_COMBAT_IDLE				0x06
#define NAI_COMBAT_DAMAGE			0x07
#define NAI_COMBAT_POWERUP			0x08
#define NAI_COMBAT_ATTACK			0x09
#define NAI_DEATH_READY				0x0A
#define NAI_DEATH_DIE				0x0B
#define NAI_SKILL_ATTACK			0x0C
#define NAI_SKILL_SUPPORT			0x0D
#define NAI_PEACE_CRITICAL			0x0E
#define NAI_COMBAT_CRITICAL			0x0F

//------------------------------------------------------------------------------
// Aminus Animation ID ( AAI_ )
#define AAI_CREATE_CREATE			0x00
#define AAI_CREATE_DYING			0x01
#define AAI_CREATE_DEATH			0x02
#define AAI_CREATE_SUMMON			0x03
#define AAI_CREATE_RETURN			0x04
#define AAI_MOVE_STOP				0x05
#define AAI_MOVE_WALK				0x06
#define AAI_MOVE_IDLE				0x07
#define AAI_COMBAT_ATTACK			0x08
#define AAI_FORCE_ATTACK			0x09
#define AAI_DAMAGE_CRITICAL			0x0A

//------------------------------------------------------------------------------
// Guard Tower Animation ID ( AAI_ )
#define GAI_CREATE_CREATE			0x00
#define GAI_CREATE_DYING			0x01
#define GAI_CREATE_DEATH			0x02
#define GAI_CREATE_SUMMON			0x03
#define GAI_CREATE_RETURN			0x04
#define GAI_MOVE_STOP				0x05
#define GAI_MOVE_WALK				0x06
#define GAI_MOVE_IDLE				0x07
#define GAI_COMBAT_ATTACK			0x08
#define GAI_FORCE_ATTACK			0x09
#define GAI_DAMAGE_CRITICAL			0x0A


//------------------------------------------------------------------------------
// Item Animation ID ( IAI_ )
#define IAI_DEFAULT					0x00
#define IAI_EQUIP					0x01
#define IAI_UNEQUIP					0x02
#define IAI_ATTACK					0x03
#define IAI_USE						0x04

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////            Race            //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*//----------------------------------------------------------------------------
// Character Race Type
#define CRT_INVALID					0xFFFFFFFF
#define CRT_BELLATOR_M				0x00000000
#define CRT_BELLATOR_F				0x00000001
#define CRT_CORA_M					0x00000002
#define CRT_CORA_F					0x00000003
#define CRT_ACCRETIAN				0x00000004
#define CRT_DEFAULT					CRT_BELLATOR_M*/

//------------------------------------------------------------------------------
// Character Race Type
#define CRT_BELLATOR_MALE			0x0
#define CRT_BELLATOR_FEMALE			0x1
#define CRT_CORA_MALE				0x2
#define CRT_CORA_FEMALE				0x3
#define CRT_ACCRETIA				0x4
#define CRT_DEFAULT					CRT_BELLATOR_MALE

#define CRT_MALE					0x5
#define CRT_FEMALE					0x6
#define CRT_BELLATOR				0x7
#define CRT_CORA					0x8
#define CRT_BELLATOR_CORA			0x9
#define CRT_ALL						0xA			// 모든 종족

//------------------------------------------------------------------------------
// Character Zender Type
//#define CZT_MALE					0x0
//#define CZT_FEMALE					0x1
//#define CZT_DEFAULT					CZT_MALE

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////      Resource & Item       //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// 아이템 타입에 대한 매크로 ( ITI = Item Type ID )
#define ITI_GROUND					0x0				// ground item
#define ITI_PROPERTY				0x1				// property item
#define ITI_TRADE					0x2				// item for trade
#define ITI_DATA					0x3				// item for data

//------------------------------------------------------------------------------
// Character Default Part Type
#define MAX_DEFAULT_PART			( 6 )
#define CDPT_FACE					0
#define CDPT_UPPER_PART				1
#define CDPT_LOWER_PART				2
#define CDPT_GLOVES					3
#define CDPT_SHOES					4
#define CDPT_HELMET					5

//------------------------------------------------------------------------------
// Character Equip Part Type
#define MAX_EQUIP_PART				( 9 )
#define CEPT_FACE					CDPT_FACE
#define CEPT_UPPER_PART				CDPT_UPPER_PART
#define CEPT_LOWER_PART				CDPT_LOWER_PART
#define CEPT_GLOVES					CDPT_GLOVES
#define CEPT_SHOES					CDPT_SHOES
#define CEPT_HELMET					CDPT_HELMET
#define CEPT_WEAPON					6
#define CEPT_SHIELD					7
#define CEPT_CLOAK					8
#define CEPT_HAIR					MAX_EQUIP_PART	// hair는 클라이언트에서만 사용됨 => IEPT_RING과 값이 겹치지만 캐릭터의 생성시 데이터 세팅에만 사용하므로 상관없음 => 단순히 플레이어의 렌더링과 애니메이션시에만 쓰인다.

//------------------------------------------------------------------------------
// Item Equip or Possession Type ( 아이템의 장착 혹은 소유 형태에 따른 타입 )
#define MAX_ITEM_TYPE				( 26 + 8 )		// 항상 + 8 이다.
#define IEPT_FACE					CEPT_FACE
#define IEPT_UPPER_PART				CEPT_UPPER_PART
#define IEPT_LOWER_PART				CEPT_LOWER_PART
#define IEPT_GLOVES					CEPT_GLOVES
#define IEPT_SHOES					CEPT_SHOES
#define IEPT_HELMET					CEPT_HELMET
#define IEPT_WEAPON					CEPT_WEAPON
#define IEPT_SHIELD					CEPT_SHIELD
#define IEPT_CLOAK					CEPT_CLOAK
#define IEPT_RING					9
#define IEPT_AMULET					10
#define IEPT_BULLET					11
#define IEPT_WORKTOOL				12
#define IEPT_POTION					13
#define IEPT_ETC					14
#define IEPT_BATTERY				15
#define IEPT_ORE					16
#define IEPT_INGOT					17
#define IEPT_FORCE					18
#define IEPT_UNIT					19
#define IEPT_LOOT					20
#define IEPT_MAP					21
#define IEPT_RETURN					22
#define IEPT_DUNGEON				23
#define IEPT_ANIMUS					24
#define IEPT_GUARD_TOWER			25
#define IEPT_UNIT_HELMET			26				// Unit Part 데이터부터는 클라이언트 전용임.
#define IEPT_UNIT_UPPER_PART		27
#define IEPT_UNIT_LOWER_PART		28
#define IEPT_UNIT_ARMS				29
#define IEPT_UNIT_SHOULDER			30
#define IEPT_UNIT_BACKPACK			31
#define IEPT_UNIT_BULLET			32
#define IEPT_ITEM_WORK				33				// 아이템 제작 데이터는 항상 제일 마지막 ( 원래는 아이템 데이터가 아니지만, 편의상! )

//------------------------------------------------------------------------------
// item type ( with Server )
const CHAR IEPT_SERVER_TO_CLIENT[MAX_ITEM_TYPE] =
{
	IEPT_UPPER_PART, IEPT_LOWER_PART, IEPT_GLOVES, IEPT_SHOES, IEPT_HELMET,
	IEPT_SHIELD, IEPT_WEAPON, IEPT_CLOAK, IEPT_RING, IEPT_AMULET, 
	IEPT_BULLET, IEPT_WORKTOOL, IEPT_ETC, IEPT_POTION, IEPT_FACE,
	IEPT_FORCE, IEPT_BATTERY, IEPT_ORE, IEPT_INGOT, IEPT_UNIT,
	IEPT_LOOT, IEPT_MAP, IEPT_RETURN, IEPT_DUNGEON, IEPT_ANIMUS,
	IEPT_GUARD_TOWER
};
const CHAR IEPT_CLIENT_TO_SERVER[MAX_ITEM_TYPE] =
{
	tbl_code_face, tbl_code_upper, tbl_code_lower, tbl_code_gauntlet, tbl_code_shoe,
	tbl_code_helmet, tbl_code_weapon, tbl_code_shield, tbl_code_cloak, tbl_code_ring,
	tbl_code_amulet, tbl_code_bullet, tbl_code_maketool, tbl_code_potion, tbl_code_bag,
	tbl_code_battery, tbl_code_ore, tbl_code_res, tbl_code_fcitem, tbl_code_key,
	tbl_code_booty, tbl_code_map, tbl_code_town, tbl_code_bdungeon, tbl_code_animus,
	tbl_code_tower
};

//------------------------------------------------------------------------------
// character equip part type ( with UI )
#define MAX_EQUIP_WINDOW_SLOT		( 15 )

#define UIPT_FACE					-1
#define UIPT_HELMET					0
#define UIPT_UPPER_PART				1
#define UIPT_LOWER_PART				2
#define UIPT_WEAPON					3
#define UIPT_SHOES					4
#define UIPT_CLOAK					5
#define UIPT_SHIELD					6
#define UIPT_GLOVES					7
#define UIPT_RING_LEFT				8
#define UIPT_RING_RIGHT				9
#define UIPT_AMULET_LEFT			10
#define UIPT_AMULET_RIGHT			11
#define UIPT_BULLET_LEFT			12
#define UIPT_BULLET_RIGHT			13

const CHAR CEPT_UI_TO_CLIENT[MAX_EQUIP_WINDOW_SLOT] =
	{ IEPT_HELMET, IEPT_UPPER_PART, IEPT_LOWER_PART, IEPT_WEAPON,
	  IEPT_SHOES, IEPT_CLOAK, IEPT_SHIELD, IEPT_GLOVES,
	  IEPT_RING, IEPT_RING, IEPT_AMULET, IEPT_AMULET, IEPT_BULLET, IEPT_BULLET, -1 };

const CHAR CEPT_CLIENT_TO_UI[MAX_EQUIP_WINDOW_SLOT] =
	{ UIPT_FACE, UIPT_UPPER_PART, UIPT_LOWER_PART, UIPT_GLOVES, UIPT_SHOES,
	  UIPT_HELMET, UIPT_WEAPON, UIPT_SHIELD, UIPT_CLOAK, UIPT_RING_LEFT,
	  UIPT_AMULET_LEFT, UIPT_BULLET_LEFT, -1, -1, -1 };

//------------------------------------------------------------------------------
// Character Bone & Mesh
#define MAX_DEFAULT_ITEM_NUM		( 16 )			// 각 캐릭터별 디폴트 아이템의 갯수

#define ID_DEFAULT_ARMOR_BM			( 0 )			// 휴먼 남자
#define ID_DEFAULT_ARMOR_BF			( 16 )			// 휴먼 여자
#define ID_DEFAULT_ARMOR_CM			( 32 )			// 이계 남자
#define ID_DEFAULT_ARMOR_CF			( 48 )			// 이계 여자
#define ID_DEFAULT_ARMOR_AC			( 64 )			// 메카
const	DWORD ID_DEFAULT_ARMOR[MAX_DEFAULT_ITEM_NUM] =
									{ ID_DEFAULT_ARMOR_BM, ID_DEFAULT_ARMOR_BF,
									ID_DEFAULT_ARMOR_CM, ID_DEFAULT_ARMOR_CF,
									ID_DEFAULT_ARMOR_AC };

#define ID_DEFAULT_BONE_BM			0
#define ID_DEFAULT_BONE_BF			1
#define ID_DEFAULT_BONE_CM			2
#define ID_DEFAULT_BONE_CF			3
#define ID_DEFAULT_BONE_AC			4

//------------------------------------------------------------------------------
// ITEM Weapon Type ( IWT_ )
#define IWT_SWORD_R					0x00		// Right Hand
#define IWT_SWORD_B					0x01		// Both Hands
#define IWT_SWORD_D					0x02		// Dual Sword
#define IWT_KNIFE_R					0x03
#define IWT_KNIFE_B					0x04
#define IWT_KNIFE_LT				0x05		// Left Hand & Throw
#define IWT_AXE_R					0x06
#define IWT_AXE_B					0x07
#define IWT_AXE_LT					0x08
#define IWT_AXE_D					0x09
#define IWT_MACE_R					0x0A
#define IWT_MACE_B					0x0B
#define IWT_STAFF_R					0x0C
#define IWT_STAFF_B					0x0D
#define IWT_SPEAR_B					0x0E
#define IWT_BOW_B					0x0F
#define IWT_CROSSBOW_B				0x10
#define IWT_GUN_B					0x11
#define IWT_GUN_D					0x12
#define IWT_RIFLE_B					0x13
#define IWT_LAUNCHER_B				0x14
#define IWT_FAUST_B					0x15
#define IWT_MACHINEGUN_B			0x16
#define IWT_FLAMETHROWER_B			0x17
#define IWT_BEAM_GUN_B				0x18
#define IWT_BEAM_GUN_D				0x19
#define IWT_BEAM_RIFLE_B			0x1A
#define IWT_BEAM_PLASMA_B			0x1B
#define IWT_MINING_TOOL				0x1C
#define IWT_FIST					0xFF

//------------------------------------------------------------------------------
// Item Weapon Category Type ( IWCT_ )
#define IWCT_KNIFE					0x0
#define IWCT_SWORD					0x1
#define IWCT_AXE					0x2
#define IWCT_MACE					0x3
#define IWCT_SPEAR					0x4
#define IWCT_BOW					0x5
#define IWCT_FIREARM				0x6
#define IWCT_LUANCHER				0x7
#define IWCT_THROW					0x8
#define IWCT_STAFF					0x9
#define IWCT_MINE					0xA
#define IWCT_EX1					0xB
#define IWCT_EX2					0xC
#define IWCT_EX3					0xD
#define IWCT_EX4					0xE
#define IWCT_EX5					0xF

//------------------------------------------------------------------------------
// Item Ore Type ( IOT_ )
#define IOT_BLUE					0x0
#define IOT_RED						0x1
#define IOT_YELLOW					0x2
#define IOT_GREEN					0x3
#define IOT_BLACK					0x4

//------------------------------------------------------------------------------
// Item Hold Type ( IHT_ )
#define IHT_NONE					0
#define IHT_LEFT_HAND				1
#define IHT_RIGHT_HAND				2
#define IHT_BOTH_HAND				3
#define IHT_DUAL_HAND				4

//------------------------------------------------------------------------------
// Item Attribute Type ( IAT_ )
#define MAX_ITEM_ATTRIBUTE			( 4 )		// 아이템의 최대 속성 수
#define IAT_FIRE					0x0
#define IAT_WATER					0x1
#define IAT_EARTH					0x2
#define IAT_WIND					0x3

#define MAX_RESIST					( 200 )		// 캐릭터의 내성 한계치

//------------------------------------------------------------------------------
// Item Money Type ( IMT_ )
#define IMT_DALANT					( money_unit_dalant )
#define IMT_GOLD					( money_unit_gold )

//------------------------------------------------------------------------------
// Expertness Value Type ( EVT_ )
/*
#define MAX_EXPERTNESS_VALUE_TYPE	0xB
#define EVT_MELEE					0x0
#define EVT_BOW						0x1
#define EVT_FIREARM					0x2
#define EVT_THROW					0x3
#define EVT_RACE_SKILL				0x4		// 종족별 특수 스킬 ( 유닛, 소환, 런쳐 )
#define EVT_FORCE					0x5
#define EVT_SHIELD					0x6
#define EVT_DEFENCE					0x7

#define EVT_MAKE_WEAPON				0x8
#define EVT_MAKE_ARMOR				0x9
#define EVT_MAKE_BULLET				0xA
*/
#define MAX_EXPERTNESS_VALUE_TYPE	0x9
#define EVT_NEAR					0x0
#define EVT_FAR						0x1
#define EVT_RACE_SKILL				0x2		// 종족별 특수 스킬 ( 유닛, 소환, 런쳐 )
#define EVT_FORCE					0x3
#define EVT_SHIELD					0x4
#define EVT_DEFENCE					0x5

#define EVT_MAKE_WEAPON				0x6
#define EVT_MAKE_ARMOR				0x7
#define EVT_MAKE_BULLET				0x8

//--------------------------------------------------------------------------
// Effect Type ID ( ETI_ )
#define	ETI_PLAYER					0x01
#define	ETI_MONSTER					0x02
#define	ETI_ANIMUS					0x03
#define	ETI_TOWER					0x04
#define	ETI_SKILL					0x05
#define	ETI_FORCE					0x06
#define	ETI_ITEM					0x07
#define	ETI_UNIT					0x08

//------------------------------------------------------------------------------
// Item Effect Type ( IET_ )
#define IET_DEFAULT					0x00		// default
#define IET_ATTACK_NORMAL			0x01		// attack effect ( by normal )
#define IET_ATTRIBUTE				0x02		// attribute of item ( fire, water, etc... )
#define IET_USE						0x03		// use item ( include arrow )
#define IET_ATTACK_SKILL			0x04		// attack effect ( by skill )
#define IET_ATTACK_FORCE			0x05		// attack effect ( by force )
#define IET_USE_STOP				0x06		// stop use

//------------------------------------------------------------------------------
// Player Effect Type ( PET_ )
#define PET_DAMAGE_NORMAL1			0x00
#define PET_DAMAGE_NORMAL2			0x01
#define PET_GESTURE_LEVELUP			0x02
#define PET_CREATE_CREATE			0x03
#define PET_DAMAGE_CRITICAL			0x04
#define PET_DAMAGE_MISS				0x05
#define PET_CREATE_DESTROY			0x06
#define PET_ATTACK_ERROR			0x07
#define PET_GESTURE_MOUNT			0x08
#define PET_GESTURE_DISMOUNT		0x09
#define PET_GESTURE_SUMMON			0x0A
#define PET_GESTURE_REPATRIATE		0x0B
#define PET_MOVEMAP_ENTER			0x0C
#define PET_MOVEMAP_EXIT			0x0D
#define PET_DUNGEON_ENTER			0x0E
#define PET_DUNGEON_EXIT			0x0F
#define PET_GESTURE_CLASS_CHANGE	0x10
#define PET_GESTURE_LOGOUT			0x11

//------------------------------------------------------------------------------
// Monster Effect Type ( MET_ )
#define MET_CREATE_DESTROY			0x00
#define MET_CREATE_CREATE			0x01
#define MET_DAMAGE_NORMAL			0x02
#define MET_DAMAGE_CRITICAL			0x03
#define MET_DAMAGE_MISS				0x04
#define MET_ATTACK_NORMAL			0x05
#define MET_ATTACK_FORCE			0x06
#define MET_ATTACK_ERROR			0x07
#define MET_GESTURE_SUMMON			0x08
#define MET_GESTURE_REPATRIATE		0x09
#define MET_GESTURE_LEVELUP			0x0A
#define MET_GESTURE_CRAZY			0x0B

//------------------------------------------------------------------------------
// Animus Effect Type ( AET_ )
#define AET_CREATE_DESTROY			0x00
#define AET_CREATE_CREATE			0x01
#define AET_DAMAGE_NORMAL			0x02
#define AET_DAMAGE_CRITICAL			0x03
#define AET_DAMAGE_MISS				0x04
#define AET_ATTACK_NORMAL			0x05
#define AET_ATTACK_FORCE			0x06
#define AET_ATTACK_ERROR			0x07
#define AET_GESTURE_SUMMON			0x08
#define AET_GESTURE_REPATRIATE		0x09
#define AET_GESTURE_LEVELUP			0x0A
#define AET_GESTURE_CRAZY			0x0B

//------------------------------------------------------------------------------
// Unit Effect Type ( UET_ )
#define UET_CREATE					0x00
#define UET_DESTROY					0x01
#define UET_DAMAGE					0x02
#define UET_DAMAGE_MISS				0x03
#define UET_CRITICAL				0x04
#define UET_ATTACK					0x05
#define UET_REPAIR					0x06
#define UET_GESTURE_MOUNT			0x07
#define UET_GESTURE_DISMOUNT		0x08

//------------------------------------------------------------------------------
// Fade Render Mode ( FRM_ )
#define	FRM_NONE					0x0
#define FRM_IN						0x1
#define FRM_OUT						0x2
#define FRM_IN_END					0x3			// fade in 이 끝났음
#define FRM_OUT_END					0x4			// fade out 이 끝났음

//------------------------------------------------------------------------------
// Item Trade Type ( ITT_ )
#define ITT_NONE					0x0
#define ITT_BUY						0x1
#define ITT_SELL					0x2
#define ITT_REPAIR					0x3
#define ITT_EXCHANGE				0x4

//------------------------------------------------------------------------------
// NPC Merchant Type ( NMT_ )
#define NMT_WEAPON					0x00
#define NMT_POTION					0x01
#define NMT_ARMOR					0x02
#define NMT_SUNDRIES				0x03
#define NMT_UNIT					0x04
#define NMT_ORE						0x05
#define NMT_LOOT					0x06
#define NMT_GENERAL					0x07
#define NMT_PEDDLER					0x08
#define NMT_GAMBLER					0x09
#define NMT_EXCHANGE				0x0A
#define NMT_KEEPING					0x0B
#define NMT_GUILD_MANAGER			0x0C
#define NMT_GUILD_WAREHOUSE			0x0D
#define NMT_CASINO					0x0E
#define NMT_RACE_MANAGER			0x0F
#define NMT_FORCE					0x10
#define NMT_LIST					0xFF

//------------------------------------------------------------------------------
// NPC Button Function Type ( NBFT_ )
#define NBFT_NONE					0			// 기능없음
#define NBFT_TRADE					1			// 아이템 거래
#define NBFT_QUEST					2			// 퀘스트
#define NMBT_ECONOMY				3			// 경제지표 확인
#define NMBT_EXCHANGE				4			// 환전
#define NMBT_PROCESS				5			// 가공
#define NMBT_WAREHOUSE				6			// 아이템/돈 창고 기능
#define NMBT_INFO					7			// 안내
#define NMBT_ENTER					8			// 입장
#define NMBT_VIEW_KILL_POINT		9			// 킬포인트
#define NMBT_UNIT_TUNING			10			// 기갑 장비 튜닝
#define NMBT_UNIT_STOREHOUSE		11			// 기갑 장비 보관
#define NMBT_CLASS_CHANGE			12			// 전직

//------------------------------------------------------------------------------
// ETC
#define MAX_EFFECT_FRAME_NUM		10			// 애니메이션 1개당 이펙트 프레임의 최대 갯수
#define	MAX_ITEM_LEVEL				7			// 아이템의 최대 업그레이드 단계
#define MAX_ITEM_EFFECT				( 4 )		// 아이템에 걸려있는 최대 이펙트 수

#define	DEFAULT_EXCHANGE_RATE		( 1000 )	// 기본 환율

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////       Skill & Force        //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Skill Class Type ( SCT_ )
#define SCT_MELEE					0x0
#define SCT_BOW						0x1
#define SCT_FIREARM					0x2
#define SCT_THROW					0x3
#define SCT_LAUNCHER				0x4
#define SCT_KNOWLEDGE				0x5

#define SCT_ASSISTANCE_FOR_ANI		0xF		// 보조는 애니메이션만을 위해서 사용된다 => 실제로 보조는 존재하지 않는다.

// Skill Class Type ( for UI - SCT_UI_ )
#define SCT_UI_MELEE				0x0
#define SCT_UI_MISSILE				0x1
#define SCT_UI_KNOWLEDGE			0x2

//------------------------------------------------------------------------------
// Force Class Type ( FCT_ )
#define FCT_DARK					0x0
#define FCT_HOLY					0x1
#define FCT_FIRE					0x2
#define FCT_AQUA					0x3
#define FCT_TERRA					0x4
#define FCT_WIND					0x5

// Force Class Type ( for UI - FCT_UI_ )
#define FCT_UI_HOLYnDARK			0x0
#define FCT_UI_FIRE					0x1
#define FCT_UI_AQUA					0x2
#define FCT_UI_TERRA				0x3
#define FCT_UI_WIND					0x4

//------------------------------------------------------------------------------
// Skill / Force Target Type ( SFT_ )
#define MAX_SF_TARGET_TYPE			( 5 )
#define SFT_ANYWHERE				( 0 )
#define SFT_SAME_RACE				( 1 )
#define SFT_ENEMY_PLAYER			( 2 )
#define SFT_ENEMY_NPC				( 3 )
#define SFT_MAP						( 4 )

//------------------------------------------------------------------------------
// Skill / Force Attack Type
#define SFAT_ATTACK						0x0		// 공격형
#define SFAT_SUPPORT_ONCE				0x1		// 보조형 - 일시적
#define SFAT_SUPPORT_CONTINUANCE_LOSS	0x2		// 보조형 - 일시적 - 실
#define SFAT_SUPPORT_CONTINUANCE_PROFIT	0x3		// 보조형 - 일시적 - 득

//------------------------------------------------------------------------------
// Skill / Force Range Type
#define SFRT_ONE_HIT				0x0
#define SFRT_DOUBLE_HIT				0x1
#define SFRT_COMBO_HIT				0x2
#define SFRT_PENALTY				0x3
#define SFRT_SELF_AREA				0x4
#define SFRT_FLASH_DAMAGE			0x5
#define SFRT_EXTENT					0x6
#define SFRT_SECTOR_AREA			0x7
#define SFRT_SUPPORT				0x8

//------------------------------------------------------------------------------
// ETC
#define	MAX_MONSTER_NUM				192		// 데이터 테이블에 존재하는 몬스터의 총 갯수
#define	MAX_SUMMONS_NUM				20		// 아바타가 소환할 수 있는 몬스터의 총 수

#define MAX_SKILL_CLASS_TYPE		6
#define MAX_FORCE_CLASS_TYPE		6

#define MAX_UI_SKILL_CLASS_TYPE		3		// for UI ( melee, far, knowledge )
#define MAX_UI_FORCE_CLASS_TYPE		5		// for UI

#define	MAX_SKILL_MASTERY_NUM		12		// 스킬의 마스터리 갯수
#define	MAX_FORCE_MASTERY_NUM		24		// 포스의 마스터리 갯수
#define	MAX_SF_SOCKET_NUM			100		// 스킬/포스 클래스 당 소켓의 수
#define MAX_SKILL					100
#define MAX_FORCE					100

#define MAX_APPLIED_SF				( 5 )	// 캐릭터에게 적용될 수 있는 지속 보조 스킬/포스의 최대 갯수 ( 득/실 각각 5개 씩임 )

#define	MAX_SF_LEVEL				( 7 )	// 스킬/포스의 최대 레벨
#define MAX_SF_STEP					( 4 )
#define	MAX_SF_EFFECT				( 7 )	// 1개의 스킬/포스가 가질 수 있는 최대 효과 수

#define MAX_ANIMUS_LEVEL			( max_animus_level )

#define	MAX_NAME_LENGTH				( 32 )
#define MAX_DESCRIPTION_LENGTH		( 512 )

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////       Game Progress        //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*//------------------------------------------------------------------------------
// Game Progress ID ( GPI_ )
#define MAX_GAME_PROGRESS			6
#define GPI_TITLE					0
#define GPI_LOGIN					1
#define GPI_CREATE_ACCOUNT			2
#define GPI_SELECT_CHARACTER		3
#define GPI_CREATE_CHARACTER		4
#define GPI_MAINGAME				5*/

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////       User Interface       //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// User Interface ID ( UII_ )
#define MAX_UI							38
#define UII_INVENTORY					0
#define UII_CHAR_INFO					1
#define UII_CHAT_WINDOW					2
#define UII_ITEM_BELT_WINDOW			3
#define UII_SF_BELT_WINDOW				4
#define UII_GAUGE_WINDOW				5
#define UII_SKILL_WINDOW				6
#define UII_FORCE_WINDOW				7
#define UII_SHOP_WINDOW					8
#define	UII_FUNC_KEY_WINDOW				9
#define	UII_CHAR_INFO_WINDOW			10
#define UII_SELECT_ATTACK_PART_WINDOW	11
#define UII_MINING_WINDOW				12
#define UII_ORE_PROC_WINDOW				13
#define UII_EXCHANGE_WINDOW				14
#define	UII_NPC_INTRODUCE_WINDOW		15
#define	UII_ITEM_WORKING_WINDOW			16
#define	UII_ITEM_UPGRADE_WINDOW			17
#define UII_TARGET_INFO_BAR				18
#define UII_TARGET_INFO_WINDOW			19
#define UII_PARTY_WINDOW				20
#define UII_PLAYER_POPUP_MENU			21
#define	UII_MOVE_MAP_WINDOW				22
#define UII_APPLIED_SF_WINDOW			23
#define UII_TRADE_WINDOW				24
#define UII_RADAR_WINDOW				25
#define UII_MINIMAP_WINDOW				26
#define UII_MAP_LOADING_WINDOW			27
#define UII_OPTION_WINDOW				28
#define UII_EVENT_WINDOW				29
#define UII_HELP_WINDOW					30
#define UII_DUNGEON_KILL_COUNTER		31
#define UII_DUNGEON_MISSION_DESC		32
#define UII_ANIMUS_WINDOW				33
#define UII_ANIMUS_CONTROL_WINDOW		34
#define UII_UNIT_TUNING_WINDOW			35
#define UII_UNIT_STOREHOUSE_WINDOW		36
#define UII_CLASS_CHANGE_WINDOW			37

#define UII_EQUIP_WINDOW				90
#define UII_INVENTORY_PACK				91

#define UII_NUMBER_INPUT_DIALOG			100

//------------------------------------------------------------------------------
// Item Board Type ( IBT_ ) - 서버와 맞춘값들임
// struct _STORAGE_POS { enum { INVEN = 0, EQUIP, EMBELLISH, BELT, FORCE, LINKER }; };
#define MAX_BOARD_TYPE				( 19 )
#define	IBT_INVENTORY				( _STORAGE_POS::INVEN )
#define IBT_EQUIP					( _STORAGE_POS::EQUIP )
#define IBT_ITEM_BELT				( _STORAGE_POS::BELT )
#define IBT_SF_BELT					( _STORAGE_POS::LINKER )
#define IBT_LINK					IBT_SF_BELT
#define IBT_SKILL					( 7 )
#define IBT_FORCE					( _STORAGE_POS::FORCE )
#define IBT_ANIMUS					( _STORAGE_POS::ANIMUS )
#define IBT_ANIMUS_CONTROL			( 8 )
#define IBT_SHOP					( 9 )
#define IBT_PLAYER_TRADE			( 10 )
#define IBT_ORE_PROCESSING			( 11 )
#define IBT_UNIT_TUNING				( 12 )
#define	IBT_UNIT_STOREHOUSE			( 13 )
#define	IBT_ITEM_WORK				( 14 )
#define	IBT_ITEM_UPGRADE			( 15 )
#define	IBT_EXCHANGE				( 16 )
#define	IBT_MINING					( 17 )
#define	IBT_CLASS_CHANGE			( 18 )

//------------------------------------------------------------------------------
// UI Board 관련
#define MAX_INVENTORY_PACK			( 5 )
#define MAX_INVENTORY_SOCKET		( 20 )		// per 1 pack

#define MAX_ITEM_BELT_SOCKET		( 4 )
#define MAX_SF_BELT_SOCKET			( 8 )
#define MAX_ITEM_BULLET_SOCKET		( 2 )

#define MAX_ITEM_QUANTITY			( 99 )		// per 1 item

#define MAX_SOCKET					( 20 )		// per 1 page

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////          Distance          //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define DISTANCE_ITEM_PICKUP		( 20.0f )	// 아이템을 주울 수 있는 거리
#define	DISTANCE_OPEN_STORE			( 30.0f )	// NPC 상점 거래 거리
#define	DISTANCE_MINING				( 3.0f )	// 채굴 거리

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////            ETC             //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Game Start Type ( GST_ )
#define GST_NEW_GAME				mapin_type_start	// ( 0 )	// 게임 시작
#define GST_PORTAL					mapin_type_move		// ( 1 )	// 포탈로 이동
#define GST_REBIRTH					mapin_type_revival	// ( 2 )	// 부활
#define GST_BASE_PORTAL				mapin_type_goto		// ( 3 )	// 맵의 베이스 포탈로 이동
#define GST_TELEPORT				mapin_type_recall 	// ( 4 )	// 플레이어 이름을 입력하여 순간이동
#define GST_DUNGEON					mapin_type_dungeon	// ( 5 )	//던젼..

//------------------------------------------------------------------------------
// Chatting Message Type ( CMT_ )
#define CMT_NORMAL					0
#define CMT_PARTY					1
#define CMT_GUILD					2
#define CMT_NATION					3
#define CMT_RACE					4
#define CMT_WHISPER					5
#define CMT_WHISPER_BY_NAME			6
#define	CMT_WHISPER_BY_GM			7
#define CMT_CHEAT					8
#define CMT_SYSTEM					9

#endif // __DEFINE_H__
