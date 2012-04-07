////////////////////////////////////////////////////////////////////////////
//
// CGUIDungeon UI Header
//
////////////////////////////////////////////////////////////////////////////
#ifndef ___GUIDUNGEON_H__
#define ___GUIDUNGEON_H__

#include "../../GUIController/GUIWindow.h"
#include "../../GUIController/GUIText.h"

#define POLY_NUM	4


/*//////////////////////////////////////////////////////////////////////////

[ CGUIKillingCounter ]

   - 배틀던전 킬카운트 UI

//////////////////////////////////////////////////////////////////////////*/
class CGUIKillingCounter : public CGUIWindow
{
// < Data Member > ---------------------------------------------------------
private:
	WORD		m_wMaxKillingNum;
	WORD		m_wCurKillingNum;

	BOOL		m_bIsCreated;

	SPRITE_INFO				m_sGaugeSpriteInfo;
	float					m_fGaugeUV[2];	
	_D3DR3TLVERTEX_TEX1		m_listVertex[POLY_NUM][3];
	BYTE					m_byPolyNum;
		

// < Member Function > -----------------------------------------------------
public:
	CGUIKillingCounter();
	virtual ~CGUIKillingCounter();

	void	Init( void );
	void	SetSprite( void );

	void	SetMaxKillingNum( WORD pi_wMaxKillingNum );
	void	SetCurrentKillingNum( WORD pi_wCurKillingNum );

	BOOL	Draw( void );
};




/*//////////////////////////////////////////////////////////////////////////

[ CGUIMissionStateWin ]

   - 미션상태표시

//////////////////////////////////////////////////////////////////////////*/

class CGUIMissionStateWin : public CGUIWindow
{
public:
	enum MissionState
	{
		Start,
		Clear,
		Fail,
		NoneState
	};

// < Data Member > ---------------------------------------------------------
private:
	MissionState	m_eMissionState;

	DWORD			m_dwShowTime;
	DWORD			m_dwShowStartTime;
	DWORD			m_dwFadeOutInterval;
	BOOL			m_bIsStartedFadeOut;

	DWORD			m_dwShowMissionInfoStartTime;
	
	DWORD			m_dwMsgBoardColor;		
	DWORD			m_dwMissionInfoColor;

	CGUIObject		m_uiMsgBoard;
	CGUIObject		m_uiMissionText;
	CGUIObject		m_uiStateText;
	CGUIText		m_uiMissionInfoStr[2];

	CGUIText		m_uiMissionStr;


	BOOL			m_bIsCreated;

// < Member Function > -----------------------------------------------------
public:	
	CGUIMissionStateWin();
	~CGUIMissionStateWin();

	void	Init( void );
	void	SetSprite( void );

	void	Open( void );
	void	Close( void );

	// ---------------------------------------------------------------------------

	void	SetMissionState( MissionState pi_eState );

	void	SetMissionInfoStr( char *pi_pInfoStr[2] );
	void	SetMissionInfoStrColor( DWORD pi_dwColor );


	void	SetShowTime( DWORD pi_dwTime );

	// ---------------------------------------------------------------------------
	
	void	Update( void );	
};

#endif // ___GUIDUNGEON_H__
