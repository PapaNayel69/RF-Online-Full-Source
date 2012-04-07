////////////////////////////////////////////////////////////////////////////
//
// Global Class and Data
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <Queue>

#include "Define.h"

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Base Data        ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct BASE_DATA
{
};

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////    Resoruce Data List    ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Resource Data List ID ( RLI_ )
#define MAX_RES_DATA_LIST	9
#define RLI_PLAYER			0
#define RLI_MONSTER			1
#define RLI_ANIMUS			2
#define RLI_GUARD_TOWER		3
#define RLI_NPC				4
#define RLI_ITEM			5
#define RLI_UNIT			6
#define RLI_SKILL			7
#define RLI_FORCE			8

//------------------------------------------------------------------------------

class CCharResDataMgr;
extern CCharResDataMgr * g_pCharResDataMgr;

#define _GetCharResDataMgr() g_pCharResDataMgr

////////////////////////////////////////////////////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////     Game Progress     /////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////////////////////////////////////////////////////////

class CMainApp;
extern CMainApp * g_pMainApp;

#define _GetMainApp()	g_pMainApp

class CGP_MainGame;
extern CGP_MainGame * g_pMainGame;

#define _GetMainGame()	g_pMainGame

////////////////////////////////////////////////////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////         Land          /////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////////////////////////////////////////////////////////

class CLand;
extern	CLand * g_pLand;				// 현재 랜드에 대한 포인터

#define _GetLand()	g_pLand

////////////////////////////////////////////////////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////         Level         /////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////////////////////////////////////////////////////////

class CLevel;
extern	CLevel * g_pLevel;				// 현재의 레벨에 대한 포인터
										// 레벨을 생성할 때를 포인터를 값을 넣는다.
										// 이후에는 무조건 읽기 속성으로 처리

#define	_GetLevel()	g_pLevel

////////////////////////////////////////////////////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////    Network Manager    /////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////////////////////////////////////////////////////////

class	CNetworkMgr;
extern	CNetworkMgr * g_pNetworkMgr;		// 네트웍 매니져에 대한 포인터

#define _GetNetworkMgr()	g_pNetworkMgr

////////////////////////////////////////////////////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////   Character Manager   /////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////////////////////////////////////////////////////////

class	CCharacterMgr;
extern	CCharacterMgr * g_pCharacterMgr;

#define _GetCharMgr() g_pCharacterMgr

////////////////////////////////////////////////////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////      Item Manager     /////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////        Avatar         /////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////////////////////////////////////////////////////////

class	CAvatar;
extern	CAvatar * g_pAvatar;

#define _GetAvatar() g_pAvatar

//BOOL SendNetMessage( DWORD pi_dwClientIndex, BYTE * pi_pType, char * pi_pMsg, int pi_nLen );

//#define NetLogWrite _GetNetworkMgr()->GetClientLog()->Write

////////////////////////////////////////////////////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////        Camera         /////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////////////////////////////////////////////////////////

class	CCamera;
extern	CCamera * g_pCamera;

#define _GetCamera()	g_pCamera

////////////////////////////////////////////////////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////  Character Interface  /////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////////////////////////////////////////////////////////

class ChInterface;
extern	ChInterface * g_ifCharIF;				// 캐릭터 인터페이스
extern	DWORD		 g_dwCharIFRefCnt;			// 캐릭터 인터페이스의 레퍼런스 카운트

//--------------------------------------------------------------------------
// - 캐릭터 인터페이스는 게임상에 오직 하나만 존재하므로 게임의 제일 첫
//   부분에서 생성시키고 마지막 부분에서 파괴시킨다.
BOOL	_CreateCharIF( LPDIRECT3DDEVICE8 pi_pD3DDevice );
BOOL	_DestroyCharIF( void );

//--------------------------------------------------------------------------
#define _GetCharIF()		g_ifCharIF

////////////////////////////////////////////////////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////         Bone          /////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
// Bone Manager ID ( BMI_ )
#define MAX_BONE_MGR		( MAX_RES_DATA_LIST - 2 )	// RLI_SKILL, RLI_FORCE 제외

//--------------------------------------------------------------------------

class	CObject;
class	CObjectManager;
class	CHARACTEROBJECT;
typedef	CObjectManager		CBoneMgr;
typedef	CHARACTEROBJECT		CBone;

