////////////////////////////////////////////////////////////////////////////
//
// CGUIRadarWindow Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUIRADARWINDOW_H__
#define __GUIRADARWINDOW_H__

#include "../../GUIController/GUIWindow.h"
#include "../../GUIController/GUIButton.h"
#include "../../GUIController/GUIText.h"

#define	UI_PARTY_LEADER_COLOR	D3DCOLOR_XRGB(  79, 255, 255 )
#define	UI_PARTY_MEMBER_COLOR	D3DCOLOR_XRGB( 125, 170, 215 ) //D3DCOLOR_XRGB( 77, 125, 125 )

struct RADAR_CHAR_INFO
{	
	DWORD			dwCharID;
	BYTE			byType;		
	Vector3f		vecCharPos;
	CGUIObject		uiCharObj;
	CGUIObjectEx	uiCharDir;		// 캐릭터 방향 표시
	BOOL			bDrawCharDir;	// 레이다밖에 있을 때 방향표시를 해줄 것인가
};

typedef list< RADAR_CHAR_INFO * >		CGUIRadarCharList;

/*//////////////////////////////////////////////////////////////////////////

[ CGUIRadarWindow ]   

//////////////////////////////////////////////////////////////////////////*/
class CGUIRadarWindow : public CGUIWindow
{
public:	
	enum CharType
	{	
		Same_Race,
		PartyLeader,
		PartyMember,
		Guild,
		Nation,
		
		Other_Bel,
		Other_Cor,
		Other_Acc,

		NPC_Bel,
		NPC_Cor,
		NPC_Acc,

		Mob,
		Avatar_Recall_Mob,
		Other_Recall_Mob,
		
		Stealth,

		TOTAL_CHAR_TYPE_NUM
	};


// < Data Member > ---------------------------------------------------------
private:	
	CGUIContainer		m_uiRadarBoard;		// object를 올려놓는 곳
	CGUIObjectEx		m_uiCompassBase;	// 나침반	

	// ----------------------------------------------------	

	float				m_fMapRotate;
	float				m_fPixelPerMeter[2];

	float				m_fCameraRotate;	
	
	float				m_fRotateSinTheta;
	float				m_fRotateCosTheta;

	CGUIObjectEx		m_uiAvatarObject;	// 레이다 보이는 아바타 객체	
	Vector3f			m_vecAvatarPos;		// 맵상에 아바타 위치	

	CGUIRadarCharList	m_listChar;			// 아바타 주위에 캐릭터들
	POINT				m_ptCharObjSize;

	float				m_fYAxisViewRange;	// Y축으로 레이더에 보이는 범위	
	

	BOOL				m_bCreated;

// < Member Function > ----------------------------------------------------
public:
	CGUIRadarWindow();
	virtual ~CGUIRadarWindow();

	void	Init( void );
	void	SetSprite( void );

	//--------------------------------------------------------------
	// 맵 크기 설정
	void	SetMapSize( POINT pi_ptSize );				// 레이다에 보이는 영역
	void	SetMapRotate( float pi_fAngle );

	void	SetYAxisViewRange( float pi_fViewRange );	// Y축으로 레이더에 보이는 범위 설정

	void	SetCameraRotate( float pi_fAngle );	
	void	SetAvatarRotate( float pi_fAngle );	

	// 아바타 설정	
	void	SetAvatarPosition( float pi_fPosX, float pi_fPosY, float pi_fPosZ );	


	// 아바타 주위에 캐릭터들 추가	
	BOOL	IsExistChar( DWORD pi_dwID );		// by J.S.H
	void	AddCharacter( BYTE			pi_byCharType,
						  DWORD			pi_dwID,
						  CharType		pi_eCharType,						  
						  float			pi_fPosX,
						  float			pi_fPosY,
						  float			pi_fPosZ,
						  BOOL			pi_bDrawCharDir = FALSE );
	
	void	RemoveCharacter( BYTE pi_byCharType, DWORD pi_dwID );
	void	RemoveAllCharacter( void );

	// 캐릭터 위치 update
	void	UpdateCharacterPos( BYTE pi_byCharType, DWORD pi_dwID, float pi_fPosX, float pi_fPosY, float pi_fPosZ );
	// 캐릭터 type update
	void	UpdateCharacterType( BYTE pi_byCharType, DWORD pi_dwID, CharType pi_eNewType );	
	// 캐릭터 방향 표시 update
	void	UpdateCharacterDirDrawFlag( BYTE pi_byCharType, DWORD pi_dwID, BOOL pi_bDrawDir );


private:
	RADAR_CHAR_INFO	* GetCharInfo( DWORD pi_dwID );
	
	void	SetSprite_CharObj( CharType pi_eCharType, CGUIObject * po_pCharObj );	
	void	SetSprite_CharDir( CharType pi_eCharType, CGUIObjectEx * po_pCharDir );

	void	UpdateCharPosition( RADAR_CHAR_INFO * pi_pCharInfo );

	void	SetViewedCharInRadarBoard( RADAR_CHAR_INFO * pi_pCharInfo );
};

#endif // __GUIRADARWINDOW_H__
