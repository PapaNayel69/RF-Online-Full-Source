////////////////////////////////////////////////////////////////////////////
//
// CNetworkMgr Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __NETWORK_MGR_H__
#define __NETWORK_MGR_H__

#include "./NetworkLib/NetWorking.h"

#include "./Protocol/Protocol.h"
#include "./Protocol/pt_zone_client.h"
#include "./Protocol/pt_login_client.h"
#include "./Protocol/ErrorEventFromServer.h"
#include "./Protocol/AIMessage.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct cnm_create_player
{
	DWORD	dwClientIndex;
	DWORD	dwServerIndex;
	DWORD	dwRecIndex;
	DWORD	dwEquipItemVersion;
};

struct cnm_create_monster
{
	DWORD	dwClientIndex;
	DWORD	dwServerIndex;
	DWORD	dwRecIndex;

	float	fScale;
};

struct cnm_create_animus
{
	DWORD	dwClientIndex;
	DWORD	dwServerIndex;
	DWORD	dwRecIndex;
	BYTE	byLevel;

	float	fScale;
};

struct cnm_create_guardtower
{
	DWORD	dwClientIndex;
	DWORD	dwServerIndex;
	DWORD	dwRecIndex;
};

struct cnm_create_item
{
	WORD	wIndex;
	WORD	wRecIndex;
	BYTE	byEquipPart;
	float	vecPos[3];
};

inline void
ShortPosToFloatPos( float pio_fPos[3], short pi_zPos[3] )
{
	pio_fPos[0] = (float)pi_zPos[0];
	pio_fPos[1] = (float)pi_zPos[1];
	pio_fPos[2] = (float)pi_zPos[2];
}