//--------------------------------------------------------------------------

class CUnityBoneMgr
{
protected :
	CBoneMgr * m_pBoneMgr[MAX_BONE_MGR];

public :
	CUnityBoneMgr();
	~CUnityBoneMgr();

	void Init( void );
	BOOL Create( void );
	BOOL Destroy( void );

	inline	CBone *		AddBone( DWORD pi_dwBoneMgrIndex, char * pi_pFileName )
						{ return _GetCharIF()->LoadMeshData( m_pBoneMgr[pi_dwBoneMgrIndex], pi_pFileName, TRUE, NULL ); }

	inline	CBoneMgr *	GetBoneMgr( DWORD pi_dwBoneMgrIndex )
						{ return m_pBoneMgr[pi_dwBoneMgrIndex]; }

			CBone *		GetBoneByObject( CObject * pi_pObject, DWORD pi_dwBoneIndex );

	//inline	CBone *		GetBone( DWORD pi_dwBoneMgrIndex, DWORD pi_dwBoneIndex )
	//					{ return m_pBoneMgr[pi_dwBoneMgrIndex]->GetCharacter( pi_dwBoneIndex ); }
			CBone *		GetBone( DWORD pi_dwBoneMgrIndex, DWORD pi_dwBoneIndex );
};

//--------------------------------------------------------------------------

extern	CUnityBoneMgr * g_pBoneMgr;				// 본 매니져

BOOL	_CreateBoneMgr( void );					// 본 매니져 생성
BOOL	_DestroyBoneMgr( void );				// 본 매니져 파괴

#define	_GetBoneMgr()	g_pBoneMgr

////////////////////////////////////////////////////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////         Mesh          /////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
// Mesh Manager ID ( MMI_ )
#define MAX_MESH_MGR		( MAX_RES_DATA_LIST - 2 )	// RLI_SKILL, RLI_FORCE 제외

//--------------------------------------------------------------------------

typedef	CObjectManager		CMeshMgr;
typedef	CHARACTEROBJECT		CMesh;

//--------------------------------------------------------------------------

class CUnityMeshMgr
{
protected :
	CMeshMgr * m_pMeshMgr[MAX_MESH_MGR];

public :
	CUnityMeshMgr();
	~CUnityMeshMgr();

	void Init( void );
	BOOL Create( void );
	BOOL Destroy( void );

	inline	CMesh *		AddMesh( DWORD pi_dwMeshMgrIndex, char * pi_pFileName, CBone * pi_pBone )
						{ return _GetCharIF()->LoadMeshData( m_pMeshMgr[pi_dwMeshMgrIndex], pi_pFileName, TRUE, pi_pBone ); }

	inline	CMeshMgr *	GetMeshMgr( DWORD pi_dwMeshMgrIndex )
						{ return m_pMeshMgr[pi_dwMeshMgrIndex]; }

	//inline	CMesh *		GetMesh( DWORD pi_dwMeshMgrIndex, DWORD pi_dwObjectIndex )
	//					{ return m_pMeshMgr[pi_dwMeshMgrIndex]->GetCharacter( pi_dwObjectIndex ); }
			CMesh *		GetMesh( DWORD pi_dwMeshMgrIndex, DWORD pi_dwObjectIndex );
};

//--------------------------------------------------------------------------

extern	CUnityMeshMgr * g_pMeshMgr;				// 메시 매니져

BOOL	_CreateMeshMgr( void );					// 메시 매니져 생성
BOOL	_DestroyMeshMgr( void );				// 메시 매니져 파괴

#define	_GetMeshMgr()	g_pMeshMgr

////////////////////////////////////////////////////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////       Animation       /////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
// Animation Manager ID ( MMI_ )
#define MAX_ANI_MGR			MAX_RES_DATA_LIST	// RLI_SKILL, RLI_FORCE 포함

//--------------------------------------------------------------------------

typedef	AnimationManager	CAniDataMgr;
typedef	ChAnimation			CAniData;

//--------------------------------------------------------------------------

class CUnityAniMgr
{
protected :
	CAniDataMgr * m_pAniMgr[MAX_ANI_MGR];

public :
	CUnityAniMgr();
	~CUnityAniMgr();

