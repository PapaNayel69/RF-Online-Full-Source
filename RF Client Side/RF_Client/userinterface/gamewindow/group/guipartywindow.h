////////////////////////////////////////////////////////////////////////////
//
// CGUIPartyWindow Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef ___GUIPARTYWINDOW_H__
#define ___GUIPARTYWINDOW_H__

#include "../../GUIController/GUIWindow.h"
#include "../../GUIController/GUIButton.h"
#include "../../GUIController/GUIText.h"
#include "../../GUIController/GUIItem.h"
#include "../GUIGaugeExpWindow.h"

#define UI_PMT_NONE						0
#define UI_PMT_MEMBER					1
#define UI_PMT_LEADER					2

#define	MAX_PARTY_MEMBER_NUM			7

#define	MSGBOX_ID_PARTY_INVITE			1	// 파티 생성(초대) 메시지 창
#define	MSGBOX_ID_PARTY_ACCEPT_INVITE	2	// 파티 생성(초대) 메시지 => 응답창
#define	MSGBOX_ID_PARTY_JOIN			3	// 파티 가입 메시지 창
#define	MSGBOX_ID_PARTY_ACCEPT_JOIN		4	// 파티 가입 메시지 => 응답창

/*//////////////////////////////////////////////////////////////////////////

[ CGUIPartyMemberInfo ]
 
   - 파티원 정보

//////////////////////////////////////////////////////////////////////////*/

class CGUIPartyMemberInfo : public CGUIContainer,
							public IMouseObserver
{
// < Data Member > ---------------------------------------------------------
private:
	DWORD			m_dwMemberID;

	CGUIText		m_uiCharName;

	CGUIText		m_uiLevel;

	CGUIObjectEx	m_uiHPGauge;
	CGUIObjectEx	m_uiFPGauge;
	CGUIObjectEx	m_uiSPGauge;


	CGUIButton		m_uiRemoveButton;
	CGUIButton		m_uiChangeReaderButton;

	CGUIItem		m_uiDamageSF[5];
	CGUIItem		m_uiHelpSF[5];

	int				m_nGaugeSizeX;	
	float			m_fGaugeSizeU;	

	CGUIObject		m_uiEventCover;

	BOOL			m_bCreated;

// < Member Function > -----------------------------------------------------
public:
	CGUIPartyMemberInfo();
	~CGUIPartyMemberInfo();

	void		Init( int pi_nSizeX, int pi_nSizeY );
	void		SetSprite( void );

	void		ClearMemberInfo( void );
	
	
	void		SetMemberID( DWORD pi_dwMemberID ) { m_dwMemberID = pi_dwMemberID; }
	DWORD		GetMemberID( void ) { return m_dwMemberID; }	

	void		SetCharName( char * pi_pName );
	CGUIText*	GetCharNameInfo( void ) { return &m_uiCharName; }
	void		SetLevel( int pi_nLevel );

	// HP, FP, SP setting
	void		SetHPGauge( float pi_fHPRate );	// range : 0.f ~ 1.f
	void		SetFPGauge( float pi_fFPRate );	// range : 0.f ~ 1.f
	void		SetSPGauge( float pi_fSPRate );	// range : 0.f ~ 1.f

	// skill, force
	void		AddDamageSF( BYTE pi_bySFType, DWORD pi_dwItemID, DWORD pi_dwSpriteID );
	void		AddHelpSF( BYTE pi_bySFType, DWORD pi_dwItemID, DWORD pi_dwSpriteID );
	void		RemoveDamageSF( BYTE pi_bySFType, DWORD pi_dwItemID );
	void		RemoveHelpSF( BYTE pi_bySFType, DWORD pi_dwItemID );
	void		RemoveAllSF( void );

	void		EnableRemveButton( BOOL pi_bEnable );
	void		EnableChLeaderButton( BOOL pi_bEnable );

	void		MouseEvent( _mouse_event & event );
};


