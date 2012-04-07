////////////////////////////////////////////////////////////////////////////////
//
// CBaseItemData Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifdef _NEW_UI_

#ifndef __ITEM_DATA_H__
#define __ITEM_DATA_H__

#include "../UserInterface/GUIController/GUIItem.h"
#include "../UserInterface/GameWindow/GUIForceBeltWindow.h"
#include "../datatable/CCharacterDataMgr.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

enum { BOOT_FALSE, BOOT_TRUE, BOOT_CHANGE_QUANTITY };
enum { BOT_BASE, BOT_NORMAL_ITEM, BOT_BOARD_ITEM, BOT_MESH_ITEM, BOT_EQUIP_ITEM, BOT_GROUND_ITEM,
	   BOT_SF, BOT_ANIMUS, BOT_UNIT_FRAME, BOT_UNIT_PARTS };

class CBoardObject : public CObject
{
protected :

	BYTE			m_byItemType;
	BOOL			m_bIsUsing;					// Object가 사용중인가?

	CGUIItem *		m_uiIcon;

public :

	CBoardObject();
	virtual ~CBoardObject();

			BOOL			IsEqual( DWORD pi_dwIndex );
	virtual	BOOL			IsEqual( CBoardObject * pi_pTarObject );

	virtual	CBoardObject *	New( void ) = 0;
	virtual	CBoardObject *	Clone( void ) = 0;

	virtual	BOOL			IsEqualItemType( BYTE pi_byType )	{ return ( m_byItemType == pi_byType ); }
	virtual	BYTE			GetItemType( void )					{ return m_byItemType; }

	inline	void			SetUsingState( BOOL pi_bFlag )		{ m_bIsUsing = pi_bFlag; }
	inline	BOOL			IsUsing( void )						{ return m_bIsUsing; }

	inline	void			SetIcon( CGUIItem * pi_pIcon )		{ m_uiIcon = pi_pIcon; }
	virtual	CGUIItem *		GetIcon( void )						{ return m_uiIcon; }

			void			BeginDelayAnimation( DWORD pi_dwTime );
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*==============================================================================

[ CBaseItem ]
	- 아이템의 가장 기본적인 ITEM_DATA 구조체와 오퍼레이션을 가지고 있다.
	- 이 클래스 자체로는 쓰지 않고, 상속받은 클래스를 사용한다.

==============================================================================*/

class CBaseItem
{
protected :

	ITEM_DATA *			m_pData;

public :

	CBaseItem();
	virtual ~CBaseItem();

			void		Init( void );
	virtual	void		Clear( void );
	virtual	BOOL		Create( BYTE pi_byItemKind, DWORD pi_dwDTIndex );

	virtual	void		SetData( ITEM_DATA * pi_pData )			{ m_pData = pi_pData; }
	inline	ITEM_DATA *	GetData( void )							{ return m_pData; }

			DWORD		GetOrgMaxDurability( void );
	virtual	float		GetDurabilityRate( void )				{ return 1.0f; }

	inline	DWORD		GetDTIndex( void )						{ if( m_pData ) return m_pData->dwDTIndex; else return ID_INVALID; }
	inline	DWORD		GetDTCode( void )						{ if( m_pData ) return m_pData->dwDTCode; else return ID_INVALID; }
	inline	BYTE		GetItemKind( void )						{ if( m_pData ) return m_pData->byEquipType; else return ID_INVALID; }
	inline	DWORD		GetMeshID( void )						{ if( m_pData ) return m_pData->dwMeshID; else return ID_INVALID; }
	inline	DWORD		GetTypeMeshID( void )					{ if( m_pData ) return ( m_pData->dwMeshID | 0x000000FF ); else return ID_INVALID; }
	inline	DWORD		GetIconID( void )						{ if( m_pData ) return m_pData->dwIconID; else return ID_INVALID; }

			BYTE		GetUsableLevel( void );
			BYTE		GetUsableExpertnessType( BYTE pi_byOrderNo );
			BYTE		GetUsableExpertness( BYTE pi_byOrderNo );

			BOOL		IsSameKind( CBaseItem * pi_pItem );
			BOOL		IsSameKind( BYTE pi_byItemKind, DWORD pi_dwDTIndex );
			BOOL		IsEquipmentItem( void );
			BOOL		IsArmor( void );
	inline	BOOL		IsEqualItemKind( BYTE pi_byType )		{ if( m_pData ) return ( m_pData->byEquipType == pi_byType ); else return FALSE; }

