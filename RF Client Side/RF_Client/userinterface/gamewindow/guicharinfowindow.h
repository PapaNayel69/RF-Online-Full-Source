////////////////////////////////////////////////////////////////////////////
//
// CGUICharInfoWindow Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef ___GUICHARINFOWINDOW_H__
#define ___GUICHARINFOWINDOW_H__

#include "../GUIController/GUIWindow.h"
#include "../GUIController/GUIButton.h"
#include "../GUIController/GUIText.h"
#include "../GUIController/GUIScrollbar.h"

#include "GUIGaugeExpWindow.h"


// 공격 기술 숙련도
#define	MELEE_MASTERY		0
#define	MISSILE_MASTERY		1
#define	RACE_MASTERY		2	// 종족별 기술 ( 유닛, 소환, 런처 )
#define	FORCE_MASTERY		3
#define	SHIELD_MASTERY		4
#define	DEFFENCE_MASTERY	5

// 제작
#define	WEAPON_MAKE			0
#define	ARMOR_MAKE			1
#define	BULLET_MAKE			2

// 저항력
#define	FIRE_RESIST			0
#define	AQUA_RESIST			1
#define	TERRA_RESIST		2
#define	WIND_RESIST			3

// 스킬
#define	MELEE_SKILL			0
#define	BOW_SKILL			1
#define	FIREARM_SKILL		2
#define	THROW_SKILL			3
#define	LAUNCHER_SKILL		4

// 포스
#define	HOLY_DARK_FORCE		0
#define	FIRE_FORCE			1
#define	AQUA_FORCE			2
#define	TERRA_FORCE			3
#define	WIND_FORCE			4