	void Init( void );
	BOOL Create( void );
	BOOL Destroy( void );

	inline	CAniData *		AddAni( DWORD pi_dwAniMgrIndex, char * pi_pFileName )
							{ return _GetCharIF()->AddAnimationData( m_pAniMgr[pi_dwAniMgrIndex], pi_pFileName ); }

	inline	CAniDataMgr *	GetAniMgr( DWORD pi_dwAniMgrIndex )
							{ return m_pAniMgr[pi_dwAniMgrIndex]; }

			CAniData *		GetAni( DWORD pi_dwAniMgrIndex, DWORD pi_dwObjectIndex );
};

//--------------------------------------------------------------------------

extern	CUnityAniMgr *	g_pAnimationMgr;			// 애니메이션 매니져

BOOL	_CreateAnimationMgr( void );				// 애니메이션 매니져 생성
BOOL	_DestroyAnimationMgr( void );				// 애니메이션 매니져 파괴

#define	_GetAniMgr()	g_pAnimationMgr

//--------------------------------------------------------------------------

struct AnimationExtraInfo							// ChAnimation 에 추가적으로 쓰일 정보를 담은 구조체
{
	char	nAniNumOfSameAction;					// 동일한 액션의 애니메이션 최대 갯수

	char	nMaxFrameNum;							// 이펙트가 발생하는 프레임의 갯수 ( 현재는 최대 3개임 )
	char	nCurrentFrameNum;						// 현재 이펙트 프레임의 번호
	DWORD	dwEffectFrame[MAX_EFFECT_FRAME_NUM];	// 이펙트가 발생하는 프레임 ( -1이면 이펙트가 없음 )
	BOOL	bIsInvokedEffect[MAX_EFFECT_FRAME_NUM];	// 이펙트가 발생되었는가?	( 애니메이션 시작 프레임마다 초기화 됨 )

	AnimationExtraInfo()
	{
		nAniNumOfSameAction		= 1;

		nMaxFrameNum			= 0;
		nCurrentFrameNum		= 0;

		for( int i = 0; i < MAX_EFFECT_FRAME_NUM; ++i )
		{
			dwEffectFrame[i]	= -1;
			bIsInvokedEffect[i]	= FALSE;
		}
	};
};

typedef AnimationExtraInfo	ANI_EXT_INFO;

////////////////////////////////////////////////////////////////////////////
////////////////////////                       /////////////////////////////
//////////////////////// Character Data Table  /////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////////////////////////////////////////////////////////

class	CCharacterDataMgr;
extern	CCharacterDataMgr * g_pCharDataMgr;

//--------------------------------------------------------------------------

BOOL	_CreateCharDataMgr( void );					// 캐릭터 데이터 매니져 생성
BOOL	_DestroyCharDataMgr( void );				// 캐릭터 데이터 매니져 파괴

//--------------------------------------------------------------------------

#define	_GetCharDataMgr()	g_pCharDataMgr

////////////////////////////////////////////////////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////    Item Data Table    /////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////////////////////////////////////////////////////////

class	CItemDataMgr;
extern	CItemDataMgr * g_pItemDataMgr;

//--------------------------------------------------------------------------

BOOL	_CreateItemDataMgr( void );					// 아이템 데이터 매니져 생성
BOOL	_DestroyItemDataMgr( void );				// 아이템 데이터 매니져 파괴

//--------------------------------------------------------------------------

#define	_GetItemDataMgr()	g_pItemDataMgr

////////////////////////////////////////////////////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////       Bound Box       /////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////////////////////////////////////////////////////////

// Bound Box Manager ID ( BBMI_ )
#define MAX_BBOX_MGR	5
/*#define BBMI_PLAYER		0
#define BBMI_MONSTER	1
#define BBMI_NPC		2
#define BBMI_ITEM		3
#define BBMI_UNIT		4*/

struct BoundBox
{
	float	pMin[3];
	float	pMax[3];

	DWORD	dwBoneID;		// 바운드 박스를 가지고 있는 Bone ID
};
typedef BoundBox	BBOX;

//--------------------------------------------------------------------------

typedef list< BBOX * > BBoxList;

class CBoundBoxManager
{
private :
	BBoxList	m_stdBBoxList[MAX_BONE_MGR];

public :
	CBoundBoxManager();
	~CBoundBoxManager();

