////////////////////////////////////////////////////////////////////////////
//
// CGUIRecallWindow Class Header
//
////////////////////////////////////////////////////////////////////////////
#ifndef ___GUIRECALLWINDOW_H__
#define ___GUIRECALLWINDOW_H__

#include "../../GUIController/GUIWindow.h"
#include "../../GUIController/GUIButton.h"
#include "../../GUIController/GUIScrollbar.h"
#include "../../GUIController/GUIItem.h"
#include "../GUIGaugeExpWindow.h"

/*//////////////////////////////////////////////////////////////////////////

[ CGUIRecallUnit ]  

//////////////////////////////////////////////////////////////////////////*/
class CGUIRecallUnit : public CGUIContainer
{
public:
	enum UnitState {
		Enabled,	// 사용 가능
		Disabled,	// 사용 불가능
		Linked,		// belt에 link된 상태
		NoneUnit	// 아이템이 없는 상태
	};

// < Data Member > ---------------------------------------------------------
private:	
	UnitState		m_eUnitState;

	DWORD			m_dwIndex;

	CGUIText		m_cMobName;	
	CGUIText		m_cLevel;

	CGUIItem		m_cItem;
	CGUIObject		m_cLink;
	
	CGUIObject		m_cGagugeBase;
	CGUIObjectEx	m_cHPGauge;
	CGUIObjectEx	m_cFPGauge;
	CGUIObjectEx	m_cExpGauge;
	
	CGUINumber		m_cCurHP;	
	CGUINumber		m_cMaxHP;
	CGUIObject		m_cHPSlash;

	CGUINumber		m_cCurFP;
	CGUINumber		m_cMaxFP;
	CGUIObject		m_cFPSlash;	

	CGUIObject		m_cExpBase;
	CGUINumber		m_cExp;
	CGUIObject		m_cExpPercent;	

	CGUIObject		m_cUnderline;
	CGUIObject		m_cCover;

	BOOL			m_bLoadedSprite;

// < Member Function > -----------------------------------------------------
public:
	CGUIRecallUnit();
	~CGUIRecallUnit();

	void	SetSprite( void );

	void	SetUnitState( UnitState pi_eState );
	UnitState	GetUnitState( void ) { return m_eUnitState; }
	
	void	SetIndex( DWORD pi_dwIndex ) { m_dwIndex = pi_dwIndex; }
	DWORD	GetIndex( void ) { return m_dwIndex; }

	void	SetMobName( char * pi_pName );
	void	SetLevel( BYTE pi_byLevel );
	
	void	SetItem( DWORD pi_dwItemID, DWORD pi_dwSpriteID );	
	DWORD	GetItemID( void ) { return m_cItem.GetItemID(); }
	CGUIItem * GetItem( void ) { return &m_cItem; }
	
	void	SetHP( WORD pi_wCurHP, WORD pi_wMaxHP );
	void	SetFP( WORD pi_wCurHP, WORD pi_wMaxFP );

	void	SetExp( float pi_fExpPercent );
	float	GetExpRate( void );		// return 0.f ~ 1.f

	CGUIObject *	GetCover( void ) { return &m_cCover; }
};

// ---------------------------------------------------------------------------
struct _recall_info {	

	DWORD		dwIndex;

	char	*	pName;

	DWORD		dwItemID;
	DWORD		dwSpriteID;	
	
	BYTE		byLevel;		// 레벨

	int			nCurHP;			// hp
	int			nMaxHP;

	int			nCurFP;			// fp
	int			nMaxFP;			
	
	float		fExp;			// exp 범위 : 0 ~ 100
};

/*//////////////////////////////////////////////////////////////////////////

[ CGUIRecallWindow ]

  - 소환창

//////////////////////////////////////////////////////////////////////////*/
class CGUIRecallWindow : public CGUIWindow,
						 public IScrollObserver
{
public:
	typedef list< CGUIRecallUnit * >		CGUIRecallList;

// < Data Member > ---------------------------------------------------------
private:
	CGUIRecallList		m_listRecallUnit;

	CGUIContainerEx		m_uiBoard;

	CGUIText			m_uiTitle;
	CGUIObjectAni		m_uiAniTitleBar;
	CGUIButton			m_uiCloseButton;
	CGUIButton			m_uiHelpButton;
	
	CGUIContainerEx		m_uiUnitBoard;	
	CGUIScrollbar		m_uiScrollbar;

	CGUIObject			m_uiLOutline;
	CGUIObject			m_uiROutline;

	CGUIObject			m_uiResizeUnderBar;
	
	BOOL				m_bIsCreated;

// < Member Function > -----------------------------------------------------
public:
	CGUIRecallWindow();
	virtual ~CGUIRecallWindow();

	void	Init( void );
	void	SetSprite( void );	
	void	SetBound( RECT & pi_rtBound );


	BOOL	TakeInItem( _recall_info * pi_pRecallInfo );

	BOOL	TakeOutItem( DWORD pi_dwIndex );

	BOOL	TakeOutLink( DWORD pi_dwIndex, BOOL pi_bByIndex = TRUE );
	
	void	SetLevel( DWORD pi_dwIndex, BYTE pi_byLevel, BOOL pi_bByIndex = TRUE );
	void	SetHP( DWORD pi_dwIndex, int pi_nCurHP, int pi_nMaxHP, BOOL pi_bByIndex = TRUE );
	void	SetFP( DWORD pi_dwIndex, int pi_nCurFP, int pi_nMaxFP, BOOL pi_bByIndex = TRUE );
	void	SetExp( DWORD pi_dwIndex, float pi_fExp, BOOL pi_bByIndex = TRUE );

	void	SetItemState( DWORD pi_dwIndex, CGUIRecallUnit::UnitState pi_eState, BOOL pi_bByIndex = TRUE );

	// ---------------------------------------------------------------------	
	void	MouseEvent( _mouse_event & event );
	void	ScrollEvent( _scroll_event & event );	

private:	
	void	UpdateScrollbar( void );

public :
	CGUIRecallUnit *	GetRecallUnitByIndex( DWORD pi_dwIndex );
	CGUIRecallUnit *	GetRecallUnitByItemID( DWORD pi_dwItemID );
	CGUIRecallUnit *	GetRecallUnitByEventObj( CGUIObject * pi_pObj );
};

#endif // ___GUIRECALLWINDOW_H__
