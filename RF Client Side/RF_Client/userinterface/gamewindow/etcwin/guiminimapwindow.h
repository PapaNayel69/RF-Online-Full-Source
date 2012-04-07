////////////////////////////////////////////////////////////////////////////
//
// CGUIMiniMapWindow Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUIMINIMAPWINDOW_H__
#define __GUIMINIMAPWINDOW_H__

#include "../../GUIController/GUIWindow.h"
#include "../../GUIController/GUIButton.h"
#include "../../GUIController/GUIText.h"

struct MINIMAP_OBJ_INFO
{	
	DWORD			dwID;
	BYTE			byObjType;	
	POINT			ptObjPos;
	CGUIObject		uiObject;
};

typedef list< MINIMAP_OBJ_INFO * >		CGUIMiniMapObjList;


#define MINIMAP_TOOLTIP_STR_LEN		64

struct MINIMAP_TOOLTIP_OBJ_INFO
{
	DWORD			dwID;	
	CGUIObject		uiObject;
	char			pTooltipStr[MINIMAP_TOOLTIP_STR_LEN];
};

typedef list< MINIMAP_TOOLTIP_OBJ_INFO * >		CGUIMiniMapTooltipList;

/*//////////////////////////////////////////////////////////////////////////

[ CGUIMiniMapWindow ]   

//////////////////////////////////////////////////////////////////////////*/
class CGUIMiniMapWindow : public CGUIWindow
{
public:
	enum MiniMapObjType
	{
		PartyLeader,
		PartyMember,
		Teleport,
		Gate
	};

// < Data Member > ---------------------------------------------------------
private:
	CGUIObject			m_uiTitleBoard;
	CGUIText			m_uiTitle;
	CGUIButton			m_uiCloseButton;

	CGUIObject			m_uiMiniMap;

	CGUIObjectAni		m_uiAvatarObject;		// 미니맵상에 아바타 객체
	
	CGUIMiniMapObjList	m_listMiniMapObject;	// 미니맵상에 아바타를 제외한 객체
	CGUIContainer		m_uiObjectBoard;		// m_listObject를 담고 있다.

	CGUIText			m_uiAvatarPosition;


	CGUIContainer			m_uiTooltipObjBoard;
	CGUIMiniMapTooltipList	m_listTooltipList;
	

	// --------------------------------------------------------------------	
	POINT				m_ptRealMinimapSize;	// 실제 미니맵 사이즈
	POINT				m_ptSpriteMapSize;		// 스프라이트 미니맵 사이즈	
	POINT				m_ptCutSize;			

	RECT				m_rtMapBound;			// 실제 맵 영역
	int					m_nMapRotate;			// 맵 방위 설정
	

	float				m_fPixelPerMeter[2];	// x, z축 미터당 픽셀수
	
	POINT				m_ptAvatarPos;			// 맵상의 아바타 위치	

	BYTE				m_byMapIndex;	
	BYTE				m_byMiniMapIndex;
	

	BOOL				m_bCreated;


// < Member Function > ----------------------------------------------------
public:
	CGUIMiniMapWindow();
	virtual ~CGUIMiniMapWindow();

			void	Init( void );
			void	SetSprite( void );	

			void	SetMapName( char * pi_pName );

			void	SetMapIndex( BYTE pi_byMapIndex, BYTE pi_byMiniMapIndex );

	inline	BOOL	IsLoadedMap( void )			{ return ( m_byMiniMapIndex != 0xFF ); }
	inline	BYTE	GetMiniMapIndex( void )		{ return m_byMiniMapIndex; }
	
	
			void	SetMapBound( RECT & pi_rtBound );	// 맵 경계선 설정
			void	SetMapRotate( int pi_nAngle );

			
			// --------------------------------------------------------------------
			// Avatar
			void	SetAvatarPosition( int pi_nPosX, int pi_nPosZ );
			void	SetAvatarRotate( float pi_fAngle );	


			// --------------------------------------------------------------------
			// Avatar를 제외한 나머지 객체
			void	AddMiniMapObject( MiniMapObjType pi_eObjType, DWORD pi_dwID, int pi_nPosX, int pi_nPosZ );
			MINIMAP_OBJ_INFO *	ExistMiniMapObject( DWORD pi_dwID );
			
			void	RemoveMiniMapObject( DWORD pi_dwID );
			void	RemoveAllMiniMapObject( void );
			
			void	UpdateMiniMapObject( DWORD pi_dwID, int pi_nPosX, int pi_nPosZ );	


			// --------------------------------------------------------------------
			// Tooltip Object
			void	AddTooltipObject( DWORD pi_dwID, int pi_nPosX, int pi_nPosY, int pi_nSizeX, int pi_nSizeY, char * pi_pTooltipStr );
			BOOL	ExistTooltipObject( DWORD pi_dwID );

			void	RemoveTooltipObject( DWORD pi_dwID );
			void	RemoveAllTooltipObject( void );


			// --------------------------------------------------------------------
			void	MouseEvent( _mouse_event & event );	

			BOOL	Draw( void );

private:

			void	UpdateObjectPosition( MINIMAP_OBJ_INFO * pi_pObjInfo );	

			void	SetMinimapObjectSprite( MINIMAP_OBJ_INFO * po_pObjInfo );

			void	SetPixelPerMeter( void );
};

#endif // __GUIMINIMAPWINDOW_H__
