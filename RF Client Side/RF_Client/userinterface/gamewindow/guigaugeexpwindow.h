////////////////////////////////////////////////////////////////////////////
//
// CGUIGaugeExpWindow Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef ___GUIGAUGEEXPWINDOW_H__
#define ___GUIGAUGEEXPWINDOW_H__

#include "../GUIController/GUIWindow.h"
#include "../GUIController/GUIText.h"

#define	MAX_GAUGE_NO	3

class CGUICharInfoWindow;

/*//////////////////////////////////////////////////////////////////////////

[ CGUINumber ]

//////////////////////////////////////////////////////////////////////////*/
class CGUINumber : public CGUIObjectEx
{
// < Data Member > ---------------------------------------------------------
private:
	char	*	m_pNumberStr;
	BYTE		m_byNumberCnt;
	
	BYTE		m_byDecimalPointPos;

	DWORD		m_dwNumberColor;
	POINT		m_ptUnitSize;
	float		m_fSpriteUVUnit[2];

// < Member Function > -----------------------------------------------------
public:
	CGUINumber();
	virtual ~CGUINumber();

	void	SetNumberSprite( SPRITE_INFO * pi_pSpriteInfo, POINT & pi_ptNumSize );
	
	void	SetNumber( DWORD pi_nNumber );										// 정수
	void	SetNumber( double pi_fNumber, int pi_nDigitCntAfterDecimalPoint );	// 실수, 소숫점 뒷자리갯수

	void	SetNumberColor( DWORD pi_dwColor ) { m_dwNumberColor = pi_dwColor; }


	BOOL	Draw( void );
	BOOL	Draw( RECT & pi_rtClippingArea );

private:
	void	SetNumberStr( char * pi_pStr );
};


/*//////////////////////////////////////////////////////////////////////////

[ CGUIUnitGaugeBoard ]

   - Unit Gauge

//////////////////////////////////////////////////////////////////////////*/
class CGUIUnitGaugeBoard : public CGUIContainer
{	
// < Data Member > ---------------------------------------------------------
private:
	CGUIText			m_uiUnitName;
	
	CGUIObjectEx		m_uiHPGauge;
	CGUINumber			m_uiCurHPGaugeNum;
	CGUINumber			m_uiMaxHPGaugeNum;
	CGUIObject			m_uiHPGaugeSlash;	
	
	CGUIObjectAni		m_uiWarningLight;

	BOOL				m_bIsCreated;

// < Member Function > -----------------------------------------------------
public:
	CGUIUnitGaugeBoard();
	virtual ~CGUIUnitGaugeBoard();
	
	void	Init( void );
	void	SetSprite( void );
	
	void	SetUnitName( char * pi_pName );

	void	SetHP( DWORD pi_dwCurHP, DWORD pi_dwMaxHP );
};


/*//////////////////////////////////////////////////////////////////////////

[ CGUIGaugeExpWindow ]

  - Avatar, Unit Gauge 표시

//////////////////////////////////////////////////////////////////////////*/
class CGUIGaugeExpWindow : public CGUIWindow
{
public:
	enum Bar {
		LIFE_BAR,
		FORCE_BAR,
		STAMINA_BAR
	};

// < Data Member > ---------------------------------------------------------
private:		
	CGUIObject				m_uiBoard;
	CGUIObjectAni			m_uiBoardAni;
	
	// ---------------------------------------------------------------------
	// Avatar Gauge Board
	CGUIContainer			m_uiAvatarGaugeBoard;

	CGUIObject				m_uiGaugeBoard[MAX_GAUGE_NO];
	CGUIObjectEx			m_uiGauge[MAX_GAUGE_NO];

	CGUIObject				m_uiExpBoard;
	CGUIObjectEx			m_uiExp;	

	// gauge data
	LONG					m_nMaxGauge[MAX_GAUGE_NO];
	LONG					m_nCurrentGauge[MAX_GAUGE_NO];

	CGUINumber				m_uiCurGaugeNum[MAX_GAUGE_NO];
	CGUINumber				m_uiMaxGaugeNum[MAX_GAUGE_NO];
	CGUIObject				m_uiGaugeSlash[MAX_GAUGE_NO];	

	// exp data
	double					m_fTotalExp;
	double					m_fCurrentExp;
	
	CGUIText				m_uiExpProb;

	// level
	WORD					m_wLevel;
	CGUINumber				m_uiLevel;

	CGUICharInfoWindow	*	m_pCharInfoWin;
	// ---------------------------------------------------------------------


	// ---------------------------------------------------------------------
	// Unit Gauge Board
	CGUIUnitGaugeBoard *	m_pUnitGaugeBoard;
	// ---------------------------------------------------------------------


	BOOL					m_bCreated;

// < Member Function > -----------------------------------------------------
public:
	CGUIGaugeExpWindow();
	~CGUIGaugeExpWindow();

	void	Init();
	void	SetSprite( void );

	void	SetCharInfoWin( CGUICharInfoWindow * pi_pWin ) { m_pCharInfoWin = pi_pWin; }

	// Gauge
	void	SetGauge( Bar pi_eBar, DWORD pi_dwMaxGauge, DWORD pi_dwCurGauge );
	void	SetGauge( Bar pi_eBar, DWORD pi_dwCurGauge);
	DWORD	GetTotalGauge( Bar pi_eBar ) { return m_nMaxGauge[pi_eBar]; }
	DWORD	GetCurrentGauge( Bar pi_eBar ) { return m_nCurrentGauge[pi_eBar]; }

	// Exp
	void	SetTotalExp( double pi_fTotalExp );
	void	SetCurrentExp( double pi_fCurrentExp );
	double	GetTotalExp( void ) { return m_fTotalExp; }
	double	GetCurrentExp( void ) { return m_fCurrentExp; }

	// Level
	void	SetLevel( WORD pi_wLevel );
	WORD	GetLevel( void ) { return m_wLevel; }	


	//============================================================================//
	//                                  Boundary                                  //
	//============================================================================//
	// Unit
	void	SetUnitGaugeBoard( void );
	void	SetAvatarGaugeBoard( void );

	void	SetUnitName( char * pi_pName );
	void	SetUnitHP( DWORD pi_dwCurHP, DWORD pi_dwMaxHP );	
};

#endif // ___GUIGAUGEEXPWINDOW_H__
