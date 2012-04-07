////////////////////////////////////////////////////////////////////////////
//
// CGUISelectedCharWin Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUISELECTEDCHARWIN_H__
#define __GUISELECTEDCHARWIN_H__

#include "../GUIController/GUIWindow.h"
#include "../GUIController/GUIText.h"


/*//////////////////////////////////////////////////////////////////////////

[ CGUISelectedCharWin ]

//////////////////////////////////////////////////////////////////////////*/
class CGUISelectedCharWin : public CGUIWindow
{
// < Data Member > ---------------------------------------------------------
private:
	CGUIObject		m_uiStateBoard;
//	CGUIObject		m_uiStateLightR;
//	CGUIObject		m_uiStateLightL;
	CGUIObjectEx	m_uiHPGauge;
	CGUIText		m_uiCharName;


	WORD			m_wGaugeXLen;
	BOOL			m_bIsAttackState;


	BOOL			m_bCreated;

// < Member Function > ----------------------------------------------------
public:
	CGUISelectedCharWin();
	virtual ~CGUISelectedCharWin();

	void	Init( void );
	void	SetSprite( void );
	
	void	SetCharName( char * pi_pName );			// 선택된 캐릭터 이름 설정
	void	SetCharNameColor( DWORD pi_dwColor );
	
	void	SetAttackState( void );					// 공격가능한 상태로 설정한다.
	void	SetSelectState( void );					// 선택된 상태

	void	SetHPRate( float pi_fHPRate );			// 0.f ~ 1.f

	void	Update( void );
};

/*//////////////////////////////////////////////////////////////////////////

[ CGUISelectedMobInfoWin ]

//////////////////////////////////////////////////////////////////////////*/
class CGUISelectedMobInfoWin : public CGUIWindow
{
// < Data Member > ---------------------------------------------------------
private:
	CGUIText		m_uiMobName;

	BYTE			m_byMaxGaugeLen;
	CGUIObjectEx	m_uiHPGauge;

	CGUIObject		m_uiPowerBase;
	CGUIObjectEx	m_uiPower;

	BOOL			m_bIsCreated;

// < Member Function > ----------------------------------------------------
public:
	CGUISelectedMobInfoWin();
	virtual ~CGUISelectedMobInfoWin();

	void	Init( void );
	void	SetSprite( void );


	void	SetMobName( char * pi_pString );

	void	SetHP( float pi_fHP );			// 범위 : 0.f ~ 1.f

	void	SetPower( BYTE pi_byPower );	// 범위 : 0 ~ 7
};

#endif // __GUISELECTEDCHARWIN_H__