typedef struct _group_member_info
{
	DWORD	dwMemberID;

	char	pCharName[20];
	WORD	wLevel;
	float	fHPRate;		// 0.f ~ 1.f
	float	fFPRate;		// 0.f ~ 1.f
	float	fSPRate;		// 0.f ~ 1.f

	struct _sf_Info {
		BYTE	bySFType;	// SKILL_ICON or FORCE_ICON
		DWORD	dwItemID;
		DWORD	dwSpriteID;
		_sf_Info()
		{
			dwItemID	=	NONE_ITEM_ID;
			dwSpriteID	=	0;
			bySFType	=	0;
		}

	} sfDamage[5], sfHelp[5];	

	_group_member_info()
	{
		dwMemberID	=	0xffffffff;
		wLevel		=	0;
		fHPRate		=	0.f;
		fFPRate		=	0.f;
		fSPRate		=	0.f;
	}
	
} GROUP_MEMBER_INFO;


/*//////////////////////////////////////////////////////////////////////////

[ CGUIPartyWindow ]
 
   - 파티창

//////////////////////////////////////////////////////////////////////////*/

class CGUIPartyWindow : public CGUIWindow
{
// < Data Member > ---------------------------------------------------------
private:
	
	CGUIObject		m_uiTitleBoard;
	CGUIText		m_uiTitle;


	CGUIPartyMemberInfo		m_uiMember[MAX_PARTY_MEMBER_NUM];	
	BYTE					m_byMemberOrder[MAX_PARTY_MEMBER_NUM];

	BYTE			m_byTotalMember;	

	CGUIButtonEx	m_uiLockNLeaveButton;	
	
	BYTE			m_byMemberType;

	BOOL			m_bIsLocked;

	BOOL			m_bCreated;
	

// < Member Function > -----------------------------------------------------
public:
	CGUIPartyWindow();
	virtual ~CGUIPartyWindow();

	void		Init( void );
	void		SetSprite( void );
	
	// ------------------------------------------------------------------------
	// 파티리더인지 파티원인지 설정한다.
	void		SetWindowType( BYTE pi_byMemberType );
	BYTE		GetWindowType( void )	{ return m_byMemberType; }


	// 파티원 추가 삭제
	BOOL		AddMember( GROUP_MEMBER_INFO * pi_pMemberInfo );
	void		RemoveMember( DWORD pi_dwMemberID );
	// 파티원 모두 삭제
	void		RemoveAllMember( void );


	// 파티리더 설정
	void		SetLeader( GROUP_MEMBER_INFO * pi_pMemberInfo );
	void		SetLeader( DWORD pi_dwMemberID );		// 기존 멤버를 파티리더로 설정할 때 사용

	// 파티리더 -> 파티원
	void		SetMember( DWORD pi_dwMemberID );
		

	// ------------------------------------------------------------------------
	// 파티원 정보 update
	void		SetLevel( DWORD pi_dwMemberID, int pi_nLevel );

	void		SetHPGauge( DWORD pi_dwMemberID, float pi_fHPRate );	// 0.f ~ 1.f
	void		SetFPGauge( DWORD pi_dwMemberID, float pi_fFPRate );	// 0.f ~ 1.f
	void		SetSPGauge( DWORD pi_dwMemberID, float pi_fSPRate );	// 0.f ~ 1.f

	void		AddSkillForce( DWORD	pi_dwMemberID,
							   BYTE		pi_bySFType,	// Skill or force
							   BYTE		pi_byContType,	// 피해 or 보조
							   DWORD	pi_dwItemID,
							   DWORD	pi_dwSpriteID );

	void		RemoveSkillForce( DWORD	pi_dwMemberID,
								  BYTE	pi_bySFType,
								  BYTE	pi_byContType,
								  DWORD	pi_dwItemID );
	void		RemoveAllSkillForce( DWORD	pi_dwMemberID );

	// ------------------------------------------------------------------------	
	void		Lock( BOOL pi_bLock );
	

	void		EnableLockNLeaveButton( BOOL pi_bEnable );


	void		MouseEvent( _mouse_event & event );
	BOOL		Draw( void );

	void		ClosingDialogBox( CGUIDialogBox * pi_pDlg );


private:
	void		SetTotalMemberNum( int pi_nNum );

};

#endif // ___GUIPARTYWINDOW_H__
