////////////////////////////////////////////////////////////////////////////////
//
// CAvatar Relation Infomation Header 2
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __AVATAR_INFO2_H__
#define __AVATAR_INFO2_H__

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Structure          //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 채팅 정보
#define MAX_SEND_MESSAGE	10

struct CHAT_INFO
{
	char	m_pMessageType[32];							// 일반, 귓속말, 파티 등의 메세지 타입 ( 문자열 형태 )

	char	m_listSendMessage[MAX_SEND_MESSAGE][256];	// 아바타가 보낸 메세지를 저장
	BYTE	m_byCurrentSendMessageNo;
	BYTE	m_byMaxSendMessageNum;

	char	m_listWhisper[MAX_SEND_MESSAGE][64];		// 아바타가 보낸 귓속말 상대를 저장
	BYTE	m_byCurrentWhisperNo;
	BYTE	m_byMaxSendWhisperNum;

	char	m_pLatestSendWhisperName[64];				// 아바타가 귓속말을 보낸 가장 최근 상대
	char	m_pLatestReceiveWhisperName[64];			// 아바타가 귓속말을 받은 가장 최근 상대
	DWORD	m_pLatestReceiveWhisperIndex;				// 아바타가 귓속말을 받은 가장 최근 상대

	//--------------------------------------------------------------------------

	CHAT_INFO() { Init(); }
	void	Init( void );

	BOOL	IsEmptyString( char * pi_pMessage );
	BOOL	SendMessage( char * pi_pMessage );

	void	SaveSendMessage( char * pi_pMessage );
	void	DisplaySavedSendMessage( void );

	void	SaveWhisperList( char * pi_pName );
	void	DeleteWhisperList( char * pi_pName );
	void	DisplaySavedWhisperList( void );

	void	SetLatestReceiveWhisperPlayer( CPlayer * pi_pPlayer );
	void	SetLatestReceiveWhisperPlayer( DWORD pi_dwPlayerIndex );
	void	SetLatestReceiveWhisperPlayer( char * pi_pPlayerName );
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Structure          //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 잔상 정보
#define MAX_AFTER_IMAGE_NUM		( 5 )

struct AFTER_IMAGE_INFO
{
	enum AFTER_IMAGE_TYPE { AIT_NONE, AIT_MOVE, AIT_ATTACK };

	CPlayer *	m_pImage;
	BOOL		m_bIsRenderAfterImage;
	BYTE		m_byAfterImageType;

	DWORD		m_dwImageSavedTime;

	//--------------------------------------------------------------------------

	AFTER_IMAGE_INFO();
	~AFTER_IMAGE_INFO();

	void	EnableRender( CPlayer * pi_pOrgPlayer );
	void	DisableRender( void );

	void	Render( void );
	void	Animation( CPlayer * pi_pOrgChar );

private :

	void	CopyImage( CPlayer * po_pTarImage, CPlayer * pi_pOrgImage );
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Structure          //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Observer Mode ID ( OMI_ )
#define MAX_OBSERVER_MODE		0x3

#define OMI_NORMAL				0x0			// 일반 아바타와 같이 동작 [ 마우스 조작 ]
#define OMI_FLY					0x1			// 플라이 카메라 [ 키보드 (이동) + 마우스 (회전) ]
#define OMI_ANIMATION			0x2			// 애니메이션 카메라 [ 자동 ]

struct OBSERVER_INFO
{
	BOOL	m_bIsObserver;					// 옵져버인가?
	BYTE	m_byObserverMode;				// 옵져버 모드
	BYTE	m_byKeyDownCode;				// 옵져버 이동시 눌려진 키 타입

	OBSERVER_INFO();
	~OBSERVER_INFO();
	void	Init( void );

	void	SetObserver( BOOL pi_bFlag );
	void	SetObserverMode( BYTE pi_byMode );
};

#endif // __AVATAR_INFO2_H__