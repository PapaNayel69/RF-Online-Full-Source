////////////////////////////////////////////////////////////////////////////
//
// CGUISelectedCharMenu Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUISELECTEDCHARMENU_H__
#define __GUISELECTEDCHARMENU_H__

#include "../GUIController/GUIWindow.h"
#include "../GUIController/GUIText.h"
#include "../GUIController/GUIButton.h"

/*//////////////////////////////////////////////////////////////////////////

[ CGUISelectedCharMenu ]

//////////////////////////////////////////////////////////////////////////*/
class CGUISelectedCharMenu : public CGUIWindow
{
public :
	enum { PLAYER, UNIT, GUARD_TOWER };	// 메뉴 타입


	enum								// 플레이어 메뉴				
	{
		Whisper,
		Trade,
		Party,
		Guild,
		Friend,
		Follow,

		PLAYER_MENU_NUM		
	};
	enum PartyMenu				
	{
		Invite,
		Join,		
		None
	};

	
	enum								// 유닛 메뉴
	{
		Riding,							// 탑승		
		Traction,						// 견인
		Custody,						// 보관

		UNIT_MENU_NUM
	};


	enum
	{
		GetBack,						// 회수

		GUARD_TOWER_MENU_NUM
	};


	enum { MAX_MENU_NUM = PLAYER_MENU_NUM };	

// < Data Member > ---------------------------------------------------------
private:

	BYTE			m_byMenuType;

	CGUIObject		m_uiCharNameBase;
	CGUIText		m_uiCharName;
	
	CGUIButtonEx	m_uiFuncMenu[MAX_MENU_NUM];
	

	PartyMenu		m_ePartyMenu;

	BOOL			m_bIsCreated;


// < Member Function > ----------------------------------------------------
public:
	CGUISelectedCharMenu();
	virtual ~CGUISelectedCharMenu();

	void	Init( void );	
	void	SetSprite( void );

	void	SetMenuType( BYTE pi_byMenuType );

	
	void	SetCharName( char * pi_pName );			// 선택된 캐릭터 이름 설정	

	
	void	SetPartyMenu( PartyMenu pi_eMenu );		// 파티 메뉴	


	void	EnableButton( BYTE pi_byMenuIndex, BOOL pi_bEnable );

	void	MouseEvent( _mouse_event & event );

	BOOL	Draw( void );
};

#endif // __GUISELECTEDCHARMENU_H__