	BBOX *	LoadBoundBox( DWORD pi_dwBBoxMgrIndex, char * pi_pName, int pi_nBoneID );
	BBOX *	GetBoundBox( DWORD pi_dwBBoxMgrIndex, int nBoneID );
	BBOX *	GetBoundBoxByObject( CObject * pi_pObject, int nBoneID );
};

//--------------------------------------------------------------------------
extern CBoundBoxManager *	g_pBoundBoxMgr;			// 바운드 박스 매니져
extern int					g_nTotalBoundBoxNum;	// 바운드 박스의 총 갯수

//--------------------------------------------------------------------------
BOOL	_CreateBoundBoxMgr( void );					// 바운드 박스 매니져 생성
BOOL	_DestroyBoundBoxMgr( void );				// 바운드 박스 매니져 파괴

//--------------------------------------------------------------------------
#define	_GetBBoxMgr()	g_pBoundBoxMgr

////////////////////////////////////////////////////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////   Effect Queue 관련   /////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////////////////////////////////////////////////////////

class CCharacter;

struct EffectInfo
{
	DWORD		dwType;						// 이펙트 타입 ( 플레이어, 몬스터, 스킬, 포스, etc )
	DWORD		dwID;						// 이펙트 ID
	DWORD		dwMyself[2];				// 이펙트의 주체가 되는 캐릭터 ID
	DWORD		dwTarget[2];				// 이펙트의 타겟이 되는 캐릭터 ID
	Vector3f	dwTargetPos;				// 이펙트의 타겟이 되는 포지션
	BYTE		byRangeType;				// 이펙트의 거리 타입 ( self area, extent, flash 등 )
	BYTE		byRangeType_For_Effect;		// 이펙트의 거리 타입 ( self area, extent, flash 등 )
	float		fRange;						// 이펙트의 범위 or 각도
	DWORD		dwValue;					// 이펙트의 값

	EffectInfo();

	void	SetMyself( CCharacter * pi_pMyself );
	void	SetTarget( CCharacter * pi_pTarget );
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

typedef list< EffectInfo * >			EFFECT_INFO_LIST;
typedef list< EffectInfo * >::iterator	EFFECT_INFO_LIST_ITER;

//------------------------------------------------------------------------------

class CEffectInfoList
{
protected :

	EFFECT_INFO_LIST		m_listEffectInfo;

public :

	CEffectInfoList();
	~CEffectInfoList();

			void					Clear( void );

			void					push_back( EffectInfo * pi_pEffectInfo );
			EFFECT_INFO_LIST_ITER	erase( EFFECT_INFO_LIST_ITER & pi_iter );

	inline	EFFECT_INFO_LIST_ITER	begin( void )	{ return m_listEffectInfo.begin(); }
	inline	EFFECT_INFO_LIST_ITER	end( void )		{ return m_listEffectInfo.end(); }

	inline	DWORD					size( void )	{ return m_listEffectInfo.size(); }
	inline	BOOL					empty( void )	{ return m_listEffectInfo.empty(); }
};

//------------------------------------------------------------------------------

class CCharacter;

class CReturnEffectInfoList : public CEffectInfoList
{
public :

	CReturnEffectInfoList();
	~CReturnEffectInfoList();

	void	Clear( void );
	void	ResultProcessing( void );

private :

	BOOL	IsAttackEffect( EFFECT_INFO_LIST_ITER & pi_iter );
	BOOL	IsAttackEffect_SkillForce( EFFECT_INFO_LIST_ITER & pi_iter );
	BOOL	IsAttackEffect_Normal( EFFECT_INFO_LIST_ITER & pi_iter );

