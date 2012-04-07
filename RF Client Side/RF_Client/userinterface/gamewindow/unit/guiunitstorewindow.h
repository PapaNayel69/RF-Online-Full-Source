#ifndef ___GUIUNITSTOREWINDOW_H__
#define ___GUIUNITSTOREWINDOW_H__

#include "../../GUIController/GUIWindow.h"
#include "../../GUIController/GUIItem.h"
#include "../../GUIController/GUIText.h"
#include "../../GUIController/GUIButton.h"
#include "../../GUIController/GUIScrollbar.h"
#include "../../GUIController/GUIListBoard.h"
#include <vector>

class STORE_UNIT_INFO : public CGUIContainer
{
private :

	DWORD			m_dwUnitID;

	CGUIObject		m_uiKeyBase;
	CGUIItem		m_uiKeyIcon;

	CGUIObject		m_uiUnitNameBase;
	CGUIText		m_uiUnitName;
	CGUIText		m_uiUnitCost;
	CGUIObject		m_uiOutLine;
	CGUIObject		m_uiCover;
	
public :
	STORE_UNIT_INFO();
	~STORE_UNIT_INFO();	
	
	virtual	void			SetSprite( void );

	inline	void			SetUnitID( DWORD pi_dwUnitID )	{ m_dwUnitID = pi_dwUnitID; }
	inline	DWORD			GetUnitID( void )				{ return m_dwUnitID; }

			void			SetKeyIcon( DWORD pi_dwItemID, DWORD pi_dwSpriteID );
			void			SetUnitName( char * pi_pName );
			void			SetCost( DWORD pi_dwCost );

	inline	CGUIObject *	GetCorver( void ) { return &m_uiCover; }

			void			Select( void );
			void			UnSelect( void );
};

/*//////////////////////////////////////////////////////////////////////////

[ CGUIUnitStoreWindow ]

//////////////////////////////////////////////////////////////////////////*/
class CGUIUnitStoreWindow : public CGUIWindow
{
public:
	typedef	vector< STORE_UNIT_INFO * >			CUnitList;

// < Data Member > ---------------------------------------------------------
private:
	CGUIObject			m_uiTitleBoard;	
	CGUIText			m_uiTitle;
	CGUIButton			m_uiCloseButton;

	CGUIListBoard		m_uiUnitListBoard;
	STORE_UNIT_INFO *	m_pSelectedUnit;


	CGUIButtonEx		m_uiTakeOutUnitButton;

	BOOL				m_bIsCreated;

// < Member Function > -----------------------------------------------------
public:
	CGUIUnitStoreWindow();
	virtual ~CGUIUnitStoreWindow();

	void	Init( void );
	void	SetSprite( void );	

	void	InsertUnit( DWORD pi_dwInsertIndex, DWORD pi_dwUnitID, DWORD pi_dwItemID, DWORD pi_dwSpriteID, char * pi_pUnitName, DWORD pi_dwCost );
	void	RemoveUnit( DWORD pi_dwUnitID );
	void	RemoveAllUnit( void );

	void	UpdateCost( DWORD pi_dwUnitID, DWORD pi_dwCost );	// 보관 비용 설정
	
	BOOL	ExistUnit( DWORD pi_dwUnitID );


	// select unit
	void	SelectUnit( DWORD pi_dwUnitID );
	void	SelectUnit( STORE_UNIT_INFO * pi_pUnitInfo );

	void	UnSelectUnit( void );
	

	void	MouseEvent( _mouse_event &event );

	BOOL	Draw( void );

private:
	STORE_UNIT_INFO * GetUnit( DWORD pi_dwUnitID );

	STORE_UNIT_INFO * GetSelectedUnit( CGUIObject * pi_pEventObj );
};

#endif // ___GUIUNITSTOREWINDOW_H__