	inline	BOOL		IsExchangable( void )					{ if( m_pData ) return m_pData->bIsExchangable; else return FALSE; }
	inline	BOOL		IsSellable( void )						{ if( m_pData ) return m_pData->bIsSellable; else return FALSE; }
	inline	BOOL		IsDumpable( void )						{ if( m_pData ) return m_pData->bIsDumpable; else return FALSE; }
			BOOL		IsSplitable( void );
			BOOL		IsRepairable( void );
			BOOL		IsUpgradable( void );
			BOOL		IsMineTool( void );

	virtual	BYTE		GetLevel( void )						{ return 0; }

			BYTE		GetHoldType( void );					// 아이템이 무기나 방패일 경우 어느 손에 장착 하는지를 가져온다.
	inline	BOOL		IsEqualHoldType( BYTE pi_byHoldType )	{ return ( GetHoldType() == pi_byHoldType ); }
			BYTE		GetAttackDistanceType( void );
			float		GetAttackRange( void );
			int			GetAttackDelay( void );
			int			GetWeaponCategoryID( void );
	inline	BOOL		IsEqualWeaponCategoryID( int pi_nID )	{ return ( GetWeaponCategoryID() == pi_nID ); }
			BOOL		CheckWeaponWithBullet( DWORD pi_dwWeaponDTCode );

			BYTE		GetUnitPartType( void );

	inline	BYTE		GetMoneyType( void )					{ if( m_pData ) return m_pData->byMoneyType; else return ID_INVALID; }
	inline	BOOL		IsEqualMoneyType( BYTE pi_byType )		{ if( m_pData ) return m_pData->byMoneyType == pi_byType; else return FALSE; }
	inline	DWORD		GetStandardPrice( void )				{ if( m_pData ) return m_pData->dwStandardPrice; else return 0; }
			float		GetSellPrice( BYTE pi_byNum = 1 );
			float		GetBuyPrice( BYTE pi_byNum = 1 );
			float		GetOreProcPrice( BYTE pi_byNum = 1 );	// 원석 가공 비용