	BOOL	SetDamage( EFFECT_INFO_LIST_ITER & pi_iter );
	void	SetDamage_OneCharacter( CCharacter * pi_pChar );
	void	SetDamage_ManyCharacter( CCharacter * pi_pChar );
};

//------------------------------------------------------------------------------

extern CEffectInfoList 			g_pEffectInfoList;
extern CReturnEffectInfoList 	g_pReturnEffectInfoList;

#define	_GetEffectInfoList()		( &g_pEffectInfoList )
#define	_GetReturnEffectInfoList()	( &g_pReturnEffectInfoList )

////////////////////////////////////////////////////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////  네트웍 메세지 관련   /////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////////////////////////////////////////////////////////

// Network Message
#define NETMSG_MAX_NUM			( 1000 )	// 저장할 네트웍 메세지의 총 갯수
#define NETMSG_MAX_CHAR_NUM		( 256 )		// 네트웍 메세지 한개의 글자수

extern	BOOL	g_bIsSaveNetworkMessage;	// 네트웍 메세지를 저장할 것인가?
extern	int		g_nTotalNetworkMessage;		// 현재 저장중인 네트웍 메세지의 총 갯수

struct	NetMsgStr
{
	char msg[NETMSG_MAX_CHAR_NUM];
};

typedef	queue< NetMsgStr >	NetMsgQueue;
extern	NetMsgQueue			g_stdNetMsgQueue;	// 네트웍 메세지 저장

void	_CreateNetLog( void );
void	_SaveNetMsg( char * pi_pMsg );

#define _GetNetMsgQueue()	( &g_stdNetMsgQueue )

////////////////////////////////////////////////////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////  User Interface 관련  /////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////////////////////////////////////////////////////////

class CSpriteMgr;
extern CSpriteMgr *	g_pUiSpriteMgr;

#define _GetUISpriteMgr() g_pUiSpriteMgr

//--------------------------------------------------------------------------

class CInputMgr;
extern CInputMgr * g_pUiInputMgr;

#define _GetUIInputMgr() g_pUiInputMgr

//--------------------------------------------------------------------------

class CMouseDev;
extern CMouseDev *	g_pUiMouseDev;

#define _GetUIMouseDev() g_pUiMouseDev

//--------------------------------------------------------------------------

class CGUIScreen;
extern CGUIScreen * g_pUiScreen;

#define _GetUIScreen() g_pUiScreen

//--------------------------------------------------------------------------

class CGUIMouse;
extern CGUIMouse * g_pUiMouse;

#define _GetUIMouse() g_pUiMouse

//--------------------------------------------------------------------------

class CGUIGaugeExpWindow;
extern CGUIGaugeExpWindow * g_pUiGaugeWindow;

#define _GetGaugeWindow() g_pUiGaugeWindow

//--------------------------------------------------------------------------

class CGUIItemBeltWindow;
extern CGUIItemBeltWindow * g_pUiItemBeltWindow;

#define _GetItemBeltWindow() g_pUiItemBeltWindow

//--------------------------------------------------------------------------

class CGUIForceBeltWindow;
extern CGUIForceBeltWindow * g_pUiSFBeltWindow;

#define _GetSFBeltWindow() g_pUiSFBeltWindow

//--------------------------------------------------------------------------

class CGUIChatWindow;
extern CGUIChatWindow * g_pUiChatWindow;

#define _GetChatWindow() g_pUiChatWindow

//--------------------------------------------------------------------------
class CGUIChatInputWindow;
extern CGUIChatInputWindow * g_pUiChatInputWindow;

#define	_GetChatInputWindow() g_pUiChatInputWindow

//--------------------------------------------------------------------------

class CGUIInventoryWindow;
extern CGUIInventoryWindow * g_pUiInventWindow;

#define _GetInventoryWindow() g_pUiInventWindow

//--------------------------------------------------------------------------

class CGUIShopWindow;
extern CGUIShopWindow * g_pUiShopWindow;

#define _GetShopWindow() g_pUiShopWindow

//--------------------------------------------------------------------------

//class CGUILinkIconBoard;
//extern CGUILinkIconBoard * g_pUiLinkIconBoard;

//#define _GetUILinkIconBoard() g_pUiLinkIconBoard

//--------------------------------------------------------------------------

class CGUIForceWindow;
extern CGUIForceWindow * g_pUiForceWindow;
#define _GetForceWindow() g_pUiForceWindow

//--------------------------------------------------------------------------

class CGUISkillWindow;
extern CGUISkillWindow * g_pUiSkillWindow;
#define	_GetSkillWindow() g_pUiSkillWindow

//--------------------------------------------------------------------------

class CGUICharInfoWindow;
extern CGUICharInfoWindow * g_pUiCharInfoWindow;
#define _GetCharInfoWindow() g_pUiCharInfoWindow

//--------------------------------------------------------------------------

class CGUIHotKeyBoard;
extern CGUIHotKeyBoard * g_pUiFuncKeyWindow;
#define _GetFuncKeyWindow() g_pUiFuncKeyWindow

//--------------------------------------------------------------------------

class CGUIAttackPartWindow;
extern CGUIAttackPartWindow * g_pUiSelectAttackPartWindow;
#define _GetSelectAttackPartWindow() g_pUiSelectAttackPartWindow

//--------------------------------------------------------------------------

class CGUIMiningWindow;
extern CGUIMiningWindow * g_pUiMiningWindow;
#define _GetMiningWindow() g_pUiMiningWindow

//--------------------------------------------------------------------------

class CGUIOreProcWindow;
extern CGUIOreProcWindow * g_pUiOreProcWindow;
#define _GetOreProcWindow() g_pUiOreProcWindow

//--------------------------------------------------------------------------

class CGUIExchangeWindow;
extern CGUIExchangeWindow * g_pUiExchangeWindow;
#define _GetExchangeWindow() g_pUiExchangeWindow

//--------------------------------------------------------------------------

class CGUIRecallWindow;
extern CGUIRecallWindow * g_pUiSummonsSelectWindow;
#define _GetSummonsSelectWindow() g_pUiSummonsSelectWindow

//--------------------------------------------------------------------------

class CGUIMobControl;
extern CGUIMobControl * g_pUiSummonsControlWindow;
#define _GetSummonsControlWindow() g_pUiSummonsControlWindow

//--------------------------------------------------------------------------

class CGUINpcSelectWindow;
extern CGUINpcSelectWindow * g_pUiNpcIntroduceWindow;
#define _GetNpcIntroWindow() g_pUiNpcIntroduceWindow

//--------------------------------------------------------------------------

class CGUIItemMakeWindow;
extern CGUIItemMakeWindow * g_pUiItemWorkWindow;
#define _GetItemWorkWindow() g_pUiItemWorkWindow

//--------------------------------------------------------------------------

class CGUIItemUpgradeWindow;
extern CGUIItemUpgradeWindow * g_pUiItemUpgradeWindow;
#define _GetItemUpgradeWindow() g_pUiItemUpgradeWindow

//--------------------------------------------------------------------------

//class CGUISelectedCharWin;
//extern CGUISelectedCharWin * g_pUiTargetInfoBar;
//#define _GetTargetInfoBar() g_pUiTargetInfoBar

//--------------------------------------------------------------------------

//class CGUISelectedMobInfoWin;
//extern CGUISelectedMobInfoWin * g_pUiTargetInfoWindow;
//#define _GetTargetInfoWindow() g_pUiTargetInfoWindow

//--------------------------------------------------------------------------

class CGUIPartyWindow;
extern CGUIPartyWindow * g_pUiPartyWindow;
#define _GetPartyWindow() g_pUiPartyWindow

//--------------------------------------------------------------------------

//class CGUISelectedCharMenu;
//extern CGUISelectedCharMenu * g_pUiSelectedCharMenu;
//#define _GetSelectedCharMenu() g_pUiSelectedCharMenu

//--------------------------------------------------------------------------

class CGUIMapMoveWindow;
extern CGUIMapMoveWindow * g_pUiMoveMapWindow;
#define _GetMoveMapWindow() g_pUiMoveMapWindow

//--------------------------------------------------------------------------

class CGUISkillForceQueue;
extern CGUISkillForceQueue * g_pUiAppliedSFWindow;
#define _GetAppliedSFWindow() g_pUiAppliedSFWindow

//--------------------------------------------------------------------------

class CGUITradeWindow;
extern CGUITradeWindow * g_pUiTradeWindow;
#define _GetTradeWindow() g_pUiTradeWindow

//--------------------------------------------------------------------------

class CGUIRadarWindow;
extern CGUIRadarWindow * g_pUiRadarWindow;
#define _GetRadarWindow() g_pUiRadarWindow

//--------------------------------------------------------------------------

class CGUIMiniMapWindow;
extern CGUIMiniMapWindow * g_pUiMinimapWindow;
#define _GetMinimapWindow() g_pUiMinimapWindow

//--------------------------------------------------------------------------

class CGUIMapLoadingWindow;
extern CGUIMapLoadingWindow * g_pUiMapLoadingWindow;
#define _GetMapLoadingWindow() g_pUiMapLoadingWindow

//--------------------------------------------------------------------------

class CGUIOptionWindow;
extern CGUIOptionWindow * g_pUiOptionWindow;
#define _GetOptionWindow() g_pUiOptionWindow

//--------------------------------------------------------------------------

extern BOOL g_bUiWaitServerMsg;

////////////////////////////////////////////////////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////      Avatar Dummy     /////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define	MAX_AVATAR_DUMMY	( 3 )

class CAvatar;

extern CAvatar * g_pAvatarDummy;

#define _GetAvatarDummy(x) ( &g_pAvatarDummy[x] )

void _CreateAvatarDummy( void );
void _DestroyAvatarDummy( void );

////////////////////////////////////////////////////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////        Message        /////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////////////////////////////////////////////////////////

// 시스템 메세지 출력
//void _SetSystemMsg( char * pi_pMessage, BOOL pi_bIsNotify = FALSE );
//void _SetSystemMsg( DWORD pi_dwID, BOOL pi_bIsNotify = FALSE );

void _SetSystemMsg( const char * l_pFormatString, ... );
void _SetSystemMsg( DWORD pi_dwID );
void _SetNotifyMsg( const char * l_pFormatString, ... );

////////////////////////////////////////////////////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////          Etc          /////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////////////////////////////////////////////////////////

extern	HWND			g_hWnd;
#define	_GetHWND()		g_hWnd

extern	HINSTANCE		g_hInstance;
#define	_GetInstance()	g_hInstance

extern	float			g_fFPS;
#define	_GetFPS()		g_fFPS

extern	DWORD			g_dwCurTime;
#define _GetTime()		g_dwCurTime

void _DrawLine( float pi_fX1, float pi_fY1, float pi_fZ1,
			    float pi_fX2, float pi_fY2, float pi_fZ2,
			    DWORD pi_dwColor = 0xFFFFFFFF );

void _DrawLine( Vector3f pi_vecPoint1, Vector3f pi_vecPoint2,
			    DWORD pi_dwColor = 0xFFFFFFFF );

//--------------------------------------------------------------------------

#define MAX_GBUF_LENGTH	( 2048 )
extern char g_pBuf[MAX_GBUF_LENGTH];

//--------------------------------------------------------------------------

void _strcpy( char * po_pDestBuffer, char * pi_pOrgString, DWORD pi_dwDestBufferSize = MAX_NAME_LENGTH );

//--------------------------------------------------------------------------

extern RECT g_rectWindow;
extern RECT g_rectClient;
extern BOOL g_bIsWindowed;
#define _GetWindowRect() ( &g_rectWindow )
#define _GetClientRect() ( &g_rectClient )
#define _IsWindowed()	 ( g_bIsWindowed )

//--------------------------------------------------------------------------

// 오브젝트의 이름을 출력할 위치를 계산한다.
class CResObject;
#define FONT_WIDTH	( 6 )
#define FONT_HEIGHT	( 20 )
void _GetRenderNamePos( CResObject * pi_pResObject, Vector4f pio_vecPos, BYTE pi_bySizeOfName );

//--------------------------------------------------------------------------
// A와 B지점의 중간 지점을 가져온다.
void	_GetWaypointOfAandB( Vector3f po_vecWaypoint, Vector3f pi_vecCurPos, Vector3f pi_vecTarPos, float pi_fDist );

// log를 남긴다.
void	_WriteLog( const char * pi_pFormat, ... );

//--------------------------------------------------------------------------
// 캐릭터 라이브러리 안에 있음 ( 선언만 여기로 가져왔음 )
BOOL	IsBBoxInFrustum( float* bb_min,float* bb_max,float FrustumPlane[6][4] );			
void	SetFustumNormalPlane( float pi_pFrustumPlane[6][4], D3DXMATRIX & pi_matViewProj );

extern	float g_pFrustumPlane[6][4];
void	SetCharacterClipping( void );					// 캐릭터 클리핑

void	_R3CalculateTime();
#define R3CalculateTime_Char()  _R3CalculateTime()
#define GetCharLoopTime()		(R3GetLoopTime()*1000)

#endif // __GLOBAL_H__