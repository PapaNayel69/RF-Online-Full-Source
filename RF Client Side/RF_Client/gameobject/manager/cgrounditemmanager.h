////////////////////////////////////////////////////////////////////////////////
//
// CGroundItemManager Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __GROUND_ITEM_MANAGER_H__
#define __GROUND_ITEM_MANAGER_H__

class CGroundItem;

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CCollisionItem
{
	friend class CGroundItemManager;

	CGroundItem *	m_pGroundItem;
	Vector3f		m_vecCollidedPoint;					// 스크린 좌표와 오브젝트가 충돌한 좌표값

public :

	CCollisionItem()	{ ZeroMemory( this, sizeof( CCollisionItem ) ); }
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CGroundItemManager
{
	typedef list< CGroundItem * >				ITEM_LIST;
	typedef list< CGroundItem * >::iterator		ITEM_LIST_ITER;

	typedef list< CCollisionItem >				COLLISION_LIST;
	typedef list< CCollisionItem >::iterator	COLLISION_LIST_ITER;

protected :

	ITEM_LIST		m_listItem;						// 그라운드 아이템 리스트

	float			m_fVisualField;					// 아바타의 시야
	BOOL			m_bIsDrawBBox;
	BOOL			m_bIsRenderItemName;

	CGroundItem		m_clsHoverItem;

public :

	CGroundItemManager();
	virtual ~CGroundItemManager();

			void			Clear( void );

			void			Render( void );
			void			Animation( void );
			void			FrameMove( void );

			CObject *		CollisionCheckWithBoundBox( int pi_nPosX, int pi_nPosY, Vector3f & pi_vecCamPos, Vector3f & po_vecCollidedPoint );
			void			CheckOutPositionOfVisualField( void );
			BOOL			CheckGroundItemForCreating( CCharacter * pi_pChar );

			CGroundItem *	GetLootableItem( void );

public :

			CGroundItem *	AddItem( void );
			void			DeleteItem( DWORD pi_dwIndex );
			CGroundItem *	GetItem( DWORD pi_dwIndex );
			CGroundItem *	GetItemByMeshID( DWORD pi_dwMeshID );

			void			ClearItemEffect( void );

protected :

			void			DeleteItem( ITEM_LIST_ITER & pio_itItem );

public :

	inline	int				GetTotalItemNum( void )				{ return m_listItem.size(); }

	inline	void			SetDrawBBox( BOOL pi_bFlag )		{ m_bIsDrawBBox = pi_bFlag; }
	inline	BOOL			IsDrawBBox( void )					{ return m_bIsDrawBBox; }

	inline	void			SetRenderItemName( BOOL pi_bFlag )	{ m_bIsRenderItemName = pi_bFlag; }
	inline	BOOL			IsRenderItemName( void )			{ return m_bIsRenderItemName; }

			void			SetMouseHoverItem( CGroundItem * pi_pGroundItem );
			CGroundItem *	GetMouseHoverItem( void );
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

extern CGroundItemManager g_clsGroundItemManager;
#define _GetGroundItemMgr() ( &g_clsGroundItemManager )

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// MAYUN < begin > -------------------------------------------------------------

#define		MAX_EFFECT_OBJ	256

class CItemEffectRenderer
{
	typedef list< CMeshItem * >				ITEM_LIST;
	typedef list< CMeshItem * >::iterator	ITEM_LIST_ITER;

public :

	DWORD			m_Max;
	DWORD			m_Start;
	DWORD			m_MaxObj[MAX_EFFECT_OBJ];
	D3DXMATRIX		m_Matrix[MAX_EFFECT_OBJ][64][2];

protected :

	ITEM_LIST		m_listItem;
	DWORD			m_BlendNum[MAX_EFFECT_OBJ];

public :

	CItemEffectRenderer();
	~CItemEffectRenderer();

	void	Clear( void );

	void	AddItem( CMeshItem * pi_pOrgItem );

	BOOL	Render( void );
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

extern CItemEffectRenderer g_clsItemEffectRenderer;
#define _GetItemEffectRenderer() ( &g_clsItemEffectRenderer )

// MAYUN < end > ---------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif	// __GROUND_ITEM_MANAGER_H__