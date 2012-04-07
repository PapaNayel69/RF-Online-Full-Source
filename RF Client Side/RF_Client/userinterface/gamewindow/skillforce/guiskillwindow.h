////////////////////////////////////////////////////////////////////////////
//
// CGUISkillWindow Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUISKILLWINDOW_H__
#define __GUISKILLWINDOW_H__

#pragma warning( disable : 4786 )

#include "../../GUIController/GUIWindow.h"
#include "../../GUIController/GUIButton.h"
#include "../../GUIController/GUIScrollbar.h"
#include "../../GUIController/GUIItem.h"
#include "../../GUIController/GUIText.h"
#include "../GUICharInfoWindow.h"
#include "../GUIGaugeExpWindow.h"
#include <vector>

#define	UNIT_SIZE_Y			34

#define	SKILL_MODE_NUM		3

#define	STEP_LOW			0
#define	STEP_MIDDLE			1
#define	STEP_HIGH			2
#define	STEP_HIGHESST		3


#define TAB_MARGIN_W		4
#define TAB_MARGIN_H		8

/*//////////////////////////////////////////////////////////////////////////

[ CGUISkFcUnit ]

  - Skill, Force Unit

//////////////////////////////////////////////////////////////////////////*/
class CGUISkFcUnit : public CGUIContainer
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
	DWORD			m_dwIndex;

	BYTE			m_byStep;	// 단계 ( 저, 중, 고, 최고급 )

	UnitState		m_eUnitState;

	BOOL			m_bHovered;

	CGUISFItem		m_cItem;
	CGUIText		m_cItemName;
	CGUIObject		m_cLink;
	CGUIText		m_cLevelStr;
	CGUIObject		m_cExpBase;
	CGUITileObject	m_cExp;
	CGUINumber		m_cExpNumber;
	CGUIObject		m_cExpPercent;	
	CGUIObject		m_cGM;

	CGUIObject		m_cUnderline;
	CGUIObject		m_cCover;

	BOOL			m_bLoadedSprite;
	
// < Member Function > ----------------------------------------------------
public:
	CGUISkFcUnit();
	~CGUISkFcUnit();		
	
	void		SetSprite( void );

	void		SetSize( LONG pi_nWidth, LONG pi_nHeight );

	
	void		SetIndex( DWORD pi_dwIndex ) { m_dwIndex = pi_dwIndex; }
	void		SetStep( BYTE pi_byStep ) { m_byStep = pi_byStep; }
	void		SetItem( CGUIItem * pi_pItem );
	void		SetItem( DWORD pi_dwItemID, DWORD pi_dwSpriteID, BYTE pi_byIconType );
	void		SetItemName( char *pi_pName );
	void		SetLevel( BYTE pi_byLevel );
	void		SetExp( float pi_fExp );	
	
	DWORD		GetIndex( void ) { return m_dwIndex; }
	CGUIItem *	GetItem( void ) { return &m_cItem; }
	BYTE		GetStep( void ) { return m_byStep; }

	//--------------------------------------------------------------------

	void		SetUnitState( UnitState pi_eState );
	UnitState	GetUnitState( void ) { return m_eUnitState; }
	
	//--------------------------------------------------------------------

	void		Hovered( void );
	void		UnHovered( void );

	CGUIObject*	GetCover( void ) { return &m_cCover; }	
};

///////////////////////////////////////////////////////////////////////////
struct _skill_force_info {	

	BYTE		bySFType;		// skill or force 

	DWORD		dwIndex;	
	
	BYTE		byStep;			// 단계 ( 저, 중, 고 최고급 )
	
	char	*	pName;			// skill, force 이름
	DWORD		dwItemID;
	DWORD		dwSpriteID;
	BYTE		byLevel;		// 레벨
	float		fExpByPercent;	// 경험치
};


////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////


typedef	vector< CGUISkFcUnit * > SkillForceUnitList;