/*//////////////////////////////////////////////////////////////////////////

[ CGUICharInfoWindow ]
 
   - 캐릭터의 정보를 보여준다.

//////////////////////////////////////////////////////////////////////////*/
class CGUICharInfoWindow : public CGUIWindow,
						   public IScrollObserver
{
public:
	enum CharInfo {
		Name,		
		Race,
		Attr,
		Sex,
		Level,
		Guild,
		Hp,
		Fp,
		Sp,
		Exp,
		PvP,
		GenAttPower,
		FcAttPower,
		Deffense,		
		CharInfoNum		
	};
// < Data Member > ---------------------------------------------------------
public:

	CGUIText			m_uiTitle;
	CGUIObjectAni		m_uiAniTitleBar;
	CGUIButton			m_uiCloseButton;
	CGUIButton			m_uiHelpButton;

	CGUIObject			m_uiLOutline;
	CGUIObject			m_uiROutline;

	CGUIContainerEx		m_uiBoard;
	
	CGUIContainer		m_uiTabBase;
	CGUIScrollbar		m_uiScrollbar;	

	CGUIObject			m_uiTab[4];
	CGUIText			m_uiTabName[4];
	CGUIContainer		m_uiTabBoard[4];
	CGUIContainer		m_uiInfoBoard[4];
	
	CGUIObject			m_uiResizeUnderBar;	

	//---------------------------------------------------------------------------------
	// char info data
	CGUIText			m_uiCharInfoTitle[CharInfoNum];
	CGUIText			m_uiCharInfo[CharInfoNum];

	CGUIText			m_uiUpgradeGenAttFc[2];		// 업그레이드 일반 공격력
	CGUIText			m_uiUpgradeForceAttFc[2];	// 업그레이드 포스 공격력
	
	// battle	
	CGUIText			m_uiBattleTitle;
	CGUIText			m_uiBattleName[DEFFENCE_MASTERY+1];
	CGUIText			m_uiBattlePoint[DEFFENCE_MASTERY+1];
	BYTE				m_byBattlePoint[DEFFENCE_MASTERY+1];
	CGUITileObject		m_uiBattleGauge[DEFFENCE_MASTERY+1];	

	// make
	CGUIText			m_uiMakeTitle;
	CGUIText			m_uiMakeName[BULLET_MAKE+1];
	CGUIText			m_uiMakePoint[BULLET_MAKE+1];
	CGUITileObject		m_uiMakeGauge[BULLET_MAKE+1];

	// resist
	CGUIObject			m_uiResistIcon;
	CGUIText			m_uiResistPoint[WIND_RESIST+1];


	//---------------------------------------------------------------------------------
	CGUIGaugeExpWindow	*	m_pRefGaugeExpWindow;

	BYTE				m_byRaceType;			// 종족 type

	BOOL				m_bCreated;

// < Member Function > -----------------------------------------------------
public:
	CGUICharInfoWindow();
	virtual ~CGUICharInfoWindow();
	
	void	Init( void );
	void	SetSprite( void );

	void	SetBound( RECT & pi_rtBound );	
	
	void	MouseEvent( _mouse_event &event );
	void	ScrollEvent( _scroll_event & event );

	//---------------------------------------------------------------------------------
	// HP, FP, ST, Exp, Level 정보를 가져오기 위해 gaugeexpwindow를 참조한다.
	void	SetGaugeExpWindow( CGUIGaugeExpWindow * l_pRefWindow );

	
	WORD	GetLevel( void ) { return m_pRefGaugeExpWindow->GetLevel(); }
	BYTE	GetRaceType( void ) { return m_byRaceType; }

	void	SetTab( BYTE pi_byTabIndex, BOOL pi_bOpen );
	BOOL	IsOpenedTab( BYTE pi_byTabIndex );


	//---------------------------------------------------------------------------------
	void	SetCharName( char * pi_pNameStr )	{ m_uiCharInfo[Name].SetString( pi_pNameStr ); }		
	void	SetAttribute( char * pi_pAttrStr )	{ m_uiCharInfo[Attr].SetString( pi_pAttrStr ); }
	void	SetRaceType( BYTE pi_byRaceType );
	void	SetRace( char * pi_pRaceStr )		{ m_uiCharInfo[Race].SetString( pi_pRaceStr ); }
	void	SetSex( char * pi_pSexStr )			{ m_uiCharInfo[Sex].SetString( pi_pSexStr ); }
	void	SetLevel( WORD pi_wLevel );
	void	SetGuildName( char * pi_pGuildStr )	{ m_uiCharInfo[Guild].SetString( pi_pGuildStr ); }

	void	SetHP( DWORD pi_dwCurHP, DWORD pi_dwMaxHP ); 
	void	SetFP( DWORD pi_dwCurFP, DWORD pi_dwMaxFP );
	void	SetSP( DWORD pi_dwCurSP, DWORD pi_dwMaxSP );

	void	SetExp( float pi_pRate );

	void	SetPvPPoint( DWORD pi_dwPvPPoint );

	////////////////////////////////////////////////////////////////////////////////////
	void	SetGenAttackFc( WORD pi_wMinGenAttackFc, WORD pi_wMaxGenAttackFc );	
	void	SetForceAttackFc( WORD pi_wMinForceAttackFc, WORD pi_wMaxForceAttackFc );

	void	SetDeffenceFc( WORD pi_wDefFc );
	void	SetHitRate( WORD pi_wHitRate ){;}		// not available
	void	SetAvoidRate( WORD pi_wAvoidRate ){;}	// not available
	
	
	void	SetGenAttackFc2( WORD pi_wMinGenAttackFc[2], WORD pi_wMaxGenAttackFc[2] );	
	void	SetForceAttackFc2( WORD pi_wMinForceAttackFc[2], WORD pi_wMaxForceAttackFc[2] );	

	void	SetDeffenceFc2( WORD pi_wDefFc[2] );
	////////////////////////////////////////////////////////////////////////////////////
	
	
	// --------------------------------------------------------------------------------	
	// 전투 기술 정보	
	void	SetBattleMastery( BYTE pi_byMasteryIndex, WORD pi_wMastery );
	void	SetBattleMasteryExp( BYTE pi_byMasteryIndex, float pi_fExp );	// pi_fExp : 0.0 ~ 1.0
	BYTE	GetBattleMastery( BYTE pi_byMasteryIndex ) { return m_byBattlePoint[pi_byMasteryIndex]; }

	// 제작 기술
	void	SetMakeMastery( BYTE pi_byMakeIndex, WORD pi_wMastery ); 
	void	SetMakeMasteryExp( BYTE pi_byMakeIndex, float pi_fExp );		// pi_fExp : 0.0 ~ 1.0

	// 저항력
	void	SetResistPoint( BYTE pi_byResistIndex, WORD pi_wResistPoint );		
};

#endif // ___GUICHARINFOWINDOW_H__
