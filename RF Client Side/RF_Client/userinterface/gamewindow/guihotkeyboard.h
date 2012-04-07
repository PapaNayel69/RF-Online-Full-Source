////////////////////////////////////////////////////////////////////////////
//
// CGUIHotKeyBoard Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUIHOTKEYBOARD_H__
#define __GUIHOTKEYBOARD_H__

#include "../GUIController/GUIWindow.h"
#include "../GUIController/GUIButton.h"
#include "../GUIController/GUIItem.h"
#include "GUIGaugeExpWindow.h"


/*//////////////////////////////////////////////////////////////////////////

[ CGUIUnitControlBoard ]  

  - 기갑장비 탑승시 사용자 컨트롤 보드가 이것으로 바뀐다.

//////////////////////////////////////////////////////////////////////////*/

class CGUIUnitControlBoard : public CGUIContainer, public IMouseObserver
{
public:
	enum WeaponType 
	{
		Main_Weapon,
		Sub_Weapon,
		None_Weapon
	};

// < Data Member > ---------------------------------------------------------
private:
	CGUIObject			m_uiActionKey;
	CGUIObject			m_uiMoveKey;
	CGUIObject			m_uiAttackKey;

	CGUIObject			m_uiWeaponKey[2];
	CGUINumber			m_uiBulletNum[2];

	CGUIObjectAni		m_uiSelectedWeaponAni;
	
	CGUIObject			m_uiGetOffUnitKey;

	BOOL				m_bIsCreated;	

// < Member Function > -----------------------------------------------------
public:
	CGUIUnitControlBoard();
	~CGUIUnitControlBoard();

	void		Init();
	void		SetSprite( void );
	
	void		SetActionMode( BYTE pi_byMode );
	void		SetMoveMode( BYTE pi_byMode );
	void		SetAttackMode( BYTE pi_byMode );
	

	void		SelectWeapon( WeaponType pi_eWeapon );
	void		SetBulletNum( WeaponType pi_eWeapon, WORD pi_wBulletNum );
	

	void		MouseEvent( _mouse_event & event );
};


////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////        Boundary          ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CGUIItemBeltWindow;
class CGUIForceBeltWindow;


#define HOTKEY_ID_ACTION			0
#define HOTKEY_ID_MOVE				1
#define HOTKEY_ID_ATTACK			2
#define HOTKEY_ID_INVENTORY			3
#define HOTKEY_ID_CHARINFO			4
#define HOTKEY_ID_SKILL				5
#define HOTKEY_ID_FORCE				6
#define HOTKEY_ID_SUMMONS			7
#define HOTKEY_ID_GROUP				8
#define HOTKEY_ID_QUEST				9
#define HOTKEY_ID_OPTION			10

#define	HOTKEY_NUM					11
	

#define	MAX_SUB_KEY_NUM				6
/*//////////////////////////////////////////////////////////////////////////

[ CGUIHotKeyBoard ]

  - Inventory, Spell, Skill창을 띄우기 위한
    단축 아이콘들을 모아놓은 board이다.

//////////////////////////////////////////////////////////////////////////*/

class CGUIHotKeyBoard : public CGUIWindow
{
public:
	enum BoardType
	{
		Avatar_Board,
		Unit_Board
	};

	enum ActionMode
	{
		Peace,
		Combat
	};

	enum MoveMode
	{
		Walk,
		Run		
	};

	enum AttackMode
	{
		Manual,
		Auto
	};

	enum Window_Hotkey
	{
		Inventory,

		// submenu
		CharInfo,
		Skill,
		Force,		
		Summons,
		Group,
		Quest,		
		// -------

		Menu,

		WIN_NUM
	};

	enum Skill_Subkey
	{
		Melee,
		Missile,
		Make,
		Knowledge
	};

// < Data Member > ---------------------------------------------------------
private:

	BoardType				m_eBoardType;		// Avatar or Unit
	
	
	CGUIContainer			m_uiAvatarCtrlBoard;	// Avatar Control Board
	CGUIUnitControlBoard *	m_pUnitCtrlBoard;		// Unit Control Board
	
	CGUIObject				m_uiActionHotkey;		// avatar action key
	CGUIObject				m_uiMoveHotkey;
	CGUIObject				m_uiAttackHotkey;		

	// window hotkey
	CGUIObject				m_uiInventoryKey;
	CGUIObject				m_uiSystemKey;

	CGUIObject				m_uiCharInfoKey;

	CGUIContainer			m_uiSubkeyBoard;
	CGUIObject				m_uiCharInfoSubKey[MAX_SUB_KEY_NUM];		

	CGUIWindow	*			m_pRefWindow[WIN_NUM];

	CGUIItemBeltWindow	*	m_pPotionWin;
	CGUIForceBeltWindow	*	m_pSFWin;

	
	int						m_nOnMouseSubkey;	

	CGUIObjectEx			m_uiBoard;
	CGUIObjectAni			m_uiBoardAni;	

	char					m_pHotKeyStr[HOTKEY_NUM][32];	

	BOOL					m_bIsCreated;

// < Member Function > -----------------------------------------------------
public :
	CGUIHotKeyBoard();
	virtual ~CGUIHotKeyBoard();

	void	Init( void );
	void	SetSprite( void );

	void	SetRaceType( BYTE pi_byRaceType );

	void	SetUnitControlBoard( void );
	void	SetAvatarControlBoard( void );
	
	// Avatar or Unit 상태 변경 
	void	ChangeActionMode( ActionMode pi_eMode );
	void	ChangeMoveMode( MoveMode pi_eMode );
	void	ChangeAttackMode( AttackMode pi_eMode );	

	// Only Unit Control
	void	SelectUnitWeapon( CGUIUnitControlBoard::WeaponType pi_eWeapon );
	void	SetUnitBulletNum( CGUIUnitControlBoard::WeaponType pi_eWeapon, WORD pi_wBulletNum );
	
	// key에 해당하는 window를 설정한다.
	void	SetRefWindow( Window_Hotkey pi_eHotkey, CGUIWindow * pi_pWindow );		

	void	SetBeltWindow( CGUIItemBeltWindow * pi_pPotionWin, CGUIForceBeltWindow * pi_pSFWin );
	void	SetHotKeyStr( BYTE pi_byIndex, char * pi_pStr );
	
	void	MouseEvent( _mouse_event & event );
	void	MouseMotionEvent( _mouse_motion_event & event );
	
	void	ClosingDialogBox( CGUIDialogBox * pi_pDlg );

	BOOL	IsCrossed( LONG pi_nXPos, LONG pi_nYPos );

	BOOL	IsOpenedCharInfoSubkey( void );
	void	OpenCharInfoSubkey( void );
	void	CloseCharInfoSubkey( void );

	void	TooltipSetting( CGUIObject * pi_pObject, BYTE pi_byIndex );
	
private :	

	void	ScaleSubkey( void );	
};


#endif // __GUIHOTKEYBOARD_H__