inline void
FloatPosToShortPos( short pio_zPos[3], float pi_fPos[3] )
{
	pio_zPos[0] = (short)pi_fPos[0];
	pio_zPos[1] = (short)pi_fPos[1];
	pio_zPos[2] = (short)pi_fPos[2];
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define MAX_NETWORK_SOCKET		5					// 로그인, 죤, 로컬챗, 2개는 여분

// Network Server Type ( NST_ )
#define NST_GAME_SERVER			0
#define NST_LOCAL_CHAT			1

// Game Server Type ( GST_ )
#define MAX_GAME_SERVER			3					// 0 = Gate, 1 = World, 2 = Zone
#define GST_NONE				-1
#define GST_GATE				0
#define GST_WORLD				1
#define GST_ZONE				2

//------------------------------------------------------------------------------
// for Master Server

// registration ( == join )
#define USER_REG_NOT_RECIVED				0xFF
#define USER_REG_SUCCESS					RET_CODE_SUCCESS
#define USER_REG_ASYNC_ERROR				RET_CODE_ASYNC_ERR 
#define USER_REG_DB_ERROR					DB_QUERY_ERR
#define USER_REG_ALREADY_EXIST_ACCOUNT		RET_CODE_REGED_ACCOUNT
#define USER_REG_ALREADY_LOGIN				RET_CODE_ALREADY_LOGIN 

// login
#define USER_LOGIN_NOT_RECIVED				0xFF
#define USER_LOGIN_SUCCESS					RET_CODE_SUCCESS
#define USER_LOGIN_ASYNC_ERROR				RET_CODE_ASYNC_ERR
#define USER_LOGIN_DB_ERROR					DB_QUERY_ERR
#define USER_LOGIN_INVALID_ID				RET_CODE_WRONG_USERID
#define USER_LOGIN_INVALID_PASSWORD			RET_CODE_WRONG_PASSWD
#define USER_LOGIN_ALREADY_LOGIN			RET_CODE_ALREADY_LOGIN
#define USER_LOGIN_USER_FULL				RET_CODE_USER_FULL

// request for world server list
#define WORLD_LIST_NOT_RECIVED				0xFF
#define WORLD_LIST_SUCCESS					RET_CODE_SUCCESS

// select world server
#define WORLD_SELECT_NOT_RECIVED			0xFF
#define WORLD_SELECT_SUCCESS				RET_CODE_SUCCESS

// enter the world sercer
#define WORLD_ENTER_NOT_RECIVED				0xFF
#define WORLD_ENTER_SUCCESS					RET_CODE_SUCCESS

// request for disconnect account
#define WORLD_DISCONNECT_ACCOUNT_NOT_RECIVED	0xFF
#define WORLD_DISCONNECT_ACCOUNT_SUCCESS		RET_CODE_SUCCESS

// user info
#define USER_INFO_NOT_RECIVED				0xFF
#define USER_INFO_SUCCESS					RET_CODE_SUCCESS
#define USER_INFO_ASYNC_ERROR				RET_CODE_ASYNC_ERR
#define USER_INFO_DB_ERROR					DB_QUERY_ERR
#define USER_INFO_NOT_LOGIN					RET_CODE_BEFORE_LOGIN
#define USER_INFO_INVALID_INDEX				RET_CODE_FAULT_SERIAL
#define USER_INFO_ALREADY_REGISTER			RET_CODE_ALREADY_REGED

// create character ( == add )
#define CHAR_CREATE_NOT_RECIVED				0xFF
#define CHAR_CREATE_SUCCESS					RET_CODE_SUCCESS
#define CHAR_CREATE_ASYNC_ERROR				RET_CODE_ASYNC_ERR
#define CHAR_CREATE_DB_ERROR				DB_QUERY_ERR
#define CHAR_CREATE_SLOT_ERROR				RET_CODE_SLOT_ERR
#define CHAR_CREATE_NOT_LOGIN				RET_CODE_BEFORE_LOGIN
#define CHAR_CREATE_ALREADY_EXIST_AVATAR	RET_CODE_REGED_AVATOR
#define CHAR_CREATE_INVALID_INDEX			RET_CODE_FAULT_SERIAL

// delete character
#define CHAR_DELETE_NOT_RECIVED				0xFF
#define CHAR_DELETE_SUCCESS					RET_CODE_SUCCESS
#define CHAR_DELETE_ASYNC_ERROR				RET_CODE_ASYNC_ERR
#define CHAR_DELETE_DB_ERROR				DB_QUERY_ERR
#define CHAR_DELETE_NOT_LOGIN				RET_CODE_BEFORE_LOGIN
#define CHAR_DELETE_EMPTY_SLOT				RET_CODE_EMPTY_SLOT
#define CHAR_DELETE_INVALID_INDEX			RET_CODE_FAULT_SERIAL

// select character
#define CHAR_SELECT_NOT_RECIVED				0xFF
#define CHAR_SELECT_SUCCESS					RET_CODE_SUCCESS
#define CHAR_SELECT_ASYNC_ERROR				RET_CODE_ASYNC_ERR
#define CHAR_SELECT_DB_ERROR				DB_QUERY_ERR
#define CHAR_SELECT_NOT_REGISTER			RET_CODE_NOT_REGED
#define CHAR_SELECT_NOT_LOGIN				RET_CODE_BEFORE_LOGIN
#define CHAR_SELECT_ALREADY_SELECT			RET_CODE_ALREADY_SEL
#define CHAR_SELECT_EMPTY_SLOT				RET_CODE_EMPTY_SLOT
#define CHAR_SELECT_INVALID_INDEX			RET_CODE_FAULT_SERIAL

// move out from server
#define MOVEOUT_FROM_SERVER_NOT_RECIVED		0xFF
#define MOVEOUT_FROM_SERVER_SUCCESS			0

// move into server server
#define MOVEINTO_SERVER_NOT_RECIVED			0xFF
#define MOVEINTO_SERVER_SUCCESS				0	

// inventory upload
#define	INVEN_UPLOAD_NOT_RECIVED			0xFF
#define	INVEN_UPLOAD_SUCCESS				0
#define	INVEN_UPLOAD_FAILURE				1

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class	CObject;
class	CCharacter;
class	CPlayer;
class	CMonster;
class	CAnimus;
class	CGuardTower;
class	CNpc;
class	CBoardItem;
class	CEquipItem;
class	CGroundItem;

struct	BUILD_GUARD_TOWER;
struct	SHOP_TRADE_INFO;

class	CNetMsgProcessor_Base;

//------------------------------------------------------------------------------

#define MAX_WORLD_SERVER ( 100 )
struct WORLD_SERVER_INFO
{
	BOOL m_bIsOpen;			// 월드 서버가 열려있는가?
	char m_pName[64];		// 월드 서버명
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

enum NET_MSG_PROCESSOR
{
	NMP_COMBAT,
	NMP_ITEM, NMP_ITEM_ARRANGE,
	NMP_PARTY, NMP_MAP, NMP_DUNGEON,
	NMP_SUMMON, NMP_UNIT,
	NMP_ITEM_TRADE, NMP_ITEM_TRADE_PLAYER,
	NMP_RESOURCE,
	NMP_EVENT,
	MAX_NET_MSG_PROCESSOR
};

class CNetworkMgr : public CNetWorking
{
// < Data Member > -------------------------------------------------------------
protected :

	CNetMsgProcessor_Base *	m_pNetMsgProcessor[MAX_NET_MSG_PROCESSOR];

	float		m_fServerVersion;
	BYTE		m_byAvatarGrade;							// 아바타의 등급

	DWORD		m_dwServerIP[MAX_GAME_SERVER];				// Server IP [ 0 = Master, 1 = Zone ]
	DWORD		m_dwServerPort[MAX_GAME_SERVER];			// Server Port No
	char		m_pServerName[MAX_GAME_SERVER][32];			// Server Name
	char		m_nActiveServerID;							// Server ID

	BOOL		m_bIsConnectedToServer[MAX_GAME_SERVER];	// Server 연결에 성공했는가?

	DWORD		m_dwAccountIndex;							// 유저 계정의 인덱스 ( = server serial )
	DWORD		m_dwAvatarIndex;							// 유저의 월드 서버에서의 인덱스

	DWORD		m_dwWorldServerKey[KEY_NUM];
	DWORD		m_dwZoneServerKey;

	BYTE		m_byWorldServerNum;
	WORLD_SERVER_INFO	m_pWorldServerList[MAX_WORLD_SERVER];

	//--------------------------------------------------------------------------

	char		m_pPassword[32];							// 사용자의 패스워드
	char		m_pID[32];									// 사용자의 ID

	//--------------------------------------------------------------------------

	BYTE		m_byResultOfUserRegistration;				// 사용자 등록 결과
	BYTE		m_byResultOfLogin;							// 로그인 결과
	BYTE		m_byResultOfRequestForWorldServerList;		// 월드 서버 리스트 요청 결과
	BYTE		m_byResultOfSelectWorldServer;				// 월드 서버 선택 결과
	BYTE		m_byResultOfEnterTheWorldServer;			// 월드 서버 접속 결과
	BYTE		m_byResultOfRequestForDisconnectAccount;	// 월드 서버에 접속한 기존 계정의 디스커넥트를 요청 결과

	BYTE		m_byResultOfUserInfo;						// 사용자 정보 요청 결과
	BYTE		m_byResultOfCharacterCreate;				// 캐릭터 생성 요청 결과
	BYTE		m_byResultOfCharacterSelect;				// 캐릭터 선택 요청 결과
	BYTE		m_byResultOfCharacterDelete;				// 캐릭터 삭제 요청 결과
	BYTE		m_byResultOfMoveOutFromServer;				// 현재 커넥트된 서버에서 나감
	BYTE		m_byResultOfMoveIntoServer;					// 현재 커넥트된 서버로 진입
	BYTE		m_byResultOfInventoryUpload;				// 인벤토리 업로드에 대한 결과

	//--------------------------------------------------------------------------

	/*
	BOOL		m_bIsSendMsg;								// 서버에 메세지를 보냈는가? ( 현재는 로그인에서만 사용되고 있음. FALSE일때만 메세지를 보낼 수 있음 )
	DWORD		m_dwSendMsgTime;							// 서버에 메세지를 보낸시간 ( 현재는 로그인에서만 사용되고 있음. )

	//--------------------------------------------------------------------------

	BOOL		m_bIsRequestClose;							// 서버의 Close를 요청했는가?
	BOOL		m_bIsRequestUseSF;							// 스킬/포스 사용을 요청했는가?
	BOOL		m_bIsRequestChangeMode;						// 변경을 요청했는가?*/

	BOOL		m_bIsSendMsg;								// 서버에 메세지를 보냈는가?
	DWORD		m_dwSendMsgTime;							// 서버에 메세지를 보낸시간
	BOOL		m_bIsRequestClose;							// 서버의 Close를 요청했는가?

	//--------------------------------------------------------------------------

	DWORD		m_dwLastestAvatarIndex;						// 가장 최근에 접속했었던 아바타의 인덱스 ( 마스터 서버상의 인덱스 번호 )
	float		m_vecAvatarStartPos[3];						// 게임 시작시의 아바타의 시작 지점
	DWORD		m_dwItemIndexForFirstItemSetting;			// 인벤토리나 벨트, 포스 등의 인덱스 카운팅용으로 쓰임 ( 프로토콜처리시에만 임시로 쓰임 )

	//--------------------------------------------------------------------------

public :
	CLogFile	m_ClientLogFile;

// < Member Function > ---------------------------------------------------------
public :
	CNetworkMgr();
	~CNetworkMgr();

	virtual void	Init( void );
	virtual BOOL	Create( void );
	virtual BOOL	Destroy ( void );

	//--------------------------------------------------------------------------

			void	FrameMove( void );

			void	Connect( BYTE pi_byServerType );
			void	Close( BYTE pi_byServerType );

			//void	SendChatMessage_PlayerList( void );
			//void	SendChatMessage_Normal( char * l_pChatMessage );
			//void	SendChatMessage_ManToMan( char * l_pChatMessage );

			void	SettingPlayerRebirth( CPlayer * l_pPlayer );

	//--------------------------------------------------------------------------

			BOOL	SendNetMessage( DWORD pi_dwClientIndex, BYTE * pi_pType, void * pi_pMsg, int pi_nSize );
			void	NetLogWrite( const char * pi_pFormat, ... );

protected :

	virtual	bool	DataAnalysis( DWORD pi_dwProID, DWORD pi_dwClientIndex, _MSG_HEADER* pMsgHeader, char* pMsg );
			bool	AnalysisServerPacket( _MSG_HEADER* pMsgHeader, char* pMsg );
			bool	AnalysisLocalChatServerPacket( _MSG_HEADER* pMsgHeader, char* pMsg );
	virtual void	AcceptClientCheck( DWORD dwProID, DWORD dwIndex, DWORD dwSerial );
	virtual void	CloseClientCheck( DWORD	dwProID, DWORD dwIndex, DWORD dwSerial );

	////////////////////////////////////////////////////////////////////////////
	////////////////////////        Protocol       /////////////////////////////
	////////////////////////////////////////////////////////////////////////////

	//--------------------------------------------------------------------------
	// Account Message
	void	AccountMsg_JoinAccountResult_locl( char * pi_pMsg );
	void	AccountMsg_LoginAccountResult_locl( char * pi_pMsg );
	void	AccountMsg_WorldListResult_locl( char * pi_pMsg );
	void	AccountMsg_SelectWorldResult_locl( char * pi_pMsg );
	void	AccountMsg_PushCloseResult_locl( char * pi_pMsg );
	void	AccountMsg_ServerNotifyInform_locl( char * pi_pMsg );

	//--------------------------------------------------------------------------
	// System Message
	void	SystemMsg_EnterWorldResult_zone( char * pi_pMsg );
	void	SystemMsg_MoveinUserResult_zone( char * pi_pMsg );
	void	SystemMsg_MoveoutUserResult_zone( char * pi_pMsg );
	void	SystemMsg_RegedCharResult_zone( char * pi_pMsg );
	void	SystemMsg_AddCharResult_zone( char * pi_pMsg );
	void	SystemMsg_DelCharResult_zone( char * pi_pMsg );
	void	SystemMsg_SelCharResult_zone( char * pi_pMsg );
	void	SystemMsg_ServerNotifyInform_zone( char * pi_pMsg );

	//--------------------------------------------------------------------------
	// Initialize Message
	void	InitMsg_BaseDownloadResult_zocl( char * pi_pMsg );
	void	InitMsg_InvenDownloadResult_zocl( char * pi_pMsg );
	void	InitMsg_ExitSaveDataResult_zocl( char * pi_pMsg );
	void	InitMsg_StatInform_zocl( char * pi_pMsg );
	void	InitMsg_FcitemInform_zocl( char * pi_pMsg );
	void	InitMsg_CumDownloadResult_zocl( char * pi_pMsg );
	void	InitMsg_ForceDownloadResult_zocl( char * pi_pMsg );
	void	InitMsg_UnitDownloadResult_zocl( char * pi_pMsg );
	void	InitMsg_AnimusDownloadResult_zocl( char * pi_pMsg );
	void	InitMsg_NewPosStartResult_zocl( char * pi_pMsg );

	void	InitMsg_MonsterCreate_zocl( char * pi_pMsg );
	void	InitMsg_RecallMonCreate_zocl( char * pi_pMsg );
	void	InitMsg_AnimusCreate_zocl( char * pi_pMsg );
	void	InitMsg_TowerCreate_zocl( char * pi_pMsg );
	void	InitMsg_ItemboxCreate_zocl( char * pi_pMsg );
	void	InitMsg_ParkingUnitCreate_zocl( char * pi_pMsg );

	void	InitMsg_PlayerDestroy_zocl( char * pi_pMsg );
	void	InitMsg_PlayerDie_zocl( char * pi_pMsg );
	void	InitMsg_MonsterDestroy_zocl( char * pi_pMsg );
	void	InitMsg_AnimusDestroy_zocl( char * pi_pMsg );
	void	InitMsg_TowerDestroy_zocl( char * pi_pMsg );
	void	InitMsg_ItemboxDestroy_zocl( char * pi_pMsg );
	void	InitMsg_ParkingUnitDestroy_zocl( char * pi_pMsg );

	void	InitMsg_OtherShapeAll_zocl( char * pi_pMsg );
	void	InitMsg_OtherShapePart_zocl( char * pi_pMsg );
	void	InitMsg_OtherShapeFailure_zocl( char * pi_pMsg );
	void	InitMsg_OtherShapeChange_zocl( char * pi_pMsg );
	void	InitMsg_OtherUnitRideChange_zocl( char * pi_pMsg );

	void	InitMsg_OtherNewView_zocl( char * pi_pMsg );
	void	InitMsg_PlayerRevival_zocl( char * pi_pMsg );
	void	InitMsg_OtherMapOut_zocl( char * pi_pMsg );
	void	InitMsg_PlayerResurrect_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------
	// Position Message
	void	PositionMsg_MoveErrorResult_zocl( char * pi_pMsg );
	void	PositionMsg_PlayerMove_zocl( char * pi_pMsg );
	void	PositionMsg_MonsterMove_zocl( char * pi_pMsg );
	void	PositionMsg_AnimusMove_zocl( char * pi_pMsg );
	void	PositionMsg_MonsterStop_zocl( char * pi_pMsg );
	void	PositionMsg_MonsterChase_zocl( char * pi_pMsg );

	void	PositionMsg_PlayerFixPosition_zocl( char * pi_pMsg );
	void	PositionMsg_MonsterFixPosition_zocl( char * pi_pMsg );
	void	PositionMsg_AnimusFixpositon_zocl( char * pi_pMsg );
	void	PositionMsg_TowerFixpositon_zocl( char * pi_pMsg );
	void	PositionMsg_ItemboxFixPosition_zocl( char * pi_pMsg );
	void	PositionMsg_ParkingunitFixpositon_zocl( char * pi_pMsg );

	void	PositionMsg_PlayerStop_zocl( char * pi_pMsg );
	void	PositionMsg_PlayerRealMove_zocl( char * pi_pMsg );
	void	PositionMsg_MonsterRealMove_zocl( char * pi_pMsg );
	void	PositionMsg_AnimusRealMove_zocl( char * pi_pMsg );
	void	PositionMsg_StateInform_zocl( char * pi_pMsg );
	void	PositionMsg_GotoBaseportalResult_zocl( char * pi_pMsg );
	void	PositionMsg_GotoAvatorResult_zocl( char * pi_pMsg );
	void	PositionMsg_BreakStopResult_zocl( char * pi_pMsg );
	void	PositionMsg_ObjectRealFixpositon_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------
	// Fight Message
	
	//--------------------------------------------------------------------------
	// Magic Message

	//--------------------------------------------------------------------------
	// Item Message
	void	ItemMsg_ItemboxStateChange_zocl( char * pi_pMsg );
	void	ItemMsg_ItemboxTakeNewResult_zocl( char * pi_pMsg );
	void	ItemMsg_ItemboxTakeAddResult_zocl( char * pi_pMsg );
	void	ItemMsg_ThrowStorageResult_zocl( char * pi_pMsg );
	void	ItemMsg_ThrowEquipResult_zocl( char * pi_pMsg );
	void	ItemMsg_UsePotionResult_zocl( char * pi_pMsg );
	void	ItemMsg_EquipPartResult_zocl( char * pi_pMsg );
	void	ItemMsg_EmbellishResult_zocl( char * pi_pMsg );
	void	ItemMsg_OffPartResult_zocl( char * pi_pMsg );
	void	ItemMsg_MakeItemResult_zocl( char * pi_pMsg );
	void	ItemMsg_UpgradeItemResult_zocl( char * pi_pMsg );
	void	ItemMsg_DowngradeItemResult_zocl( char * pi_pMsg );
	void	ItemMsg_AddBagResult_zocl( char * pi_pMsg );
	void	ItemMsg_DeleteStorageInform_zocl( char * pi_pMsg );
	void	ItemMsg_StorageRefreshInform_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------
	// Observer Message
	void	ObserverMsg_MoveResult_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------
	// Event Message
	void	EventMsg_LevelUp_zocl( char * pi_pMsg );
	void	EventMsg_Recover_zocl( char * pi_pMsg );
	void	EventMsg_MaxHfsp_zocl( char * pi_pMsg );
	void	EventMsg_ExpAlter_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------
	// Item Trade Message
	//void	ItemTradeMsg_UsedStorageResult_zocl( char * pi_pMsg );
	void	ItemTradeMsg_BuyStoreSuccess_zocl( char * pi_pMsg );
	void	ItemTradeMsg_BuyStoreFailure_zocl( char * pi_pMsg );
	void	ItemTradeMsg_SellStoreResult_zocl( char * pi_pMsg );
	void	ItemTradeMsg_StoreListResult_zocl( char * pi_pMsg );
	void	ItemTradeMsg_RepairResult_zocl( char * pi_pMsg );
	void	ItemTradeMsg_ExchangeMoneyResult_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------
	// Item Trade Message - Player
	void	ItemTradeMsg_Player_TradeAskResult_zocl( char * pi_pMsg );
	void	ItemTradeMsg_Player_TradeAskInform_zocl( char * pi_pMsg );
	void	ItemTradeMsg_Player_TradeAnswerResult_zocl( char * pi_pMsg );
	void	ItemTradeMsg_Player_TradeStartInform_zocl( char * pi_pMsg );
	void	ItemTradeMsg_Player_TradeCancleResult_zocl( char * pi_pMsg );
	void	ItemTradeMsg_Player_TradeCancleInform_zocl( char * pi_pMsg );
	void	ItemTradeMsg_Player_TradeLockResult_zocl( char * pi_pMsg );
	void	ItemTradeMsg_Player_TradeLockInform_zocl( char * pi_pMsg );
	void	ItemTradeMsg_Player_TradeAddResult_zocl( char * pi_pMsg );
	void	ItemTradeMsg_Player_TradeAddInform_zocl( char * pi_pMsg );
	void	ItemTradeMsg_Player_TradeDelResult_zocl( char * pi_pMsg );
	void	ItemTradeMsg_Player_TradeDelInform_zocl( char * pi_pMsg );
	void	ItemTradeMsg_Player_TradeBetResult_zocl( char * pi_pMsg );
	void	ItemTradeMsg_Player_TradeBetInform_zocl( char * pi_pMsg );
	void	ItemTradeMsg_Player_TradeOkResult_zocl( char * pi_pMsg );
	void	ItemTradeMsg_Player_TradeOkInform_zocl( char * pi_pMsg );
	void	ItemTradeMsg_Player_TradeAccomplishInform_zocl( char * pi_pMsg );
	void	ItemTradeMsg_Player_TradeCloseInform_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------
	// UI Message
	void	UIMsg_SFLinkChangeResult_zocl( char * pi_pMsg );
	void	UIMsg_ForceInvenChangeResult_zocl( char * pi_pMsg );
	void	UIMsg_ResDivisionResult_zocl( char * pi_pMsg );
	void	UIMsg_ResSeparationResult_zocl( char * pi_pMsg );
	void	UIMsg_PotionsocketChangeResult_zocl( char * pi_pMsg );
	void	UIMsg_PotionsocketSeparationResult_zocl( char * pi_pMsg );
	void	UIMsg_PotionsocketDivisionResult_zocl( char * pi_pMsg );
	void	UIMsg_UnitGetOnResult_zocl( char * pi_pMsg );
	void	UIMsg_UnitGetOnOther_zocl( char * pi_pMsg );
	void	UIMsg_UnitGetOffResult_zocl( char * pi_pMsg );
	void	UIMsg_UnitGetOffOther_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------
	// Resource Message
	void	ResMsg_MineStartResult_zocl( char * pi_pMsg );
	void	ResMsg_MineCancleResult_zocl( char * pi_pMsg );
	void	ResMsg_MineCompleteResult_zocl( char * pi_pMsg );
	void	ResMsg_MineStartResultOther_zocl( char * pi_pMsg );
	void	ResMsg_MineEndResultOther_zocl( char * pi_pMsg );
	void	ResMsg_OreCuttingResult_zocl( char * pi_pMsg );
	void	ResMsg_OreIntoBagResult_zocl( char * pi_pMsg );
	void	ResMsg_CuttingCompleteResult_zocl( char * pi_pMsg );

	/*
	//--------------------------------------------------------------------------
	// Party Message
	void	PartyMsg_PartyJoinInvitationQuestion_zocl( char * pi_pMsg );
	void	PartyMsg_PartyJoinApplicationQuestion_zocl( char * pi_pMsg );
	void	PartyMsg_PartyJoinJoinerResult_zocl( char * pi_pMsg );
	void	PartyMsg_PartyJoinMemberResult_zocl( char * pi_pMsg );
	void	PartyMsg_PartyLeaveSelfResult_zocl( char * pi_pMsg );
	void	PartyMsg_PartyLeaveCompulsionResult_zocl( char * pi_pMsg );
	void	PartyMsg_PartyDisjointResult_zocl( char * pi_pMsg );
	void	PartyMsg_PartySuccessionResult_zocl( char * pi_pMsg );
	void	PartyMsg_PartyLockResult_zocl( char * pi_pMsg );
	void	PartyMsg_PartyExpDivision_zocl( char * pi_pMsg );
	*/

	//--------------------------------------------------------------------------
	// Effect Message
	void	EffectMsg_ForceResultOne_zocl( char * pi_pMsg );
	void	EffectMsg_ForceResultOther_zocl( char * pi_pMsg );
	void	EffectMsg_SkillResultOne_zocl( char * pi_pMsg );
	void	EffectMsg_SkillResultOther_zocl( char * pi_pMsg );
	void	EffectMsg_ClassSkillResultOne_zocl( char * pi_pMsg );
	void	EffectMsg_ClassSkillResultOther_zocl( char * pi_pMsg );

	void	EffectMsg_EffectAddInform_zocl( char * pi_pMsg );
	void	EffectMsg_EffectRemoveInform_zocl( char * pi_pMsg );
	void	EffectMsg_SetFpInform_zocl( char * pi_pMsg );
	void	EffectMsg_SetHpInform_zocl( char * pi_pMsg );
	void	EffectMsg_SetSpInform_zocl( char * pi_pMsg );
	void	EffectMsg_InsertItemInform_zocl( char * pi_pMsg );
	void	EffectMsg_StunInform_zocl( char * pi_pMsg );
	void	EffectMsg_LastEffectChangeInform_zocl( char * pi_pMsg );

	void	EffectMsg_MakeTowerResult_zocl( char * pi_pMsg );
	void	EffectMsg_BackTowerResult_zocl( char * pi_pMsg );
	void	EffectMsg_AlterTowerHp_zocl( char * pi_pMsg );
	void	EffectMsg_TowerCompleteInform_zocl( char * pi_pMsg );
	void	EffectMsg_AlterContEffectTime_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------
	// Chatting Message
	//void	ChatMsg_ChatMessageReceipt_zone( char * pi_pMsg );
	void	ChatMsg_ChatFarFailure_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------
	// Sync Message
	void	SyncMsg_ObjectServerPosResult_zocl( char * pi_pMsg );
	void	SyncMsg_AdjustAmountInform_zocl( char * pi_pMsg );

	//--------------------------------------------------------------------------
	// ETC
public :

	CPlayer  *		CreatePlayer( cnm_create_player * pi_pRecv, BOOL pi_bRequestExterior = TRUE );
	CMonster *		CreateMonster( cnm_create_monster * pi_pRecv );
	CAnimus *		CreateAnimus( cnm_create_animus * pi_pRecv );
	CGuardTower *	CreateGuardTower( cnm_create_guardtower * pi_pRecv );
	void			SetPlayerItem( CPlayer * pi_pPlayer, WORD * pi_pItemIndex, WORD pi_wFaceIndex );
	CGroundItem *	CreateGroundItem( cnm_create_item * pi_pRecv );

public :

	////////////////////////////////////////////////////////////////////////////
	////////////////////////        Protocol       /////////////////////////////
	////////////////////////////////////////////////////////////////////////////

			BOOL	AccountMsg_JoinAccountRequest_cllo( void );
			BOOL	AccountMsg_LoginAccountRequest_cllo( void );
			BOOL	AccountMsg_WorldListRequest_cllo( void );
			BOOL	AccountMsg_SelectWorldRequest_cllo( DWORD pi_dwIndex );
			BOOL	AccountMsg_PushCloseRequest_cllo( void );

			//------------------------------------------------------------------

			BOOL	SystemMsg_EnterWorldRequest_zone( void );
			BOOL	SystemMsg_MoveinUserRequest_zone( void );
			BOOL	SystemMsg_MoveoutUserRequest_zone( BYTE pi_byMoveoutType );
			BOOL	SystemMsg_RegedCharRequest_zone( void );
			BOOL	SystemMsg_AddCharRequest_zone( BYTE pi_byAvatarIndex );
			BOOL	SystemMsg_DelCharRequest_zone( BYTE pi_byAvatarIndex );
			BOOL	SystemMsg_SelCharRequest_zone( BYTE pi_byAvatarIndex );

			//------------------------------------------------------------------

			BOOL	InitMsg_BaseDownloadRequest_clzo( void );
			BOOL	InitMsg_InvenDownloadRequest_clzo( void );
			BOOL	InitMsg_ExitSaveDataRequest_clzo( void );
			BOOL	InitMsg_CumDownloadRequest_clzo( void );
			BOOL	InitMsg_ForceDownloadRequest_clzo( void );
			BOOL	InitMsg_SpecialDownloadRequest_clzo( void );
			BOOL	InitMsg_NewPosStartRequest_clzo( BYTE pi_byStartMode );

			BOOL	InitMsg_OtherShapeRequest_clzo( CPlayer * pi_pPlayer, BOOL pi_bIsRequestAllPart );
			BOOL	InitMsg_PlayerRevivalRequest_clzo( void );

			//------------------------------------------------------------------

			BOOL	PositionMsg_MoveRequest_clzo( float * pi_vecTargetPos, BOOL pi_bMoveByUserInput, BOOL pi_bLimitMinDistance = FALSE, float pi_fChaseDistance = 0.0f );
			BOOL	PositionMsg_MoveRequest_Simple_clzo( float * pi_vecTargetPos );
			BOOL	PositionMsg_RealMovposRequest_clzo( void );
			BOOL	PositionMsg_PlayerStop_clzo( BOOL pi_bSetAction = TRUE );
			BOOL	PositionMsg_GotoBaseportalRequest_clzo( DWORD pi_dwIndex );
			BOOL	PositionMsg_GotoAvatorRequest_clzo( char * pi_pPlayerName );
			BOOL	PositionMsg_MoveTypeChangeRequest_clzo( BYTE pi_byMoveMode );

			//------------------------------------------------------------------

			
			//------------------------------------------------------------------

			BOOL	ItemMsg_ItemboxTakeRequest_clzo( DWORD pi_dwGroundItemIndex, DWORD pi_dwInventoryItemIndex );
			BOOL	ItemMsg_ThrowStorageRequest_clzo( BYTE pi_byWindowType, DWORD pi_dwItemIndex, BYTE pi_byQuantity );
			BOOL	ItemMsg_UsePotionRequest_clzo( BYTE pi_byWindowType, CBoardItem * pi_pItem );
			BOOL	ItemMsg_EquipPartRequest_clzo( DWORD pi_dwItemIndex );
			BOOL	ItemMsg_Embellish_Request_Clzo( BYTE pi_byTarBoardType, DWORD pi_dwTarItemIndex, DWORD pi_dwOrgItemIndex );
			BOOL	ItemMsg_OffPartRequest_clzo( CBoardItem * pi_pItem );
			BOOL	ItemMsg_MakeItemRequest_clzo( void );
			BOOL	ItemMsg_UpgradeItemRequest_clzo( void );
			BOOL	ItemMsg_DowngradeItemRequest_clzo( void );
			BOOL	ItemMsg_AddBagRequest_clzo( DWORD pi_dwPackItemIndex );

			//------------------------------------------------------------------

			BOOL	ObserverMsg_MoveRequest_clzo( float * pi_vecTargetPos );

			//------------------------------------------------------------------

			BOOL	ItemTradeMsg_UsedStorageRequest_clzo( BYTE pi_byStoreIndex, DWORD pi_dwItemListVersion, BYTE pi_byPackNo );
			BOOL	ItemTradeMsg_BuyStoreRequest_clzo( BYTE pi_byMerchantTypeID, SHOP_TRADE_INFO * pi_pTradeInfo );
			BOOL	ItemTradeMsg_SellStoreRequest_clzo( SHOP_TRADE_INFO * pi_pTradeInfo );
			BOOL	ItemTradeMsg_StoreListRequest_clzo( void );
			BOOL	ItemTradeMsg_RepairRequest_clzo( SHOP_TRADE_INFO * pi_pTradeInfo );
			BOOL	ItemTradeMsg_ExchangeDalantForGoldRequest_clzo( DWORD pi_dwDalant );
			BOOL	ItemTradeMsg_ExchangeGoldForDalantRequest_clzo( DWORD pi_dwGold );

			//------------------------------------------------------------------

			BOOL	ItemTradeMsg_Player_TradeAskRequest_clzo( DWORD pi_dwServerIndex );
			BOOL	ItemTradeMsg_Player_TradeAnswerRequest_clzo( void );
			BOOL	ItemTradeMsg_Player_TradeCancleRequest_clzo( void );
			BOOL	ItemTradeMsg_Player_TradeLockRequest_clzo( void );
			BOOL	ItemTradeMsg_Player_TradeAddRequest_clzo( void );
			BOOL	ItemTradeMsg_Player_TradeDelRequest_clzo( void );
			BOOL	ItemTradeMsg_Player_TradeBetRequest_clzo( BYTE pi_byMoneyType, DWORD pi_dwAmount );
			BOOL	ItemTradeMsg_Player_TradeOkRequest_clzo( void );

			//------------------------------------------------------------------

			BOOL	UIMsg_SFLinkChangeRequest_clzo( char pi_bySrcBoardType, DWORD pi_dwSrcSFIndex,
													char pi_byTarBoardType, DWORD pi_dwTarSFIndex );
			BOOL	UIMsg_ForceInvenChangeRequest_clzo( DWORD pi_dwSrcIndex, BOOL pi_bIsInvenToForceWindow );
			//BOOL	UIMsg_InvenIndexLineupRequest_clzo( void );
			//BOOL	UIMsg_BeltIndexLineupRequest_clzo( void );
			//BOOL	UIMsg_LinkerIndexLineupRequest_clzo( void );
			BOOL	UIMsg_ResSeparationRequest_clzo( DWORD pi_dwItemIndex, BYTE pi_byItemNum );
			BOOL	UIMsg_ResDivisionRequest_clzo( DWORD pi_dwOrgItemIndex, BYTE pi_byQuantity, DWORD pi_dwTarItemIndex );
			BOOL	UIMsg_PotionsocketChangeRequest_clzo( BYTE pi_byOrgBoardType, BYTE pi_byTarBoardType, DWORD pi_dwOrgItemIndex, DWORD pi_dwTarItemIndex );
			BOOL	UIMsg_PotionsocketSeparationRequest_clzo( BYTE pi_bySrcWindow, BYTE pi_byTarWindow,
															  DWORD pi_dwItemIndex, BYTE pi_byQuantity );
			BOOL	UIMsg_PotionsocketDivisionRequest_clzo( BYTE pi_bySrcWindow, BYTE pi_byTarWindow,
															DWORD pi_dwSrcItemIndex, DWORD pi_dwTarItemIndex,
															BYTE pi_byQuantity );
			BOOL	UIMsg_UnitGetOnRequest_clzo( DWORD pi_dwItemIndex );
			BOOL	UIMsg_UnitGetOffRequest_clzo( void );

			//------------------------------------------------------------------

			BOOL	ResMsg_MineStartRequest_clzo( void );
			BOOL	ResMsg_MineCancleRequest_clzo( void );
			BOOL	ResMsg_MineCompleteRequest_clzo( void );
			BOOL	ResMsg_OreCuttingRequest_clzo( void );
			BOOL	ResMsg_OreIntoBagRequest_clzo( BYTE pi_byOreDTIndex, BYTE pi_byOreNum, DWORD pi_dwInvenOreSerial );
			BOOL	ResMsg_CuttingCompleteRequest_clzo( void );

			//------------------------------------------------------------------

			/*
			//BOOL	PartyMsg_PartyJoinInvitation_clzo( void );
			BOOL	PartyMsg_PartyJoinInvitation_clzo( CPlayer * pi_pPlayer );
			//BOOL	PartyMsg_PartyJoinInvitationAnswer_clzo( void );
			BOOL	PartyMsg_PartyJoinInvitationAnswer_clzo( CPlayer * pi_pPlayer );
			//BOOL	PartyMsg_PartyJoinApplication_clzo( void );
			BOOL	PartyMsg_PartyJoinApplication_clzo( CPlayer * pi_pPlayer );
			//BOOL	PartyMsg_PartyJoinApplicationAnswer_clzo( void );
			BOOL	PartyMsg_PartyJoinApplicationAnswer_clzo( CPlayer * pi_pPlayer );
			BOOL	PartyMsg_PartyLeaveSelfRequest_clzo( void );
			BOOL	PartyMsg_PartyLeaveCompulsionRequest_clzo( DWORD pi_dwIndex );
			BOOL	PartyMsg_PartyDisjointRequest_clzo( void );
			BOOL	PartyMsg_PartySuccessionRequest_clzo( DWORD pi_dwLeaderIndex );
			BOOL	PartyMsg_PartyLockRequest_clzo( BOOL pi_bIsLock );
			*/

			//------------------------------------------------------------------

			BOOL	EffectMsg_ForceRequest_clzo( CCharacter * pi_pChar );
			BOOL	EffectMsg_SkillRequest_clzo( CCharacter * pi_pChar );
			BOOL	EffectMsg_ClassSkillRequest_clzo( CCharacter * pi_pChar );

			BOOL	EffectMsg_MakeTowerRequest_clzo( BUILD_GUARD_TOWER & pi_stGuardTowerInfo );
			BOOL	EffectMsg_BackTowerRequest_clzo( void );

			//------------------------------------------------------------------

			BOOL	ChatMsg_ChatOperatorRequest_clzo( char * pi_pChatMessage );
			BOOL	ChatMsg_ChatCircleRequest_clzo( char * pi_pChatMessage );
			BOOL	ChatMsg_ChatFarRequest_clzo( char * pi_pPlayerName, char * pi_pChatMessage );
			BOOL	ChatMsg_ChatPartyRequest_clzo( char * pi_pChatMessage );
			BOOL	ChatMsg_ChatRaceRequest_clzo( char * pi_pChatMessage );
			BOOL	ChatMsg_ChatCheatRequest_clzo( char * pi_pChatMessage );
			BOOL	ChatMsg_ChatMgrWhisperRequest_clzo( char * pi_pChatMessage );

			//------------------------------------------------------------------

			BOOL	SecureMsg_ResponFreeRequest( void );

			//------------------------------------------------------------------

			BOOL	SyncMsg_ObjectServerPosRequest_clzo( CCharacter * pi_pCharacter );

	////////////////////////////////////////////////////////////////////////////
	////////////////////////          UDP          /////////////////////////////
	////////////////////////////////////////////////////////////////////////////

			/*
			void	PartyMsg_PartyMemberInfo_upd( char * pi_pMsg );

			void	PartyMsg_PartyMemberHp_upd( char * pi_pMsg );
			void	PartyMsg_PartyMemberFp_upd( char * pi_pMsg );
			void	PartyMsg_PartyMemberSp_upd( char * pi_pMsg );
			void	PartyMsg_PartyMemberLv_upd( char * pi_pMsg );
			void	PartyMsg_PartyMemberPos_upd( char * pi_pMsg );
			void	PartyMsg_PartyMemberMaxHfsp_upd( char * pi_pMsg );
			void	PartyMsg_PartyMemberEffect_upd( char * pi_pMsg );
			*/

			//------------------------------------------------------------------

			void	ChatMsg_ChatMessageReceipt_udp( char * pi_pMsg );
			void	ChatMsg_AnnounMessageReceipt_udp( char * pi_pMsg );

	////////////////////////////////////////////////////////////////////////////
	////////////////////////          ETC          /////////////////////////////
	////////////////////////////////////////////////////////////////////////////

			CNetMsgProcessor_Base *	GetNetMsgProcessor( NET_MSG_PROCESSOR pi_nmtType );

	inline	float	GetVersion( void )									{ return m_fServerVersion; }
	inline	BYTE	GetAvatarGrade( void )								{ return m_byAvatarGrade; }
	inline	BOOL	IsEqualAvatarGrade( BYTE pi_byGrade )				{ return ( m_byAvatarGrade == pi_byGrade ); }

			char	GetActiveServerID( void )							{ return m_nActiveServerID; }
	inline	BOOL	IsConnectedToServer( BYTE pi_byServerType )			{ return m_bIsConnectedToServer[pi_byServerType]; }

	inline	void	SetAccountIndex( DWORD pi_dwIndex )					{ m_dwAccountIndex = pi_dwIndex; }
	inline	DWORD	GetAccountIndex( void )								{ return m_dwAccountIndex; }
	inline	void	SetAvatarIndex( DWORD pi_dwIndex )					{ m_dwAvatarIndex = pi_dwIndex; }
	inline	DWORD	GetAvatarIndex( void )								{ return m_dwAvatarIndex; }

	inline	void	SetWorldServerKey( BYTE pi_byOrderNo, DWORD pi_dwKey )	{ m_dwWorldServerKey[pi_byOrderNo] = pi_dwKey; }
	inline	DWORD	GetWorldServerKey( BYTE pi_byOrderNo )				{ return m_dwWorldServerKey[pi_byOrderNo]; }
	inline	void	SetWorldServerKey( DWORD * pi_pKey )				{ memcpy( m_dwWorldServerKey, pi_pKey, sizeof( DWORD ) * KEY_NUM ); }
	inline	DWORD *	GetWorldServerKey( void )							{ return m_dwWorldServerKey; }

	inline	void	SetZoneServerKey( DWORD pi_dwKey )					{ m_dwZoneServerKey = pi_dwKey; }
	inline	DWORD	GetZoneServerKey( void )							{ return m_dwZoneServerKey; }

	inline	BYTE	GetWorldServerNum( void )							{ return m_byWorldServerNum; }
	inline	WORLD_SERVER_INFO *	GetWorldServerInfoList( void )			{ return m_pWorldServerList; }

	//--------------------------------------------------------------------------

			void	SetID( char * pi_pID );
	inline	char *	GetID( void )										{ return m_pID; }
			void	SetPassword( char * pi_pPassword );
	inline	char *	GetPassword( void )									{ return m_pPassword; }

	//--------------------------------------------------------------------------

	inline	void	SetResultOfUserRegistration( BOOL pi_bFlag )		{ m_byResultOfUserRegistration = pi_bFlag; }
	inline	void	SetResultOfLogin( BOOL pi_bFlag )					{ m_byResultOfLogin = pi_bFlag; }
	inline	void	SetResultOfRequestForWorldServerList( BOOL pi_bFlag ){ m_byResultOfRequestForWorldServerList = pi_bFlag; }
	inline	void	SetResultOfSelectWorldServer( BOOL pi_bFlag )		{ m_byResultOfSelectWorldServer = pi_bFlag; }
	inline	void	SetResultOfEnterTheWorldServer( BOOL pi_bFlag )		{ m_byResultOfEnterTheWorldServer = pi_bFlag; }
	inline	void	SetResultOfRequestForDisconnectAccount( BOOL pi_bFlag ){ m_byResultOfRequestForDisconnectAccount = pi_bFlag; }
	inline	void	SetResultOfUserInfo( BOOL pi_bFlag )				{ m_byResultOfUserInfo = pi_bFlag; }
	inline	void	SetResultOfCharacterCreate( BOOL pi_bFlag )			{ m_byResultOfCharacterCreate = pi_bFlag; }
	inline	void	SetResultOfCharacterSelect( BOOL pi_bFlag )			{ m_byResultOfCharacterSelect = pi_bFlag; }
	inline	void	SetResultOfCharacterDelete( BOOL pi_bFlag )			{ m_byResultOfCharacterDelete = pi_bFlag; }
	inline	void	SetResultOfMoveOutFromServer( BOOL pi_bFlag )		{ m_byResultOfMoveOutFromServer = pi_bFlag; }
	inline	void	SetResultOfMoveIntoServer( BOOL pi_bFlag )			{ m_byResultOfMoveIntoServer = pi_bFlag; }
	inline	void	SetResultOfInventoryUpload( BOOL pi_bFlag )			{ m_byResultOfInventoryUpload = pi_bFlag; }

	inline	BYTE	GetResultOfUserRegistration( void )					{ return m_byResultOfUserRegistration; }
	inline	BYTE	GetResultOfLogin( void )							{ return m_byResultOfLogin; }
	inline	BYTE	GetResultOfRequestForWorldServerList( void )		{ return m_byResultOfRequestForWorldServerList; }
	inline	BYTE	GetResultOfSelectWorldServer( void )				{ return m_byResultOfSelectWorldServer; }
	inline	BYTE	GetResultOfEnterTheWorldServer( void )				{ return m_byResultOfEnterTheWorldServer; }
	inline	BYTE	GetResultOfRequestForDisconnectAccount( void )		{ return m_byResultOfRequestForDisconnectAccount; }
	inline	BYTE	GetResultOfUserInfo( void )							{ return m_byResultOfUserInfo; }
	inline	BYTE	GetResultOfCharacterCreate( void )					{ return m_byResultOfCharacterCreate; }
	inline	BYTE	GetResultOfCharacterSelect( void )					{ return m_byResultOfCharacterSelect; }
	inline	BYTE	GetResultOfCharacterDelete( void )					{ return m_byResultOfCharacterDelete; }
	inline	BYTE	GetResultOfMoveOutFromServer( void )				{ return m_byResultOfMoveOutFromServer; }
	inline	BYTE	GetResultOfMoveIntoServer( void )					{ return m_byResultOfMoveIntoServer; }
	inline	BYTE	GetResultOfInventoryUpload( void )					{ return m_byResultOfInventoryUpload; }

	//--------------------------------------------------------------------------

	inline	void	SetServerIP( BYTE pi_byType, DWORD pi_dwIP )		{ m_dwServerIP[pi_byType] = pi_dwIP; }
	inline	DWORD	GetServerIP( BYTE pi_byType )						{ return m_dwServerIP[pi_byType]; }
	inline	void	SetServerPort( BYTE pi_byType, DWORD pi_dwPort )	{ m_dwServerPort[pi_byType] = pi_dwPort; }
	inline	DWORD	GetServerPort( BYTE pi_byType )						{ return m_dwServerPort[pi_byType]; }
	inline	void	SetServerName( BYTE pi_byType, char * pi_pName )	{ memcpy( m_pServerName[pi_byType], pi_pName, 32 ); }
	inline	char *	GetServerName( BYTE pi_byType )						{ return m_pServerName[pi_byType]; }
	inline	void	GetServerName( BYTE pi_byType, BYTE * po_pName )	{ memcpy( po_pName, m_pServerName[pi_byType], 32 ); }

	//--------------------------------------------------------------------------

			void	RequestQuitProgram( void );			// 클라이언트 종료 요청

			void	SendRequestMsg( void );				// 서버에 요청 메세지를 보냈음
			void	ReceiveResponseMsg( void );			// 요청 메세지에 대한 응답이 도착했음
	inline	BOOL	IsSendRequestMsg( void )							{ return m_bIsSendMsg; }
	inline	DWORD	GetSendRequestMsgTime( void )						{ return m_dwSendMsgTime; }

			BOOL	IsPassed_RequestLimitTime( void );	// 서버에 요청 메세지를 보낸후, 한계 시간이 지났는지를 체크

	//--------------------------------------------------------------------------

	inline	DWORD	GetLastestAvatarIndex( void )						{ return m_dwLastestAvatarIndex; }
	inline	float *	GetAvatarStartPos( void )							{ return m_vecAvatarStartPos; }
	
//	inline	CLogFile *	GetLog( void )									{ return &m_LogFile; }
	inline	CLogFile *	GetClientLog( void )							{ return &m_ClientLogFile; }	
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
class CUDPNetworkMgr : public CUDPNetwork
{
// < Data Member > -------------------------------------------------------------
protected :
	CLogFile	m_ClientLogFile;

// < Member Function > ---------------------------------------------------------
public :
	CUDPNetworkMgr();
	~CUDPNetworkMgr();

	void	Init( void );
	BOOL	Create( void );
	BOOL	Destroy ( void );

	bool	DataAnalysis( UDPMsg * pi_pMsg, sockaddr_in * pi_pOutFrom );

	////////////////////////////////////////////////////////////////////////////
	////////////////////////        Protocol       /////////////////////////////
	////////////////////////////////////////////////////////////////////////////

	void	PartyMsg_PartyMemberInfo_upd( char * pi_pMsg );

	void	PartyMsg_PartyMemberHp_upd( char * pi_pMsg );
	void	PartyMsg_PartyMemberFp_upd( char * pi_pMsg );
	void	PartyMsg_PartyMemberLv_upd( char * pi_pMsg );
	void	PartyMsg_PartyMemberPos_upd( char * pi_pMsg );
	void	PartyMsg_PartyMemberEffect_upd( char * pi_pMsg );

	//--------------------------------------------------------------------------

	void	ChatMsg_ChatMessageReceipt_udp( char * pi_pMsg );
	void	ChatMsg_AnnounMessageReceipt_udp( char * pi_pMsg );
};
*/
#endif // __NETWORK_MGR_H__