/*//////////////////////////////////////////////////////////////////////////

[ CGUISkillForceBoard ]

//////////////////////////////////////////////////////////////////////////*/
class CGUISkillForceBoard : public CGUIContainerEx,
							public IScrollObserver,
							public IMouseObserver
{

// < Data Member > ---------------------------------------------------------
private:
	SkillForceUnitList	m_listUnit;

	CGUIContainer		m_uiTabBase;

	BYTE				m_byTabNum;
	CGUIObject			m_uiTab[4];
	CGUIText			m_uiTabName[4];
	CGUIContainer		m_uiTabBoard[4];
	CGUIContainer		m_uiSFBoard[4];


	CGUISkFcUnit	*	m_pHoveredUnit;

	BOOL				m_bCreated;

public:
	CGUIScrollbar		m_uiScrollbar;

// < Member Function > ----------------------------------------------------
public:
	CGUISkillForceBoard();
	virtual ~CGUISkillForceBoard();

	void				Init( POINT & pi_ptSize, BYTE m_byTabNum );
	void				SetSprite( void );
	void				SetBound( RECT & pi_rtBound );

	BOOL				IsOpenedTab( BYTE pi_byTab );
	void				SetTab( BYTE pi_byTabIndex, BOOL pi_pOpen );
	
	void				SetLevelMastery( BYTE pi_byStep, int pi_nMastery );	// 각 단계별 마스터리 설정

	void				SetTabName( BYTE pi_byStep, char * pi_pTabName );
	
	BOOL				AddUnit( CGUISkFcUnit * pi_pUnit );
	CGUISkFcUnit	*	RemoveUnit( WORD wIndex );
	void				RemoveAllUnit( SkillForceUnitList * po_pListUnit );
	
	CGUISkFcUnit	*	GetUnit( DWORD pi_dwIndex, BOOL pi_bByIndex = TRUE );	
	CGUISkFcUnit	*	GetUnit( CGUIItem * pi_pItem );
	CGUISkFcUnit	*	GetUnitByID( DWORD pi_dwItemID );

	CGUISkFcUnit	*	GetSource( CGUIObject * pi_pSrcObj );

	SkillForceUnitList*	GetList( void ) { return &m_listUnit; }		

	
	void				SetHoverUnit( CGUISkFcUnit * pi_pUnit );
	CGUISkFcUnit	*	GetHoverUnit( void ) { return m_pHoveredUnit; }

	
	void				UpdateBoard( void );
	
	void				ScrollEvent( _scroll_event & event );	
	void				MouseEvent( _mouse_event & event );	
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////


/*//////////////////////////////////////////////////////////////////////////

[ CGUISFWindow ]

//////////////////////////////////////////////////////////////////////////*/

class CGUISFWindow : public CGUIWindow
{
// < Data Member > ---------------------------------------------------------
protected:
	
	CGUISkillForceBoard	*	m_pSFBoardList;
	BYTE					m_bySFBoardNum;

	CGUIButton			*	m_uiSFBoardTab;
	

	CGUIText				m_uiTitle;
	CGUIObjectAni			m_uiAniTitleBar;
	CGUIButton				m_uiCloseButton;
	CGUIButton				m_uiHelpButton;

	CGUIObject				m_uiBoard;

	CGUIObject				m_uiLOutline;
	CGUIObject				m_uiROutline;
	CGUIObject				m_uiTabUnderLine;	
	CGUIObject				m_uiResizeUnderBar;

	BYTE					m_byCurBoardTab;

	BOOL					m_bIsCreated;

// < Member Function > ----------------------------------------------------
public:
	
	CGUISFWindow();
	~CGUISFWindow();

			void			CreateSFBoard( BYTE pi_bySFBoardNum, BYTE * pi_pTabNumList ); // init()하기 전에 board부터 생성해야된다.
			void			Init( void );
			void			SetSprite( void );	
			void			SetBound( RECT & pi_rtBound );
			

			BOOL			TakeInItem( BYTE pi_bySFBoardIndex, _skill_force_info * pi_pSFInfo );
			BOOL			TakeOutItem( BYTE pi_bySFBoardIndex, DWORD pi_dwItemIndex );

			void			SetLevelMastery( BYTE pi_bySFBoardIndex, BYTE pi_byStep, int pi_nMastery );

			void			SetTabName( BYTE pi_bySFBoardIndex, BYTE pi_byStep, char * pi_pTabName );

			CGUISkFcUnit *	GetIconBoard( BYTE pi_bySFBoardIndex, DWORD pi_dwItemIndex, BOOL pi_bByIndex = TRUE );			

	inline	BYTE			GetCurBoardTab( void ) { return m_byCurBoardTab; }
			void			SetCurBoardTab( BYTE pi_byBoardTab );

			void			MouseEvent( _mouse_event & event );
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////


/*//////////////////////////////////////////////////////////////////////////

[ CGUISkillWindow ]

//////////////////////////////////////////////////////////////////////////*/

class CGUISkillWindow : public CGUISFWindow
{
public:
	enum SkillMode {
		Melee = 0,
		Missile,
		Knowledge,
		None_Mode
	};

	enum MissileType
	{
		Bow,
		Firearm,
		Throw,
		Launcher,
		None_Type
	};

// < Data Member > ---------------------------------------------------------
private:
	

// < Member Function > ----------------------------------------------------
public:
	CGUISkillWindow();
	virtual ~CGUISkillWindow();

	
	void	Init( void );
	void	SetSprite( void );			
};

#endif // __GUISKILLWINDOW_H__