			char *		GetDescription( void );
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CItemUpgradeEffect
{
	friend class CNormalItem;

public :

	float			m_fStrikingPower;				// 공격력 증가 ( * )
	float			m_fDefencePower;				// 방어력 증가 ( * )
	float			m_fStunProb;					// 공격시 타겟을 스턴시킬 확률 증가 ( * )
	BYTE			m_byAttackRange;				// 공격 사정 거리 증가 ( + )
	BYTE			m_byAttackAttribute;			// 공격 속성 ( default = 0xFF )
	BYTE			m_byResist[MAX_ITEM_ATTRIBUTE];	// 내성 증가 ( + )
	float			m_fAvoidProb;					// 회피율 증가 ( * )

public :

	CItemUpgradeEffect();
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

class CNormalItem : public CBaseItem
{
	friend class CItemUpgradeEffect;
	friend class CBoardItem;

protected :

	BYTE				m_byQuantity;				// 아이템의 수량 ( 수량일 경우 => default = 1, 최대 = MAX_ITEM_QUANTITY )
	DWORD				m_dwDurability;				// 아이템의 내구성
	DWORD				m_dwCurMaxDurability;		// 아이템의 최대 내구성 ( 현재의 최대 내구성이다. )

	BYTE				m_byMaxSocketNum;			// 아이템의 업그레이드를 위한 소켓의 최대 갯수
	BYTE				m_byLevel;					// 아이템의 업그레이드 레벨 ( defalut = 0 )
	INGOT_DATA *		m_listTalik[MAX_ITEM_LEVEL];// 아이템을 업그레이드 시키기 위해 삽입한 탈릭의 데이터
	CItemUpgradeEffect	m_clUpgradeEffect;			// 아이템 업그레이드 효과 정보

public :

	CNormalItem();
	virtual ~CNormalItem();

			void		Init( void );
	virtual	void		Clear( void );
	virtual	BOOL		Create( BYTE pi_byItemKind, DWORD pi_dwDTIndex );

	virtual	void		SetQuantity( BYTE pi_byQuantity )		{ m_byQuantity = pi_byQuantity; }
	inline	BYTE		GetQuantity( void )						{ return m_byQuantity; }
	inline	void		SetDurability( DWORD pi_dwCurDP )		{ m_dwDurability = pi_dwCurDP; }
	inline	DWORD		GetDurability( void )					{ return m_dwDurability; }
	inline	void		SetCurMaxDurability( DWORD pi_dwMaxDP )	{ m_dwCurMaxDurability = pi_dwMaxDP; }
	inline	DWORD		GetCurMaxDurability( void )				{ return m_dwCurMaxDurability; }
	virtual	float		GetDurabilityRate( void );
			DWORD		GetRepairableMaxDP( void );				// 수리 가능한 최대 내구성 포인트
			float		GetRepairPrice( void );					// 아이템의 수리 가격

	inline	void		SetLevel( BYTE pi_byLevel )				{ m_byLevel = pi_byLevel; }
	virtual	BYTE		GetLevel( void )						{ return m_byLevel; }
			DWORD		GetDefencePoint( void );
			WORD		GetResist( BYTE pi_byType );
			BYTE		GetWeaponAttributeType( void );

public :

	inline	void		SetMaxSocketNum( BYTE pi_bySocketNo )	{ m_byMaxSocketNum = pi_bySocketNo; }
	inline	BYTE		GetMaxSocketNum( void )					{ return m_byMaxSocketNum; }
			void		InsertTalik( BYTE pi_byTalikDTIndex );
			void		InsertTalik( INGOT_DATA * pi_pTalikData );
			void		InsertTalikFromServerFlag( DWORD pi_dwServerFlag );
			void		ExtractTalik( void );
			void		ExtractAllTalik( void );
			INGOT_DATA*	GetTalik( BYTE pi_bySocketNo );

			DWORD		IsEnableInsertJewel( DWORD pi_dwJewelDTIndex );		// 아이템이 무기/갑옷일 경우 pi_dwJewelDTIndex 에 해당하는 탈릭/보석을 넣을 수 있는지를 체크한다.

			CItemUpgradeEffect*	GetUpgradeEffectInfo( void )	{ return &m_clUpgradeEffect; }
			void				ApplyUpgradeEffect( INGOT_DATA * pi_pTalik, BOOL pi_bIsAdd );

private :

			BYTE		GetSameTalikNum( INGOT_DATA * pi_pTalik );
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CBoardItem : public CBoardObject, public CNormalItem
{
protected :

	DWORD	m_dwExtra;									// 추가적인 정보를 저장하기 위한 공간. 아이템의 종류에 따라서 사용하는 용도가 다르다.

public :

	CBoardItem();

			void			Init( void );
	virtual	void			Clear( void );
	virtual	BOOL			Create( BYTE pi_byItemKind, DWORD pi_dwDTIndex );

	virtual	void			SetData( ITEM_DATA * pi_pData );

	virtual	void			SetQuantity( BYTE pi_byQuantity );
			BYTE			AddQuantity( BYTE pi_byQuantity );
			BYTE			RemoveQuantity( BYTE pi_byQuantity );

	virtual	CBoardObject *	New( void );
	virtual	CBoardObject *	Clone( void );
	virtual	void			Copy( CObject * pi_pOrgObject );
			void			CopyToNormalItem( CNormalItem * pi_pNormalItem );

	inline	void			SetExtraData( DWORD pi_dwData )		{ m_dwExtra = pi_dwData; }
	inline	DWORD			GetExtraData( void )				{ return m_dwExtra; }
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define MAX_EFFECT_DUMMY	( 10 )

class CMeshItem : public CResObject, public CBaseItem
{
protected :

	CNormalItem *	m_pLinkItem;
	CMesh *			m_pMesh;

	//--------------------------------------------------------------------------

	struct		// by J.U.I
	{
		BOOL	m_bCheckExistEF;				// 리소스에 붙는 효과가 있는지 쳌크한  (장언일)
		float	m_dwRandRot;					// 그라운드 아이템 다양하게 회전하기위한 변수(장언일)
		DWORD	m_dwEFUniqueID;					// 리소스에 붙는 효과 unique코드 (장언일)
		BYTE	m_byDummy[MAX_EFFECT_DUMMY];	// 아이템 더미 오브젝트 인덱스들... 이걸로 궤적이나 총구등 위치를 잡는다.(장언일)
	};

public :

	CMeshItem();
	CMeshItem( CNormalItem * pi_pLinkItem );

	virtual	BOOL		IsEqualItemType( BYTE pi_byType )		{ return ( BOT_MESH_ITEM == pi_byType ); }
	virtual	BYTE		GetItemType( void )						{ return BOT_MESH_ITEM; }

			void		Init( void );
	virtual	void		Clear( void );
	virtual	BOOL		Create( BYTE pi_byItemKind, DWORD pi_dwDTIndex );

	virtual	void		Init_EveryFrame( void );

	virtual	BOOL		Render( void );
			BOOL		RenderWithShadow( void );
	virtual BOOL		Animation( DWORD pi_dwAniFrame = -1 );
	virtual	void		FrameMove( void );
	virtual	BOOL		RenderName( void );

			void		DeleteItemEffect( void );

protected :

			BOOL		Render_CheckBeforeRender( void );
			void		Render_PushMeshAlpha( void );
			void		Render_PopMeshAlpha( void );
			void		Render_RenderWithEffect( void );

			void		FrameMove_ItemEffect( void );

public :

			BOOL		SetAnimation( DWORD pi_dwAniID );

protected :

			CAniData *	GetAnimationData( DWORD pi_dwAniID );

public :

	inline	void		SetLinkItem( CNormalItem * pi_pLinkItem )	{ m_pLinkItem = pi_pLinkItem; }

	virtual	void		SetBone( DWORD pi_dwItemDTIndex, BYTE pi_byRLIType = RLI_ITEM );
	virtual	void		SetMesh( DWORD pi_dwItemDTIndex, BYTE pi_byRLIType = RLI_ITEM );
	inline	CMesh *		GetMesh( void )								{ return m_pMesh; }

	virtual	BYTE		GetLevel( void );
	virtual	BYTE		GetQuantity( void )							{ if( m_pLinkItem ) return m_pLinkItem->GetQuantity(); else return 0; }

	inline	void		ClearItemEffect( void )						{ m_bCheckExistEF = FALSE; }

public :

			BYTE		GetDummyIndex( DWORD pi_dwArrayId );
			void		SetDummyIndex( DWORD pi_dwArrayId, DWORD pi_dwId );
			BOOL		GetDummyPos( Vector3f * pi_pVecPos );
	inline	float		GetRandRot( void )							{ return m_dwRandRot; }
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CEquipItem : public CMeshItem
{
protected :

	BYTE		m_byFadeMode;					// fade in / out
	BYTE		m_byMaxFadeFrame;				// fade 되는 총 프레임 수
	BYTE		m_byCurFadeFrame;				// fade 되는 현재 프레임 수

public :

	CEquipItem();

	virtual	BOOL	IsEqualItemType( BYTE pi_byType )	{ return ( BOT_EQUIP_ITEM == pi_byType ); }
	virtual	BYTE	GetItemType( void )					{ return BOT_EQUIP_ITEM; }

	virtual	BOOL	Render( void );

			void	SetFadeMode( BYTE pi_byMode, BYTE pi_byMaxFrame = 60 );
	inline	BYTE	GetFadeMode( void )					{ return m_byFadeMode; }
	inline	BOOL	IsEqualFadeMode( BYTE pi_byMode )	{ return ( m_byFadeMode == pi_byMode ); }
			BOOL	IsFading( void );
			BOOL	IsStartFading( void );

protected :

			void	ProcessFading( void );
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CGroundItem : public CMeshItem
{
protected :

	DWORD		m_dwOwnerIndex;					// 아이템을 획득할 수 있는 권한을 가지는 캐릭터의 서버 인덱스 ( -1 이면 누구나 가질 수 있음 )
	DWORD		m_dwDroperIndex[2];				// 아이템을 버린 사람
	DWORD		m_dwCreateTime;					// 아이템이 생성된 시간

	BYTE		m_byQuantity;

	BOOL		m_bIsPortalGotoDungeon;			// 던젼으로 들어가는 포탈인가?

public :

	CGroundItem();

	virtual	BOOL	IsEqualItemType( BYTE pi_byType )		{ return ( BOT_GROUND_ITEM == pi_byType ); }
	virtual	BYTE	GetItemType( void )						{ return BOT_GROUND_ITEM; }

	virtual	void	SetBone( DWORD pi_dwItemDTIndex, BYTE pi_byRLIType = RLI_ITEM );
	virtual	void	SetMesh( DWORD pi_dwItemDTIndex, BYTE pi_byRLIType = RLI_ITEM );

			void	SetOwner( BYTE pi_byLootState, DWORD pi_dwOwnerIndex );
	inline	DWORD	GetOwnerIndex( void )					{ return m_dwOwnerIndex; }

			void	SetDroper( DWORD pi_dwCharTypeID, DWORD pi_dwIndex );
	inline	DWORD	GetDroperTypeID( void )					{ return m_dwDroperIndex[0]; }
	inline	DWORD	GetDroperIndex( void )					{ return m_dwDroperIndex[1]; }

	inline	void	SetCreateTime( DWORD pi_dwTime )		{ m_dwCreateTime = pi_dwTime; }
	inline	DWORD	GetCreateTime( void )					{ return m_dwCreateTime; }

	virtual	void	SetQuantity( BYTE pi_byQuantity )		{ m_byQuantity = pi_byQuantity; }
	virtual	BYTE	GetQuantity( void )						{ return m_byQuantity; }

	inline	BOOL	IsPortalGotoDungeon( void )				{ return m_bIsPortalGotoDungeon; }
	inline	void	SetPortalGotoDungeon( BOOL pi_bFlag )	{ m_bIsPortalGotoDungeon = pi_bFlag; }
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct SF_DATA;
class CGUISkFcUnit;

class CSFItem : public CBoardObject
{
public :
	enum { SFS_ACTIVE, SFS_DEACTIVE, SFS_LINK, SFS_NOT_EXIST };		//	Skill Force State ( SFS_ )

protected :

	BYTE			m_bySFType;					// CAT_SKILL / CAT_FORCE

	DWORD			m_dwSuccessCount;			// 스킬/포스의 성공 횟수
	BYTE			m_byLevel;					// 스킬/포스의 현재 레벨 ( 0 ~ 6, 출력은 1 ~ 7 )
	float			m_fExpByPercent;			// 레벨업 퍼센트

	SF_DATA *		m_pData;					// 추가 정보
	CGUISkFcUnit *	m_uiIconBoard;

	BOOL			m_bIsHave;					// 스킬/포스를 가지고 있는가?
	DWORD			m_dwItemDTIndex;			// 포스에서만 사용. 포스창과 인벤토리간의 이동시에 사용됨.

public :

	CSFItem();
	virtual	~CSFItem();

			void			Init( void );
	virtual	void			Clear( void );
	virtual	BOOL			Create( BYTE pi_bySFType, DWORD pi_dwDTIndex );

	virtual	BOOL			IsEqual( CBoardObject * pi_pTarObject );

	virtual	CBoardObject *	New( void );
	virtual	CBoardObject *	Clone( void );

			void			SetSFType( BYTE pi_bySFType )		{ m_bySFType = pi_bySFType; }
			BYTE			GetSFType( void )					{ return m_bySFType; }
			BOOL			IsEqualSFType( BYTE pi_byType )		{ return ( m_bySFType == pi_byType ); }
			BOOL			IsSameKind( CSFItem * pi_pSFItem );

	virtual	void			SetIndex( DWORD pi_dwIndex );
	virtual	void			SetName( char * pi_pName );
	inline	DWORD			GetDTIndex( void )					{ if( m_pData ) return m_pData->dwDTIndex; else return ID_INVALID; }
	inline	DWORD			GetDTCode( void )					{ if( m_pData ) return m_pData->dwDTCode; else return ID_INVALID; }
	inline	DWORD			GetIconID( void )					{ if( m_pData ) return m_pData->dwIconID; else return ID_INVALID; }
	inline	BYTE			GetMasteryIndex( void )				{ if( m_pData ) return m_pData->byMasteryIndex; else return ID_INVALID; }
	virtual	void			SetData( SF_DATA * pi_pData );
	inline	SF_DATA *		GetData( void )						{ return m_pData; }

			// Set은 제일 처음 서버로부터 받은 데이터를 세팅할 때만 쓰고, 게임내에서는 Increase를 사용한다.
			void			CalculateSuccessCount( DWORD pi_dwCount = ID_INVALID );

	inline	void			SetSuccessCount( DWORD pi_dwSC )	{ m_dwSuccessCount = pi_dwSC; }
	inline	DWORD			GetSuccessCount( void )				{ return m_dwSuccessCount; }
	inline	DWORD			IncreaseSuccessCount( void )		{ return ( ++m_dwSuccessCount ); }
			void			SetLevel( BYTE pi_byLevel );
	inline	BYTE			GetLevel( void )					{ return m_byLevel; }
			void			SetExpByPercent( float pi_fExp );
	inline	float			GetExpByPercent( void )				{ return m_fExpByPercent; }

			BOOL			IsEqualTargetType( BYTE pi_byTargetType );
			BYTE			GetUsingType( BYTE pi_byOrderNo );
			BOOL			IsAttackable( void );						// 공격 가능한 Skill / Force 인가?
			BOOL			IsHaveUsingType( BYTE pi_byUsingType );		// 스킬/포스의 해당 타입을 가지고 있는가?
			BYTE			GetContinuanceUsingType( void );			// 지속형 스킬/포스의 타입을 가져온다.
			BYTE			GetRangeType( void );						// 공격 가능한 스킬/포스일 경우, 공격 범위를 가져온다.
			float			GetAttackRange( void );
	inline	BYTE			GetStep( void )						{ if( m_pData ) return m_pData->byStep; else return 0; }
	inline	int				GetCastDelay( void )				{ if( m_pData ) return m_pData->fDelay; else return 0; }
			BYTE			GetEffectType( BYTE pi_byOrderNo );
			BYTE			GetEffectUnit( BYTE pi_byOrderNo );
			float			GetEffectValue( BYTE pi_byOrderNo );
			DWORD			GetEffectTime( void );
	inline	BYTE			GetSpecialEffect( void )			{ if( m_pData ) return m_pData->bySpecialEffectType; else return 0xFF; }

			BOOL			IsUsableRace( BYTE pi_byRace );
			BOOL			IsUsableClassType( BYTE pi_byClassType );

	inline	void			SetHave( BOOL pi_bIsHave )			{ m_bIsHave = pi_bIsHave; }
	inline	BOOL			IsHave( void )						{ return m_bIsHave; }
	inline	void			SetItemDTIndex( DWORD pi_dwIndex )	{ m_dwItemDTIndex = pi_dwIndex; }
	inline	DWORD			GetItemDTIndex( void )				{ return m_dwItemDTIndex; }

			void			SetIconBoard( CGUISkFcUnit * pi_pIconBoard );
	inline	CGUISkFcUnit*	GetIconBoard( void )				{ return m_uiIconBoard; }

	virtual	BYTE			GetUIClassType( void );

			void			SetState( BYTE pi_byState );
			BOOL			IsEqualState( BYTE pi_byState );

	inline	BYTE			GetRangeTypeForEffect( void )		{ if( m_pData ) return m_pData->byRangeType_For_Effect; else return 0xFF; }
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CGUIRecallUnit;

class CAnimusItem : public CBoardObject, public CBaseItem
{
public :

	enum { AIS_ACTIVE, AIS_DEACTIVE, AIS_LINK, AIS_NOT_EXIST };				// Animus Item State ( AIS_ )
	enum { AGT_RECRUIT, AGT_VETERAN, AGT_MASTER, AGT_AMI, MAX_AGT_TYPE };	// Animus Grade Type ( AGT_ )

	friend class CAnimusBoard;

protected :

	CAnimusData *		m_pCharData;
	CGUIRecallUnit *	m_uiIconBoard;

	DWORD				m_dwExp;
	DWORD				m_dwHP;
	DWORD				m_dwFP;

	DWORD				m_dwSummonTime;			// 애니머스를 소환 혹은 귀환시킨 시간

public :

	CAnimusItem();
	virtual ~CAnimusItem();

			void			Init( void );
	virtual	void			Clear( void );

	virtual	CBoardObject *	New( void );
	virtual	CBoardObject *	Clone( void );

	virtual	void			SetIndex( DWORD pi_dwIndex );
	virtual	void			SetName( char * pi_pName );
	virtual	void			SetData( ITEM_DATA * pi_pData );
			void			SetCharData( CAnimusData * pi_pData );
	inline	CAnimusData *	GetCharData( void )					{ return m_pCharData; }

public :

	inline	BYTE			GetLevel( void )					{ if( m_pCharData ) return m_pCharData->m_byLevel; else return 1; }
			BYTE			GetGrade( void );
			void			SetExp( DWORD pi_dwExp );
	inline	DWORD			GetExp( void )						{ return m_dwExp; }
			DWORD			GetExpByLevel( void );
			DWORD			GetMaxExpByLevel( void );

			void			SetHP( DWORD pi_dwHP );
	inline	DWORD			GetHP( void )						{ return m_dwHP; }
	inline	DWORD			GetMaxHP( void )					{ if( m_pCharData ) return m_pCharData->m_dwMaxHP; else return 0; }
			void			RecoverHP( void );
	inline	DWORD			GetHPRecoveryDelay( void )			{ if( m_pCharData ) return m_pCharData->m_dwHPRecoveryDelay; else return 0; }

			void			SetFP( DWORD pi_dwFP );
	inline	DWORD			GetFP( void )						{ return m_dwFP; }
	inline	DWORD			GetMaxFP( void )					{ if( m_pCharData ) return m_pCharData->m_dwMaxFP; else return 0; }
			void			RecoverFP( void );
	inline	DWORD			GetFPRecoveryDelay( void )			{ if( m_pCharData ) return m_pCharData->m_dwFPRecoveryDelay; else return 0; }

public :

			DWORD			GetMinAttPower( void );
			DWORD			GetMaxAttPower( void );
	inline	DWORD			GetDeffencePower( void )			{ if( m_pCharData ) return m_pCharData->m_dwDefencePower; else return 0; }
			BYTE			GetResist( BYTE pi_byAttributeType );

			DWORD			GetNeedfulFP( void );				// 소환 가능 FP
			DWORD			GetNeedfulExpertness( void );		// 소환 가능 숙련도

			void			SetIconBoard( CGUIRecallUnit * pi_pIconBoard );
	inline	CGUIRecallUnit*	GetIconBoard( void )				{ return m_uiIconBoard; }

			void			SetState( BYTE pi_byState );
			BOOL			IsEqualState( BYTE pi_byState );
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CGUIUnitTuningWindow;
class CUnitTuningBoard;

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

class CUnitPartsItem : public CBoardObject, public CBaseItem
{
	friend class CUnitTuningBoard;
	friend class CUnitProc;

protected :

	DWORD	m_dwDurability;					// 아이템의 현재 내구성 ( Backpack일 경우에는 Boost Point, Bullet일 경우에는 수량임 )
	BOOL	m_bIsHave;						// 아바타가 소지한 아이템인가?

public :

	CUnitPartsItem();
	virtual ~CUnitPartsItem();

			void			Init( void );
	virtual	void			Clear( void );
	virtual	BOOL			Create( BYTE pi_byItemKind, DWORD pi_dwDTIndex );

	virtual	CBoardObject *	New( void );
	virtual	CBoardObject *	Clone( void );
	virtual	void			Copy( CObject * pi_pOrgObject );

			BOOL			IsDefaultParts( void );

	virtual	void			SetDurability( DWORD pi_dwCurDP )	{ m_dwDurability = pi_dwCurDP; }
	inline	DWORD			GetDurability( void )				{ return m_dwDurability; }
	virtual	float			GetDurabilityRate( void );

	inline	void			SetHave( BOOL pi_bFlag )			{ m_bIsHave = pi_bFlag; }
	inline	BOOL			IsHave( void )						{ return m_bIsHave; }

	virtual	DWORD			GetMaxStrikingPower( void );
	virtual	DWORD			GetMinStrikingPower( void );
	virtual	DWORD			GetDefencePower( void );
	virtual	WORD			GetResist( BYTE pi_byAttributeType );
	virtual	float			GetMoveSpeed( void );

	virtual	BYTE			GetAttackExpertness( void );
	virtual	BYTE			GetDefenceExpertness( void );

			BYTE			GetBulletSocketNum( void );
			BYTE			GetBoosterSpeed( void );
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

#define	DEFAULT_KEEPING_PRICE	( 10000 )				// 기본 보관료
#define	DEFAULT_DELIVERY_PRICE	( 1000 )				// 기본 출고료
#define	DEFAULT_TRACTION_PRICE	( 1000 )				// 기본 견인료

class CUnitItem : public CUnitPartsItem
{
	friend class CUnitTuningBoard;
	friend class CUnitProc;

protected :

	CUnitPartsItem			m_listPart[MAX_UNIT_PART_TYPE];
	CUnitPartsItem 			m_listBullet[MAX_UNIT_BULLET_TYPE];
	CUnitPartsItem			m_listBackpackBullet[MAX_BACKPACK_BULLET_NUM];
	BYTE					m_byBackpackBulletNum;

	CUnitPartsItem *		m_pWeaponParts;				// 현재 사용중인 공격 파츠

	DWORD					m_dwTractionDebt;			// 채무 견인료

	CGUIUnitTuningWindow *	m_uiTuningWindow;			// 튜닝 윈도우

public :

	CUnitItem();
	virtual ~CUnitItem();

	virtual	CBoardObject *	New( void );
	virtual	CBoardObject *	Clone( void );
	virtual	void			Copy( CObject * pi_pOrgObject );

			BOOL			CreateByFrameType( BYTE pi_byUnitFrameType );
			BOOL			CreateByUnitItem( CUnitItem * pi_pUnitItem );

protected :

			BOOL			CreateFrameByUnitItem( CUnitItem * pi_pUnitItem );
			BOOL			CreateAllPartsByUnitItem( CUnitItem * pi_pUnitItem );
			BOOL			CreateAllBulletsByUnitItem( CUnitItem * pi_pUnitItem );
			BOOL			CreateAllBackpackBulletsByUnitItem( CUnitItem * pi_pUnitItem );

public :

			void			Clear( void );

			CUnitItem *		AddFrame( DWORD pi_dwDTIndex );
			void			RemoveFrame( void );
			CUnitPartsItem*	AddParts( BYTE pi_byPartType, DWORD pi_dwDTIndex );
			void			RemoveParts( BYTE pi_byPartType );
			CUnitPartsItem*	AddBullet( BYTE pi_byPartType, DWORD pi_dwDTIndex );
			void			RemoveBullet( BYTE pi_byPartType );
			CUnitPartsItem*	AddBackpackBullet( BYTE pi_byOrderNo, DWORD pi_dwDTIndex );
			void			RemoveBackpackBullet( BYTE pi_byOrderNo );

protected :

			CUnitItem *		AddFrame( CUnitItem * pi_pUnitItem );
			void			SetFrameUI( BOOL pi_bIsAdd );
			CUnitPartsItem*	AddParts( BYTE pi_byPartType, CUnitPartsItem * pi_pOrgPartsItem );
			void			SetPartsUI( CUnitPartsItem * pi_pParts, BOOL pi_bIsAdd );
			CUnitPartsItem*	AddBullet( BYTE pi_byPartType, CUnitPartsItem * pi_pOrgBulletItem );
			void			SetBulletUI( BYTE pi_byPartType, CUnitPartsItem * pi_pBullet, BOOL pi_bIsAdd );
			CUnitPartsItem*	AddBackpackBullet( BYTE pi_byOrderNo, CUnitPartsItem * pi_pOrgBackpackBulletItem );
			void			SetBackpackBulletUI( BYTE pi_byOrderNo, CUnitPartsItem * pi_pBackpackBullet, BOOL pi_bIsAdd );

public :

			CUnitPartsItem*	GetParts( BYTE pi_byPartType );
			CUnitPartsItem*	GetBullet( BYTE pi_byPartType );
			CUnitPartsItem*	GetBackpackBullet( BYTE pi_byOrderNo );
			BYTE			GetBulletSocketNum( BYTE pi_byPartType );
			DWORD			GetBulletNum( BYTE pi_byPartType );
			void			RefreshBulletUI( BYTE pi_byPartType, BOOL pi_bIsAdd );

public :

	virtual	void			SetDurability( DWORD pi_dwCurDP );

	virtual	DWORD			GetMaxStrikingPower( void );
	virtual	DWORD			GetMinStrikingPower( void );
	virtual	DWORD			GetDefencePower( void );
	virtual	float			GetMoveSpeed( void );
			void			GetMoveSpeedString( char * po_pOutputString );
	virtual	WORD			GetResist( BYTE pi_byAttributeType );
	virtual	BYTE			GetAttackExpertness( void );
	virtual	BYTE			GetDefenceExpertness( void );
			void			SetCurrentBoosterPoint( BYTE pi_byCurBP );
			BYTE			GetCurrentBoosterPoint( void );

			DWORD			GetDeliveryPrice( DWORD pi_dwMerchantIndex );
			DWORD			GetRepairPrice( void );

	inline	void			SetTractionDebt( DWORD pi_dwDebt )	{ m_dwTractionDebt = pi_dwDebt; }
	inline	DWORD			GetTractionDebt( void )				{ return m_dwTractionDebt; }

			void			SetAttackParts( BYTE pi_byItemKind );
	inline	CUnitPartsItem*	GetAttackParts( void )				{ return m_pWeaponParts; }
			void			SetBulletNum( BYTE pi_byAttackParts, DWORD pi_dwBulletNum );
			void			SetBulletNumThatCurAttackParts( DWORD pi_dwBulletNum );
			DWORD			GetBulletNumThatCurAttackParts( void );

			BOOL			IsHavePartsOrBullets( void );
			void			SetHaveAllPartAndBullets( void );
			void			SetPartsHaveState( BYTE pi_byPartType, BOOL pi_bState );
			void			SetBulletHaveState( BYTE pi_byPartType, BOOL pi_bState );
			void			SetBackpackBulletHaveState( BYTE pi_byOrderNo, BOOL pi_bState );

protected :

			void			ResetUnitDefenceAndSpeed( void );

public :

			BOOL			AddByTuningBoard( BYTE pi_byPackNo, CBoardItem * pi_pBoardItem, BOOL pi_bIsOrgItem );
			BOOL			AddByTuningBoard( BYTE pi_byPackNo, BYTE pi_bySocketNo, CUnitPartsItem * pi_pPartItem );
			void			RemoveByTuningBoard( BYTE pi_byPackNo, CBoardItem * pi_pBoardItem );
			void			RemoveByTuningBoard( BYTE pi_byPackNo, BYTE pi_bySocketNo );
			CUnitPartsItem*	GetPartsByTuningBoard( BYTE pi_byPackNo, CBoardItem * pi_pBoardItem );
			CUnitPartsItem*	GetPartsByTuningBoard( BYTE pi_byPackNo, BYTE pi_bySocketNo );
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __ITEM_DATA_H__

#endif // _NEW_UI_
