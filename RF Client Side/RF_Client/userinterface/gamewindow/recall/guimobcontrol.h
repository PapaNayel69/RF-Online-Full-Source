/*//////////////////////////////////////////////////////////////////////////

[ CGUIMobControl ]
 
   - 소환몹 컨트롤러

//////////////////////////////////////////////////////////////////////////*/

#ifndef ___GUIMOBCONTROL__
#define ___GUIMOBCONTROL__

#include "../../GUIController/GUIWindow.h"
#include "../../GUIController/GUIButton.h"

#include "../GUIGaugeExpWindow.h"


class CGUIMobControl : public CGUIWindow
{
public:
	enum Command
	{
		Cmd_Target,
		Cmd_Action,
		Cmd_Follow,		
	
		TOTAL_COMMAND_NUM
	};

	enum MobState
	{
		Action,
		Follow
	};

// < Data Member > ---------------------------------------------------------
private:

	CGUIObject			m_uiBoard;

	CGUIText			m_uiMobLevel;
	CGUIText			m_uiMobName;

	CGUIObjectEx		m_uiHPGauge;
	CGUIObjectEx		m_uiFPGauge;
	CGUIObjectEx		m_uiExpGauge;

	CGUINumber			m_uiCurHPGaugeNum;
	CGUINumber			m_uiMaxHPGaugeNum;
	CGUIObject			m_uiHPGaugeSlash;

	CGUINumber			m_uiCurFPGaugeNum;
	CGUINumber			m_uiMaxFPGaugeNum;
	CGUIObject			m_uiFPGaugeSlash;

	CGUIObject			m_uiMobCmdIcon;
	CGUIObject			m_uiMobCmdBoard;

	CGUIContainerEx		m_uiCmdViewBaord;
	CGUIContainer		m_uiCmdBoard;
	CGUIButton			m_uiCmdButton[TOTAL_COMMAND_NUM];

	BOOL				m_bIsOpenedCmdBoard;

	BOOL				m_bIsCreated;

// < Member Function > -----------------------------------------------------
public:
	CGUIMobControl();
	virtual ~CGUIMobControl();

	void	Init( void );
	void	SetSprite( void );

	void	SetMobName( char * pi_pName );
	void	SetLevel( BYTE pi_byLevel );

	void	SetHP( int pi_nHP, int pi_nMaxHP );
	void	SetFP( int pi_nFP, int pi_nMaxFP );
	void	SetExp( float pi_fExpRate );	// 0.f ~ 1.f

	void	SetMobState( MobState pi_eMobState );

	// ----------------------------------------------------------------------


	void	MouseEvent( _mouse_event & event );
	BOOL	IsCrossed( LONG pi_nXPos, LONG pi_nYPos );

	void	Update( void );

private:
	void	OpenCmdBoard( BOOL pi_bOpen );
	
};

#endif // ___GUIMOBCONTROL__
