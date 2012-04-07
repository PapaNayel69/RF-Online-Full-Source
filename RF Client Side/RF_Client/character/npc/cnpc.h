////////////////////////////////////////////////////////////////////////////////
//
// CNpc Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __CHAR_NPC_H__
#define __CHAR_NPC_H__

#include "../Character/CCharacter.h"

/*//////////////////////////////////////////////////////////////////////////////

[ CMerchantItemMgr ]

//////////////////////////////////////////////////////////////////////////////*/

class CMerchantItemMgr
{
protected :
	DWORD			m_dwMaxNum;			// 메모리를 할당받을 아이템의 최대 수
	DWORD			m_dwTotalNum;		// 현재 리스트에 있는 아이템의 최대 수
	CBoardItem *	m_pItemList;

public :

	CMerchantItemMgr();
	virtual ~CMerchantItemMgr();

	//--------------------------------------------------------------------------

			BOOL			Create( DWORD pi_dwMaxItemNum = MAX_MERCHANT_ITEM_NUM );

	//--------------------------------------------------------------------------

			CBoardItem *	AddItem( DWORD pi_dwOrderIndex );
			CBoardItem *	AddItem2( DWORD pi_dwOrderIndex );
			void			DeleteItem( DWORD pi_dwOrderIndex );
			void			Clear( void );

	//--------------------------------------------------------------------------

			CBoardItem *	GetItem( DWORD pi_dwIndex );
			CBoardItem *	GetItemByOrder( DWORD pi_dwOrderIndex );		// Index가 ID_INVALID인 아이템도 포함
			CBoardItem *	GetItemByOrder2( DWORD pi_dwOrderIndex );		// Index가 ID_INVALID인 아이템 미포함
			CBoardItem *	GetItemByMeshID( DWORD pi_dwMeshID );

	inline	DWORD			GetTotalItemNum( void )	{ return m_dwTotalNum; }

	//--------------------------------------------------------------------------

			BOOL			IsPassedLimitTimeFromReceiveNetworkMsg( void );
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*//////////////////////////////////////////////////////////////////////////////

[ CNpc ]

//////////////////////////////////////////////////////////////////////////////*/

class CNpc : public CCharacter
{
protected :

	BYTE				m_byMerchantTypeID;		// 상인 타입
	DT_CNMInfo *		m_pData;				// 상인의 데이터

	CMesh *				m_pMesh;				// 메시

	CMerchantItemMgr	m_cItemList;			// 상인의 아이템 리스트
	DWORD				m_dwTotalItemNum;		// 중고품 상인은 아이템 리스트를 페이지마다 요청하므로 m_cItemList 에서는 최대량을 알 수 없다. 때문에, 따로 유지를 해 줘야 한다.
	DWORD				m_dwItemListVersion;	// 상인이 가지고 있는 아이템 리스트의 버젼 ( 현재는 중고품 상인만이 쓰고 있다. )


public :

	CNpc();
	virtual ~CNpc();

	virtual	CCharacter *	New( void );
	virtual	CCharacter *	Clone( void );

	//--------------------------------------------------------------------------

			BOOL			Render( void );							// 렌더링
			void			RenderShadow( void );					// 그림자 렌더링
			void			CreateShadow( void );					// 그림자 생성
			BOOL			Animation( DWORD pi_dwAniFrame = -1 );	// 애니메이션 ( 본 or 메시 )
			void			FrameMove( void );						// 캐릭터의 움직임

			void			SetAction( DWORD pi_dwActionID, BOOL pi_bCheckActionDuplication = TRUE );

			BOOL			SetEffectInfo( DWORD pi_dwActionID );	// 이펙트의 정보를 설정한다.

			void			RenderInfo( void );

	//--------------------------------------------------------------------------

protected :

			CAniData *		GetAnimationData( DWORD pi_dwActionID );
			void			SetAnimationByAction( DWORD pi_dwActionID );

	//--------------------------------------------------------------------------

public :

			void			UI_OpenIntroduceWindow( void );

			BOOL			OpenShopWindow( BYTE pi_byButtonOrder );
			

	//--------------------------------------------------------------------------

	//inline	void		SetMerchantTypeID( BYTE pi_bType )	{ m_byMerchantTypeID = pi_bType; }
	//inline	BYTE		GetMerchantTypeID( void )			{ return m_byMerchantTypeID; }
	inline	BYTE			GetMerchantType( void )				{ if( m_pData ) return m_pData->byMerchantType; else return ID_INVALID; }
	inline	BYTE			IsEqualMerchantType( BYTE pi_bType ){ if( m_pData ) return ( m_pData->byMerchantType == pi_bType ); else return FALSE; }
	inline	BYTE			GetRace( void )						{ if( m_pData ) return m_pData->byRace; else return ID_INVALID; };
	virtual	BYTE			GetWidth( void ) { return 0; }

	inline	void			SetData( DT_CNMInfo * pi_pData )	{ m_pData = pi_pData; }
	inline	DT_CNMInfo * 	GetData( void )						{ return m_pData; }

	virtual	void			SetBone( DWORD pi_dwBoneID );
			void			SetMesh( DWORD pi_dwIndex );
			DWORD			GetMeshID( void );
	inline	CMesh *			GetMesh( void )						{ return m_pMesh; }

	inline	void			SetItemListVersion( DWORD pi_dwVer ){ m_dwItemListVersion = pi_dwVer; }
	inline	DWORD			GetItemListVersion( void )			{ return m_dwItemListVersion; }

	inline	void			SetTotalItemNum( DWORD pi_dwNum )	{ m_dwTotalItemNum = pi_dwNum; }
	inline	DWORD			GetTotalItemNum( void )				{ return m_dwTotalItemNum; }

	inline	CMerchantItemMgr *	GetShopItemList( void )		{ return &m_cItemList; }						

	//--------------------------------------------------------------------------

			void			Enable( DWORD pi_dwFlag );
			void			Disable( DWORD pi_dwFlag );

			BOOL			ProcessTimeOverFromReceiveFixPositionMsg( void );
			BOOL			IsPassedLimitTimeFromReceiveNetworkMsg( void );
			BOOL			IsPassedLimitTimeForDelete( void );
			BOOL			IsReceiveFixPositionMsg( void );

			void			UI_SetRadar( void );
};

#endif // __CHAR_NPC_H__